
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
void recCon(Mat input)
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
		//rectangle(input, boundRect[i], Scalar(255, 255, 255), 2);
		Mat temp = input(boundRect[i]);
		Scalar mean = cv:: mean(temp);
		cout << "Contour #" << i << endl;
		cout << "B: " << mean[0] << endl
			 << "G: " << mean[1] << endl
			 << "R: " << mean[2] << endl;
		if (mean[0] > mean[1]+ mean[2])
		{
			bluelight.push_back(contours[i]);
		}
		else if (mean[2] > mean[1]+ mean[0] )
		{

			redlight.push_back(contours[i]);
		}
	}
	for (int i = 0; i < bluelight.size(); i++)
	{
		rectangle(input, boundingRect(bluelight[i]), Scalar(0, 0, 255), 2);
	}
	for (int i = 0; i < redlight.size(); i++)
	{
		rectangle(input, boundingRect(redlight[i]), Scalar(255, 0, 0), 2);
	}
	imshow("frame", input);
	waitKey(10);
}

int main(int, char**)
{


	/*
	Mat capsule = imread("capsule.bmp");
	if (capsule.empty()) {
		cout << "Error loading image!" << endl;
		return 0;
	}
	recCon(capsule);
	waitKey(0);
	*/

	
	cap.open(1);
	if (!cap.isOpened())  // check if we succeeded
		return -1;
	
	while (1)
	{
		cap.set(CAP_PROP_EXPOSURE, -10);
		cap >> frame;
		//resize(frame,frame,Size(200, 200));
		imshow("Input", frame);
		recCon(frame);
		waitKey(10);
	}
	


	return 0;
}
