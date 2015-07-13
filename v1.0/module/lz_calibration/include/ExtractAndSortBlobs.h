#ifndef EXTRACTANDSORTBLOBS_H
#define EXTRACTANDSORTBLOBS_H

#include <opencv/cv.h>
#include <opencv/highgui.h>

using namespace cv;

void extractBlobs(const Mat& image, int thresh, Size blobsize, vector<Point2f>& corners);

bool sortPattern(vector<Point2f>& corners, Size patternsize);

void drawCorners(Mat& inout, vector<Point2f> corners);

#endif