Ŀ¼˵����

icon ͼ��Ŀ¼

Startup ���������������������ļ�

copyexe.bat
- ��module�и���VS �����Ѿ������release�汾��ǰ���£�ֱ�ӿ���exe����Ŀ¼�µ�Release�ļ����У�ͬʱ��ѹRelease_dll.rar��ReleaseĿ¼�С�

copy_config.bat
- ����4��txt�����ļ���ReleaseĿ¼��

copy_config_test.bat
- ����4��txt�����ļ���ReleaseĿ¼��

��Ҫ�����г���˵����
1) �ͻ��˳��򣨻𳵺Ͱ칫�Ҿ���ʹ�ã���lanzhouclient2013.exe
2) ���س��򣨻��ϣ�
   2.1) ȫ�����򣨰����ɼ����㱸�ݣ���lz_master_gui.exe
   2.2) �����汾��ֻ�����ɼ����֣���lz_master_monitor.exe
3) �ӿس��򣨻��ϣ�
   3.1) �ӿ�������lz_slave.exe
   3.2) �ӿؿ��Ƴ���-�ͻ��ˣ�slave_mini.exe
4) �칫�ҷ��������򡢴ӿؿ��Ƴ���-���������칫�ҡ��𳵣���lzservertest.exe
5) �궨���򣨼���˫Ŀ�����б궨��
   5.1) ˫Ŀ�궨�ɼ�����LzGrasp.exe
   5.2) �궨�������Lz_Calibration.exe
6) ToolBoxС����
   6.1) �ļ��鿴-�ɼ�ԭʼͼ���ļ��鿴	��viewacquisitionfile.exe
   6.2) �ļ��鿴-�м���������ںϼ������ļ��鿴�����澫�����Աȣ�viewcalcumatfile.exe
   6.3) �ļ��鿴-˫Ŀ��ά��+ԭͼ�鿴��view3dtvmatfile.exe
   6.4) �������㣨���ݶ��ڱ��������ǻ��ϵ�����ֲ�ʽ���㷽ʽ����calcu_noslave.exe
ÿ�������г����ڳ�ʼ��ʱ���ȶ�����Ҫ�Լ��ĳ�ʼ�������ļ�����������ڻ���Ϣ������򿪲��ɹ���
�����ʼ�������ļ�����ȷָ������Ŀ������·�������������Ҫ������·���ж�ȡ��Ŀ����Ŀ¼��system�µ������ļ������������ļ�����������������в����ڻ��ȡ���󣬳���Ҳ��������ʧ�ܡ�


�����г������ʼ�������ļ���Ӧ��ϵ��
�ڳ����ʼ��ʱ�����ȼ���Ƿ������ʼ�������ļ������������ļ�����Ϣ���룬�����ִ���������ʧ�ܡ�
ÿ��������exe���򶼻��Ӧһ����ʼ�������ļ�����Ӧ��ϵ���£�
lanzhouclient2013.exe      -> client_datastore_path.txt
lz_master_gui.exe          -> master_datastore_path.txt
lz_master_monitor.exe      -> master_datastore_path.txt
lzservertest.exe           -> server_datastore_path.txt
lz_slave.exe               -> slave_datastore_path.txt
slave_mini.exe             -> slave_datastore_path.txt
viewacquisitionfile.exe    -> slave_datastore_path.txt
viewcalcumatfile.exe       -> slave_datastore_path.txt
view3dtvmatfile.exe        -> slave_datastore_path.txt
calcu_noslave.exe          -> master_datastore_path.txt


�����ļ�˵����
1)	client_datastore_path.txt �ͻ��˳��������ļ�
ֻ����1�У���ʾ�����洢��Ŀ����Ŀ¼
2)	master_datastore_path.txt���س��������ļ�
����2�У�
��һ�У���ʾ�����洢��Ŀ����Ŀ¼��
�ڶ��У���ʾĬ�ϵ�0.5m/֡ģʽ�µģ�������֡����̼����
3)	server_datastore_path.txt�칫�ҷ��������򡢴ӿؿ��Ƴ���-�����������ļ�
����2�У�
��һ�У���ʾ�����洢��Ŀ����Ŀ¼��
�ڶ��У���ʾNAS�ϴ洢���ػ������ļ���NAS IP��ַ��
        ��NAS�ڻ��ϱ���ʱ��NAS1����NAS2��
Ĭ��NAS1�ϴ洢�������ݡ��ӿ�1-4�����ݣ�NAS2�ϴ洢�ӿ�5-9�����ݡ�
4)	slave_datastore_path.txt
����4�У�
��һ�У���ʾ�����洢��Ŀ����Ŀ¼��
�ڶ��У����������������ػ��ı����˿�IP��ַ����Ϊ��������˿ڿ����кࣩܶ��
�����У��ӿس���Ŀ¼��ר�����ڴӿؿ��Ƴ���-�ͻ��ˣ�slave_mini.exe����
�����У��ӿس����������Ŀ¼��ר�����ڴӿؿ��Ƴ���-�ͻ��ˣ�slave_mini.exe����