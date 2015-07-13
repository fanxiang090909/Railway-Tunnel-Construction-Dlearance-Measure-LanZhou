/*/////////////////////////////////////////////////////////
*
*	CameraConfiguration.h
*	兰州隧道项目 相机配置头文件
*	
*	版本：	alpha v1.1
*	作者：	Ding (leeleedzy@gmail.com)
*	日期：	2014.1.18
*
*//////////////////////////////////////////////////////////


#ifndef LZCAMERACONFIGURATION_H
#define LZCAMERACONFIGURATION_H

#include <pylon/gige/BaslerGigEInstantCamera.h>
#include <pylon/ConfigurationEventHandler.h>

#include <lz_exception.h>
#include <iostream>

#define ROI_WIDTH (1280)
#define ROI_HEIGHT (512)
#define EXPOSURE_TIME (500)
#define CAMERA_BUFFER (100)

namespace Pylon
{
	class CInstantCamera;


	using namespace Basler_GigECameraParams;


	class CameraRegularConfiguration : public Pylon::CBaslerGigEConfigurationEventHandler
	{
	public:    
		void OnOpened( Pylon::CBaslerGigEInstantCamera& camera);
		CameraRegularConfiguration(bool is_ref_camera, unsigned int _exposure_time = EXPOSURE_TIME)
			: ref_camera(is_ref_camera), exposure_time(_exposure_time) {}

	private: 
		bool ref_camera;
		unsigned int exposure_time;
	};

	class CHardwareTriggerConfiguration : public Pylon::CBaslerGigEConfigurationEventHandler
	{
	public:
		void OnOpened( Pylon::CBaslerGigEInstantCamera& camera);
	};

	class CLzSoftwareTriggerConfiguration : public Pylon::CBaslerGigEConfigurationEventHandler
	{
	public:
		void OnOpened( Pylon::CBaslerGigEInstantCamera& camera);
	};

}
#endif /* LZCAMERACONFIGURATION_H */