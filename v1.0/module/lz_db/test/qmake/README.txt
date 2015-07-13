测试数据库连接

【作者】 范翔
【版本】 v1.0.0
【日期】 2013年11月

【运行注意事项】
QT需要自编译MYSQL的驱动库后在debug文件夹中加入以下dll文件
libmysql.dll
qsqlmysql4.dll
qsqlmysqld4.dll


【备注】
编译注意事项，需要添加mysql的lib库，
64位的mysql库对应64位的QT编译出64位的QMYSQL驱动，
32位的mysql库对应32位的QT编译出32位的QMYSQL驱动。

详见服务器目录
\\10.13.29.236\Temp Directory\QT\QT编译成64位程序
\\10.13.29.236\Temp Directory\QT\QT连接mysql驱动