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
    int i;
    string v1 = "/Volumes/Sony_64GU/原始影片/65測試2.mp4";//路徑
    string v2 = "/Volumes/Joe_大usb16G/IMG_0135.MOV";//路徑
    VideoCapture video(v1);
    double vidoeFramNum = video.get(CV_CAP_PROP_FRAME_COUNT);
    int fram10 = 0;
    double framCountRtio = vidoeFramNum/100, currentFram = 0;
    Mat src;
    Point l,r;
    //Size videoSize = Size((int)video.get(CV_CAP_PROP_FRAME_WIDTH),(int)video.get(CV_CAP_PROP_FRAME_HEIGHT));
    Size videoSize = Size(1920,1080);

    VideoWriter writer;
    //writer.open("/Users/joelin/Documents/專題B10/vedio/VideoTest.avi", CV_FOURCC('M', 'J', 'P', 'G'), 30, videoSize);
    writer.open("/Users/joelin/Documents/專題B10/vedio/VideoTest1122.avi", CV_FOURCC('M', 'J', 'P', 'G'), 30, videoSize);
    cout<<"START !!"<<endl;
    Car car[10];
    while(true){
        currentFram++;
        video >> src;
        if( src.empty()){
            break;
        }
        resize(src, src, Size(1920,1080));
        
        detectCar *detectCar1 = new detectCar(src, recordnum);
        
        for(i = 0; i < 10; i++){
            if(carDetectStruct[i].flag< 7){
                car[i].carSetPoint(carDetectStruct[i].l, carDetectStruct[i].r);
                Point showL, showR;
                
                //line(src, car[i].l, car[i].r, Scalar(0,0,255));
                cout<<car[i].l<<" "<<car[i].r<<endl;
            }
        }
        
        writer.write(src);
        resize(src, src, Size(src.cols/2, src.rows/2));
        imshow("show", src);
        waitKey(1);
        
        //計算進度條
        if(currentFram > framCountRtio ){
            if(fram10%10 == 0){ //10倍
                cout<<fram10<<"%";
                fram10++;
            }
            else{//非10倍
                cout<<"/";
                fram10++;
            }
            framCountRtio += vidoeFramNum/100;
        }
        
        
    }
    cout<<"100%"<<endl<<"DONE!!";
    return 0;
}
