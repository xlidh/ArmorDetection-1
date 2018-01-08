#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "sort.h"
#include <iostream>
#include <fstream>
#include <thread>

using namespace std;
using namespace cv;
typedef vector<Point> Contour;

char winName[20] = "Live";
Mat frame;
VideoCapture cap;
int iLowH = 70;
int iHighH = 100;
int iLowS = 0;
int iHighS = 255;
int iLowV = 240;
int iHighV = 255;

int RiLowH = 100;
int RiHighH = 140;
int RiLowS = 90;
int RiHighS = 255;
int RiLowV = 90;
int RiHighV = 255;

int extime = 10;
vector<Contour> bluelight;
vector<Contour> redlight;
int minContourSize = 20;
void recConB(Mat input)
{
	bluelight.clear();

	Mat clone;
	cvtColor(input, clone, CV_RGB2GRAY);
	threshold(clone, clone, 100, 255, THRESH_BINARY);

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;


	Mat tempHSV;
	vector<Mat> hsvSplit;
	cvtColor(input, tempHSV, COLOR_BGR2HSV);
	split(tempHSV, hsvSplit);
	equalizeHist(hsvSplit[2], hsvSplit[2]);
	merge(hsvSplit, tempHSV);
	Mat imgThresholded;
	inRange(tempHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded);
	imshow("HSV", imgThresholded);
	Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
	erode(imgThresholded, imgThresholded, element);
	dilate(imgThresholded, imgThresholded, element);
	imshow("Dilate", imgThresholded);

	findContours(imgThresholded, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	//cout << "contour.size: " << contours.size() << endl;
	vector<Rect> boundRect(contours.size());

	for (int i = 0; i < contours.size(); i++)
	{
		//cout << "ContourSize: " << contours[i].size() << endl;
		boundRect[i] = boundingRect(contours[i]);
		Mat temp = input(boundRect[i]);
		Scalar mean = cv::mean(temp);
		if (mean[0] > 30)
		{
			bluelight.push_back(contours[i]);
		}
	}
	for (int i = 0; i < bluelight.size(); i++)
	{
		rectangle(input, boundingRect(bluelight[i]), Scalar(0, 0, 255), 2);
	}
	imshow("frameB", input);
	waitKey(10);
}
void recConR(Mat input)
{
	redlight.clear();
	Mat clone;
	cvtColor(input, clone, CV_RGB2GRAY);
	threshold(clone, clone, 100, 255, THRESH_BINARY);
	Mat element = getStructuringElement(MORPH_RECT, Size(5, 1));
	dilate(clone, clone, element);
	imshow("Dilate", clone);
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(clone, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	//cout << "contour.size: " << contours.size() << endl;
	vector<Rect> boundRect(contours.size());

	for (int i = 0; i < contours.size(); i++)
	{
		boundRect[i] = boundingRect(contours[i]);
		Mat temp = input(boundRect[i]);
		Mat tempHSV;
		vector<Mat> hsvSplit;
		cvtColor(temp, tempHSV, COLOR_BGR2HSV);
		split(tempHSV, hsvSplit);
		equalizeHist(hsvSplit[2], hsvSplit[2]);
		merge(hsvSplit, tempHSV);
		Mat imgThresholded;
		inRange(tempHSV, Scalar(RiLowH, RiLowS, RiLowV), Scalar(RiHighH, RiHighS, RiHighV), imgThresholded);
		Scalar mean = cv::mean(imgThresholded);
		if (mean[0] > 100)
		{
			redlight.push_back(contours[i]);
		}
	}
	for (int i = 0; i <redlight.size(); i++)
	{
		rectangle(input, boundingRect(redlight[i]), Scalar(0, 0, 255), 2);
	}
	imshow("frameR", input);
	waitKey(10);
}
void printCenter(vector<Contour> lights)
{
     vector<Pair*> pairs;
	 vector<Pair*> paired;
	 vector<Contour>::iterator pc = lights.begin();
	 for(pc = lights.begin();pc<lights.end();pc++)
	 {
		 if (contourArea(*pc) < minContourSize)
			 continue;
		 if(pc == lights.begin())
		{
			 Pair* temp = new Pair;
			 temp-> addRect(*pc);
			 pairs.push_back(temp);
			 continue;
		}
		vector<Pair*>::iterator pp = pairs.begin();
		bool matcha = false;
		for(;pp<pairs.end();pp++)
		{
            if( (*pp)->match(*pc) )
			{
				matcha =true;
				break;
			}
		}
		if(matcha)
		{
			continue;
		}

		Pair* temp = new Pair;
		temp-> addRect(*pc);
		pairs.push_back(temp);
		continue;
    }
	vector<Pair*>::iterator pp = pairs.begin();

	for(vector<Pair*>::iterator pp = pairs.begin();pp<pairs.end();pp++)
	{
     if((*pp)->isCompeleted)
	 {
		 paired.push_back(*pp);
	 }
	 else
	 {
		 delete *pp;
	 }
	}
    if(paired.empty())
	{
		cout<< " no valid center"<<endl;
		// transmit center(empty) here
	}
	else
	{
	   vector<Pair*>::iterator p = paired.begin();
	   vector<Pair*>::iterator max = p;
	   for(;p<paired.end();p++)
	   {
		  if((*p)->size >= (*max)->size)
		  {
			 max = p;
		  }
	   }
	   cout<<(*max)->center.x<<"      "<<(*max)->center.y<<endl;
	   //  transmit center here
	}
}


int main(int, char**)
{
	namedWindow("Control-----Blue", CV_WINDOW_AUTOSIZE);
	cvCreateTrackbar("LowH", "Control-----Blue", &iLowH, 179); //Hue (0 - 179)
	cvCreateTrackbar("HighH", "Control-----Blue", &iHighH, 179);
	cvCreateTrackbar("LowS", "Control-----Blue", &iLowS, 255); //Saturation (0 - 255)
	cvCreateTrackbar("HighS", "Control-----Blue", &iHighS, 255);
	cvCreateTrackbar("LowV", "Control-----Blue", &iLowV, 255); //Value (0 - 255)
	cvCreateTrackbar("HighV", "Control-----Blue", &iHighV, 255);
	cvCreateTrackbar("time", "Control-----Blue", &extime, 17);

	namedWindow("Control-----Red", CV_WINDOW_AUTOSIZE);
	cvCreateTrackbar("LowH", "Control-----Red", &iLowH, 179); //Hue (0 - 179)
	cvCreateTrackbar("HighH", "Control-----Red", &iHighH, 179);
	cvCreateTrackbar("LowS", "Control-----Red", &iLowS, 255); //Saturation (0 - 255)
	cvCreateTrackbar("HighS", "Control-----Red", &iHighS, 255);
	cvCreateTrackbar("LowV", "Control-----Red", &iLowV, 255); //Value (0 - 255)
	cvCreateTrackbar("HighV", "Control-----Red", &iHighV, 255);
	cvCreateTrackbar("time", "Control-----Red", &extime, 17);


	cap.open(1);
	if (!cap.isOpened())  // check if we succeeded
	return -1;



	
	while (1)
	{
	cap.set(CAP_PROP_EXPOSURE, -extime);
	cap >> frame;
	imshow("Input", frame);
	recConB(frame);
		
	//If don't use thread, uncomment the following line
	printCenter(bluelight); 
		
	waitKey(50);
	}




	return 0;
}
