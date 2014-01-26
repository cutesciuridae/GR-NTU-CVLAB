#include "stdafx.h"
#include "handReader.h"
using namespace std;
RNG rng(12345);
vector<vector<Point> > contours;
vector<Vec4i> hierarchy;
vector<Point2f>skinCenter;

handReader::handReader(){
	hull_size = 0;
}

handReader::~handReader(){
	frame.release();
	currframe.release();
	destframe.release();
}

inline bool sortX (Point2f i,Point2f j) { return (i.x<j.x); }
inline bool sortY (Point2f i,Point2f j) { return (i.y<j.y); }

bool handReader::bounding(Mat img, Mat skin){
	int max_radius = 0;
	Point2f max_circle;
	max_radius_index = 0;
	skinCenter.clear();
	threshold(img, img, 0, 255, THRESH_BINARY );
	findContours(img, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
	
	vector<vector<Point> > contours_poly( contours.size() );
	vector<Rect> boundRect( contours.size() );
	vector<Point2f>center( contours.size() );
	vector<float>radius( contours.size() );

	for( int i = 0; i < contours.size(); i++ )
	{ approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
	boundRect[i] = boundingRect( Mat(contours_poly[i]) );
	minEnclosingCircle( (Mat)contours_poly[i], center[i], radius[i] );
	}

	if(contours.size()==0)
		return false;
	/// Draw polygonal contour + bonding rects + circles
	//Mat drawing = Mat::zeros( img.size(), CV_8UC3 );
	for( int i = 0; i< contours.size(); i++ ){
		Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
		//drawContours( img, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
		//rectangle( img, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );
		if((int)radius[i] > 5){
			//circle( img, center[i], (int)radius[i], color, 2, 8, 0 );
			for(int x = boundRect[i].tl().x; x < boundRect[i].br().x; x++){
				for(int y = boundRect[i].tl().y; y < boundRect[i].br().y; y++){
					if(x>=0 && x < img.rows && y >=0 && y < img.cols && skin.data[skin.step[0]*y + skin.step[1]*x + 0] == 255){
						img.data[img.step[0]*y + img.step[1]*x + 0] = 255;						
					}
				}
			}
			if(skin.data[skin.step[0]*(int(boundRect[i].tl().y+boundRect[i].br().y)/2) + skin.step[1]*(int(boundRect[i].tl().x+boundRect[i].br().x)/2) + 0] == 255){	
				skinCenter.push_back(center[i]);
				if((int)radius[i] > max_radius){
					max_radius = (int)radius[i];
					max_circle = center[i];
					//big_radius_v.push_back(center[i]);
				}
			}
		}
	}
	
	//cout<<skinCenter.size()<<endl;
	sort (skinCenter.begin(), skinCenter.end(), sortX);
	//sort (skinCenter.begin(), skinCenter.end(), sortY);
	if(skinCenter.size()!=0){
		//cout <<skinCenter.front().x<<":"<<skinCenter.back().x<<endl;
		
		//pathX.push_back(skinCenter);
		big_radius_v.push_back(max_circle);
		
		//max_radius_index.push_back(mri);
	}
	//cout<<img.rows<<endl;
	//cout<<img.cols<<endl;
	return true;
}
bool handReader::boundingBox(Mat img, Mat skin,vector<int>coor[4]){
	threshold(img,img,80,255,CV_THRESH_BINARY);
	int change;
	int label = 1;
	int k = 0;
	
	do{
		change = 0;
		for(int i=0; i < img.rows; i++){
			for(int j=0; j < img.cols; j++){
				if((int)(img.data[img.step[0]*i + img.step[1]*j + 0]) != 0){
					if(i>0 && (int)(img.data[img.step[0]*(i-1) + img.step[1]*j + 0])!=0 && 
						(int)(img.data[img.step[0]*i + img.step[1]*j + 0]) > (int)(img.data[img.step[0]*(i-1) + img.step[1]*j + 0])){
						img.data[img.step[0]*i + img.step[1]*j + 0] = img.data[img.step[0]*(i-1) + img.step[1]*j + 0];
                        change++;
					}
					if(j>0 && (int)(img.data[img.step[0]*i + img.step[1]*(j-1) + 0])!=0 && 
						(int)(img.data[img.step[0]*i + img.step[1]*j + 0]) > (int)(img.data[img.step[0]*i + img.step[1]*(j-1) + 0])){
						img.data[img.step[0]*i + img.step[1]*j + 0] = img.data[img.step[0]*i + img.step[1]*(j-1) + 0];
                        change++;
					}
				}
			}
		}
	}while (change > 0);
	int total = 0;
    struct BOUNDINGBOX box[1000];

	for(int i=0;i < img.rows; i++){
        for(int j=0; j < img.cols; j++){
			if(img.data[img.step[0]*i + img.step[1]*j + 0]!=0){
				if(total == 0){
                    box[0].label = (int)(img.data[img.step[0]*i + img.step[1]*j + 0]);
                    box[0].num = 1;
                    box[0].top = i;
                    box[0].bottom = i;
                    box[0].left = j;
                    box[0].right = j;
                    total++;
                }else{
					for(int k=0;k<total;k++)
                        if(box[k].label == (int)(img.data[img.step[0]*i + img.step[1]*j + 0]))
                            break;

                    if(k == total){
                        box[k].label = (int)(img.data[img.step[0]*i + img.step[1]*j + 0]);
                        box[k].num = 1;
                        box[k].top = i;
                        box[k].bottom = i;
                        box[k].left = j;
                        box[k].right = j;
                        total++;
                    }
                    else{
                        box[k].num++;
                        box[k].bottom = box[k].bottom>i ? box[k].bottom : i;
                        box[k].left = box[k].left<j ? box[k].left : j;
                        box[k].right = box[k].right>j ? box[k].right : j;
                    }
				}
			}
		}
	}
	if(total == 0)
		return false;
	for(int k=0;k<total;k++){
        if(box[k].num >=20){
            for(int i=box[k].top; i<=box[k].bottom; i++){
                if(i == box[k].top || i == box[k].bottom)
                    for(int j=box[k].left; j<=box[k].right; j++){
                        img.data[img.step[0]*i + img.step[1]*j + 0] = 255;
                    }
                else{
						img.data[img.step[0]*i + img.step[1]*(box[k].left) + 0] = 255;
                        img.data[img.step[0]*i + img.step[1]*(box[k].right) + 0] = 255;
                }
            }
        }
	}


	if(box[0].top >= 0){
		if(coor[0].size()!=0)
			coor[0].pop_back();
		coor[0].push_back(box[0].top);
	}
	if(box[0].bottom >= 0){
		if(coor[1].size()!=0)
			coor[1].pop_back();
		coor[1].push_back(box[0].bottom);
	}
	if(box[0].left >= 0){
		if(coor[2].size()!=0)
			coor[2].pop_back();
		coor[2].push_back(box[0].left);
	}
	if(box[0].right >= 0){
		if(coor[3].size()!=0)
			coor[3].pop_back();
		coor[3].push_back(box[0].right);
	}
	for(int i=0; i < img.rows; i++){
		for(int j=0; j<img.cols; j++){
			if(i < coor[0][0]&& i > coor[1][0] && j < coor[2][0] && j > coor[3][0])
				if(((int)skin.data[img.step[0]*i + skin.step[1]*j + 0] < 128))
					img.data[img.step[0]*i + img.step[1]*j + 0] = 0;
				else{
					img.data[img.step[0]*i + img.step[1]*j + 0] = 255;
				}
		}
	}

	/*fix here*/
	/*int temp = 0;
	for(int j = coor[2][0]; j > coor[3][0]; j--){
		if(((int)skin.data[img.step[0]*(box[0].bottom) + skin.step[1]*j + 0] > 128)){
			temp = j;
			break;
		}
	}
	cout<<temp<<endl;*/
	max_circle.x = (box[0].left+box[0].right)/2;
	max_circle.y = (box[0].top+box[0].bottom)/2;
	log_border[0].push_back(abs(box[0].right-box[0].left));
	pathX.push_back(max_circle);
	return true;
}

/*bool handReader::find(CvSize size, Mat motion, Mat sobel, Mat skin, Mat binary, vector<int>new_coor[4]){
	if(new_coor[0].size() == 0 || new_coor[1].size() == 0 || new_coor[2].size() == 0 || new_coor[3].size() == 0)
		return false;
	threshold(sobel, sobel, 128, 255, THRESH_BINARY);
	int pos = 3;
	//cv::dilate(skin, skin, Mat());
	//cv::erode(skin, skin, Mat());
	for(int i = 0; i < motion.rows; i++){
		for(int j = 0; j < motion.cols; j++){
			if(i < new_coor[0][0]-tolerance || i > new_coor[1][0]+tolerance || j < new_coor[2][0]-tolerance || j > new_coor[3][0]+tolerance)
				sobel.data[sobel.step[0]*i + sobel.step[1]*j + 0] = 0;
			else{
				if((int)skin.data[skin.step[0]*i + skin.step[1]*j + 0] > 128)
					sobel.data[sobel.step[0]*i + sobel.step[1]*j + 0] = 255;
				else if((int)binary.data[binary.step[0]*i + binary.step[1]*j + 0] > 128)
					sobel.data[sobel.step[0]*i + sobel.step[1]*j + 0] = 255;
				else
					sobel.data[sobel.step[0]*i + sobel.step[1]*j + 0] = 0;
			}
		}
	}
	cv::dilate(sobel, sobel, Mat());
	cv::erode(sobel, sobel, Mat());
	medianBlur(sobel, sobel, 5);
	boundingBox(sobel, sobel_coor);
	return true;
}*/