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
        DetectAndDisplay( frameROI, recordnum );
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
    
    void DetectAndDisplay( Mat frame, int recordnum )
    {
        car_cascade.load(car_cascade_name);
        std::vector<Rect> car;
        Mat frame_gray;
        cvtColor( frame, frame_gray, CV_BGR2GRAY );
        equalizeHist( frame_gray, frame_gray );
        
        double detect_start, detect_end;
        // 開始偵測車子的時間
        detect_start = clock();
        
        // Detect cars
        car_cascade.detectMultiScale( frame_gray, car, 1.1, 30, 0|CV_HAAR_SCALE_IMAGE, Size(0,0) );
        // 結束偵測車子的時間
        detect_end = clock();
        
        // 輸出偵測執行時間
        //cout << "detect speed : " << (detect_end - detect_start) / CLOCKS_PER_SEC << "sec" << endl;
        
        // 偵測到車子大概位置後，擴大偵測的邊界，並再次切出ROI
        for( int i = 0; i < car.size(); i++ )
        {
            int x1 = car[i].x, y1 = car[i].y, x2 = car[i].x + car[i].width, y2 = car[i].y + car[i].height;
            real_x[0] = car[i].x;
            real_x[1] = car[i].x + car[i].width;
    
            x1 -= car[i].width * 0.3;
            if(x1 < 0)
                x1 = 0;
            x2 += car[i].width * 0.3;
            if(x2 > 1920)
                x2 = 1920;
            
            // 切ROI
            CvRect ROI = cvRect(x1, y1, x2 - x1, y2 - y1);
            Mat frameROI = frame(ROI);
            
            Detect_Boundary(frame, frameROI, frameROI.cols / 2, i, x1, y1);
            
        }
    }
    
    void Detect_Boundary(Mat src, Mat car, int x, int Car_num, int initial_x, int initial_y)
    {
        int cnt_black[500] = {}, i, j, max = 0, min_point = 0;
        Mat Black, hsv;
        
        // 將三原色轉成hsv
        cvtColor(car, hsv, CV_BGR2HSV);
        
        // 針對黑色的陰影做二值化
        inRange(hsv, Scalar(0,0,0), Scalar(180,255,46), Black);
        
        // 從中間一小段距離，計算二值化後的黑色像素有多少個
        for(i= x - car.cols / 10; i<= x + car.cols / 10; ++i){
            for(j=car.rows-1; j>car.rows / 2; --j){
                if(Black.at<uchar>(j, i) == 255){
                    cnt_black[j]++;
                }
            }
        }
        
        // 判斷車子底部（陰影處）位置
        for(j=car.rows-1; j>car.rows / 2; --j){
            if (cnt_black[j] - max > 20){
                max = cnt_black[j];
                min_point = j;
            }
            else if(max < cnt_black[j] && cnt_black[j] - max < 20)
                max = cnt_black[j];
        }
        
        // 判斷車子的左邊位置
        int left_flag = 0;
        for(i=0; i<car.cols; ++i){
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
        
        // 判斷車子的右邊位置
        int right_flag = 0;
        for(i=car.cols-1; i>0; --i){
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
        
        // 偵測紅色部分（用於車燈）
        Mat Red1, Red2, Red_merge;
        
        // 紅色的h為156~10，因為程式限制所以要分兩個偵測並二值化，再合併
        inRange(hsv, Scalar(0,50,90), Scalar(10,255,255), Red1);
        inRange(hsv, Scalar(156,50,90), Scalar(180,255,255), Red2);
        
        // 合併兩個不同區間的紅色
        Red_merge = Red1 + Red2;
//        imshow("Red", Red_merge);
        
        // 設定範圍，偵測車燈
        int cnt_red_row[500] = {} ,cnt_red_left[500] = {}, cnt_red_right[500] = {};
        int left = real_x[0] - car.cols / 20 > 0 ? real_x[0] - car.cols / 20 : 0;
        int right = real_x[1] + car.cols / 20 < car.cols - 1 ? real_x[1] + car.cols / 20 : car.cols;
        int up = min_point * 0.25, down = min_point * 0.7;
        
        // 計算左邊車燈紅色像素個數
        for(i=left; i<=real_x[0]+(real_x[1]-real_x[0])*0.25; i++){
            for(j=up; j<down; j++){
                if(Red_merge.at<uchar>(j,i) == 255){
                    cnt_red_left[i]++;
                    cnt_red_row[j]++;
                }
            }
        }
        
        // 計算右邊車燈紅色像素個數
        for(i=real_x[0]+(real_x[1]-real_x[0])*0.75; i<=right; i++){
            for(j=up; j<down; j++){
                if(Red_merge.at<uchar>(j,i) == 255){
                    cnt_red_right[i]++;
                    cnt_red_row[j]++;
                }
            }
        }
        
        // 假定初始車燈位置
        int red_x1 = real_x[0], red_x2 = real_x[0], red_x3 = real_x[1], red_x4 = real_x[1], red_y1 = min_point * 0.25, red_y2 = min_point * 0.7;
        
        // 判斷車燈上邊界
        max = 0;
        for(j=up; j<down; j++){
            if(cnt_red_row[j] - max * 4 > 0){
                max = cnt_red_row[j];
                red_y1 = j;
            }
            else if(cnt_red_row[j] - max * 2 > 0 && cnt_red_row[j] - max * 4 < 0)
                max = cnt_red_row[j];
        }
        
        // 判斷車燈下邊界
        max = 0;
        for(j=down; j>up; j--){
            if(cnt_red_row[j] - max * 4 > 0){
                max = cnt_red_row[j];
                red_y2 = j;
            }
            else if(cnt_red_row[j] - max * 2 > 0 && cnt_red_row[j] - max * 4 < 0)
                max = cnt_red_row[j];
        }
        
        // 判斷左車燈左邊界
        max = 0;
        for(i=left; i<=real_x[0]+(real_x[1]-real_x[0])*0.25; i++){
            if(cnt_red_left[i] - max * 4 > 0){
                max = cnt_red_left[i];
                red_x1 = i;
            }
            else if(cnt_red_left[i] - max * 2 > 0 && cnt_red_left[i] - max * 4 < 0)
                max = cnt_red_left[i];
        }
        
        // 判斷左車燈右邊界
        max = 0;
        for(i=real_x[0]+(real_x[1]-real_x[0])*0.25; i>left; i--){
            if(cnt_red_left[i] - max * 4 > 0){
                max = cnt_red_left[i];
                red_x2 = i;
            }
            else if(cnt_red_left[i] - max * 2 > 0 && cnt_red_left[i] - max * 4 < 0)
                max = cnt_red_left[i];
        }
        
        // 判斷右車燈左邊界
        max = 0;
        for(i=real_x[0]+(real_x[1]-real_x[0])*0.75; i<right; i++){
            if(cnt_red_right[i] - max * 4 > 0){
                max = cnt_red_right[i];
                red_x3 = i;
            }
            else if(cnt_red_right[i] - max * 2 > 0 && cnt_red_right[i] - max * 4 < 0)
                max = cnt_red_right[i];
        }
        
        // 判斷右車燈右邊界
        max = 0;
        for(i=right; i>real_x[0]+(real_x[1]-real_x[0])*0.75; i--){
            if(cnt_red_right[i] - max * 4 > 0){
                max = cnt_red_right[i];
                red_x4 = i;
            }
            else if(cnt_red_right[i] - max * 2 > 0 && cnt_red_right[i] - max * 4 < 0)
                max = cnt_red_right[i];
        }
        
        int real_width = abs(real_x[1] - real_x[0]) > 0 ? abs(real_x[1] - real_x[0]) : 1;
        int red_width = abs(red_x4 - red_x1) > 0 ? abs(red_x4 - red_x1) : 1;
        
        // 調整車子實際寬度
        if( real_width - red_width > 0 ){
            real_x[0] = red_x1 - 1;
            real_x[1] = red_x4 + 1;
        }
        
        // 偵測黃色
        Detect_Yellow(car, hsv, red_x1, red_x4, red_y1, red_y2, Car_num);
        
        // 偵測煞車燈
        Mat Brakelight_1, Brakelight_2, Brakelight_merge;
        inRange(hsv, Scalar(0,50,200), Scalar(10,255,255), Brakelight_1);
        inRange(hsv, Scalar(156,50,200), Scalar(180,255,255), Brakelight_2);
        
        Brakelight_merge = Brakelight_1 + Brakelight_2;
//        imshow("Brakelight", Brakelight_merge);
        
        int count_left_red = 0;
        for(i=red_x1; i<=red_x2; i++){
            for(j=red_y1; j<=red_y2; j++){
                if(Brakelight_merge.at<uchar>(j,i) == 255)
                    count_left_red++;
            }
        }
        
        int count_right_red = 0;
        for(i=red_x3; i<=red_x4; i++){
            for(j=red_y1; j<=red_y2; j++){
                if(Brakelight_merge.at<uchar>(j,i) == 255)
                    count_right_red++;
            }
        }
        
        //cout << count_left_red << "   " << count_right_red << endl;
        
        if(count_left_red > 20 && count_right_red > 20 && count_left_red + count_right_red > 40){
            putText(car, "Warning!", Point( red_x1, red_y2 - 3 ), FONT_HERSHEY_COMPLEX_SMALL, 0.5, Scalar(0,0,0) );
            putText(car, "Warning!", Point( red_x3, red_y2 - 3 ), FONT_HERSHEY_COMPLEX_SMALL, 0.5, Scalar(0,0,0) );
        }
        
        // 將車燈的邊線畫出
        DrawLightLine(car, red_x1, red_x2, red_x3, red_x4, red_y1, red_y2);
        
        // 將車子的邊線畫出
        DrawCarLine(src, car, real_x[0], min_point * 0.4, real_x[1], min_point, initial_x, initial_y);
    }
    
    void Detect_Yellow( Mat car, Mat hsv, int x1, int x2, int y1, int y2, int Car_Num )
    {
        Mat Yellow;
        inRange(hsv, Scalar(18,50,50), Scalar(38,255,255), Yellow);
//        imshow("Yellow", Yellow);
        int cnt = 0;
        for ( int y=y1*0.8; y<=y2*1.2; y++ ){
            for ( int x=x1*0.8; x<=x2*1.2; x++ ){
                if( Yellow.at<uchar>(y,x) == 255){
                    cnt++ ;
                }
            }
        }
        Mat Yellow_Light;
        inRange(hsv, Scalar(20,50,220), Scalar(34,255,255), Yellow_Light);
//        imshow("Yellow_Light", Yellow_Light);
        
        int count = 0;
        for ( int y=y1*0.8; y<=y2*1.2; y++ ){
            for ( int x=x1*0.8; x<=x2*1.2; x++ ){
                if( Yellow_Light.at<uchar>(y,x) == 255){
                    count++ ;
                }
            }
        }
        
        if(count > 20){
            
        }
        
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
            pt2.y += 17;
            putText(car_frame,"change" , pt2  ,  FONT_HERSHEY_COMPLEX , 1,Scalar(0,255,0));//show文字
            pt2.y -= 17;

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
    
    void DrawLightLine(Mat car_frame, int x1, int x2, int x3, int x4, int y1, int y2){
        
        Point pt1(x1, y1);
        Point pt2(x2, y2);
        Point pt3(x3, y1);
        Point pt4(x4, y2);
        
        rectangle(car_frame, pt1, pt2, Scalar(255,0,0));
        rectangle(car_frame, pt3, pt4, Scalar(255,0,0));
    }
    
    
    
    
    
};
#endif /* DetectCar_hpp */
