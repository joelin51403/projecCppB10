//
//  main.cpp
//  opencvtest0527
//
//  Created by 凌中漢 on 2017/5/27.
//  Copyright © 2017年 凌中漢. All rights reserved.
//

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
//#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

double innerD(double n, double m, double x1, double x2){
    double sol;
    sol = (m * x2 + n * x1)/(m+n);
    
    return sol;
}

int main(){
    string v1 = "/Users/joelin/Documents/檢舉/2017_0523_132430.MP4";//路徑
    string v2 = "/Users/joelin/Documents/檢舉/2017_0524_085719.MP4";
    string v3 = "/Users/joelin/Documents/檢舉/2017_0525_095823.MP4";
    string v4 = "/Users/joelin/Documents/檢舉/2017_0525_101518.MP4";
    
    VideoCapture video(v2);
    //VideoCapture video("/Users/joelin/Documents/testcv/2017_0522_095851.MP4");
    Size videoSize = Size((int)video.get(CV_CAP_PROP_FRAME_WIDTH),(int)video.get(CV_CAP_PROP_FRAME_HEIGHT));
    namedWindow("video demo", WINDOW_NORMAL);
    int key;
    Mat src;
    Mat hsv;
    Mat mask_yellow;
    Mat mask_white;
    Mat mask_line;
    Mat gray;
    Mat gray_hsv;
    Mat mask_yw;
    Mat mask_yw_image;
    Mat dst;
    Mat gauss_gray;
    int CutHeight = 550;
    int CutWidth = 1200;
    bool ifLeftLine = 0, ifRightLine = 0;
    int xSite = 168, ySite = 126;
    int xRange = 70, yRange = 70;
    Point pt1L, pt2L;//儲存前一次線段
    Point pt1R, pt2R;
    
    
    Vec4i mainL;
    CvRect Rect1;
    Rect1=cvRect(550,750,720,130);
    float rho, theta;
    float rhoR = 7,thetaR  = 0.4;
    
    
    //vector<Vec4i> lines;
    vector<Vec2f> lines;
    
    int low_threshold = 50;
    int high_threshold = 150;
    
    while(true){
        
        video >> src;
        if( src.empty()){
            break;
        }
        Mat mask_line = src;
        Mat srcROI = src(Rect1);
        cvtColor(srcROI, gray_hsv, CV_BGR2GRAY);
        cvtColor(srcROI,hsv,CV_BGR2HSV);
        //cvtColor(gray, gray_hsv, CV_GRAY2HSV);
        inRange(hsv,Scalar(20,100,100) , Scalar(30, 255, 255), mask_yellow);
        inRange(gray_hsv,150  , 230 , mask_white);
        //inRange(hsv,Scalar(20,100,100) , Scalar(30, 255, 255), mask_white);
        bitwise_or( mask_yellow, mask_white, mask_yw);
        bitwise_and(gray_hsv,mask_yw,mask_yw_image);
        GaussianBlur( mask_yw_image, gauss_gray, Size( 5, 5), 0, 0 );
        Canny(gauss_gray,gauss_gray,low_threshold,high_threshold,3);
        
        
        Canny(mask_yw, dst, 50, 200, 3);
        
        HoughLines(dst, lines, 2, CV_PI/180, 50);
        //HoughLinesP(dst, lines, 2, CV_PI/180, 15, 10, 20);
        for( size_t i = 0; i < lines.size(); i++ )
        {
            /*Vec4i l = lines[i];
             int x = l[2] - l[0], y = l[1] - l[3];
             //Vec4i l = lines[i];
             if(l[0] - l[1] ){
             
             }
             cout<< l[0]<<endl;
             cout<< l[1]<<endl;
             cout<< l[2]<<endl;
             cout<< l[3]<<endl;
             cout<<"x = "<<x<<endl;
             cout<<"y = "<<y<<endl;
             
             if( (x > xSite - xRange && x < xSite + xRange) && (y > ySite - yRange && y < ySite + yRange) && ifLeftLine == 0)
             {
             
             line( src, Point(l[0]+550, l[1]+750) , Point(l[2]+550, l[3]+750), Scalar(0,0,255), 3, CV_AA);
             ifLeftLine = 1;
             }*/
            for( size_t i = 0; i < lines.size()&& ifLeftLine == 0; i++ )
            {
                
                float rho = lines[i][0], theta = lines[i][1];
                //cout<< "rho = "<<rho << endl;
                //cout<< "theta = "<<theta << endl;
                
                if((rho > 165 - rhoR) && (rho < 165 + rhoR) && (theta > 0.95 - thetaR) && (theta < 0.95 + thetaR))
                {
                    Point pt1, pt2;
                    double m;
                    double a = cos(theta), b = sin(theta);
                    double x0 = a*rho, y0 = b*rho;
                    pt1.x = cvRound(x0 + 1000*(-b))+550;
                    pt1.y = cvRound(y0 + 1000*(a))+750;
                    pt2.x = cvRound(x0 - 1000*(-b))+550;
                    pt2.y = cvRound(y0 - 1000*(a))+750;
                    //double m =
                    
                    
                    pt2.x = innerD(2.0, 3.3, pt1.x, pt2.x);
                    pt2.y = innerD(2.0, 3.3, pt1.y, pt2.y)-4;
                    // if(pt2.x < 350){
                    line( src, pt1, pt2, Scalar(0,0,255), 3, CV_AA);
                    ifLeftLine = 1;
                    
                    pt1L.x = pt1.x;
                    pt1L.y = pt1.y;
                    pt2L.x = pt2.x;
                    pt2L.y = pt2.y;
                    
                    
                    
                    //}
                    
                }
                
            }
            
            if(ifLeftLine == 0)
            {
                line( src, pt1L, pt2L, Scalar(0,0,255), 3, CV_AA);
                
            }
            
            for( size_t i = 0; i < lines.size()&& ifRightLine == 0; i++ )
            {
                
                float rho = lines[i][0], theta = lines[i][1];
                //cout<< "rho = "<<rho << endl;
                //cout<< "theta = "<<theta << endl;
                
                if((rho > -175 - rhoR) && (rho < -175 + rhoR) && (theta > 2.0- thetaR) && (theta < 2.0 + thetaR))
                {
                    Point pt1, pt2;
                    double a = cos(theta), b = sin(theta);
                    double x0 = a*rho, y0 = b*rho;
                    pt1.x = cvRound(x0 + 1000*(-b))+550;
                    pt1.y = cvRound(y0 + 1000*(a))+750;
                    pt2.x = cvRound(x0 - 1000*(-b))+550;
                    pt2.y = cvRound(y0 - 1000*(a))+750;
                    
                    pt1.x = innerD(1.0, 2.0, pt1.x, pt2.x);
                    pt1.y = innerD(1.0, 2.0, pt1.y, pt2.y)-4;
                    if(pt1.y > 700){
                        line( src, pt1, pt2, Scalar(0,0,255), 3, CV_AA);
                        ifRightLine = 1;
                        
                        pt1R.x = pt1.x;
                        pt1R.y = pt1.y;
                        pt2R.x = pt2.x;
                        pt2R.y = pt2.y;
                        /*cout<<"1.x = "<<pt1.x<<endl;
                         cout<<"1.y = "<<pt1.y<<endl;
                         cout<<"2.x = "<<pt2.x<<endl;
                         cout<<"2.y = "<<pt2.y<<endl<<endl;*/
                    }
                }
                
            }
            if(ifRightLine == 0)
            {
                line( src, pt1R, pt2R, Scalar(0,0,255), 3, CV_AA);
                
            }
            //cout<< endl<<l[3];
            
            // line( src, Point(l[0]+550, l[1]+750) , Point(l[2]+550, l[3]+750), Scalar(0,0,255), 3, CV_AA);
            
            
            
        }
        
        ifLeftLine = 0;
        ifRightLine = 0;
        
        
        //imshow("mask_yellow demo", mask_yellow);
        //imshow("mask_white demo", mask_white);
        //imshow("gray demo", gray_hsv);
        //imshow("dst", dst);
        //imshow("mask_yw_image", gauss_gray);
        
        
        imshow("video demo2", src);
        waitKey(1);
        
        
    }
    return 0;
}
