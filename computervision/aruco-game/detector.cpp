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
    // coefficients for approximated PS3 Eye camera
    //! Camera matrix
    cv::Mat camera_matrix = (cv::Mat_<double>(3, 3) << 600, 0, 320,
        0, 600, 240,
        0, 0, 1);
    //! Distortion coefficients
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

    Mat obstacle_img = imread("obstacle.png");
    Mat car_img = imread("car0.png");
    double car_pos = 160.0; 


    list<pair<std::chrono::steady_clock::time_point, vector<Point2f>>> detectedPositions;

    VideoCapture cam(0);
    Ptr<aruco::DetectorParameters> parameters = aruco::DetectorParameters::create();
    Ptr<aruco::Dictionary> dictionary = aruco::getPredefinedDictionary(aruco::DICT_6X6_250);
    namedWindow("markers", WINDOW_NORMAL);

    auto [camera_matrix, dist_coeffs] = get_camera_properties_ps3eye();
    auto obj_points = get_object_points();


    while (waitKey(10) != 27) {
        Mat inputImage;
        Mat detected;
        cam >> inputImage;
        //resize(inputImage, inputImage, Size(320, 240), 0, 0, INTER_CUBIC);
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

            Mat rot_mat;
            Rodrigues(rvecs, rot_mat);
            double angle_z = atan2(rot_mat.at<double>(1,0), rot_mat.at<double>(0,0));
            double angle_x = atan2(rot_mat.at<double>(2,1), rot_mat.at<double>(2,2));
            angle_x = -angle_x+((angle_x < 0)?(-M_PI):M_PI);
            double angle_y = -asin(rot_mat.at<double>(2,0));

            std::cout << angle_x << " " << angle_y << " " << angle_z << std::endl;
            auto found = markerCorners.at(foundIdx);
            double direction_x = angle_z;

            
            if (std::abs(direction_x) > 0.1) car_pos += 5*direction_x;
            if (car_pos <= (car_img.cols/2)) car_pos = (car_img.cols/2);
            if (car_pos >= (320-(car_img.cols/2))) car_pos = (320-(car_img.cols/2))-1;
        }
        Mat insetImage(detected, Rect(car_pos-(car_img.cols/2), 160, car_img.rows, car_img.cols));
        car_img.copyTo(insetImage);
        imshow("markers", detected);
    }

    return 0;
}
