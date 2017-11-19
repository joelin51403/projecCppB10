//
//  DetectCar.hpp
//  car_cascade
//
//  Created by  austin on 2017/11/12.
//  Copyright © 2017年 B10. All rights reserved.
//
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <time.h>
#include <iostream>
#include <stdio.h>
using namespace std;
using namespace cv;


#ifndef DetectCar_hpp
#define DetectCar_hpp

class detectCar
{
public:
    detectCar(Mat frameROI, int recordnum)
    {
        CvRect ROI = CvRect(roiCar[0],roiCar[1],roiCar[2],roiCar[3]);
        frameROI = frameROI(ROI);
        detectAndDisplay( frameROI, recordnum );
    }
    
    void implement( Mat frame , int record , int car_Num )
    {
        Mat mat;
        Mat matRed;
        frame.copyTo(mat);
        frame.copyTo(matRed);
        //int real_x[2] = {};
        
        for( int y=0 ; y<frame.rows ; y++ ){
            for( int x=0 ; x<frame.cols ; x++ ){
                
                float D= (frame.at<Vec3b>(y,x)[0] - targetBlue) *  (frame.at<Vec3b>(y,x)[0] - targetBlue) +
                (frame.at<Vec3b>(y,x)[1] - targetGreen) * (frame.at<Vec3b>(y,x)[1] -targetGreen) +
                (frame.at<Vec3b>(y,x)[2] - targetRed) * (frame.at<Vec3b>(y,x)[2]- targetRed);
                if (sqrt(D) < TH1){
                    mat.at<Vec3b>(y,x)[0] = 0;
                    mat.at<Vec3b>(y,x)[1] = 255;
                    mat.at<Vec3b>(y,x)[2] = 255;
                }
                else{
                    mat.at<Vec3b>(y,x)[0] = 0;
                    mat.at<Vec3b>(y,x)[1] = 0;
                    mat.at<Vec3b>(y,x)[2] = 0;
                }
            }
        }
        
        imshow("?????????", mat);
        switch(record)
        {
            case 1:
                orignal[car_Num]=detectyellow(mat) ;
                break ;
            case 2:
                break;
            case 3:
                break;
            case 4:
                yellow[car_Num]=detectyellow(mat) ;
                break ;
            default:
                if( yellow[car_Num] > orignal[car_Num] ){
                    if( yellow[car_Num] - orignal[car_Num] > 30 )
                        printf("打燈囉\n") ;
                }
                else
                    if( orignal[car_Num] - yellow[car_Num] > 30 )
                        printf("打燈囉\n") ;
                printf("%d %d\n",orignal[car_Num],yellow[car_Num]) ;
        }
        for( int y=0 ; y<frame.rows ; y++ ){
            for( int x=0 ; x<frame.cols ; x++ ){
                float D= (frame.at<Vec3b>(y,x)[0] - refBlue) *  (frame.at<Vec3b>(y,x)[0] - refBlue) +
                (frame.at<Vec3b>(y,x)[1] - refGreen) * (frame.at<Vec3b>(y,x)[1] -refGreen) +
                (frame.at<Vec3b>(y,x)[2] - refRed) * (frame.at<Vec3b>(y,x)[2]- refRed);
                if (sqrt(D) < TH1){
                    matRed.at<Vec3b>(y,x)[0] = 0;
                    matRed.at<Vec3b>(y,x)[1] = 0;
                    matRed.at<Vec3b>(y,x)[2] = 255;
                }
                else{
                    matRed.at<Vec3b>(y,x)[0] = 0;
                    matRed.at<Vec3b>(y,x)[1] = 0;
                    matRed.at<Vec3b>(y,x)[2] = 0;
                }
            }
        }
        //*real_x = detectRed( frame, matRed );
        detectRed( frame, matRed );
        //return *real_x;
    }
    
    void detectRed( Mat frame, Mat matred )
    {
        int record = 0;
        int x2, x3, y2, y3;//x2 y2 燈左邊座標，x3 y3燈右邊座標
        //int real_x[2] = {};
        
        if( record == 0 )
            for( int x=0; x<matred.cols; x++ ){
                for(int y=0; y<matred.rows; y++){
                    if( matred.at<Vec3b>(y,x)[0] == 0 && matred.at<Vec3b>(y,x)[1] == 0 && matred.at<Vec3b>(y,x)[2] == 255 ){
                        record++;
                        x2 = x ;
                        y2 = y ;
                        break;
                    }
                }
                if( record == 1 )
                    break;
                
            }
        if( record == 1 )
            for( int x=matred.cols; x>0 ; x-- ){
                for(int y=0; y<matred.rows; y++){
                    if( matred.at<Vec3b>(y,x)[0] == 0 && matred.at<Vec3b>(y,x)[1] == 0 && matred.at<Vec3b>(y,x)[2] == 255 ){
                        record++;
                        x3 = x ;
                        y3 = y ;
                        break;
                    }
                }
                if( record == 2 )
                    break;
            }
        if( record == 2 )
            for( int y=0; y<matred.rows; y++ ){
                for(int x=0; x<matred.cols; x++){
                    if( matred.at<Vec3b>(y,x)[0] == 0 && matred.at<Vec3b>(y,x)[1] == 0 && matred.at<Vec3b>(y,x)[2] == 255 ){
                        record++;
                        Point pt1(x2,y);
                        Point pt2(x3,y3+(y2-y));
//                        cout << pt1 << endl;
//                        cout << pt2 << endl;
//                        cout << record << endl;
                        rectangle( frame , pt1 , pt2 , Scalar(255,0,0)) ;
                        real_x[0] = x2;
                        real_x[1] = x3;
                        imshow("A",frame);
                        //waitKey(1);
                        break;
                    }
                }
                if( record == 3 )
                    break;
            }
        if( record == 0 )
            printf("沒偵測到車燈\n");
        //return *real_x;
    }
    
    int detectyellow( Mat frame )
    {
        int yellow=0 ;
        for ( int y=0; y<frame.rows; y++ ){
            for ( int x=0; x<frame.cols; x++ ){
                if( frame.at<Vec3b>(y,x)[2] == 255 && frame.at<Vec3b>(y,x)[1] == 255 ){
                    yellow++ ;
                }
            }
            
        }//黃色
        return yellow ;
        
    }
    
    
    void detectAndDisplay( Mat frame, int recordnum )
    {
        car_cascade.load(car_cascade_name);
        std::vector<Rect> car;
        Mat frame_gray;
        cvtColor( frame, frame_gray, CV_BGR2GRAY );
        equalizeHist( frame_gray, frame_gray );
        int real_height = 100;
        
        // 計算偵測車子的執行時間
        clock_t s,t;
        s = clock();
        //-- Detect cars
        car_cascade.detectMultiScale( frame_gray, car, 1.1, 30, 0|CV_HAAR_SCALE_IMAGE, Size(0,0) );
        t = clock();
//        cout << "detect speed : " << (double) (t-s) / CLOCKS_PER_SEC << endl;
        
        // 偵測到車子大概位置後，擴大偵測的邊界，並再次切出ROI
        for( int i = 0; i < car.size(); i++ )
        {
            //int flag_width = 0, flag_height = 0;
            int x1 = car[i].x, y1 = car[i].y, x2 = car[i].x + car[i].width, y2 = car[i].y + car[i].height;
            real_x[0] = car[i].x;
            real_x[1] = car[i].x + car[i].width;
            /*if (car[i].x - car[i].width * 0.3 > 0)
             x1 = car[i].x - car[i].width * 0.30;
             else{
             x1 = 0;
             //flag_width = 1;
             }
             if (car[i].y - car[i].height * 0.3 > 0)
             y1 = car[i].y - car[i].height * 0.29;
             else{
             y1 = 0;
             //flag_height = 1;
             }
             if(car[i].x + car[i].width * 1.3 < 1920)
             car[i].width = car[i].width * 1.29;
             else
             car[i].width = 1420 - car[i].x + car[i].width * 1.25;*/
            x1 -= car[i].width * 0.3;
            if(x1 < 0)
                x1 = 0;
            x2 += car[i].width * 0.3;
            if(x2 > 1920)
                x2 = 1920;
            /*y1 -= car[i].height * 0.3;
             if(y1 < 0)
             y1 = 0;
             y2 += car[i].height * 0.3;
             if(y2 > 380)
             y2 = 380;*/
            
            // 切ROI
            CvRect ROI = cvRect(x1, y1, x2 - x1, y2 - y1);
            //CvRect ROI = cvRect(car[i].x, car[i].y , car[i].width , car[i].height );
            
            Mat frameROI = frame(ROI);
            //frameROI.copyTo();
            
            //equalizeHist(Red, Red);
            
            //if(i == 0)
            //imshow("1", frameROI);
            
            // 三個Mat用來表示相對的數值
            Mat dst_vertical(frameROI.rows,256,CV_8UC3,Scalar(255, 255, 255)); // 測灰階，測垂直線
            Mat dst_horizontal(256, frameROI.cols,CV_8UC3,Scalar(255, 255, 255)); // 測灰階，測水平線
            Mat dst_red(256, frameROI.cols, CV_8UC3, Scalar(255, 255, 255)); // 測紅色，水平線
            
            real_height = vertical_histogram(frame, frameROI, dst_vertical, frameROI.cols / 2, x1, y1);
            y2 = y1 + real_height;
            
            
            //imshow("Red", frameROI);
            //cvtColor(Red, Red, CV_HSV2BGR);
            
            
            //imshow("Red1", Red1);
            //imshow("Red2", Red2);
            //red_histogram(frameROI, dst_red, frameROI.rows);
            
            //horizontal_histogram(frameROI, dst_horizontal, frameROI.rows);
            
            //implement( frameROI , recordnum , i );
            
            //*real_x = horizontal_histogram(frameROI, dst_horizontal, frameROI.rows / 2);
            //red_histogram(src, dst_red, 35);
            
            /*Point pt1(car[i].x, car[i].y);
             Point pt2(car[i].x, car[i].y + real_y);
             Point pt3(car[i].x + car[i].width, car[i].y);
             Point pt4(car[i].x + car[i].width, car[i].y + real_y);
             
             line(frame, pt1, pt2, Scalar(255,0,0));
             line(frame, pt3, pt4, Scalar(255,0,0));
             line(frame, pt2, pt4, Scalar(255,0,0));*/
            
            //DrawCarLine(frame, car[i].x, car[i].y, car[i].width, car[i].height);
            //DrawCarLine(frameROI, real_x[0], car[i].y, real_x[1], real_height);
            //x1 = real_x[0], x2 = real_x[1];
            
            //Point pt1 (car[i].x, car[i].y);
            //Point pt2 (car[i].x + car[i].width, car[i].y + car[i].height);
            //rectangle(frame, pt1 , pt2 , Scalar(0,0,255));
            //cout << Point(car[i].x, car[i].y + car[i].height) << ", " << Point(car[i].x + car[i].width, car[i].y + car[i].height) << endl;
            //cout << car[i].width << endl;
        }
    }
    
    int vertical_histogram(Mat src, Mat car_frame, Mat dst, int x, int initial_x, int initial_y)
    {
        int cnt[500] = {}, i, j, max = 0, min_point = 0;
        Mat Black, hsv;
        
        cvtColor(car_frame, hsv, CV_BGR2HSV);
        // 針對黑色的陰影做二值化
        inRange(hsv, Scalar(0,0,0), Scalar(180,255,46), Black);
        //imshow("Black image", Black);
        
        // 從中間一小段距離，計算二值化後的黑色像素有多少個
        for(i= x - car_frame.cols / 10; i<= x + car_frame.cols / 10; ++i){
            for(j=car_frame.rows-1; j>car_frame.rows / 2; --j){
                if(Black.at<uchar>(j, i) == 255){
                    cnt[j]++;
                }
            }
        }
        
        // 判斷車子底部（陰影處）位置
        for(j=car_frame.rows-1; j>car_frame.rows / 2; --j){
            if (max < cnt[j] && cnt[j] - max > 20){
                max = cnt[j];
                min_point = j;
            }
            else if(max < cnt[j] && cnt[j] - max < 20)
                max = cnt[j];
        }
        
        // 計算車子的左邊位置
        int left_flag = 0;
        for(i=0; i<car_frame.cols; ++i){
            if (left_flag == 0 && Black.at<uchar>(min_point - 7, i) == 255){
                left_flag++;
                real_x[0] = i;
            }
            else if(left_flag != 0 && Black.at<uchar>(min_point - 7, i) == 255){
                left_flag++;
            }
            else if(left_flag > 0)
                left_flag--;
            if( left_flag == 5 )
                break;
        }
        
        // 計算車子的右邊位置
        int right_flag = 0;
        for(i=car_frame.cols-1; i>0; --i){
            if (right_flag == 0 && Black.at<uchar>(min_point - 7, i) == 255){
                right_flag++;
                real_x[1] = i;
            }
            else if(right_flag != 0 && Black.at<uchar>(min_point - 7, i) == 255){
                right_flag++;
            }
            else if(right_flag > 0)
                right_flag--;
            if( right_flag == 5 )
                break;
        }
        
        // 將車子的邊線畫出
        DrawCarLine(src, car_frame, real_x[0], min_point * 0.4, real_x[1], min_point, initial_x, initial_y);
        
        return min_point;
    }
    
    int horizontal_histogram(Mat src, Mat dst, int y)
    {
        int a[4] = {}, i, j;
        int start = y - src.rows / 10, end = y + src.rows / 10;
        for(i=0; i<src.cols; i++){
            int sum = 0;
            for(j=start; j<=end; ++j){
                sum += ( src.at<Vec3b>(j,i)[0] * 0.114 + src.at<Vec3b>(j,i)[1] * 0.587 + src.at<Vec3b>(j,i)[2] * 0.299 );
            }
            sum /= end - start + 1;
            //line(dst,Point(i,255),Point(i,255 - sum),Scalar(0));
        }
        for(j=205; j>=0; j-=50){
            line(dst, Point(0,j), Point(src.cols,j), Scalar(255,255,0));
        }
        return *a;
    }
    
    void red_histogram(Mat src, Mat dst, int y)
    {
        Mat Red1, Red2, Red_merge, hsv;
        cvtColor(src, hsv, CV_BGR2HSV);
        inRange(hsv, Scalar(0,45,50), Scalar(10,255,255), Red1);
        inRange(hsv, Scalar(156,45,50), Scalar(180,255,255), Red2);
        Red_merge = Red1 + Red2;
        imshow("Red", Red_merge);
        
        int red[256] = {}, i, j;
        for(i=0; i<src.cols; i++){
            int sum = ( src.at<Vec3b>(y,i)[2] ) ;
            line(dst,Point(i,255),Point(i,255 - sum),Scalar(0));
        }
        for(j=205; j>=0; j-=50){
            line(dst, Point(0,j), Point(src.cols,j), Scalar(255,255,0));
        }
    }
    
    int vertical_analysis(int gray[256], int row_size)
    {
        int i, point = 0, min = 10000;
        for(i=row_size-1; i>=row_size/3; i--){
            if (min > gray[i]){
                min = gray[i];
                point = i;
            }
        }
        int diff = 0;
        for(i=point;i<row_size * 0.9; ++i){
            for(int j=i; j<row_size * 0.9 - 3; j+=3){
                if(gray[j+3] - gray[j] > diff){
                    diff = gray[j+3] - gray[j];
                    point = j;
                }
            }
        }
        return point;
    }

    void DrawCarLine(Mat src, Mat car_frame, int x1, int y1, int x2, int y2, int initial_x, int initial_y)
    {
        
        Point pt1(x1, y1);
        Point pt2(x1, y2);
        Point pt3(x2, y1);
        Point pt4(x2, y2);

//        line(car_frame, pt1, pt2, Scalar(0,0,0));
//        line(car_frame, pt3, pt4, Scalar(0,0,0));
//        line(car_frame, pt2, pt4, Scalar(0,0,0));
        line(car_frame, pt2, pt4,  Scalar(0,66,255), 3, CV_AA);
        
        if(crossLaneR >3 || crossLaneL >3){
            putText(car_frame,"trans" , pt2  ,  FONT_HERSHEY_COMPLEX , 1,Scalar(0,255,0));//show文字
        }


        pt2.x += initial_x; pt2.y += 650 + initial_y; pt4.x += initial_x; pt4.y += 650 + initial_y;
        
        Car *car1 = new Car(pt2,pt4);
        
        pt2.x -= initial_x; pt2.y -= 650 + initial_y; pt4.x -= initial_x; pt4.y -= 650 + initial_y;

        putText(car_frame, car1->meterCarString , (pt2 + pt4)/2,  FONT_HERSHEY_COMPLEX , 1,Scalar(0,255,0));//show文字
        //car1->carRoiTransLane();
        Lane *detect = new Lane(src,*car1);
//        line(src, car1->distance[0], car1->distanceR[0],  Scalar(0,0,0));
//        line(src, car1->distance[1], car1->distanceR[1],  Scalar(0,0,0));
//        line(src, car1->distance[2], car1->distanceR[2],  Scalar(0,0,0));
//        line(src, car1->distance[3], car1->distanceR[3],  Scalar(0,0,0));
        

        delete detect;
        delete car1;

    }
    
    
    
    
    
};
#endif /* DetectCar_hpp */
