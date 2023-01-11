#include <opencv2/aruco.hpp>
#include <opencv2/opencv.hpp>

// g++ `pkg-config --cflags opencv4` markers.cpp -o markers `pkg-config --libs opencv4`
// https://docs.opencv.org/4.x/d5/dae/tutorial_aruco_detection.html

const int COLS = 4;
const int ROWS = 6;
const int SIDE = 100;
const int BORDER = 50;

int main()
{
    using namespace cv;
    Ptr<aruco::Dictionary> dictionary = aruco::getPredefinedDictionary(aruco::DICT_7X7_250);
    Mat1f ret_image(ROWS * (SIDE + BORDER), COLS * (SIDE + BORDER), 255.0);
    for (int x = 0; x < COLS; x++) {
        for (int y = 0; y < ROWS; y++) {
            int i = y * COLS + x;
            Mat markerImage;
            aruco::drawMarker(dictionary, i, SIDE, markerImage, 1);
            Rect roi(Point(BORDER / 2 + x * (SIDE + BORDER), BORDER / 2 + y * (SIDE + BORDER)), markerImage.size());
            markerImage.copyTo(ret_image(roi));
        }
    }
    imwrite("markers_7_7.png", ret_image);
}
