#include <opencv2/aruco.hpp>
#include <opencv2/opencv.hpp>

// g++ `pkg-config --cflags opencv4` markers.cpp -o markers `pkg-config --libs opencv4`
// https://docs.opencv.org/4.x/d5/dae/tutorial_aruco_detection.html

const int COLS = 8;
const int ROWS = 10;
const int SIDE = 100;
const int BORDER = 50;

int main()
{
    cv::Mat1f ret_image(ROWS * (SIDE + BORDER), COLS * (SIDE + BORDER), 255.0);
    for (int x = 0; x < COLS; x++) {
        for (int y = 0; y < ROWS; y++) {
            int i = y * COLS + x;
            cv::Mat markerImage;
            cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
            cv::aruco::drawMarker(dictionary, i, SIDE, markerImage, 1);
            cv::Rect roi(cv::Point(BORDER / 2 + x * (SIDE + BORDER), BORDER / 2 + y * (SIDE + BORDER)), markerImage.size());
            markerImage.copyTo(ret_image(roi));
        }
    }
    cv::imwrite("markers.png", ret_image);
}
