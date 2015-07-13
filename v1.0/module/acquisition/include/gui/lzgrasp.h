#ifndef LZGRASP_H
#define LZGRASP_H

#ifdef _DEBUG

#pragma comment(lib, "opencv_core249d.lib")
#pragma comment(lib, "opencv_highgui249d.lib")
#pragma comment(lib, "opencv_imgproc249d.lib")
#pragma comment(lib, "opencv_features2d249d.lib")
#pragma comment(lib, "opencv_calib3d249d.lib")

#else

#pragma comment(lib, "opencv_core249.lib")
#pragma comment(lib, "opencv_highgui249.lib")
#pragma comment(lib, "opencv_imgproc249.lib")
#pragma comment(lib, "opencv_features2d249.lib")
#pragma comment(lib, "opencv_calib3d249.lib")

#endif

#include <QtGui/QMainWindow>
#include "MatToQImage.h"
#include "ui_lzgrasp.h"
#include <opencv2\opencv.hpp>
using namespace cv;

#include <direct.h>
#include <sstream>
#include <string>
#include <iostream>
using namespace std;

#include "network_config_list.h"
#include "InitCamera.h"
#include "ConvertMat.h"
using namespace Pylon;

/**
 * 兰州双目标定采集程序界面
 * @author 丁志宇
 * @date 20141201
 * @version 1.0.0
 */
class LzGrasp : public QWidget
{
	Q_OBJECT

public:
	LzGrasp(QWidget *parent = 0, Qt::WFlags flags = 0);
	~LzGrasp();

private slots:
	void comfirmSetup();
	void graspframe();
	void saveframe();

    void sellectCamGroup(int);
private:
	Ui::LzGraspClass ui;

	Mat leftimg, rightimg;

	Size patternsize;
	Size blobsize;
	int graythreshold;

	int exposuretime;
	string left_camseq, right_camseq;
	Mat cur_left, cur_right;

	cv::Ptr<cv::FeatureDetector> blob_detector;

	stringstream ss;
	string lpath;
	string rpath;
	string suffix;
	string count;
	int save_count;

	int left_index, right_index;

	//convertMat
	ConvertMat cvt;

	//pylon stuffs
	PylonAutoInitTerm autoInitTerm;
	CBaslerGigEInstantCameraArray cameras;
	CBaslerGigEGrabResultPtr lptrGrabResult;
	CBaslerGigEGrabResultPtr rptrGrabResult;

    // 读Network config xml文件
    SlaveModel slaveModel;
    bool networkfileok;

};

#endif // LZGRASP_H
