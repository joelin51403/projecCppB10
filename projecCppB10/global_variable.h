//
//  global_variable.h
//  B10CppTest_1014
//
//  Created by 凌中漢 on 2017/11/12.
//  Copyright © 2017年 joelin. All rights reserved.
//
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "laneDetect.hpp"
#include "opencv2/objdetect/objdetect.hpp"
using namespace cv;

#ifndef global_variable_h
#define global_variable_h

static Point lastPtL1,lastPtL2,lastPtR1, lastPtR2;
static int ifLeftLineNum, ifRightLineNum;

int orignal[99]={};
int yellow[99]={};
int recordnum = 0;
int crossLine = 0;
int rConti = 0, lConti = 0;

String car_cascade_name = "/Users/joelin/Documents/專題B10/cascade_all_fix_w80_h80_pos405_neg769.xml";
CascadeClassifier car_cascade;

int roiCar[4] = {0,650,1920,300};
int roiLane[4] = {300,780,1000,169};
struct carStruct{
    int flag = 99;
    Point r, l, lUp, rUp;
    int turn_signal_flag = 99;
    int yellow = 0;
};
carStruct carDetectStruct[10];
Point lanePt[4];

#endif /* global_variable_h */
