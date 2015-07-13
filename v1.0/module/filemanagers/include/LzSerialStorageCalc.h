﻿/*
 *	LzSerialStorageCalc.h
 *	兰州隧道限界检测项目 对极几何计算结果流式存储
 *
 *	实现 Class LzSerialStorageCalc 继承 LzSerialStorageMat
 *
 *	存取块数据类型: cv::Mat
 *
 *	作者: Ding(leeleedzy@gmail.com)
 *	日期: 2014.1.14
 */

#ifndef LZ_SERIALSTORAGECALC_H
#define LZ_SERIALSTORAGECALC_H

#include <opencv2\core\core.hpp>
#include "LzSerialStorageMat.h"

#include <datastructure.h>

class LzSerialStorageCalc : public LzSerialStorageMat
{
public:

	bool createFile(const char* filename) {
		return LzSerialStorageBase::createFile(filename, sizeof(DataHead));
	}

	void setAcquiHead( DataHead* _acquihead ) 
	{
		memcpy(&head, _acquihead, sizeof(DataHead) );
	}
	void fillAcquiHead( )
	{ }

	void writeHead( DataHead *fh ) {
		LzSerialStorageBase::writeHead((char*)fh);
	}
	void readHead( DataHead *fh ) {
		LzSerialStorageBase::readHead((char*)fh);
	}

	//vector<BLOCK_KEY> readKeys()

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

private:
	DataHead head;
};

#endif