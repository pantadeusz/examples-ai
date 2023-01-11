#include <opencv2/aruco.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <tuple>
#include <chrono>
#include <list>


// g++ `pkg-config --cflags opencv4` detector.cpp -o detector `pkg-config --libs opencv4`
// https://docs.opencv.org/4.x/d5/dae/tutorial_aruco_detection.html

std::pair<cv::Mat, cv::Mat> get_camera_properties_ps3eye()
{
    // Camera matrix
    cv::Mat camera_matrix = (cv::Mat_<double>(3, 3) << 600, 0, 320,
        0, 600, 240,
        0, 0, 1);

    // Distortion coefficients
    cv::Mat dist_coeffs = (cv::Mat_<double>(5, 1) << 0, 0, 0, 0, 0);

    return {camera_matrix, dist_coeffs};
}

cv::Mat get_object_points()
{
    const double MARKER_MM = 30;
    cv::Mat obj_points(4, 1, CV_32FC3);
    obj_points.ptr<cv::Vec3f>(0)[0] = cv::Vec3f(-MARKER_MM / 2.f, MARKER_MM / 2.f, 0);
    obj_points.ptr<cv::Vec3f>(0)[1] = cv::Vec3f(MARKER_MM / 2.f, MARKER_MM / 2.f, 0);
    obj_points.ptr<cv::Vec3f>(0)[2] = cv::Vec3f(MARKER_MM / 2.f, -MARKER_MM / 2.f, 0);
    obj_points.ptr<cv::Vec3f>(0)[3] = cv::Vec3f(-MARKER_MM / 2.f, -MARKER_MM / 2.f, 0);
    return obj_points;
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

    auto [camera_matrix, dist_coeffs] = get_camera_properties_ps3eye();
    auto obj_points = get_object_points();


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

        auto found = find(markerIds.begin(), markerIds.end(), 7);
        if (found != markerIds.end()) {
            int foundIdx = distance(markerIds.begin(), found);
            
            cv::Vec3d rvecs, tvecs;
            solvePnP(obj_points, markerCorners.at(foundIdx), camera_matrix, dist_coeffs, rvecs, tvecs);
            cv::drawFrameAxes(detected, camera_matrix, dist_coeffs, rvecs, tvecs, 10.1);

            auto found = markerCorners.at(foundIdx);
            //std::cout << found << std::endl;
            auto p = found[0];
            string txt = "[" + to_string(p.x) + "," + to_string(p.y) + "]";
            putText(detected, txt.c_str(), p,
                FONT_HERSHEY_PLAIN, 0.5, {0, 255, 0});
        }
        imshow("markers", detected);
    }

    return 0;
}
