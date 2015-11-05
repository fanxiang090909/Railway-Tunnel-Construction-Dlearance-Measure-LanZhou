@echo 小工具
mkdir .\filemanagers\test\viewcalcumatfile\nmake\x64\Release
copy ..\release\v1.0\Release\QtCore4.dll .\filemanagers\test\viewcalcumatfile\nmake\x64\Release\QtCore4.dll
copy ..\release\v1.0\Release\QtGui4.dll .\filemanagers\test\viewcalcumatfile\nmake\x64\Release\QtGui4.dll

mkdir .\filemanagers\test\view3dtvfile\nmake\x64\Release
copy ..\release\v1.0\Release\QtCore4.dll .\filemanagers\test\view3dtvfile\nmake\x64\Release\QtCore4.dll
copy ..\release\v1.0\Release\QtGui4.dll .\filemanagers\test\view3dtvfile\nmake\x64\Release\QtGui4.dll

mkdir .\filemanagers\test\viewacquisitionfile\nmake\x64\Release
copy ..\release\v1.0\Release\QtCore4.dll .\filemanagers\test\viewacquisitionfile\nmake\x64\Release\QtCore4.dll
copy ..\release\v1.0\Release\QtGui4.dll .\filemanagers\test\viewacquisitionfile\nmake\x64\Release\QtGui4.dll

mkdir .\lz_master_main\test\calcu_noslave\nmake\x64\Release
copy ..\release\v1.0\Release\QtCore4.dll .\lz_master_main\test\calcu_noslave\nmake\x64\Release\QtCore4.dll
copy ..\release\v1.0\Release\QtGui4.dll .\lz_master_main\test\calcu_noslave\nmake\x64\Release\QtGui4.dll

@echo 标定程序
mkdir .\lz_calibration\test\x64\Release
copy ..\release\v1.0\Release\QtCore4.dll .\lz_calibration\test\x64\Release\QtCore4.dll
copy ..\release\v1.0\Release\QtGui4.dll .\lz_calibration\test\x64\Release\QtGui4.dll

@echo 客户端程序
mkdir .\lz_client_main\test\gui\nmake\x64\Debug
copy ..\release\v1.0\Debug\libmysql.dll .\lz_client_main\test\gui\nmake\x64\Debug\libmysql.dll
mkdir .\lz_client_main\test\gui\nmake\x64\Release
copy ..\release\v1.0\Release\QtCore4.dll .\lz_client_main\test\gui\nmake\x64\Release\QtCore4.dll
copy ..\release\v1.0\Release\QtGui4.dll .\lz_client_main\test\gui\nmake\x64\Release\QtGui4.dll
copy ..\release\v1.0\Release\libmysql.dll .\lz_client_main\test\gui\nmake\x64\Release\libmysql.dll

@echo 办公室服务器程序
mkdir .\lz_office_server_main\test\nmake\x64\Release
copy ..\release\v1.0\Release\QtCore4.dll .\lz_office_server_main\test\nmake\x64\Release\QtCore4.dll
copy ..\release\v1.0\Release\QtGui4.dll .\lz_office_server_main\test\nmake\x64\Release\QtGui4.dll

@echo 主控程序
mkdir .\lz_master_main\test\gui\nmake\x64\Debug
copy ..\release\v1.0\Debug\libmysql.dll .\lz_master_main\test\gui\nmake\x64\Debug\libmysql.dll

mkdir .\lz_master_main\test\gui\nmake\x64\Release
copy ..\release\v1.0\Release\QtCore4.dll .\lz_master_main\test\gui\nmake\x64\Release\QtCore4.dll
copy ..\release\v1.0\Release\QtGui4.dll .\lz_master_main\test\gui\nmake\x64\Release\QtGui4.dll
copy ..\release\v1.0\Release\libmysql.dll .\lz_master_main\test\gui\nmake\x64\Release\libmysql.dll

mkdir .\lz_master_main\test\monitor\nmake\x64\Release
copy ..\release\v1.0\Release\QtCore4.dll .\lz_master_main\test\monitor\nmake\x64\Release\QtCore4.dll
copy ..\release\v1.0\Release\QtGui4.dll .\lz_master_main\test\monitor\nmake\x64\Release\QtGui4.dll

@echo 从控程序
mkdir .\lz_slave_main\test\slave_main\nmake\x64\Release
copy ..\release\v1.0\Release\QtCore4.dll .\lz_slave_main\test\slave_main\nmake\x64\Release\QtCore4.dll
copy ..\release\v1.0\Release\QtGui4.dll .\lz_slave_main\test\slave_main\nmake\x64\Release\QtGui4.dll

mkdir .\lz_slave_main\test\slave_mini\nmake\x64\Release
copy ..\release\v1.0\Release\QtCore4.dll .\lz_slave_main\test\slave_mini\nmake\x64\Release\QtCore4.dll
copy ..\release\v1.0\Release\QtGui4.dll .\lz_slave_main\test\slave_mini\nmake\x64\Release\QtGui4.dll
