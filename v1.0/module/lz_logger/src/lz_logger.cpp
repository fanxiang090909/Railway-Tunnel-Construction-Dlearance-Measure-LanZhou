#include "lz_logger.h"
#include <QDateTime>
#include <QByteArray>
#include <QFileInfo>

/**
 * 兰州项目日志类定义，记录各种日志信息
 * @author 范翔
 * @date 2014-04-02
 * @version 1.0.0
 */
LzLogger::LzLogger(LogType type)
{
	this->fileType = type;
	filename = "";
	this->isopen = false;
}

LzLogger::~LzLogger() {}

/**
 * 外部接口，是否正在写入文件，文件正打开
 */
bool LzLogger::isLogging()
{
    return this->isopen;
}

void LzLogger::setFilename(string newfilename)
{
    filename = newfilename;
}

/**
 * @return 0 open已有文件成功
 * 	1 没有已有文件，但创建成功
 *	2 没有已有文件，且创建文件失败
 */
int LzLogger::open()
{
    //if (!QFileInfo(filename.c_str()).exists())
        
	////TODO///
	out.open(filename.c_str(), ios::app);
	isopen = true;
	return 0;
}

void LzLogger::close()
{
	////TODO///
	out.close();
	isopen = false;
	return;
}

/**
 * 日志记录
 * @param msg a string to log
 */
void LzLogger::log(string msg)
{
    QByteArray tmpba = QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss]").toLocal8Bit();
    out << tmpba.constData() << msg << endl;
}