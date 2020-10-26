#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>

// g++ `pkg-config --cflags opencv4` cv.cpp `pkg-config --libs opencv4`

using namespace std;
using namespace cv;

class MyObject
{
public:
	int maxC;
	vector<Point> pos;
	Point getP()
	{
		Point sump = std::accumulate(pos.begin(), pos.end(), Point(0, 0));
		sump.x /= pos.size();
		sump.y /= pos.size();
		return sump;
	}
	void addP(Point p)
	{
		pos.push_back(p);
		if (pos.size() > maxC)
		{
			pos = vector<Point>(pos.begin() + 1, pos.end());
		}
	}
	void addEmpty()
	{
		if (pos.size() > 0)
		{
			pos = vector<Point>(pos.begin() + 1, pos.end());
		}
	}
};

int main()
{
	int loRange[3] = {56, 88, 76};
	int hiRange[3] = {119, 255, 255};

	namedWindow("jakostam", cv::WINDOW_AUTOSIZE);
	createTrackbar("loRange0", "jakostam", &(loRange[0]), 255);
	createTrackbar("loRange1", "jakostam", &(loRange[1]), 255);
	createTrackbar("loRange2", "jakostam", &(loRange[2]), 255);
	createTrackbar("hiRange0", "jakostam", &(hiRange[0]), 255);
	createTrackbar("hiRange1", "jakostam", &(hiRange[1]), 255);
	createTrackbar("hiRange2", "jakostam", &(hiRange[2]), 255);
	VideoCapture camera(0);
	//VideoCapture backgroundvid("Multiwave.wmv");
	Mat background = imread("plaza.jpg", cv::IMREAD_COLOR);

	MyObject myobj;
	myobj.maxC = 10;
	while (waitKey(1) != 27)
	{
		int dilation_size = 5;
		auto structElem = getStructuringElement(MORPH_ELLIPSE,
												Size(2 * dilation_size + 1, 2 * dilation_size + 1),
												Point(dilation_size, dilation_size));
		Mat frame;
		Mat backgroundScaled;
		Mat frameMask, frameNegMask;
		Mat frameWithMask, backgroundScaledWithMask;
		Mat meinniceplace;

		camera >> frame;
		flip(frame, frame, 1);
		resize(background, backgroundScaled, {frame.cols, frame.rows});

		cvtColor(frame, frameMask, cv::COLOR_BGR2HSV);
		inRange(frameMask, Scalar(loRange[0], loRange[1], loRange[2]),
				Scalar(hiRange[0], hiRange[1], hiRange[2]), frameNegMask);
		morphologyEx(frameNegMask, frameNegMask, MORPH_CLOSE, structElem);
		morphologyEx(frameNegMask, frameNegMask, MORPH_OPEN, structElem);
		imshow("dilate", frameNegMask);

		vector<vector<Point>> contours;
		findContours(frameNegMask, contours, cv::RETR_LIST, cv::CHAIN_APPROX_TC89_KCOS);
		sort(contours.begin(), contours.end(),
			 [](auto &a, auto &b) {
				 return contourArea(a, false) > contourArea(b, false);
			 });

		for (int i = 0; i < contours.size(); i++)
		{
			approxPolyDP(contours.at(i), contours.at(i), 10, true);
			//			drawContours(frame, contours, i, {0, 0, 255, 255});
			//			auto txtpos = contours.at(i).at(0);
			//			putText(frame, to_string(contours.at(i).size()), txtpos, cv::FONT_HERSHEY_PLAIN, 2, {0, 0, 255, 255});
			//			txtpos.y += 30;
			//			putText(frame, to_string(contourArea(contours.at(i), false)), txtpos, cv::FONT_HERSHEY_PLAIN, 2, {0, 0, 255, 255});
			//			txtpos.y -= 60;
			//			putText(frame, to_string(i), txtpos, cv::FONT_HERSHEY_PLAIN, 2, {0, 0, 255, 255});
		}
		if (contours.size())
		{
			Point avg;
			Rect r = boundingRect(contours.at(0));
			avg.x = r.x + r.width / 2;
			avg.y = r.y + r.height / 2;
			myobj.addP(avg);
//			putText(frame, "0", avg, cv::FONT_HERSHEY_PLAIN, 2, {0, 255, 255, 255});
		}
		else
		{
			myobj.addEmpty();
		}
		if (myobj.pos.size() > 1) {
			putText(frame, "X", myobj.getP(), cv::FONT_HERSHEY_PLAIN, 2, {255, 0, 255, 255});
			
			vector<vector<Point>>ctrs = {myobj.pos};
			drawContours(frame, ctrs, 0, {255, 0, 255, 255});	
		}
		imshow("contours", frame);
	}
	return 0;
}
