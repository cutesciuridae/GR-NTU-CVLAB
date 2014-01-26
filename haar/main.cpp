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

int haar_f1[4][4];
int haar_f2[4][4];
int haar_f3[4][4];
vector<int>temp;


void* new2d(int h, int w, int size){
    register int i;
    void **p;

    p = (void**)new char[h*sizeof(void*) + h*w*size];
    for(i = 0; i < h; i++)
    {
        p[i] = ((char *)(p + h)) + i*w*size; 
    }
	return p;
}
int matching_for_up_feature(Mat img, int **mask, int size, vector<int>record_coor[2], int pro_threshold){
	int e = 0;
	int ssd = 0;
	int match_num = 0;
	record_coor[0].clear();
	record_coor[1].clear();
	/*int match_point_counter = 0;*/
	for(int i = 0; i < img.rows; i++){
		for(int j = 0; j < img.cols; j++){
			e = 0;
			ssd = 0;
			//match_point_counter = 0;
			if(i<=img.rows-size && j<= img.cols-size){
				for(int k = 0; k < size; k++){
					for(int m = 0; m < size; m++){
						/*if(img.data[img.step[0]*(i+k) + img.step[1]*(j+m) + 0] == mask[k][m])
							match_point_counter++;*/
						e = img.data[img.step[0]*(i+k) + img.step[1]*(j+m) + 0]-mask[k][m];
						ssd += e*e;
					}
				}
				/*if(match_point_counter > 8 && match_point_counter < 10){
					record_coor[0].push_back(i);
					record_coor[1].push_back(j);
					match_num++;
				}*/
				if(ssd < size*size*255*255/pro_threshold){
					record_coor[0].push_back(i);
					record_coor[1].push_back(j);
					match_num++;
				}
			}
		}
	}
	return match_num;
}

int feature_matching(Mat img, int **mask, int size, vector<int>record_coor[2], int threshold){
	int e = 0;
	int ssd = 0;
	int match_num = 0;
	record_coor[0].clear();
	record_coor[1].clear();
	for(int i = 0; i < img.rows; i++){
		for(int j = 0; j < img.cols; j++){
			e = 0;
			ssd = 0;
			if(i<=img.rows-size && j<= img.cols-size){
				for(int k = 0; k < size; k++){
					for(int m = 0; m < size; m++){
						e = img.data[img.step[0]*(i+k) + img.step[1]*(j+m) + 0]-mask[k][m];
						ssd += e*e;
					}
				}
				if(ssd < threshold){
					record_coor[0].push_back(i);
					record_coor[1].push_back(j);
					match_num++;
				}
			}
		}
	}
	return match_num;
}

char checkRGBW(int B, int G, int R){
	if(B+G+R == 765)
		return 'W';
	else{
		if(B == 255)
			return 'B';
		else if(G == 255)
			return 'G';
		else
			return 'R';
	}
}
int main(){
	Mat src_image = imread("test.jpg");
		Mat taste;
	src_image.copyTo(taste);
	cv::dilate(src_image, src_image, Mat());
	medianBlur(src_image, src_image, 5);
	Mat test2;
	//IplImage * gray = cvCreateImage(cvGetSize(src_image),IPL_DEPTH_8U,1);
	cvtColor(src_image, src_image, CV_RGB2GRAY);
	threshold(src_image, src_image, 128, 255, THRESH_BINARY);
	for(int i = 0; i < src_image.rows; i++){
		for(int j = 0; j < src_image.cols; j++){
			if(src_image.data[src_image.step[0]*i + src_image.step[1]*j + 0] == 0)
				src_image.data[src_image.step[0]*i + src_image.step[1]*j + 0] = 255;
			else
				src_image.data[src_image.step[0]*i + src_image.step[1]*j + 0] = 0;
		}
	}

	cvtColor(src_image, test2, CV_GRAY2RGB);

	int **mask_l = (int **)new2d(4, 4, sizeof(int));
	int **mask_r = (int **)new2d(4, 4, sizeof(int));
	int **mask_u = (int **)new2d(16, 16, sizeof(int));
	vector<int>coor4l[2];
	vector<int>coor4r[2];
	vector<int>coor4u[2];
	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 4; j++){
			if(j < 2){
				mask_l[i][j] = 255;
				mask_r[i][j] = 0;
			}else{
				mask_l[i][j] = 0;
				mask_r[i][j] = 255;
			}
		}
	}
	for(int i = 0; i < 8; i++){
		for(int j = 0; j < 8; j++){
			if(i < 4)
				mask_u[i][j] = 0;
			else
				mask_u[i][j] = 255;
		}
	}
	/*start haar feature matching*/
	int cl = feature_matching(src_image, mask_l, 4, coor4l, 20);
	int cr = feature_matching(src_image, mask_r, 4, coor4r, 20);
	int cu = matching_for_up_feature(src_image, mask_u, 8, coor4u, 6);
	
	for(int i =0; i < cl;i++){
		for(int j=0; j < 3; j++)
			for(int k=0; k < 3; k++){
				test2.data[test2.step[0]*((coor4l[0][i])+j) + test2.step[1]*((coor4l[1][i])+k) + 0] = 255;
				test2.data[test2.step[0]*((coor4l[0][i])+j) + test2.step[1]*((coor4l[1][i])+k) + 1] = 0;
				test2.data[test2.step[0]*((coor4l[0][i])+j) + test2.step[1]*((coor4l[1][i])+k) + 2] = 0;
			}
	}
	for(int i =0; i < cr;i++){
		for(int j=0; j < 3; j++)
			for(int k=0; k < 3; k++){
				test2.data[test2.step[0]*((coor4r[0][i])+j) + test2.step[1]*((coor4r[1][i])+k) + 1] = 255;
				test2.data[test2.step[0]*((coor4r[0][i])+j) + test2.step[1]*((coor4r[1][i])+k) + 0] = 0;
				test2.data[test2.step[0]*((coor4r[0][i])+j) + test2.step[1]*((coor4r[1][i])+k) + 2] = 0;
			}
	}
	for(int i =0; i < cu;i++){
		for(int j=0; j < 3; j++)
			for(int k=0; k < 3; k++){
				test2.data[test2.step[0]*((coor4u[0][i])+j) + test2.step[1]*((coor4u[1][i])+k) + 2] = 255;
				test2.data[test2.step[0]*((coor4u[0][i])+j) + test2.step[1]*((coor4u[1][i])+k) + 0] = 0;
				test2.data[test2.step[0]*((coor4u[0][i])+j) + test2.step[1]*((coor4u[1][i])+k) + 1] = 0;
			}
	}
	int skip_rows = 4;
	int R, G, B;
	int fea_ctr_r = 0;
	int fea_ctr_g = 0;
	int fea_ctr_b = 0;
	vector<int>color_centro[3];/*0:B, 1:G, B: 2:R*/
	vector<int> cens;
	vector<int> cens2;
	int sum = 0;
	int c = 0;
	int kkk = 0;
	for(int i = 0; i < src_image.rows; i+=skip_rows){
		for(int j = 0; j < src_image.cols; j++){
			if(src_image.data[src_image.step[0]*i + src_image.step[1]*j + 0] == 255){
				fea_ctr_r = 0;
				fea_ctr_g = 0;
				fea_ctr_b = 0;
				color_centro[0].clear();
				color_centro[1].clear();
				color_centro[2].clear();
				for(int k = -20; k < 20; k++){
					if(j+k >= 0 && j+k < src_image.cols){
						B = test2.data[test2.step[0]*i + test2.step[1]*(j+k) + 0];
						G = test2.data[test2.step[0]*i + test2.step[1]*(j+k) + 1];
						R = test2.data[test2.step[0]*i + test2.step[1]*(j+k) + 2];
						if(B+G+R == 255){
							if(B == 255){
								fea_ctr_b++;
								color_centro[0].push_back(j+k);
							}else if(G == 255){
								fea_ctr_g++;
								color_centro[1].push_back(j+k);
							}else{
								fea_ctr_r++;
								color_centro[2].push_back(j+k);
							}
						}
					}
				}
				if(color_centro[0].size()!=0 && color_centro[1].size()!=0 &&
					color_centro[1][0] < color_centro[0][0])
				{
					sum = 0;
					c = 0;
					for(int n = 0; n < test2.rows; n+=2){
						for(int m = color_centro[1][0]; m < color_centro[0][0]; m++){
							if(test2.data[test2.step[0]*n + test2.step[1]*m + 2] == 255&&
								test2.data[test2.step[0]*n + test2.step[1]*m + 0] == 0 &&
								test2.data[test2.step[0]*n + test2.step[1]*m + 1] == 0){
									/*test2.data[test2.step[0]*n + test2.step[1]*m + 0] = 0;
									test2.data[test2.step[0]*n + test2.step[1]*m + 1] = 255;
									test2.data[test2.step[0]*n + test2.step[1]*m + 2] = 255;*/
									sum+=n;
									c++;
							}
						}
					}
					if(c!=0){
						cens.push_back(sum/c);
						cens2.push_back( (color_centro[0][0]+color_centro[1][0])/2);
						kkk++;
					}
				}
			}
		}
	}


	/*ohohoh*/

	for(int i = 0; i < kkk; i++){
		circle( taste, cvPoint(cens2[i], cens[i]), 10, Scalar( 0, 0, 255 ), 2, 8);
	}

	//circle( taste, cvPoint(50,50), 10, Scalar( 0, 0, 255 ), 2, 8);
	/*Mat taste;
	cvtColor(src_image, taste, CV_GRAY2RGB);
	vector<int>yellow[2];
	int fuck = 0;
	for(int i = 0; i < test2.rows; i++){
		for(int j = 0; j < test2.cols; j++){
			if(test2.data[test2.step[0]*i + test2.step[1]*j + 0] == 0 &&
				test2.data[test2.step[0]*i + test2.step[1]*j + 1] == 255 &&
				test2.data[test2.step[0]*i + test2.step[1]*j + 2] == 255){
					yellow[0].push_back(i);
					yellow[1].push_back(j);
					fuck++;
			}
		}
	}
	for(int i =0; i < fuck;i++){
		for(int j=0; j < 3; j++)
			for(int k=0; k < 3; k++){
				taste.data[taste.step[0]*((yellow[0][i])+j) + taste.step[1]*((yellow[1][i])+k) + 2] = 255;
				taste.data[taste.step[0]*((yellow[0][i])+j) + taste.step[1]*((yellow[1][i])+k) + 0] = 0;
				taste.data[taste.step[0]*((yellow[0][i])+j) + taste.step[1]*((yellow[1][i])+k) + 1] = 0;
			}
	}*/
	imshow("test", taste);
	waitKey(0);
	
	return 0;
}