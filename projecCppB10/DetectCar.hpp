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
#include <math.h>
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
        for(int i = 0; i < 10; i++){
            if(carDetectStruct[i].flag != 9999){
                carDetectStruct[i].flag++;
                carDetectStruct[i].turn_signal_flag++;
            }
            if (carDetectStruct[i].flag >6 && carDetectStruct[i].flag != 9999) {
                carDetectStruct[i].flag = 9999;
                carDetectStruct[i].turn_signal_flag = 120;
                carDetectStruct[i].r.x = 0;
                carDetectStruct[i].r.y = 0;
                carDetectStruct[i].l.x = 0;
                carDetectStruct[i].l.y = 0;
                carDetectStruct[i].lUp.x = 0;
                carDetectStruct[i].lUp.y = 0;
                carDetectStruct[i].rUp.x = 0;
                carDetectStruct[i].rUp.y = 0;
            }
        }
        
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
        car_cascade.detectMultiScale( frame_gray, car, 1.1, 30, 0|CV_HAAR_SCALE_IMAGE, Size(1,30) );
        // 結束偵測車子的時間
        detect_end = clock();
        
        // 輸出偵測執行時間
        //cout << "detect speed : " << (detect_end - detect_start) / CLOCKS_PER_SEC << "sec" << endl;
        
        // 偵測到車子大概位置後，擴大偵測的邊界，並再次切出ROI
        for( int i = 0; i < car.size(); i++ )
        {
            int x1 = car[i].x, y1 = car[i].y, x2 = car[i].x + car[i].width, y2 = car[i].y + car[i].height;
    
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
        int left_boundary = 0, right_boundary = car.cols;
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
                left_boundary = i;
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
                right_boundary = i;
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
        inRange(hsv, Scalar(0,100,100), Scalar(10,255,255), Red1);
        inRange(hsv, Scalar(156,100,100), Scalar(180,255,255), Red2);
        
        // 合併兩個不同區間的紅色
        Red_merge = Red1 + Red2;
//        imshow("Red", Red_merge);
        
        // 設定範圍，偵測車燈
        int cnt_red_row[500] = {} ,cnt_red_left[500] = {}, cnt_red_right[500] = {};
        int left = left_boundary - car.cols / 20 > 0 ? left_boundary - car.cols / 20 : 0;
        int right = right_boundary + car.cols / 20 < car.cols - 1 ? right_boundary + car.cols / 20 : car.cols;
        int up = min_point * 0.25, down = min_point * 0.7;
        
        // 計算左邊車燈紅色像素個數
        for(i=left; i<=left_boundary+(right_boundary-left_boundary)*0.25; i++){
            for(j=up; j<down; j++){
                if(Red_merge.at<uchar>(j,i) == 255){
                    cnt_red_left[i]++;
                    cnt_red_row[j]++;
                }
            }
        }
        
        // 計算右邊車燈紅色像素個數
        for(i=left_boundary+(right_boundary-left_boundary)*0.75; i<=right; i++){
            for(j=up; j<down; j++){
                if(Red_merge.at<uchar>(j,i) == 255){
                    cnt_red_right[i]++;
                    cnt_red_row[j]++;
                }
            }
        }
        
        // 假定初始車燈位置
        int red_x1 = left_boundary, red_x2 = left_boundary, red_x3 = right_boundary, red_x4 = right_boundary, red_y1 = min_point * 0.25, red_y2 = min_point * 0.7;
        
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
        for(i=left; i<=left_boundary+(right_boundary-left_boundary)*0.25; i++){
            if(cnt_red_left[i] - max * 4 > 0){
                max = cnt_red_left[i];
                red_x1 = i;
            }
            else if(cnt_red_left[i] - max * 2 > 0 && cnt_red_left[i] - max * 4 < 0)
                max = cnt_red_left[i];
        }
        
        // 判斷左車燈右邊界
        max = 0;
        for(i=left_boundary+(right_boundary-left_boundary)*0.25; i>left; i--){
            if(cnt_red_left[i] - max * 4 > 0){
                max = cnt_red_left[i];
                red_x2 = i;
            }
            else if(cnt_red_left[i] - max * 2 > 0 && cnt_red_left[i] - max * 4 < 0)
                max = cnt_red_left[i];
        }
        
        // 判斷右車燈左邊界
        max = 0;
        for(i=left_boundary+(right_boundary-left_boundary)*0.75; i<right; i++){
            if(cnt_red_right[i] - max * 4 > 0){
                max = cnt_red_right[i];
                red_x3 = i;
            }
            else if(cnt_red_right[i] - max * 2 > 0 && cnt_red_right[i] - max * 4 < 0)
                max = cnt_red_right[i];
        }
        
        // 判斷右車燈右邊界
        max = 0;
        for(i=right; i>left_boundary+(right_boundary-left_boundary)*0.75; i--){
            if(cnt_red_right[i] - max * 4 > 0){
                max = cnt_red_right[i];
                red_x4 = i;
            }
            else if(cnt_red_right[i] - max * 2 > 0 && cnt_red_right[i] - max * 4 < 0)
                max = cnt_red_right[i];
        }
        
        int real_width = abs(right_boundary - left_boundary) > 0 ? abs(right_boundary - left_boundary) : 1;
        int red_width = abs(red_x4 - red_x1) > 0 ? abs(red_x4 - red_x1) : 1;
        
        // 調整車子實際寬度
        if( real_width - red_width > 0 ){
            left_boundary = red_x1 - 1;
            right_boundary = red_x4 + 1;
        }
        
        
        
        
        // 將車燈的邊線畫出
        DrawLightLine(car, red_x1, red_x2, red_x3, red_x4, red_y1, red_y2);
        
        // 將車子的邊線畫出
        MatchCar(src, car, left_boundary, min_point * 0.4, right_boundary, min_point, initial_x, initial_y, red_y1, red_y2);
        
        
        Brake_light(car, hsv, red_x1, red_x2, red_x3, red_x4, red_y1, red_y2);
        
    }
    
    // 偵測煞車燈
    void Brake_light( Mat car, Mat hsv, int x1, int x2, int x3, int x4, int y1, int y2)
    {
        Mat Brakelight_1, Brakelight_2, Brakelight_merge;
        int i, j;
        inRange(hsv, Scalar(0,50,200), Scalar(10,255,255), Brakelight_1);
        inRange(hsv, Scalar(156,50,200), Scalar(180,255,255), Brakelight_2);
        
        Brakelight_merge = Brakelight_1 + Brakelight_2;
        //        imshow("Brakelight", Brakelight_merge);
        
        int count_left_red = 0;
        for(i=x1; i<=x2; i++){
            for(j=y1; j<=y2; j++){
                if(Brakelight_merge.at<uchar>(j,i) == 255)
                    count_left_red++;
            }
        }
        
        int count_right_red = 0;
        for(i=x3; i<=x4; i++){
            for(j=y1; j<=y2; j++){
                if(Brakelight_merge.at<uchar>(j,i) == 255)
                    count_right_red++;
            }
        }
        
        //cout << count_left_red << "   " << count_right_red << endl;
        
        if(count_left_red > 20 && count_right_red > 20 && count_left_red + count_right_red > 40){
            putText(car, "Warning!", Point( x1, y2 - 3 ), FONT_HERSHEY_COMPLEX_SMALL, 0.5, Scalar(0,0,0) );
            putText(car, "Warning!", Point( x3, y2 - 3 ), FONT_HERSHEY_COMPLEX_SMALL, 0.5, Scalar(0,0,0) );
        }

    }
    
    void Detect_Yellow( Mat car, int x1, int x2, int y1, int y2, int Car_Num )
    {
        /*Mat Yellow;
        inRange(hsv, Scalar(18,50,50), Scalar(38,255,255), Yellow);
        imshow("Yellow", Yellow);
        int cnt = 0;
        for ( int y=y1*0.8; y<=y2*1.2; y++ ){
            for ( int x=x1*0.8; x<=x2*1.2; x++ ){
                if( Yellow.at<uchar>(y,x) == 255){
                    cnt++ ;
                }
            }
        }*/
        Mat hsv, Yellow_Light;
        cvtColor(car, hsv, CV_BGR2HSV);
        inRange(hsv, Scalar(20,50,220), Scalar(34,255,255), Yellow_Light);
        imshow("Yellow_Light", Yellow_Light);
        
        int count = 0;
        for ( int y=y1*0.8; y<=y2*0.8; y++ ){
            for ( int x=x1*0.8; x<=x2*1.2; x++ ){
                if( Yellow_Light.at<uchar>(y,x) == 255){
                    count++ ;
                }
            }
        }
//        cout << count << endl;
        if( abs(count - carDetectStruct[Car_Num].yellow) > 20){
            carDetectStruct[Car_Num].turn_signal_flag = 0;
            carDetectStruct[Car_Num].yellow = count;
            //putText(car, "turn signal", Point(0,0), FONT_HERSHEY_COMPLEX_SMALL, 0.7, Scalar(0,255,255));
        }
        
    }

    
    void MatchCar(Mat src, Mat car_frame, int x1, int y1, int x2, int y2, int initial_x, int initial_y, int red_y1, int red_y2)
    {
        int i;
        Point pt1(x1, y1);
        Point pt2(x1, y2);
        Point pt3(x2, y1);
        Point pt4(x2, y2);
        
        pt2.x += initial_x; pt2.y += 650 + initial_y; pt4.x += initial_x; pt4.y += 650 + initial_y;
        pt1.x += initial_x; pt1.y += 650 + initial_y; pt3.x += initial_x; pt3.y += 650 + initial_y;
        
        bool match = false;
        int num = 99;
        char str[10];
       
        for(i = 0; i < 10; i++){
            if((abs(carDetectStruct[i].l.x - pt2.x) < 40 && abs(carDetectStruct[i].l.y - pt2.y) < 40) && carDetectStruct[i].flag != 99){
                carDetectStruct[i].l.x = pt2.x;
                carDetectStruct[i].l.y = pt2.y;
                carDetectStruct[i].r.x = pt4.x;
                carDetectStruct[i].r.y = pt4.y;
                carDetectStruct[i].lUp.x = pt1.x;
                carDetectStruct[i].lUp.y = pt1.y;
                carDetectStruct[i].rUp.x = pt3.x;
                carDetectStruct[i].rUp.y = pt3.y;
                carDetectStruct[i].flag = 0;
                match = true;
                num = i;
                
                sprintf(str, "%d", i);
                
                //putText(car_frame, str , (carDetectStruct[i].l + carDetectStruct[i].r)/2,  FONT_HERSHEY_COMPLEX , 1,Scalar(0,255,0));//lane detect number
                putText(car_frame, str , Point((pt2.x+pt4.x)/2-initial_x, (pt2.y+pt4.y)/2-initial_y-650),  FONT_HERSHEY_COMPLEX , 1,Scalar(0,255,0));//lane detect number
                break;
            }
        }
        if(match == false){
            for(i = 0; i < 10; i++){
                if(carDetectStruct[i].flag == 9999){
                    carDetectStruct[i].l.x = pt2.x;
                    carDetectStruct[i].l.y = pt2.y;
                    carDetectStruct[i].r.x = pt4.x;
                    carDetectStruct[i].r.y = pt4.y;
                    carDetectStruct[i].lUp.x = pt1.x;
                    carDetectStruct[i].lUp.y = pt1.y;
                    carDetectStruct[i].rUp.x = pt3.x;
                    carDetectStruct[i].rUp.y = pt3.y;
                    carDetectStruct[i].flag = 0;
                    carDetectStruct[i].turn_signal_flag = 120;
                    break;
                }
            }
        }
        
        // 偵測黃色
        Detect_Yellow(car_frame, x1, x2, red_y1, red_y2, i);


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
