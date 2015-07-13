/*
 *	LzSerialStorageMat.h
 *	兰州隧道限界检测项目 流式存储 cv::Mat
 *
 *	实现 Class LzSerialStorageMat 继承 LzSerialStorageBase
 *
 *	存取块数据类型: cv::Mat
 *
 *	作者: Ding(leeleedzy@gmail.com)
 *	日期: 2014.1.14
 */

#ifndef LZ_SERIALSTORAGEMAT_H
#define LZ_SERIALSTORAGEMAT_H

#include <opencv2\core\core.hpp>
#include "LzSerialStorageBase.h"

#define MAT_SIZE (1024*1024*10)

class LzSerialStorageMat : public LzSerialStorageBase
{
public:
	LzSerialStorageMat( size_t mtbufsz=MAT_SIZE ) : specifykey(false) {
        setting(250, 1024*1024*50, true);
		matbuffer = new char[mtbufsz];
	}
	~LzSerialStorageMat() {
		delete[] matbuffer;
	}

	void writeMat( cv::Mat &m ) {
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

	bool retrieveMat( BLOCK_KEY key ) {
		return LzSerialStorageBase::retrieveBlock(key);
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

	//vector<BLOCK_KEY> readKeys()
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

    // @author 范翔 20150122 流式文件拷贝时用
    BlockInfo getCurrentBlockInfo()
    {
        return blockinfo;
    }

protected:
	bool specifykey;

private:
	char* matbuffer;
};



#endif