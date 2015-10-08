#include "setting_server.h"
#include <QDebug>
#include <QDir>

#include <string>
#include <QString>
#include <QFileInfo>
#include "xmlprojectfileloader.h"

/**
 * 办公室服务器程序参数配置设置类实现
 * @author fanxiang
 * @version 1.0.0
 * @date 2014-2-11
 */
// singleton设计模式，静态私有实例变量
ServerSetting * ServerSetting::settingInstance = NULL;

/**
 * singleton设计模式，公有静态方法返回单一实例
 * @return 单一实例
 */
ServerSetting* ServerSetting::getSettingInstance()
{
    if (settingInstance == NULL)
    {
        settingInstance = new ServerSetting();
    }
    return settingInstance;
}

// singleton单一设计模式，构造函数私有
ServerSetting::ServerSetting(QObject *parent) :
    QObject(parent), currentProjectModel(1.0)
{
    initprojectok = false;
    parentpath = ".";
    projectpath = ".";

    lineNamesModel = new QStringListModel(this);
}

ServerSetting::~ServerSetting()
{
    if (lineNamesModel != NULL)
        delete lineNamesModel;
}

/**
 * 设置顶级存放路径
 */
void ServerSetting::setParentPath(QString newpath) { parentpath = newpath; }
QString ServerSetting::getParentPath() { return parentpath; }

void ServerSetting::setNASIPAddress(QString newnasaddress) { nasip = newnasaddress; }
QString ServerSetting::getNASAddress() { return nasip; }

/**
 * 设置工程存放路径
 */
void ServerSetting::setProjectPath(QString newpath) { projectpath = newpath; }
QString ServerSetting::getProjectPath() { return projectpath; }

/**
 * 采集线路工程文件的配置加载
 */
bool ServerSetting::setProjectName(QString newfilename)
{
    if (!newfilename.endsWith(".proj"))
    {
        initprojectok = false;
        return false;
    }
    XMLProjectFileLoader * profileLoder = new XMLProjectFileLoader(newfilename);
    initprojectok = profileLoder->loadFile(currentProjectModel);
    qDebug() << "project loader" << initprojectok;
    delete profileLoder;

    if (initprojectok == false)
    {
        projectfilename = "";
        return false;
    }

    // 保存文件名
    projectfilename = QFileInfo(newfilename).fileName();
    projectpath = QFileInfo(newfilename).path();

    qDebug() << currentProjectModel.getPlanFilename() << currentProjectModel.getRealCollectDate() << currentProjectModel.getCheckedFilename();
    return true;
}

QString ServerSetting::getProjectFilename()
{
    return projectfilename;
}

ProjectModel & ServerSetting::getProjectModel()
{
    return currentProjectModel;
}

/**
 * 得到所有可编辑线路名称
 */
QStringListModel * ServerSetting::getEditableLineNames()
{
    QDir dir;
    dir.setPath(ServerSetting::getSettingInstance()->getNASAddress());
    dir.setFilter(QDir::Dirs | QDir::Hidden);
    dir.setSorting(QDir::DirsFirst);
    QStringList names = dir.entryList();

    // 除去该目录下非工程目录名，如“..”“plan_tasks”“tmp_img”“system”“output”等等，以便显示给用户
    QStringList resultlist;
    for (int i = 0; i < names.length(); i++)
    {
        QStringList tmp(names.at(i).split("_"));
        if (tmp.length() >= 2 && tmp.at(0).length() > 0 && tmp.at(tmp.length() - 1).length() == 8)
            resultlist.append(names.at(i));
    }

    lineNamesModel->setStringList(resultlist);
    return lineNamesModel;
}