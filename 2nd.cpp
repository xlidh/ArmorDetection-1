#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <fstream>
using namespace std; 
using namespace cv;
typedef vector<Point> Contour;

char winName[20] = "Live";
Mat frame;
VideoCapture cap;
RNG rng(12345);
int iLowH = 100;
int iHighH = 140;
int iLowS = 90;
int iHighS = 255;
int iLowV = 90;
int iHighV = 255;

int RiLowH = 100;
int RiHighH = 140;
int RiLowS = 90;
int RiHighS = 255;
int RiLowV = 90;
int RiHighV = 255;

int extime = 5;

void recConB(Mat input)
{
	
	vector<Contour> bluelight;
	vector<Contour> redlight;
	Mat clone;
	cvtColor(input, clone, CV_RGB2GRAY);
	threshold(clone, clone, 100,255,THRESH_BINARY);
	Mat element = getStructuringElement(MORPH_RECT, Size(5, 5));
	dilate(clone, clone, element);
	imshow("Dilate", clone);
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(clone,contours,RETR_EXTERNAL,CHAIN_APPROX_SIMPLE);
	cout << "contour.size: " << contours.size() << endl;
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
		inRange(tempHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded);
	    Scalar mean = cv::mean(imgThresholded);
		if (mean[0] > 100)
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
	vector<Contour> bluelight;
	vector<Contour> redlight;
	Mat clone;
	cvtColor(input, clone, CV_RGB2GRAY);
	threshold(clone, clone, 100, 255, THRESH_BINARY);
	Mat element = getStructuringElement(MORPH_RECT, Size(5, 5));
	dilate(clone, clone, element);
	imshow("Dilate", clone);
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(clone, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	cout << "contour.size: " << contours.size() << endl;
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

int main(int, char**)
{
	namedWindow("Control-----Blue", CV_WINDOW_AUTOSIZE);
	cvCreateTrackbar("LowH", "Control-----Blue", &iLowH, 179); //Hue (0 - 179)
	cvCreateTrackbar("HighH", "Control-----Bluel", &iHighH, 179);
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
	
	cap.open(0);
	if (!cap.isOpened())  // check if we succeeded
		return -1;
	
	while (1)
	{
		cap.set(CAP_PROP_EXPOSURE, -extime);
		cap >> frame;
		imshow("Input", frame);
		recConB(frame);
		recConR(frame);
		waitKey(10);
	}
	


	return 0;
}
