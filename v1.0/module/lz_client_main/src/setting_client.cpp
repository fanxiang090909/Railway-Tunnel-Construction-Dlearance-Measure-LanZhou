#include "setting_client.h"

#include <QDebug>

#include <string>
#include <QString>
#include <QDir>

#include "plantask_list.h"
#include "xmlprojectfileloader.h"
#include "daoadmin.h"

/**
 * 办公室操作终端的程序参数设置类实现
 * @author fanxiang
 * @version 1.0.0
 * @date 2013-11-17
 */
// singleton设计模式，静态私有实例变量
ClientSetting * ClientSetting::settingClientInstance = NULL;

/**
 * singleton设计模式，公有静态方法返回单一实例
 * @return 单一实例
 */
ClientSetting* ClientSetting::getSettingInstance()
{
    if (settingClientInstance == NULL)
    {
        settingClientInstance = new ClientSetting();
    }
    return settingClientInstance;
}

// singleton单一设计模式，构造函数私有
ClientSetting::ClientSetting(QObject *parent) :
	QObject(parent)
{
    // 办公室服务器ip地址
    serverAddress = "";
    dbserverAddress = "";
    parentpath = "./";
    currentUser = "fanxiang";    ///TODO
    currentEditingProject = "";
    currentEditingTunnel = "";

    canEditing = true;    ////TODO

    //设置输出excel用的excel模板路径
    templatexlspath = getParentPath() + "/template_output/";

    // 默认是单隧道模式输出
    singleMode = Single_Mode;

    outputTunnelDataModel = NULL;
    correctTunnelDataModel = NULL;

    lineNamesModel = new QStringListModel(this);

    // 未初始化TunnelDataModel
    hasinitoutputtunneldatamodel = false;
    hasinitcorrecttunneldatamodel = false;
}

ClientSetting::~ClientSetting()
{
    if (outputTunnelDataModel != NULL)
        delete outputTunnelDataModel;
    if (correctTunnelDataModel != NULL)
        delete correctTunnelDataModel;
    if (lineNamesModel != NULL)
        delete lineNamesModel;
}

void ClientSetting::setServerIP(QString newip) { serverAddress = newip; }
QString ClientSetting::getServerIP() { return serverAddress; }
void ClientSetting::setDBServerIP(QString newip) { dbserverAddress = newip; }
QString ClientSetting::getDBServerIP() { return dbserverAddress; }
void ClientSetting::setNASAccessIP(QString newip) { nasaccessip = newip; }
QString ClientSetting::getNASAccessIP() { return nasaccessip; }

// 图表excel保存输出目录
QString ClientSetting::getOutExcelTemplatePath()
{
    return templatexlspath;
}

bool ClientSetting::setOutputTunnelDataModel(int tunnelid)
{
    if (outputTunnelDataModel != NULL)
    {
        delete outputTunnelDataModel;
    }
    outputTunnelDataModel = new TunnelDataModel(tunnelid);
    hasinitoutputtunneldatamodel = outputTunnelDataModel->loadTunnelData();
    return hasinitoutputtunneldatamodel;
}

bool ClientSetting::setCorrectTunnelDataModel(int tunnelid)
{
    if (correctTunnelDataModel != NULL)
    {
        delete correctTunnelDataModel;
    }
    correctTunnelDataModel = new TunnelDataModel(tunnelid);
    hasinitcorrecttunneldatamodel = correctTunnelDataModel->loadTunnelData();
    return hasinitcorrecttunneldatamodel;
}

/**
 * 返回输出时用的TunnelDataModel
 */
TunnelDataModel * ClientSetting::getOutputTunnelDataModel(bool & ret)
{
    ret = hasinitoutputtunneldatamodel;
    if (outputTunnelDataModel == NULL)
        return NULL;
    else
        return outputTunnelDataModel;
}

/**
 * 返回修正时用的TunnelDataModel
 */
TunnelDataModel * ClientSetting::getCorrectTunnelDataModel(bool & ret)
{
    ret = hasinitcorrecttunneldatamodel;
    if (correctTunnelDataModel == NULL)
        return NULL;
    else
        return correctTunnelDataModel;
}

/**
 * 单隧道限界结果模型
 */
ClearanceSingleTunnel & ClientSetting::getSingleTunnelModel() { return singleTunnelModel; }

/**
 * 多隧道综合限界结果模型
 */
ClearanceMultiTunnels & ClientSetting::getMultiTunnelsModel() { return multiTunnelsModel; }

/**
 * 设置输出模式单隧道输出图表还是多隧道
 */
bool ClientSetting::setSingleMultiMode(SingleOrMultiSelectionMode newMode)
{
    singleMode = newMode;
    return true;
}

/**
 * 得到输出模式单隧道输出图表还是多隧道
 */
SingleOrMultiSelectionMode ClientSetting::getSingleMultiMode()
{
    return singleMode;
}

/**
 * 设置数据存放路径
 */
void ClientSetting::setParentPath(QString newpath) 
{ 
    parentpath = newpath; 
    templatexlspath = newpath + "/template_output/"; 
}

QString ClientSetting::getParentPath() { return parentpath; }

/**
 * 设置客户端本地临时数据存储路径
 */
void ClientSetting::setClientTmpLocalParentPath(QString newpath)
{
    clientTmpLocalParentPath = newpath;
}

QString ClientSetting::getClientTmpLocalParentPath() { return clientTmpLocalParentPath; }

/**
 * 设置当前正在编辑隧道名
 */
void ClientSetting::setCurrentEditingTunnel(QString newTunnelName) { currentEditingTunnel = newTunnelName; }
/**
 * @return 当前正在编辑隧道名
 */
QString ClientSetting::getCurrentEditingTunnel() { return currentEditingTunnel; }
/**
 * 设置当前正在编辑工程名
 */
void ClientSetting::setCurrentEditingProject(QString newPrjectName) { currentEditingProject = newPrjectName; }
/**
 * @return 当前正在编辑工程名
 */
QString ClientSetting::getCurrentEditingProject() { return currentEditingProject; }
/**
 * 设置当前操作用户名
 */
void ClientSetting::setCurrentUser(QString newUser) { currentUser = newUser; }
/**
* @return 当前操作用户名
*/
QString ClientSetting::getCurrentUser() { return currentUser; }
/**
 * @return true 当前修正隧道可以被修正
 */
bool ClientSetting::getCanEditing() { return canEditing; }
/**
 * 设置当前操作隧道是否可以被修正
 * @param projectname 正在编辑工程名
 * @param tunnelname 正在编辑隧道名
 * @param newCanEditing 输入是否可以被修正
 * @return true 设置成功,false不能设置成功,原因projectname、tunnelname与currentProject、currentTunnel不符
 */
bool ClientSetting::setCanEditing(QString projectname, QString tunnelname, bool newCanEditing)
{
    if (newCanEditing && projectname.compare(currentEditingProject) == 0 && tunnelname.compare(currentEditingTunnel) == 0)
    {
        canEditing = newCanEditing;
        return true;
    }
    else 
    {
        canEditing = false;
        return false;
    }
}

/**
 * 得到所有可编辑线路名称
 */
QStringListModel * ClientSetting::getEditableLineNames()
{
    QDir dir;
    dir.setPath(ClientSetting::getSettingInstance()->getParentPath());
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