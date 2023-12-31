#include "pch.h"
#include "Form1.h"

std::string path = "C:/F/astro/matlab/m1test/";
std::string parameterDir = "/parametersCPP/";
std::string outDir = "/outCPP/";
std::string lightDir = "/lights/";
std::string darkDir = "/darks/";
std::string flatDir = "/flats/";
std::string flatDarksDir = "/flatDarks/";
std::string darkGroup = "RGB";
std::string flatDarksGroup = "LRGB";
std::string ext = ".png";
int detectionThreshold = 0.5;
float discardPercentage = 10;
int medianBatchSize = 30;
int interpolationFlag = 2;
int maxStars = 15;
int topMatches = 6;
std::string filter = "R";
std::string align = "R";

std::vector<std::string> readStrings(std::string path)
{
    std::vector<std::string> stringArray;
    std::string line;
    std::ifstream stringArrayStream(path);
    if (stringArrayStream.is_open())
    {
        while (std::getline(stringArrayStream, line)) {
            stringArray.push_back(line.c_str());
        }
        stringArrayStream.close();
    }
    return stringArray;
}

void writeStrings(std::string path, std::vector<std::string> stringArray)
{
    std::ofstream stringFileStream(path);
    for (int i = 0; i < stringArray.size(); i++)
    {
        stringFileStream << stringArray[i] << "\n";
    }
    stringFileStream.close();
}

std::vector<std::vector<float>> readCSV(std::string path, int size1, int size2)
{
    std::vector<std::vector<float>> commaSeparatedArray(size1, std::vector<float>(size2));
    std::string line;
    std::ifstream commaSeparatedArrayStream(path);
    if (commaSeparatedArrayStream.is_open())
    {
        int s = 0;
        int r = 0;
        while (std::getline(commaSeparatedArrayStream, line)) {
            int pos = 0;
            int r = 0;
            while (pos > -1) {
                pos = line.find(",");
                commaSeparatedArray[s][r] = stof(line.substr(0, pos));
                line.erase(0, pos + 1);
                r++;
            }
            s++;
        }
        commaSeparatedArrayStream.close();
    }
    return commaSeparatedArray;
}

void writeCSV(std::string path, std::vector<std::vector<float>> numberVector)
{
    std::ofstream numberFileStream(path);
    for (int i = 0; i < numberVector.size(); i++)
    {
        for (int j = 0; j < numberVector[0].size() - 1; j++)
        {
            numberFileStream << numberVector[i][j] << ",";
        }
        numberFileStream << numberVector[i][numberVector[0].size() - 1];
        numberFileStream << "\n";
    }
    numberFileStream.close();
}

std::vector<float> clean(std::vector<float> v)
{
    std::vector<float> vFiltered;
    for (int i = 0; i < v.size(); i++) {
        if (v[i] != -1) {
            vFiltered.push_back(v[i]);
        }
    }
    return vFiltered;
}

int argmax(std::vector<std::vector<float>> v, int col)
{
    int argMax = 0;
    int tmp = 0;
    for (int i = 0; i < v.size(); i++) {
        if (tmp < v[i][col])
        {
            tmp = v[i][col];
            argMax = i;
        }
    }
    return argMax;
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
    std::vector<std::vector<float>> triangleParameters((x.size() * (x.size() - 1) * (x.size() - 2)) / 6, std::vector<float>(5));
    float minEdge = 50;
    int count = 0;
    for (int i = 0; i < x.size() - 2; i++) {
        for (int j = i + 1; j < x.size() - 1; j++) {
            for (int k = j + 1; k < x.size(); k++) {
                std::vector<double> d = { sqrt(pow(x[i] - x[j], 2) + pow(y[i] - y[j], 2)), sqrt(pow(x[j] - x[k], 2) + pow(y[j] - y[k], 2)), sqrt(pow(x[i] - x[k], 2) + pow(y[i] - y[k], 2)) };
                if (*std::min_element(d.begin(), d.end()) > minEdge) {
                    std::sort(d.begin(), d.end());
                    float u = d[1] / *std::max_element(d.begin(), d.end());
                    float v = *std::min_element(d.begin(), d.end()) / *std::max_element(d.begin(), d.end());
                    triangleParameters[count][0] = float(i);
                    triangleParameters[count][1] = float(j);
                    triangleParameters[count][2] = float(k);
                    triangleParameters[count][3] = u;
                    triangleParameters[count][4] = v;
                    count++;
                }
            }
        }
    }
    return triangleParameters;
}

//Function for computing angular and translational offsets between vectors
std::tuple<float, float, float> findRT(Eigen::MatrixXf A, Eigen::MatrixXf B) {
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
    double theta = std::atan2(R(1, 0), R(0, 0));
    Eigen::Vector2f t = -R * centroid_A + centroid_B;
    return std::make_tuple(theta, t[0], t[1]);
}

//Function for computing the "vote matrix"
std::vector<std::vector<float>> getCorrectedVoteMatrix(std::vector<std::vector<float>> refTriangles, std::vector<std::vector<float>> frameTriangles, std::vector<float> refVectorX, std::vector<float> yvec) {
    float e = 0.005;
    std::vector<std::vector<float>> vote(refVectorX.size(), std::vector<float>(yvec.size(), 0));
    std::vector<std::vector<float>> corrVote(refVectorX.size(), std::vector<float>(yvec.size(), 0));
    for (int a = 0; a < refTriangles.size(); a++) {
        std::vector<int> triangleList;
        for (int b = 0; b < frameTriangles.size(); b++) {
            if (std::abs(refTriangles[a][3] - frameTriangles[b][3]) < e) {
                triangleList.push_back(b);
            }
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
std::tuple<float, float, float> alignFrames(std::vector<std::vector<float>> corrVote, std::vector<float> refVectorX, std::vector<float> refVectorY, std::vector<float> xvec, std::vector<float> yvec, int topMatches) {
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

    Eigen::MatrixXf referenceM(2, topMatches);
    Eigen::MatrixXf frameM(2, topMatches);

    float theta;
    float t1;
    float t2;

    for (int i = 0; i < topMatches; i++) {
        referenceM(0, i) = refVectorX[rankPairs[i][1]];
        referenceM(1, i) = refVectorY[rankPairs[i][1]];
        frameM(0, i) = xvec[rankPairs[i][0]];
        frameM(1, i) = yvec[rankPairs[i][0]];
    }
    std::tuple<float, float, float> tuple = findRT(frameM, referenceM);
    t1 = std::get<0>(tuple);
    t2 = std::get<1>(tuple);
    theta = std::get<2>(tuple);

    return std::make_tuple(t1, t2, theta);
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
        cv::Mat filteredImage;
        cv::medianBlur(lightFrame, filteredImage, 3);

        cv::Mat thresh;
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
    bool masterFrameExists = std::filesystem::exists(calibrationPath + "/" + "masterFrame.tif");

    if (masterFrameExists)
    {
        cv::Mat tmpCalibrationFrame = cv::imread(calibrationPath + "/" + "masterFrame.tif", cv::IMREAD_ANYDEPTH);
        if (tmpCalibrationFrame.cols == masterFrame.cols && tmpCalibrationFrame.rows == masterFrame.rows)
        {
            masterFrame = tmpCalibrationFrame;
        }
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

//Function to read images
int Hydra::Form1::ReadImages() {
    int elapsedTime = 0;

    std::vector<std::string> lightFrames = getFrames(path + lightDir + filter, ext);

    if (!lightFrames.empty())
    {
        auto t1 = std::chrono::high_resolution_clock::now();

        std::vector<std::vector<float>> qualVec(lightFrames.size(), std::vector<float>(5,0));
        std::vector<std::vector<float>> xvec(lightFrames.size(), std::vector<float>(maxStars, -1));
        std::vector<std::vector<float>> yvec(lightFrames.size(), std::vector<float>(maxStars, -1));

        #pragma omp parallel for num_threads(8)
        for (int n = 0; n < lightFrames.size(); n++) {
            cv::Mat lightFrame = cv::imread(lightFrames[n], cv::IMREAD_ANYCOLOR | cv::IMREAD_ANYDEPTH);
            if (lightFrame.data != NULL) {
                std::vector<std::vector<float>> starMatrix = analyzeStarField(lightFrame, float(detectionThreshold) / 100);

                qualVec[n][0] = starMatrix.size();
                qualVec[n][1] = cv::sum(lightFrame)[0];
                qualVec[n][2] = lightFrame.cols;
                qualVec[n][3] = lightFrame.rows;
                qualVec[n][4] = lightFrame.elemSize();

                if (starMatrix.size() > 3) {

                    sortFloatByColumn(starMatrix, 4);

                    for (int i = 0; i < std::min(maxStars, int(starMatrix.size())); i++) {
                        xvec[n][i] = starMatrix[i][0];
                        yvec[n][i] = starMatrix[i][1];
                    }
                }
            }
        }
        auto t2 = std::chrono::high_resolution_clock::now();
        auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
        elapsedTime = ms_int.count();

        if (!std::filesystem::exists(path + parameterDir))
        {
            std::filesystem::create_directory(path + parameterDir);
        }

        writeStrings(path + parameterDir + "lightFrameArray" + filter + ".csv", lightFrames);
        writeCSV(path + parameterDir + "qualVec" + filter + ".csv", qualVec);
        writeCSV(path + parameterDir + "xvec" + filter + ".csv", xvec);
        writeCSV(path + parameterDir + "yvec" + filter + ".csv", yvec);
    }
    return elapsedTime;
}

//Function for computing angular and translational offsets
int Hydra::Form1::ComputeOffsets() {
    int elapsedTime = 0;

    std::string lightFrameArrayPath = path + parameterDir + "lightFrameArray" + filter + ".csv";
    std::string lightFrameArrayAlignPath = path + parameterDir + "lightFrameArray" + align + ".csv";
    std::string xvecPath =  path + parameterDir + "xvec" +  filter + ".csv";
    std::string xvecAlignPath = path + parameterDir + "xvec" + align + ".csv";
    std::string yvecPath =  path + parameterDir + "yvec" +  filter + ".csv";
    std::string yvecAlignPath = path + parameterDir + "yvec" + align + ".csv";
    std::string qualVecPath =  path + parameterDir + "qualVec" +  filter + ".csv";
    std::string qualVecAlignPath =  path + parameterDir + "qualVec" +  align + ".csv";

    bool filesExist = (std::filesystem::exists(lightFrameArrayPath) && std::filesystem::exists(lightFrameArrayAlignPath) &&       
                       std::filesystem::exists(xvecPath) && std::filesystem::exists(xvecAlignPath) &&
                       std::filesystem::exists(yvecPath) && std::filesystem::exists(yvecAlignPath) &&
                       std::filesystem::exists(qualVecPath) && std::filesystem::exists(qualVecAlignPath));

    if (filesExist)
    {
        auto t1 = std::chrono::high_resolution_clock::now();

        std::vector<std::string> lightFrameArray = readStrings(lightFrameArrayPath);
        std::vector<std::string> lightFrameArrayAlign = readStrings(lightFrameArrayAlignPath);

        std::vector<std::vector<float>> xvec = readCSV(xvecPath, size(lightFrameArray), maxStars);
        std::vector<std::vector<float>> xvecAlign = readCSV(xvecAlignPath, size(lightFrameArrayAlign), maxStars);

        std::vector<std::vector<float>> yvec = readCSV(yvecPath, size(lightFrameArray), maxStars);
        std::vector<std::vector<float>> yvecAlign = readCSV(yvecAlignPath, size(lightFrameArrayAlign), maxStars);

        std::vector<std::vector<float>> qualVec = readCSV(qualVecPath, size(lightFrameArray), 5);
        std::vector<std::vector<float>> qualVecAlign = readCSV(qualVecAlignPath, size(lightFrameArrayAlign), 5);

        bool sizesEqual = true;

        int l = 0;

        while (sizesEqual == true && l<qualVec.size())
        {
            if (qualVec[l][2] != qualVec[0][2])
            {
                sizesEqual = false;
            }

            if (qualVec[l][3] != qualVec[0][3])
            {
                sizesEqual = false;
            }
            l++;
        }

        if (sizesEqual)
        {
            std::vector xRef = clean(xvecAlign[argmax(qualVecAlign, 0)]);
            std::vector yRef = clean(yvecAlign[argmax(qualVecAlign, 0)]);

            if (!xRef.empty() && xRef.size() >= topMatches)
            {
                std::vector<std::vector<float>> refTriangles = triangles(xRef, yRef);

                std::vector<float> rankedQualVec(qualVec.size());

                for (int i = 0; i < qualVec.size(); i++) {
                    rankedQualVec[i] = qualVec[i][0];
                }

                std::sort(rankedQualVec.begin(), rankedQualVec.end());

                float qualityThreshold = rankedQualVec[floor(rankedQualVec.size() * discardPercentage / 100)];

                std::vector<std::vector<int>> q;
                for (int i = 0; i < qualVec.size(); i++) {
                    if (qualVec[i][0] > qualityThreshold) {
                        std::vector<int> f = {0, 0};
                        f[0] = i;
                        f[1] = qualVec[i][0];
                        q.push_back(f);
                    }
                }

                sortIntByColumn(q, 1);

                std::vector<int> e(q.size(),0);

                for (int i = 0; i < q.size(); i++) {
                    e[i] = q[i][0];
                }

                std::vector<std::vector<float>> offsets(e.size(), std::vector<float>(7));
                std::vector<std::string> stackArray(size(e));

                for (int k = 0; k < size(e); k++) {
                    if (!clean(xvec[e[k]]).empty() && clean(xvec[e[k]]).size() >= topMatches)
                    {
                        std::vector<std::vector<float>> frameTriangles = triangles(clean(xvec[e[k]]), clean(yvec[e[k]]));
                        std::vector<std::vector<float>> correctedVoteMatrix = getCorrectedVoteMatrix(refTriangles, frameTriangles, clean(xvecAlign[argmax(qualVecAlign, 0)]), clean(yvec[argmax(qualVec, 0)]));
                        std::tuple<float, float, float> tuple = alignFrames(correctedVoteMatrix, clean(xvecAlign[argmax(qualVecAlign, 0)]), clean(yvecAlign[argmax(qualVecAlign, 0)]), clean(xvec[e[k]]), clean(yvec[e[k]]), topMatches);                      
                        offsets[k][0] = float(qualVec[e[k]][0]);
                        offsets[k][1] = float(qualVec[e[k]][1]);
                        offsets[k][2] = float(qualVec[e[k]][2]);
                        offsets[k][3] = float(qualVec[e[k]][3]);
                        offsets[k][4] = std::get<0>(tuple);
                        offsets[k][5] = std::get<1>(tuple);
                        offsets[k][6] = std::get<2>(tuple);
                        stackArray[k] = lightFrameArray[e[k]];
                    }
                }

                auto t2 = std::chrono::high_resolution_clock::now();
                auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
                elapsedTime = ms_int.count();

                writeCSV(path + parameterDir + "offsets" + filter + ".csv", offsets);
                writeStrings(path + parameterDir + "stackArray" + filter + ".csv", stackArray);

                std::vector<float> xDeb(maxStars);
                std::vector<float> yDeb(maxStars);

                int scaling = 4;

                cv::Mat maxQualFrame = cv::imread(lightFrameArrayAlign[argmax(qualVecAlign, 0)], cv::IMREAD_GRAYSCALE);
                cv::Mat small;

                cv::resize(maxQualFrame, small, cv::Size(maxQualFrame.cols / scaling, maxQualFrame.rows / scaling), 0, 0, cv::INTER_CUBIC);
                cv::Mat img_rgb(small.size(), CV_8UC3);
                cv::cvtColor(small, img_rgb, cv::COLOR_GRAY2BGR);

                for (int i = 0; i < xRef.size(); i++) {
                    cv::circle(img_rgb, cv::Point_(xRef[i] / scaling, yRef[i] / scaling), 8, cv::Scalar(0, 0, 255));
                }

                for (int i = 0; i < offsets.size(); i++) {
                    float R[2][2] = { {cos(offsets[i][4]), -sin(offsets[i][4])}, {sin(offsets[i][4]), cos(offsets[i][4])} };
                    float t[2] = { offsets[i][5], offsets[i][6] };

                    for (int j = 0; j < xvec[e[i]].size(); j++) {
                        xDeb[j] = R[0][0] * xvec[e[i]][j] + R[0][1] * yvec[e[i]][j] + t[0];
                        yDeb[j] = R[1][0] * xvec[e[i]][j] + R[1][1] * yvec[e[i]][j] + t[1];
                    }
                    xDeb = clean(xDeb);
                    yDeb = clean(yDeb);
                    for (int i = 0; i < xDeb.size(); i++) {
                        cv::circle(img_rgb, cv::Point_(xDeb[i] / scaling, yDeb[i] / scaling), 6, cv::Scalar(0, 255, 0));
                    }
                }
                cv::imshow("Debug", img_rgb);
                cv::waitKey(0);
                cv::destroyAllWindows();
            }
        }
    }

    return elapsedTime;
}

//Function for stacking the images
int Hydra::Form1::Stack() {
    int elapsedTime = 0;
    int scaling = 4;

    std::string stackArrayPath =  path + parameterDir + "stackArray" +  filter + ".csv";
    std::string offsetsPath =  path + parameterDir + "offsets" +  filter + ".csv";

    bool filesExist = (std::filesystem::exists(stackArrayPath) && std::filesystem::exists(offsetsPath));

    if (filesExist)
    {
        auto t1 = std::chrono::high_resolution_clock::now();
      
        std::vector<std::string> stackArray = readStrings(stackArrayPath);
        std::vector<std::vector<float>> offsets = readCSV(offsetsPath, size(stackArray), 7);

        std::vector<float> th(offsets.size());
        std::vector<float> dx(offsets.size());
        std::vector<float> dy(offsets.size());
        std::vector<float> background(offsets.size());

        float mean_background = 0;

        for (int i = 0; i < offsets.size(); i++) {
            th[i] = offsets[i][4];
            dx[i] = offsets[i][5];
            dy[i] = offsets[i][6];
            background[i] = offsets[i][1];
            mean_background = mean_background + background[i]/float(offsets.size());
        }

        int xSize = offsets[0][2];
        int ySize = offsets[0][3];

        cv::Mat meanFrame(ySize, xSize, CV_32FC1, cv::Scalar(0));
        cv::Mat medianFrame(ySize, xSize, CV_32FC1, cv::Scalar(0));
        cv::Mat stackFrame(ySize, xSize, CV_32FC1, cv::Scalar(0));
        cv::Mat tempFrame(ySize, xSize, CV_32FC1, cv::Scalar(0));
        std::vector<cv::Mat> tempArray(medianBatchSize, cv::Mat(ySize, xSize, CV_32FC1));

        cv::Mat masterDarkFrame = getCalibrationFrame(ySize, xSize, path + darkDir + darkGroup, 0);
        cv::Mat calibratedFlatFrame = getCalibrationFrame(ySize, xSize, path + flatDir + filter, 1) - getCalibrationFrame(ySize, xSize, path + flatDarksDir + flatDarksGroup, 0);

        double minVal,maxVal;
        cv::minMaxLoc(calibratedFlatFrame, &minVal, &maxVal);

        if (minVal>0)
        {
            calibratedFlatFrame *= xSize * ySize / cv::sum(calibratedFlatFrame)[0];
            if (offsets.size() < medianBatchSize)
            {
                medianBatchSize = offsets.size();
            }

            int batches = (offsets.size() / medianBatchSize);
            int iterations = medianBatchSize * batches;

            std::vector<int> m(iterations);

            for (int j = 0; j < iterations; j++)
            {
                m[j] = j;
            }

            unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
            shuffle(m.begin(), m.end(), std::default_random_engine(seed));

            for (int k = 0; k < batches; k++) {
                #pragma omp parallel for num_threads(8)
                for (int tempcount = 0; tempcount < medianBatchSize; tempcount++) {
                    int i = m[k * medianBatchSize + tempcount];
                    cv::Mat lightFrame = cv::imread(stackArray[i], cv::IMREAD_GRAYSCALE);
                    lightFrame.convertTo(lightFrame, CV_32FC1, 1.0 / pow(255, lightFrame.elemSize()));
                    lightFrame = (lightFrame - masterDarkFrame) / calibratedFlatFrame;
                    lightFrame *= mean_background / background[i];
                    cv::Mat M = (cv::Mat_<float>(2, 3) << cos(th[i]), -sin(th[i]), dx[i], sin(th[i]), cos(th[i]), dy[i]);
                    warpAffine(lightFrame, lightFrame, M, lightFrame.size(), interpolationFlag);
                    tempArray[tempcount] = lightFrame;
                }

                int rows = medianFrame.rows;
                int cols = medianFrame.cols;

                medianFrame.reshape(cols * rows);
                tempFrame.reshape(cols * rows);

                #pragma omp parallel for num_threads(8) 
                for (int h = 0; h < cols * rows; h++)
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
                medianFrame.reshape(cols, rows);
                tempFrame.reshape(cols, rows);

                addWeighted(medianFrame, 1, tempFrame, 1 / float(offsets.size() / medianBatchSize), 0.0, medianFrame);
            }

            if (!std::filesystem::exists(path + outDir))
            {
                std::filesystem::create_directory(path + outDir);
            }

            imwrite(path + outDir + "outMedian" + filter + ".tif", medianFrame);

            #pragma omp parallel for num_threads(8) 
            for (int k = 0; k < offsets.size(); k++) {
                cv::Mat lightFrame = cv::imread(stackArray[k], cv::IMREAD_GRAYSCALE);
                lightFrame.convertTo(lightFrame, CV_32FC1, 1.0 / pow(255, lightFrame.elemSize()));
                lightFrame = (lightFrame - masterDarkFrame) / calibratedFlatFrame;
                lightFrame *= mean_background / background[k];
                cv::Mat M = (cv::Mat_<float>(2, 3) << cos(th[k]), -sin(th[k]), dx[k], sin(th[k]), cos(th[k]), dy[k]);
                warpAffine(lightFrame, lightFrame, M, lightFrame.size(), interpolationFlag);
                addWeighted(meanFrame, 1, lightFrame, 1 / float(offsets.size()), 0.0, meanFrame);

                int rows = lightFrame.rows;
                int cols = lightFrame.cols;

                lightFrame.reshape(cols*rows);
                for (int h = 0; h < cols * rows; h++)
                {
                    float mf = medianFrame.at<float>(h);
                    float lf = lightFrame.at<float>(h);

                    if (abs(lf - mf) > (0.5 * sqrt(mf)))
                    {
                        lightFrame.at<float>(h) = mf;
                    }
                }
                lightFrame.reshape(cols, rows);

                addWeighted(stackFrame, 1, lightFrame, 1 / float(offsets.size()), 0.0, stackFrame);
            }

            imwrite(path + outDir + "outMean" + filter + ".tif", meanFrame);
            imwrite(path + outDir + "outStack" + filter + ".tif", stackFrame);

            auto t2 = std::chrono::high_resolution_clock::now();
            auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
            elapsedTime = ms_int.count();

            cv::Mat small;
            cv::resize(stackFrame, small, cv::Size(stackFrame.cols / scaling, stackFrame.rows / scaling), 0, 0, cv::INTER_CUBIC);

            cv::imshow("Stack", small * 5);
            cv::waitKey(0);
            cv::destroyAllWindows();
        }
    }       
    
    return elapsedTime;
}
