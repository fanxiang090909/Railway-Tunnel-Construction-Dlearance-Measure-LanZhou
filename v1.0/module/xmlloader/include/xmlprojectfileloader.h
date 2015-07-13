#ifndef XMLPROJECTFILELOADER_H
#define XMLPROJECTFILELOADER_H

#include "xmlfileloader.h"
#include "projectmodel.h"

/**
 * 工程入口配置的XML文件解析类声明
 * @see ProjectModel
 * @author fanxiang
 * @date 20140327
 * @version 1.0.0
 */
class XMLProjectFileLoader
{
public:
    XMLProjectFileLoader(QString filename);

    ~ XMLProjectFileLoader();

    /**
     * 加载文件，从文件中读取信息
     * @param ProjectModel 代表工程目录，其中包含工程相关信息（@see ProjectModel）
     */
    bool loadFile(ProjectModel & projectmodel);

    /**
     * 存储文件，从projectModel中读取信息存入文件
     * @param ProjectModel 代表工程目录，其中包含工程相关信息（@see ProjectModel）
     */
    bool saveFile(ProjectModel & projectmodel);

    void setFilename(QString filename);

    QString getFilename();

protected:
    QMutex mutex;

private:
    QString filename;
};

#endif // XMLPROJECTFILELOADER_H
