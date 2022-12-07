#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <vector>

// g++ `pkg-config --cflags opencv4` cv.cpp `pkg-config --libs opencv4`

// see https://docs.opencv.org/4.x/dd/d49/tutorial_py_contour_features.html


using namespace std;
using namespace cv;

void drawPictureOnPict(const Mat srcMat, const Mat imgToPut, Mat &resultMat, const vector<Point> contour)
{
	Mat dstMat(srcMat.rows, srcMat.cols, CV_8UC3);
	Mat cutter(imgToPut.rows, imgToPut.cols, imgToPut.type());

	cutter = Scalar(0);
	bitwise_not(cutter, cutter);
	vector<Point2f> src = {{0, 0}, {imgToPut.cols, 0}, {imgToPut.cols, imgToPut.rows}, {0, imgToPut.rows}};
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

int main(int argc, char** argv)
{
    int C = 47;
    int epsilon = 12;
    namedWindow("parametry", cv::WINDOW_AUTOSIZE);
    createTrackbar("C", "parametry", &C, 255);
    createTrackbar("epsilon", "parametry", &epsilon, 255);

	if (argc < 2) throw std::invalid_argument("Podaj argumenty");
	Mat picture = imread(argv[1]);

    imshow("toput",picture);
    vector<VideoCapture> cameras;
    for (auto e : vector<string>(argv + 2, argv + argc)) {
        cout << "camera number " << e << endl;
        cameras.push_back(VideoCapture(stoi(e)));
        if (!cameras.back().isOpened()) {
            cerr << "error opening camera " << e << endl;
            return -1;
        }
    }

    while (true) {
        int i = 0;
        for (auto cap : cameras) {
            Mat original_image, m;
            cap >> original_image;
            Mat k = getStructuringElement(MORPH_ELLIPSE, {5, 5});
            Mat diledges;

            cvtColor(original_image, m, COLOR_BGR2GRAY);
            //threshold(m, m, C, 255, THRESH_BINARY);
            // adaptiveThreshold(m, m, 128, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 5, C / 10.0);
            morphologyEx(m, m, MORPH_OPEN, k);
            morphologyEx(m, m, MORPH_CLOSE, k);
            Canny(m,m,100,200);
            imshow("capture " + to_string(i++), m);
            vector<vector<Point>> contours;
            findContours(m, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
            Mat imgContours = original_image.clone();
            drawContours(imgContours,contours, -1,{255,0,255},1);
            imshow("found contours", imgContours);
            for (int i = 0; i < contours.size(); i++) {
                approxPolyDP(contours[i], contours[i], epsilon, true);
                if ((contours[i].size() == 4) && (contourArea(contours[i], true) > 100)) {
                    int area = contourArea(contours[i], true);
                    if (area > 0)
                        drawContours(original_image, contours, i, {0, 255, 0}, 1);
                    else
                        drawContours(original_image, contours, i, {255, 0, 255}, 1);

					drawPictureOnPict(original_image, picture, original_image, contours[i]);
                }
            }
            imshow("contours " + to_string(i++), original_image);
        }
        if (waitKey(1) == 27)
            break;
    }
    return 0;
}
