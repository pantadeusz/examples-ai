#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <thread>
#include <future>

// g++ `pkg-config --cflags opencv4` cv.cpp `pkg-config --libs opencv4`

using namespace std;
using namespace cv;

pair<string, vector<Point2d>> processMat(Mat frame)
{
	string ret;
	vector<Point2d> pts;
	static QRCodeDetector qrdetector;

	if (qrdetector.detect(frame, pts))
	{
		ret = qrdetector.decode(frame, pts);
		// cout << "found at " << pts << endl;
	}

	return {ret, pts};
}

int main()
{
	VideoCapture camera(0);
	vector<future<pair<string, vector<Point2d>>>> tasks;
	vector<Point2f> points;
	int pointsTTL = 0;

	while (waitKey(1) != 27)
	{

		Mat frame, frameFlipped;
		camera >> frame;
		if (tasks.size() == 0)
		{
			tasks.push_back(async(std::launch::async, processMat, frame));
		}
		else if (tasks.back().valid())
		{
			auto [v, pts0] = tasks.back().get();
			vector<Point2f> pts;
			tasks.pop_back();
			if (pts0.size() == 4)
			{
				for (auto p : pts0)
					pts.push_back(p);
				if (contourArea(pts, false) > 0.0025 * (frame.rows * frame.cols))
				{
					cout << v << endl;
					points = pts;
					pointsTTL = 10;

					Mat dstMat(Size(400, 400), CV_8UC3);
					vector<Point2f> src = {{0, 0}, {dstMat.cols, 0}, {dstMat.cols, dstMat.rows}, {0, dstMat.rows}};
					vector<Point2f> dst;
					for (auto p : points)
						dst.push_back(Point2f(p.x, p.y));
					auto wrap_mtx = getPerspectiveTransform(dst, src);
					warpPerspective(frame, dstMat, wrap_mtx, Size(dstMat.cols, dstMat.rows));

					imshow("RESULT", dstMat);
				}
			}
		}
		if (points.size() > 0)
		{
			for (int i = 0; i < 4; i++)
				line(frame, points.at(i), points.at((i + 1) % 4), Scalar(255, i * 63, 255));
		}
		if (pointsTTL-- <= 0)
		{
			pointsTTL = 0;
			points.clear();
		}
		flip(frame, frameFlipped, 1);
		imshow("frameFlipped", frameFlipped);
	}
	return 0;
}
