�ɼ����Գ�������

�����ߡ� ��־��
���汾�� v1.0.0
�����ڡ� 2014��1��

��VS2012�������á�
1. ����ѡ��x64
2. ���ͷ�ļ�����Ŀ¼
	����->VC++Ŀ¼->����Ŀ¼
C:\opencvx64\include;
C:\Program Files\Basler\pylon 4\pylon\include;
C:\Program Files\Basler\pylon 4\genicam\library\cpp\include;
C:\Qt\include;
C:\Qt\include\QtCore;
C:\Program Files\Basler\pylon 4\pylon\include;C:\Program Files\Basler\pylon 4\genicam\library\cpp\include;C:\Qt\include;C:\Qt\include\QtCore;C:\Qt\include\QtNetwork;C:\Qt\include\QtGui;C:\Qt\include\QtSql;..\..\..\..\..\include;..\..\..\include;..\..\..\include\gui;..\..\..\..\datastructure\include;..\..\..\..\filetransport\include;..\..\..\..\xmlloader\include;..\..\..\..\xmlloader\include\tinyxml;..\..\..\..\network_cs\include\client;..\..\..\..\calculate\include;..\..\..\..\calculate\include\icp;..\..\..\..\compress\include;..\..\..\..\lz_exception\include;..\..\..\..\filemanagers\include;..\..\..\..\filemanagers\include\gui;..\..\..\..\..\..\include;..\..\..\..\acquisition\include;D:\Boost\include\boost-1_55;D:\opencvx64\include;
3. ��Ӿ�̬���ӿ�libĿ¼
	����->VC++Ŀ¼->��Ŀ¼
C:\Program Files\Basler\pylon 4\pylon\lib\x64;
C:\Program Files\Basler\pylon 4\genicam\library\cpp\lib\win64_x64;
C:\Qt\lib;
C:\opencvx64\lib;
C:\Program Files\Basler\pylon 4\pylon\lib\x64;C:\Program Files\Basler\pylon 4\genicam\library\cpp\lib\win64_x64;C:\Qt\lib;D:\opencvx64\lib;D:\Boost\lib\x64;

4. ��Ӹ����������̬���ӿ�lib�ļ�
��Debug�桿
qtmaind.lib
QtCored4.lib
opencv_calib3d249d.lib
opencv_contrib249d.lib
opencv_core249d.lib
opencv_features2d249d.lib
opencv_flann249d.lib
opencv_gpu249d.lib
opencv_highgui249d.lib
opencv_imgproc249d.lib
opencv_legacy249d.lib
opencv_ml249d.lib
opencv_nonfree249d.lib
opencv_objdetect249d.lib
opencv_ocl249d.lib
opencv_photo249d.lib
opencv_stitching249d.lib
��VS2012��VS2010���е���ʱ��Ҫ
����һ������ӿ⣬debug�����ã�
�������� #pragma comment QAxContainerd.lib����

��Release�桿
qtmain.lib
QtCore4.lib
opencv_calib3d249.lib
opencv_contrib249.lib
opencv_core249.lib
opencv_features2d249.lib
opencv_flann249.lib
opencv_gpu249.lib
opencv_highgui249.lib
opencv_imgproc249.lib
opencv_legacy249.lib
opencv_nonfree249.lib
��VS2012��VS2010���е���ʱ��Ҫ
����һ������ӿ⣬release�����ã�
�������� #pragma comment QAxContainer.lib����