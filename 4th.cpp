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
int iLowH = 70;
int iHighH = 100;
int iLowS = 0;
int iHighS = 255;
int iLowV = 250;
int iHighV = 255;

int RiLowH = 0;
int RiHighH = 35;
int RiLowS =10;
int RiHighS = 192;
int RiLowV = 250;
int RiHighV = 255;

int extime = 10;
vector<Contour> bluelight;
vector<Contour> redlight;
int minContourSize = 100;
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
        element = getStructuringElement(MORPH_RECT, Size(5, 5));
	dilate(imgThresholded, imgThresholded, element);
	//imshow("Dilate", imgThresholded);

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

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;


	Mat tempHSV;
	vector<Mat> hsvSplit;
	cvtColor(input, tempHSV, COLOR_BGR2HSV);
	split(tempHSV, hsvSplit);
	equalizeHist(hsvSplit[2], hsvSplit[2]);
	merge(hsvSplit, tempHSV);
	Mat imgThresholded;
	inRange(tempHSV, Scalar(RiLowH, RiLowS, RiLowV), Scalar(RiHighH, RiHighS, RiHighV), imgThresholded);
	imshow("HSV", imgThresholded);
	Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
	erode(imgThresholded, imgThresholded, element);
        element = getStructuringElement(MORPH_RECT, Size(5, 5));
	dilate(imgThresholded, imgThresholded, element);
	//imshow("Dilate", imgThresholded);

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
			redlight.push_back(contours[i]);
		}
	}
	for (int i = 0; i < redlight.size(); i++)
	{
		rectangle(input, boundingRect(redlight[i]), Scalar(0, 0, 255), 2);
	}
	imshow("frameB", input);
	waitKey(10);
}


void printCenter(vector<Contour> lights,Mat input)
{
     vector<RotatedRect> rects;
     vector<Pair*> pairs;
     for(vector<Contour>::iterator p = lights.begin();p<lights.end();p++)
     {
            if( contourArea(*p) < 100)
            continue;
            RotatedRect temp;
            temp = minAreaRect(*p);
            rects.push_back(temp);
     }
     for(vector<RotatedRect>::iterator p = rects.begin();p< (rects.end()-1);p++)
     {
        double preD = -1;
        double preR = -1;
            for(vector<RotatedRect>::iterator q = (p + 1) ; q<rects.end() ; q ++)
           {
               	float angleOne = (*p).angle;
        	float angleTwo = (*q).angle;
 		float dif = angleOne - angleTwo;
                float area1 = (*p).size.height * (*p).size.width;
                float area2 = (*q).size.height * (*q).size.width;
                float r = area1/area2;
                if(dif*dif < 100 && r < 3 && r > 0.3)
                {
                    if(preD == -1)
                    {
                       Pair* temp = new Pair;
                       temp->memberOne = new RotatedRect;
                       temp->memberTwo = new RotatedRect;
                       *(temp->memberTwo) = *q;
                       *(temp->memberOne) = *p;
                       temp->isCompeleted = true;
                       pairs.push_back(temp);
                       preD = dif;
                       preR = r;
                    }
                    if(preD > dif && preR > r)
                    {
                       RotatedRect* temp = new RotatedRect;
                       *temp = *q;
                       delete (*pairs.end())->memberTwo;
                       (*pairs.end())->memberTwo = temp;
                       preD = dif;
                       preR = r;
                    }
                      
                }
           }
    
     }
     for(vector<Pair*>::iterator p = pairs.begin();p<pairs.end();p++)
     {
            (*p)->id = (*p)->findID();
            (*p)-> size = (*p)->findSize();
            (*p)-> center =(*p)-> findCenter();
     }
        



         if(pairs.empty())
	{
		cout<< " no valid center"<<endl;
		// transmit center(empty) here
	}
	else
	{
	   vector<Pair*>::iterator p = pairs.begin();
	   vector<Pair*>::iterator max = p;
	   for(;p<pairs.end();p++)
	   {
		  if((*p)->memberOne->angle < 30)
                  continue;
                  if((*p)->size >= (*max)->size)
		  {
			 max = p;
		  }
	   }
	   cout<<(*max)->center.x<<"      "<<(*max)->center.y<<endl;
	   //  transmit center here
	}
}



int main(int a, char**)
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
	cvCreateTrackbar("LowH", "Control-----Red", &RiLowH, 179); //Hue (0 - 179)
	cvCreateTrackbar("HighH", "Control-----Red", &RiHighH, 179);
	cvCreateTrackbar("LowS", "Control-----Red", &RiLowS, 255); //Saturation (0 - 255)
	cvCreateTrackbar("HighS", "Control-----Red", &RiHighS, 255);
	cvCreateTrackbar("LowV", "Control-----Red", &RiLowV, 255); //Value (0 - 255)
	cvCreateTrackbar("HighV", "Control-----Red", &RiHighV, 255);
	cvCreateTrackbar("time", "Control-----Red", &extime, 17);


 VideoCapture cap(0); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
        return -1;


	
	while (1)
	{
        Mat frame;

	cap >> frame;
	//imshow("Input", frame);
	if(a == 0)
        {
        recConB(frame);
	printCenter(bluelight,frame); 
        }
        //if(a == 1)
        //{
        recConR(frame);
	printCenter(redlight,frame);
        //}
		
	waitKey(100);
	}




	return 0;
}
