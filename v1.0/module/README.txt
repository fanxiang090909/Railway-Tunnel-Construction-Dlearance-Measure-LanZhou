/**
 * ��Ҫ����˵��������ģ��˵��
 * @author ����
 * @date 2015-05-26
 * @version 1.0.0
 */
����������Windows��x64����VS2012��MSVC110�� + Qt4.8.0 + Boost + OpenCV2.4.9
�����������а�װ���������� \\10.13.29.236\Public Resource\0.������Ŀ\Lz������·2012\_��������+Ŀ�����װ���л���

��Ҫ����˵����
1���ͻ��˳��򣨻𳵺Ͱ칫�Ҿ���ʹ�ã�
	module\lz_client_main\test\gui\nmake\lanzhouclient2013.sln

2) ���س��򣨻��ϣ�
   2.1) ȫ�����򣨰����ɼ����㱸�ݣ���
	module\lz_master_main\test\lz_master_gui\nmake\lz_master_gui.sln
   2.2) �����汾��ֻ�����ɼ����֣�
	module\lz_master_main\test\monitor\nmake\lz_master_monitor.sln

3���ӿس��򣨻��ϣ�
   3.1���ӿ�������
   	module\lz_slave_main\test\nmake\slave_main\lz_slave.sln
   3.2���ӿؿ��Ƴ���-�ͻ���
   	module\lz_slave_main\test\nmake\slave_mini\slave_mini.sln


4) �칫�ҷ��������򡢴ӿؿ��Ƴ���-���������칫�ҡ��𳵣�
	module\lz_office_server_main\test\nmake\lzservertest.sln

5���궨���򣨼���˫Ŀ�����б궨��
   5.1��˫Ŀ�궨�ɼ�����
        module\acquistion\test\LzGrasp\nmake\LzGrasp.sln
   5.2���궨�������
	module\lz_calibration\test\Lz_Calibration.sln

6) ToolBoxС����
   6.1) �ļ��鿴-�ɼ�ԭʼͼ���ļ��鿴
	module\filemanagers\test\viewacquisitionfile\nmake\viewacquisitionfile.sln
   6.2) �ļ��鿴-�м���������ںϼ������ļ��鿴�����澫�����Ա�
	module\filemanagers\test\viewcalcumatfile\nmake\viewcalcumatfile.sln
   6.3) �ļ��鿴-˫Ŀ��ά��+ԭͼ�鿴
	module\filemanagers\test\view3dtvfile\nmake\view3dtvmatfile.sln
   6.4) �������㣨���ݶ��ڱ��������ǻ��ϵ�����ֲ�ʽ���㷽ʽ��
	module\lz_master_main\test\calcu_noslave\nmake\calcu_noslave.sln


ģ��˵��
acquisition - �ɼ�
calculate - �����ܡ�Ԥ����˫Ŀ���㡢�ںϼ��㡢�ۺϼ���
compress - ѹ�����ɼ�ͼƬѹ�����������ļ�ѹ����ѹ��
datastructure - �����������ͣ��ƻ�������У�ʵ�ʲɼ�������У�У��������е�
filemanagers - ��ʽ�ļ���д����QtUI��ͼ���ά����ά�������ʾ����
filetransport - �����ļ����䣺�ļ����ͽ���
lz_calibration - �궨
lz_client_main - �ͻ�����س���ҵ���߼���QtUI
lz_db - ���ݿ����
lz_exception - �쳣
lz_logger - ��־
lz_master_main - ������س���ҵ���߼���QtUI
lz_office_server_main �칫�ҷ�������س���ҵ���߼���QtUI
lz_output - ͼ�������html��excel
lz_slave_main - �ӿ���س���ҵ���߼���QtUI
network_cs - ������Ϣ����
pulsehandle - Ӳ���ӿڿ���أ����ɳ�������ͽ��������
xmlloader - XML�����ļ���д