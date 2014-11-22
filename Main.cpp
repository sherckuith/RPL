#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv/highgui.h>
#include <iostream>

using namespace std;
using namespace cv;

int main(int argc, char * argv[]){
    Mat Im = imread("image000012.bmp");
    namedWindow("Original",WINDOW_AUTOSIZE);
    imshow("Original",Im);

    // Canny del tiron
    Mat Im_Edges;
    int vApertSize = 2;
    int Edg_Thresh = 1;
    int Edg_Thresh2 = 80;
    Canny(Im,Im_Edges,Edg_Thresh,Edg_Thresh2);
    namedWindow("Edge",WINDOW_AUTOSIZE);
    imshow("Edge",Im_Edges);

    // HoughLines()
    vector<Vec2f> Lines;
    //int Hough_thresh = 250;
    int Hough_thresh = 100;
    cout <<"Hough Threshold = " << Hough_thresh <<endl;
    HoughLines(Im_Edges,Lines,1,CV_PI/180.f,Hough_thresh);
    // Draw lines
    Mat img_show = Im.clone();
        float rho = Lines[1][0];
        float theta = Lines[1][1];
        cout << "Rho = "<<rho<<" Theta = "<<theta<<endl;
        double a = cos(theta), b = sin(theta);
        double x0 = a * rho, y0 = b * rho;
        Point pt1(cvRound(x0 + 1000 * (-b)), cvRound(y0 + 1000 * (a)));
        Point pt2(cvRound(x0 - 1000 * (-b)), cvRound(y0 - 1000 * (a)));
        line(img_show, pt1, pt2, Scalar(0, 0, 255));
    namedWindow("Shapes");
    imshow("Shapes", img_show);

    while((char)waitKey(1)!='q'){}
}
