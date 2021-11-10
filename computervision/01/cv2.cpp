#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <vector>

// g++ `pkg-config --cflags opencv4` cv2.cpp `pkg-config --libs opencv4`

using namespace std;
using namespace cv;

int main(int argc, char **argv)
{
	int trackval = 0;
	VideoCapture cap1(0);
	if (!cap1.isOpened())
		return -1;

	namedWindow("pierwsze", WINDOW_AUTOSIZE);
	createTrackbar("parametr", "pierwsze", &trackval, 100);
	while (true)
	{
		Mat f1;
		//cap1 >> f1;
		cap1.read(f1);
		Mat dst;
		Mat kernel(3, 3, CV_32F);
		kernel.at<float>(0,0) = 0;
		kernel.at<float>(1,0) = -1;
		kernel.at<float>(2,0) = 0;
		kernel.at<float>(0,1) = -1;
		kernel.at<float>(1,1) = 5;
		kernel.at<float>(2,1) = -1;
		kernel.at<float>(0,2) = 0;
		kernel.at<float>(1,2) = -1;
		kernel.at<float>(2,2) = 0;
		
		filter2D(f1, dst, -1, kernel);
		
		imshow("pierwsze", f1);
		imshow("wynik", dst);
		if (waitKey(1) == 27)
			break;
	}
	return 0;
}
