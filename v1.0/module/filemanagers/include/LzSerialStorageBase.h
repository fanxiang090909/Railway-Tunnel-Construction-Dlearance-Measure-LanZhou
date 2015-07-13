/*///////////////////////////////////////////////////
*	LzSerialStorageBase.h
*   流式存储管理
*	
*	提供包装文件读写，向外提供读写小数据块以及特殊数据块（文件头）
*
*	文件结构:
*	内容						占位						备注
*	--------------------------------------------------------------------
*	FORMAT					|	8bit					  |	用户透明
*	SerialStorageInfo		|	sizeof(SerialStorageInfo) |	用户透明
*	SpecialField(Head)		|	headsize(用户指定)		  |	用户写入
*							|							  |
*	BlockInfo				|	sizeof(BlockInfo)		  |	衍生类访问	
*	Block entity			|							  |	用户写入
*	BlockInfo				|							  |
*	Block entity			|							  |
*	....					|							  | 
*							|							  |	block添加处
*	map<BLOCK_KEY, Offset>	|							  |	提供随机访问
*	--------------------------------------------------------------------
*
*	
*	
*   基本使用方法(Useage)：
*			请查看public域声明及定义。
*
*   版本：	beta v2.1
*	作者：	Ding (leeleedzy@gmail.com)
*
*///////////////////////////////////////////////////

#ifndef LZSERIALSTORAGEBASE_H
#define LZSERIALSTORAGEBASE_H

#include <QDebug>
#include <fstream>
#include <map>
#include <string>
#include <assert.h>

#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

using namespace std;

typedef size_t BLOCK_KEY;
typedef fstream::streampos Offset;

#define BUFFER 1024*1024*1100;
#define FRAMES 250;
#define USEMUTEX true;

//流式文件信息结构体（用户透明）
struct SerialStorageInfo
{
	// headsize 应该由用户在创建流式存储文件时进行设置,并且不能修改
	size_t headsize;

	Offset firstblockpos;
	size_t blocknumbers;
	// 
	Offset writepos;
	// 
	Offset mappos;	
	size_t mapsize;
};

//流式文件子块信息结构体（衍生类可以定义访问）
struct BlockInfo
{
	BLOCK_KEY key;
	size_t size;	//block大小
	size_t seqindex;
	bool isvalid;
	char reservebit[256];
};

class LzSerialStorageBase //Manipulator
{
public:
	LzSerialStorageBase();
	~LzSerialStorageBase() { closeFile(); }

	//关闭，打开，创建文件，创建文件第二个参数是被用来指定特殊数据域的大小
	//特殊数据域大小在流式文件被创建时设置，不能修改
	//所有流式文件必须由createFile函数创建
	//三个函数返回值用于指示操作是否成功
	bool createFile(const char* filename, size_t headsize);
	bool openFile(const char* filename);
	void closeFile();

	bool isOpen() { return isholdfile; }
	size_t containBlock() { return info.blocknumbers; }

	//主动将缓存数据写入硬盘
	//可以设置flushmutex,决定是否独占硬盘
	void flush();

	//读写特殊数据域
	//特殊数据域大小已经被指定，所以参数中不需要指定大小
	//读写操作不负责内存的申请与释放，参数的指针指向的内存
	//空间应该由用户管理或者在衍生类中进行管理。
	void writeHead(char* dpt);
	void readHead(char* dpt);

	//写入小数据块(block)，将调用generateKey为此数据块生成键，存入blockinfo中，继而写入blockinfo以及block
	void writeBlock(char* block,  size_t size);

    /**
     * Rewrite使用必须保证写入的block与原block块大小完全相同
     * @author 丁志宇
     */
	bool rewriteBlock(BLOCK_KEY key, char* block) 
	{
		// find key block;
		if ( (bmap.find(key)) == (bmap.end()) )
			return false;

		// move file pointer;
		fs.seekg(bmap[key], ios::beg);
		BlockInfo info;
		fs.read((char*)(&info), blockinfosize);

		continueread = false;
		continuewrite = false;

		// write new block with same size as before;
		fs.seekp(fs.tellg(), ios::beg);
		fs.write(block, info.size);

		return true;
	}

    /**
     * Rewrite使用必须保证写入的block与原block块大小完全相同
     * 为了LzSerialStorageSynshsis中的rewriteMap写
     * 
     * 新的blockinfo中要rewrite重写的内容写到预留位
     * @param infonokey 新的blockinfo中要rewrite重写的内容写到预留位
     * @author 范翔
     * @date 20150626 
     */
    bool rewriteBlock(BLOCK_KEY key, BlockInfo infonokey, char* block) 
	{
		// find key block;
		if ( (bmap.find(key)) == (bmap.end()) )
			return false;

		// move file pointer;
		fs.seekg(bmap[key], ios::beg);
		BlockInfo inforead;
		fs.read((char*)(&inforead), blockinfosize);
        
        qDebug() << inforead.key << inforead.isvalid << inforead.size << inforead.seqindex;

		fs.seekg(bmap[key], ios::beg);
   		fs.seekp(fs.tellg(), ios::beg);
        infonokey.key = inforead.key;
        infonokey.isvalid = inforead.isvalid;
        infonokey.seqindex = inforead.seqindex;
        infonokey.size = inforead.size;

        fs.write((char *)&infonokey, sizeof(BlockInfo));

		continueread = false;
		continuewrite = false;

		// write new block with same size as before;
		fs.seekp(fs.tellg(), ios::beg);
		fs.write(block, infonokey.size);

		return true;
	}

	//将文件指针指向key对应的block偏移量处
	bool retrieveBlock(BLOCK_KEY key);
	//读取block,每次读取block,文件指针都将往后移动到下一个block处
	//即可以循环调用readBlock()读取所有block
	bool readBlock(char* block);

	//返回文件当中所有block的键值 map<BLLOCK_KEY, Offset>
	vector<BLOCK_KEY> readKeys();

	//Key应该按加入顺序升序排列
	virtual BLOCK_KEY generateKey(char* block, size_t size) 
	{ 
		return info.blocknumbers; 
	}

	//设置刷新缓存帧数，缓存大小，是否使用写硬盘互斥
	void setting( unsigned int bufframes, size_t bufsize, bool mutex )
	{
		bufferframes = bufframes;
		buffersize = bufsize;
		flushmutex = mutex;
	}

	//返回文件的一些基本信息
	/*
	filesize();	
	isHoldFile();
	buffersize();
	numberofblocks();
	*/

protected:
	//衍生类中可以重新定义文件格式标识符
	virtual bool checkFormat() {
		char ff[100];
		ff[formatsize] = '\0';
		fs.seekg(0,ios_base::beg);
		fs.read(ff,formatsize);
		return strcmp(ff, format)==0;
	}
	virtual void writeFormat() { 
		fs.seekp(0,ios_base::beg);
		fs.write(format, formatsize);
	}
	char* format;	//文件格式标识符，长度为8

	BlockInfo blockinfo;	//blockinfo 存储和提取自块的中间变量

	virtual void init();

	map<BLOCK_KEY, Offset> bmap;
	SerialStorageInfo info;

private:
	std::fstream fs;
	bool isholdfile;

	///
	size_t writecounts;
	bool continuewrite;
	size_t readseqindex;
	bool continueread;
	bool inlastblock;

	///@todo setting
	unsigned int bufferframes;
	size_t buffersize;
	char* fbuffer;
	bool flushmutex;
	static boost::interprocess::interprocess_mutex fmutex;

	//特殊块偏移量
	const static size_t formatsize;
	const static size_t infosize;
	const static size_t headstartoffset;
	const static size_t blockinfosize;

	//创建文件预留文件头位置	
	void reserveHead() {
		fs.seekp(headstartoffset, ios::beg);
		char *tmp = new char[info.headsize];
		fs.write(tmp, info.headsize);
		delete[] tmp;
	}

	//缓存
	void allocateBuffer() 
	{
		if (!isholdfile)
			return;

		fbuffer = new char[buffersize];
		filebuf* fb = fs.rdbuf();
		fb->pubsetbuf(fbuffer, buffersize);
	}
	void deleteBuffer() {
		if (fbuffer != NULL)
			delete[] fbuffer;
	}

	//写入,读取文件信息
	void initInfo(size_t headsize) {
		info.blocknumbers = 0;
		info.firstblockpos = formatsize + infosize + headsize;
		info.headsize = headsize;
		info.mappos = info.firstblockpos;
		info.mapsize = 0;
		info.writepos = info.firstblockpos;
	}
	void loadInfo() {
		fs.seekg(formatsize, ios::beg);
		fs.read((char*)(&info), infosize);
	}
	void writeInfo() {
		fs.seekp(formatsize, ios::beg);
		fs.write((char*)(&info), infosize);
	}

	//random retrieve map operete
	void loadBlockMap();	
	void saveBlockMap();	
};

#endif