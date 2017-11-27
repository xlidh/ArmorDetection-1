#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <fstream>
#include <thread>
using namespace std;
using namespace cv;
typedef vector<Point> Contour;

char winName[20] = "Live";
Mat frame;
VideoCapture cap;
int iLowH = 110;
int iHighH = 140;
int iLowS = 90;
int iHighS = 250;
int iLowV = 90;
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
	GaussianBlur(imgThresholded, imgThresholded, Size(5, 5), 0, 0);
	imshow("Gaussian", imgThresholded);
	Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
	dilate(imgThresholded, imgThresholded, element);
	imshow("Dilate", imgThresholded);

	findContours(imgThresholded, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	cout << "contour.size: " << contours.size() << endl;
	vector<Rect> boundRect(contours.size());

	for (int i = 0; i < contours.size(); i++)
	{
		cout << "ContourSize: " << contours[i].size() << endl;
		boundRect[i] = boundingRect(contours[i]);
		Mat temp = input(boundRect[i]);
		Scalar mean = cv::mean(temp);
		//if (mean[0] > 100)
		//{
			bluelight.push_back(contours[i]);
		//}
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
void printCenter(vector<Contour> lights)
{
	Point center;
	if (lights.size() != 0)
	{
		Contour Max = lights[0];
		for (int i = 1; i < lights.size(); i++)
		{
			if (contourArea(lights[i]) > 1.5 * contourArea(Max))
			{
				Max = lights[i];
			}
			else if (contourArea(Max) < 1.5 * contourArea(lights[i]))
			{
				center.x = (boundingRect(Max).x + boundingRect(lights[i]).x + boundingRect(Max).width) / 2;
				center.y = (boundingRect(Max).y + boundingRect(lights[i]).y + boundingRect(Max).height) / 2;
			}
		}
	}
	if (center.x * center.y == 0)
	{
		cout << "No armor detected." << endl;
	}
	else {
		cout << "Neareat armor at <" << center.x << ", " << center.y << ">." << endl;
	}
}

void threadB() // Thread to print center of blue armor
{
	while (1) 
	{
		printCenter(bluelight);
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


	// Second thread
	thread tb(threadB);
	tb.join;
	
	while (1)
	{
	cap.set(CAP_PROP_EXPOSURE, -extime);
	cap >> frame;
	imshow("Input", frame);
	recConB(frame);
		
	//If don't use thread, uncomment the following line
	//printCenter(bluelight); 
		
	waitKey(50);
	}




	return 0;
}
