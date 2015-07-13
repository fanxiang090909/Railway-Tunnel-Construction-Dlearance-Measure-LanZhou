#ifndef OPENCV_HEAD_H
#define OPENCV_HEAD_H

#include <opencv2\opencv.hpp>
using namespace cv;
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


#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

class Writeimg{
public:
	Writeimg() : 
		suffix(".bmp"), count(0), ss(""){}
	Writeimg(const Writeimg &_wi) { count = _wi.count; suffix = ".bmp"; }

	void operator()( Mat &img )
	{
		if (img.empty())
			return;

		ss.str("");
		ss << count++;
		string tmp = ss.str() + suffix;
		imwrite(tmp, img);
	}

private:
	std::string suffix;
	std::stringstream ss;
	int count;

};

void writeImgsVec( std::vector<Mat> &imgs )
{
	if (imgs.empty())
		return;
	
	Writeimg writeimg;
	for_each(imgs.begin(), imgs.end(), writeimg);
}


#endif
