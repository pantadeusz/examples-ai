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


#include <cv.hpp>
#include <highgui.h>
#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;
Mat glasses;
/** Function Headers */
void detectAndDisplay( Mat frame );

/**
 * @function main
 */
int main( void ) {
	VideoCapture capture;
	Mat frame;

	//-- 1. Load the cascade
	if( !face_cascade.load( String( "lbpcascade_frontalface.xml" ) ) ) {
		printf( "--(!)Error loading face cascade\n" );
		return -1;
	};
	if( !eyes_cascade.load( String( "haarcascade_eye_tree_eyeglasses.xml" ) ) ) {
		printf( "--(!)Error loading eyes cascade\n" );
		return -1;
	};
	glasses = imread( "dwi.png",-1 );
	std::cout << "C:" << glasses.channels() << "\n";
	//-- 2. Read the video stream
	capture.open( -1 );
	if ( ! capture.isOpened() ) {
		printf( "--(!)Error opening video capture\n" );
		return -1;
	}

	while ( capture.read( frame ) ) {
		if( frame.empty() ) {
			printf( " --(!) No captured frame -- Break!" );
			break;
		}

		//-- 3. Apply the classifier to the frame
		detectAndDisplay( frame );

		//-- bail out if escape was pressed
		int c = waitKey( 10 );
		if( ( char )c == 27 ) {
			break;
		}
	}
	return 0;
}

// funkcja nakladajaca obraz z przezroczystoscia
// w oparciu o http://dsynflo.blogspot.in/2014/08/simplar-2-99-lines-of-code-for.html
void imageOverImageBGRA( const Mat &srcMat, Mat &dstMat, const vector<Point2f> &dst ) {
	if ( srcMat.channels() != 4 ) throw "Nakladam tylko obrazy BGRA";

	vector<Point2f> src; // wspolrzedne punktow z obrazu nakladanego
	vector<Mat> rgbaChannels( 4 );
	Mat cpy_img( dstMat.rows, dstMat.cols, dstMat.type() );
	Mat neg_img( dstMat.rows, dstMat.cols, dstMat.type() );
	Mat blank( srcMat.rows, srcMat.cols, srcMat.type() );

	src.push_back( Point2f( 0,0 ) );
	src.push_back( Point2f( srcMat.cols,0 ) );
	src.push_back( Point2f( srcMat.cols, srcMat.rows ) );
	src.push_back( Point2f( 0, srcMat.rows ) );
	Mat warp_matrix = getPerspectiveTransform( src, dst );
	split( srcMat, rgbaChannels );
	rgbaChannels[0] = rgbaChannels[3]; // generujemy maske
	rgbaChannels[1] = rgbaChannels[3];
	rgbaChannels[2] = rgbaChannels[3];
	rgbaChannels.pop_back();
	merge( rgbaChannels,blank );

	warpPerspective( srcMat, neg_img, warp_matrix, Size( neg_img.cols, neg_img.rows ) ); // Transform overlay Image to the position - [ITEM1]
	warpPerspective( blank, cpy_img, warp_matrix, Size( cpy_img.cols, neg_img.rows ) ); // Transform a blank overlay image to position
	dstMat = dstMat-cpy_img;
	rgbaChannels = vector<Mat>( 3 );
	split( neg_img, rgbaChannels );
	rgbaChannels.pop_back();
	merge( rgbaChannels,neg_img );
	neg_img = neg_img.mul(cpy_img);
	dstMat = dstMat+neg_img;
}


void detectAndDisplay( Mat frame ) {
	std::vector<Rect> faces;
	Mat frame_gray;

	cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
	equalizeHist( frame_gray, frame_gray );
	imshow("frame_gray",frame_gray);

	//-- Wykrywamy twarz
	face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0, Size( 12, 12 ) );

	for( size_t i = 0; i < faces.size(); i++ ) {
		Mat faceROI = frame_gray( faces[i] ); // range of interest
		imshow ("ROI", faceROI);
		std::vector<Rect> eyes;
		eyes_cascade.detectMultiScale( faceROI, eyes, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size( 5, 5 ) );
		if( eyes.size() > 0 ) {
			vector<Point2f> dst;
			dst.push_back( Point2f( faces[i].x               ,faces[i].y+faces[i].height*5/20               ) );
			dst.push_back( Point2f( faces[i].x+faces[i].width,faces[i].y+faces[i].height*5/20               ) );
			dst.push_back( Point2f( faces[i].x+faces[i].width,faces[i].y+faces[i].height*5/20+faces[i].height*3/10 ) );
			dst.push_back( Point2f( faces[i].x               ,faces[i].y+faces[i].height*5/20+faces[i].height*3/10 ) );
			imageOverImageBGRA( glasses.clone(),frame, dst );
		}
	}
	cv::flip( frame, frame, 1 );

	imshow( "DWI", frame );
}
