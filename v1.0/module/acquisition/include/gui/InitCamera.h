#ifndef __INIT_CAMERA_H
#define __INIT_CAMERA_H


#include <pylon/PylonIncludes.h>
#include <pylon/PylonGUI.h>
#include <pylon/GrabResultPtr.h>

#include <pylon/gige/BaslerGigEInstantCamera.h>
#include <pylon\gige\BaslerGigEInstantCameraArray.h>
#include <pylon/ConfigurationEventHandler.h>


#define ROI_WIDTH (1280)
#define ROI_HEIGHT (512)

/**
 * 兰州双目标定采集程序-相机配置
 * @author 丁志宇
 * @date 20141201
 * @version 1.0.0
 */
namespace Pylon
{
	class CInstantCamera;


	using namespace Basler_GigECameraParams;


	class CameraRegularConfiguration : public Pylon::CBaslerGigEConfigurationEventHandler
	{
	public:    
		void OnOpened( Pylon::CBaslerGigEInstantCamera& camera);

		CameraRegularConfiguration( bool is_ref_camera, int expos_time )
			: ref_camera(is_ref_camera), exposure_time(expos_time) {}

		void setExposure(int _time) { exposure_time = _time; }

	private: 
		bool ref_camera;
		unsigned int exposure_time;
	};
}


#endif