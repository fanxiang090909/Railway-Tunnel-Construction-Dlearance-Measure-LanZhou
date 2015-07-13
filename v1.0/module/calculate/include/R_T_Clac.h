#ifndef RTCALCULATION_H
#define  RTCALCULATION_H
/*********************************************************************************
**********************************************************************************
                          ����˵����������Ŀ����RT�������ģ��
                          �汾��Ϣ��beta 1.0
                          �ύʱ�䣺2014/8/10
			              ���ߣ�    ���麽
						  �������⣺����ڲ����Ա�֤��³����
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
///@to do ��ӱ궨�ļ�·��

class LzCalculationRT
{

public:
	
	Mat pnts3d; //���ڼ������ɵ���ά�����

	void calc_single(Mat &img, vector< vector<double> > &sppfs);

	//����궨���ݣ����� remap ����
	void loadcalib(string filename);
	void loadImgMat(string L_mat,string R_Mat);
	void loadImgMat(Mat L_mat,Mat R_mat);
	// remap, threshold process
	int conv_Center(Mat &img, Point2f center, Mat &kernel);//����ÿһ������ȡ��ľ����Ӧ

    void coarseCenter(Mat &img, vector<Point2f> &pfs, int color_threshold);

    bool is_match(double point_L,double point_R);

    void subpix_Direct(Mat &img,vector<Point2f> &pfs, vector< vector<double> > &sppfs, int Img_Direct);//�����ĸ������������

    void subpixCenter(Mat &img, vector<Point2f> &pfs , vector< vector<double> > &sppfs);

    void match_pts(vector<double> &Vec_L, vector<double> &Vec_R);

    void match2dpts(Mat img, vector< vector<double> > &sppfs_L, vector< vector<double> > &sppfs_R, 
	            vector<Point2f> &lft_pts, vector<Point2f> &rgt_pts);
	
	int ostu_threshold(Mat& img);

	void calc();
	
private:
	//setup things..
	// load things..	
	int Ver_val,Rt_val,Lt_val,Hor_val,Img_Direct,Light_width; //���������
	Mat P1,P2,R1,R2;
	Mat lcameramat,rcameramat,ldistcoeffs,rdistcoeffs;
	Mat R,T;
	Size recsize;
	Mat lmap1, lmap2, rmap1, rmap2;
	Mat kernel_vertic;        //��ֱ����ģ��
    Mat kernel_Rtilt;         //���㷽��ģ��
    Mat kernel_Ltilt;         //���㷽��ģ��
    Mat kernel_Hor;           //ˮƽ����ģ��
	Mat img_L,img_R;   //����ͼ����ά������
	double M_Cen_L;
	double M_Cen_R;
	int count;
	
    FileStorage fs;

};


#endif // RTCALCULATION_H