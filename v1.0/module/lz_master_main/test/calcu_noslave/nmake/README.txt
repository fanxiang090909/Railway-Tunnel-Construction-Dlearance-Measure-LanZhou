���ؽ�������������

�����ߡ� ��ѩ ����
���汾�� v1.0.0
�����ڡ� 2014��1��

��VS2012�������á�
1. ����ѡ��x64
2. ���ͷ�ļ�����Ŀ¼
	����->VC++Ŀ¼->����Ŀ¼
C:\Qt\include
C:\Qt\include\QtCore
C:\Qt\include\QtNetwork��Qt���紫��Socket��̣�
C:\Qt\include\QtGui��Qt���棩
C:\Qt\include\QtSql��Qt���ݿ�������������⣩

C:\Qt\include;C:\Qt\include\QtCore;C:\Qt\include\QtNetwork;C:\Qt\include\QtGui;C:\Qt\include\QtSql

3. ��Ӿ�̬���ӿ�libĿ¼
	����->VC++Ŀ¼->��Ŀ¼
C:\Qt\lib

4. ��Ӹ����������̬���ӿ�lib�ļ�
��Debug�桿
qtmaind.lib
QtCored4.lib
QtGuid4.lib
QtNetworkd4.lib
QtSqld4.lib
��Release�桿
qtmain.lib
QtCore4.lib
QtGui4.lib
QtNetwork4.lib
QtSql4.lib

5. �������Ӻ��ڹ���Debug��ReleaseĿ¼������ִ��.exe�ļ��������

qsqlmysql4.dl
qsqlmysqld4.dll
libmysql.dll
codecs�ļ���
iconengines�ļ���
sqldrivers�ļ����е��ļ�
ע��
(1)qsqlmysql4.dll��libmysql.dll��MySQL���ݿ����������Qt��װ���Դ�����Ҫ�Լ����룩
����ע�������Ҫ���mysql��lib�⣬
64λ��mysql���Ӧ64λ��QT�����64λ��QMYSQL������
32λ��mysql���Ӧ32λ��QT�����32λ��QMYSQL������
���������Ŀ¼
\\10.13.29.236\Temp Directory\QT\QT�����64λ����
\\10.13.29.236\Temp Directory\QT\QT����mysql����
(2)codecs��Qt��װĿ¼�µ�һ�������������Ӣ�ĵ�ת��
��QTĿ¼�У�C:\Qt\plugins\codecs
iconengines��Qt��װĿ¼�µ�һ��������������ͼ��icon���
��QTĿ¼�У�C:\Qt\plugins\iconengines
sqldrivers��Qt��װĿ¼�µ�һ���������Qt���ݿ�������
��QTĿ¼�У�C:\Qt\plugins\sqldrivers