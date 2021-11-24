#include <algorithm>
#include <iostream>
#include <numeric>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <vector>

using namespace cv;

int main()
{
    using namespace std;
    int loRange[3] = {29, 21, 0};
    int hiRange[3] = {52, 245, 248};
    namedWindow("jakostam");
    createTrackbar("loRange0", "jakostam", &(loRange[0]), 255);
    createTrackbar("loRange1", "jakostam", &(loRange[1]), 255);
    createTrackbar("loRange2", "jakostam", &(loRange[2]), 255);
    createTrackbar("hiRange0", "jakostam", &(hiRange[0]), 255);
    createTrackbar("hiRange1", "jakostam", &(hiRange[1]), 255);
    createTrackbar("hiRange2", "jakostam", &(hiRange[2]), 255);
    VideoCapture camera(0);
    // VideoCapture backgroundvid("Multiwave.wmv");
    list<Point> tracking_path;

    while (waitKey(1) != 27) {
        Mat frame;
        Mat backgroundScaled;
        Mat frameMask, frameNegMask;
        Mat frameWithMask, backgroundScaledWithMask;
        Mat meinniceplace;
        //(with animated background) Mat background;
        //(with animated background) backgroundvid >> background;
        camera >> frame;
        flip(frame, frame, 1);
        // resize(background, backgroundScaled,{frame.cols, frame.rows});
        cvtColor(frame, frameMask, COLOR_RGB2HSV);
        inRange(frameMask, Scalar(loRange[0], loRange[1], loRange[2]),
            Scalar(hiRange[0], hiRange[1], hiRange[2]), frameNegMask);
        int dilation_size = 5;
        auto structElem = getStructuringElement(MORPH_ELLIPSE,
            Size(2 * dilation_size + 1, 2 * dilation_size + 1),
            Point(dilation_size, dilation_size));
        erode(frameNegMask, frameNegMask, structElem);
        dilate(frameNegMask, frameNegMask, structElem);

        imshow("jakostam", frameNegMask);
        vector<vector<Point>> contours;
        findContours(frameNegMask, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
        std::sort(contours.begin(), contours.end(), [](auto& a, auto& b) {
            return contourArea(a, false) > contourArea(b, false);
        });
        if ((contours.size() > 0) && (contourArea(contours[0], false) > 400)) {
            approxPolyDP(contours[0], contours[0], 2, true);
            int area = contourArea(contours[0], true);
            float radius;
            Point2f center;
            minEnclosingCircle(contours[0], center, radius);
            putText(frame, to_string(area), center, FONT_HERSHEY_PLAIN, 1.0, {255, 33, 0});
            tracking_path.push_back(center);
        } else {
            if (tracking_path.size()) tracking_path.pop_front();
        }
        if (tracking_path.size()) {
            vector<Point> shape_contour(tracking_path.begin(),tracking_path.end());
            approxPolyDP(shape_contour, shape_contour, 15, true);
            vector<vector<Point>> to_show = {shape_contour};
            drawContours(frame, to_show,0,{0,200,100});

            to_show = {{tracking_path.begin(), tracking_path.end()}};
            drawContours(frame, to_show,0,{0,100,200});

            if ((shape_contour.size() == 4) && (contourArea(shape_contour,false) > 10000)) {
                vector<double> lengths(4);
                for (int i = 0 ; i < 4; i ++) {
                    Point2i vec = shape_contour[i]-shape_contour[(i+1)%4];
                    lengths[i] = sqrt(vec.x*vec.x + vec.y*vec.y);
                }
                if ((abs(lengths[0]/lengths[2]-1.0) < 0.1) || (abs(lengths[1]/lengths[3]-1.0) < 0.1)) {
                    cout << "jest prostokat!! " << contourArea(shape_contour,false) << endl;
                    tracking_path.clear();
                    system("gimp");
                }
            }

            if (tracking_path.size() > 60) {
                tracking_path.pop_front();
            }
        }
        imshow("oryginal", frame);

        // bitwise_not(frameNegMask,frameMask); // negation
        // frame.copyTo(frameWithMask,frameMask); // copy with mask (keying)
        // backgroundScaled.copyTo(backgroundScaledWithMask,frameNegMask);
        //
        // meinniceplace = backgroundScaledWithMask + frameWithMask;
    }
    return 0;
}
