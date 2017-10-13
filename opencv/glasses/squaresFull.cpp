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

int lowThreshold = 12;//45;
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
	Mat preview( image.clone() );
	medianBlur( image, blurred, 9 );
	Mat gray0( blurred.size(), CV_8U ), gray;
	vector<vector<Point> > contours;

	for ( int c = 0; c < 3; c++ ) {
		int ch[] = {c, 0};
		mixChannels( &blurred, 1, &gray0, 1, ch, 1 );
		//cv::cvtColor(blurred, gray0, CV_BGR2GRAY);

		// kilka poziomow progowania
		const int threshold_level = 4;
		for ( int l = -1; l < threshold_level; l++ ) {
			if ( l == 0 ) {
				adaptiveThreshold( gray, gray, 255,CV_ADAPTIVE_THRESH_MEAN_C,CV_THRESH_BINARY,13, 1 );
			} else if ( l == -1 ) {
				//Canny(gray0, gray, 10, 20, 3); //
				Canny( gray0, gray, lowThreshold, lowThreshold*2, 3 ); //
				dilate( gray, gray, Mat(), Point( -1,-1 ) );
			} else {
				// progowanie (kilka progow)
				gray = gray0 >= ( l+1 ) * 255 / threshold_level;
			}

			// Find contours and store them in a list
			findContours( gray, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE );

			// Test contours
			vector<Point> approx;
			for ( size_t i = 0; i < contours.size(); i++ ) {
				// approximate contour with accuracy proportional
				// to the contour perimeter
				approxPolyDP( Mat( contours[i] ), approx, arcLength( Mat( contours[i] ), true )*0.02, true );
				// Note: absolute value of an area is used because
				// area may be positive or negative - in accordance with the
				// contour orientation
				double ca = fabs(contourArea( Mat( approx ) ));
				bool notOnFrame = true;
				// dodatek -- nie przyjmujemy czworokatow ktore dotykaja krawedzi ekranu
				for ( int j = 0; j < approx.size() && notOnFrame; j++ ) {
					if ( approx[j].x < 20 ||
					     approx[j].y < 20 ||
					     approx[j].x > ( image.cols-20 ) ||
					     approx[j].y > ( image.rows-20 ) )
					notOnFrame = false;
				}
				if ( approx.size() == 4 &&
				     notOnFrame &&
				     ca > 1200 &&
				     ca < gray.cols*gray.rows*0.45 &&
				     isContourConvex( Mat( approx ) ) ) {
					double maxCosine = 0;
					for ( int j = 2; j < 5; j++ ) {
						double cosine = fabs( angle( approx[j%4], approx[j-2], approx[j-1] ) );
						maxCosine = MAX( maxCosine, cosine );
					}
					if ( maxCosine < 0.7 ) {
						squares.push_back( approx );
					}
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
	vector<Point2f> src; 
	
	src.push_back( Point2f( 0,0 ) );
	src.push_back( Point2f( srcMat.cols,0 ) );
	src.push_back( Point2f( srcMat.cols, srcMat.rows ) );
	src.push_back( Point2f( 0, srcMat.rows ) );
	Mat warp_matrix = getPerspectiveTransform( src, dst );
	blank = Scalar( 0 );

	bitwise_not( blank,blank );

	warpPerspective( srcMat, neg_img, warp_matrix, Size( neg_img.cols, neg_img.rows ) );
	warpPerspective( blank, cpy_img, warp_matrix, Size( cpy_img.cols, neg_img.rows ) );
	imshow("neg_img",neg_img);
	imshow("cpy_img",cpy_img);
	bitwise_not( cpy_img, cpy_img );
	bitwise_and( cpy_img, dstMat, cpy_img );
	bitwise_or( cpy_img, neg_img, dstMat );
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
		//img = imread("squ.jpg", CV_LOAD_IMAGE_COLOR);
		cap >> img;
		img0 = img.clone();
		display = imread( "nyan.jpg", CV_LOAD_IMAGE_COLOR );
		vector<vector<Point> > squares;
		vector<vector<int> > groups;
		find_squares( img, squares );
		for ( int i = 0; i < squares.size(); i++ ) {
			int gr = -1;
			for ( int j = 0; j < groups.size() && gr < 0; j++ ) {
				for ( int jj = 0; jj < groups[j].size() && gr < 0; jj++ ) {
					for ( int k = 0; k < 4 && gr < 0; k++ ) {
						if ( pointPolygonTest( squares[groups[j][jj]], squares[i][k], false ) >= 0 ) {
							gr = j;
						} else if ( pointPolygonTest( squares[i], squares[groups[j][jj]][k], false ) >= 0 ) {
							gr = j;
						}
					}
				}
			}
			if ( gr < 0 ) {
				vector<int> vv;
				vv.push_back( i );
				groups.push_back( vv );
			} else {
				for ( int j = 0; j < groups[gr].size(); j++ ) {
					if ( contourArea( squares[groups[gr][j]] ) < contourArea( squares[i] ) ) {
						groups[gr].push_back( groups[gr][j] );
						groups[gr][j] = i;
						break;
					}
				}

			}
		}
		for ( int i = 0; i < groups.size(); i++ ) {
			vector<Point2f> dst;
			for ( int j = 0; j < 4; j++ )
				dst.push_back( squares[groups[i][0]][j] );
			imageOverImage( display, img, dst );
			drawContours( img, squares, groups[i][0], Scalar( 255,0,0 ) );
		}
		/*		for (int i = 0; i < sq.size(); i++)
					drawContours( img, sq, i, Scalar(255,0,0)); */
		imshow( "cam", img );
	} while ( ( waitKey( 1 ) & 0x0ff ) != 27 );
	return 0;
}
