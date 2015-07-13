#include "setting_slave.h"
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
SlaveSetting* SlaveSetting::settingInstance = NULL;

SlaveSetting* SlaveSetting::getSettingInstance()
{
    if(settingInstance == NULL)
    {
        settingInstance =new SlaveSetting();
    }
    return settingInstance;
}

// 构造函数myslavemodel赋初值
SlaveSetting::SlaveSetting(QObject *parent) :
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
    // NAS的IP地址
    nasaccessip = "";
    hasnasip = false;

    workingState = Preparing;
    // Windows版 rar程序文件路径
    Rar_address = "C:/Program Files/WinRAR/Rar.exe";
}

SlaveSetting::~SlaveSetting()
{
}

/**
 * 得到当前从机的连接相相机数量，常理应该为4
 */
int SlaveSetting::getCurrentSalveCameraNumber()
{
    // 读取解析的配置文件并且根据当前配置情况获取相机数量
    /*********TODO**********/
    return 4;
}

/**
 * 得到当前的从机模型，一个引用值
 * @see SlaveModel 类
 */
SlaveModel & SlaveSetting::getCurrentSlaveModel()
{
    return myslavemodel;
}

/************从控机存储顶级目录**************/
QString SlaveSetting::getParentPath() { return parentpath; }
void SlaveSetting::setParentPath(QString newpath) { parentpath = newpath; }

/************本从控机IP地址获取**************/
QString SlaveSetting::getMySlaveIP() { return myslaveip; }
void SlaveSetting::setMySlaveIP(QString newip) { myslaveip = newip; }

/*******从控机模型标志的重置set和获取get方法********/
void SlaveSetting::setHasModel(bool has) { hasSlaveModel = has;	}
bool SlaveSetting::hasModel() {	return hasSlaveModel; }

/********主控IP地址，默认为""（空字符串）**********/
QString SlaveSetting::getMasterIP() { return masterip; }
void SlaveSetting::setMasterIP(QString ip) { masterip = ip; hasmip = true; }
void SlaveSetting::setHasMasterIP(bool has) { hasmip = has; }
bool SlaveSetting::hasMasterIP() { return hasmip; }
void SlaveSetting::setNASAccessIP(QString newip) { nasaccessip = newip; hasnasip = true; }
bool SlaveSetting::getHasNasAccessIP() { return hasnasip; }
QString SlaveSetting::getNASAccessIP() { return nasaccessip; }

/**
 * 创建新的工程在此从机的存储路径
 * 【参考】设计-每个工程目录结构-从控.txt
 */
bool SlaveSetting::createSlaveProjectDir(QString projectfullfilepath)
{
    // 创建的新的工程目录 【参考】设计-每个工程目录结构.txt
    QDir mydir;
    QString newpath = projectfullfilepath;
    QString newdir = newpath;
    if (mydir.exists(newpath))
    {
        //LocalFileOperation::removeDirwithContent(projectfullfilepath);
    }
    mydir.mkpath(newpath);
    newpath = newdir + "/collect";
    mydir.mkpath(newpath);
    newpath = newdir + "/calcu_calibration";
    mydir.mkpath(newpath);
    newpath = newdir + "/mid_calcu";
    mydir.mkpath(newpath);
    newpath = newdir + "/fuse_calcu";
    mydir.mkpath(newpath);
    newpath = newdir + "/syn_data";
    mydir.mkpath(newpath);
    newpath = newdir + "/tmp_img";
    mydir.mkpath(newpath);
    return true;
    
}

QString SlaveSetting::getRarAddressname() {return Rar_address; }
