/*/////////////////////////////////////////////////////////
*
*	LzCamera.h
*	兰州隧道项目 数据采集
*	
*	持有管理相机实例，设置相机，采集数据，压缩数据，存储数据
*
*	版本：	alpha v2.0
*	作者：	Ding (leeleedzy@gmail.com)
*	日期：	2014.8.2
*
*	修改采集控制方式
*	
*//////////////////////////////////////////////////////////

#ifndef LZ_CAMERA_H
#define LZ_CAMERA_H

#include <pylon/PylonIncludes.h>
#include <pylon/gige/BaslerGigeInstantCameraArray.h>

#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>

#include <filemanagers.h>
#include <lz_exception.h>

#include "CameraConfiguration.h"
#include "CameraImageEventHandler.h"

#include <QObject>

// Namespace for using pylon objects.
using namespace Pylon;

typedef struct AcquiRet{
	AcquiRet(Pylon::String_t _cam, size_t _beg, size_t _end) :
		cam(_cam), beg(_beg), end(_end) {}

	Pylon::String_t cam;
	size_t beg;
	size_t end;
}AcquiRet;

class LzCameraAcquisition  : public QObject
{
	Q_OBJECT

public:
	LzCameraAcquisition(size_t _numdevices = 4) : numdevices(_numdevices) {}
	~LzCameraAcquisition() {
		cameras.DestroyDevice();
		files.clear();
		imghandlers.clear();
	}

	// throw 100 if not enough camera present
	// throw 110 if lzo_init, the compress part broken
	void init();

	// throw 101 if can't match ID with present cameras
	bool cfgEventHandler( Pylon::String_t ID, bool _isref, int exposuretime = EXPOSURE_TIME);

	void cfgTriggerMode(AcquiMode mode);

	// open the cameras and the cameras will enter acquisition status
	// throw 104 if open cameras failed
	void openCamera();

	// throw 101 if can't match ID with present cameras
	// throw 102 if create file failed!
	bool assignFileInfo( Pylon::String_t ID,const char* filename, DataHead* head);

	// for each file, fill the EndMileage part for datahead
	// throw 103 if any file isn't opening
	void setEndMileage( unsigned int _endmile );
	void setStartMileage( unsigned int _endmile );

	// ****调用 waitCameraBufferEmpty()****
	// 等待相机缓存清空
	// 关闭文件
	void closeFile()
	{
		waitCameraBufferEmpty();
		for (int i=0; i<numdevices; i++)
			files[i]->closeFile();
	}

	// warnning!! 警告!!
	// warnning!! 警告!!
	// this function will be trapped in dead loop 
	// unless the ready buffers of all cameras being empty.
	// 此函数将会陷入死循环, 除非所有相机的就绪缓存数量为零
	void waitCameraBufferEmpty();

	
	
	void stopContinuesGrabbing() 
	{
		for (int i=0; i<numdevices; i++)
			cameras[i].StopGrabbing();

		for (int i=0; i<numdevices; i++)
			files[i]->closeFile();
	}
	void stopGrabbing()
	{		
		for (int i=0; i<numdevices; i++)
			cameras[i].StopGrabbing();

		closeFile();
	}
	bool checkBufferOverflow()
	{
		for (int i=0; i<numdevices; i++)
		{
			if ( cameras[i].NumEmptyBuffers.GetValue() < 1 )
				return false;
		}
	}


	//更改状态，传递帧号
	void discard( vector<AcquiRet> vecar)
	{
		for (int i=0; i<vecar.size(); i++)
		{
			imghandlers[ camtoindex[ vecar[i].cam ] ]->discard( vecar[i].end );
		}
	}

	void acqui()
	{
		for (int i=0; i<imghandlers.size(); i++)
			imghandlers[i]->acqui();
	}

	void exiting( vector<AcquiRet> vecar )
	{
		for (int i=0; i<vecar.size(); i++)
		{
			imghandlers[ camtoindex[ vecar[i].cam ] ]->out( vecar[i].end );
		}
	}

	//收集执行情况
	void dicardfinish(Pylon::String_t camera, size_t begframe, size_t endframe)
	{
		boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(discard_mutex);

		acquiret.push_back( AcquiRet(camera, begframe, endframe) );

		if (acquiret.size() == numdevices)
		{
			vector<AcquiRet> tmp = acquiret;
			acquiret.clear();
            if (numdevices == 4)
                emit discardfinish(tmp[0].cam.c_str(), tmp[0].beg, tmp[0].end, tmp[1].cam.c_str(), tmp[1].beg, tmp[1].end,
                                  tmp[2].cam.c_str(), tmp[2].beg, tmp[2].end, tmp[3].cam.c_str(), tmp[3].beg, tmp[3].end);		
        }
	}

	void acquifinish(Pylon::String_t camera, size_t begframe, size_t endframe)
	{
		boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(exit_mutex);

		acquiret.push_back( AcquiRet(camera, begframe, endframe) );

		if (acquiret.size() == numdevices)
		{
			vector<AcquiRet> tmp = acquiret;
			acquiret.clear();
            if (numdevices == 4)
                emit acquifinish(tmp[0].cam.c_str(), tmp[0].beg, tmp[0].end, tmp[1].cam.c_str(), tmp[1].beg, tmp[1].end,
                                 tmp[2].cam.c_str(), tmp[2].beg, tmp[2].end, tmp[3].cam.c_str(), tmp[3].beg, tmp[3].end);	
		}
	}

    void retriveSample(const char* cam_seq_num, size_t* sam_fra, Mat& sam_img)
    {
        if ( camtoindex.find(cam_seq_num) == camtoindex.end() )
            return;

        imghandlers[ camtoindex[cam_seq_num] ]->retrieveSample( sam_fra, sam_img );
    }


signals:
    void discardfinish(QString camera1, qint64 c1beg, qint64 c1end, QString camera2, qint64 c2beg, qint64 c2end,
                        QString camera3, qint64 c3beg, qint64 c3end, QString camera4, qint64 c4beg, qint64 c4end);
    void acquifinish(QString camera1, qint64 c1beg, qint64 c1end, QString camera2, qint64 c2beg, qint64 c2end,
                      QString camera3, qint64 c3beg, qint64 c3end, QString camera4, qint64 c4beg, qint64 c4end);

public:
	//one to one
	CBaslerGigEInstantCameraArray cameras;

	std::vector<ImageEventHandler*> imghandlers;

	std::vector<LzSerialStorageAcqui*> files;

	AcquiMode mode;

	vector<AcquiRet> acquiret;
	map<Pylon::String_t, int> camtoindex;

	static boost::interprocess::interprocess_mutex exit_mutex;
	static boost::interprocess::interprocess_mutex discard_mutex;

private:
	PylonAutoInitTerm autoInitTerm;
	size_t numdevices;

};

#endif