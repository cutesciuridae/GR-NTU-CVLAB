#include "stdafx.h"
#include "basicProcessing.h"

basicProcessing::basicProcessing(){
	scale = 1;
	delta = 0;
	ddepth = CV_16S;
};

Mat basicProcessing::smaller(Mat img, double scale){
	dst_cvsize.width = img.cols*scale;
	dst_cvsize.height = img.rows*scale;
	resize(img, small_img, dst_cvsize);
	img.release();
	return small_img;
};

Mat basicProcessing::sobel_derivation(Mat img){
	Sobel(img, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT );
	convertScaleAbs( grad_x, abs_grad_x );
	Sobel(img, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT );
	convertScaleAbs( grad_y, abs_grad_y );
	addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad );
	grad_x.release();
	grad_y.release();
	return grad;
};