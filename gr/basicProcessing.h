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
#include "iostream"
#include "stdio.h"
#include "stdlib.h"
#include <vector>
#include <vector>
#include <math.h>

using namespace cv;
using namespace std;

class basicProcessing {
	public:
		CvSize dst_cvsize;
		basicProcessing();
		Mat small_img;
		Mat sobel_img;
		Mat grad;
		Mat grad_x, grad_y;
		Mat abs_grad_x, abs_grad_y;
		int scale;
		int delta;
		int ddepth;
		Mat smaller(Mat img, double scale);
		Mat sobel_derivation(Mat img);
};