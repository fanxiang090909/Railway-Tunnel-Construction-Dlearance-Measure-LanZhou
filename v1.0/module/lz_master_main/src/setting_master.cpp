#include "setting_master.h"

#include <QDebug>

#include <string>
#include <QString>

#include "plantask_list.h"
#include "realtask_list.h"
#include "xmlrealtaskfileloader.h"
#include "xmlprojectfileloader.h"
#include "xmlnetworkfileloader.h"
#include <QFile>
#include <QProcess>

/**
 * 程序参数配置设置类实现
 * @author xiongxue
 * @author fanxiang
 * @version 1.0.0
 * @date 2013-10-13
 */
// singleton设计模式，静态私有实例变量
MasterSetting * MasterSetting::settingInstance = NULL;

// 静态常量：最大连接数（从控机个数）
int MasterSetting::SLAVE_MAX = 9;

/**
 * singleton设计模式，公有静态方法返回单一实例
 * @return 单一实例
 */
MasterSetting* MasterSetting::getSettingInstance()
{
    if (settingInstance == NULL)
    {
        settingInstance = new MasterSetting();
    }
    return settingInstance;
}

// singleton单一设计模式，构造函数私有
MasterSetting::MasterSetting(QObject *parent) :
    QObject(parent)
{
    Rar_address = "";
    // TODO
    currentUser = "fanxiang";
}

MasterSetting::~MasterSetting()
{
}

int MasterSetting::getMaxSalveNum()
{
    return SLAVE_MAX;
}

/********主控底层服务程序参数配置加载***************/
/**
 * 网络配置加载
 * 相机编号与实际物理ID的配置加载
 */
bool MasterSetting::loadNetworkSetting()
{
    XMLNetworkFileLoader *net = new XMLNetworkFileLoader(networkConfigFilename);
    //net->saveFile();//生成Networkconfig.xml文件
    return net->loadFile();//解析Networkconfig.xml文件并存入list
}

void MasterSetting::setNetworkConfigFilename(QString newfilename)
{
    this->networkConfigFilename = newfilename;
}

QString MasterSetting::getNetworkConfigFilename()
{
    return networkConfigFilename;
}

void MasterSetting::setCompressFilename(QString newfilename)
{
    this->compressFilename = newfilename;
}

QString MasterSetting::getCompressFilename()
{
    return compressFilename;
}

/**
 * 设置顶级存放路径
 */
void MasterSetting::setParentPath(QString newpath) { parentpath = newpath; }
QString MasterSetting::getParentPath() { return parentpath; }

/**
 * 设置NAS访问IP地址
 */
void MasterSetting::setNASAccessIP(QString newip) { nasaccessip = newip; }
/**
 * 获得NAS访问IP地址
 */
QString MasterSetting::getNASAccessIP() { return nasaccessip; }
/**
 * 设置当前操作用户名
 */
void MasterSetting::setCurrentUser(QString newUser) { currentUser = newUser; }
/**
 * @return 当前操作用户名
 */
QString MasterSetting::getCurrentUser() { return currentUser; }

/**
 * 按里程模式采集下相邻两帧的间隔距离
 */
double MasterSetting::getDefaultDistanceMode() { return defaultDistanceMode; }

void MasterSetting::setDefaultDistanceMode(double newDefaultDistanceMode) { this->defaultDistanceMode = newDefaultDistanceMode; }