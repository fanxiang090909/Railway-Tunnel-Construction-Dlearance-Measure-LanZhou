#ifndef LZ_CALCULATION_TWOVIEW
#define LZ_CALCULATION_TWOVIEW

/*/////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
                   
                   代码说明：兰州项目双目计算模块
	               版本信息：beta 1.0   
	               提交时间：2014/8/11
	               作者：    唐书航
				   存在问题：（1）双目计算结果的存储尚有问题。
				             （2）对三维点的Rt矫正须实测调试以验证其精度

/////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////*/
#include <opencv2\opencv.hpp>
#include <opencv\highgui.h>
#include <opencv\cxcore.h>
#include "opencv2/calib3d/calib3d.hpp"
#include <string>
#include <iostream>
#include <vector>
#include <time.h>
#include <math.h>

#define REC_IMG_SIZE_WID 1200
#define REC_IMG_SIZE_HEI 1500
using namespace cv;
///@todo 添加标定文件路径

class LzCalculation
{

public:

	Mat pnts3d;                                            //用于返回3维点矩阵
	Mat rect_pnts3d;                                        //Rt修正mat
    bool is_valid;

	void calc_single(Mat &img, Mat& Full_img, vector< vector<double> > &sppfs, Mat& map1, Mat& map2,int flag);
	//载入标定数据
	void loadcalib(string filename,int flag);

	void loadImgMat(string L_mat,string R_mat);            //载入左右图像矩阵

	void loadImgMat(Mat L_mat,Mat R_mat);

	void rectify_fenzhong(Mat &P,Mat &rect_P);

	int conv_Center(Mat &img, Point2f center, Mat &kernel);//计算每一个粗提取点的卷积响应

    void coarseCenter(Mat &img, vector<Point2f> &pfs, int color_threshold);

    bool is_match(double point_L,double point_R);

    void subpix_Direct(Mat &img,vector<Point2f> &pfs, vector< vector<double> > &sppfs, int Img_Direct);//计算四个方向的亚像素

    void subpixCenter(Mat &img, vector<Point2f> &pfs , vector< vector<double> > &sppfs);

    void match_pts(vector<double> &Vec_L, vector<double> &Vec_R);

    void match2dpts(Mat img, vector< vector<double> > &sppfs_L, vector< vector<double> > &sppfs_R, 
	            vector<Point2f> &lft_pts, vector<Point2f> &rgt_pts);
	
	int ostu_threshold(Mat& img);                              //Ostu计算分割伐值
	// single frame
	// triangulate and RT transform
	void calc();
 
	
private:	
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
	Mat img_L,img_R;          //左右图像，三维点坐标
	Mat Full_L_img,Full_R_img;//全尺寸左右图
	Mat Trans_R,Trans_T;      //分中转换的旋转，平移矩阵
	double M_Cen_L;           //当前左图的中心点
	double M_Cen_R;           //当前右图的中心点
	int valid_thres;
	int count;
	
    FileStorage fs;
};

#endif // LZ_CALCULATION_TWOVIEW