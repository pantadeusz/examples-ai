#include <opencv2/aruco.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

#include <chrono>
#include <list>


// g++ `pkg-config --cflags opencv4` detector.cpp -o detector `pkg-config --libs opencv4`
// https://docs.opencv.org/4.x/d5/dae/tutorial_aruco_detection.html
const double MARKER_MM = 30;

std::vector<cv::Point2f> fromPixelsToMm(std::vector<cv::Point2f> original)
{
    std::vector<cv::Point2f> result;
    double distances = 0;
    for (int i = 0; i < 4; i++) {
        cv::Point2f v = (original[i] - original[(i + 1) % 4]);

        distances += std::sqrt(v.x * v.x + v.y * v.y);
    }
    distances = distances / 4;

    for (int i = 0; i < 4; i++) {
        result.push_back({MARKER_MM * (original[i].x / distances), MARKER_MM * (original[i].y / distances)});
    }
    return result;
}

int main()
{
    using namespace cv;
    using namespace std;

    list<pair<std::chrono::steady_clock::time_point, vector<Point2f>>> detectedPositions;

    VideoCapture cam(0);
    Ptr<aruco::DetectorParameters> parameters = aruco::DetectorParameters::create();
    Ptr<aruco::Dictionary> dictionary = aruco::getPredefinedDictionary(aruco::DICT_6X6_250);
    namedWindow("markers", WINDOW_NORMAL);
    namedWindow("markers_rejected", WINDOW_NORMAL);
    while (waitKey(10) != 27) {
        Mat inputImage;
        Mat detected;
        cam >> inputImage;
        detected = inputImage.clone();
        vector<int> markerIds;
        vector<vector<Point2f>> markerCorners, rejectedCandidates;
        aruco::detectMarkers(inputImage, dictionary, markerCorners,
            markerIds, parameters, rejectedCandidates);
        Mat outputImage = inputImage.clone();
        aruco::drawDetectedMarkers(detected, markerCorners, markerIds);
        auto found = find(markerIds.begin(), markerIds.end(), 0);
        if (found != markerIds.end()) {
            int foundIdx = distance(markerIds.begin(), found);
            cout << foundIdx << endl;
            auto found = markerCorners.at(foundIdx);
            std::cout << found << std::endl;
            auto p = found[0];
            auto r = fromPixelsToMm(found)[0];
            string txt = "[" + to_string(p.x) + "," + to_string(p.y) + "] [" + to_string(r.x) + "," + to_string(r.y) + "]";
               putText(detected, txt.c_str(), p,
            FONT_HERSHEY_PLAIN, 0.5, {0, 255, 0});
        }
        imshow("markers", detected);
    }

    return 0;
}
