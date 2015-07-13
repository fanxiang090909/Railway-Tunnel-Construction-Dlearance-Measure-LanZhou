/*
 *	LzSerialStoragePulse.h
 *	兰州隧道限界检测项目 脉冲流式存储类
 *
 *	作者: FanX
 *	版本: alpha v1.0.0
 *	日期：2014.4.6
 */

#ifndef LZSERIALSTORAGESYNTHESIS_H
#define LZSERIALSTORAGESYNTHESIS_H

#include "LzSerialStorageBase.h"

#include "headdefine.h"

class LzSerialStoragePulse : public LzSerialStorageBase
{
public:

	LzSerialStoragePulse() {this->specifykey = false;}

	~LzSerialStoragePulse() {}

	bool createFile(const char* filename) {
		return LzSerialStorageBase::createFile(filename, sizeof(DataHead));
	}

	void writeHead( DataHead *fh ) {
		LzSerialStorageBase::writeHead((char*)fh);
	}
	void readHead( DataHead *fh ) {
		LzSerialStorageBase::readHead((char*)fh);
	}

	void writePulse(int pulse, long frameCounter) {
		size_t szcount = 0;
		// 给mapbuffer和szcount赋值
		
		memcpy(blockinfo.reservebit+128, &pulse, sizeof(int));
		memcpy(blockinfo.reservebit+128+sizeof(long), &frameCounter, sizeof(long));

		LzSerialStorageBase::writeBlock(mapbuffer, 0);
	}
	void writePulse(long mileCount, long frameCounter, BlockInfo &blockinfo, bool specifykey = true ) {
		memcpy(&(this->blockinfo), &blockinfo, sizeof(BlockInfo));
		this->specifykey = specifykey;
		writePulse(mileCount, frameCounter);
	}

	bool retrieveMap( BLOCK_KEY key ) {
		return LzSerialStorageBase::retrieveBlock(key);
	}
	bool readPulse(int& pulse, long& frameCounter) 
	{
		if ( !LzSerialStorageBase::readBlock(mapbuffer) )
			return false;

		size_t szcount;
		// fan: memcpy 某种意义上可理解为赋值号“=”，尤其适用大数据块的赋值
		// fan: 与strcpy()不同的是,memcpy()会完整的复制n个字节,不会因为遇到字符串结束'\0'而结束
		memcpy(&pulse, blockinfo.reservebit+128, sizeof(int));
		memcpy(&frameCounter, blockinfo.reservebit+128+sizeof(long), sizeof(long));
		return true;
	}
	bool readPulse(int& pulse, long& frameCounter, BlockInfo &blockinfo ) 
	{
		if ( !readPulse(pulse, frameCounter) )
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

private:
	//char* mapbuffer;
	char mapbuffer[1024];
	
	size_t mapbuffersize;

	bool specifykey;

};

#endif // LZSERIALSTORAGESYNTHESIS_H