#include "..\include\CameraConfiguration.h"

namespace Pylon{

	void CameraRegularConfiguration::OnOpened( CBaslerGigEInstantCamera& camera )
	{
		try
		{
			// Allow all the names in the namespace GenApi to be used without qualification.
			using namespace GenApi;
			// Get the camera control object.
			INodeMap &control = camera.GetNodeMap();
			// Get the parameters for setting the image area of interest (Image AOI).
			const CIntegerPtr width = control.GetNode("Width");
			const CIntegerPtr height = control.GetNode("Height");
			const CIntegerPtr offsetX = control.GetNode("OffsetX");
			const CIntegerPtr offsetY = control.GetNode("OffsetY");

			// Maximize the Image AOI.
			width->SetValue( ROI_WIDTH );
			height->SetValue( ROI_HEIGHT );

			if ( IsWritable(offsetX) )
				offsetX->SetValue(offsetX->GetMin());

			if ( IsWritable(offsetY) ){
				if ( ref_camera )
					offsetY->SetValue( offsetY->GetMax() );
				else
					offsetY->SetValue( offsetY->GetMin() );
			}  

			//enalble chunk mode
			if (GenApi::IsWritable(camera.ChunkModeActive))
				camera.ChunkModeActive.SetValue(true);
			else
				throw RUNTIME_EXCEPTION("The camera doesn't support chunk features");

			// enable frame counter
			camera.ChunkSelector.SetValue(ChunkSelector_Framecounter);
			camera.ChunkEnable.SetValue(true);

			// enable trigger input counter
			camera.ChunkSelector.SetValue( ChunkSelector_Triggerinputcounter );
			camera.ChunkEnable.SetValue(true);

			// clear frame counter
			camera.CounterSelector.SetValue(CounterSelector_Counter2);
			camera.CounterEventSource.SetValue(CounterEventSource_FrameStart);
			camera.CounterResetSource.SetValue(CounterResetSource_Software);
			camera.CounterReset.Execute();
			camera.CounterSelector.SetValue(CounterSelector_Counter1);
			camera.CounterEventSource.SetValue(CounterEventSource_FrameTrigger);
			camera.CounterResetSource.SetValue(CounterResetSource_Software);
			camera.CounterReset.Execute();
			camera.CounterResetSource.SetValue(CounterResetSource_Off);

			// set camera buffer
			camera.MaxNumBuffer.SetValue( CAMERA_BUFFER );

			//setting exposure time
			camera.ExposureAuto.SetValue(ExposureAuto_Off);
			camera.ExposureMode.SetValue(ExposureMode_Timed);
			camera.ExposureTimeAbs.SetValue( exposure_time );

			// Get the Packet Size for GigE cameras
			const CIntegerPtr packetSizeGigE= control.GetNode("GevSCPSPacketSize");
			// Set the Packet Size to 1500 by default.
			// If your network adapter and/or switch supported Jumbo Frames,
			// you may consider using higher values for Packet Size, e.g. 3000, 6000, 9000.
			// By rule of thumb, the bigger the Packet Size- the better the overall performance.
			if (IsWritable(packetSizeGigE))
				packetSizeGigE->SetValue(1500);
		}
		catch (std::exception &ex)
		{
			ex.what();
			throw LzException(105, "CameraRegularConfiguration, Could not apply configuration!");
		}
	}

	void CHardwareTriggerConfiguration::OnOpened( CBaslerGigEInstantCamera& camera ) 
	{
		try{
			
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
			
			//硬触发
			camera.TriggerSource.SetValue ( TriggerSource_Line1 );
			camera.TriggerActivation.SetValue( TriggerActivation_RisingEdge );

		}
		catch(std::exception &ex)
		{
			ex.what();
			throw LzException(106, "CHardwareTriggerConfiguration, Could not set trigger!");
		}
	}

	void CLzSoftwareTriggerConfiguration::OnOpened( CBaslerGigEInstantCamera& camera ) 
	{
		try{
			
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
		}
		catch(std::exception &ex)
		{
			ex.what();
			throw LzException(106, "CSoftwareTriggerConfiguration, Could not set trigger!");
		}
	}
}