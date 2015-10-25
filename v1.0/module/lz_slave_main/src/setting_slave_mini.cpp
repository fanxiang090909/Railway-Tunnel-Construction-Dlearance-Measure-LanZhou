#include "setting_slave_mini.h"
#include <QDebug>
#include <QDir>
#include <QFileInfoList>
#include <QString>
#include <QHostinfo>

#include "xmlprojectfileloader.h"
#include "fileoperation.h"

/**
 * 从控机参数配置程序定义
 * 计划隧道文件和相机参数配置文件解析
 * @author xiongxue
 * @author fanxiang
 * @date 2013-12-30
 */
//singleton设计模式，静态私有实例变量
SlaveMiniSetting* SlaveMiniSetting::settingInstance = NULL;

SlaveMiniSetting* SlaveMiniSetting::getSettingInstance()
{
    if(settingInstance == NULL)
    {
        settingInstance =new SlaveMiniSetting();
    }
    return settingInstance;
}

// 构造函数myslavemodel赋初值
SlaveMiniSetting::SlaveMiniSetting(QObject *parent) :
    QObject(parent), myslavemodel(0)
{
    parentpath = ".";

    // 获取从机IP地址
    // 获取主机名。
    QString localHostName = QHostInfo::localHostName();
    // 获取本机的IP地址。
    QHostInfo info = QHostInfo::fromName(localHostName);
    for (int i = 0; i < info.addresses().size(); i++)
    {
        if ((info.addresses().value(i) != QHostAddress::LocalHost) && (info.addresses().value(i).protocol() == QAbstractSocket::IPv4Protocol))
        {
            myslaveip = info.addresses().value(i).toString();
            qDebug() << localHostName << " IP Address:" << myslaveip;
        }
    }
    qDebug() << localHostName << "local network IP Address:" << myslaveip;
    // 初始没有slavemodel
    hasSlaveModel = false;
    /**************/
    // 主控机地址默认为空字符串
    masterip = "";
    hasmip = false;

    // Windows版 rar程序文件路径
    Rar_address = "C:/Program Files/WinRAR/Rar.exe";

    slaveEXEPath = "C:\\Users\\Administrator\\Desktop\\SLAVE_RELEASE_DLL\\lz_slave.exe";

    slaveStartupBATPath = "C:\\Users\\Administrator\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\\startslave.bat";
}

SlaveMiniSetting::~SlaveMiniSetting()
{
}

/**
 * 得到当前从机的连接相相机数量，常理应该为4
 */
int SlaveMiniSetting::getCurrentSalveCameraNumber()
{
    // 读取解析的配置文件并且根据当前配置情况获取相机数量
    /*********TODO**********/
    return 4;
}

/**
 * 得到当前的从机模型，一个引用值
 * @see SlaveModel 类
 */
SlaveModel & SlaveMiniSetting::getCurrentSlaveModel()
{
    return myslavemodel;
}

/************从控机存储顶级目录**************/
QString SlaveMiniSetting::getParentPath() { return parentpath; }
void SlaveMiniSetting::setParentPath(QString newpath) { parentpath = newpath; }

/************本从控机IP地址获取**************/
QString SlaveMiniSetting::getMySlaveIP() { return myslaveip; }
void SlaveMiniSetting::setMySlaveIP(QString newip) { myslaveip = newip; }

/*******从控机模型标志的重置set和获取get方法********/
void SlaveMiniSetting::setHasModel(bool has) { hasSlaveModel = has;	}
bool SlaveMiniSetting::hasModel() {	return hasSlaveModel; }

/********主控IP地址，默认为""（空字符串）**********/
QString SlaveMiniSetting::getMasterIP() { return masterip; }
void SlaveMiniSetting::setMasterIP(QString ip) { masterip = ip; hasmip = true; }
void SlaveMiniSetting::setHasMasterIP(bool has) { hasmip = has; }
bool SlaveMiniSetting::hasMasterIP() { return hasmip; }

QString SlaveMiniSetting::getRarAddressname() { return Rar_address; }

QString SlaveMiniSetting::getSlaveEXEPath() { return this->slaveEXEPath; }
void SlaveMiniSetting::setSlaveEXEPath(QString newpath) { this->slaveEXEPath = newpath; }

QString SlaveMiniSetting::getSlaveStartupBATPath() { return this->slaveStartupBATPath; }
void SlaveMiniSetting::setSlaveStartupBATPath(QString newpath) { this->slaveStartupBATPath = newpath; }