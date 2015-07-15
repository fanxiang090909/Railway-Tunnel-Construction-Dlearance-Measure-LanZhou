#ifndef __CONVERT_MAT_H
#define __CONVERT_MAT_H

#include <opencv2/opencv.hpp>

using namespace cv;

#define FRAME_COLS (1280)
#define FRAME_ROWS (1024)
#define FRAME_SIZE (FRAME_COLS*FRAME_ROWS)
#define FRAME_TYPE (CV_8UC1)

/**
 * 兰州双目标定采集程序-图像Mat配置
 * @author 丁志宇
 * @date 20141201
 * @version 1.0.0
 */
class ConvertMat{

public:
	ConvertMat() {
		readtemp.create(FRAME_ROWS, FRAME_COLS, FRAME_TYPE);
		rotax.create(FRAME_COLS, FRAME_ROWS, CV_32FC1);
		rotay.create(FRAME_COLS, FRAME_ROWS, CV_32FC1);

		for (int i=0; i<FRAME_COLS; i++)
		{
			for (int j=0; j<FRAME_ROWS; j++)
			{
				rotax.at<float>(i,j) = FRAME_COLS - i;
				rotay.at<float>(i,j) = j;
			}
		}
	}

	void convert(char* data, Mat& m, size_t size = FRAME_SIZE)
	{
		memcpy(readtemp.data, data, size);

		if ( !( m.rows==FRAME_COLS && 
			m.cols==FRAME_ROWS &&
			m.type()==FRAME_TYPE &&
			m.isContinuous() ) )
		{
			m.create(FRAME_COLS, FRAME_ROWS, FRAME_TYPE);
		}

		cv::remap(readtemp, m, rotax, rotay, CV_INTER_LINEAR);
	}

private:
	Mat rotax, rotay;
	Mat readtemp;
};



#endif