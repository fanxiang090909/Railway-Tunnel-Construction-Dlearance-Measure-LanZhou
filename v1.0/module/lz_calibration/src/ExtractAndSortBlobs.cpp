#include "ExtractAndSortBlobs.h"

#define PI 3.14159265

void extractBlobs(const Mat& image, int thresh, Size blobsize, vector<Point2f>& corners)
{
	Mat src, src_1;

	if ( image.type() == CV_8UC1 )
		src = image.clone();
	else if ( image.type() == CV_8UC3 )
		cvtColor(image, src, CV_BGR2GRAY);
	
	medianBlur(src, src, 3);
	src_1 = src.clone();

	threshold(src, src, thresh, 255, THRESH_BINARY);


	vector< vector<Point> > contours;

	Canny(src, src, 10, 255);


	findContours(src, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);


	vector<Rect> boundRect( contours.size() ); 
	vector<vector<Point> > contours_poly( contours.size() ); 

	Mat roi;

	for( int i = 0; i < contours.size(); i++ ) 
	{
		approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true ); 
		boundRect[i] = boundingRect( Mat(contours_poly[i]) );
		roi = src_1( boundRect[i] );
		threshold(roi, roi, 0, 255, THRESH_OTSU);
	}
	threshold(src_1, src_1, 254, 255, THRESH_BINARY);

	// src_1 is the mid result of the preprocess
	// we gonna to extract the circumcircles of the blobs in src_1

	findContours(src_1, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	vector<Point2f>center( contours.size() );
	vector<float>radius( contours.size() );

	for( int i = 0; i < contours.size(); ) 
	{
		if (contours[i].size() < 4)
		{
			contours.erase(contours.begin() + i);
			continue;
		}
		approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true ); 
		minEnclosingCircle( (Mat)contours_poly[i], center[i], radius[i] );
		i++;
	}

	double radiusmax = blobsize.height;
	double radiusmin = blobsize.width;

	for ( int i = 0; i < center.size(); )
	{
		if ( radius[i]>radiusmax || radius[i]<radiusmin )
		{
			center.erase( center.begin() + i );
			radius.erase( radius.begin() + i );
			continue;
		}
		i++;
	}

	corners.assign( center.begin(), center.end() );
}

void drawCorners(Mat& inout, vector<Point2f> corners)
{
	//draw point
	for (int i=0; i<corners.size()-1; i++)
	{
		line(inout, corners[i], corners[i+1], Scalar(0,0,255),4);
	}

}

struct RotaPtsComparer{

	bool operator() (Point2f p1, Point2f p2) {
		p1 = rotaPt(p1);
		p2 = rotaPt(p2);

		if (!colorrow)
			return p1.x < p2.x;
		else
			return p1.y < p2.y;
	}

	bool colorrow;	// true -> compare with colume coordiante, false -> compare with row coordiante;

	void setColOrRow(bool bl) { colorrow = bl; }

	RotaPtsComparer() { 

		colorrow = false;

		rotamat = Mat::zeros( 2, 2, CV_64F);
		originpt = Mat::zeros( 2, 1, CV_64F);
		rotatedpt = Mat::zeros( 2, 1, CV_64F);
	}

	Mat rotamat;
	 // temp variables;
	Mat originpt;
	Mat rotatedpt;

	void setRotatedAngle(double angle) {
		
		double _cos = cos ( angle * PI / 180 );
		double _sin = sin ( angle * PI / 180 );

		rotamat.at<double>(0,0) = _cos;
		rotamat.at<double>(0,1) = -_sin;
		rotamat.at<double>(1,0) = _sin;
		rotamat.at<double>(1,1) = _cos;
	}

	Point2f rotaPt(Point2f p)
	{
		originpt.at<double>(0,0) = p.x;
		originpt.at<double>(1,0) = p.y;

		rotatedpt = rotamat * originpt;

		return Point2f( rotatedpt.at<double>(0,0), rotatedpt.at<double>(1,0) );
	}

};

bool SortRotatedPts(vector<Point2f>& centers, Size patternsize, struct RotaPtsComparer rpc)
{
	rpc.setColOrRow(true);
	sort(centers.begin(), centers.end(), rpc);

	rpc.setColOrRow(false);
	vector<Point2f>::iterator iter = centers.begin();
	for (int i=0; i < patternsize.height; i++)
	{
		sort(iter, iter + patternsize.width, rpc);
		iter += patternsize.width;
	}
	
	Rect rect;
	vector<Point2f>	rectpoints;
	vector<int> countpts(patternsize.height);

	vector<Point2f> rotacenters( centers.size() );
	for ( int i=0; i<rotacenters.size(); i++ )
		rotacenters[i] = rpc.rotaPt(centers[i]);

	for ( int i=0 ; i<patternsize.height; i++ )
	{
		rectpoints.assign( rotacenters.begin() + i*patternsize.width, rotacenters.begin() + (i+1)*patternsize.width );
		rect =  boundingRect( Mat(rectpoints) );

		for ( int j=0; j<rotacenters.size(); j++ )
			if ( rotacenters[j].inside(rect) )
				countpts[i]++;
	};

	for ( int i=0; i<patternsize.height; i++ )
		if ( countpts[i] > patternsize.width )
			return false;

	return true;
}

bool sortPattern(vector<Point2f>& corners, Size patternsize)
{
	RotatedRect minRect; 
	minRect = minAreaRect( Mat(corners) );

	double angle = minRect.angle;

	RotaPtsComparer rpc;

	if ( patternsize.width * patternsize.height != corners.size() )
		return false;


	rpc.setRotatedAngle(-angle);	
	bool goodrota = SortRotatedPts(corners, patternsize, rpc);

	if (!goodrota)
	{
		rpc.setRotatedAngle(90-angle);
		goodrota = SortRotatedPts(corners, patternsize, rpc);
	}

	// if there are still no good
	return goodrota;
}