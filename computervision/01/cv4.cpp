#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <vector>

// g++ `pkg-config --cflags opencv4` cv3.cpp `pkg-config --libs opencv4`

using namespace std;
using namespace cv;

int main(int argc, char** argv)
{
    int h1 = 0;
    int h2 = 255;
    int s1 = 0;
    int s2 = 255;
    int v1 = 0;
    int v2 = 255;


    VideoCapture cap1(0);
    if (!cap1.isOpened())
        return -1;

    namedWindow("pierwsze", WINDOW_AUTOSIZE);
    createTrackbar("h1", "pierwsze", &h1, 255);
    createTrackbar("h2", "pierwsze", &h2, 255);
    createTrackbar("s1", "pierwsze", &s1, 255);
    createTrackbar("s2", "pierwsze", &s2, 255);
    createTrackbar("v1", "pierwsze", &v1, 255);
    createTrackbar("v2", "pierwsze", &v2, 255);

    while (true) {
        Mat f1;
        // cap1 >> f1;
        cap1.read(f1);
        Mat dst;
        Mat edges;
        Mat range;
        cvtColor(f1, dst, COLOR_BGR2HSV);

        imshow("zrodlo", f1);
        imshow("czarnobialy", dst);
        inRange(dst, Scalar{h1, s1, v1}, Scalar{h2, s2, v2}, range);

        Mat k = getStructuringElement(MORPH_ELLIPSE, {9, 9});
        Mat diledges;
		morphologyEx(range,range,MORPH_CLOSE,k);
        imshow("inrange", range);

        if (waitKey(1) == 27)
            break;
    }
    return 0;
}
