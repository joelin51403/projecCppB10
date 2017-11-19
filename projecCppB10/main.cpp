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
#include "laneDetect.hpp"
#include "DetectCar.hpp"
using namespace cv;
using namespace std;

int main(int argc, char *argv[])
{
    string v1 = "/Users/joelin/Documents/專題B10/vedio/65測試2.mp4";//路徑
    VideoCapture video(v1);
    Mat src;
    Point l,r;
    
    l.x = 0; l.y = 50;
    r.x = 100; r.y = 50;
    
    while(true){
        
        video >> src;
        if( src.empty()){
            break;
        }
        resize(src, src, Size(1920,1080));
        
        //Car *car1 = new Car(l,r);
        detectCar *detectCar1 = new detectCar(src, recordnum);
        //Lane *detect = new Lane(src,*car1);
        
        //namedWindow("video demo", CV_WINDOW_NORMAL);
        
        resize(src, src, Size(src.cols/2, src.rows/2)); // to half size or even smaller
        namedWindow( "video demo",CV_WINDOW_AUTOSIZE);
        imshow("video demo", src);
        
        waitKey(1);
        
        
        
        l.x=l.x+15;
        r.x=r.x+15;
        //cout<< l.x<<endl;
        recordnum++;
        if( recordnum > 5){
            recordnum = 0;
        }
        if(l.x >1400){
            l.x = 0; l.y = 50;
            r.x = 100; r.y = 50;
        }
        
        //        delete detect;
        delete detectCar1;
        //        delete car1;
        
        //cout<< "x "<< distance[0].x<<endl;
    }
    cout<<"test github";
    return 0;
}
