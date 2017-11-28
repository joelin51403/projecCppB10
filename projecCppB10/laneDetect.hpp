//
//  laneDetect.hpp
//  B10CppTest_1014
//
//  Created by 凌中漢 on 14/10/2017.
//  Copyright © 2017 joelin. All rights reserved.
//

#ifndef laneDetect_hpp
#define laneDetect_hpp

#include "opencv2/core/core.hpp"
#include <cv.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include <stdio.h>
#include <iostream>
#include <string>
#include <math.h>
#include "car.hpp"
#include "global_variable.h"
#define eps 1e-8
#define zero(x) (((x)>0?(x):-(x))<eps)
using namespace cv;
using namespace std;
class Lane{
public:
    Point lanePtL1, lanePtR1, lanePtL2, lanePtR2;
    Point test1, test2;
    Point pt1,pt2;
    Lane(Mat fram, Car car[]){
        laneDetectDraw(fram, car);
    }
    
    void laneDetectDraw(Mat src, Car car[]){
        vector<Vec2f> lines;
        vector<Point> laneVec,laneVec2;
        Mat dst, cdst;
        float rhoRange = 50,thetaRange = 0.2;
        bool ifRightLine = 0, ifLeftLine = 0;

        CvRect Rect1=cvRect(roiLane[0],roiLane[1],roiLane[2],roiLane[3]);
        src = src(Rect1);

        Canny(src, dst, 50, 200, 3);
        imshow("1",dst);

        
        HoughLines(dst, lines, 1, CV_PI/180, 80, 0, 0 );
        
        for( size_t i = 0; i < lines.size(); i++ )
        {
            float rho = lines[i][0], theta = lines[i][1];
            double a = cos(theta), b = sin(theta);
            double x0 = a*rho, y0 = b*rho;
            pt1.x = cvRound(x0 + 1000*(-b));
            pt1.y = cvRound(y0 + 1000*(a));
            pt2.x = cvRound(x0 - 1000*(-b));
            pt2.y = cvRound(y0 - 1000*(a));

            if((rho > 240 - rhoRange) && (rho < 240 + rhoRange) && (theta > 0.95 - thetaRange) && (theta < 0.95 + thetaRange) && ifLeftLine != 1)//left line get
            {
//                line( src2, pt1, pt2, Scalar(0,0,255), 3, CV_AA);
                ifLeftLine = 1;
                lanePtL1.x = pt1.x; lanePtL1.y = pt1.y;
                lanePtL2.x = pt2.x; lanePtL2.y = pt2.y;
                lastPtL1 = pt1; lastPtL2 = pt2;
                lanePt[0].x = pt1.x;
                lanePt[0].y = pt1.y;
                lanePt[1].x = pt2.x;
                lanePt[1].y = pt2.y;
                laneVec.push_back(pt1);
                laneVec.push_back(pt2);
                ifLeftLineNum = 0;
            }
            if((rho > -250 - rhoRange) && (rho < -250 + rhoRange) && (theta > 2.0- thetaRange) && (theta < 2.0 + thetaRange) && ifRightLine != 1)//right line get
            {
//                line( src2, pt1, pt2, Scalar(0,0,255), 3, CV_AA);
                ifRightLine = 1;
                lanePtR1 = pt1; lanePtR2 = pt2;
                lastPtR1 = pt1; lastPtR2 = pt2;
                lanePt[2].x = pt1.x;
                lanePt[2].y = pt1.y;
                lanePt[3].x = pt2.x;
                lanePt[3].y = pt2.y;
                laneVec.push_back(pt1);
                laneVec.push_back(pt2);
                ifRightLineNum = 0;
            }
            if(ifLeftLine == 1 && ifRightLine == 1){//both get
                break;
            }
        }
        if(ifLeftLine == 0 && ifLeftLineNum < 30){//no left line
            lanePtL1 = lastPtL1; lanePtL2 = lastPtL2;
            lanePt[0].x = lastPtL1.x;
            lanePt[0].y = lastPtL1.y;
            lanePt[1].x = lastPtL2.x;
            lanePt[1].y = lastPtL2.y;
            laneVec.push_back(lastPtL1);
            laneVec.push_back(lastPtL2);
            ifLeftLineNum++;
        }
        if(ifRightLine == 0 && ifRightLineNum < 30){//no right line
            lanePtR1 = lastPtR1; lanePtR2 = lastPtR2;
            lanePt[2].x = lastPtR1.x;
            lanePt[2].y = lastPtR1.y;
            lanePt[3].x = lastPtR2.x;
            lanePt[3].y = lastPtR2.y;
            laneVec.push_back(lastPtR1);
            laneVec.push_back(lastPtR2);

            ifRightLineNum++;
        }
        Mat mask = src.clone();
        if (parallel(lanePt[0],lanePt[1],lanePt[2],lanePt[3])||!intersect_in(lanePt[0],lanePt[1],lanePt[2],lanePt[3])){
            fillConvexPoly(mask,laneVec,Scalar(0,200,0));
            addWeighted(src, 0.7, mask, 0.3, 3, src);

        }// no intersec
        else{//intersec
            Point ptt[3];
            ptt[0] = lanePt[0];
            ptt[1] = lanePt[3];
            ptt[2] = *intersectionPoint(lanePt[0],lanePt[1],lanePt[2],lanePt[3]);//third point
            laneVec2.push_back(ptt[0]);
            laneVec2.push_back(ptt[1]);
            laneVec2.push_back(ptt[2]);

            fillConvexPoly(mask,laneVec2,Scalar(0,200,0));
            addWeighted(src, 0.7, mask, 0.3, 3, src);
        }


        //line(src, car.r, car.l,  Scalar(0,0,255), 3, CV_AA);//測試的線
        for(int i = 0; i < 10; i++){
            car[i].carRoiTransLane();
            //if illegal
            if((car[i].croosLaneNumL > 5 || car[i].croosLaneNumR > 5)){
                car[i].crossLine = 1;
            }
            else if(car[i].crossLineNum != 0){
                car[i].crossLineNum--;
            }
            if( car[i].turn_signal_flag > 50){
                car[i].noLight = 1;
            }
            if(car[i].crossLine == 1 && car[i].noLight == 1){
                car[i].illegal = 1;
            }
            if(car[i].illegal == 1){
                putText(src, "illegal" , (car[i].l + car[i].r)/2,  FONT_HERSHEY_COMPLEX , 1.5,Scalar(0,0,255));
            }
            // reset
            if(car[i].crossLineNum == 0){
                car[i].illegal = 0;
                car[i].noLight = 0;
                car[i].crossLine = 0;
                car[i].crossLineNum = 7;
            }
            // if cross line
            if(laneDetectLineL(car[i].l, car[i].r)){
                car[i].croosLaneNumL++;
            }
            else{
                car[i].croosLaneNumL = 0;
            }
            
            if(laneDetectLineR(car[i].l, car[i].r)){
                car[i].croosLaneNumR++;
            }
            else{
                car[i].croosLaneNumR = 0;
            }
            
            car[i].carRoiTransLaneRe();
            
        }
        
    }

                           
    bool laneDetectLineR(Point u1, Point u2){
        
        if (parallel(u1,u2,lanePtR1,lanePtR2)||!intersect_in(u1,u2,lanePtR1,lanePtR2)){
            return 0;
        }
        return 1;
        
    }
    
    bool laneDetectLineL(Point u1, Point u2){
        
        if (parallel(u1,u2,lanePtL1,lanePtL2)||!intersect_in(u1,u2,lanePtL1,lanePtL2)){
            return 0;
        }
        return 1;
        
    }

    double xmult(Point p1,Point p2,Point p0){
        return (p1.x-p0.x)*(p2.y-p0.y)-(p2.x-p0.x)*(p1.y-p0.y);
    }
    int dot_online_in(Point p,Point l1,Point l2){
        return zero(xmult(p,l1,l2))&&(l1.x-p.x)*(l2.x-p.x)<eps&&(l1.y-p.y)*(l2.y-p.y)<eps;
    }
    int same_side(Point p1,Point p2,Point l1,Point l2){
        return xmult(l1,p1,l2)*xmult(l1,p2,l2)>eps;
    }
    int parallel(Point u1,Point u2,Point v1,Point v2){
        return zero((u1.x-u2.x)*(v1.y-v2.y)-(v1.x-v2.x)*(u1.y-u2.y));
    }
    int dots_inline(Point p1,Point p2,Point p3){
        return zero(xmult(p1,p2,p3));
    }
    int intersect_in(Point u1,Point u2,Point v1,Point v2){
        if (!dots_inline(u1,u2,v1)||!dots_inline(u1,u2,v2))
            return !same_side(u1,u2,v1,v2)&&!same_side(v1,v2,u1,u2);
        return dot_online_in(u1,v1,v2)||dot_online_in(u2,v1,v2)||dot_online_in(v1,u1,u2)||dot_online_in(v2,u1,u2);
    }
    Point intersection(Point u1,Point u2,Point v1,Point v2){
        Point ret=u1;
        double t=((u1.x-v1.x)*(v1.y-v2.y)-(u1.y-v1.y)*(v1.x-v2.x))
        /((u1.x-u2.x)*(v1.y-v2.y)-(u1.y-u2.y)*(v1.x-v2.x));
        ret.x+=(u2.x-u1.x)*t;
        ret.y+=(u2.y-u1.y)*t;
        return ret;
    }

    void laneDetectDrawtest(Mat src){
        Mat dst, cdst;
        CvRect Rect1=cvRect(550,780,720,100);
         src = src(Rect1);

        Canny(src, dst, 50, 200, 3);
        cvtColor(dst, cdst, CV_GRAY2BGR);

#if 1
        vector<Vec2f> lines;
        HoughLines(dst, lines, 1, CV_PI/180, 30, 0, 0 );
        
        for( size_t i = 0; i < lines.size(); i++ )
        {
            float rho = lines[i][0], theta = lines[i][1];
            Point pt1, pt2;
            double a = cos(theta), b = sin(theta);
            double x0 = a*rho, y0 = b*rho;
            pt1.x = cvRound(x0 + 1000*(-b));
            pt1.y = cvRound(y0 + 1000*(a));
            pt2.x = cvRound(x0 - 1000*(-b));
            pt2.y = cvRound(y0 - 1000*(a));
            line( src, pt1, pt2, Scalar(0,0,255), 3, CV_AA);
        }
#else
        vector<Vec4i> lines;
        HoughLinesP(dst, lines, 1, CV_PI/180, 50, 50, 10 );
        for( size_t i = 0; i < lines.size(); i++ )
        {
            Vec4i l = lines[i];
            line( src, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, CV_AA);
        }
#endif
        
        //imshow("1123", dst);
    
    
    }
    
    Point* intersectionPoint(Point p1, Point p2, Point p3, Point p4) {
        // Store the values for fast access and easy
        // equations-to-code conversion
        float x1 = p1.x, x2 = p2.x, x3 = p3.x, x4 = p4.x;
        float y1 = p1.y, y2 = p2.y, y3 = p3.y, y4 = p4.y;
        
        float d = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
        // If d is zero, there is no intersection
        
        // Get the x and y
        float pre = (x1*y2 - y1*x2), post = (x3*y4 - y3*x4);
        float x = ( pre * (x3 - x4) - (x1 - x2) * post ) / d;
        float y = ( pre * (y3 - y4) - (y1 - y2) * post ) / d;
        // Check if the x and y coordinates are within both lines
        // Return the point of intersection
        Point* ret = new Point();
        ret->x = x;
        ret->y = y;
        return ret;
    }

    
};

#endif /* laneDetect_hpp */
