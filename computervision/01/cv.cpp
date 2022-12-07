#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <vector>

// g++ `pkg-config --cflags opencv4` cv.cpp `pkg-config --libs opencv4`

using namespace std;
using namespace cv;

int main(int argc, char **argv)
{
	VideoCapture cap1(0);
	if (!cap1.isOpened())
		return -1;
 	namedWindow("A", WINDOW_AUTOSIZE);
	while (true)
	{
		Mat f1, f2;
		cap1 >> f1;
		imshow("A", f1);
		if (waitKey(1) == 27)
			break;
	}
	return 0;
}
