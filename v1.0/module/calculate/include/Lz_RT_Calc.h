/*
*	Lz_RT_Clac.h
*	
*	兰州项目 计算功能实现
*
*	模块功能：
*
*
*	说明：
*
*	Author: Ding(dingzhiyuu@gmail.com)
*	date: 2014.12.22
*
*/

#ifndef LZ_CALCULATOR_RT_H
#define LZ_CALCULATOR_RT_H

#define UNIT_TEST

#include "LzCalculator.h"

//using opencv helper
#include <opencv2\opencv.hpp>
#include <iostream>
#include <fstream>
using namespace cv;
using namespace std;
#ifdef _DEBUG
#pragma comment(lib, "opencv_core249d.lib")
#pragma comment(lib, "opencv_highgui249d.lib")
#pragma comment(lib, "opencv_imgproc249d.lib")
#pragma comment(lib, "opencv_calib3d249d.lib")
#else
#pragma comment(lib, "opencv_core249.lib")
#pragma comment(lib, "opencv_highgui249.lib")
#pragma comment(lib, "opencv_imgproc249.lib")
#pragma comment(lib, "opencv_calib3d249.lib")
#endif

#define RAIL_PNTS_Min 600          //用于定义寻找轨点时搜索的最小起始范围
#define RAIL_PNTS_Max 900          //用于定义寻找轨点时搜索的最大结束范围
#define RAIL_H_Max  1050
// image amplify 


// calibparas helper class
class calibparasRT{
public:
	// l->left r->right t->top b->bottom
	int lxlt, lylt, lxrb, lyrb;
	int rxlt, rylt, rxrb, ryrb;
	Mat R,T;
	Mat R1,R2,P1,P2;
	Mat standard_P;      //标准三维点
	void loadcali(string path){

        bool ret = fs.open(path, FileStorage::READ);

        if ( !fs.isOpened() )
        {
            // throw exception;
            std::cout<<"Can't open the Calib_file!";
            return;
        }

		// exception~
		// try catch
		fs["ldistcoeffs"] >> Distort1;
		fs["rdistcoeffs"] >> Distort2;
		fs["lcameramat"] >> CamMat1;
		fs["rcameramat"] >> CamMat2;
		fs["R1"] >> R1;
		fs["R2"] >> R2;
		fs["P1"] >> P1;
		fs["P2"] >> P2;
		fs["standard_point"] >> standard_P;
		fs["R"] >> R;
		fs["T"] >> T;
		fs["recsizewid"] >> rctfsize.width;
		fs["recsizehei"] >> rctfsize.height;

	}	//exception?
	void calcRemap(){
		initUndistortRectifyMap(CamMat1, Distort1, R1, P1, rctfsize, CV_16SC2, lremap1, lremap2);
		initUndistortRectifyMap(CamMat2, Distort2, R2, P2, rctfsize, CV_16SC2, rremap1, rremap2);
		generateRoi();
	}
	#define ORIGINAL_WIDTH (512)
	#define ORIGINAL_HEIGHT (1280)
	void generateRoi()
	{
		// generate Roi Box Point from original images
		vector<Point2f> left, right;
		//horizontal
		for (int i=0; i<ORIGINAL_WIDTH; i++)
		{
			left.push_back( Point2f(ORIGINAL_WIDTH+i, 0) );
			left.push_back( Point2f(ORIGINAL_WIDTH+i, 1280) );
			right.push_back( Point2f(i, 0) );
			right.push_back( Point2f(i, 1280) );
		}
		//vertical
		for (int i=0; i<ORIGINAL_HEIGHT; i++)
		{
			left.push_back( Point2f(ORIGINAL_WIDTH, i) );
			left.push_back( Point2f(ORIGINAL_WIDTH + ORIGINAL_WIDTH, i) );
			right.push_back( Point2f(0, i) );
			right.push_back( Point2f(ORIGINAL_WIDTH, i) );
		}

		// transform those points to rectified image coordinates;
		// UndistortPoints()
		undistortPoints(left, left,
			CamMat2, Distort1, R1, P1);
		undistortPoints(right, right,
			CamMat2, Distort1, R1, P1);

		// Bounding Box;
		Rect lrect = boundingRect(left);
		Rect rrect = boundingRect(right);

		// rectify bounding box
		lxlt = lrect.x; lylt = lrect.y; 
		lxrb = lxlt + lrect.width; lyrb = lylt + lrect.height;
		rxlt = rrect.x; rylt = rrect.y; 
		rxrb = rxlt + rrect.width; ryrb = rylt + rrect.height;

		lylt = rylt = std::max( lylt, rylt );
		lyrb = ryrb = std::min( lyrb, ryrb );

	}
	inline void rctfimgs(Mat &_left, Mat &_right, Mat &img_L, Mat &img_R){

		Mat full_left = Mat::zeros(1280, 1024, _left.type());
		Mat full_right = Mat::zeros(1280, 1024, _right.type());
		_left.copyTo(full_left.colRange(512,1024));
		_right.copyTo(full_right.colRange(0,512));

		// remap
		Mat rctf_full_left, rctf_full_right;
		remap( full_left, rctf_full_left, lremap1, lremap2, INTER_LINEAR);
		remap( full_right, rctf_full_right, rremap1, rremap2, INTER_LINEAR);

		cvtColor(rctf_full_left , img_L ,CV_GRAY2BGR);
	    cvtColor(rctf_full_right, img_R ,CV_GRAY2BGR);
		
	    _left = rctf_full_left.colRange(lxlt, lxrb).rowRange(lylt, lyrb).clone();
		_right = rctf_full_right.colRange(rxlt, rxrb).rowRange(rylt, ryrb).clone();
//		_left = rctf_full_left;
//		_right = rctf_full_right;

	};
	inline Size lsize() { return Size(lxrb-lxlt,lyrb-lylt); }
	inline Size rsize() { return Size(rxrb-rxlt,ryrb-rylt); }
private:
	//load 
	Mat Distort1,Distort2;
	Mat CamMat1, CamMat2;
	Size rctfsize;
	//Size 

	// remap
	Mat lremap1, lremap2;
	Mat rremap1, rremap2;

    FileStorage fs;
};

class Lz_RT_Clac
{
public:
	// init()
	// initialize the module by several steps
	// 1. load calibration parameters
	// 2. assemble image rectification parameters, loading image SIZEs
	// 3. allocate buffer for following pipeline
	void init( const char* path );
    Mat _Pnts3d;
	vector<vector<double>> L_points;
	vector<vector<double>> R_points;
	vector<double> L_rail_ps;
	vector<double> R_rail_ps;
	vector<double> L_opt_ps,R_opt_ps;
	vector<Point3i> single_seed;
	vector<int>  record_L; 
	vector<int>  record_R;
	vector<int>  pnts_index;               //用于记录铁轨点在地面的位置
	vector<Point2f> lft_pts, rgt_pts;
	Mat _rect_Pnts3d;
	bool is_valid;                  //用于判断当前图像是否有效
	Point2d rail_P;                 //记录铁轨的高度和水平位置
	Point2d std_P;                  //记录标准轨高度和水平位置
	Point2d valid_rail_P;           //记录有效轨高度和水平位置
	// calc()
	// input: Mat_<uchar> _LEFT, _RIGHT
	// output: vector<Point3f> pts3f (in railroad coord but not yet rectified)
	// pipeline: calcLightCenters(), calc3Dpts(), coordProject()
	void calc(Mat &_LEFT, Mat &_RIGHT, Mat &_Ledge, Mat &_Redge, int flag);
	calibparasRT _calibparas;
    vector<Point2f> record;            //用于记录轨点的数据

/*
#ifdef UNIT_TEST
	void init( const char* path ) 
	{ 
		_calibparas.loadcali(path);
		_calibparas.calcRemap();

		_Lem.init( _calibparas.lsize(), 12, 48 ); 
		_Rem.init( _calibparas.rsize(), 12, 48 );
	}
	inline void unitt( Mat &_LEFT, Mat &_RIGHT, Mat &_Ledge, Mat &_Redge ) 
	{
		Mat img_L,img_R;
		rctfndroi(_LEFT, _RIGHT, img_L ,img_R);


		Mat _Lamp, _Ramp;
		argument(_LEFT, _Lamp);
		argument(_RIGHT, _Ramp); 

		GaussianBlur(_Lamp, _Lamp, Size(5,5), 1.5, 1.5);
		GaussianBlur(_Ramp, _Ramp, Size(5,5), 1.5, 1.5);

		_Lem.markedge( _Lamp, _Ledge );
		_Rem.markedge( _Ramp, _Redge );

		Mat _LP,_RP,_LsubP,_RsubP;
		L_points.resize(_Ledge.rows,vector<double>(0));
		R_points.resize(_Redge.rows,vector<double>(0));
		record_L.resize(_Ledge.rows,0);
		record_R.resize(_Redge.rows,0);
		L_opt_ps.resize(_Ledge.rows,-1);
        R_opt_ps.resize(_Redge.rows,-1);
		calccenters(_Ledge, _LEFT, _LP, _LsubP,1);
		calccenters(_Redge, _RIGHT, _RP, _RsubP,2);
     	center_match(L_points,R_points,L_opt_ps,R_opt_ps);
		calc3Dpts();
		coordProject();
	    ofstream SaveFile("C:\\Users\\Administrator\\Desktop\\Output.txt");
		for(int i=0; i<_rect_Pnts3d.cols; i++)
	    {
			SaveFile<<"第"<<i<<"点坐标："<<endl;
			SaveFile<<"高度： "<<*_rect_Pnts3d.ptr<float>(0,i)<<" 原点:"<<*_Pnts3d.ptr<float>(0,i)<<endl;
			SaveFile<<"水平： "<<*_rect_Pnts3d.ptr<float>(1,i)<<" 原点:"<<*_Pnts3d.ptr<float>(1,i)<<endl;
			SaveFile<<"Z坐标："<<*_rect_Pnts3d.ptr<float>(2,i)<<" 原点:"<<*_Pnts3d.ptr<float>(2,i)<<endl;
			SaveFile<<endl;

		}
		SaveFile.close();
		for(int i=0;i<lft_pts.size();i++)
		{
			img_L.at<Vec3b>(int(lft_pts[i].y),int(lft_pts[i].x)) = Vec3b(0,255,0);
			//cout<<i<<endl;
		}
		for(int i=0;i<rgt_pts.size();i++)
		{
			img_R.at<Vec3b>(int(rgt_pts[i].y),int(rgt_pts[i].x)) = Vec3b(0,255,0);
			//cout<<i<<endl;
		}
		imwrite("C:\\Users\\Administrator\\Desktop\\标定数据_1127\\代码_TSH_1125\\兰州代码11_13\\Lz_Calc\\L1.bmp",img_L);
		imwrite("C:\\Users\\Administrator\\Desktop\\标定数据_1127\\代码_TSH_1125\\兰州代码11_13\\Lz_Calc\\R1.bmp",img_R);
        imwrite("L1.bmp",_LP);
		imwrite("R1.bmp",_RP);
		L_opt_ps.clear();
		R_opt_ps.clear();
		L_points.clear();
		R_points.clear();
		single_seed.clear();
		record_L.clear();
		record_R.clear();
	    lft_pts.clear();
	    rgt_pts.clear();
	}
#endif
*/
private:
	// calcLightCenters()
	// pipeline:
	// 
	void calcLightCenters(){};
	//	call: void triangulatePoints(InputArray projMatr1, InputArray projMatr2,
	//	InputArray projPoints1, InputArray projPoints2, OutputArray points4D)
	inline void rail_pnts_detect(int flag);
        
    inline void calc3Dpts()
	{
	   Point2f pts_L,pts_R;
	   for(int i=0;i<L_points.size();i++)
	   {
		   if(L_opt_ps[i]!=-1&&R_opt_ps[i]!=-1)        //判断条件L_op_ps时加入地面三维点，rail_pnts时判断铁轨三维点
		  {
			  pts_L.x = L_opt_ps[i]+_calibparas.lxlt;
			  pts_L.y = float(i)+_calibparas.lylt;
			  pts_R.x = R_opt_ps[i]+_calibparas.rxlt;
			  pts_R.y = float(i)+_calibparas.lylt;
			  lft_pts.push_back(pts_L);
			  rgt_pts.push_back(pts_R);
			  /*if(L_rail_ps[i]!=-1&&R_rail_ps[i]!=-1)
			  {
				  pnts_index.push_back(-1);
			  }
			  else
				  pnts_index.push_back(1);*/
		  }
		  
	   }
	   if(lft_pts.size()>0)
	   {
	      _Pnts3d = Mat(4,lft_pts.size(),CV_64FC4);
	      _rect_Pnts3d = Mat(4,lft_pts.size(),CV_64FC4);
	      triangulatePoints(_calibparas.P1, _calibparas.P2, lft_pts, rgt_pts, _Pnts3d);  //计算三维点函数
	      _Pnts3d.row(0) /=  _Pnts3d.row(3);
	      _Pnts3d.row(1) /=  _Pnts3d.row(3);
	      _Pnts3d.row(2) /=  _Pnts3d.row(3);
	      _Pnts3d.row(3) /=  _Pnts3d.row(3);
		  is_valid = true;
	   }

	};
	Mat _LP, _RP;

	//
	void coordProject(bool flag)
	{
        float temp;                             //x，y坐标互换。
		if(_Pnts3d.cols>0)
		{
		   _rect_Pnts3d = _calibparas.R * (_Pnts3d.rowRange(0,3));
	       for(int i=0; i<_Pnts3d.cols; i++)
	       {
		      _rect_Pnts3d.at<float>(0,i) += _calibparas.T.at<float>(0,0); 
              _rect_Pnts3d.at<float>(1,i) += _calibparas.T.at<float>(1,0);
		      _rect_Pnts3d.at<float>(2,i) += _calibparas.T.at<float>(2,0); 
    //          temp = _rect_Pnts3d.at<float>(0,i);
    //          _rect_Pnts3d.at<float>(0,i) = _rect_Pnts3d.at<float>(1,i);
    //          _rect_Pnts3d.at<float>(1,i) = temp;

		   }
		   /*for(int i=0;i<_Pnts3d.cols;i++)
		   {
			  if(pnts_index[i]==-1)                         //判断是否为钢轨点
			  {
				  _rect_Pnts3d.at<float>(2,i) = -1;
			  }
		   }*/
		   pnts_index.clear();
		}
	};
	Mat _rota, _tran;

	// calcLightCenters() pipeline functions
	inline void rctfndroi(Mat &_left, Mat &_right, Mat &img_L, Mat &img_R)
	{ _calibparas.rctfimgs(_left, _right, img_L, img_R); }	// rectify and roi of originalimages
	// src.type() == CV_8UC1;
	void argument(Mat& _src, Mat& _dst,
		int _modoffset = 15, int _range = 45)		// amplify images
    {
        LzCalculator::argument(_src, _dst, _modoffset, _range);
    }
	// edge marker
	edgemarker _Lem, _Rem;
	
	// calc centers helper function
	inline int assigncenters(int _l, int _r, uchar* rc, uchar* _pp, float* _sbp){
		int sum1=0, sum2=0;
		for (int i=_l; i<_r; i++)
		{
			sum1 += i*rc[i];
			sum2 += rc[i];
		}
		float _subp = (float)sum1 / (float)sum2;

//		std::cout << "hahhaha" << std::endl;
		int c = (int)(_subp+0.5);

		
///		std::cout << c << std::endl;
		if(sum2!=0)
		{
		_pp[c] = 255;
		_sbp[c] = _subp;
		return c;
		}
		else
			return -1;
//		std::cout << "xixixixi" << std::endl;

		
	}
	// calc centers
	// Mat_<uchar> _edge, Mat_<uchar> _RctfImg
	// create Mat_<uchar> _P;	// 255 center, 0 not center
	// create Mat_<float> _subP;
	// threshold: the widest value of a band
	void calccenters(Mat& _edge, Mat& _RctfImg, Mat& _P, Mat& _subP,int flag);

	void center_match(vector<vector<double>> &L_points,vector<vector<double>> &R_points,vector<double>& L_opt_ps,vector<double>& R_opt_ps);

	void seed_fill(vector<Point3i>& seed);

	void fill_img_front(int _begin, int _end);                   //向上填充种子
	                                          
	void fill_img_back(int _begin, int _end);                    //向下填充种子

	void pnts_add();                                             //加入不确定点

	void get_rail_ps(Mat &img,vector<double> &rail_pfs,vector<double> &sppfs);                //截取轨面轮廓！

	void get_rail(Mat& rect_pnts3d);

	// build group
	// build match
	// optimize group
	// optimize match
};

#endif // LZ_CALCULATOR_RT_H