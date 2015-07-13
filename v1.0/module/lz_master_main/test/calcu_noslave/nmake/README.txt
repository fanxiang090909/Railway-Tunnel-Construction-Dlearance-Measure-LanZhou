主控界面主程序运行

【作者】 熊雪 范翔
【版本】 v1.0.0
【日期】 2014年1月

【VS2012运行配置】
1. 调试选择x64
2. 添加头文件包含目录
	属性->VC++目录->包含目录
C:\Qt\include
C:\Qt\include\QtCore
C:\Qt\include\QtNetwork（Qt网络传输Socket编程）
C:\Qt\include\QtGui（Qt界面）
C:\Qt\include\QtSql（Qt数据库驱动及访问类库）

C:\Qt\include;C:\Qt\include\QtCore;C:\Qt\include\QtNetwork;C:\Qt\include\QtGui;C:\Qt\include\QtSql

3. 添加静态链接库lib目录
	属性->VC++目录->库目录
C:\Qt\lib

4. 添加附加依赖项，静态链接库lib文件
【Debug版】
qtmaind.lib
QtCored4.lib
QtGuid4.lib
QtNetworkd4.lib
QtSqld4.lib
【Release版】
qtmain.lib
QtCore4.lib
QtGui4.lib
QtNetwork4.lib
QtSql4.lib

5. 编译链接后在工程Debug或Release目录（含可执行.exe文件）下添加

qsqlmysql4.dl
qsqlmysqld4.dll
libmysql.dll
codecs文件夹
iconengines文件夹
sqldrivers文件夹中的文件
注：
(1)qsqlmysql4.dll和libmysql.dll（MySQL数据库访问驱动，Qt安装不自带，需要自己编译）
编译注意事项，需要添加mysql的lib库，
64位的mysql库对应64位的QT编译出64位的QMYSQL驱动，
32位的mysql库对应32位的QT编译出32位的QMYSQL驱动。
详见服务器目录
\\10.13.29.236\Temp Directory\QT\QT编译成64位程序
\\10.13.29.236\Temp Directory\QT\QT连接mysql驱动
(2)codecs在Qt安装目录下的一个插件，负责中英文的转换
在QT目录中：C:\Qt\plugins\codecs
iconengines在Qt安装目录下的一个插件，与界面中图标icon相关
在QT目录中：C:\Qt\plugins\iconengines
sqldrivers在Qt安装目录下的一个插件，与Qt数据库服务相关
在QT目录中：C:\Qt\plugins\sqldrivers