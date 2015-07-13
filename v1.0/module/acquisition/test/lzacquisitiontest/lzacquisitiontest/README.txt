采集测试程序运行

【作者】 丁志宇
【版本】 v1.0.0
【日期】 2014年1月

【VS2012运行配置】
1. 调试选择x64
2. 添加头文件包含目录
	属性->VC++目录->包含目录
C:\opencvx64\include;
C:\Program Files\Basler\pylon 4\pylon\include;
C:\Program Files\Basler\pylon 4\genicam\library\cpp\include;
C:\Qt\include;
C:\Qt\include\QtCore;
C:\Program Files\Basler\pylon 4\pylon\include;C:\Program Files\Basler\pylon 4\genicam\library\cpp\include;C:\Qt\include;C:\Qt\include\QtCore;C:\Qt\include\QtNetwork;C:\Qt\include\QtGui;C:\Qt\include\QtSql;..\..\..\..\..\include;..\..\..\include;..\..\..\include\gui;..\..\..\..\datastructure\include;..\..\..\..\filetransport\include;..\..\..\..\xmlloader\include;..\..\..\..\xmlloader\include\tinyxml;..\..\..\..\network_cs\include\client;..\..\..\..\calculate\include;..\..\..\..\calculate\include\icp;..\..\..\..\compress\include;..\..\..\..\lz_exception\include;..\..\..\..\filemanagers\include;..\..\..\..\filemanagers\include\gui;..\..\..\..\..\..\include;..\..\..\..\acquisition\include;D:\Boost\include\boost-1_55;D:\opencvx64\include;
3. 添加静态链接库lib目录
	属性->VC++目录->库目录
C:\Program Files\Basler\pylon 4\pylon\lib\x64;
C:\Program Files\Basler\pylon 4\genicam\library\cpp\lib\win64_x64;
C:\Qt\lib;
C:\opencvx64\lib;
C:\Program Files\Basler\pylon 4\pylon\lib\x64;C:\Program Files\Basler\pylon 4\genicam\library\cpp\lib\win64_x64;C:\Qt\lib;D:\opencvx64\lib;D:\Boost\lib\x64;

4. 添加附加依赖项，静态链接库lib文件
【Debug版】
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
在VS2012（VS2010）中调试时需要
方法一添加链接库，debug中配置，
方法二用 #pragma comment QAxContainerd.lib声明

【Release版】
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
在VS2012（VS2010）中调试时需要
方法一添加链接库，release中配置，
方法二用 #pragma comment QAxContainer.lib声明