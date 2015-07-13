/*
 *	LzSerialStorageRT.h
 *	兰州隧道限界检测项目  计算车身晃动流式存储类
 *
 *	作者: Ding(leeleedzy@gmail.com)
 *  版本: alpha v1.0.0
 *	日期：2014.1.14
 */

#ifndef LZ_SERIALSTORAGERT_H
#define LZ_SERIALSTORAGERT_H

#include <opencv2\core\core.hpp>
#include "LzSerialStorageMat.h"

#include <datastructure.h>

class LzSerialStorageRT : public LzSerialStorageMat
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