#include "pch.h"
#include "Form1.h"

std::string path = "C:/F/astro/matlab/m76/";
std::string parameterDir = "/parameters/";
std::string outputDir = "/output/";
std::string lightDir = "/lights/";
std::string darkDir = "/darks/";
std::string flatDir = "/flats/";
std::string flatDarksDir = "/flatDarks/";
std::string darkGroup = "RGB";
std::string flatDarksGroup = "LRGB";
std::string ext = ".png";
int detectionThreshold = 50;
int discardPercentage = 10;
int medianBatchSize = 30;
int interpolationFlag = 2;
int maxStars = 15;
int topMatches = 6;
int scaling = 4;
float samplingFactor = 1;
std::string filter = "R";
std::string align = "R";

std::vector<std::vector<std::string>> readStringMatrix(std::string path)
{
    std::vector<std::vector<std::string>> commaSeparatedArray;
    std::string line;
    std::ifstream commaSeparatedArrayStream(path);
    if (commaSeparatedArrayStream.is_open())
    {
        while (std::getline(commaSeparatedArrayStream, line)) {
            int pos = 0;
            std::vector<std::string> commaSeparatedLine;
            while (pos > -1) {
                pos = line.find(",");
                commaSeparatedLine.push_back(line.substr(0, pos));
                line.erase(0, pos + 1);
            }
            commaSeparatedArray.push_back(commaSeparatedLine);
        }
        commaSeparatedArrayStream.close();
    }
    return commaSeparatedArray;
}

void writeStringMatrix(std::string path, std::vector<std::vector<std::string>> stringArray)
{
    std::ofstream stringFileStream(path);
    for (int i = 0; i < stringArray.size(); i++)
    {
        for (int j = 0; j < stringArray[0].size(); j++)
        {
            stringFileStream << stringArray[i][j];
            if (j < stringArray[0].size() - 1)
                stringFileStream << ",";
        }
        stringFileStream << "\n";
    }
    stringFileStream.close();
}

std::tuple<std::vector<std::string>, std::vector<std::vector<float>>, std::vector<std::vector<float>>, std::vector<std::vector<float>>> unpack(std::vector<std::vector<std::string>> inputArray)
{
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

std::vector<float> clean(std::vector<float> v)
{
    std::vector<float> vFiltered;
    for (int i = 0; i < v.size(); i++) {
        if (v[i] != -1)
            vFiltered.push_back(v[i]);
    }
    return vFiltered;
}

void sortFloatByColumn(std::vector<std::vector<float>>& data, size_t column) {
    std::sort(data.begin(), data.end(), [column](std::vector<float> const& v1, std::vector<float> const& v2)
        {
            return v1[column] > v2[column];
        });
}

void sortIntByColumn(std::vector<std::vector<int>>& data, size_t column) {
    std::sort(data.begin(), data.end(), [column](std::vector<int> const& v1, std::vector<int> const& v2)
        {
            return v1[column] > v2[column];
        });
}

//Function for enumerating star triangles
std::vector<std::vector<float>> triangles(std::vector<float> x, std::vector<float> y) {
    std::vector<std::vector<float>> triangleParameters;
    float minEdge = 50;
    for (int i = 0; i < x.size() - 2; i++) {
        for (int j = i + 1; j < x.size() - 1; j++) {
            for (int k = j + 1; k < x.size(); k++) {
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
std::vector<float> findRT(Eigen::MatrixXf A, Eigen::MatrixXf B) {
    Eigen::Vector2f centroid_A = A.rowwise().mean().reshaped(-1, 1);
    Eigen::Vector2f centroid_B = B.rowwise().mean().reshaped(-1, 1);
    Eigen::MatrixXf Am = A - centroid_A.replicate(1, A.cols());
    Eigen::MatrixXf Bm = B - centroid_B.replicate(1, B.cols());
    Eigen::MatrixXf H = Am * Bm.transpose();
    Eigen::JacobiSVD<Eigen::MatrixXf> svd(H, Eigen::ComputeThinU | Eigen::ComputeThinV);
    Eigen::MatrixXf U = svd.matrixU();
    Eigen::MatrixXf V = svd.matrixV();
    Eigen::MatrixXf R = V * U.transpose();
    if (R.determinant() < 0) {
        V.col(1) *= -1;
        R = V * U.transpose();
    }
    Eigen::Vector2f t = -R * centroid_A + centroid_B;
    return { std::atan2(R(1, 0), R(0, 0)), t[0], t[1] };
}

//Function for computing the "vote matrix"
std::vector<std::vector<float>> getCorrectedVoteMatrix(std::vector<std::vector<float>> refTriangles, std::vector<std::vector<float>> frameTriangles, std::vector<float> refVectorX, std::vector<float> yvec) {
    float e = 0.005;
    std::vector<std::vector<float>> vote(refVectorX.size(), std::vector<float>(yvec.size(), 0)), corrVote(refVectorX.size(), std::vector<float>(yvec.size(), 0));
    for (int a = 0; a < refTriangles.size(); a++) {
        std::vector<int> triangleList;
        for (int b = 0; b < frameTriangles.size(); b++) {
            if (std::abs(refTriangles[a][3] - frameTriangles[b][3]) < e)
                triangleList.push_back(b);
        }
        for (int c = 0; c < triangleList.size(); c++) {
            int b = triangleList[c];
            if (std::abs(refTriangles[a][3] - frameTriangles[b][3]) + std::abs(refTriangles[a][4] - frameTriangles[b][4]) < e) {
                vote[(int)refTriangles[a][0]][(int)frameTriangles[b][0]] += 1;
                vote[(int)refTriangles[a][1]][(int)frameTriangles[b][1]] += 1;
                vote[(int)refTriangles[a][2]][(int)frameTriangles[b][2]] += 1;
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
std::vector<float> alignFrames(std::vector<std::vector<float>> corrVote, std::vector<float> refVectorX, std::vector<float> refVectorY, std::vector<float> xvec, std::vector<float> yvec, int topMatches) {
    std::vector<std::vector<int>> votePairs;
    for (int i = 0; i < corrVote[0].size(); i++) {
        int maxIndex = 0;
        int maxValue = corrVote[0][i];
        for (int j = 1; j < corrVote.size(); j++) {
            if (corrVote[j][i] > maxValue) {
                maxIndex = j;
                maxValue = corrVote[j][i];
            }
        }
        votePairs.push_back({ i, maxIndex, maxValue });
    }

    sortIntByColumn(votePairs, 2);

    std::vector<std::vector<int>> rankPairs(votePairs.begin(), votePairs.end());
    Eigen::MatrixXf referenceM(2, topMatches), frameM(2, topMatches);

    for (int i = 0; i < topMatches; i++) {
        referenceM(0, i) = refVectorX[rankPairs[i][1]];
        referenceM(1, i) = refVectorY[rankPairs[i][1]];
        frameM(0, i) = xvec[rankPairs[i][0]];
        frameM(1, i) = yvec[rankPairs[i][0]];
    }
    std::vector<float> RTparams = findRT(frameM, referenceM);
    return RTparams;
}

//Function to get all the file names in the given directory.
std::vector<std::string> getFrames(std::string path, std::string ext) {
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
std::vector<std::vector<float>> analyzeStarField(cv::Mat lightFrame, float t) {
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
            std::vector<float> starVector = { center.x, center.y, width, height, std::sqrt(width * width + height * height) };
            if (std::sqrt(width * width + height * height) < 25)
                starMatrix.push_back(starVector);
        }
    }
    return starMatrix;
}

//Function to fetch a calibration frame
cv::Mat getCalibrationFrame(int ySize, int xSize, std::string calibrationPath, float defaultValue)
{
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
            #pragma omp parallel for num_threads(8)
            for (int n = 0; n < calibrationFrameArray.size(); n++)
            {
                cv::Mat calibrationFrame = cv::imread(calibrationFrameArray[n], cv::IMREAD_ANYDEPTH);
                if (calibrationFrame.cols == masterFrame.cols && calibrationFrame.rows == masterFrame.rows)
                {
                    calibrationFrame.convertTo(calibrationFrame, CV_32FC1, 1.0 / pow(255, calibrationFrame.elemSize()));
                    addWeighted(masterFrame, 1, calibrationFrame, 1 / float(calibrationFrameArray.size()), 0.0, masterFrame);
                }
            }
            imwrite(calibrationPath + "/" + "masterFrame" + ".tif", masterFrame);
        }
    }
    return masterFrame;
}

//Function to rotate images
cv::Mat processFrame(std::string framePath, cv::Mat masterDarkFrame, cv::Mat calibratedFlatFrame, float backGroundCorrection, std::vector<float> RTparams)
{
    cv::Mat lightFrame = cv::imread(framePath, cv::IMREAD_GRAYSCALE);
    lightFrame.convertTo(lightFrame, CV_32FC1, 1.0 / pow(255, lightFrame.elemSize()));
    lightFrame = backGroundCorrection * (lightFrame - masterDarkFrame) / calibratedFlatFrame;
    cv::resize(lightFrame, lightFrame, cv::Size(samplingFactor * lightFrame.cols, samplingFactor * lightFrame.rows), 0, 0, cv::INTER_CUBIC);
    cv::Mat M = (cv::Mat_<float>(2, 3) << cos(RTparams[0]), -sin(RTparams[0]), RTparams[1], sin(RTparams[0]), cos(RTparams[0]), RTparams[2]);
    warpAffine(lightFrame, lightFrame, M, lightFrame.size(), interpolationFlag);
    return lightFrame;
}

//Function to read images
std::vector<int> Hydra::Form1::ReadImages() {
    int elapsedTime = 0;

    std::vector<std::string> lightFrames = getFrames(path + lightDir + filter, ext);
    int n = lightFrames.size();

    if (!lightFrames.empty())
    {
        auto startTime = std::chrono::high_resolution_clock::now();
        std::vector<std::vector<float>> qualVec(lightFrames.size(), std::vector<float>(6 + 2 * maxStars, -1));
        std::vector<std::vector<std::string>> qualVecS(lightFrames.size(), std::vector<std::string>(6 + 2 * maxStars));

        #pragma omp parallel for num_threads(8)
        for (int k = 0; k < lightFrames.size(); k++) {
            cv::Mat lightFrame = cv::imread(lightFrames[k], cv::IMREAD_ANYCOLOR | cv::IMREAD_ANYDEPTH);
            if (lightFrame.data != NULL) {
                std::vector<std::vector<float>> starMatrix = analyzeStarField(lightFrame, float(detectionThreshold) / 100);

                qualVec[k][0] = k;
                qualVec[k][1] = starMatrix.size();
                qualVec[k][2] = cv::sum(lightFrame)[0];
                qualVec[k][3] = lightFrame.cols;
                qualVec[k][4] = lightFrame.rows;
                qualVec[k][5] = lightFrame.elemSize();

                if (starMatrix.size() > 3) {
                    sortFloatByColumn(starMatrix, 4);
                    for (int i = 0; i < std::min(maxStars, int(starMatrix.size())); i++) {
                        qualVec[k][i + 6] = starMatrix[i][0];
                        qualVec[k][i + 6 + maxStars] = starMatrix[i][1];
                    }
                }
            }
        }

        sortFloatByColumn(qualVec, 1);

        for (int k = 0; k < qualVec.size(); k++) {
            qualVecS[k][0] = lightFrames[int(qualVec[k][0])];
            for (int l = 1; l < qualVec[0].size(); l++) {
                qualVecS[k][l] = std::to_string(qualVec[k][l]);
            }
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
                        std::vector<std::vector<float>> correctedVoteMatrix = getCorrectedVoteMatrix(refTriangles, frameTriangles, clean(xvecAlign[0]), clean(yvec[0]));
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
                cv::Mat img_rgb(small.size(), CV_8UC3);
                cv::cvtColor(small, img_rgb, cv::COLOR_GRAY2BGR);

                for (int i = 0; i < xRef.size(); i++) {
                    if (align == "R")
                        cv::circle(img_rgb, cv::Point_(xRef[i] / scaling, yRef[i] / scaling), 8, cv::Scalar(0, 0, 255));
                    else if (align == "G")
                        cv::circle(img_rgb, cv::Point_(xRef[i] / scaling, yRef[i] / scaling), 8, cv::Scalar(0, 255, 0));
                    else if (align == "B")
                        cv::circle(img_rgb, cv::Point_(xRef[i] / scaling, yRef[i] / scaling), 8, cv::Scalar(255, 0, 0));
                    else
                        cv::circle(img_rgb, cv::Point_(xRef[i] / scaling, yRef[i] / scaling), 8, cv::Scalar(255, 255, 255));
                }

                for (int i = 0; i < offsets.size(); i++) {
                    for (int j = 0; j < xvec[i].size(); j++) {
                        xDeb[j] = cos(offsets[i][4]) * xvec[i][j] - sin(offsets[i][4]) * yvec[i][j] + offsets[i][5];
                        yDeb[j] = sin(offsets[i][4]) * xvec[i][j] + cos(offsets[i][4]) * yvec[i][j] + offsets[i][6];
                    }
                    xDeb = clean(xDeb);
                    yDeb = clean(yDeb);
                    for (int i = 0; i < xDeb.size(); i++) {
                        if (filter == "R")
                            cv::circle(img_rgb, cv::Point_(xDeb[i] / scaling, yDeb[i] / scaling), 5, cv::Scalar(0, 0, 255));
                        else if (filter == "G")
                            cv::circle(img_rgb, cv::Point_(xDeb[i] / scaling, yDeb[i] / scaling), 5, cv::Scalar(0, 255, 0));
                        else if (filter == "B")
                            cv::circle(img_rgb, cv::Point_(xDeb[i] / scaling, yDeb[i] / scaling), 5, cv::Scalar(255, 0, 0));
                        else
                            cv::circle(img_rgb, cv::Point_(xDeb[i] / scaling, yDeb[i] / scaling), 5, cv::Scalar(255, 255, 255));
                    }
                }
                cv::imshow("Debug", img_rgb);
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

        cv::Mat masterDarkFrame = getCalibrationFrame(ySize, xSize, path + darkDir + darkGroup, 0);
        cv::Mat calibratedFlatFrame = getCalibrationFrame(ySize, xSize, path + flatDir + filter, 1) - getCalibrationFrame(ySize, xSize, path + flatDarksDir + flatDarksGroup, 0);

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
            {
                m[j] = j;
            }

            xSize = int(xSize * samplingFactor);
            ySize = int(ySize * samplingFactor);

            cv::Mat p(ySize, xSize, CV_32FC1, cv::Scalar(0)), psqr(ySize, xSize, CV_32FC1, cv::Scalar(0)), var(ySize, xSize, CV_32FC1, cv::Scalar(0)), medianFrame(ySize, xSize, CV_32FC1, cv::Scalar(0)), stackFrame(ySize, xSize, CV_32FC1, cv::Scalar(0)), tempFrame(ySize, xSize, CV_32FC1, cv::Scalar(0));
            std::vector<cv::Mat> tempArray(medianBatchSize, cv::Mat(ySize, xSize, CV_32FC1));

            unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
            shuffle(m.begin(), m.end(), std::default_random_engine(seed));

            for (int k = 0; k < batches; k++) {
                #pragma omp parallel for num_threads(8)
                for (int tempcount = 0; tempcount < medianBatchSize; tempcount++) {
                    int i = m[k * medianBatchSize + tempcount];
                    tempArray[tempcount] = processFrame(stackArray[i], masterDarkFrame, calibratedFlatFrame, mean_background / background[i], RTparams[i]);;
                    addWeighted(p, 1, tempArray[tempcount] / iterations, 1, 0.0, p);
                    addWeighted(psqr, 1, tempArray[tempcount].mul(tempArray[tempcount]) / iterations, 1, 0.0, psqr);
                }

                #pragma omp parallel for num_threads(8) 
                for (int h = 0; h < xSize * ySize; h++)
                {
                    std::vector<float> tmpVec(medianBatchSize);
                    for (int f = 0; f < medianBatchSize; f++)
                    {
                        tmpVec[f] = tempArray[f].at<float>(h);
                    }
                    if (medianBatchSize % 2 != 0)
                    {
                        std::partial_sort(tmpVec.begin(), tmpVec.begin() + medianBatchSize / 2, tmpVec.end());
                        tempFrame.at<float>(h) = tmpVec[(medianBatchSize / 2) - 1];
                    }
                    else
                    {
                        std::partial_sort(tmpVec.begin(), tmpVec.begin() + medianBatchSize / 2 + 1, tmpVec.end());
                        tempFrame.at<float>(h) = (tmpVec[medianBatchSize / 2] + tmpVec[(medianBatchSize / 2) - 1]) / 2;
                    }
                }

                addWeighted(medianFrame, 1, tempFrame, 1 / float(stackInfo.size() / medianBatchSize), 0.0, medianFrame);
            }

            var = (psqr - p.mul(p)) * iterations / (iterations - 1);

            if (!std::filesystem::exists(path + outputDir))
                std::filesystem::create_directory(path + outputDir);

            imwrite(path + outputDir + "Median" + "_" + std::to_string(stackInfo.size()) + "_" + filter + "_" + std::to_string(int(samplingFactor * 10)) + ".tif", medianFrame);
            imwrite(path + outputDir + "Mean" + "_" + std::to_string(stackInfo.size()) + "_" + filter + "_" + std::to_string(int(samplingFactor * 10)) + ".tif", p);

            #pragma omp parallel for num_threads(8) 
            for (int k = 0; k < stackInfo.size(); k++) {
                cv::Mat lightFrame = processFrame(stackArray[k], masterDarkFrame, calibratedFlatFrame, mean_background / background[k], RTparams[k]);

                for (int h = 0; h < xSize * ySize; h++)
                {
                    if (abs(lightFrame.at<float>(h) - medianFrame.at<float>(h)) > 2.0 * cv::sqrt(var.at<float>(h)))
                        lightFrame.at<float>(h) = medianFrame.at<float>(h);
                }

                addWeighted(stackFrame, 1, lightFrame, 1 / float(stackInfo.size()), 0.0, stackFrame);
            }

            imwrite(path + outputDir + "Stack" + "_" + std::to_string(stackInfo.size()) + "_" + filter + "_" + std::to_string(int(samplingFactor * 10)) + ".tif", stackFrame);

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