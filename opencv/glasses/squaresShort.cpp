//
// Przykład algorytmu genetycznego dopasowującego układ kresek do zadanego (na stałe) obrazka
//
// Program działa w nieskończoność
//
// Kompilacja:
//   g++ -fopenmp -std=c++11   `pkg-config --cflags opencv` squaresShort.cpp -o squaresShort `pkg-config --libs opencv`
//

#include <iostream>
#include <cstdlib>
#include <vector>
#include <cmath>

#include <cstdlib>
#include <iostream>

#include <cv.hpp>
#include <highgui.h>
//#include <opencv.hpp>
//#include <highgui/highgui_c.h>

using namespace std;
using namespace cv;

int lowThreshold = 16;//45;
int const max_lowThreshold = 100;

// kod za http://stackoverflow.com/questions/8667818/opencv-c-obj-c-detecting-a-sheet-of-paper-square-detection
double angle( cv::Point pt1, cv::Point pt2, cv::Point pt0 ) {
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return ( dx1*dx2 + dy1*dy2 )/sqrt( ( dx1*dx1 + dy1*dy1 )*( dx2*dx2 + dy2*dy2 ) + 1e-10 );
}

// za przykładem z dokumentacji OpenCV (mocno zmodyfikowany)
void find_squares( const Mat& image, vector<vector<Point> >& squares ) {
	Mat blurred( image.clone() );
	medianBlur( image, blurred, 9 );
	Mat gray0( blurred.size(), CV_8U ), gray;
	vector<vector<Point> > contours;
	cv::cvtColor(blurred, gray0, CV_BGR2GRAY);
	Canny( gray0, gray, lowThreshold, lowThreshold*2, 3 ); //

	dilate( gray, gray, Mat(), Point( -1,-1 ) );
	//imshow("po dilate", gray);
	//imshow("c",gray);
	findContours( gray, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE );

	vector<Point> approx;
	for ( size_t i = 0; i < contours.size(); i++ ) {
		approxPolyDP( Mat( contours[i] ), approx, arcLength( Mat( contours[i] ), true )*0.02, true );
		double ca = fabs(contourArea( Mat( approx ) ));
		bool notOnFrame = true;
		// dodatek -- nie przyjmujemy czworokatow ktore dotykaja krawedzi ekranu
		for ( int j = 0; j < approx.size() && notOnFrame; j++ ) {
			if ( approx[j].x < 10 ||
				 approx[j].y < 10 ||
				 approx[j].x > ( image.cols-10 ) ||
				 approx[j].y > ( image.rows-10 ) )
			notOnFrame = false;
		}
		// czy to czworokat?
		if ( approx.size() == 4 &&
			 notOnFrame &&
			 ca > 1200 &&
			 ca < gray.cols*gray.rows*0.75 &&
			 isContourConvex( Mat( approx ) ) ) {
			double maxCosine = 0;
			for ( int j = 2; j < 5; j++ ) {
				double cosine = fabs( angle( approx[j%4], approx[j-2], approx[j-1] ) );
				maxCosine = MAX( maxCosine, cosine );
			}
			if ( maxCosine < 0.4 ) {
				squares.push_back( approx );
			}
		}
	}
	Mat m = gray.clone();
	for (int i = 0; i < squares.size(); i++) {
		drawContours(m, squares, i, Scalar(255,0,0));
	}
	//imshow("sq",m);
}

// rysowanie obrazu display do obrazu dstMat w prostokacie (dla obrazu nakładanego w formacie BGR)
void imageToImage( const Mat &srcMat, Mat &dstMat, const vector<Point2f> &dst ) {
	vector<Point2f> src; 
	src.push_back( Point2f( 0,0 ) );
	src.push_back( Point2f( dstMat.cols,0 ) );
	src.push_back( Point2f( dstMat.cols, dstMat.rows ) );
	src.push_back( Point2f( 0, dstMat.rows ) );
	Mat warp_matrix = getPerspectiveTransform( dst, src );
	warpPerspective( srcMat, dstMat, warp_matrix, Size( dstMat.cols, dstMat.rows ) );
}

int main( int argc, char** argv ) {
	VideoCapture cap( 0 );
	Mat imgorig;
	cap >> imgorig;
	imshow( "cam", imgorig );
	createTrackbar( "Wspolczynnik:", "cam", &lowThreshold, max_lowThreshold );
	do {
		Mat img,img0;
		Mat display;
		cap >> img;
		img0 = img.clone();
		vector<vector<Point> > squares;
		vector<vector<Point> > squareSel;
		find_squares( img, squares );
		if (squares.size() > 0) {
			int biggest = 0;
			double biggestSize = fabs(contourArea(squares[0]));
			for ( int i = 1; i < squares.size(); i++ ) {
				double currArea = fabs(contourArea( squares[i] ));
				if ( currArea > biggestSize ) {
					biggestSize = currArea;
					biggest = i;
				}
			}
			squareSel.push_back(squares[biggest]);
			for ( int i = 0; i < squareSel.size(); i++ ) {
				Mat imgD(256, 256, img.type());
				vector<Point2f> dst;
				// tworzymy prostokat
				for ( int j = 0; j < 4; j++ )
					dst.push_back( squareSel[i][j] ); 
				imageToImage(img0, imgD, dst );
				cvtColor( imgD, imgD, COLOR_BGR2GRAY );
				equalizeHist( imgD, imgD );
				imshow( "camD", imgD );
			}
		}
		imshow( "cam", img );
	} while ( ( waitKey( 1 ) & 0x0ff ) != 27 );
	return 0;
}
