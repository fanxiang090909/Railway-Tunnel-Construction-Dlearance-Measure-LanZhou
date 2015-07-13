#ifndef CALIBPARAMETERS_H
#define CALIBPARAMETERS_H

#include "opencvhead.h"

#define ORIGINAL_WIDTH (512)
#define ORIGINAL_HEIGHT (1280)

/*CalibParameters calibp;
calibp.loadCali("C:\\Users\\dingzhiyu\\Desktop\\Lz_Cali_20141117_A.xml");

Mat orleft, orright;
orleft = imread("C:\\Users\\dingzhiyu\\Desktop\\Aleft\\6169.jpg", 0);
orright = imread("C:\\Users\\dingzhiyu\\Desktop\\Aright\\6169.jpg", 0);


calibp.rctfImage(orleft, orright, subleft, subright);
*/

class CalibParameters
{
public:
	
	// l->left r->right t->top b->bottom
	int lxlt, lylt, lxrb, lyrb;
	int rxlt, rylt, rxrb, ryrb;
	Mat R,T;
    Mat R1,R2,P1,P2;
	void loadCali(const char* path)
	{
		FileStorage fs( path, FileStorage::READ );

		/*
		check path valid
		Is it necessary to throw exception?
		if ( !fs.isOpened() )
		return;
		*/

		// read all useful parameters from xml
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

		// do generate remap by calling initUndistortRectifyMap
		/*
		initUndistortRectifyMap(lcameramat, ldistcoeffs, R1, P1, recsize, CV_32FC1, lmap1, lmap2);
		*/
		generateRemap();

		// do calculate roi of rectified images
		// this could be done at calibrating period
		// but doing in there is also not bad
		generateRoi();
	}

	// rectify image
	// all debug and test
	void rctfImage( Mat& orgn_left, Mat& orgn_right, Mat& rctf_left, Mat& rctf_right)
	{
		// check image format
		// @todo
		//

		// fill full resolution
		// it can be done faster
		Mat full_left = Mat::zeros(1280, 1024, orgn_left.type());
		Mat full_right = Mat::zeros(1280, 1024, orgn_right.type());
		orgn_left.copyTo(full_left.colRange(512,1024));
		orgn_right.copyTo(full_right.colRange(0,512));

		// remap
		Mat rctf_full_left, rctf_full_right;
		remap( full_left, rctf_full_left, lremap1, lremap2, INTER_LINEAR);
		remap( full_right, rctf_full_right, rremap1, rremap2, INTER_LINEAR);

		////debug
		//Mat tmpleft, tmpright;
		//cvtColor(rctf_full_left, tmpleft, CV_GRAY2BGR);
		//cvtColor(rctf_full_right, tmpright, CV_GRAY2BGR);

		//line(tmpleft, Point(lxlt, lylt), Point(lxrb, lylt), CV_RGB(255,0,0));
		//line(tmpleft, Point(lxrb, lylt), Point(lxrb, lyrb), CV_RGB(255,0,0));
		//line(tmpleft, Point(lxrb, lyrb), Point(lxlt, lyrb), CV_RGB(255,0,0));
		//line(tmpleft, Point(lxlt, lyrb), Point(lxlt, lylt), CV_RGB(255,0,0));

		//line(tmpright, Point(rxlt, rylt), Point(rxrb, rylt), CV_RGB(255,0,0));
		//line(tmpright, Point(rxrb, rylt), Point(rxrb, ryrb), CV_RGB(255,0,0));
		//line(tmpright, Point(rxrb, ryrb), Point(rxlt, ryrb), CV_RGB(255,0,0));
		//line(tmpright, Point(rxlt, ryrb), Point(rxlt, rylt), CV_RGB(255,0,0));
		//
		//imwrite("C:\\Users\\dingzhiyu\\Desktop\\rctf_full_left.bmp", tmpleft);
		//imwrite("C:\\Users\\dingzhiyu\\Desktop\\rctf_full_right.bmp", tmpright);

		// get sub img
		rctf_left = rctf_full_left.colRange(lxlt, lxrb).rowRange(lylt, lyrb).clone();
		rctf_right = rctf_full_right.colRange(rxlt, rxrb).rowRange(rylt, ryrb).clone();
	}

private:
	//
	void generateRemap()
	{
		initUndistortRectifyMap(CamMat1, Distort1, R1, P1, rctfsize, CV_16SC2, lremap1, lremap2);
		initUndistortRectifyMap(CamMat2, Distort2, R2, P2, rctfsize, CV_16SC2, rremap1, rremap2);
	}

	// 因为摄像机被车厢遮挡（相机孔开的不到位），或许应该交互式的修改roi
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

	//load 
	Mat Distort1,Distort2;
	Mat CamMat1, CamMat2;
	Size rctfsize;
	//Size 

	// remap
	Mat lremap1, lremap2;
	Mat rremap1, rremap2;

};

#endif 