//
// Przykład algorytmu genetycznego dopasowującego układ kresek do zadanego (na stałe) obrazka
//
// Program działa w nieskończoność
//
// Kompilacja:
//   g++ -fopenmp `pkg-config --cflags opencv` squaresFull.cpp -o squaresFull `pkg-config --libs opencv`
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

// kod za http://stackoverflow.com/questions/8667818/opencv-c-obj-c-detecting-a-sheet-of-paper-square-detection
double angle( cv::Point pt1, cv::Point pt2, cv::Point pt0 ) {
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return ( dx1 * dx2 + dy1 * dy2 ) / sqrt( ( dx1 * dx1 + dy1 * dy1 ) * ( dx2 * dx2 + dy2 * dy2 ) + 1e-10 );
}

// za przykładem z dokumentacji OpenCV (mocno zmodyfikowany)
void find_squares( const Mat& image, vector<vector<Point> >& squares ) {
	Mat blurred( image.clone() );
	Mat preview( image.clone() );

	int dilation_size = 7;
	auto structElem = getStructuringElement( MORPH_ELLIPSE,
					  Size( 2 * dilation_size + 1, 2 * dilation_size + 1 ),
					  Point( dilation_size, dilation_size ) );
	morphologyEx( image, blurred, MORPH_CLOSE, structElem );

	dilation_size = 10;
	structElem = getStructuringElement( MORPH_ELLIPSE,
										Size( 2 * dilation_size + 1, 2 * dilation_size + 1 ),
										Point( dilation_size, dilation_size ) );

	Mat gray0( blurred.size(), CV_8U ), gray;
	vector<vector<Point> > contours;
	int c = 0;
	cv::cvtColor( blurred, gray0, CV_BGR2GRAY );
	// kilka poziomow progowania
	const int threshold_level = 8;
	for ( int l = 0; l < threshold_level; l++ ) {
		if ( l == 0 ) {
			adaptiveThreshold( gray0, gray, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 11, 5 );
			morphologyEx( gray, gray, MORPH_OPEN, structElem );
		} else {
			// progowanie (kilka progow)
			gray = gray0 >= ( l ) * 255 / threshold_level;
		}
		// Find contours and store them in a list
//		findContours( gray, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE );
			findContours( gray, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE );
		// Test contours
		vector<Point> approx;
		for ( size_t i = 0; i < contours.size(); i++ ) {
			// approximate contour with accuracy proportional
			// to the contour perimeter
			//approxPolyDP( Mat( contours[i] ), approx, arcLength( Mat( contours[i] ), true )*0.02, true );
			approxPolyDP( Mat( contours[i] ), approx, arcLength( Mat( contours[i] ), true ) * 0.02, true );
			// Note: absolute value of an area is used because
			// area may be positive or negative - in accordance with the
			// contour orientation
			double ca = fabs( contourArea( Mat( approx ) ) );
			if ( approx.size() == 4 &&
					ca > 800 &&
					ca < gray.cols * gray.rows * 0.45 &&
					isContourConvex( Mat( approx ) ) ) {
				double maxCosine = 0;
				for ( int j = 2; j < 5; j++ ) {
					double cosine = fabs( angle( approx[j % 4], approx[j - 2], approx[j - 1] ) );
					maxCosine = MAX( maxCosine, cosine );
				}
				if ( maxCosine < 0.7 ) {
					squares.push_back( approx );
				}
			}
		}
	}
}

// rysowanie obrazu display do obrazu dstMat w prostokacie (dla obrazu nakładanego w formacie BGR)
void imageOverImage( const Mat &srcMat, Mat &dstMat, const vector<Point2f> &dst ) {
	Mat cpy_img( dstMat.rows, dstMat.cols, dstMat.type() );
	Mat neg_img( dstMat.rows, dstMat.cols, dstMat.type() );

	Mat blank( srcMat.rows, srcMat.cols, srcMat.type() );

	vector<Point2f> src = {{0, 0}, {( float )srcMat.cols, 0}, {( float )srcMat.cols, ( float )srcMat.rows}, {0, ( float )srcMat.rows}};

	Mat warp_matrix = getPerspectiveTransform( src, dst );

	blank = Scalar( 0 );
	bitwise_not( blank, blank );

	warpPerspective( srcMat, neg_img, warp_matrix, Size( neg_img.cols, neg_img.rows ) );
	warpPerspective( blank, cpy_img, warp_matrix, Size( cpy_img.cols, neg_img.rows ) );
	bitwise_not( cpy_img, cpy_img );
	bitwise_and( cpy_img, dstMat, cpy_img );
	bitwise_or( cpy_img, neg_img, dstMat );
}


int main( int argc, char** argv ) {
	VideoCapture cap( 0 );
	Mat imgorig;
	cap >> imgorig;
	imshow( "cam", imgorig );
	auto nyan = imread( "nyan.jpg", CV_LOAD_IMAGE_COLOR );
	do {
		Mat img, img0;
		Mat display;
		cap >> img;
		img0 = img.clone();
		display = nyan.clone();
		vector<vector<Point> > squares;
		vector<vector<int> > groups;
		find_squares( img, squares );
		for ( int i = 0; i < squares.size(); i++ ) {
			vector<Point2f> dst;
			// konwersja do Point2f z Point2i
			for ( int j = 0; j < 4; j++ )
				dst.push_back( squares[i][j] );
			imageOverImage( display, img, dst );
		}
		imshow( "cam", img );
	} while ( ( waitKey( 1 ) & 0x0ff ) != 27 );
	return 0;
}
