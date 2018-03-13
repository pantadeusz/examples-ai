#include <cv.hpp>
#include <highgui.h>
#include <iostream>

using namespace std;
using namespace cv;

int main( int argc, char** argv ) {
	double w = 320,h = 240,fps;
	bool continueCapture = true;
	namedWindow( "efekty", CV_WINDOW_AUTOSIZE );
	Mat background;
	{
		Mat bg = imread("plaza.jpg", CV_LOAD_IMAGE_COLOR);
		resize(bg,background, {w,h});
	}
	VideoCapture cap(0);
	if ( !cap.isOpened() ) return -1;
	while( continueCapture ) {
		int key;
		Mat frame;
		if ( cap.read( frame ) ) {
			resize(frame,frame, {w,h});
			Mat maskWhite, maskBlack;
			inRange(frame,Scalar(160,0,0),Scalar(255,255,255),maskWhite); 	
			bitwise_not(maskWhite,maskBlack); 	
			Mat frame2;
			Mat bg2;
			frame.copyTo(frame2, maskBlack);
			background.copyTo(bg2, maskWhite);
			frame = frame2 + bg2;
			imshow( "efekty", frame );
			
		} else continueCapture = false;
		if( (waitKey( 33 )&0x0ff) == 27 ) continueCapture = false;
	}
	return 0;
}
