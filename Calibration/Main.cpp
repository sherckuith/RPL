#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv/highgui.h>
#include <iostream>

using namespace std;
using namespace cv;

int main(int argc, char * argv[]){
    bool FLAG = true;
    Mat Im;
    if (argc == 1){
        Im = imread("00.bmp");
    }    else{
        Im = imread(argv[1]);
    }

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
        float rho = Lines[0][0];
        float theta = Lines[0][1];
        cout << "Rho = "<<rho<<" Theta = "<<theta<<endl;
        double a = cos(theta), b = sin(theta);
        double x0 = a * rho, y0 = b * rho;
        Point pt1(cvRound(x0 + 1000 * (-b)), cvRound(y0 + 1000 * (a)));
        Point pt2(cvRound(x0 - 1000 * (-b)), cvRound(y0 - 1000 * (a)));
        line(img_show, pt1, pt2, Scalar(0, 0, 255));
    namedWindow("Shapes");
    imshow("Shapes", img_show);




    //Rotate
    Point center = Point(Im_Edges.cols/2, Im_Edges.rows/2);
    Mat Im_Rot;
    double ang= theta;
    double scale = 1;
    if (ang>=0 && ang<(CV_PI/2)){
    cout<<"hola"<<endl;
        ang = -ang/CV_PI*180;
    }else{
    cout<<"hola2"<<endl;
        //ang = (CV_PI/2-ang)/CV_PI*180;
        ang = ang-CV_PI/2;
        ang = ang/CV_PI*180;
    }
    cout <<"Rotating "<<ang<<" degrees"<<endl;
    Mat rot_mat = getRotationMatrix2D(center,ang,scale);
    cout <<"El fondo es "<< Im.at<Vec3b>(Im.rows-1,Im.cols-1)<<endl;
    warpAffine(Im,Im_Rot,rot_mat,Im.size(),INTER_LINEAR,BORDER_CONSTANT,Im.at<Vec3b>(0,0));
    namedWindow("Edges Rotated",WINDOW_AUTOSIZE);
    imshow("Edges Rotated", Im_Rot);




    //Detecting Corners
    Mat Im_BW, Im_BW_eq, Im_BW_Thres;
    cvtColor(Im_Rot, Im_BW,CV_RGB2GRAY);
    //cvtColor(Im, Im_BW,CV_RGB2GRAY);

    int maxCorners = 11;
    float quality = 0.01;
    int min_distance = 50;
    vector<Point2d> corners;
    goodFeaturesToTrack(Im_BW,corners,maxCorners,quality,min_distance);



    //Draw the corners as little circles
    vector<int>Esquinas(11);
    vector<bool>Used(11);
    vector<Vec3i>Distt(2);
    for (int i=0;i<2;i++){
        Distt[i][0]=-99;
        Distt[i][1]=-99;
        Distt[i][2]=-99;
    }
    for (int i=0;i<11;i++){
        Esquinas[i] = i;
        Used[i]=false;
    }

    Mat Im_Edges_Rot_Corn = Im_BW.clone();
    int WHO=0,WHOF=0,DIST = 0;
    int PINTADO = 0;
    int X0=0,Y0=0,XF=0,YF=0;
    int k = 0;
    while (k<6 && Distt[1][2]==-99){
        WHO = 0;
        WHOF = 0;
        DIST = 0;
        for (int i = 0;i<corners.size(); i++){
            //circle(Im_Edges_Rot_Corn, corners[i],4,CV_RGB(155,0,0),-1);
            for (int j = i;j<corners.size();j++){
                //If they are not alligned, and the middle pizel is gray
                if ( abs(corners[i].x-corners[j].x)>250 && abs(corners[i].y-corners[j].y)>250 && abs(abs(corners[i].x-corners[j].x) - abs(corners[i].y-corners[j].y))>10){
                    if (sqrt((corners[i].x-corners[j].x)*(corners[i].x-corners[j].x)+(corners[i].y-corners[j].y)*(corners[i].y-corners[j].y))>DIST){
                        if(Used[j]==false && Used[i]==false){
                            DIST = sqrt((corners[i].x-corners[j].x)*(corners[i].x-corners[j].x)+(corners[i].y-corners[j].y)*(corners[i].y-corners[j].y));
                            WHOF = i;
                            WHO = j;
                        }
                    }
                }
            }

        }
        if (abs(DIST-Distt[0][2])<20 || abs(DIST-Distt[1][2])<20){
            Used[WHOF]=true;
            Used[WHO]=true;
        }else{
            if (Distt[0][2]==-99){
                Distt[0][0]=WHO;
                Distt[0][1]=WHOF;
                Distt[0][2]=DIST;
                X0 = min(corners[WHO].x,corners[WHOF].x);
                Y0 = min(corners[WHO].y,corners[WHOF].y);
                XF = max(corners[WHO].x,corners[WHOF].x);
                YF = max(corners[WHO].y,corners[WHOF].y);
                //rectangle(Im_Edges_Rot_Corn,Rect(X0,Y0,XF-X0,YF-Y0),Scalar(0,0,255),2);
                rectangle(Im_Edges_Rot_Corn,corners[WHO],corners[WHOF],Scalar(0,0,255),2);
                Used[WHOF]=true;
                Used[WHO]=true;
            }else{
                if(Distt[1][2]==-99 && (abs(corners[WHO].x-corners[WHOF].x)<(XF-X0-48))){
                cout << " "<<abs(corners[WHO].x-corners[WHOF].x) << " " << XF-X0<<endl;
                    Distt[1][2]=DIST;
                    Distt[1][0]=WHO;
                    Distt[1][1]=WHOF;
                    X0 = min(corners[WHO].x,corners[WHOF].x);
                    Y0 = min(corners[WHO].y,corners[WHOF].y);
                    XF = max(corners[WHO].x,corners[WHOF].x);
                    YF = max(corners[WHO].y,corners[WHOF].y);
                    rectangle(Im_Edges_Rot_Corn,Rect(X0,Y0,XF-X0,YF-Y0),Scalar(0,0,255),2);
                    Used[WHOF]=true;
                    Used[WHO]=true;
                }
            }
            //line(Im_Edges_Rot_Corn,corners[WHOF],corners[WHO],Scalar(0, 0, 255),2);
            namedWindow("Corners");
            imshow("Corners",Im_Edges_Rot_Corn);
            //while((char)waitKey(1)!='n'){}
        }
    k++;
    }

    //I assume that the second square is the good one.
    // So that I
    X0 = min(corners[Distt[1][0]].x,corners[Distt[1][1]].x);
    Y0 = min(corners[Distt[1][0]].y,corners[Distt[1][1]].y);
    XF = max(corners[Distt[1][0]].x,corners[Distt[1][1]].x);
    YF = max(corners[Distt[1][0]].y,corners[Distt[1][1]].y);
    Rect rec(X0+5,Y0+5,XF-X0-10,YF-Y0-10);
    Mat Mask = Im_BW+255;
    Mask(rec)=1;
    if(FLAG==true){
        namedWindow("Mask");
        imshow("Mask",Mask);
    }
    Mat Im_Crop;
    multiply(Im_BW,Mask,Im_Crop);
    if (FLAG == true){
        namedWindow("Image Cropped");
        imshow("Image Cropped",Im_Crop);
    }

    //Threshold nuevos
    Mat Im_Crop_Thres,Temp;

    threshold(Im_Crop,Im_Crop_Thres,190,255,THRESH_BINARY);
    Mat Im_Crop_Thres_Invert;
    if (FLAG==true){
        namedWindow("Image Cropped and Threshold");
        Temp = Im_Crop_Thres+255;
        subtract(Temp,Im_Crop_Thres,Im_Crop_Thres_Invert);
        imshow("Image Cropped and Threshold",Im_Crop_Thres_Invert);
    }



    //Características
    float Factor = 0;
    Factor = 20/(float)abs(YF-Y0);
    cout << "20 cm y "<<abs(YF-Y0)<<" cm. Factor" << Factor << endl;

    double AREA = 0;
    for (int i = 0;i<Im_Crop_Thres_Invert.rows;i++){
        for (int j = 0;j<Im_Crop_Thres_Invert.cols;j++){
            if (Im_Crop_Thres_Invert.at<uchar>(i,j)>30)
            AREA =AREA+ 1;
        }
    }
    cout << "Area = " << AREA*Factor*Factor << endl;

    float PERIM = 0;
    Mat Im_Crop_Thres_Invert_Edge;
    Canny(Im_Crop_Thres_Invert,Im_Crop_Thres_Invert_Edge,100,150);
    if (FLAG == true){
        namedWindow("Canny Invert");
        imshow("Canny Invert",Im_Crop_Thres_Invert_Edge);
    }
    for (int i = 0;i<Im_Crop_Thres_Invert.rows;i++){
        for (int j = 0;j<Im_Crop_Thres_Invert.cols;j++){
            if (Im_Crop_Thres_Invert_Edge.at<uchar>(i,j)>200)
            PERIM =PERIM+ 1;
        }
    }
    cout << "Perim = " << PERIM*Factor << endl;


    while((char)waitKey(1)!='q'){}
}
