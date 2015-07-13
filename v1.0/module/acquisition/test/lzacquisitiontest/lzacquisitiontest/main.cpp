#include "..\..\..\include\LzCamera.h"
#include <Windows.h>

#pragma comment(lib,"opencv_core249d.lib")
#pragma comment(lib,"opencv_highgui249d.lib")
#pragma comment(lib,"opencv_imgproc249d.lib")

int main()
{
	LzCameraAcquisition lzcam(1);
	lzcam.init();

	lzcam.cfgEventHandler("21395152", true);

	lzcam.cfgTriggerMode(AcquiMode::HardwareTrigger);

	lzcam.openCamera();

	vector< AcquiRet > acquiret;
	acquiret.push_back( AcquiRet("21395152",0 ,10) );
	
	for (int i =0; i <60; i++)
	{
		Sleep(500);
		//如果硬触发请屏蔽下面发送软触发信号
		lzcam.cameras[0].TriggerSoftware.Execute( );
	}

	cout << "*********************" << endl;

	Sleep(1000);

	lzcam.discard( acquiret );

	acquiret[0].end = 60;

	////关
	//lzcam.cameras[0].AcquisitionStop.Execute();

	//for (int i =0; i <10; i++)
	//{
	//	Sleep(500);
	//	//如果硬触发请屏蔽下面发送软触发信号
	//	lzcam.cameras[0].TriggerSoftware.Execute( );
	//}

	//cout << "%%%%%%%%%%%%%%%%%%%%%%%" << endl;
	//Sleep(1000);

	////开
	//lzcam.cameras[0].AcquisitionStart.Execute();

	for (int i =0; i <10; i++)
	{
		Sleep(500);
		//如果硬触发请屏蔽下面发送软触发信号
		lzcam.cameras[0].TriggerSoftware.Execute( );
	}

	lzcam.acqui();
	lzcam.exiting(acquiret);

	Sleep(10000);

	return 1;

}