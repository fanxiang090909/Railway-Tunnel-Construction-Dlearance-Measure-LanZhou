@echo 创建目录
mkdir Release

@echo 小工具
copy ..\..\module\filemanagers\test\viewcalcumatfile\nmake\x64\Release\viewcalcumatfile.exe Release\viewcalcumatfile.exe
copy ..\..\module\filemanagers\test\view3dtvfile\nmake\x64\Release\view3dtvmatfile.exe Release\view3dtvmatfile.exe
copy ..\..\module\filemanagers\test\viewacquisitionfile\nmake\x64\Release\viewacquisitionfile.exe Release\viewacquisitionfile.exe
copy ..\..\module\lz_master_main\test\calcu_noslave\nmake\x64\Release\calcu_noslave.exe Release\calcu_noslave.exe

@echo 标定程序
copy ..\..\module\acquisition\test\LzGrasp\nmake\x64\Release\LzGrasp.exe Release\LzGrasp.exe
copy ..\..\module\lz_calibration\test\x64\Release\Lz_Calibration.exe Release\Lz_Calibration.exe

@echo 客户端程序
copy ..\..\module\lz_client_main\test\gui\nmake\x64\Release\lanzhouclient2013.exe Release\lanzhouclient2013.exe

@echo 办公室服务器程序
copy ..\..\module\lz_office_server_main\test\nmake\x64\Release\lzservertest.exe Release\lzservertest.exe

@echo 主控程序
copy ..\..\module\lz_master_main\test\gui\nmake\x64\Release\lz_master_gui.exe Release\lz_master_gui.exe
copy ..\..\module\lz_master_main\test\monitor\nmake\x64\Release\lz_master_monitor.exe Release\lz_master_monitor.exe

@echo 从控程序
copy ..\..\module\lz_slave_main\test\slave_main\nmake\x64\Release\lz_slave.exe Release\lz_slave.exe
copy ..\..\module\lz_slave_main\test\slave_mini\nmake\x64\Release\slave_mini.exe Release\slave_mini.exe


@echo 解压缩DLL
set path=%path%;C:\Program Files\WinRAR
cd Release
Rar.exe x -ibck -y -o+ ..\Release_dll.rar