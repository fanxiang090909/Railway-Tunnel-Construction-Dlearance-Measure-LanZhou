#include "InitCamera.h"

/**
 * 兰州双目标定采集程序-相机配置
 * @author 丁志宇
 * @date 20141201
 * @version 1.0.0
 */
void Pylon::CameraRegularConfiguration::OnOpened(Pylon::CBaslerGigEInstantCamera& camera)
{
	try
	{
		// Allow all the names in the namespace GenApi to be used without qualification.
		using namespace GenApi;

		camera.LineSelector.SetValue( LineSelector_Line1 );
		camera.LineDebouncerTimeAbs.SetValue( 1 );
		// Set the acquisition mode to continuous(the acquisition mode must
		// be set to continuous when acquisition start triggering is on)


		camera.AcquisitionMode.SetValue( AcquisitionMode_Continuous );

		// Select the acquisition start trigger
		camera.TriggerSelector.SetValue( TriggerSelector_AcquisitionStart );
		// Set the mode for the selected trigger
		camera.TriggerMode.SetValue( TriggerMode_Off );


		camera.TriggerSelector.SetValue( TriggerSelector_FrameStart );
		camera.TriggerMode.SetValue( TriggerMode_On );


		//软触发
		camera.TriggerSource.SetValue ( TriggerSource_Software );

		//camera.AcquisitionStart.Execute();

		//setting exposure time
		camera.ExposureAuto.SetValue(ExposureAuto_Off);
		camera.ExposureTimeAbs.SetValue( exposure_time );
	}
	catch (std::exception &ex)
	{
		ex.what();
		throw ex;
	}

}