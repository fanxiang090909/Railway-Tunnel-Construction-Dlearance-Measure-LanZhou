办公室界面程序

【作者】 熊雪 范翔
【版本】 v1.0.0
【日期】 2014年1月


【编译连接注意】
 需要连接器
 * QtCore4.lib QtCored4.lib QtGui4.lib QtGuid4.lib QtNetwork4.lib QtNetworkd4.lib
 * QtSql4.lib QtSqld4.lib 
在VS2012（VS2010）中调试时需要
方法一添加链接库，debug中配置，
方法二用 #pragma comment QAxContainerd.lib声明

QTCreator中需要
在pro中加CONFIG+=qaxcontainer

【参考说明】
 QAxContainerd.lib EXCEL的读取 Using ActiveX controls and COM in Qt 
详见 http://qt-project.org/doc/qt-4.8/activeqt-container.html
