#include "..\include\LzSerialStorageBase.h"

const size_t LzSerialStorageBase::formatsize = 8;
const size_t LzSerialStorageBase::infosize = sizeof(SerialStorageInfo);
const size_t LzSerialStorageBase::headstartoffset = LzSerialStorageBase::formatsize + LzSerialStorageBase::infosize;
const size_t LzSerialStorageBase::blockinfosize = sizeof(BlockInfo);

boost::interprocess::interprocess_mutex LzSerialStorageBase::fmutex;

void LzSerialStorageBase::init()
{
	writecounts = 0;
	continuewrite = false;
	readseqindex = 0;
	continueread = false;
	inlastblock = false;
}

LzSerialStorageBase::LzSerialStorageBase() :
	format("LZSERIAL")
{
	bmap.clear();
	isholdfile = false;

	init();

	bufferframes = FRAMES;
	buffersize = BUFFER;
	flushmutex = USEMUTEX;
}

bool LzSerialStorageBase::createFile(const char* filename, size_t headsize)
{
	if (isholdfile)
		closeFile();

	fs.open(filename, std::fstream::out | std::fstream::trunc | std::fstream::binary);
	if ( ! fs.is_open() )
		return false;

	//write format into new file
	initInfo( headsize );
	writeFormat();
	writeInfo();
	reserveHead();
	fs.close();

	//open file
	openFile(filename);

	return true;
}

bool LzSerialStorageBase::openFile(const char* filename)
{
	if (isholdfile)
		closeFile();

	fs.open(filename, std::fstream::in | std::fstream::out | std::fstream::binary);

	if (!fs.is_open())
		return false;

	if (!checkFormat())
		return false;

	init();
	isholdfile = true;
	loadInfo();
	loadBlockMap();
	allocateBuffer();

	return true;
}

void LzSerialStorageBase::closeFile()
{
	if (!isholdfile)
		return;

	saveBlockMap();
	writeInfo();
	flush();
	fs.close();
	deleteBuffer();

	isholdfile = false;
}

void LzSerialStorageBase::writeHead( char* dpt )
{
	assert(dpt);

	flush();
	fs.seekp(headstartoffset, ios_base::beg);

	fs.write(dpt, info.headsize);
}

void LzSerialStorageBase::readHead(char* dpt)
{
	assert(dpt);

	flush();
	fs.seekg(headstartoffset, ios_base::beg);

	fs.read(dpt, info.headsize);
}

void LzSerialStorageBase::writeBlock(char* block, size_t size)
{
	if (!continuewrite)
	{
		flush();
		fs.seekp(info.writepos);

		continuewrite = true;
	}

	blockinfo.size = size;
	blockinfo.key = generateKey(block, size);
	blockinfo.seqindex = info.blocknumbers++;

	bmap.insert(pair<BLOCK_KEY,Offset>(blockinfo.key, fs.tellp()));

	fs.write((char*)(&blockinfo), blockinfosize);
	fs.write(block, size);

	writecounts++;
	if (writecounts >= bufferframes)
	{
		flush();

		continuewrite = true;
		writecounts = 0;
	}

	info.writepos = fs.tellp();
}

bool LzSerialStorageBase::retrieveBlock(BLOCK_KEY key)
{
	if ( (bmap.find(key)) == (bmap.end()) )
		return false;

	flush();
	fs.seekg(bmap[key], ios::beg);

	continueread = true;
	return true;
}

bool LzSerialStorageBase::readBlock(char* block)
{
	if (inlastblock || !continueread)
		return false;

	fs.read((char*)(&blockinfo), blockinfosize);
	fs.read(block, blockinfo.size);

	if ( blockinfo.seqindex == (info.blocknumbers-1) )
		inlastblock = true;

	return true;
}

void LzSerialStorageBase::flush()
{
	init();

	if (!flushmutex)
		fs.flush();
	else
	{
		//写硬盘互斥锁
		boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(fmutex);
		fs.flush();
	}
}

vector<BLOCK_KEY> LzSerialStorageBase::readKeys()
{
	vector<BLOCK_KEY> vb;
	vb.clear();

	if (!isholdfile)
		return vb;

  	map<BLOCK_KEY,Offset>::iterator miter = bmap.begin();
    while (miter != bmap.end())
	{
        vb.push_back(miter->first);
        miter++;
    }

	return vb;
}

void LzSerialStorageBase::loadBlockMap()
{
	fs.seekg( info.mappos, ios_base::beg );

	BLOCK_KEY ky;
	Offset	  off;

	for (int i=0; i<info.mapsize; i++)
	{
		fs.read((char*)(&ky),sizeof(BLOCK_KEY));
		fs.read((char*)(&off),sizeof(Offset));
		bmap.insert( pair<BLOCK_KEY,Offset>(ky,off) );
	}
}

void LzSerialStorageBase::saveBlockMap()
{

	fs.seekp(info.writepos, ios_base::beg);
	info.mappos = info.writepos;
	info.mapsize = bmap.size();

	map<BLOCK_KEY,Offset>::iterator miter = bmap.begin();
	while (miter != bmap.end())
	{
		fs.write( (char*)(&miter->first),sizeof(BLOCK_KEY)) ;
		fs.write((char*)(&miter->second),sizeof(Offset)) ;
        miter++;
	}
}


