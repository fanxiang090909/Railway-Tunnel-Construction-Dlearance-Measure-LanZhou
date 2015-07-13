#ifndef LZCALCULATE_BV_H
#define LZCALCULATE_BV_H

#include <datastructure.h>
#include <filemanagers.h>

#include <opencv2/opencv.hpp>

using namespace cv;
///@todo 添加标定文件路径

class LzCalculate_BV 
{

public:
	bool init (string calibrationxml, string input_camref, __int64 starffc_camref, string input_cam, __int64 starffc_cam, string output, __int64 output_fc);

	Mat pnts3d;                                            //用于返回3维点矩阵
	Mat rect_pnts3d;                                        //Rt修正mat

	void calc_single(Mat &img, vector< vector<double> > &sppfs);
	//载入标定数据
	void loadcalib(string filename);

	void loadImgMat(string L_mat,string R_mat);            //载入左右图像矩阵

	void loadImgMat(Mat L_mat,Mat R_mat);

	void rectifyRT(Mat &P);

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
	Mat img_L,img_R;   //左右图像，三维点坐标
	double M_Cen_L;
	double M_Cen_R;
	bool is_valid;
	int valid_thres;
	int count;
	
};

#endif // LZCALCULATE_BV_H