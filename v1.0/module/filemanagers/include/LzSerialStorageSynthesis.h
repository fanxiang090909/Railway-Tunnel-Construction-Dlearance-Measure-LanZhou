/*
 *	LzSerialStorageSynthesis.h
 *	兰州隧道限界检测项目 综合流式存储类
 *
 *	作者: FanX
 *	版本: alpha v1.0.0
 *	日期：2014.1.10
 */

#ifndef LZSERIALSTORAGESYNTHESIS_H
#define LZSERIALSTORAGESYNTHESIS_H

#include "LzSerialStorageBase.h"

#include "headdefine.h"
#include "clearance_item.h"

class LzSerialStorageSynthesis : public LzSerialStorageBase
{
public:

    LzSerialStorageSynthesis() {this->specifykey = false; setting(250, 1024*1024*50, true);}

	~LzSerialStorageSynthesis() {}

	bool createFile(const char* filename) {
		return LzSerialStorageBase::createFile(filename, sizeof(DataHead));
	}

	void writeHead( DataHead *fh ) {
		LzSerialStorageBase::writeHead((char*)fh);
	}
	void readHead( DataHead *fh ) {
		LzSerialStorageBase::readHead((char*)fh);
	}

	void writeMap(_int64 frameCounter, double mileCount, float centerHeight, std::map<int, item> &map ) {
		size_t szcount = 0;
		// 给mapbuffer和szcount赋值
		mapToBlock(szcount, map);
		
		memcpy(blockinfo.reservebit+128, &frameCounter, sizeof(_int64));
		memcpy(blockinfo.reservebit+128+sizeof(_int64), &mileCount, sizeof(double));
   		memcpy(blockinfo.reservebit+128+sizeof(_int64)+sizeof(double), &centerHeight, sizeof(float));
		memcpy(blockinfo.reservebit+128+sizeof(_int64)+sizeof(double)+sizeof(float), &szcount, sizeof(size_t));

		/*
		丁志宇测试用
		int height;
		item tmpitem;
		char* dpt = mapbuffer;
		for (int i=0; i<szcount; i++)
		{
			memcpy(&height, dpt, sizeof(int));
			dpt += sizeof(int);
			memcpy(&tmpitem, dpt, sizeof(item));
			dpt += sizeof(item);
		}*/

		// 【注意！】szcount是数据块的大小，字节长度
		//LzSerialStorageBase::writeBlock(mapbuffer, szcount);
		LzSerialStorageBase::writeBlock(mapbuffer, szcount*(sizeof(item)+sizeof(int)));
	}
	void writeMap(_int64 frameCounter, double mileCount, float centerHeight, std::map<int, item> & map, BlockInfo &blockinfo, bool specifykey = true ) {
		memcpy(&(this->blockinfo), &blockinfo, sizeof(BlockInfo));
		this->specifykey = specifykey;
        writeMap(frameCounter, mileCount, centerHeight, map);
	}

    /**
     * 改入新值,重写函数，将centerHeight，和 mileCount 重新写入文件
     * @author 范翔
     * @date 20150626
     */
	void rewriteMapV2(_int64 frameCounter, double mileCount, float centerHeight, std::map<int, item> & map) {
		
        size_t si = 0;
		mapToBlock(si, map);

        BlockInfo toRewriteInfo;
        memcpy(toRewriteInfo.reservebit+128, &frameCounter, sizeof(_int64));
		memcpy(toRewriteInfo.reservebit+128+sizeof(_int64), &mileCount, sizeof(double));
   		memcpy(toRewriteInfo.reservebit+128+sizeof(_int64)+sizeof(double), &centerHeight, sizeof(float));
		memcpy(toRewriteInfo.reservebit+128+sizeof(_int64)+sizeof(double)+sizeof(float), &si, sizeof(size_t));

        rewriteBlock(frameCounter, toRewriteInfo, mapbuffer);
    }

	// 改入新值
	void rewriteMap(_int64 frameCounter, double mileCount, float centerHeight, std::map<int, item> & map) {
		// 给mapbuffer和szcount赋值
		size_t si = -1; // 没有用
		mapToBlock(si, map);
		rewriteBlock(frameCounter, mapbuffer);
	}

	bool retrieveMap( BLOCK_KEY key ) {
		return LzSerialStorageBase::retrieveBlock(key);
	}
	bool readMap(_int64& frameCounter, double& mileCount, float& centerHeight, SectionData & sectiondata) 
	{
		if ( !LzSerialStorageBase::readBlock(mapbuffer) )
			return false;

		size_t szcount;
		// fan: memcpy 某种意义上可理解为赋值号“=”，尤其适用大数据块的赋值
		// fan: 与strcpy()不同的是,memcpy()会完整的复制n个字节,不会因为遇到字符串结束'\0'而结束
		memcpy(&frameCounter, blockinfo.reservebit+128, sizeof(_int64));
		memcpy(&mileCount, blockinfo.reservebit+128+sizeof(_int64), sizeof(double));
   		memcpy(&centerHeight, blockinfo.reservebit+128+sizeof(_int64) + sizeof(double), sizeof(float));
		memcpy(&szcount, blockinfo.reservebit+128+sizeof(_int64) + sizeof(double) + sizeof(float), sizeof(size_t));

		blockToMap(mapbuffer, szcount, sectiondata);
        sectiondata.setCenterHeight(centerHeight);
        sectiondata.setMile(mileCount);

		return true;
	}
	bool readMap(_int64& frameCounter, double& mileCount, float& centerHeight, SectionData & sectiondata, BlockInfo &blockinfo ) 
	{
		if ( !readMap(frameCounter, mileCount, centerHeight, sectiondata) )
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
	char mapbuffer[1024*1024];
	
	size_t mapbuffersize;

	bool specifykey;

	/**
     * map转成block中的buffer
     * @see lz_synthesis程序
     */
    bool mapToBlock(size_t & szcount, std::map<int, item>& mapVal)
	{
		size_t len = sizeof(int) + sizeof(item);
		// 【注意！】szcount是数据块的大小，字节长度
		szcount = mapVal.size();

		std::cout << mapVal.size() << len * mapVal.size();

		std::map<int, item>::iterator it = mapVal.begin();
		
		char* dptr = mapbuffer;

		while (it != mapVal.end())
		{
			memcpy( dptr, &(it->first), sizeof(int) );
			memcpy( dptr+sizeof(int), &(it->second), sizeof(item) );
			dptr += len;
			it++;
		}
		return true;
	}

	/**
     * block中的buffer转map
	 * @param mapbuffer block块缓存
	 * @param sectiondata 含有map块
	 * @see SectionData类
     * @see lz_synthesis程序
     */
	bool blockToMap(char* mapbuffer, size_t & szcount, SectionData & sectiondata)
	{
		//size_t len = (sizeof(int) + sizeof(float) + sizeof(float));
		char* dptr = mapbuffer;
		size_t len = sizeof(int) + sizeof(item);

		int height;
		item tmpitem;
		float leftval;
		float rightval;
		// sectiondata清零
		sectiondata.resetMaps();
		
		for (int i = 0; i < szcount; i++)
		{
			memcpy( &height, dptr, sizeof(int));
			memcpy( &tmpitem, dptr+sizeof(int), sizeof(item) );
			
			// 插入map中，可调用SectionData类中的updateToMapVals
			sectiondata.updateToMapVals(height, tmpitem.left, tmpitem.right);

			dptr += len;
		}

		return true;
	}
};

#endif // LZSERIALSTORAGESYNTHESIS_H