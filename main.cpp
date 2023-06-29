#include "opencv2/opencv.hpp"
#include <opencv2/highgui/highgui.hpp>
#include "iostream"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
using namespace cv;
using namespace std;
RNG rng(12345);
int main(int, char**) {
    
    cv::VideoCapture camera(0);
    if (!camera.isOpened()) {
        std::cerr << "ERROR: Could not open camera" << std::endl;
        return 1;
    }

    
    cv::namedWindow("org", cv::WindowFlags::WINDOW_AUTOSIZE);

    
    Mat img;
    Mat img_ROI;
    Mat img_gray;
    Mat img_thr;

  
   
   while (1) {
        
        camera.read(img);
        Rect region_of_interest = Rect(300,100,300,300);
        img_ROI = img(region_of_interest);
        //rectangle(img, Point(500, 100), Point(270, 500), (0, 0, 255), 3, 8, 0);
        cvtColor(img_ROI, img_gray, COLOR_RGB2GRAY);
        GaussianBlur(img_gray, img_gray, Size(19, 19), 0.0, 0);
        threshold(img_gray, img_thr, 50, 209, THRESH_BINARY_INV + THRESH_OTSU);


        vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;
        findContours(img_thr, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
      /*  for (size_t i = 0; i < contours.size(); i++)
        {
            
            drawContours(img_ROI, contours, (int)i, (255,0,0), 2, LINE_8, hierarchy, 0);
        }*/
        vector<vector<Point> > contours_poly(contours.size());
        vector<Rect> boundRect(contours.size());
        vector<Point2f>centers(contours.size());
        vector<float>radius(contours.size());
        for (size_t i = 0; i < contours.size(); i++)
        {
            approxPolyDP(contours[i], contours_poly[i], 3, true);
            boundRect[i] = boundingRect(contours_poly[i]);
            minEnclosingCircle(contours_poly[i], centers[i], radius[i]);
        }
       
        for (size_t i = 0; i < contours.size(); i++)
        {
            Scalar color = Scalar(rng.uniform(0, 256), rng.uniform(0, 256), rng.uniform(0, 256));
            drawContours(img_ROI, contours_poly, (int)i, color);
            rectangle(img_ROI, boundRect[i].tl(), boundRect[i].br(), color, 2);
            circle(img_ROI, centers[i], (int)radius[i], color, 2);
            circle(img_ROI, centers[i], 4, color, 2);

        }
        vector<vector<Point> >hull(contours.size());
        for (size_t i = 0; i < contours.size(); i++)
        {
            convexHull(contours[i], hull[i]);
        }
        
        for (size_t i = 0; i < contours.size(); i++)
        {
            Scalar color = Scalar(rng.uniform(0, 256), rng.uniform(0, 256), rng.uniform(0, 256));
            drawContours(img_ROI, contours, (int)i, color);
            drawContours(img_ROI, hull, (int)i, color);
        }

        
       if (cv::waitKey(30) == 27) {
            return -1;
        }
       cv::imshow("org", img_ROI);
    }
    return 0;
}