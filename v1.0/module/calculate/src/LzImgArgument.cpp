#include "LzCalculator.h"

//helper class copy from OPENCV
class EqualizeHistCalcHist_Invoker : public cv::ParallelLoopBody
{
public:
    enum {HIST_SZ = 256};

    EqualizeHistCalcHist_Invoker(cv::Mat& src, int* histogram, cv::Mutex* histogramLock)
        : src_(src), globalHistogram_(histogram), histogramLock_(histogramLock)
    { }

    void operator()( const cv::Range& rowRange ) const
    {
        int localHistogram[HIST_SZ] = {0, };

        const size_t sstep = src_.step;

        int width = src_.cols;
        int height = rowRange.end - rowRange.start;

        if (src_.isContinuous())
        {
            width *= height;
            height = 1;
        }

        for (const uchar* ptr = src_.ptr<uchar>(rowRange.start); height--; ptr += sstep)
        {
            int x = 0;
            for (; x <= width - 4; x += 4)
            {
                int t0 = ptr[x], t1 = ptr[x+1];
                localHistogram[t0]++; localHistogram[t1]++;
                t0 = ptr[x+2]; t1 = ptr[x+3];
                localHistogram[t0]++; localHistogram[t1]++;
            }

            for (; x < width; ++x)
                localHistogram[ptr[x]]++;
        }

        cv::AutoLock lock(*histogramLock_);

        for( int i = 0; i < HIST_SZ; i++ )
            globalHistogram_[i] += localHistogram[i];
    }

    static bool isWorthParallel( const cv::Mat& src )
    {
        return ( src.total() >= 640*480 );
    }

private:
    EqualizeHistCalcHist_Invoker& operator=(const EqualizeHistCalcHist_Invoker&);

    cv::Mat& src_;
    int* globalHistogram_;
    cv::Mutex* histogramLock_;
};

//helper class copy from OPENCV
class EqualizeHistLut_Invoker : public cv::ParallelLoopBody
{
public:
    EqualizeHistLut_Invoker( cv::Mat& src, cv::Mat& dst, int* lut )
        : src_(src),
          dst_(dst),
          lut_(lut)
    { }

    void operator()( const cv::Range& rowRange ) const
    {
        const size_t sstep = src_.step;
        const size_t dstep = dst_.step;

        int width = src_.cols;
        int height = rowRange.end - rowRange.start;
        int* lut = lut_;

        if (src_.isContinuous() && dst_.isContinuous())
        {
            width *= height;
            height = 1;
        }

        const uchar* sptr = src_.ptr<uchar>(rowRange.start);
        uchar* dptr = dst_.ptr<uchar>(rowRange.start);

        for (; height--; sptr += sstep, dptr += dstep)
        {
            int x = 0;
            for (; x <= width - 4; x += 4)
            {
                int v0 = sptr[x];
                int v1 = sptr[x+1];
                int x0 = lut[v0];
                int x1 = lut[v1];
                dptr[x] = (uchar)x0;
                dptr[x+1] = (uchar)x1;

                v0 = sptr[x+2];
                v1 = sptr[x+3];
                x0 = lut[v0];
                x1 = lut[v1];
                dptr[x+2] = (uchar)x0;
                dptr[x+3] = (uchar)x1;
            }

            for (; x < width; ++x)
                dptr[x] = (uchar)lut[sptr[x]];
        }
    }

    static bool isWorthParallel( const cv::Mat& src )
    {
        return ( src.total() >= 640*480 );
    }

private:
    EqualizeHistLut_Invoker& operator=(const EqualizeHistLut_Invoker&);

    cv::Mat& src_;
    cv::Mat& dst_;
    int* lut_;
};

//
void LzCalculator::argument(Mat& _src, Mat& _dst, int _modoffset, int _range)
{
    _dst.create( _src.size(), _src.type() );

    Mutex histogramLockInstance;

    const int hist_sz = EqualizeHistCalcHist_Invoker::HIST_SZ;
    int hist[hist_sz] = {0,};
    int lut[hist_sz];

    EqualizeHistCalcHist_Invoker calcBody(_src, hist, &histogramLockInstance);
    EqualizeHistLut_Invoker      lutBody(_src, _dst, lut);
    cv::Range heightRange(0, _src.rows);

	// calculate histogram
    parallel_for_(heightRange, calcBody);
    
	// modify pixel map relationship from src to dst
	int i;

	int _max = 0, _mod = 0;
	for ( i = 0; i < 50; ++i )
		if ( hist[i]>_max ) _max = hist[i], _mod = i;

	int rdlow = _mod + _modoffset;
	int rdhigh = (rdlow + _range) > 255 ? 255 : (rdlow + _range);

	int total = 0;
	for ( i=rdlow; i<rdhigh; i++)
		total += hist[i];

    float scale = (hist_sz - 1.f)/total;
    int sum = 0;
	i = 0;

	while(i<rdlow) lut[i++] = 0;
	for ( ; i < rdhigh; ++i )
	{
		sum += hist[i];
        lut[i] = saturate_cast<uchar>(sum * scale);
	}
	while(i<hist_sz) lut[i++] = 255;
	////////////////////////////////////////////////////////

	//apply mapping
    parallel_for_(heightRange, lutBody);

}