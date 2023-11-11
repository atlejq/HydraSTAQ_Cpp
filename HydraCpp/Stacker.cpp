#include "pch.h"
#include "Stacker.h"

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
        int rmax = 0;
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

void SortByColumn(std::vector<std::vector<float>>& data, size_t column) {
    std::sort(data.begin(), data.end(), [column](std::vector<float> const& v1, std::vector<float> const& v2)
        {
            return v1[column] > v2[column];
        });
}

void SortByColumnI(std::vector<std::vector<int>>& data, size_t column) {
    std::sort(data.begin(), data.end(), [column](std::vector<int> const& v1, std::vector<int> const& v2)
        {
            return v1[column] > v2[column];
        });
}

std::vector<std::vector<float>> triangles(std::vector<float> x, std::vector<float> y) {
    std::vector<std::vector<float>> triangleParameters;
    float minEdge = 50;
    int count = 0;
    for (int i = 0; i < x.size() - 2; i++) {
        for (int j = i + 1; j < x.size() - 1; j++) {
            for (int k = j + 1; k < x.size(); k++) {
                std::vector<double> d = { sqrt(pow(x[i] - x[j], 2) + pow(y[i] - y[j], 2)), sqrt(pow(x[j] - x[k], 2) + pow(y[j] - y[k], 2)), sqrt(pow(x[i] - x[k], 2) + pow(y[i] - y[k], 2)) };
                if (*std::min_element(d.begin(), d.end()) > minEdge) {
                    std::sort(d.begin(), d.end());
                    int m = d.size() / 2;
                    float u = ((d[m] + d[d.size() - m - 1]) / 2) / *std::max_element(d.begin(), d.end());
                    float v = *std::min_element(d.begin(), d.end()) / *std::max_element(d.begin(), d.end());
                    triangleParameters.push_back({ float(i), float(j), float(k), u, v });
                    count++;
                }
            }
        }
    }
    return triangleParameters;
}

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
    return std::make_tuple(t[0], t[1], theta);
}

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

std::tuple<float, float, float, int> alignFrames(std::vector<std::vector<float>> corrVote, std::vector<float> refVectorX, std::vector<float> refVectorY, std::vector<float> xvec, std::vector<float> yvec, int topMatches) {
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

    SortByColumnI(votePairs, 2);

    std::vector<std::vector<int>> rankPairs(votePairs.begin(), votePairs.end());

    Eigen::MatrixXf referenceM(2, topMatches);
    Eigen::MatrixXf frameM(2, topMatches);

    if (rankPairs.size() >= topMatches) {
        for (int i = 0; i < topMatches; i++) {
            referenceM(0, i) = refVectorX[rankPairs[i][1]];
            referenceM(1, i) = refVectorY[rankPairs[i][1]];
            frameM(0, i) = xvec[rankPairs[i][0]];
            frameM(1, i) = yvec[rankPairs[i][0]];
        }
    }

    float theta;
    float t1;
    float t2;
    int d;

    if (rankPairs.size() >= topMatches) {
        std::tuple<float, float, float> tuple = findRT(frameM, referenceM);
        t1 = std::get<0>(tuple);
        t2 = std::get<1>(tuple);
        theta = std::get<2>(tuple);
        d = 0;
    }
    else {
        theta = 0;
        t1 = 0;
        t2 = 0;
        d = 1;
    }

    return std::make_tuple(t1, t2, theta, d);
}

//Function to get all the file names in the given directory.
std::vector<std::string> getFrames(std::string path, std::string ext) {
    std::vector<std::string> filenames;

    for (auto& p : std::filesystem::recursive_directory_iterator(path))
    {
        if (p.path().extension() == ext)
            filenames.push_back(p.path().string());
    }

    return filenames;
}

// Function to analyze the star field in the given light frame.
std::vector<std::vector<float>> analyzeStarField(cv::Mat lightFrame, float t) {
    cv::Mat filteredImage;
    cv::medianBlur(lightFrame, filteredImage, 3);

    cv::Mat thresh;
    cv::threshold(filteredImage, thresh, t * 255 * lightFrame.elemSize(), 255 * lightFrame.elemSize(), 0);

    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(thresh, contours, hierarchy, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

    std::vector<std::vector<float>> starMatrix;
    for (const auto& c : contours) {
        cv::RotatedRect rect = cv::minAreaRect(c);
        cv::Point2f center = rect.center;
        float height = rect.size.height;
        float width = rect.size.width;
        std::vector<float> starVector = { center.x, center.y, width, height, std::sqrt(width * width + height * height) };
        if (std::sqrt(width * width + height * height) < 25)
            starMatrix.push_back(starVector);
    }

    return starMatrix;
}

//Function to read images
int Stacker::ReadImages() {
    std::vector<std::string> lightFrames = getFrames(config.path + config.lightDir + config.filter, config.ext);

    auto t1 = std::chrono::high_resolution_clock::now();

    std::vector<std::vector<float>> qualVec(lightFrames.size(), std::vector<float>(2));
    std::vector<std::vector<float>> xvec(lightFrames.size(), std::vector<float>(config.maxStars));
    std::vector<std::vector<float>> yvec(lightFrames.size(), std::vector<float>(config.maxStars));

#pragma omp parallel for num_threads(8)
    for (int n = 0; n < lightFrames.size(); n++) {
        cv::Mat lightFrame = cv::imread(lightFrames[n], cv::IMREAD_GRAYSCALE);
        if (lightFrame.data != NULL) {
            std::vector<std::vector<float>> starMatrix = analyzeStarField(lightFrame, config.detectionThreshold);

            qualVec[n][0] = starMatrix.size();
            qualVec[n][1] = cv::sum(lightFrame)[0];

            for (int i = 0; i < config.maxStars; i++) {
                xvec[n][i] = -1;
                yvec[n][i] = -1;
            }

            int max = config.maxStars;
            if (starMatrix.size() < config.maxStars) {
                max = starMatrix.size();
            }

            if (starMatrix.size() > 3) {

                SortByColumn(starMatrix, 4);

                for (int i = 0; i < max; i++) {
                    xvec[n][i] = starMatrix[i][0];
                    yvec[n][i] = starMatrix[i][1];
                }
            }
        }
    }

    writeStrings(config.path + config.parameterDir + "lightFrameArray" + config.filter + ".csv", lightFrames);
    writeCSV(config.path + config.parameterDir + "qualVec" + config.filter + ".csv", qualVec);
    writeCSV(config.path + config.parameterDir + "xvec" + config.filter + ".csv", xvec);
    writeCSV(config.path + config.parameterDir + "yvec" + config.filter + ".csv", yvec);

    auto t2 = std::chrono::high_resolution_clock::now();
    auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);

    return ms_int.count();
}

int Stacker::ComputeOffsets() {
    auto t1 = std::chrono::high_resolution_clock::now();

    std::string lightFrameArrayPath = config.path + config.parameterDir + "lightFrameArray" + config.filter + ".csv";
    std::string xvecPath = config.path + config.parameterDir + "xvec" + config.filter + ".csv";
    std::string yvecPath = config.path + config.parameterDir + "yvec" + config.filter + ".csv";
    std::string qualVecPath = config.path + config.parameterDir + "qualVec" + config.filter + ".csv";
    std::string xvecAlignPath = config.path + config.parameterDir + "xvec" + config.align + ".csv";
    std::string yvecAlignPath = config.path + config.parameterDir + "yvec" + config.align + ".csv";
    std::string qualVecAlignPath = config.path + config.parameterDir + "qualVec" + config.align + ".csv";

    bool filesExist = (std::filesystem::exists(lightFrameArrayPath) && std::filesystem::exists(xvecPath) && std::filesystem::exists(yvecPath) &&
                       std::filesystem::exists(qualVecPath) && std::filesystem::exists(xvecAlignPath) && std::filesystem::exists(yvecAlignPath) &&
                       std::filesystem::exists(qualVecAlignPath));

    if (filesExist)
    {
        std::vector<std::string> lightFrameArray = readStrings(lightFrameArrayPath);
        std::vector<std::vector<float>> xvec = readCSV(xvecPath, lightFrameArray.size(), config.maxStars);
        std::vector<std::vector<float>> yvec = readCSV(yvecPath, lightFrameArray.size(), config.maxStars);
        std::vector<std::vector<float>> qualVec = readCSV(qualVecPath, lightFrameArray.size(), 2);
        std::vector<std::vector<float>> xvecAlign = readCSV(xvecAlignPath, lightFrameArray.size(), config.maxStars);
        std::vector<std::vector<float>> yvecAlign = readCSV(yvecAlignPath, lightFrameArray.size(), config.maxStars);
        std::vector<std::vector<float>> qualVecAlign = readCSV(qualVecAlignPath, lightFrameArray.size(), 2);

        std::vector<std::vector<float>> refTriangles = triangles(clean(xvec[argmax(qualVec, 0)]), clean(yvec[argmax(qualVec, 0)]));

        std::vector<std::vector<float>> offsets(size(xvec), std::vector<float>(4));

        for (int k = 0; k < size(xvec); k++) {
            if (!clean(xvec[k]).empty())
            {
                std::vector<std::vector<float>> frameTriangles = triangles(clean(xvec[k]), clean(yvec[k]));
                std::vector<std::vector<float>> correctedVoteMatrix = getCorrectedVoteMatrix(refTriangles, frameTriangles, clean(xvec[argmax(qualVec, 0)]), clean(yvec[argmax(qualVec, 0)]));
                std::tuple<float, float, float, int> tuple = alignFrames(correctedVoteMatrix, clean(xvecAlign[argmax(qualVec, 0)]), clean(yvecAlign[argmax(qualVec, 0)]), clean(xvec[k]), clean(yvec[k]), config.topMatches);
                offsets[k][0] = std::get<0>(tuple);
                offsets[k][1] = std::get<1>(tuple);
                offsets[k][2] = std::get<2>(tuple);
            }
            offsets[k][3] = k;
        }
        writeCSV(config.path + config.parameterDir + "offsets" + config.filter + ".csv", offsets);
    }

    auto t2 = std::chrono::high_resolution_clock::now();
    auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);

    return ms_int.count();
}

int Stacker::Stack() {
    auto t1 = std::chrono::high_resolution_clock::now();
    std::string lightFrameArrayPath = config.path + config.parameterDir + "lightFrameArray" + config.filter + ".csv";
    std::string offsetsPath = config.path + config.parameterDir + "offsets" + config.filter + ".csv";
    std::string qualVecPath = config.path + config.parameterDir + "qualVecPath" + config.filter + ".csv";

    bool filesExist = (std::filesystem::exists(lightFrameArrayPath) && std::filesystem::exists(offsetsPath) && std::filesystem::exists(qualVecPath));

    if (filesExist)
    {
        std::vector<std::string> lightFrameArray = readStrings(lightFrameArrayPath);
        std::vector<std::vector<float>> offsets = readCSV(offsetsPath, lightFrameArray.size(), 4);
        std::vector<std::vector<float>> qualVec = readCSV(qualVecPath, lightFrameArray.size(), 2);
    }

    auto t2 = std::chrono::high_resolution_clock::now();
    auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);

    return ms_int.count();
}
