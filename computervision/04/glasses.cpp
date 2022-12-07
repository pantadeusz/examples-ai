/**
 * @file objectDetection2.cpp
 * @author A. Huaman ( based in the classic facedetect.cpp in samples/c )
 * @brief A simplified version of facedetect.cpp, show how to
 *       load a cascade classifier and how to find objects (Face + eyes)
 *       in a video stream - Using LBP here
 */
// kod zmodyfikowany na potrzeby zajęć z NAI na PJATK Gdańsk

// g++ -fopenmp `pkg-config --cflags opencv4` glasses.cpp -o glasses `pkg-config --libs opencv4`
// okulary pobrane z http://memesvault.com/wp-content/uploads/Deal-With-It-Sunglasses-07.png
// elementy związane z przekształceniem geometrycznym http://dsynflo.blogspot.in/2014/08/simplar-2-99-lines-of-code-for.html
// zachęcam do zapoznania się z https://docs.opencv.org/2.4/modules/imgproc/doc/geometric_transformations.html

#include <iostream>
#include <list>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;
Mat glasses;
/** Function Headers */
std::vector<vector<Point2f>> detectFacesLocations(Mat frame);
void imageOverImageBGRA(const Mat& srcMat, Mat& dstMat, const vector<Point2f>& dstFrameCoordinates);

double rectDiastance(vector<Point2f> a, vector<Point2f> b)
{
    double sum = 0;
    for (int i = 0; i < a.size(); i++) {
        auto difference = a.at(i) - b.at(i);
        sum += sqrt((difference.x * difference.x) + (difference.y * difference.y));
    }
    return sum;
}

/**
 * @function main
 */
int main(void)
{
    VideoCapture capture;
    Mat frame;

    //-- 1. Load the cascade
    if (!face_cascade.load("lbpcascade_frontalface.xml")) {
        return -9;
    };
    if (!eyes_cascade.load("haarcascade_eye_tree_eyeglasses.xml")) {
        return -8;
    };
    glasses = imread("dwi.png", -1);
    // 	std::cout << "C:" << glasses.channels() << "\n";
    capture.open(-1);
    if (!capture.isOpened()) {
        return -7;
    }

    list<vector<Point2f>> detection_history;
    int iteration = 0;
    vector<Point2f> avg = {{0, 0}, {0, 0}, {0, 0}, {0, 0}};
    while (capture.read(frame)) {
        if (frame.empty()) return -1;
        if ((iteration % 2) == 0) {
            auto detected_faces = detectFacesLocations(frame);

            avg = {{0, 0}, {0, 0}, {0, 0}, {0, 0}};
            if (detection_history.size() > 0) {
                for (auto e : detection_history) {
                    for (int i = 0; i < 4; i++)
                        avg[i] = avg[i] + e[i];
                }
                for (int i = 0; i < 4; i++) {
                    avg[i].x = avg[i].x / detection_history.size();
                    avg[i].y = avg[i].y / detection_history.size();
                }
            }

            if (detected_faces.size() > 0) {
                if (detection_history.size() > 0) {
                    sort(detected_faces.begin(), detected_faces.end(), [&](auto a, auto b) {
                        return rectDiastance(a, detection_history.back()) < rectDiastance(b, detection_history.back());
                    });

					for (auto a : detected_faces) {
						std::cout << rectDiastance(a, detection_history.back()) << " ";
					}
					std::cout << std::endl;
                }
                detection_history.push_back(detected_faces.at(0));
            } else if (detection_history.size()) {
                detection_history.pop_front();
            }
            if (detection_history.size() > 5) {
                detection_history.pop_front();
            }
        }
        imageOverImageBGRA(glasses.clone(), frame, avg);
        cv::flip(frame, frame, 1);
        imshow("DWI", frame);

        if ((waitKey(1) & 0x0ff) == 27) return 0;
        iteration++;
    }
    return 0;
}

// funkcja nakladajaca obraz z przezroczystoscia
// w oparciu o http://dsynflo.blogspot.in/2014/08/simplar-2-99-lines-of-code-for.html
void imageOverImageBGRA(const Mat& srcMat, Mat& dstMat, const vector<Point2f>& dstFrameCoordinates)
{
    if (srcMat.channels() != 4) throw "Nakladam tylko obrazy BGRA";

    // tylko kanal alpha
    vector<Mat> rgbaChannels(4);
    Mat srcAlphaMask(srcMat.rows, srcMat.cols, srcMat.type());
    split(srcMat, rgbaChannels);
    rgbaChannels = {rgbaChannels[3], rgbaChannels[3], rgbaChannels[3]};
    merge(rgbaChannels, srcAlphaMask);

    // wspolrzedne punktow z obrazu nakladanego
    vector<Point2f> srcFrameCoordinates = {{0, 0}, {(float)srcMat.cols, 0}, {(float)srcMat.cols, (float)srcMat.rows}, {0, (float)srcMat.rows}};
    Mat warp_matrix = getPerspectiveTransform(srcFrameCoordinates, dstFrameCoordinates);

    Mat cpy_img(dstMat.rows, dstMat.cols, dstMat.type());
    warpPerspective(srcAlphaMask, cpy_img, warp_matrix, Size(cpy_img.cols, cpy_img.rows));
    Mat neg_img(dstMat.rows, dstMat.cols, dstMat.type());
    warpPerspective(srcMat, neg_img, warp_matrix, Size(neg_img.cols, neg_img.rows));
    dstMat = dstMat - cpy_img;

    cvtColor(neg_img, neg_img, COLOR_BGRA2BGR);
    cpy_img = cpy_img / 255;
    neg_img = neg_img.mul(cpy_img);
    dstMat = dstMat + neg_img;
}

std::vector<vector<Point2f>> detectFacesLocations(Mat frame_orig)
{
    Mat frame;
    resize(frame_orig, frame, Size(frame_orig.cols / 2, frame_orig.rows / 2));

    std::vector<Rect> faces;
    std::vector<vector<Point2f>> detection_results;

    Mat frame_gray;

    cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
    equalizeHist(frame_gray, frame_gray);

    // detect face
    face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0, Size(6, 6));

    for (size_t i = 0; i < faces.size(); i++) {
        Mat faceROI = frame_gray(faces[i]); // range of interest
        // imshow ( "ROI_" + to_string(i), faceROI );
        std::vector<Rect> eyes;
        eyes_cascade.detectMultiScale(faceROI, eyes, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(2, 2));
        if (eyes.size() > 0) {
            vector<Point2f> dst = {
                Point2f((faces[i].x) * 2, (faces[i].y + faces[i].height * 5 / 20) * 2),
                Point2f((faces[i].x + faces[i].width) * 2, (faces[i].y + faces[i].height * 5 / 20) * 2),
                Point2f((faces[i].x + faces[i].width) * 2, (faces[i].y + faces[i].height * 5 / 20 + faces[i].height * 3 / 10) * 2),
                Point2f((faces[i].x) * 2, (faces[i].y + faces[i].height * 5 / 20 + faces[i].height * 3 / 10) * 2)};
            detection_results.push_back(dst);
        }
    }
    return detection_results;
}
