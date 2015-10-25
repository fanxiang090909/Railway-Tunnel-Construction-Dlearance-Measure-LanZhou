#include "..\include\CameraImageEventHandler.h"
#include <acquisition.h>

namespace Pylon{

	void ImageEventHandler::OnImageGrabbed( CBaslerGigEInstantCamera& camera, const CBaslerGigEGrabResultPtr& ptrGrabResult )
	{
         
		boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(mutex);

		if (ptrGrabResult.IsValid())
		{
			if (ptrGrabResult->GrabSucceeded())
			{

				if (mode == AcquiMode::HardwareTrigger)
				{
					currentframe = ptrGrabResult->ChunkTriggerinputcounter.GetValue();
				}
				else
				{
					currentframe = ptrGrabResult->ChunkFramecounter.GetValue();
				}

				//当未进入预进洞无效状态或采集状态时，休眠
				while ( !discarding && !acquisiting )
				{
					boost::this_thread::sleep_for( boost::chrono::milliseconds(10) );
				}


				//当进入预进洞无效状态
				if ( discarding )
				{
					//记录第一帧帧号
					if ( begining )
					{
						beginframe = currentframe;
						begining = false;
					}
				}

				//当进入采集状态
				if ( acquisiting )
				{

					//记录第一帧帧号
					if ( begining )
					{
						beginframe = currentframe;
						begining = false;
					}

					// 采集图像并写入文件

					// vaild data checking
					if (ptrGrabResult->HasCRC() && ptrGrabResult->CheckCRC() == false)
					{
						throw RUNTIME_EXCEPTION( "Image was damaged!");
					}

					// write into blockinfo
					blockinfo.key = currentframe;
					blockinfo.isvalid  = true;

					///////////////add at 11/22 2014
					if ( currentframe % SAMPLE_RATE == 0)
					{
						boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(mutex_sample);

						if (ptrGrabResult->GetImageSize() == FRAME_SIZE)
						{
						    memcpy(readtemp.data, (unsigned char*)ptrGrabResult->GetBuffer(), ptrGrabResult->GetImageSize());
						    remap(readtemp, sampleImg, rotax, rotay, CV_INTER_LINEAR);
						}
                        sample_frame = currentframe;
					}
					///////////////add at 11/22 2014
					
					//compress
					lzo1x_1_compress( (unsigned char*)ptrGrabResult->GetBuffer(),
						ptrGrabResult->GetImageSize(), 
						(unsigned char*)compressbuf, 
						&(blockinfo.size), 
						wrkmem );

					//add block into serial storage file
					file->addFrame((char*)compressbuf, blockinfo.size, &blockinfo);
				}

			}
		}
		//////////////////////////////////////////////////
		//当进入预进洞无效状态
		if ( discarding )
		{
			//检测是否到对应的预进洞最后一帧 或 缓存是否为空
			if ( camera.NumReadyBuffers.GetValue() == 0 || currentframe >= discardto )
			{
				//若是，则预进洞无效结束，做相应处理
				discardfinish();
				cameraholder->dicardfinish(camid, beginframe, currentframe);
			}
		}
		// 进入出洞状态
		else if (exiting)
		{
			// 检测是否缓存已空或已经采购足够帧数
			if ( camera.NumReadyBuffers.GetValue() == 0 || currentframe >= acquito )
			{
				acquifinish();
				cameraholder->acquifinish(camid, beginframe, currentframe);
			}
		}

	}

};