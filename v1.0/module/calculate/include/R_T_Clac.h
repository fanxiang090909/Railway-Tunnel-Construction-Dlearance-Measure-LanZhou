#ifndef RTCALCULATION_H
#define  RTCALCULATION_H
/*********************************************************************************
**********************************************************************************
                          代码说明：兰州项目车厢RT矩阵计算模块
                          版本信息：beta 1.0
                          提交时间：2014/8/10
			              作者：    唐书航
						  存在问题：需后期测试以保证其鲁棒性
***********************************************************************************
*********************************************************************************/
#include <opencv2\opencv.hpp>
#include <opencv\highgui.h>
#include <opencv\cxcore.h>
#include "opencv2/calib3d/calib3d.hpp"
#include <string>
#include <iostream>
#include <vector>
#include <time.h>
#include <math.h>


using namespace cv;
///@to do 添加标定文件路径

class LzCalculationRT
{

public:
	
	Mat pnts3d; //用于计算生成的三维点矩阵

	void calc_single(Mat &img, vector< vector<double> > &sppfs);

	//载入标定数据，计算 remap 数据
	void loadcalib(string filename);
	void loadImgMat(string L_mat,string R_Mat);
	void loadImgMat(Mat L_mat,Mat R_mat);
	// remap, threshold process
	int conv_Center(Mat &img, Point2f center, Mat &kernel);//计算每一个粗提取点的卷积响应

    void coarseCenter(Mat &img, vector<Point2f> &pfs, int color_threshold);

    bool is_match(double point_L,double point_R);

    void subpix_Direct(Mat &img,vector<Point2f> &pfs, vector< vector<double> > &sppfs, int Img_Direct);//计算四个方向的亚像素

    void subpixCenter(Mat &img, vector<Point2f> &pfs , vector< vector<double> > &sppfs);

    void match_pts(vector<double> &Vec_L, vector<double> &Vec_R);

    void match2dpts(Mat img, vector< vector<double> > &sppfs_L, vector< vector<double> > &sppfs_R, 
	            vector<Point2f> &lft_pts, vector<Point2f> &rgt_pts);
	
	int ostu_threshold(Mat& img);

	void calc();
	
private:
	//setup things..
	// load things..	
	int Ver_val,Rt_val,Lt_val,Hor_val,Img_Direct,Light_width; //定义光带宽度
	Mat P1,P2,R1,R2;
	Mat lcameramat,rcameramat,ldistcoeffs,rdistcoeffs;
	Mat R,T;
	Size recsize;
	Mat lmap1, lmap2, rmap1, rmap2;
	Mat kernel_vertic;        //竖直方向模板
    Mat kernel_Rtilt;         //右倾方向模板
    Mat kernel_Ltilt;         //左倾方向模板
    Mat kernel_Hor;           //水平方向模板
	Mat img_L,img_R;   //左右图像，三维点坐标
	double M_Cen_L;
	double M_Cen_R;
	int count;
	
    FileStorage fs;

};


#endif // RTCALCULATION_H