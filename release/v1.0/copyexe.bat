@echo ����Ŀ¼
mkdir Debug
mkdir Release

@echo С����
copy ..\..\v1.0\module\filemanagers\test\viewcalcumatfile\nmake\x64\Debug\viewcalcumatfile.exe Debug\viewcalcumatfile.exe
copy ..\..\v1.0\module\filemanagers\test\viewacquisitionfile\nmake\x64\Release\viewacquisitionfile.exe Release\viewacquisitionfile.exe
copy ..\..\v1.0\module\lz_master_main\test\calcu_noslave\nmake\x64\Release\calcu_noslave.exe Release\calcu_noslave.exe

@echo �궨����
copy ..\..\v1.0\module\acquisition\test\LzGrasp\nmake\x64\Release\LzGrasp.exe Release\LzGrasp.exe
copy ..\..\v1.0\module\lz_calibration\test\x64\Release\Lz_Calibration.exe Release\Lz_Calibration.exe

@echo �ͻ��˳���
copy ..\..\v1.0\module\lz_client_main\test\gui\nmake\x64\Debug\lanzhouclient2013.exe Debug\lanzhouclient2013.exe

@echo �칫�ҷ���������
copy ..\..\v1.0\module\lz_office_server_main\test\nmake\x64\Release\lzservertest.exe Release\lzservertest.exe

@echo ���س���
copy ..\..\v1.0\module\lz_master_main\test\gui\nmake\x64\Release\lz_master_gui.exe Release\lz_master_gui.exe
copy ..\..\v1.0\module\lz_master_main\test\monitor\nmake\x64\Release\lz_master_monitor.exe Release\lz_master_monitor.exe

@echo �ӿس���
copy ..\..\v1.0\module\lz_slave_main\test\nmake\x64\Release\lz_slave.exe Release\lz_slave.exe
