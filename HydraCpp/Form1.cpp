#include "pch.h"
#include "Form1.h"

using namespace cv;
using namespace std;

string path = "";
string parameterDir = "/parameters/";
string outputDir = "/output/";
string lightDir = "/lights/";
string darkDir = "/darks/";
string flatDir = "/flats/";
string flatDarksDir = "/flatDarks/";
string darksGroup = "RGB";
string flatDarksGroup = "LRGB";
string ext = ".png";
string frameFilter = "R";
string alignFilter = "R";

int detectionThreshold = 50;
int discardPercentage = 10;
int medianBatchSize = 30;
int interpolationFlag = 2;
int maxStars = 15;
int topMatches = 6;
int scaling = 4;
int numLogicalCores = omp_get_max_threads();
float samplingFactor = 1;

vector<vector<string>> readStringMatrix(const string& path) {
    ifstream file(path);
    vector<vector<string>> result;
    string line;

    while (getline(file, line)) {
        vector<string> row;
        stringstream ss(line);
        string cell;

        while (getline(ss, cell, ',')) 
            row.push_back(cell);

        result.push_back(row);
    }

    return result;
}

void writeStringMatrix(const string& path, const vector<vector<string>>& stringArray) {
    ofstream stringFileStream(path);
    for (const auto& row : stringArray) {
        for (size_t j = 0; j < row.size() - 1; j++)
            stringFileStream << row[j] << ",";

        stringFileStream << row[row.size() - 1] << "\n";
    }
    stringFileStream.close();
}

void unpack(const vector<vector<string>>& inputArray, vector<string>* lightFrameArray, vector<vector<float>>* qualVec, vector<vector<float>>* xvec, vector<vector<float>>* yvec) {
    for (size_t i = 0; i < inputArray.size(); i++) {
        (*lightFrameArray)[i] = inputArray[i][0];
        (*qualVec)[i] = { stof(inputArray[i][1]), stof(inputArray[i][2]), stof(inputArray[i][3]), stof(inputArray[i][4]), stof(inputArray[i][5]) };

        for (size_t j = 0; j < maxStars; j++) {
            (*xvec)[i][j] = stof(inputArray[i][j + 6]);
            (*yvec)[i][j] = stof(inputArray[i][j + 6 + maxStars]);
        }
    }
}

vector<string> getFrames(const string& path, const string& ext) {
    vector<string> filenames;
    if (filesystem::exists(path))
        for (auto& p : filesystem::recursive_directory_iterator(path))
            if (p.path().extension() == ext)
                filenames.push_back(p.path().string());

    return filenames;
}

vector<float> clean(vector<float>& v) {
    v.erase(std::remove(v.begin(), v.end(), 0), v.end());
    return v;
}

template <typename T> void sortByColumn(vector<vector<T>>& data, size_t column) {
    sort(data.begin(), data.end(), [column](const vector<T>& v1, const vector<T>& v2) { return v1[column] > v2[column]; });
}

string filterSelector(string input) {
    if (input == "LRGB") return "LRGB";
    else if (input == "RGB" && frameFilter == "L") return "L";
    else if (input == "RGB" && frameFilter != "L") return "RGB";
    else return frameFilter;
}

//Function for enumerating star triangles
vector<vector<float>> triangles(const vector<float>& x, const vector<float>& y) {
    vector<vector<float>> triangleParameters;
    const float minSquare = 2500; //50 * 50;
    const int n = x.size();
    float s0, s1, s2;
    for (int i = 0; i < n - 2; i++)
        for (int j = i + 1; j < n - 1; j++) {
            if (float s3 = ((x[i] - x[j]) * (x[i] - x[j]) + (y[i] - y[j]) * (y[i] - y[j])) > minSquare) {
                for (int k = j + 1; k < n; k++) {
                    s1 = ((x[j] - x[k]) * (x[j] - x[k]) + (y[j] - y[k]) * (y[j] - y[k]));
                    s2 = ((x[i] - x[k]) * (x[i] - x[k]) + (y[i] - y[k]) * (y[i] - y[k]));
                    if (s1 > minSquare && s2 > minSquare) {
                        s0 = s3;
                        if (s1 > s2) swap(s1, s2);
                        if (s0 > s2) swap(s0, s2);
                        if (s0 > s1) swap(s0, s1);
                        triangleParameters.push_back({ float(i), float(j), float(k), sqrt(s1 / s2), sqrt(s0 / s2) });
                    }
                }
            }
        }

    return triangleParameters;
}

//Function for computing the "vote matrix"
vector<vector<int>> getStarPairs(const vector<vector<float>>& refTriangles, const vector<vector<float>>& frameTriangles, const int& refVectorSize, const int& vecSize) {
    constexpr float eSquare = 0.002 * 0.002; 
    vector<vector<int>> starPairs;
    vector<vector<int>> voteMatrix(refVectorSize, vector<int>(vecSize, 0));
    for (const auto& refTri : refTriangles)
        for (int b = 0; b < frameTriangles.size(); b++)
            if ((refTri[3] - frameTriangles[b][3]) * (refTri[3] - frameTriangles[b][3]) + (refTri[4] - frameTriangles[b][4]) * (refTri[4] - frameTriangles[b][4]) < eSquare)
                for (int i = 0; i < 3; i++)
                    voteMatrix[static_cast<int>(refTri[i])][static_cast<int>(frameTriangles[b][i])] += 1;

    for (int row = 0; row < voteMatrix.size(); row++) {
        int maxRowVote = 0;
        int maxRowVoteIndex = 0;
        int nextLargestColElement = 0;
        int nextLargestRowElement = 0;

        for (int c = 0; c < voteMatrix[0].size(); c++)
            if (maxRowVote < voteMatrix[row][c]) {
                nextLargestRowElement = maxRowVote;
                maxRowVote = voteMatrix[row][c];
                maxRowVoteIndex = c;
            }
            else if (nextLargestRowElement < voteMatrix[row][c])
                nextLargestRowElement = voteMatrix[row][c];                

        for (int r = 0; r < voteMatrix.size(); r++) 
            if (r != row)
                nextLargestColElement = max(nextLargestColElement, voteMatrix[r][maxRowVoteIndex]);

        int correctedVotes = max((maxRowVote - max(nextLargestColElement, nextLargestRowElement)), 0);

        if (correctedVotes > 0) starPairs.push_back({ row, maxRowVoteIndex, correctedVotes });
    }
    sortByColumn(starPairs, 2);
    return starPairs;
}

//Function for computing angular and translational offsets between vectors
vector<float> alignFrames(const vector<vector<int>>& starPairs, const vector<float>& refVectorX, const vector<float>& refVectorY, const vector<float>& xvec, const vector<float>& yvec, const int& topMatches) {
    Mat frameMatrix(2, topMatches, CV_32F), referenceMatrix(2, topMatches, CV_32F), centroid_F, centroid_R, U, S, Vt, R, t;

    if (topMatches <= starPairs.size()) 
        for (int i = 0; i < topMatches; i++) {
            referenceMatrix.at<float>(0, i) = refVectorX[starPairs[i][0]];
            referenceMatrix.at<float>(1, i) = refVectorY[starPairs[i][0]];
            frameMatrix.at<float>(0, i) = xvec[starPairs[i][1]];
            frameMatrix.at<float>(1, i) = yvec[starPairs[i][1]];
        }

    reduce(frameMatrix, centroid_F, 1, REDUCE_AVG);
    reduce(referenceMatrix, centroid_R, 1, REDUCE_AVG);
    subtract(frameMatrix, repeat(centroid_F, 1, frameMatrix.cols), frameMatrix);
    subtract(referenceMatrix, repeat(centroid_R, 1, referenceMatrix.cols), referenceMatrix);

    SVD::compute(frameMatrix * referenceMatrix.t(), S, U, Vt);
    Vt.row(1) *= (determinant(U * Vt) < 0) ? -1 : 1;
    R = (U * Vt).t();

    t = -R * centroid_F + centroid_R;
    return { R.at<float>(0, 0), R.at<float>(1, 0), t.at<float>(0, 0), t.at<float>(1, 0) };
}

//Function to analyze the star field in the given light frame.
vector<vector<float>> analyzeStarField(Mat lightFrame, const float& t) {
    vector<vector<float>> starMatrix;

    if ((lightFrame.elemSize() == 1 || lightFrame.elemSize() == 2) && lightFrame.channels() == 1) {
        if (lightFrame.elemSize() == 2) 
            lightFrame.convertTo(lightFrame, CV_8U, 1.0 / 255);

        Mat filteredImage, thresh;
        vector<vector<Point>> contours;
        medianBlur(lightFrame, filteredImage, 3);
        threshold(filteredImage, thresh, t * 255, 255, 0);
        findContours(thresh, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

        for (const auto& c : contours) {
            RotatedRect rect = minAreaRect(c);

            if (float d = std::sqrt(rect.size.width * rect.size.width + rect.size.height * rect.size.height); d < 25)
                starMatrix.push_back({ rect.center.x, rect.center.y, d });
        }
    }
    return starMatrix;
}

//Function to fetch a calibration frame
Mat getCalibrationFrame(const int& height, const int& width, const string& calibrationPath, const float& defaultValue) {
    Mat masterFrame(height, width, CV_32FC1, Scalar(defaultValue));
    string masterFramePath = calibrationPath + "/" + "masterFrame.tif";
    
    if (filesystem::exists(masterFramePath)) {
        Mat tmpCalibrationFrame = imread(masterFramePath, IMREAD_ANYDEPTH);
        if (tmpCalibrationFrame.cols == width && tmpCalibrationFrame.rows == height)
            masterFrame = tmpCalibrationFrame;
    }
    else {
        vector<string> calibrationFrameArray = getFrames(calibrationPath + "/", ext);
        if (!calibrationFrameArray.empty()) {
            Mat tmpMasterFrame(height, width, CV_32FC1, Scalar(0));
            #pragma omp parallel for num_threads(numLogicalCores*2)
            for (int n = 0; n < calibrationFrameArray.size(); n++) {
                Mat calibrationFrame = imread(calibrationFrameArray[n], IMREAD_ANYDEPTH);
                if (calibrationFrame.cols == width && calibrationFrame.rows == height) {
                    normalize(calibrationFrame, calibrationFrame, 0, 1, cv::NORM_MINMAX, CV_32F);
                    addWeighted(tmpMasterFrame, 1, calibrationFrame, 1 / float(calibrationFrameArray.size()), 0.0, tmpMasterFrame);
                }
            }
            imwrite(masterFramePath, tmpMasterFrame);
            masterFrame = tmpMasterFrame;
        }
    }
    return masterFrame;
}

//Find hot pixels
void findHotPixels(const Mat& masterDarkFrame, const int& ySize, const int& xSize, vector<vector<int>>& hotPixels) {
    float meanValue = mean(masterDarkFrame)[0];

    for (int y = 1; y < ySize - 1; y++)
        for (int x = 1; x < xSize - 1; x++)
            if (masterDarkFrame.at<float>(y, x) > 10 * meanValue)
                hotPixels.push_back({ x,y });
}

//Function to remove hotpixels
void removeHotPixels(Mat& lightFrame, const vector<vector<int>>& hotPixels) {
    for (const auto& hotPix : hotPixels) lightFrame.at<float>(hotPix[1], hotPix[0]) = (lightFrame.at<float>(hotPix[1], hotPix[0] + 1) + lightFrame.at<float>(hotPix[1], hotPix[0] - 1) + lightFrame.at<float>(hotPix[1] + 1, hotPix[0]) + lightFrame.at<float>(hotPix[1] - 1, hotPix[0])) / 4;
}

//Function to rotate images
Mat processFrame(const string& framePath, const Mat& masterDarkFrame, const Mat& invertedFlatFrame, const Size& s, const float& backGroundCorrection, const vector<float>& RTparams, const vector<vector<int>>& hotPixels) {
    Mat lightFrame = imread(framePath, IMREAD_GRAYSCALE);
    normalize(lightFrame, lightFrame, 0, 1, cv::NORM_MINMAX, CV_32F);
    lightFrame = backGroundCorrection * (lightFrame - masterDarkFrame).mul(invertedFlatFrame);
    removeHotPixels(lightFrame, hotPixels);
    resize(lightFrame, lightFrame, s, interpolationFlag);
    Mat M = (Mat_<float>(2, 3) << RTparams[0], -RTparams[1], RTparams[2], RTparams[1], RTparams[0], RTparams[3]);
    warpAffine(lightFrame, lightFrame, M, Size(), interpolationFlag);
    return lightFrame;
}

//Function to compute median image
Mat computeMedianImage(const vector<Mat>& imageStack, const int& rows, const int& cols) {
    Mat medianImage(rows, cols, CV_32FC1);
    int numImages = imageStack.size();
    int midIndex = numImages / 2;

    #pragma omp parallel num_threads(numLogicalCores*2) 
    {
        vector<float> pixelValues(numImages);
        #pragma omp for
        for (int i = 0; i < rows * cols; i++) {
            for (int imgIdx = 0; imgIdx < numImages; imgIdx++)
                pixelValues[imgIdx] = imageStack[imgIdx].at<float>(i);

            partial_sort(pixelValues.begin(), pixelValues.begin() + midIndex + 1, pixelValues.end());
            medianImage.at<float>(i) = (numImages % 2 == 0) ? (pixelValues[midIndex] + pixelValues[midIndex - 1]) / 2.0f : pixelValues[midIndex];
        }
    }

    return medianImage;
}

//Function to read images
vector<int> Hydra::Form1::RegisterFrames() {
    int elapsedTime = 0;

    vector<string> lightFrames = getFrames(path + lightDir + frameFilter, ext);
    int n = lightFrames.size();

    if (!lightFrames.empty()) {
        auto startTime = chrono::high_resolution_clock::now();
        vector<vector<float>> qualVec(lightFrames.size(), vector<float>(6 + 2 * maxStars, 0));
        vector<vector<string>> qualVecS(lightFrames.size(), vector<string>(6 + 2 * maxStars));

        #pragma omp parallel for num_threads(numLogicalCores*2)
        for (int k = 0; k < n; k++) {
            Mat lightFrame = imread(lightFrames[k], IMREAD_ANYCOLOR | IMREAD_ANYDEPTH);
            vector<vector<float>> starMatrix = analyzeStarField(lightFrame, float(detectionThreshold) / 100);

            qualVec[k][0] = k;
            qualVec[k][1] = starMatrix.size();
            qualVec[k][2] = sum(lightFrame)[0];
            qualVec[k][3] = lightFrame.cols;
            qualVec[k][4] = lightFrame.rows;
            qualVec[k][5] = lightFrame.elemSize();

            if (starMatrix.size() > 3) {
                sortByColumn(starMatrix, 2);
                for (int i = 0; i < min(maxStars, int(starMatrix.size())); i++) {
                    qualVec[k][i + 6] = starMatrix[i][0];
                    qualVec[k][i + 6 + maxStars] = starMatrix[i][1];
                }
            }
        }

        sortByColumn(qualVec, 1);

        for (int k = 0; k < qualVec.size(); k++) {
            qualVecS[k][0] = lightFrames[int(qualVec[k][0])];
            for (int l = 1; l < qualVec[0].size(); l++)
                qualVecS[k][l] = to_string(qualVec[k][l]);
        }

        elapsedTime = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - startTime).count();

        if (!filesystem::exists(path + parameterDir)) filesystem::create_directory(path + parameterDir);

        writeStringMatrix(path + parameterDir + "qualVec" + frameFilter + ".csv", qualVecS);
    }
    return { n, elapsedTime };
}

//Function for computing angular and translational offsets
vector<int> Hydra::Form1::ComputeOffsets() {
    int elapsedTime = 0;
    int n = 0;

    string qualVecPath = path + parameterDir + "qualVec" + frameFilter + ".csv";
    string qualVecAlignPath = path + parameterDir + "qualVec" + alignFilter + ".csv";

    if ((filesystem::exists(qualVecPath) && filesystem::exists(qualVecAlignPath))) {
        auto startTime = chrono::high_resolution_clock::now();

        vector<vector<string>> inputMatrix = readStringMatrix(qualVecPath);
        vector<vector<string>> inputMatrixAlign = readStringMatrix(qualVecAlignPath);
        vector<string> lightFrameArray(inputMatrix.size()), lightFrameArrayAlign(inputMatrixAlign.size());
        vector<vector<float>> qualVec(inputMatrix.size()), xvecFrame(inputMatrix.size(), vector<float>(maxStars)), yvecFrame(inputMatrix.size(), vector<float>(maxStars));
        vector<vector<float>> qualVecAlign(inputMatrixAlign.size()), xvecAlign(inputMatrixAlign.size(), vector<float>(maxStars)), yvecAlign(inputMatrixAlign.size(), vector<float>(maxStars));

        unpack(inputMatrix, &lightFrameArray, &qualVec, &xvecFrame, &yvecFrame);
        unpack(inputMatrixAlign, &lightFrameArrayAlign, &qualVecAlign, &xvecAlign, &yvecAlign);

        bool sizesEqual = true;

        for (int l = 0; l < qualVec.size() && sizesEqual; l++)
            if ((qualVec[l][2] != qualVec[0][2]) || (qualVec[l][3] != qualVec[0][3]))
                sizesEqual = false;

        if (sizesEqual) {
            vector xRef = clean(xvecAlign[0]);
            vector yRef = clean(yvecAlign[0]);

            if (!xRef.empty() && xRef.size() >= topMatches) {
                n = floor(qualVec.size() * (100 - float(discardPercentage)) / 100);
                vector<vector<string>> stackArray(n, vector<string>(8));
                vector<vector<float>> refTriangles = triangles(xRef, yRef);

                Mat maxQualFrame = imread(lightFrameArrayAlign[0], IMREAD_GRAYSCALE);
                resize(maxQualFrame, maxQualFrame, cv::Size(), 1.0 / scaling, 1.0 / scaling, INTER_CUBIC);
                cvtColor(maxQualFrame, maxQualFrame, COLOR_GRAY2BGR);
                static const map<string, Scalar> colorMap = { {"R", Scalar(0, 0, 255)}, {"G", Scalar(0, 255, 0)}, {"B", Scalar(255, 0, 0)}, {"L", Scalar(255, 255, 255)} };

                for (int j = 0; j < xRef.size(); j++) circle(maxQualFrame, Point_(xRef[j] / scaling, yRef[j] / scaling), 8, colorMap.at(alignFilter));

                #pragma omp parallel for num_threads(numLogicalCores*2)
                for (int k = 0; k < n; k++) {
                    vector xFrame = clean(xvecFrame[k]);
                    vector yFrame = clean(yvecFrame[k]);
                    if (!xFrame.empty() && xFrame.size() >= topMatches) {
                        vector<vector<float>> frameTriangles = triangles(xFrame, yFrame);
                        vector<vector<int>> starPairs = getStarPairs(refTriangles, frameTriangles, xRef.size(), xFrame.size());
                        vector<float> RTparams = alignFrames(starPairs, xRef, yRef, xFrame, yFrame, topMatches);
                        stackArray[k] = { lightFrameArray[k], to_string(qualVec[k][0]), to_string(qualVec[k][1]), to_string(qualVec[k][2]), to_string(qualVec[k][3]), to_string(RTparams[0]), to_string(RTparams[1]), to_string(RTparams[2]), to_string(RTparams[3]) };

                        for (int j = 0; j < xFrame.size(); j++) circle(maxQualFrame, Point_((RTparams[0] * xFrame[j] - RTparams[1] * yFrame[j] + RTparams[2]) / scaling, (RTparams[1] * xFrame[j] + RTparams[0] * yFrame[j] + RTparams[3]) / scaling), 5, colorMap.at(frameFilter));
                    }
                }
                elapsedTime = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - startTime).count();

                writeStringMatrix(path + parameterDir + "stackArray" + frameFilter + ".csv", stackArray);
                imshow("Debug", maxQualFrame);
            }
        }
    }
    return { n, elapsedTime };
    waitKey(0);
    destroyAllWindows();
}

//Function for stacking the images
vector<int> Hydra::Form1::Stack() {
    int elapsedTime = 0;
    int n = 0;

    string stackArrayPath = path + parameterDir + "stackArray" + frameFilter + ".csv";

    if (filesystem::exists(stackArrayPath)) {
        auto startTime = chrono::high_resolution_clock::now();

        vector<vector<string>> stackInfo = readStringMatrix(stackArrayPath);
        n = stackInfo.size();

        vector<string> stackArray(n);
        vector<vector<float>> RTparams(n);
        vector<float>background(n);
        float mean_background = 0;

        for (int i = 0; i < n; i++) {
            stackArray[i] = stackInfo[i][0];
            RTparams[i] = { stof(stackInfo[i][5]), stof(stackInfo[i][6]), samplingFactor * stof(stackInfo[i][7]), samplingFactor * stof(stackInfo[i][8]) };
            background[i] = stof(stackInfo[i][2]);
            mean_background = mean_background + background[i] / float(n);
        }

        int width = stoi(stackInfo[0][3]);
        int height = stoi(stackInfo[0][4]);

        Mat calibratedFlatFrame = getCalibrationFrame(height, width, path + flatDir + frameFilter, 1) - getCalibrationFrame(height, width, path + flatDarksDir + filterSelector(flatDarksGroup), 0);
        Mat masterDarkFrame = getCalibrationFrame(height, width, path + darkDir + filterSelector(darksGroup), 0);

        double minVal;
        minMaxLoc(calibratedFlatFrame, &minVal);

        if (minVal > 0) {
            vector<vector<int>> hotPixels;
            findHotPixels(masterDarkFrame, height, width, hotPixels);

            calibratedFlatFrame *= 1 / mean(calibratedFlatFrame)[0];
            Mat invertedCalibratedFlatFrame, ones(height, width, CV_32FC1, Scalar(1));
            divide(ones, calibratedFlatFrame, invertedCalibratedFlatFrame);

            if (n < medianBatchSize) medianBatchSize = n;

            int batches = n / medianBatchSize;
            int iterations = medianBatchSize * batches;

            vector<int> m(iterations);
            for (int j = 0; j < iterations; j++) m[j] = j;
            shuffle(m.begin(), m.end(), default_random_engine(chrono::system_clock::now().time_since_epoch().count()));

            cv::Size s = cv::Size(int(width * samplingFactor), int(height * samplingFactor));
            Mat p(s.height, s.width, CV_32FC1, Scalar(0)), psqr(s.height, s.width, CV_32FC1, Scalar(0)), std(s.height, s.width, CV_32FC1, Scalar(0)), medianFrame(s.height, s.width, CV_32FC1, Scalar(0)), stackFrame(s.height, s.width, CV_32FC1, Scalar(0));
            vector<Mat> medianArray(medianBatchSize, Mat(s.height, s.width, CV_32FC1));

            int test = 0;

            for (int k = 0; k < batches; k++) {
                #pragma omp parallel for num_threads(numLogicalCores*2)
                for (int c = 0; c < medianBatchSize; c++) {
                    int i = m[k * medianBatchSize + c];
                    medianArray[c] = processFrame(stackArray[i], masterDarkFrame, invertedCalibratedFlatFrame, s, mean_background / background[i], RTparams[i], hotPixels);
                    test = medianArray[c].rows;
                    
                    addWeighted(p, 1, medianArray[c] / iterations, 1, 0.0, p);
                    addWeighted(psqr, 1, medianArray[c].mul(medianArray[c]) / iterations, 1, 0.0, psqr);
                }
                addWeighted(medianFrame, 1, computeMedianImage(medianArray, s.height, s.width), 1 / float(batches), 0.0, medianFrame);
            }

            sqrt((psqr - p.mul(p)) * iterations / (iterations - 1), std);

            #pragma omp parallel for num_threads(numLogicalCores*2) 
            for (int k = 0; k < n; k++) {
                Mat absDiff, mask;
                Mat lightFrame = processFrame(stackArray[k], masterDarkFrame, invertedCalibratedFlatFrame, s, mean_background / background[k], RTparams[k], hotPixels);
                absdiff(lightFrame, medianFrame, absDiff);
                compare(absDiff, 2.0 * std, mask, CMP_GT);
                medianFrame.copyTo(lightFrame, mask);
                addWeighted(stackFrame, 1, lightFrame, 1 / float(n), 0.0, stackFrame);
            }

            if (!filesystem::exists(path + outputDir)) filesystem::create_directory(path + outputDir);

            imwrite(path + outputDir + "Median" + "_" + to_string(n) + "_" + frameFilter + "_" + to_string(int(samplingFactor * 100)) + ".tif", medianFrame);
            imwrite(path + outputDir + "Mean" + "_" + to_string(n) + "_" + frameFilter + "_" + to_string(int(samplingFactor * 100)) + ".tif", p);
            imwrite(path + outputDir + "Stack" + "_" + to_string(n) + "_" + frameFilter + "_" + to_string(int(samplingFactor * 100)) + ".tif", stackFrame);

            elapsedTime = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - startTime).count();
            resize(stackFrame, stackFrame, cv::Size(), 1.0 / (scaling * samplingFactor), 1.0 / (scaling * samplingFactor), INTER_CUBIC);
            imshow("Stack", stackFrame * 5);
        }
    }
    return { n, elapsedTime };
    waitKey(0);
    destroyAllWindows();
}