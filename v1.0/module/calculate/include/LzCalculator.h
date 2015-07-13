/*
*	LzCalculator.h
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

#ifndef LZ_CALCULATOR_H
#define LZ_CALCULATOR_H

#define UNIT_TEST


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

// image amplify 


// calibparas helper class
class calibparas{
public:
	// l->left r->right t->top b->bottom
	int lxlt, lylt, lxrb, lyrb;
	int rxlt, rylt, rxrb, ryrb;
	Mat R,T;
	Mat R1,R2,P1,P2;
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
			CamMat1, Distort1, R1, P1);
		undistortPoints(right, right,
			CamMat1, Distort1, R1, P1);

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

// mark edge helper class
class edgemarker{
public:
	// src.size(), low_thr, high_thr
	// buffer
	// stacktop, stackbottom , stack
	edgemarker() {};
	
	void init(Size _size, int _low, int _high)
	{
		size = _size;
		low = _low;
		high = _high;
		mapstep = size.width + 2;
		buffer.allocate( (size.height+2) * (size.width+2) + mapstep*3*sizeof(int) );
		mag_buf[0] = (int*)(uchar*)buffer;
		mag_buf[1] = mag_buf[0] + mapstep;
		mag_buf[2] = mag_buf[1] + mapstep;

		map = (uchar*)(mag_buf[2] + mapstep);

		maxsize = std::max(1 << 10, size.width * size.height / 10);
		stack.resize(maxsize);
	};

	void markedge(Mat &_amp, Mat &_edge);

private:
	// src.size(), low_thr, high_thr
	// buffer
	// stacktop, stackbottom , stack
	Size size;

	int low;
	int high;

	ptrdiff_t mapstep;
	cv::AutoBuffer<uchar> buffer;
	int* mag_buf[3];
	uchar* map;

	int maxsize;
	std::vector<uchar*> stack;
	uchar **stack_top;
	uchar **stack_bottom;
};

// group class
typedef struct group
{}group;

typedef struct groupholder
{}groupholder;


class LzCalculator
{
public:
	// init()
	// initialize the module by several steps
	// 1. load calibration parameters
	// 2. assemble image rectification parameters, loading image SIZEs
	// 3. allocate buffer for following pipeline
	void init( const char* path );

	vector<vector<double>> L_points;
	vector<vector<double>> R_points;
	vector<double> L_opt_ps;
	vector<double> R_opt_ps;
	vector<Point3i> single_seed;
	vector<int>  record_L; 
	vector<int>  record_R;
	vector<Point2f> lft_pts, rgt_pts;
	bool is_valid;    //用于判断该帧是否有效
	Mat _rect_Pnts3d;
	// calc()
	// input: Mat_<uchar> _LEFT, _RIGHT
	// output: vector<Point3f> pts3f (in railroad coord but not yet rectified)
	// pipeline: calcLightCenters(), calc3Dpts(), coordProject()
	void calc(Mat &_LEFT, Mat &_RIGHT, Mat &_Ledge, Mat &_Redge);
    void pnts_add();
    void pnts_rough_add();               //加入粗提取的当光带。
	calibparas _calibparas;

   	static void argument(Mat& _src, Mat& _dst,
		int _modoffset = 5, int _range = 40);		// amplify images
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
	inline void calc3Dpts()
	{
	   Point2f pts_L,pts_R;
	   for(int i=0;i<L_points.size();i++)
	   {
		  if(L_opt_ps[i]!=-1&&R_opt_ps[i]!=-1)
		  {
			  pts_L.x = L_opt_ps[i]+_calibparas.lxlt;
			  pts_L.y = float(i)+_calibparas.lylt;
			  pts_R.x = R_opt_ps[i]+_calibparas.rxlt;
			  pts_R.y = float(i)+_calibparas.lylt;
			  lft_pts.push_back(pts_L);
			  rgt_pts.push_back(pts_R);
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
	Mat _Pnts3d;

	//
	void coordProject()
	{
		if(_Pnts3d.cols>0)
		{
		_rect_Pnts3d = _calibparas.R * (_Pnts3d.rowRange(0,3));
	    for(int i=0; i<_Pnts3d.cols; i++)
	    {
		   _rect_Pnts3d.at<float>(0,i) += _calibparas.T.at<float>(0,0); 
		   _rect_Pnts3d.at<float>(1,i) += _calibparas.T.at<float>(1,0); 
		   _rect_Pnts3d.at<float>(2,i) += _calibparas.T.at<float>(2,0); 
		}
		}
	};
	Mat _rota, _tran;

	// calcLightCenters() pipeline functions
	inline void rctfndroi(Mat &_left, Mat &_right, Mat &img_L, Mat &img_R)
	{ _calibparas.rctfimgs(_left, _right, img_L, img_R); }	// rectify and roi of originalimages
	// src.type() == CV_8UC1;

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

	void fill_img_front(int _begin, int _end);

	void fill_img_back(int _begin, int _end);

	// build group
	// build match
	// optimize group
	// optimize match
};

#endif