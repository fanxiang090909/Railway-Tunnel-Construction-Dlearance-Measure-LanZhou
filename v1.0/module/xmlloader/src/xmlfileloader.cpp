#include "xmlfileloader.h"

/**
 * XML文件解析父类定义
 * @author xiongxue
 * @author fanxiang
 * @date 20131107
 * @version 1.0.0
 */
XMLFileLoader::XMLFileLoader(QString filename) { setFilename(filename);}

XMLFileLoader::XMLFileLoader(){}

XMLFileLoader::~XMLFileLoader(){}

void XMLFileLoader::setFilename(QString filename)
{
    this->filename = filename;
    // QString转cstring
    // 方案一：
    // QString str = "xxx";
    // QByteArray ba = str.toAscii();
    // const char *cstr = strcpy(ba.constData());
    // 方案二:
    // QString str = "xxx";
    // QByteArray ba = str.toAscii();
    // someCallThatNeedsACStr(ba.constData());
    // 参见http://www.qtcentre.org/archive/index.php/t-33924.html
    // 【注意！】QString转char不能用QString().toStdString().c_str()，否则可能会出问题
}

QString XMLFileLoader::getFilename()
{
    return filename;
}
