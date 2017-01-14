#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <cv.h>
#include <time.h>
#include <limits.h>
#include <wiringPi.h>
 
using namespace std;
using namespace cv;

double great = 0;
double targetHeight = 68.25;
double cameraHeight = 14.5;
double verticalFOV = 35;
double horizontalFOV = 60;
double cameraAngle = 49;
double pi = 3.141592;

static void drawSquares( Mat& image, const vector<vector<Point> >& squares ){
	namedWindow( "wndname", WINDOW_AUTOSIZE);
   	for( size_t i = 0; i < squares.size(); i++ ){
        	const Point* p = &squares[0][0];
        	int n = (int)squares[i].size();
	        polylines(image, &p, &n, 1, true, Scalar(0,255,0), 3, CV_AA);
    	}
    	cvtColor( image, image,  CV_BGR2GRAY);
    	imshow("wndname", image);
}

int main( int argc, char** argv ){
	vector<vector<Point> > contours, approx1;
	vector<Point> approx2;
	double y;
	double x;
	double distance;
	double azimuth;
	Rect rec;
	VideoCapture cap;
	//cap.set(CV_CAP_PROP_EXPOSURE, 1/120);
	cap.open(-1);
  //  cap.open("test.mov");
	Mat image, frame, heir;
	wiringPiSetup();
	pinMode(8, OUTPUT);
	pinMode(7, OUTPUT);
	pinMode(16, OUTPUT);
	pinMode(4, OUTPUT);
	pinMode(13, OUTPUT);
	pinMode(6, OUTPUT);
	pinMode(10, OUTPUT);
	pinMode(11, OUTPUT);
	pinMode(22, OUTPUT);
	pinMode(26, OUTPUT);
	pinMode(23, OUTPUT);
	pinMode(24, OUTPUT);
	pinMode(25, OUTPUT);
	pinMode(28, OUTPUT);
	pinMode(29, OUTPUT);
    //namedWindow( "endge", WINDOW_AUTOSIZE);
    if(!cap.isOpened()){
		return -1;
	}
	for(;;){
		great = 0;
		approx1.clear();
		approx2.clear();
		cap >> image;
		if (image.data) {
            	//	imshow("img", image);
        	}	
		GaussianBlur( image, image, Size(5, 5), 0, 0);
        	if (!image.data) {
            		return 0;
        	}
		cvtColor( image, image,  COLOR_BGR2HSV);//convert to HSV
		inRange(image, Scalar(55, 180, 75), Scalar(110,255,185), frame);
        	if (frame.data) {
            	//	imshow("edges", frame);
        	}
        findContours(frame, contours, heir,CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
		for (size_t i = 0; i < contours.size(); i++){
			Rect rec = boundingRect(contours[i]);
			if(rec.height > 25 && rec.width > 25){
				approxPolyDP(Mat(contours[i]), approx2, 6, true);
				if(approx2.size() == 8 && rec.area() > great){ 
					great = rec.area();
					approx1.push_back(approx2);
					x = rec.br().x - rec.width / 2;
					y = rec.br().y + rec.height / 2;
					y = -((2 * (y / cap.get(CV_CAP_PROP_FRAME_HEIGHT))) - 1);
					distance = (targetHeight - cameraHeight) / tan(((y * verticalFOV / 2 + cameraAngle) * pi/180));
					azimuth = (x / cap.get(CV_CAP_PROP_FRAME_WIDTH)) * horizontalFOV;
					cout << distance << " " << azimuth << " " << rec.br().x << " " << x << " " << rec.width << endl;
				}
			}
            	//distance to binary output
		}
        if (image.data) {
           // drawSquares(image, approx1);
        }
    	    	if(distance >= 64){
       	     		digitalWrite(24, HIGH);
       	     		distance = distance - 64;
        	} else {
            		digitalWrite(24, LOW);
        	}
       		if(distance >= 32){
            		digitalWrite(25, HIGH);
            		distance = distance - 32;
        	} else {
            		digitalWrite(25, LOW);
        	}
        	if(distance >= 16){
            		digitalWrite(29, HIGH);
            		distance = distance - 16;
        	} else {
           		digitalWrite(29, LOW);
        	}
        	if(distance >= 8){
        		digitalWrite(28, HIGH);
            		distance = distance - 8;
        	} else {
            		digitalWrite(28, LOW);
        	}
        	if(distance >= 4){
            		digitalWrite(22, HIGH);
            		distance = distance - 4;
       		} else {
            		digitalWrite(22, LOW);
        	}
        	if(distance >= 2){
            		digitalWrite(23, HIGH);
            		distance = distance - 2;
        	} else {
            		digitalWrite(23, LOW);
        	}
        	if(distance >= 1){
           		digitalWrite(13, HIGH);
            		distance = distance - 1;
        	} else {
           		 digitalWrite(13, LOW);
        	}
        
       		//azimuth to binary output
		// we have 0-128 on the binary channels so we x2 from 0-60
		azimuth = azimuth * 2;
       		if(azimuth >= 64){
           		digitalWrite(6, HIGH);
            		azimuth = azimuth - 64;
       		} else {
            		digitalWrite(6, LOW);
        	}
       		if(azimuth >= 32){
            		digitalWrite(10, HIGH);
            		azimuth = azimuth - 32;
       		} else {
            		digitalWrite(10, LOW);
        	}
        	if(azimuth >= 16){
            		digitalWrite(11, HIGH);
            		azimuth = azimuth - 16;
        	} else {
           		digitalWrite(11, LOW);
        	}
        	if(azimuth >= 8){
           		digitalWrite(4, HIGH);
            		azimuth = azimuth - 8;
        	} else {
            		digitalWrite(4, LOW);
       		}
        	if(azimuth >= 4){
           		digitalWrite(8, HIGH);
           		azimuth = azimuth - 4;
        	} else {
            		digitalWrite(8, LOW);
       		}
       		if(azimuth >= 2){
            		digitalWrite(16, HIGH);
            		azimuth = azimuth - 2;
        	} else {
           		digitalWrite(16, LOW);
        	}
        	if(azimuth >= 1){
            		digitalWrite(7, HIGH);
           		azimuth = azimuth - 1;
        	} else {
            		digitalWrite(7, LOW);
        	}	
        	if(waitKey(30) >= 0){
			break;
		}
	}
	return 0;
}
