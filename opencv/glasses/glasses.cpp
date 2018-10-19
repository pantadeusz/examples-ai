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

#include <cv.hpp>
#include <highgui.h>
#include <iostream>

using namespace std;
using namespace cv;

CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;
Mat glasses;
/** Function Headers */
vector<Point2f> detectAndDisplay( Mat frame );
void imageOverImageBGRA( const Mat &srcMat, Mat &dstMat, const vector<Point2f> &dstFrameCoordinates );



/**
 * @function main
 */
int main( void ) {
	VideoCapture capture;
	Mat frame;

	//-- 1. Load the cascade
	if( !face_cascade.load( String( "lbpcascade_frontalface.xml" ) ) ) {
		return -9;
	};
	if( !eyes_cascade.load( String( "haarcascade_eye_tree_eyeglasses.xml" ) ) ) {
		return -8;
	};
	glasses = imread( "dwi.png", -1 );
	std::cout << "C:" << glasses.channels() << "\n";
	capture.open( -1 );
	if ( ! capture.isOpened() ) {
		return -7;
	}

	while ( capture.read( frame ) ) {
		if( frame.empty() ) return -1;
		
		auto detected_eyes = detectAndDisplay( frame );
		
		if (detected_eyes.size()) imageOverImageBGRA( glasses.clone(), frame, detected_eyes );
		cv::flip( frame, frame, 1 );
		imshow( "DWI", frame );

		if( (waitKey( 1 )&0x0ff) == 27 ) return 0;
	}
	return 0;
}

// funkcja nakladajaca obraz z przezroczystoscia
// w oparciu o http://dsynflo.blogspot.in/2014/08/simplar-2-99-lines-of-code-for.html
void imageOverImageBGRA( const Mat &srcMat, Mat &dstMat, const vector<Point2f> &dstFrameCoordinates ) {
	if ( srcMat.channels() != 4 ) throw "Nakladam tylko obrazy BGRA";

	// tylko kanal alpha
	vector<Mat> rgbaChannels( 4 );
	Mat srcAlphaMask( srcMat.rows, srcMat.cols, srcMat.type() );
	split( srcMat, rgbaChannels );
	rgbaChannels = {rgbaChannels[3],rgbaChannels[3],rgbaChannels[3]};
	merge( rgbaChannels, srcAlphaMask );

	// wspolrzedne punktow z obrazu nakladanego
	vector<Point2f> srcFrameCoordinates = {{0,0},{(float)srcMat.cols,0},{(float)srcMat.cols,(float)srcMat.rows},{0,(float)srcMat.rows}};
	Mat warp_matrix = getPerspectiveTransform( srcFrameCoordinates, dstFrameCoordinates );

	Mat cpy_img( dstMat.rows, dstMat.cols, dstMat.type() );
	warpPerspective( srcAlphaMask, cpy_img, warp_matrix, Size( cpy_img.cols, cpy_img.rows ) );
	Mat neg_img( dstMat.rows, dstMat.cols, dstMat.type() );
	warpPerspective( srcMat, neg_img, warp_matrix, Size( neg_img.cols, neg_img.rows ) );
	dstMat = dstMat - cpy_img;
	
	cvtColor(neg_img, neg_img, CV_BGRA2BGR);
	cpy_img = cpy_img / 255;
	neg_img = neg_img.mul( cpy_img );
	dstMat = dstMat + neg_img;
}


vector<Point2f> detectAndDisplay( Mat frame ) {
	std::vector<Rect> faces;
	Mat frame_gray;

	cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
	equalizeHist( frame_gray, frame_gray );

	// detect face
	face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0, Size( 12, 12 ) );

	for( size_t i = 0; i < faces.size(); i++ ) {
		Mat faceROI = frame_gray( faces[i] ); // range of interest
		//imshow ( "ROI", faceROI );
		std::vector<Rect> eyes;
		eyes_cascade.detectMultiScale( faceROI, eyes, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size( 5, 5 ) );
		if( eyes.size() > 0 ) {
			vector<Point2f> dst = {
				Point2f( faces[i].x, faces[i].y + faces[i].height * 5 / 20 ) ,
				Point2f( faces[i].x + faces[i].width, faces[i].y + faces[i].height * 5 / 20               ) ,
				Point2f( faces[i].x + faces[i].width, faces[i].y + faces[i].height * 5 / 20 + faces[i].height * 3 / 10 ) ,
				Point2f( faces[i].x, faces[i].y + faces[i].height * 5 / 20 + faces[i].height * 3 / 10 )
			};
			return dst;
		}
	}
	return {};
}
