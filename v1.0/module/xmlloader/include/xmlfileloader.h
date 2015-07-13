#ifndef XMLFILELOADER_H
#define XMLFILELOADER_H

#include "tinyxml.h"
#include "tinystr.h"
#include <QObject>
#include <QMutex>

/**
 * XML文件解析父类声明
 * @author xiongxue
 * @author fanxiang
 * @date 20131107
 * @version 1.0.0
 */
class XMLFileLoader
{
public:
    explicit XMLFileLoader(QString filename);
    XMLFileLoader();//无参构造函数

    // 父类析构函数必须是虚函数
    virtual ~XMLFileLoader();

    virtual void setFilename(QString filename);
    virtual QString getFilename();

private:
    QString filename;

protected:
    QMutex mutex;
};

#endif // XMLFILELOADER_H
