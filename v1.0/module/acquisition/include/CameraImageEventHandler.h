/*/////////////////////////////////////////////////////////
*
*	CameraImageEventHandler.h
*	兰州隧道项目 相机采集事件处理文件
*
*	版本：	alpha v1.1
*	作者：	Ding (leeleedzy@gmail.com)
*	日期：	2014.1.18
*
*//////////////////////////////////////////////////////////

#ifndef LZCAMERAIMAGEEVENTHANDLER_H
#define LZCAMERAIMAGEEVENTHANDLER_H

#include <opencv2\opencv.hpp>

using namespace cv;

#include <pylon/PylonIncludes.h>
#include <pylon/PylonGUI.h>
#include <pylon/ImageEventHandler.h>
#include <pylon/GrabResultPtr.h>

#include <iostream>

#include <filemanagers.h>
#include <compress.h>

class LzCameraAcquisition;

#include <pylon/gige/BaslerGigEInstantCamera.h>

#define HEAP_ALLOC(var,size) \
	lzo_align_t __LZO_MMODEL var [ ((size) + (sizeof(lzo_align_t) - 1)) / sizeof(lzo_align_t) ]

#define COMPRESS_BUF_SIZE (1024*1280)

#define SAMPLE_RATE 30 //update sample every 30 frames

enum AcquiMode { SoftwareTrigger, HardwareTrigger, Continues };

namespace Pylon
{
	/*static HEAP_ALLOC(wrkmem, LZO1X_1_MEM_COMPRESS);*/

	//using namespace Basler_GigECameraParams;

	class CInstantCamera;

	class ImageEventHandler : public CBaslerGigEImageEventHandler
	{
	public:

		virtual void OnImageGrabbed( CBaslerGigEInstantCamera& camera, const CBaslerGigEGrabResultPtr& ptrGrabResult);

		void retrieveSample(size_t* sam_frame, Mat& sam_img)
		{
			boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(mutex_sample);
			*sam_frame = sample_frame;
			sam_img = sampleImg;
		}

		ImageEventHandler( LzSerialStorageAcqui* _file ) 
		{ 
			attachFile(_file);
			ImageEventHandler();
		}
		ImageEventHandler( ) { 
			discarding = false;
			acquisiting = false;
			exiting = false;
			begining = true;

			discardto = 0;
			acquito = 0;

			sample_frame = 0;
			sampleImg.create(FRAME_ROWS, FRAME_COLS, FRAME_TYPE);
			// copy from LZzSerialStorageAcqui.h
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
			// copy from LZzSerialStorageAcqui.h

			beginframe = 0;
			currentframe = 0;

		}

		bool attachFile( LzSerialStorageAcqui* _file ) {
			file = _file;
			return true;
		}
		bool tieHoudlerAndCam( LzCameraAcquisition* _holder, CBaslerGigEInstantCamera *_cam )
		{
			cameraholder = _holder;
			camera = _cam;
			camid = _cam->GetDeviceInfo().GetSerialNumber();
			return true;
		}

		// 预进洞无效 采集 出洞
		void discard(size_t lastframe){
			begining = true;
			discardto = lastframe;
			discarding = true;
			OnImageGrabbed(*camera, fakeptr);
		}
		void acqui() {
			begining = true;
			acquisiting = true;
		}
		void out(size_t lastframe) {
			acquito = lastframe;
			exiting = true;
			OnImageGrabbed(*camera, fakeptr);
		}

		// 预进洞无效结束 和 采集结束（出洞）
		void discardfinish()
		{
			discarding = false;
			begining = true;

		}
		void acquifinish() 
		{
			exiting = false;
			acquisiting = false;
			begining = true;
		}

		void setAcquiMode(AcquiMode _mode)
		{
			mode = _mode;
		}

	private:
		//file
		BlockInfo blockinfo;
		LzSerialStorageAcqui* file; 

		// cameraholder
		LzCameraAcquisition *cameraholder;
		// 绑定的相机id
		Pylon::String_t camid;

		// 
		boost::interprocess::interprocess_mutex mutex;
		boost::interprocess::interprocess_mutex mutex_sample;
		CBaslerGigEGrabResultPtr fakeptr;
		CBaslerGigEInstantCamera *camera;


		Mat sampleImg;
		size_t sample_frame;
		Mat readtemp;
		Mat rotax, rotay;

		// 三种状态
		bool discarding;
		bool acquisiting;
		bool exiting;

		bool begining;

		// 计数器
		size_t discardto;
		size_t acquito;

		size_t beginframe;
		size_t currentframe;

		AcquiMode mode;

		//compress buffer
		unsigned char compressbuf[COMPRESS_BUF_SIZE];
		HEAP_ALLOC(wrkmem, LZO1X_1_MEM_COMPRESS);
	};
}

#endif /* LZCAMERAIMAGEEVENTHANDLER_H */
