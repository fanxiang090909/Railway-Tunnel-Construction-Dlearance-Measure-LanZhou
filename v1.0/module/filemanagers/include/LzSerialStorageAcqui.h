/*
*	LzSerialStorageAcqui.h
*	兰州隧道限界检测项目  采集流式存储类
*
*	作者: Ding(leeleedzy@gmail.com)
*   版本: alpha v1.1
*	日期：2014.1.18
*/


#ifndef LZSERIALSTORAGEACQUI_H
#define LZSERIALSTORAGEACQUI_H

#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <sstream>

#include "LzSerialStorageBase.h"

//for decompression
#include <compress.h>
#include <datastructure.h>
#include <lz_exception.h>

#define DECOMPRESS_BUF_SIZE (1024*1280)

#define FRAME_COLS (1280)
#define FRAME_ROWS (512)
#define FRAME_SIZE (FRAME_COLS*FRAME_ROWS)
#define FRAME_TYPE (CV_8UC1)

class LzSerialStorageAcqui : public LzSerialStorageBase
{
public:
	LzSerialStorageAcqui( size_t mtbufsz = 1024*1024*10 ) : 
		specifykey(false) {
			matbuffer = new char[mtbufsz];
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
	~LzSerialStorageAcqui() {
		delete[] matbuffer;
	}

	bool openFile(const char* filename)
	{
		bool ret = LzSerialStorageBase::openFile(filename);
		readHead( &acquihead );
        return ret;
	}
	void closeFile() {
		writeHead( &acquihead );
		LzSerialStorageBase::closeFile();
	}

	void addFrame( char* dpt, size_t size, BlockInfo *blockinfo, bool specifykey = true ) 
	{
		memcpy(&(this->blockinfo), blockinfo, sizeof(BlockInfo));
		this->specifykey = specifykey;
		LzSerialStorageBase::writeBlock(dpt, size);
	}
    
    //modify 2015.1.20
    //**********************************temp************************************************************
    /*void writeMat( cv::Mat &m ) {
		int dtype = m.type();
		int ddims = m.dims;
		int drows = m.rows;
		int dcols = m.cols;
		size_t dstep = m.step1();
		size_t szcount = m.rows * m.cols * m.elemSize();

		//////////////
		if( m.rows > 0 && m.cols > 0 )
		{
			const uchar* sptr = m.data;
			uchar* dptr = (uchar*)matbuffer;

			// to handle the copying 1xn matrix => nx1 std vector.
			cv::Size sz = m.isContinuous() ? cv::Size(m.cols*m.rows, 1) : cv::Size(m.cols, m.rows);
			size_t len = sz.width*m.elemSize();
			for( ; sz.height--; sptr += dstep, dptr += dstep )
				memcpy( dptr, sptr, len );
		}
		
		memcpy(blockinfo.reservebit+128, &dtype, sizeof(int));
		memcpy(blockinfo.reservebit+128+sizeof(int), &ddims, sizeof(int));
		memcpy(blockinfo.reservebit+128+2*sizeof(int), &drows, sizeof(int));
		memcpy(blockinfo.reservebit+128+3*sizeof(int), &dcols, sizeof(int));
		memcpy(blockinfo.reservebit+128+4*sizeof(int), &szcount, sizeof(size_t));
		///////////////

		LzSerialStorageBase::writeBlock(matbuffer, szcount);

	}
	void writeMat( cv::Mat &m, BlockInfo &blockinfo, bool specifykey = true ) {
		memcpy(&(this->blockinfo), &blockinfo, sizeof(BlockInfo));
		this->specifykey = specifykey;
		writeMat(m);
	}
    bool readMat( cv::Mat &m ) 
	{
		if ( !LzSerialStorageBase::readBlock(matbuffer) )
			return false;

		int dtype;
		int ddims;
		int drows;
		int dcols;
		size_t szcount;

		memcpy( &dtype, blockinfo.reservebit+128, sizeof(int));
		memcpy( &ddims, blockinfo.reservebit+128+sizeof(int), sizeof(int));
		memcpy( &drows, blockinfo.reservebit+128+2*sizeof(int), sizeof(int));
		memcpy( &dcols, blockinfo.reservebit+128+3*sizeof(int), sizeof(int));
		memcpy( &szcount, blockinfo.reservebit+128+4*sizeof(int), sizeof(size_t));

		m.create(drows,dcols,dtype);
		memcpy(m.data, matbuffer, szcount);

		return true;
	}
	bool readMat( cv::Mat &m, BlockInfo &blockinfo ) 
	{
		if ( !readMat(m) )
			return false;
		memcpy(&blockinfo, &(this->blockinfo), sizeof(BlockInfo));
		return true;
	}
    //**********************************temp************************************************************/
    //
	bool retrieveFrame(BLOCK_KEY key) {
		//retrieveBlock
		return LzSerialStorageBase::retrieveBlock(key);
	}

	bool readFrame( cv::Mat &m ) {
		// readBlock
		if ( !LzSerialStorageBase::readBlock(decompressbuf) )
			return false;

		decompress_status = lzo1x_decompress( 
			(unsigned char*)decompressbuf,
			blockinfo.size, 
			(unsigned char*)matbuffer,
			&decompress_len, 
			NULL);

		if ( decompress_status != LZO_E_OK || 
			decompress_len != FRAME_SIZE )
			throw LzException(111, "readFrame Decompress failed, broken block data!");

		memcpy(readtemp.data, matbuffer, decompress_len);

		//旋转前检测m类型
		if ( !( m.rows==FRAME_COLS && 
			m.cols==FRAME_ROWS &&
			m.type()==FRAME_TYPE &&
			m.isContinuous() ) )
		{
			m.create(FRAME_COLS, FRAME_ROWS, FRAME_TYPE);
		}

		//旋转
		cv::remap(readtemp, m, rotax, rotay, CV_INTER_LINEAR);

		return true;
	}
	bool readFrameSaveAsImage(BLOCK_KEY start, BLOCK_KEY end, const char* filepath, bool checkinouttunnel, bool ispng)	{

		if ( !retrieveFrame(start) )
			return false;

		cv::Mat m;

		do
		{
			if ( !readFrame(m) )
				return true;
			if( blockinfo.key > end)
				break;

			sstr.str("");
			sstr<<filepath;
			sstr<<"\\";
			sstr<<blockinfo.key;
            if (ispng)
    			sstr<<".png";
            else
                sstr<<".jpg";

			// 如果不是进出洞检测，作直方图均衡化
			if (!checkinouttunnel)
			{
				cv::cvtColor(m, m, CV_BGR2GRAY);
				cv::Mat result;
				// 直方图均衡化
				cv::equalizeHist(m, result);
				cv::imwrite(sstr.str().c_str(), result);
			}
			else
			{
				cv::imwrite(sstr.str().c_str(), m);
			}

		}
		while( blockinfo.key <= end);

		//while(true);
		/*while( blockinfo.key != end )
		{
			sstr.str("");
			sstr<<filepath;
			sstr<<"\\";
			sstr<<blockinfo.key;
			sstr<<".jpg";
			if ( !readFrame(m) )
				return false;

			cv::imwrite(sstr.str().c_str(), m);
		}*/

		return true;
	}

	void setAcquiHead( DataHead* _acquihead ) 
	{
		memcpy(&acquihead, _acquihead, sizeof(DataHead) );
	}
	void setEndMileage( unsigned int _endmile )
	{ 
		acquihead.end_mileage = _endmile;
	}
	void setStartMileage( unsigned int _startmile )
	{
		acquihead.start_mileage = _startmile;
	}

protected:
	virtual BLOCK_KEY generateKey(char* block, size_t size)
	{
		if (specifykey)
		{
			specifykey = false;
			return blockinfo.key;
		}
		else
			return info.blocknumbers;
	}

public:
	void readHead( DataHead* head ){
		LzSerialStorageBase::readHead( (char*)head );
	}

	void writeHead( DataHead* head ) {
		LzSerialStorageBase::writeHead( (char*)head );
	}

    // @author 范翔 20150107 流式文件拷贝时用
    BlockInfo getCurrentBlockInfo()
    {
        return blockinfo;
    }

private:
	DataHead acquihead;

	cv::Mat readtemp;
	cv::Mat rotax;
	cv::Mat rotay;

	bool specifykey;
	char* matbuffer;
	char decompressbuf[DECOMPRESS_BUF_SIZE];
	size_t decompress_len;
	int decompress_status;
	std::stringstream sstr;
};



#endif