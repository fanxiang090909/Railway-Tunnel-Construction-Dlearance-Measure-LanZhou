�ͻ��˳�������

�����ߡ� ��ѩ ����
���汾�� v1.0.0
�����ڡ� 2014��1��

��VS2012�������á�
1. ����ѡ��x64
2. ���ͷ�ļ�����Ŀ¼
	����->VC++Ŀ¼->����Ŀ¼
C:\Qt2\4.8.0\include
C:\Qt2\4.8.0\include\QtCore
C:\Qt2\4.8.0\include\QtNetwork��Qt���紫��Socket��̣�
C:\Qt2\4.8.0\include\QtGui��Qt���棩
C:\Qt2\4.8.0\include\QtSql��Qt���ݿ�������������⣩
C:\Qt2\4.8.0\include\QtWebKit (QtǶ��WebKit��������ʿ�)

C:\Qt2\4.8.0\include;C:\Qt2\4.8.0\include\QtCore;C:\Qt2\4.8.0\include\QtNetwork;C:\Qt2\4.8.0\include\QtGui;C:\Qt2\4.8.0\include\QtSql;C:\Qt2\4.8.0\include\QtWebKit

ֱ�ӿ���
D:\Boost\include\boost-1_55;C:\Qt2\4.8.0\include\QtSql;C:\Qt2\4.8.0\include\QtCore;C:\Qt2\4.8.0\include;C:\Qt2\4.8.0\include\QtGui;C:\Qt2\4.8.0\include\QtNetwork;C:\Qt2\4.8.0\include\Qt;C:\Qt2\4.8.0\include\ActiveQt;..\..\..\..\include;..\..\..\..\include\gui;..\..\..\..\..\lz_db\include;..\..\..\..\..\lz_db\include\gui;..\..\..\..\..\xmlloader\include;..\..\..\..\..\xmlloader\include\tinyxml;..\..\..\..\..\filetransport\include;..\..\..\..\..\compress\include;..\..\..\..\..\datastructure\include;..\..\..\..\..\network_cs\include\client;..\..\..\..\..\filemanagers\include;..\..\..\..\..\lz_exception\include;..\..\..\..\..\calculate\include;..\..\..\..\..\lz_output\include;..\..\..\..\..\lz_output\include\gui;..\..\..\..\..\lz_logger\include;$(IncludePath)

3. ��Ӿ�̬���ӿ�libĿ¼
	����->VC++Ŀ¼->��Ŀ¼
C:\Qt2\4.8.0\lib

4. ��Ӹ����������̬���ӿ�lib�ļ�
��Debug�桿
qtmaind.lib
QtCored4.lib
QtGuid4.lib
QtNetworkd4.lib
QtSqld4.lib
��VS2012��VS2010���е���ʱ��Ҫ
����һ������ӿ⣬debug�����ã�
�������� #pragma comment QAxContainerd.lib����

��Release�桿
qtmain.lib
QtCore4.lib
QtGui4.lib
QtNetwork4.lib
QtSql4.lib
��VS2012��VS2010���е���ʱ��Ҫ
����һ������ӿ⣬release�����ã�
�������� #pragma comment QAxContainer.lib����

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