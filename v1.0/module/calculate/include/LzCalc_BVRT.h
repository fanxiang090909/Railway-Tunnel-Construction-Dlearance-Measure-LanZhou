#ifndef LZCALCULATE_RT_H
#define LZCALCULATE_RT_H

#include <datastructure.h>
#include <filemanagers.h>

#include <opencv2/opencv.hpp>

using namespace cv;
///@todo 添加标定文件路径

class LzCalculate_BVRT
{

public:

    bool init (string calibrationxml_b1, string calibrationxml_b2, string input_camref, __int64 starffc_camref, string input_cam, __int64 starffc_cam, string output, __int64 output_fc);

	//载入标定数据，计算 remap 数据
	void init(string filename, string grp);
	void loadcalib(string filename);

	// remap, threshold process
	void preprocess(Mat& left, Mat& right);

	void extractlight(Mat& img);
	// coarse Center 
	// row scan 
	// single channel
	void coarseCenter(Mat &img, vector<Point2f> &pfs )
	{
		/*pfs.resize(10);*/

		//1. 参数有效性

		//2. 参数预处理

		//3. 计算

		//4. 返回结果
	}
	void subpixCenter(Mat &img, vector<Point2f> &pfs , vector< vector<double> > &sppfs)
	{
		//1. 参数有效性

		//2. 参数预处理

		//3. 计算

		//4. 返回结果
	}
	void match2dpts();

	// single frame
	// triangulate and RT transform
	void calc(Mat left, Mat right, Mat pnts3d) 
	{
		//triangulatePoints(P1,P2,)
	}
	
	/**
	* 停止计算
	*/
// 	bool stop() {
// 		force_stop = true;
// 	}
// 
// 	void start()
// 	{
// 
// 	}

private:
	//setup things..


	// load things..
	Mat P1,P2,R1,R2;
	Mat lcameramat,rcameramat,ldistcoeffs,rdistcoeffs;
	Mat R,T;
	Size recsize;
	Mat lmap1, lmap2, rmap1, rmap2;
};

#endif // LZCALCULATE_RT_H