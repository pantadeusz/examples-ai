#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <vector>

void drawPictureOnPict(const cv::Mat srcMat, const cv::Mat imgToPut, cv::Mat &resultMat,
                       const std::vector<cv::Point> contour) {
  using namespace cv;
  using namespace std;
  Mat dstMat(srcMat.rows, srcMat.cols, CV_8UC3);
  Mat imgMask(imgToPut.rows, imgToPut.cols, imgToPut.type());

  imgMask = Scalar(0);
  bitwise_not(imgMask, imgMask);
  vector<Point2f> src = {{0, 0},
                         {imgToPut.cols, 0},
                         {imgToPut.cols, imgToPut.rows},
                         {0, imgToPut.rows}};
  vector<Point2f> dst;
  for (auto p : contour) dst.push_back(Point2f(p.x, p.y));
  auto wrap_mtx = getPerspectiveTransform(src, dst);
  warpPerspective(imgToPut, dstMat, wrap_mtx, Size(dstMat.cols, dstMat.rows));
  warpPerspective(imgMask, imgMask, wrap_mtx, Size(dstMat.cols, dstMat.rows));
  bitwise_not(imgMask, imgMask);
  Mat dstMatClone;
  srcMat.copyTo(dstMatClone, imgMask);
  
  dstMat = dstMat + dstMatClone;
  resultMat = dstMat;
}

int main() {
  using namespace cv;
  using namespace std;
  VideoCapture camera(0);
  int threshold1 = 100;
  int threshold2 = 100;
  auto myImage = imread("tree.jpeg");
  namedWindow("trackbars");
  createTrackbar("threshold1", "trackbars", &threshold1, 500);
  createTrackbar("threshold2", "trackbars", &threshold2, 500);

  while (waitKey(10) != 27) {
    Mat src_image;
    Mat src_image_bw;
    Mat img_edges;
    camera >> src_image;
    cvtColor(src_image, src_image_bw, COLOR_BGR2GRAY);
    imshow("source image bw", src_image_bw);
    Canny(src_image_bw, img_edges, threshold1, threshold2);
    imshow("img_edges", img_edges);
    auto kern = getStructuringElement(MORPH_ELLIPSE, {5, 5});
    morphologyEx(img_edges, img_edges, MORPH_CLOSE, kern);
    imshow("img_edges_after", img_edges);
    vector<vector<Point>> contours;
    vector<vector<Point>> contours_simple;
    findContours(img_edges, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
    drawContours(src_image, contours, -1, {255, 255, 255});
    double epsilon = 5;
    for (auto &contour : contours) {
      vector<Point> simple;
      if (contourArea(contour, true) > 100) {
        approxPolyDP(contour, simple, epsilon, true);
        if (simple.size() == 4) {
          contours_simple.push_back(simple);
          auto m = moments(contour, false);
          Point p = {(int)(m.m10 / m.m00), (int)(m.m01 / m.m00)};
          drawPictureOnPict(src_image, myImage, src_image, simple);
          ellipse(src_image, p, Size{10, 10}, 0.0, 0, 360, {0, 0, 255}, 3);
        
        }
      }
    }

    drawContours(src_image, contours_simple, -1, {255, 0, 0}, 3);
    imshow("source image", src_image);
  }
  return 0;
}