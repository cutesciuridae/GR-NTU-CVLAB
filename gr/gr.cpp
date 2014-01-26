#include "stdafx.h"
#include "handReader.h"
#include "basicProcessing.h"
#include "Windows.h"
#include <time.h>
using namespace cv;
using namespace std;
#define PI 3.14159265

int _tmain(int argc, _TCHAR* argv[])
{
	handReader hr = handReader();
	basicProcessing bp = basicProcessing();
	hr.capture = cvCaptureFromCAM(0);
	clock_t c0 = 0, c1 = 0, c2 =0;
    double cputime;
	int check_smaller = 0, check_tracking = 0;
	int path_length = 0, base_length = 0;
	float cos_theta = 0.0;
	if(!hr.capture)
		cout << "can't open camera" <<endl;
	else
        cout << "camera open successfully" <<endl;
	POINT pt;
	//SetCursorPos(683, 384);
	while(true){
		if(cvGrabFrame(hr.capture)){
			hr.frame = cvQueryFrame(hr.capture);
			cv::GaussianBlur(hr.frame, hr.frame, Size(3, 3), 0, 0);
			hr.currframe.create(hr.frame.rows, hr.frame.cols, IPL_DEPTH_8U);
			hr.currframe.create(hr.frame.rows, hr.frame.cols, IPL_DEPTH_8U);
			hr.frame.copyTo(hr.currframe);
			hr.frame = cvQueryFrame(hr.capture);

			absdiff(hr.frame, hr.currframe, hr.destframe);
			cvtColor(hr.destframe, hr.destframe, CV_BGR2GRAY);
			threshold(hr.destframe, hr.destframe, 100, 255, THRESH_BINARY);
			cv::dilate(hr.destframe, hr.destframe, Mat());
			cv::erode(hr.destframe, hr.destframe, Mat());
			
			hr.small_frame = bp.smaller(hr.frame, 0.5);
			//cv::dilate(hr.small_frame, hr.small_frame, Mat());
			//cv::erode(hr.small_frame, hr.small_frame, Mat());
			hr.small_destframe = bp.smaller(hr.destframe, 0.5);
			
			
			hr.small_frame.copyTo(hr.currframe);
			cvtColor(hr.currframe, hr.hsv, CV_BGR2HSV);
			inRange(hr.hsv, Scalar(0, 30, 60, 0), Scalar(20, 150, 255, 0), hr.hsv_thre);
			cvtColor(hr.hsv_thre, hr.hsv_thre, CV_GRAY2BGR);
			cv::dilate(hr.hsv_thre, hr.hsv_thre, Mat());
			cv::erode(hr.hsv_thre, hr.hsv_thre, Mat());
			//medianBlur(hr.hsv_thre,hr.hsv_thre, 5);
			Mat drawing = Mat::zeros(hr.small_destframe.size(), CV_8UC3 );
			//Mat newIm = hr.boundingBox(hr.small_destframe, hr.hsv_thre, hr.coor);/*cooool*/
			hr.hsv.release();
			hr.destframe.release();
			hr.currframe.release();
			hr.frame.release();
			//hr.small_destframe.release();
			c0=clock();
			if(check_smaller == 3 && check_tracking == 1){
				cout<<"nowww"<<endl;
			}
			if(!hr.boundingBox(hr.small_destframe, hr.hsv_thre, hr.shit)){
				c1 = clock();
				/*if(check_smaller == 1){
					check_smaller = 2;
				}
				if(check_smaller == 2){
					cout<<"ready for tracking."<<c0<<endl;
				}else
					check_smaller = 0;
				check_tracking = 0;*/
			}else{
				c2 = clock();
				circle( drawing, hr.max_circle, 5, Scalar( 0, 0, 255 ), 2, 8, 0 );
				if(hr.pathX.size() > 10){
					if(hr.log_border[0].back() > 20 && float((c2-c1)/CLOCKS_PER_SEC) > 0.2){
						line(drawing, hr.pathX[hr.pathX.size()-10], hr.pathX.back(), Scalar( 0, 255, 0 ), 2, 8);
						if(sqrt(pow(abs(hr.pathX.back().x-hr.pathX[hr.pathX.size()-10].x),2)+pow(abs(hr.pathX.back().y-hr.pathX[hr.pathX.size()-10].y),2)) > 100){
							hr.log_time.push_back(c2);
							//cout<<"yoooo. "<<c0<<endl;
							if(hr.pathX.back().x-hr.pathX[hr.pathX.size()-10].x < 0){
								hr.log.push_back(2);
								if(hr.log.size() == 1){
									cout<<"right"<<endl;
									keybd_event(VK_RIGHT, 0x27, KEYEVENTF_EXTENDEDKEY | 0, 0);
									keybd_event(VK_RIGHT, 0x27, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
								}else{
									if(hr.log_time.back()-hr.log_time[hr.log_time.size()-2] > 500){
										keybd_event(VK_RIGHT, 0x27, KEYEVENTF_EXTENDEDKEY | 0, 0);
										keybd_event(VK_RIGHT, 0x27, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
										cout<<"right"<<endl;
									}
								}
								//cout<<"right"<<hr.log_time.back()<<hr.log.size()<<hr.log_time.size()<<endl;
							}else{
								hr.log.push_back(4);
								if(hr.log.size() == 1){
									cout<<"left"<<endl;
									keybd_event(VK_LEFT, 0x25, KEYEVENTF_EXTENDEDKEY | 0, 0);
									keybd_event(VK_LEFT, 0x25, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
								}else{
									if(hr.log_time.back()-hr.log_time[hr.log_time.size()-2] > 500){
										keybd_event(VK_LEFT, 0x25, KEYEVENTF_EXTENDEDKEY | 0, 0);
										keybd_event(VK_LEFT, 0x25, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
										cout<<"left"<<endl;
									}
								}
								//cout<<"left"<<hr.log_time.back()<<hr.log.size()<<hr.log_time.size()<<endl;
							}
						}
					}
					/*hr.log_time.push_back(c0);
					if(check_smaller == 2 && hr.log_border[0].back() > 20){
						check_smaller = 3;
						cout<<check_smaller<<endl;
						check_tracking = 1;
					}
					if(check_smaller != 3)
						check_smaller = 1;*/
				}else{
					cout<<"Initializing..."<<endl;
				}
			}
			imshow("test2", hr.small_destframe);
			imshow("test3", drawing);
			if(waitKey(10) == 27)
				break;
		}
	}
	return EXIT_SUCCESS;
}

