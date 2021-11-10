#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <vector>

// g++ `pkg-config --cflags opencv4` cv3.cpp `pkg-config --libs opencv4`

using namespace std;
using namespace cv;

int main(int argc, char **argv)
{
	int thr1 = 100;
	int thr2 = 100;
	VideoCapture cap1(0);
	if (!cap1.isOpened())
		return -1;

	namedWindow("pierwsze", WINDOW_AUTOSIZE);
	createTrackbar("threshold1", "pierwsze", &thr1, 1000);
	createTrackbar("threshold2", "pierwsze", &thr2, 1000);

	while (true)
	{
		Mat f1;
		//cap1 >> f1;
		cap1.read(f1);
		Mat dst;
		Mat edges;
		cvtColor(f1, dst, COLOR_BGR2GRAY);

		imshow("zrodlo", f1);
		imshow("czarnobialy", dst);

		Mat k = getStructuringElement(MORPH_ELLIPSE, {9,9});
		Mat diledges;
		dilate(dst, diledges, k);
		imshow("czarnobialy_D", diledges);
		erode(diledges, dst, k);
		imshow("czarnobialy_E", dst);

		Canny(dst, edges, thr1, thr2, 3);
		imshow("krawedzie", edges);
		
		if (waitKey(1) == 27)
			break;
	}
	return 0;
}
