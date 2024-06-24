#include "pch.h"
#include "Form1.h"

std::string path = "";
std::string parameterDir = "/parameters/";
std::string outputDir = "/output/";
std::string lightDir = "/lights/";
std::string darkDir = "/darks/";
std::string flatDir = "/flats/";
std::string flatDarksDir = "/flatDarks/";
std::string darksGroup = "RGB";
std::string flatDarksGroup = "LRGB";
std::string ext = ".png";
int detectionThreshold = 50;
int discardPercentage = 10;
int medianBatchSize = 30;
int interpolationFlag = 2;
int maxStars = 15;
int topMatches = 6;
int scaling = 4;
int numLogicalCores = omp_get_max_threads();
float samplingFactor = 1;
std::string filter = "R";
std::string align = "R";

std::vector<std::vector<std::string>> readStringMatrix(const std::string& path) {
    std::vector<std::vector<std::string>> commaSeparatedArray;
    std::string line;
    std::ifstream commaSeparatedArrayStream(path);
    if (commaSeparatedArrayStream.is_open())
    {
        while (std::getline(commaSeparatedArrayStream, line)) {
            std::vector<std::string> commaSeparatedLine;
            std::string::size_type start = 0;
            std::string::size_type end;

            while ((end = line.find(',', start)) != std::string::npos) {
                commaSeparatedLine.push_back(line.substr(start, end - start));
                start = end + 1;
            }

            commaSeparatedLine.push_back(line.substr(start));
            commaSeparatedArray.push_back(commaSeparatedLine);
        }
    }
    return commaSeparatedArray;
}

void writeStringMatrix(const std::string& path, const std::vector<std::vector<std::string>>& stringArray) {
    std::ofstream stringFileStream(path);
    for (const auto& row : stringArray) {
        for (size_t j = 0; j < row.size(); j++) {
            stringFileStream << row[j];
            if (j < row.size() - 1) 
                stringFileStream << ",";
        }
        stringFileStream << "\n";
    }
}

std::tuple<std::vector<std::string>, std::vector<std::vector<float>>, std::vector<std::vector<float>>, std::vector<std::vector<float>>> unpack(const std::vector<std::vector<std::string>>& inputArray) {
    std::vector<std::string> lightFrameArray(inputArray.size());
    std::vector<std::vector<float>> qualVec(inputArray.size()), xvec(inputArray.size(), std::vector<float>(maxStars)), yvec(inputArray.size(), std::vector<float>(maxStars));

    for (int i = 0; i < inputArray.size(); i++)
    {
        lightFrameArray[i] = inputArray[i][0];
        qualVec[i] = { stof(inputArray[i][1]), stof(inputArray[i][2]), stof(inputArray[i][3]), stof(inputArray[i][4]), stof(inputArray[i][5]) };

        for (int j = 0; j < maxStars; j++)
        {
            xvec[i][j] = stof(inputArray[i][j + 6]);
            yvec[i][j] = stof(inputArray[i][j + 6 + maxStars]);
        }
    }

    return std::make_tuple(lightFrameArray, qualVec, xvec, yvec);
}

std::vector<float> clean(const std::vector<float>& v) {
    std::vector<float> vFiltered;
    for (int i = 0; i < v.size(); i++) {
        if (v[i] != -1)
            vFiltered.push_back(v[i]);
    }
    return vFiltered;
}

template <typename T> void sortByColumn(std::vector<std::vector<T>>& data, size_t column) {
    std::sort(data.begin(), data.end(), [column](const std::vector<T>& v1, const std::vector<T>& v2) {
        return v1[column] > v2[column];
        });
}

std::string filterSelector(std::string input) {
    if (input == "LRGB")
        return "LRGB";
    else if (input == "RGB" && filter == "L")
        return "L";
    else if (input == "RGB" && filter != "L")
        return "RGB";
    else
        return filter;
}

//Function for enumerating star triangles
std::vector<std::vector<float>> triangles(const std::vector<float>& x, const std::vector<float>& y) {
    std::vector<std::vector<float>> triangleParameters;
    const float minEdge = 50;
    const int n = x.size();
    for (int i = 0; i < n - 2; i++) {
        for (int j = i + 1; j < n - 1; j++) {
            for (int k = j + 1; k < n; k++) {
                std::vector<double> d = { sqrt(pow(x[i] - x[j], 2) + pow(y[i] - y[j], 2)), sqrt(pow(x[j] - x[k], 2) + pow(y[j] - y[k], 2)), sqrt(pow(x[i] - x[k], 2) + pow(y[i] - y[k], 2)) };
                if (*std::min_element(d.begin(), d.end()) > minEdge) {
                    std::sort(d.begin(), d.end());
                    triangleParameters.push_back({ float(i), float(j), float(k), float(d[1] / d[2]), float(d[0] / d[2]) });
                }
            }
        }
    }
    return triangleParameters;
}

//Function for computing angular and translational offsets between vectors
std::vector<float> findRT(const cv::Mat& A, const cv::Mat& B) {
    cv::Mat centroid_A, centroid_B, A_centered, B_centered;
    cv::reduce(A, centroid_A, 1, cv::REDUCE_AVG);
    cv::reduce(B, centroid_B, 1, cv::REDUCE_AVG);
    cv::subtract(A, cv::repeat(centroid_A, 1, A.cols), A_centered);
    cv::subtract(B, cv::repeat(centroid_B, 1, B.cols), B_centered);

    cv::Mat U, S, Vt;
    cv::SVD::compute(A_centered * B_centered.t(), S, U, Vt);
    cv::Mat R = (U * Vt).t();
    
    if (cv::determinant(R) < 0)
        R = (U * (cv::Mat_<float>(2, 2) << 1, 0, 0, -1) * Vt).t();

    cv::Mat t = -R * centroid_A + centroid_B;
    return { std::atan2(R.at<float>(1, 0), R.at<float>(0, 0)), t.at<float>(0, 0), t.at<float>(1, 0) };
}

//Function for computing the "vote matrix"
std::vector<std::vector<float>> getCorrectedVoteMatrix(const std::vector<std::vector<float>>& refTriangles, const std::vector<std::vector<float>>& frameTriangles, const int& refVectorSize, const int& vecSize) {
    float e = 0.005;
    std::vector<std::vector<float>> vote(refVectorSize, std::vector<float>(vecSize, 0)), corrVote(refVectorSize, std::vector<float>(vecSize, 0));
    for (int a = 0; a < refTriangles.size(); a++) {
        std::vector<int> triangleList;
        for (int b = 0; b < frameTriangles.size(); b++) {
            if (std::abs(refTriangles[a][3] - frameTriangles[b][3]) < e)
                triangleList.push_back(b);
        }
        for (int c = 0; c < triangleList.size(); c++) {
            int b = triangleList[c];
            if (std::abs(refTriangles[a][3] - frameTriangles[b][3]) + std::abs(refTriangles[a][4] - frameTriangles[b][4]) < e) {
                for(int i = 0; i < 3; i++)
                    vote[(int)refTriangles[a][i]][(int)frameTriangles[b][i]] += 1;
            }
        }
    }

    for (int row = 0; row < vote.size(); row++) {
        double maxRowVote = *std::max_element(vote[row].begin(), vote[row].end());
        int ind = std::distance(vote[row].begin(), std::max_element(vote[row].begin(), vote[row].end()));
        corrVote[row][ind] = std::max(maxRowVote - std::max(*std::max_element(vote[row].begin(), vote[row].begin() + ind), *std::max_element(vote[row].begin() + ind + 1, vote[row].end())), 0.0);
    }
    return corrVote;
}

//Function for aligning frames
std::vector<float> alignFrames(const std::vector<std::vector<float>>& corrVote, const std::vector<float>& refVectorX, const std::vector<float>& refVectorY, const std::vector<float>& xvec, const std::vector<float>& yvec, const int& topMatches) {
    std::vector<std::vector<int>> starPairs;
    for (int i = 0; i < corrVote[0].size(); i++) {
        int maxIndex = 0;
        int maxValue = corrVote[0][i];
        for (int j = 1; j < corrVote.size(); j++) {
            if (corrVote[j][i] > maxValue) {
                maxIndex = j;
                maxValue = corrVote[j][i];
            }
        }
        starPairs.push_back({ i, maxIndex, maxValue });
    }

    sortByColumn(starPairs, 2);
    cv::Mat referenceM(2, topMatches, CV_32F), frameM(2, topMatches, CV_32F);

    for (int i = 0; i < topMatches; i++) {
        referenceM.at<float>(0, i) = refVectorX[starPairs[i][1]];
        referenceM.at<float>(1, i) = refVectorY[starPairs[i][1]];
        frameM.at<float>(0, i) = xvec[starPairs[i][0]];
        frameM.at<float>(1, i) = yvec[starPairs[i][0]];
    }

    std::vector<float> RTparams = findRT(frameM, referenceM);
    return RTparams;
}

//Function to get all the file names in the given directory.
std::vector<std::string> getFrames(const std::string& path, const std::string& ext) {
    std::vector<std::string> filenames;

    if (std::filesystem::exists(path))
    {
        for (auto& p : std::filesystem::recursive_directory_iterator(path))
        {
            if (p.path().extension() == ext)
                filenames.push_back(p.path().string());
        }
    }
    return filenames;
}

//Function to analyze the star field in the given light frame.
std::vector<std::vector<float>> analyzeStarField(cv::Mat lightFrame, const float& t) {
    std::vector<std::vector<float>> starMatrix;

    if ((lightFrame.elemSize() == 1 || lightFrame.elemSize() == 2) && lightFrame.channels() == 1)
    {
        if (lightFrame.elemSize() == 2)
        {
            lightFrame = lightFrame / 255;
            lightFrame.convertTo(lightFrame, CV_8U);
        }
        cv::Mat filteredImage, thresh;
        cv::medianBlur(lightFrame, filteredImage, 3);
        cv::threshold(filteredImage, thresh, t * 255, 255, 0);

        std::vector<std::vector<cv::Point>> contours;
        std::vector<cv::Vec4i> hierarchy;
        cv::findContours(thresh, contours, hierarchy, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

        for (const auto& c : contours) {
            cv::RotatedRect rect = cv::minAreaRect(c);
            cv::Point2f center = rect.center;
            float height = rect.size.height;
            float width = rect.size.width;
            std::vector<float> starVector = { center.x, center.y, std::sqrt(width * width + height * height) };
            if (std::sqrt(width * width + height * height) < 25)
                starMatrix.push_back(starVector);
        }
    }
    return starMatrix;
}

cv::Mat addCircles(cv::Mat img, const std::vector<float>& xcoords, const std::vector<float>& ycoords, const int& size) {
    cv::Scalar color;

    if (align == "R")
        color = cv::Scalar(0, 0, 255);
    else if (align == "G")
        color = cv::Scalar(0, 255, 0);
    else if (align == "B")
        color = cv::Scalar(255, 0, 0);
    else 
        color = cv::Scalar(255, 255, 255);

    for (int i = 0; i < xcoords.size(); i++) 
        cv::circle(img, cv::Point_(xcoords[i] / scaling, ycoords[i] / scaling), size, color);

    return img;
}

//Function to fetch a calibration frame
cv::Mat getCalibrationFrame(const int& ySize, const int& xSize, const std::string& calibrationPath, const float& defaultValue) {
    cv::Mat masterFrame(ySize, xSize, CV_32FC1, cv::Scalar(defaultValue));

    if (std::filesystem::exists(calibrationPath + "/" + "masterFrame.tif"))
    {
        cv::Mat tmpCalibrationFrame = cv::imread(calibrationPath + "/" + "masterFrame.tif", cv::IMREAD_ANYDEPTH);
        if (tmpCalibrationFrame.cols == masterFrame.cols && tmpCalibrationFrame.rows == masterFrame.rows)
            masterFrame = tmpCalibrationFrame;
    }
    else
    {
        std::vector<std::string> calibrationFrameArray = getFrames(calibrationPath + "/", ext);
        if (!calibrationFrameArray.empty())
        {
            cv::Mat tmpMasterFrame(ySize, xSize, CV_32FC1, cv::Scalar(0));
            #pragma omp parallel for num_threads(numLogicalCores*2)
            for (int n = 0; n < calibrationFrameArray.size(); n++)
            {
                cv::Mat calibrationFrame = cv::imread(calibrationFrameArray[n], cv::IMREAD_ANYDEPTH);
                if (calibrationFrame.cols == masterFrame.cols && calibrationFrame.rows == masterFrame.rows)
                {
                    calibrationFrame.convertTo(calibrationFrame, CV_32FC1, 1.0 / pow(255, calibrationFrame.elemSize()));
                    addWeighted(tmpMasterFrame, 1, calibrationFrame, 1 / float(calibrationFrameArray.size()), 0.0, tmpMasterFrame);
                }
            }
            imwrite(calibrationPath + "/" + "masterFrame" + ".tif", tmpMasterFrame);
            masterFrame = tmpMasterFrame;
        }
    }
    return masterFrame;
}

//Function to remove hotpixels
cv::Mat removeHotPixels(cv::Mat lightFrame, const std::vector <std::vector<int>>& hotPixels) {
    for (int i = 0; i < hotPixels.size(); i++)
    {
        int x = hotPixels[i][0];
        int y = hotPixels[i][1];
        if (x > 0 || y > 0 || x < lightFrame.cols - 1 || y < lightFrame.rows - 1)
        {
            float sum = 0;
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    if (!(dx == 0 && dy == 0))
                        sum += lightFrame.at<float>(y + dy, x + dx);
                }
            }
            lightFrame.at<float>(y, x) = sum / 8;
        }
    }
    return lightFrame;
}

//Function to rotate images
cv::Mat processFrame(const std::string& framePath, const cv::Mat& masterDarkFrame, const cv::Mat& calibratedFlatFrame, const float& backGroundCorrection, const std::vector<float>& RTparams, const std::vector<std::vector<int>>& hotPixels) {
    cv::Mat lightFrame = cv::imread(framePath, cv::IMREAD_GRAYSCALE);
    lightFrame.convertTo(lightFrame, CV_32FC1, 1.0 / pow(255, lightFrame.elemSize()));
    lightFrame = backGroundCorrection * (lightFrame - masterDarkFrame) / calibratedFlatFrame;
    lightFrame = removeHotPixels(lightFrame, hotPixels);
    cv::resize(lightFrame, lightFrame, cv::Size(samplingFactor * lightFrame.cols, samplingFactor * lightFrame.rows), 0, 0, cv::INTER_CUBIC);
    cv::Mat M = (cv::Mat_<float>(2, 3) << cos(RTparams[0]), -sin(RTparams[0]), RTparams[1], sin(RTparams[0]), cos(RTparams[0]), RTparams[2]);
    warpAffine(lightFrame, lightFrame, M, lightFrame.size(), interpolationFlag);
    return lightFrame;
}

//Function to compute median image
cv::Mat computeMedianImage(const std::vector<cv::Mat>& imageStack) {
    int rows = imageStack[0].rows;
    int cols = imageStack[0].cols;
    int totalPixels = rows * cols;
    int numImages = imageStack.size();
    int midIndex = numImages / 2;

    cv::Mat medianImage(rows, cols, CV_32FC1);

    #pragma omp parallel num_threads(numLogicalCores*2)
    {
        std::vector<float> pixelValues(numImages);

        #pragma omp for
        for (int i = 0; i < totalPixels; i++) {
            for (int imgIdx = 0; imgIdx < numImages; imgIdx++) 
                pixelValues[imgIdx] = imageStack[imgIdx].at<float>(i);

            std::partial_sort(pixelValues.begin(), pixelValues.begin() + midIndex + 1, pixelValues.end());
            medianImage.at<float>(i) = (numImages % 2 == 0) ? (pixelValues[midIndex] + pixelValues[midIndex - 1]) / 2.0f : pixelValues[midIndex];
        }
    }

    return medianImage;
}

//Function to read images
std::vector<int> Hydra::Form1::RegisterFrames() {
    int elapsedTime = 0;

    std::vector<std::string> lightFrames = getFrames(path + lightDir + filter, ext);
    int n = lightFrames.size();

    if (!lightFrames.empty())
    {
        auto startTime = std::chrono::high_resolution_clock::now();
        std::vector<std::vector<float>> qualVec(lightFrames.size(), std::vector<float>(6 + 2 * maxStars, -1));
        std::vector<std::vector<std::string>> qualVecS(lightFrames.size(), std::vector<std::string>(6 + 2 * maxStars));

        #pragma omp parallel for num_threads(numLogicalCores*2)
        for (int k = 0; k < n; k++) {
            cv::Mat lightFrame = cv::imread(lightFrames[k], cv::IMREAD_ANYCOLOR | cv::IMREAD_ANYDEPTH);
            if (!lightFrame.empty()) {
                std::vector<std::vector<float>> starMatrix = analyzeStarField(lightFrame, float(detectionThreshold) / 100);

                qualVec[k][0] = k;
                qualVec[k][1] = starMatrix.size();
                qualVec[k][2] = cv::sum(lightFrame)[0];
                qualVec[k][3] = lightFrame.cols;
                qualVec[k][4] = lightFrame.rows;
                qualVec[k][5] = lightFrame.elemSize();

                if (starMatrix.size() > 3) {
                    sortByColumn(starMatrix, 2);
                    for (int i = 0; i < std::min(maxStars, int(starMatrix.size())); i++) {
                        qualVec[k][i + 6] = starMatrix[i][0];
                        qualVec[k][i + 6 + maxStars] = starMatrix[i][1];
                    }
                }
            }
        }

        sortByColumn(qualVec, 1);

        for (int k = 0; k < qualVec.size(); k++) {
            qualVecS[k][0] = lightFrames[int(qualVec[k][0])];
            for (int l = 1; l < qualVec[0].size(); l++) 
                qualVecS[k][l] = std::to_string(qualVec[k][l]);
        }

        elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime).count();

        if (!std::filesystem::exists(path + parameterDir))
            std::filesystem::create_directory(path + parameterDir);

        writeStringMatrix(path + parameterDir + "qualVec" + filter + ".csv", qualVecS);
    }
    return { n, elapsedTime };
}

//Function for computing angular and translational offsets
std::vector<int> Hydra::Form1::ComputeOffsets() {
    int elapsedTime = 0;
    int n = 0;

    std::string qualVecPath = path + parameterDir + "qualVec" + filter + ".csv";
    std::string qualVecAlignPath = path + parameterDir + "qualVec" + align + ".csv";

    if ((std::filesystem::exists(qualVecPath) && std::filesystem::exists(qualVecAlignPath)))
    {
        auto startTime = std::chrono::high_resolution_clock::now();

        std::tuple tuple = unpack(readStringMatrix(qualVecPath));
        std::tuple alignTuple = unpack(readStringMatrix(qualVecAlignPath));

        std::vector<std::string> lightFrameArray = std::get<0>(tuple);
        std::vector<std::vector<float>> qualVec = std::get<1>(tuple);
        std::vector<std::vector<float>> xvec = std::get<2>(tuple);
        std::vector<std::vector<float>> yvec = std::get<3>(tuple);

        std::vector<std::string> lightFrameArrayAlign = std::get<0>(alignTuple);
        std::vector<std::vector<float>> qualVecAlign = std::get<1>(alignTuple);
        std::vector<std::vector<float>> xvecAlign = std::get<2>(alignTuple);
        std::vector<std::vector<float>> yvecAlign = std::get<3>(alignTuple);

        bool sizesEqual = true;
        int l = 0;

        while (sizesEqual == true && l < qualVec.size())
        {
            if ((qualVec[l][2] != qualVec[0][2]) || (qualVec[l][3] != qualVec[0][3]))
                sizesEqual = false;

            l++;
        }

        if (sizesEqual)
        {
            std::vector xRef = clean(xvecAlign[0]);
            std::vector yRef = clean(yvecAlign[0]);

            if (!xRef.empty() && xRef.size() >= topMatches)
            {
                n = floor(qualVec.size() * (100 - float(discardPercentage)) / 100);
                std::vector<std::vector<float>> refTriangles = triangles(xRef, yRef);
                std::vector<std::vector<float>> offsets(n, std::vector<float>(7));
                std::vector<std::vector<std::string>> stackArray(n, std::vector<std::string>(8));

                for (int k = 0; k < n; k++) {
                    if (!clean(xvec[k]).empty() && clean(xvec[k]).size() >= topMatches)
                    {
                        std::vector<std::vector<float>> frameTriangles = triangles(clean(xvec[k]), clean(yvec[k]));
                        std::vector<std::vector<float>> correctedVoteMatrix = getCorrectedVoteMatrix(refTriangles, frameTriangles, clean(xvecAlign[0]).size(), clean(yvec[0]).size());
                        std::vector<float> RTparams = alignFrames(correctedVoteMatrix, clean(xvecAlign[0]), clean(yvecAlign[0]), clean(xvec[k]), clean(yvec[k]), topMatches);
                        offsets[k] = { float(qualVec[k][0]), float(qualVec[k][1]), float(qualVec[k][2]), float(qualVec[k][3]), RTparams[0], RTparams[1], RTparams[2] };
                        stackArray[k] = { lightFrameArray[k], std::to_string(offsets[k][0]), std::to_string(offsets[k][1]), std::to_string(offsets[k][2]),
                                          std::to_string(offsets[k][3]), std::to_string(offsets[k][4]), std::to_string(offsets[k][5]), std::to_string(offsets[k][6]) };
                    }
                }

                elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime).count();

                writeStringMatrix(path + parameterDir + "stackArray" + filter + ".csv", stackArray);

                std::vector<float> xDeb(maxStars), yDeb(maxStars);

                cv::Mat maxQualFrame = cv::imread(lightFrameArrayAlign[0], cv::IMREAD_GRAYSCALE);
                cv::Mat small;
                cv::resize(maxQualFrame, small, cv::Size(maxQualFrame.cols / scaling, maxQualFrame.rows / scaling), 0, 0, cv::INTER_CUBIC);
                cv::Mat labelledImage(small.size(), CV_8UC3);
                cv::cvtColor(small, labelledImage, cv::COLOR_GRAY2BGR);
                labelledImage = addCircles(labelledImage, xRef, yRef, 8);

                for (int i = 0; i < offsets.size(); i++) {
                    for (int j = 0; j < xvec[i].size(); j++) {
                        xDeb[j] = cos(offsets[i][4]) * xvec[i][j] - sin(offsets[i][4]) * yvec[i][j] + offsets[i][5];
                        yDeb[j] = sin(offsets[i][4]) * xvec[i][j] + cos(offsets[i][4]) * yvec[i][j] + offsets[i][6];
                    }
                    labelledImage = addCircles(labelledImage, clean(xDeb), clean(yDeb), 5);
                }
                cv::imshow("Debug", labelledImage);
            }
        }
    }
    return { n, elapsedTime };
    cv::waitKey(0);
    cv::destroyAllWindows();
}

//Function for stacking the images
std::vector<int> Hydra::Form1::Stack() {
    int elapsedTime = 0;
    int n = 0;

    std::string stackArrayPath = path + parameterDir + "stackArray" + filter + ".csv";

    if (std::filesystem::exists(stackArrayPath))
    {
        auto startTime = std::chrono::high_resolution_clock::now();

        std::vector<std::vector<std::string>> stackInfo = readStringMatrix(stackArrayPath);
        std::vector<std::string> stackArray(stackInfo.size());
        std::vector<std::vector<float>> RTparams(stackInfo.size());
        std::vector<float>background(stackInfo.size());
        float mean_background = 0;

        n = stackInfo.size();

        for (int i = 0; i < stackInfo.size(); i++)
        {
            stackArray[i] = stackInfo[i][0];
            RTparams[i] = { stof(stackInfo[i][5]), samplingFactor * stof(stackInfo[i][6]), samplingFactor * stof(stackInfo[i][7]) };
            background[i] = stof(stackInfo[i][2]);
            mean_background = mean_background + background[i] / float(stackInfo.size());
        }

        int xSize = stoi(stackInfo[0][3]);
        int ySize = stoi(stackInfo[0][4]);

        cv::Mat calibratedFlatFrame = getCalibrationFrame(ySize, xSize, path + flatDir + filter, 1) - getCalibrationFrame(ySize, xSize, path + flatDarksDir + filterSelector(flatDarksGroup), 0);
        cv::Mat masterDarkFrame = getCalibrationFrame(ySize, xSize, path + darkDir + filterSelector(darksGroup), 0);

        cv::Scalar mean, stddev;
        cv::meanStdDev(masterDarkFrame, mean, stddev);

        std::vector<std::vector<int>> hotPixels;

        for (int y = 0; y < masterDarkFrame.rows; y++) {
            for (int x = 0; x < masterDarkFrame.cols; x++) {
                float pixelValue = masterDarkFrame.at<float>(y, x);
                if (pixelValue > 10 * mean[0]) 
                    hotPixels.push_back({ x,y });
            }
        }

        double minVal, maxVal;
        cv::minMaxLoc(calibratedFlatFrame, &minVal, &maxVal);

        if (minVal > 0)
        {
            calibratedFlatFrame *= xSize * ySize / cv::sum(calibratedFlatFrame)[0];
            if (stackInfo.size() < medianBatchSize)
                medianBatchSize = stackInfo.size();

            int batches = stackInfo.size() / medianBatchSize;
            int iterations = medianBatchSize * batches;

            std::vector<int> m(iterations);

            for (int j = 0; j < iterations; j++)
                m[j] = j;

            xSize = int(xSize * samplingFactor);
            ySize = int(ySize * samplingFactor);

            cv::Mat p(ySize, xSize, CV_32FC1, cv::Scalar(0)), psqr(ySize, xSize, CV_32FC1, cv::Scalar(0)), var(ySize, xSize, CV_32FC1, cv::Scalar(0)), medianFrame(ySize, xSize, CV_32FC1, cv::Scalar(0)), stackFrame(ySize, xSize, CV_32FC1, cv::Scalar(0));
            std::vector<cv::Mat> tempArray(medianBatchSize, cv::Mat(ySize, xSize, CV_32FC1));

            unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
            shuffle(m.begin(), m.end(), std::default_random_engine(seed));

            for (int k = 0; k < batches; k++) {
                #pragma omp parallel for num_threads(numLogicalCores*2)
                for (int tempcount = 0; tempcount < medianBatchSize; tempcount++) {
                    int i = m[k * medianBatchSize + tempcount];
                    tempArray[tempcount] = processFrame(stackArray[i], masterDarkFrame, calibratedFlatFrame, mean_background / background[i], RTparams[i], hotPixels);;
                    addWeighted(p, 1, tempArray[tempcount] / iterations, 1, 0.0, p);
                    addWeighted(psqr, 1, tempArray[tempcount].mul(tempArray[tempcount]) / iterations, 1, 0.0, psqr);
                }
                addWeighted(medianFrame, 1, computeMedianImage(tempArray), 1 / float(batches), 0.0, medianFrame);
            }

            var = (psqr - p.mul(p)) * iterations / (iterations - 1);

            if (!std::filesystem::exists(path + outputDir))
                std::filesystem::create_directory(path + outputDir);

            imwrite(path + outputDir + "Median" + "_" + std::to_string(stackInfo.size()) + "_" + filter + "_" + std::to_string(int(samplingFactor * 100)) + ".tif", medianFrame);
            imwrite(path + outputDir + "Mean" + "_" + std::to_string(stackInfo.size()) + "_" + filter + "_" + std::to_string(int(samplingFactor * 100)) + ".tif", p);

            #pragma omp parallel for num_threads(numLogicalCores*2) 
            for (int k = 0; k < stackInfo.size(); k++) {
                cv::Mat lightFrame = processFrame(stackArray[k], masterDarkFrame, calibratedFlatFrame, mean_background / background[k], RTparams[k], hotPixels);

                for (int h = 0; h < xSize * ySize; h++)
                {
                    if (abs(lightFrame.at<float>(h) - medianFrame.at<float>(h)) > 2.0 * cv::sqrt(var.at<float>(h)))
                        lightFrame.at<float>(h) = medianFrame.at<float>(h);
                }

                addWeighted(stackFrame, 1, lightFrame, 1 / float(stackInfo.size()), 0.0, stackFrame);
            }

            imwrite(path + outputDir + "Stack" + "_" + std::to_string(stackInfo.size()) + "_" + filter + "_" + std::to_string(int(samplingFactor * 100)) + ".tif", stackFrame);

            elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime).count();

            cv::Mat small;
            cv::resize(stackFrame, small, cv::Size(stackFrame.cols / (scaling * samplingFactor), stackFrame.rows / (scaling * samplingFactor)), 0, 0, cv::INTER_CUBIC);
            cv::imshow("Stack", small * 5);
        }
    }
    return { n, elapsedTime };
    cv::waitKey(0);
    cv::destroyAllWindows();
}