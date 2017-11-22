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
class Car{
public:
    Car(){
        
    }
    Car(Point L, Point R){
        r.x = R.x;
        r.y = R.y;
        l.x = L.x;
        l.y = L.y;
        temp = double(r.y);
        distanceCar();

    }
    void carSetPoint(Point L, Point R){
        r.x = R.x;
        r.y = R.y;
        l.x = L.x;
        l.y = L.y;
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
            double a = distanceNum[0] - temp / distanceNum[0] - distanceNum[1];
            double b = (distanceNum[0] - temp / distanceNum[0] - distanceNum[1]) / 10.0;
                                  
            meterCar = 10.0 + ((distanceNum[0] - temp) / (distanceNum[0] - distanceNum[1])) * 10.0;
            sprintf(meterCarString, "%.1f", meterCar);
        }
        else if(temp <= distance[1].y && temp > distance[2].y){
            double a = ((distanceNum[1] - temp )/ (distanceNum[1] - distanceNum[2]));
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
    //int flag = 5;
    Point r, l;
    int Yellow = 0;
    
    
    double meterCar, temp, tempY;
    char meterCarString[20];
    Point distance[5];
    Point distanceR[5];
    double distanceNum[5] = { 300, 240, 200, 180 };
    int turn_signal_flag = 99;
    int croosLaneNumR = 0,croosLaneNumL = 0;


};

#endif /* car_hpp */
