//
//  car.hpp
//  B10CppTest_1014
//
//  Created by 凌中漢 on 2017/11/12.
//  Copyright © 2017年 joelin. All rights reserved.
//

#ifndef car_hpp
#define car_hpp
#include <cstdlib>
#include <stdio.h>
#include <string>
using namespace cv;
using namespace std;
class Car{
public:
    Car(){}
    
    Car(Point L, Point R){
        r.x = R.x;
        r.y = R.y;
        l.x = L.x;
        l.y = L.y;
        lUp.x = lUp.x;
        lUp.y = lUp.y;
        rUp.x = rUp.x;
        rUp.y = rUp.y;
        
        temp = double(r.y);
        distanceCar();
    }
    
    void carSetPoint(Point L, Point R, Point LUp, Point RUp, Point Lturnlight1, Point Lturnlight2, Point Rturnlight1, Point Rturnlight2, int car_turn_signal_flag, int car_flag, int leftSignal, int rightSignal){
        r.x = R.x;
        r.y = R.y;
        l.x = L.x;
        l.y = L.y;
        lUp.x = LUp.x;
        lUp.y = LUp.y;
        rUp.x = RUp.x;
        rUp.y = RUp.y;
        lturn1.x = Lturnlight1.x;
        lturn1.y = Lturnlight1.y;
        lturn2.x = Lturnlight2.x;
        lturn2.y = Lturnlight2.y;
        rturn1.x = Rturnlight1.x;
        rturn1.y = Rturnlight1.y;
        rturn2.x = Rturnlight2.x;
        rturn2.y = Rturnlight2.y;
        
        leftSignalflag = leftSignal;
        rightSignalflag = rightSignal;
        turn_signal_flag = car_turn_signal_flag;
        flag = car_flag;
        temp = double(r.y);
        distanceCar();
    }
    
    void carSetValue(int flag){
        turn_signal_flag = flag;
    }
    
    void carRoiTransLane(){
        r.x -= 300;
        r.y -= 780;
        l.x -= 300;
        l.y -= 780;
    }
    void carRoiTransLaneRe(){
        r.x += 300;
        r.y += 780;
        l.x += 300;
        l.y += 780;
    }
    
    void distanceCar(){
        temp-=650;
        distance[0].x = 0; distance[0].y =300;// 10m
        distanceR[0].x = 1920; distanceR[0].y = 300;
        distance[1].x = 0; distance[1].y = 240;// 20m
        distanceR[1].x = 1920; distanceR[1].y = 240;
        distance[2].x = 0; distance[2].y = 200;//30m
        distanceR[2].x = 1920; distanceR[2].y = 200;
        distance[3].x = 0; distance[3].y = 180;//40m
        distanceR[3].x = 1920; distanceR[3].y = 180;
        
        if (temp <= distance[0].y && temp > distance[1].y) {
            meterCar = 10.0 + ((distanceNum[0] - temp) / (distanceNum[0] - distanceNum[1])) * 10.0;
            sprintf(meterCarString, "%.1f", meterCar);
        }
        else if(temp <= distance[1].y && temp > distance[2].y){
            meterCar = 15.0 + (((distanceNum[1] -temp) / (distanceNum[1] - distanceNum[2])) * 10.0);
            sprintf(meterCarString, "%.1f", meterCar);
        }
        else if(temp <= distance[2].y && temp > distance[3].y){
            meterCar = 20.0 + ((distanceNum[2] - temp) / (distanceNum[2] - distanceNum[3]) * 10.0);
            sprintf(meterCarString, "%.1f", meterCar);
        }
        else{
            strcpy(meterCarString, "far");
        }
        
    }
    
    void drawCarLine(Mat src){
        if(flag != 9999){
            if(illegal){
                isIllegal(src);
            }
            else{
                notIllegal(src);
            }
            drawLightLine(src);
        }
        
        if(turn_signal_flag <= 10){
            if(leftSignalflag == 1)
                leftTurnLight(src);
            if(rightSignalflag == 1)
                rightTurnLight(src);
        }
    }
    
    void isIllegal(Mat src){
        line( src, l, r, Scalar(0,0,255), 3, CV_AA);
        line( src, l, lUp, Scalar(0,0,255), 2);
        line( src, r, rUp, Scalar(0,0,255), 2);
        line( src, lUp, rUp, Scalar(0,0,255), 2);
    }
    
    void notIllegal(Mat src){
        line( src, l, r, Scalar(255,0,0), 3, CV_AA);
        line( src, l, lUp, Scalar(255,0,0), 2);
        line( src, r, rUp, Scalar(255,0,0), 2);
        line( src, lUp, rUp, Scalar(255,0,0), 2);
    }
    
    void drawLightLine(Mat src){
        rectangle(src, lturn1, lturn2, Scalar(100,100,200), 2);
        rectangle(src, rturn1, rturn2, Scalar(100,100,200), 2);
    }
    
    void leftTurnLight(Mat src){
        putText(src, "Left turn signal", lUp, FONT_HERSHEY_COMPLEX_SMALL, 1.3, Scalar(0,255,255), 4);
        rectangle(src, lturn1, lturn2, Scalar(0,255,255), 3);
    }
    
    void rightTurnLight(Mat src){
        putText(src, "Right turn signal", lUp, FONT_HERSHEY_COMPLEX_SMALL, 1.3, Scalar(0,255,255), 4);
        rectangle(src, rturn1, rturn2, Scalar(0,255,255), 3);
    }
    
    Point r, l, rUp, lUp;
    Point lturn1, lturn2, rturn1, rturn2;
    Point distance[5];
    Point distanceR[5];
    int leftSignalflag = 0, rightSignalflag = 0;
    //int Yellow = 0;
    int turn_signal_flag = 120;
    int flag = 9999;
    int croosLaneNumR = 0,croosLaneNumL = 0;
    int crossLineNum = 7;
    double meterCar, temp, tempY;
    double distanceNum[5] = { 300, 240, 200, 180 };
    char meterCarString[20];
    bool illegal = 0, crossLine = 0,noLight = 0;
    
};

#endif /* car_hpp */
