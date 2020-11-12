/**
 * @file objectDetection2.cpp
 * @author A. Huaman ( based in the classic facedetect.cpp in samples/c )
 * @brief A simplified version of facedetect.cpp, show how to
 *       load a cascade classifier and how to find objects (Face + eyes)
 *       in a video stream - Using LBP here
 */
// kod zmodyfikowany na potrzeby zajęć z NAI na PJATK Gdańsk

// g++ -fopenmp `pkg-config --cflags opencv` glasses.cpp -o glasses `pkg-config --libs opencv`
// okulary pobrane z http://memesvault.com/wp-content/uploads/Deal-With-It-Sunglasses-07.png
// elementy związane z przekształceniem geometrycznym http://dsynflo.blogspot.in/2014/08/simplar-2-99-lines-of-code-for.html
// zachęcam do zapoznania się z https://docs.opencv.org/2.4/modules/imgproc/doc/geometric_transformations.html

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

using namespace std;
using namespace cv;

CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;

vector<Rect> find_faces(const Mat frame0)
{
	Mat frame = frame0.clone();
	vector<Rect> faces;
	vector<Rect> real_faces;
	cvtColor(frame, frame, COLOR_BGR2GRAY);
	equalizeHist(frame, frame);

	face_cascade.detectMultiScale(frame, faces);
	for (auto face : faces)
	{
		vector<Rect> eyes;

		Mat faceimg = frame(face);
		eyes_cascade.detectMultiScale(faceimg, eyes);
		if (eyes.size() == 2)
		{
			Mat eyeimg = faceimg(eyes[0]);
			real_faces.push_back(face);
		}
	}
	return real_faces;
}
// funkcja nakladajaca obraz z przezroczystoscia
// w oparciu o http://dsynflo.blogspot.in/2014/08/simplar-2-99-lines-of-code-for.html
void imageOverImageBGRA(const Mat &srcMat, Mat &dstMat, const vector<Point2f> &dstFrameCoordinates)
{
	if (srcMat.channels() != 4)
		throw "Nakladam tylko obrazy BGRA";

	// tylko kanal alpha
	vector<Mat> rgbaChannels(4);
	Mat srcAlphaMask(srcMat.rows, srcMat.cols, srcMat.type());
	split(srcMat, rgbaChannels);
	rgbaChannels = {rgbaChannels[3], rgbaChannels[3], rgbaChannels[3]};
	merge(rgbaChannels, srcAlphaMask);

	// wspolrzedne punktow z obrazu nakladanego
	vector<Point2f> srcFrameCoordinates = {{0, 0}, {(float)srcMat.cols, 0}, {(float)srcMat.cols, (float)srcMat.rows}, {0, (float)srcMat.rows}};
	Mat warp_matrix = getPerspectiveTransform(srcFrameCoordinates, dstFrameCoordinates);

	Mat cpy_img(dstMat.rows, dstMat.cols, dstMat.type());
	warpPerspective(srcAlphaMask, cpy_img, warp_matrix, Size(cpy_img.cols, cpy_img.rows));
	Mat neg_img(dstMat.rows, dstMat.cols, dstMat.type());
	warpPerspective(srcMat, neg_img, warp_matrix, Size(neg_img.cols, neg_img.rows));
	dstMat = dstMat - cpy_img;

	cvtColor(neg_img, neg_img, COLOR_BGRA2BGR);
	cpy_img = cpy_img / 255;
	neg_img = neg_img.mul(cpy_img);
	dstMat = dstMat + neg_img;
}

/**
 * @function main
 */
int main(void)
{
	VideoCapture capture;
	Mat glassesimg = cv::imread("dwi.png", -1);
	//-- 1. Load the cascade
	if (!face_cascade.load("lbpcascade_frontalface.xml"))
	{
		return -9;
	};
	if (!eyes_cascade.load("haarcascade_eye_tree_eyeglasses.xml"))
	{
		return -8;
	};
	capture.open(-1);
	if (!capture.isOpened())
	{
		return -7;
	}

	Mat frame;
	while (capture.read(frame))
	{
		if (frame.empty())
			return -1;

		vector<Rect> faces = find_faces(frame);

		imshow("obraz", frame);
		Mat frame2 = frame.clone();
		if (faces.size())
		{
			auto face = faces[0];
			imageOverImageBGRA(glassesimg, frame2, {
													   {(float)face.x, (float)face.y + face.height / 8},
													   {(float)face.x + (float)face.width, (float)face.y + face.height / 8},
													   {(float)face.x + (float)face.width, (float)face.y + face.height / 2},
													   {(float)face.x, (float)face.y + face.height / 2},
												   });
		}
		imshow("obraz", frame2);
		if ((waitKey(1) & 0x0ff) == 27)
			return 0;
	}
	return 0;
}
