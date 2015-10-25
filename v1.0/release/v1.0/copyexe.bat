@echo 创建目录
mkdir Debug
mkdir Release

@echo 小工具
copy ..\..\module\filemanagers\test\viewcalcumatfile\nmake\x64\Debug\viewcalcumatfile.exe Debug\viewcalcumatfile.exe
copy ..\..\module\filemanagers\test\view3dtvmatfile\nmake\x64\Debug\view3dtvmatfile.exe Debug\view3dtvmatfile.exe
copy ..\..\module\filemanagers\test\viewacquisitionfile\nmake\x64\Release\viewacquisitionfile.exe Release\viewacquisitionfile.exe
copy ..\..\module\lz_master_main\test\calcu_noslave\nmake\x64\Release\calcu_noslave.exe Release\calcu_noslave.exe

@echo 标定程序
copy ..\..\module\acquisition\test\LzGrasp\nmake\x64\Release\LzGrasp.exe Release\LzGrasp.exe
copy ..\..\module\lz_calibration\test\x64\Release\Lz_Calibration.exe Release\Lz_Calibration.exe

@echo 客户端程序
copy ..\..\module\lz_client_main\test\gui\nmake\x64\Debug\lanzhouclient2013.exe Debug\lanzhouclient2013.exe

@echo 办公室服务器程序
copy ..\..\module\lz_office_server_main\test\nmake\x64\Release\lzservertest.exe Release\lzservertest.exe

@echo 主控程序
copy ..\..\module\lz_master_main\test\gui\nmake\x64\Release\lz_master_gui.exe Release\lz_master_gui.exe
copy ..\..\module\lz_master_main\test\monitor\nmake\x64\Release\lz_master_monitor.exe Release\lz_master_monitor.exe

@echo 从控程序
copy ..\..\module\lz_slave_main\test\slave_main\nmake\x64\Release\lz_slave.exe Release\lz_slave.exe
copy ..\..\module\lz_slave_main\test\slave_mini\nmake\x64\Release\slave_mini.exe Release\slave_mini.exe

@echo 配置文件
copy .\client_datastore_path.txt Debug\client_datastore_path.txt
copy .\slave_datastore_path.txt Debug\slave_datastore_path.txt
copy .\master_datastore_path.txt Release\master_datastore_path.txt
copy .\server_datastore_path.txt Release\server_datastore_path.txt
copy .\slave_datastore_path.txt Release\slave_datastore_path.txt

@echo 解压缩DLL
set path=%path%;C:\Program Files\WinRAR
cd Debug
Rar.exe x -ibck -y -o+ ..\Debug_dll.rar
cd ..
cd Release
Rar.exe x -ibck -y -o+ ..\Release_dll.rar