#include "../include/LzCamera.h"

boost::interprocess::interprocess_mutex LzCameraAcquisition::exit_mutex;
boost::interprocess::interprocess_mutex LzCameraAcquisition::discard_mutex;

void LzCameraAcquisition::init() 
{
	DeviceInfoList_t devices;
	try{
		CTlFactory& tlFactory = CTlFactory::GetInstance();

		tlFactory.EnumerateDevices( devices );

		if ( devices.empty() || devices.size()!=numdevices )
		{
			throw RUNTIME_EXCEPTION( "Not enough camera present.");
		}

		cameras.Initialize( numdevices );
		cameras.DestroyDevice();

		// 为相机实例添加设备
		for ( size_t i = 0; i < numdevices; i++)
			cameras[i].Attach( tlFactory.CreateDevice( devices[i] ) );
	}
	catch(std::exception& ex)
	{
		ex.what();
		int i = 0;
		char tmpint[2]; // 0-4是个位数
		sprintf_s(tmpint, "%d", devices.size());
		std::string tmp = "Not enough camera present.," + std::string(tmpint);
		while(i < devices.size())
		{
			tmp += "," + devices.at(i).GetSerialNumber();
			i++;
		}
		throw LzException(100, tmp.c_str());
	}

	files.clear();
	files.resize( numdevices );
	for (int i=0; i<numdevices; i++)
		files[i] = new LzSerialStorageAcqui;

	imghandlers.clear();
	imghandlers.resize( numdevices );
	for (int i=0; i<numdevices; i++)
	{
		imghandlers[i] = new ImageEventHandler;
		imghandlers[i]->attachFile(files[i]);
	}

	//@todo write exception
	if (lzo_init() != LZO_E_OK)
	{
		throw LzException(110, "init lzo_init, the compress part broken!");
	}
}

bool LzCameraAcquisition::cfgEventHandler( Pylon::String_t ID, bool _isref, int exposuretime) 
{
	int i;
	for ( i=0; i<numdevices; i++ )
		if ( cameras[i].GetDeviceInfo().GetSerialNumber() == ID )
			break;

	if ( i == numdevices )
	{
		throw LzException(101, "cfgEventHandler Can't match ID with present cameras!");
	}

	cameras[i].RegisterConfiguration( new CameraRegularConfiguration(_isref, exposuretime), RegistrationMode_ReplaceAll, Cleanup_Delete );
	cameras[i].RegisterImageEventHandler(imghandlers[i], RegistrationMode_Append, Cleanup_None );

	//添加cameraid与序类中几个vector号的对应关系
	camtoindex.insert( std::pair<Pylon::String_t, int>(cameras[i].GetDeviceInfo().GetSerialNumber(), i) );
	imghandlers[i]->tieHoudlerAndCam(this, &(cameras[i]) );

	return true;
}

void LzCameraAcquisition::cfgTriggerMode(AcquiMode mode) 
{
	this->mode = mode;

	if ( mode == SoftwareTrigger )
	{
		for (int i=0; i<numdevices; i++)
		{
			cameras[i].RegisterConfiguration( new CLzSoftwareTriggerConfiguration, RegistrationMode_Append, Cleanup_Delete);
			imghandlers[i]->setAcquiMode( SoftwareTrigger );
		}
	}
	else if (mode == HardwareTrigger )
	{
		for (int i=0; i<numdevices; i++)
		{
			cameras[i].RegisterConfiguration( new  CHardwareTriggerConfiguration, RegistrationMode_Append, Cleanup_Delete );
			imghandlers[i]->setAcquiMode( HardwareTrigger );
		}
	}
	else if (mode == Continues )
	{
		for (int i=0; i<numdevices; i++)
		{
			cameras[i].RegisterConfiguration(new CAcquireContinuousConfiguration, RegistrationMode_Append, Cleanup_Delete);
			imghandlers[i]->setAcquiMode( Continues );
		}
	}

}

void LzCameraAcquisition::openCamera()
{
	try{
		for (int i=0; i<numdevices; i++)
			cameras[i].Open();

		for ( int i=0; i<numdevices; i++ )
			cameras[i].StartGrabbing(GrabStrategy_OneByOne, GrabLoop_ProvidedByInstantCamera);
	}
	catch(std::exception &ex)
	{
		ex.what();
		throw LzException(104, "openCamera Open Camera failed!");
	}
}

void LzCameraAcquisition::waitCameraBufferEmpty()
{
	//while ()

	for (int i=0; i<numdevices; i++)
	{
		if ( cameras[i].NumReadyBuffers.GetValue() > 0 )
		{
			boost::this_thread::sleep_for(
				boost::chrono::milliseconds(10));
			continue;
		}
	}
}

bool LzCameraAcquisition::assignFileInfo( Pylon::String_t ID,const char* filename, DataHead* head) 
{
	int i;
	for ( i=0; i<numdevices; i++ )
		if ( cameras[i].GetDeviceInfo().GetSerialNumber() == ID )
			break;

	if ( i == numdevices )
	{
		throw LzException(101, "assignFileInfo Can't match ID with present cameras!");
	}

	// is grabbing or not
	// clear buffer close file

	files[i]->createFile( filename, sizeof(DataHead) );
	if ( !files[i]->isOpen() )
	{
		throw LzException(102, "assignFileInfo Create file failed!");
	}

	files[i]->setAcquiHead( head );
	return true;

}

void LzCameraAcquisition::setEndMileage( unsigned int _endmile )
{
	for ( int i=0; i<numdevices; i++ )
	{
		if ( files[i]->isOpen() )
			files[i]->setEndMileage(_endmile);
		else
			throw LzException(103, "setEndMileage File isn't opening");
	}
}

void LzCameraAcquisition::setStartMileage( unsigned int _startmile )
{
	for ( int i=0; i<numdevices; i++ )
	{
		if ( files[i]->isOpen() )
			files[i]->setStartMileage(_startmile);
		else
			throw LzException(103, "setStartMileage File isn't opening");
	}
}