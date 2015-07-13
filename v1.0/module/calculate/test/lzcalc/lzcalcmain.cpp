#include <opencv2/opencv.hpp>
#include <time.h>
#include <vector>

#include "../../include/LzCalc.h"

using namespace std;
using namespace cv;




int main(int argc, char* argv)
{
	Mat img = imread("C:\\Users\\dingzhiyu\\Desktop\\20131104\\1_5.bmp", 0);
	Mat thr;

	LzCalculate lzcalc;
	// init
		// setup, init index, ...
	
	// load img

	// coarse

	// subpix

	// output result

	int rowindex[1280];
	for(int i=0; i<1280; i++)
		rowindex[i] = i*img.rows;

	img.copyTo(thr);

	vector<Point2i> vecp;
	uchar threshold = 0;

	clock_t  clockBegin, clockEnd;   
	clockBegin = clock();   

	

	clockEnd = clock();   
	printf("clock %d\n", clockEnd - clockBegin);

	imshow("win1", thr);
	waitKey(0);



	return 1;
}