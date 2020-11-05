// 
// Przykład algorytmu genetycznego dopasowującego układ kresek do zadanego (na stałe) obrazka
// 
// Program działa w nieskończoność
//
// Kompilacja:  
//   g++ -fopenmp `pkg-config --cflags opencv` linearFilter.cpp -o linearFilter `pkg-config --libs opencv`
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

int main(int argc, char** argv) {
	Mat img;
	VideoCapture cap(0);
	if ( !cap.isOpened() ) 
	{
		cout << "Brak kamerki" << endl;
		return -1;
	}
	do {
		Mat img,fimg,kernel;
		kernel = Mat::ones( 3, 3, CV_32F );
		kernel.at<float>(0,0) =  1.00;kernel.at<float>(1,0) = 1.00;kernel.at<float>(2,0) =  1.00;
		kernel.at<float>(0,1) =  1.00;kernel.at<float>(1,1) =-8.00;kernel.at<float>(2,1) =  1.00;
		kernel.at<float>(0,2) =  1.00;kernel.at<float>(1,2) = 1.00;kernel.at<float>(2,2) =  1.00;
		cap >> img;
		imshow( "cam", img );
		// Pokazać: http://docs.opencv.org/2.4/modules/imgproc/doc/filtering.html?highlight=filter2d#filter2d
        filter2D(img, fimg, -1 , kernel);// , anchor, 0, BORDER_DEFAULT );
		imshow( "fimg", fimg );

	} while ((waitKey(1) & 0x0ff) != 27);
	return 0;
}
