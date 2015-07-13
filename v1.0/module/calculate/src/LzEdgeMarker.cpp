#include "LzCalculator.h"

// this function is modified from OPENCV SOURCE CODE canny() --Ding
void edgemarker::markedge( Mat &_amp, Mat &_edge )
{
	_edge.create( _amp.size(), CV_8U );

	Mat dx,dy;

	cv::Sobel(_amp, dx, CV_16S, 1, 0, 3, 1, 0, cv::BORDER_REPLICATE);
    cv::Sobel(_amp, dy, CV_16S, 0, 1, 3, 1, 0, cv::BORDER_REPLICATE);

	memset(mag_buf[0], 0, mapstep*sizeof(int));
	memset(map, 1, mapstep);
	memset(map + mapstep*(size.height + 1), 1, mapstep);

	stack_top = stack_bottom = &stack[0];

	/* sector numbers
       (Top-Left Origin)

        1   2   3
         *  *  *
          * * *
        0*******0
          * * *
         *  *  *
        3   2   1
    */

	#define CANNY_PUSH(d)    *(d) = uchar(128), *stack_top++ = (d)
    #define CANNY_POP(d)     (d) = *--stack_top

	// calculate magnitude and angle of gradient, perform non-maxima supression.
    // fill the map with one of the following values:
    //   0 - the pixel might belong to an edge
    //   1 - the pixel can not belong to an edge
    //   128 - the pixel does belong to an edge
	 for (int i = 0; i <= _amp.rows; i++)
    {
        int* _norm = mag_buf[(i > 0) + 1] + 1;
        if (i < _amp.rows)
        {
            short* _dx = dx.ptr<short>(i);
            short* _dy = dy.ptr<short>(i);

            
			for (int j = 0; j < _amp.cols; j++)
				_norm[j] = std::abs(int(_dx[j])) + std::abs(int(_dy[j]));
            _norm[-1] = _norm[_amp.cols] = 0;
        }
        else
            memset(_norm-1, 0, mapstep*sizeof(int));

        // at the very beginning we do not have a complete ring
        // buffer of 3 magnitude rows for non-maxima suppression
        if (i == 0)
            continue;

        uchar* _map = map + mapstep*i + 1;
        _map[-1] = _map[_amp.cols] = 1;

        int* _mag = mag_buf[1] + 1; // take the central row
        ptrdiff_t magstep1 = mag_buf[2] - mag_buf[1];
        ptrdiff_t magstep2 = mag_buf[0] - mag_buf[1];

        const short* _x = dx.ptr<short>(i-1);
        const short* _y = dy.ptr<short>(i-1);

        if ((stack_top - stack_bottom) + _amp.cols > maxsize)
        {
            int sz = (int)(stack_top - stack_bottom);
            maxsize = maxsize * 3/2;
            stack.resize(maxsize);
            stack_bottom = &stack[0];
            stack_top = stack_bottom + sz;
        }
		 
        int prev_flag = 0;
        for (int j = 0; j < _amp.cols; j++)
        {
            #define CANNY_SHIFT 15
            const int TG22 = (int)(0.4142135623730950488016887242097*(1<<CANNY_SHIFT) + 0.5);

            int m = _mag[j];

            if (m > low)
            {
                int xs = _x[j];
                int ys = _y[j];
                int x = std::abs(xs);
                int y = std::abs(ys) << CANNY_SHIFT;
				 
                int tg22x = x * TG22;

                if (y < tg22x)
                {
                    if (m > _mag[j-1] && m >= _mag[j+1]) goto __ocv_canny_push;		// horizontal
                }
                else
                {
					int tg45x = x << CANNY_SHIFT;
					if ( y < tg45x )
					{
                        int s = (xs ^ ys) < 0 ? -1 : 1;	// 135 || 45
                        if (m > _mag[j+magstep2-s] && m > _mag[j+magstep1+s]) goto __ocv_canny_push;
					}
                }
            }
            prev_flag = 0;
            _map[j] = uchar(1);
            continue;
__ocv_canny_push:
            if (!prev_flag && m > high && _map[j-mapstep] != 2)
            {
                CANNY_PUSH(_map + j);
                prev_flag = 1;
            }
            else
                _map[j] = 0;
        }

        // scroll the ring buffer
        _mag = mag_buf[0];
        mag_buf[0] = mag_buf[1];
        mag_buf[1] = mag_buf[2];
        mag_buf[2] = _mag;
    }

    // now track the edges (hysteresis thresholding)
    while (stack_top > stack_bottom)
    {
        uchar* m;
        if ((stack_top - stack_bottom) + 8 > maxsize)
        {
            int sz = (int)(stack_top - stack_bottom);
            maxsize = maxsize * 3/2;
            stack.resize(maxsize);
            stack_bottom = &stack[0];
            stack_top = stack_bottom + sz;
        }

        CANNY_POP(m);

        if (!m[-1])         CANNY_PUSH(m - 1);
        if (!m[1])          CANNY_PUSH(m + 1);
        if (!m[-mapstep-1]) CANNY_PUSH(m - mapstep - 1);
        if (!m[-mapstep])   CANNY_PUSH(m - mapstep);
        if (!m[-mapstep+1]) CANNY_PUSH(m - mapstep + 1);
        if (!m[mapstep-1])  CANNY_PUSH(m + mapstep - 1);
        if (!m[mapstep])    CANNY_PUSH(m + mapstep);
        if (!m[mapstep+1])  CANNY_PUSH(m + mapstep + 1);
    }

	const uchar* pmap = map + mapstep + 1;
    uchar* pdst = _edge.ptr();
	short* _dx = dx.ptr<short>();

    for (int i = 0; i < _amp.rows; i++, pmap += mapstep, pdst += _edge.step, _dx += dx.cols )
    {
        for (int j = 0; j < _amp.cols; j++)
		{
			if (_dx[j] < 0)	// right edge 248
				pdst[j] = (uchar)-(pmap[j] >> 4);
			else //left edge 192
				pdst[j] = (uchar)-(pmap[j] >> 1);
		}
    }
}