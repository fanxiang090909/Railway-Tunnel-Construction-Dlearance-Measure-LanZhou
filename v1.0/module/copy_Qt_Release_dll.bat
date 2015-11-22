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
mkdir .\lz_client_main\test\gui\nmake\x64\Debug\plugins\imageformats
mkdir .\lz_client_main\test\gui\nmake\x64\Debug\plugins\codecs
mkdir .\lz_client_main\test\gui\nmake\x64\Debug\plugins\sqldrivers
copy ..\release\v1.0\Debug\libmysql.dll .\lz_client_main\test\gui\nmake\x64\Debug\libmysql.dll
copy ..\release\v1.0\Debug\plugins\imageformats\qjpegd4.dll .\lz_client_main\test\gui\nmake\x64\Debug\plugins\imageformats\qjpegd4.dll
copy ..\release\v1.0\Debug\plugins\codecs\qcncodecsd4.dll .\lz_client_main\test\gui\nmake\x64\Debug\plugins\codecs\qcncodecsd4.dll
copy ..\release\v1.0\Debug\plugins\sqldrivers\qsqlmysqld4.dll .\lz_client_main\test\gui\nmake\x64\Debug\plugins\sqldrivers\qsqlmysqld4.dll
mkdir .\lz_client_main\test\gui\nmake\x64\Release
mkdir .\lz_client_main\test\gui\nmake\x64\Release\plugins\imageformats
mkdir .\lz_client_main\test\gui\nmake\x64\Release\plugins\codecs
mkdir .\lz_client_main\test\gui\nmake\x64\Release\plugins\sqldrivers
copy ..\release\v1.0\Release\QtCore4.dll .\lz_client_main\test\gui\nmake\x64\Release\QtCore4.dll
copy ..\release\v1.0\Release\QtGui4.dll .\lz_client_main\test\gui\nmake\x64\Release\QtGui4.dll
copy ..\release\v1.0\Release\libmysql.dll .\lz_client_main\test\gui\nmake\x64\Release\libmysql.dll
copy ..\release\v1.0\Release\plugins\imageformats\qjpeg4.dll .\lz_client_main\test\gui\nmake\x64\Release\plugins\imageformats\qjpeg4.dll
copy ..\release\v1.0\Release\plugins\codecs\qcncodecs4.dll .\lz_client_main\test\gui\nmake\x64\Release\plugins\codecs\qcncodecs4.dll
copy ..\release\v1.0\Release\plugins\sqldrivers\qsqlmysql4.dll .\lz_client_main\test\gui\nmake\x64\Release\plugins\sqldrivers\qsqlmysql4.dll
mkdir .\lz_client_main\test\gui\nmake\lanzhouclient2013\plugins\imageformats
mkdir .\lz_client_main\test\gui\nmake\lanzhouclient2013\plugins\codecs
mkdir .\lz_client_main\test\gui\nmake\lanzhouclient2013\plugins\sqldrivers
copy ..\release\v1.0\Release\plugins\imageformats\qjpeg4.dll .\lz_client_main\test\gui\nmake\lanzhouclient2013\plugins\imageformats\qjpeg4.dll
copy ..\release\v1.0\Release\plugins\codecs\qcncodecs4.dll .\lz_client_main\test\gui\nmake\lanzhouclient2013\plugins\qcncodecs4.dll
copy ..\release\v1.0\Release\plugins\sqldrivers\qsqlmysql4.dll .\lz_client_main\test\gui\nmake\lanzhouclient2013\plugins\sqldrivers\qsqlmysql4.dll

@echo 办公室服务器程序
mkdir .\lz_office_server_main\test\nmake\x64\Release
copy ..\release\v1.0\Release\QtCore4.dll .\lz_office_server_main\test\nmake\x64\Release\QtCore4.dll


@echo 主控程序
mkdir .\lz_master_main\test\gui\nmake\x64\Debug
mkdir .\lz_master_main\test\gui\nmake\x64\Debug\plugins\imageformats
copy ..\release\v1.0\Debug\libmysql.dll .\lz_master_main\test\gui\nmake\x64\Debug\libmysql.dll
copy ..\release\v1.0\Debug\plugins\imageformats\qjpegd4.dll .\lz_master_main\test\gui\nmake\x64\Debug\plugins\imageformats\qjpegd4.dll
copy ..\release\v1.0\Debug\plugins\codecs\qcncodecsd4.dll .\lz_master_main\test\gui\nmake\x64\Debug\plugins\codecs\qcncodecsd4.dll
copy ..\release\v1.0\Debug\plugins\sqldrivers\qsqlmysqld4.dll .\lz_master_main\test\gui\nmake\x64\Debug\plugins\sqldrivers\qsqlmysqld4.dll

mkdir .\lz_master_main\test\gui\nmake\x64\Release
mkdir .\lz_master_main\test\gui\nmake\x64\Release\plugins\imageformats
mkdir .\lz_master_main\test\gui\nmake\x64\Release\plugins\codecs
mkdir .\lz_master_main\test\gui\nmake\x64\Release\plugins\sqldrivers
copy ..\release\v1.0\Release\QtCore4.dll .\lz_master_main\test\gui\nmake\x64\Release\QtCore4.dll
copy ..\release\v1.0\Release\QtGui4.dll .\lz_master_main\test\gui\nmake\x64\Release\QtGui4.dll
copy ..\release\v1.0\Release\libmysql.dll .\lz_master_main\test\gui\nmake\x64\Release\libmysql.dll
copy ..\release\v1.0\Release\plugins\imageformats\qjpeg4.dll .\lz_master_main\test\gui\nmake\x64\Release\plugins\imageformats\qjpeg4.dll
copy ..\release\v1.0\Release\plugins\codecs\qcncodecs4.dll .\lz_master_main\test\gui\nmake\x64\Release\plugins\codecs\qcncodecs4.dll
copy ..\release\v1.0\Release\plugins\sqldrivers\qsqlmysql4.dll .\lz_master_main\test\gui\nmake\x64\Release\plugins\sqldrivers\qsqlmysql4.dll

mkdir .\lz_master_main\test\gui\nmake\lz_master_gui\plugins\imageformats
mkdir .\lz_master_main\test\gui\nmake\lz_master_gui\plugins\codecs
mkdir .\lz_master_main\test\gui\nmake\lz_master_gui\plugins\sqldrivers
copy ..\release\v1.0\Release\plugins\imageformats\qjpeg4.dll .\lz_master_main\test\gui\nmake\lz_master_gui\plugins\imageformats\qjpeg4.dll
copy ..\release\v1.0\Release\plugins\codecs\qcncodecs4.dll .\lz_master_main\test\gui\nmake\lz_master_gui\plugins\codecs\qcncodecs4.dll
copy ..\release\v1.0\Release\plugins\sqldrivers\qsqlmysql4.dll .\lz_master_main\test\gui\nmake\lz_master_gui\plugins\sqldrivers\qsqlmysql4.dll


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
