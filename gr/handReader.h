#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/legacy/legacy.hpp>
#include <opencv2/legacy/compat.hpp>
#include <opencv2/flann/flann.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/video/background_segm.hpp>
#include "iostream"
#include "stdio.h"
#include "stdlib.h"
#include <vector>
#include <queue>
#include <math.h>
#include <algorithm>

using namespace cv;
using namespace std;
struct BOUNDINGBOX{
    int label;
    int num;
    int top;
    int bottom;
    int left;
    int right;
};
class handReader {
	public:
		Ptr<CvCapture> capture;
		Ptr<IplImage> testC;
		Mat background_candi, comp, result_img, sobel_buffer,buffer, curv;
		Mat hsv, hsv_thre, frame, currframe, destframe, small_frame, small_currframe, small_destframe, binary, sobel_img;
		handReader();
		~handReader();
		vector <Point2f> pathX;
		vector<int>log;
		vector<float>log_time;
		vector<int>log_border[2];
		int max_radius_index;
		vector <Point2f> big_radius_v;
		Point2f max_circle;
		vector<int>shit[4];
		vector<int>width_layer;
		bool boundingBox(Mat img, Mat skin,vector<int>coor[4]);
		bool bounding(Mat img, Mat skin);
		void draw_box(Mat img, int top, int bottom, int left, int right);
		void histogram_eq(Ptr<IplImage> pImg);
		int hull_size;
		//queue<int>shit[4];
		queue<int>hesitate;
};