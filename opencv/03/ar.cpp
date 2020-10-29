#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>

// g++ `pkg-config --cflags opencv4` ar.cpp `pkg-config --libs opencv4`

using namespace std;
using namespace cv;

double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0)
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1 * dx2 + dy1 * dy2) / sqrt((dx1 * dx1 + dy1 * dy1) * (dx2 * dx2 + dy2 * dy2) + 1e-10);
}

void drawPictureOnPict(const Mat srcMat, const Mat imgToPut, Mat &resultMat, const vector<Point> contour)
{
	Mat dstMat(Size(srcMat.cols, srcMat.rows), CV_8UC3);
	Mat cutter(Size(imgToPut.cols, imgToPut.rows), imgToPut.type());

	cutter = Scalar(0);
	bitwise_not(cutter, cutter);
	vector<Point2f> src = {{0, 0}, {dstMat.cols, 0}, {dstMat.cols, dstMat.rows}, {0, dstMat.rows}};
	vector<Point2f> dst;
	for (auto p : contour)
		dst.push_back(Point2f(p.x, p.y));
	auto wrap_mtx = getPerspectiveTransform(src, dst);
	warpPerspective(imgToPut, dstMat, wrap_mtx, Size(dstMat.cols, dstMat.rows));

	warpPerspective(cutter, cutter, wrap_mtx, Size(dstMat.cols, dstMat.rows));
	bitwise_not(cutter, cutter);
	Mat dstMatClone;
	srcMat.copyTo(dstMatClone, cutter);

	dstMat = dstMat + dstMatClone;
	resultMat = dstMat;
}

int main()
{
	int canny_a = 30, canny_b = 86;
	Mat imgtoshow;
	imgtoshow = imread("089.jpg", IMREAD_COLOR);
	namedWindow("prostokaty", cv::WINDOW_AUTOSIZE);
	createTrackbar("A", "prostokaty", &canny_a, 255);
	createTrackbar("B", "prostokaty", &canny_b, 255);
	VideoCapture camera(0); // 0 - domyslna, 1 - podczerwien,
	while (waitKey(1) != 27)
	{

		Mat frame, frame0, frameBw, frameCanny;
		camera >> frame;
		flip(frame, frame, 1);
		frame0 = frame.clone();
		imshow("frame_from_cam", frame);
		cvtColor(frame, frameBw, COLOR_BGR2GRAY);
		imshow("bw", frameBw);
		equalizeHist(frameBw, frameBw);
		imshow("bw_hist", frameBw);

		Canny(frameBw, frameCanny, canny_a, canny_b, 3);
		imshow("bw_Canny", frameCanny);
		static auto ellipse = getStructuringElement(MORPH_ELLIPSE, Size(5, 5));
		morphologyEx(frameBw, frameBw, MORPH_CLOSE, ellipse);
		morphologyEx(frameBw, frameBw, MORPH_OPEN, ellipse);

		Canny(frameBw, frameCanny, canny_a, canny_b, 3);
		imshow("bw_Canny_2", frameCanny);
		static auto ellipse_33 = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
		morphologyEx(frameCanny, frameCanny, MORPH_DILATE, ellipse_33);

		imshow("bw_Canny_2_dil", frameCanny);
		vector<vector<Point>> contours;
		vector<vector<Point>> contours_4;
		findContours(frameCanny, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
		for (int i = 0; i < contours.size(); i++)
		{
			approxPolyDP(contours[i], contours[i], 10, true);
			if (contours[i].size() == 4)
			{
				double amax = 0;
				for (int j = 0; j < 4; j++)
				{
					double a = fabs(angle(contours[i][j], contours[i][(j + 1) % 4], contours[i][(j + 1) % 4]));
					amax = max(a, amax);
				}
				if (amax < 0.4)
					contours_4.push_back(contours[i]);
			}
		}

		for (int i = 0; i < contours_4.size(); i++)
		{
			drawContours(frame, contours_4, i, Scalar(0, 0, 255));
		}
		if (contours_4.size() > 0)
		{
			sort(contours_4.begin(), contours_4.end(),
				 [](auto &a, auto &b) {
					 return contourArea(a, false) > contourArea(b, false);
				 });
			drawContours(frame, contours_4, 0, Scalar(255, 255, 255));

			Mat resultMat;
			drawPictureOnPict(frame0, imgtoshow, resultMat, contours_4[0]);
			imshow("RESULT", resultMat);
		}
		imshow("frame_from_cam", frame);
	}
	return 0;
}
