#include <cv.hpp>
#include <highgui.h>
#include <iostream>

using namespace cv;

int main() {
	int loRange[3] = {0,0,103};
	int hiRange[3] = {175,33,197};
	
	namedWindow("jakostam", CV_WINDOW_AUTOSIZE);
	createTrackbar("loRange0", "jakostam",&(loRange[0]), 255);
	createTrackbar("loRange1", "jakostam",&(loRange[1]), 255);
	createTrackbar("loRange2", "jakostam",&(loRange[2]), 255);
	createTrackbar("hiRange0", "jakostam",&(hiRange[0]), 255);
	createTrackbar("hiRange1", "jakostam",&(hiRange[1]), 255);
	createTrackbar("hiRange2", "jakostam",&(hiRange[2]), 255);
	VideoCapture camera(0);
	//VideoCapture backgroundvid("Multiwave.wmv");
	Mat background = imread("plaza.jpg", CV_LOAD_IMAGE_COLOR);
	while ( waitKey(1) != 27  ) {
		Mat frame;
		Mat backgroundScaled;
		Mat frameMask,frameNegMask;
		Mat frameWithMask,backgroundScaledWithMask;
		Mat meinniceplace;		
		//(with animated background) Mat background;
		//(with animated background) backgroundvid >> background;
		camera >> frame;
		flip(frame, frame, 1);
		resize(background, backgroundScaled,{frame.cols, frame.rows});
		cvtColor(frame, frameMask, CV_RGB2HSV);
		inRange(frameMask, Scalar(loRange[0],loRange[1],loRange[2]),
						Scalar(hiRange[0],hiRange[1],hiRange[2]), frameNegMask);
		int dilation_size = 5;
		auto structElem = getStructuringElement( MORPH_ELLIPSE,
                                       Size( 2*dilation_size + 1, 2*dilation_size+1 ),
                                       Point( dilation_size, dilation_size ) );
		erode( frameNegMask, frameNegMask, structElem ); 
		dilate( frameNegMask, frameNegMask, structElem );
		
		bitwise_not(frameNegMask,frameMask); // negation
		frame.copyTo(frameWithMask,frameMask); // copy with mask (keying)
		backgroundScaled.copyTo(backgroundScaledWithMask,frameNegMask);
		
		meinniceplace = backgroundScaledWithMask + frameWithMask;		
		imshow("jakostam", meinniceplace);
	}
	return 0;
}
