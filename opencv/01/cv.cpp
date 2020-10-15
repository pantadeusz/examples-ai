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

	namedWindow("1", WINDOW_AUTOSIZE);
	int range = 0;
	createTrackbar("range", "1", &range, 255);
	while (true)
	{
		Mat f1, small_img, thr,thr_a, small_hsv;
		cap1 >> f1;
		resize(f1,small_img,{320,320});
		putText(f1, to_string(range), {10, 30}, FONT_HERSHEY_PLAIN, 1.0, {0, 255, 0, 255});

		imshow("1", f1);
		cvtColor(small_img,small_hsv,COLOR_BGR2HSV);
		cvtColor(small_img,small_img,COLOR_BGR2GRAY);
		imshow("small", small_img);
		imshow("small_hsv", small_hsv);
		threshold(small_img,thr,128.0,255.0,0);
		adaptiveThreshold(small_img,thr_a,255.0, ADAPTIVE_THRESH_GAUSSIAN_C,0,3,range/255.0);
		imshow("thr_a", thr_a);
		dilate(thr_a,thr_a, getStructuringElement(MORPH_ELLIPSE, {3,3}));
		imshow("thr_a_dil", thr_a);
		
		char k = waitKey(1);
		if (k == 27)
			break;
		if (k == 's')
		{
			auto r = selectROI("small_hsv", small_hsv);
			Mat roi = f1(r);
			imshow("ROI", roi);
		}
	}
	return 0;
}
