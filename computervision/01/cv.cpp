#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <vector>

// g++ `pkg-config --cflags opencv4` cv.cpp `pkg-config --libs opencv4`

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
		Vec3b bgr = f1.at<Vec3b>(f1.rows/2,f1.cols/2);
		cout << setfill('0') << setw(2) << right << hex <<
		 (int)bgr[0] << " " << (int)bgr[1] << " " << (int)bgr[2] << endl;
		imshow("pierwsze", f1);
		if (waitKey(1) == 27)
			break;
	}
	return 0;
}
