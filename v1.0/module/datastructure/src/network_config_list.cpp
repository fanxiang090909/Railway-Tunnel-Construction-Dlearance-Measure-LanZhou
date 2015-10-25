#include "network_config_list.h"
#include <QDebug>

//singleton设计模式，静态私有实例变量
NetworkConfigList * NetworkConfigList::networkconfiglistInstance = NULL;

NetworkConfigList* NetworkConfigList::getNetworkConfigListInstance()
{
    if (networkconfiglistInstance == NULL)
    {
        networkconfiglistInstance = new NetworkConfigList();
    }
    return networkconfiglistInstance;
}

NetworkConfigList::NetworkConfigList(QObject *parent) :
    QObject(parent)
{
    slaveList = new QList<SlaveModel>();
}

NetworkConfigList::~NetworkConfigList()
{
    delete slaveList;
}

const QList<SlaveModel>* NetworkConfigList::listsnid()
{
    return slaveList;
}

bool NetworkConfigList::pushBackToSlaveListsnid(SlaveModel slavesnidModel)
{
    slaveList->push_back(slavesnidModel);
    return true;
}

// 根据IP地址获得从机index号
int NetworkConfigList::findSlaveId(QString ipaddress)
{
    if (slaveList->length() == 0)
    {
        qDebug() << tr("网络配置文件未加载");
        return -1;
    }

    for (int i = 0; i < slaveList->length(); i++)
        if (slaveList->at(i).getHostAddress().compare(ipaddress.toStdString()) == 0)
            return slaveList->at(i).getIndex();
    return -1;
}

QString NetworkConfigList::findIPAddressHasCamera(QString cameraindex)
{
    if (slaveList->length() == 0)
    {
        qDebug() << tr("网络配置文件未加载");
        return "";
    }

    for (int i = 0; i < slaveList->length(); i++)
    if (slaveList->at(i).box1.camera_ref.compare(cameraindex.toStdString()) == 0
            || slaveList->at(i).box1.camera.compare(cameraindex.toStdString()) == 0
            || slaveList->at(i).box2.camera_ref.compare(cameraindex.toStdString()) == 0
            || slaveList->at(i).box2.camera.compare(cameraindex.toStdString()) == 0)
    {
        //qDebug() << "find!" << cameraindex;
        return QString(slaveList->at(i).getHostAddress().c_str());
    }
    return "";
}

/**
 * 根据IP地址查找从机并返回
 * @param ip 输入ip地址
 * @param slavemodel output从机模型
 * @return true 找到
 */
bool NetworkConfigList::findSlave(QString ip, SlaveModel & slaveModel)
{
    qDebug() << ip;
    // 【注意！】QString转cchar不能用QString().toStdString().c_str()，否则可能会出问题
    QByteArray tempstr = ip.toAscii();
    for (int i = 0; i < slaveList->length(); i++)
    {
        // 若相等
        SlaveModel ret = slaveList->at(i);
        if (strcmp(ret.getHostAddress().c_str(), tempstr.constData()) == 0)
        {
            slaveModel = ret;
            return true;
        }
        // 若不相等继续查找
    }
    return false;
}

/**
 * 根据从机号获得从机模型
 * @param slaveindex 输入从机模型索引号
 * @param outputslavemodel 输出找到的从机模型
 * @return true list中找到了该从机slaveindex
 */
bool NetworkConfigList::findSlaveModelBySlaveIndex(int slaveindex, SlaveModel & outputslavemodel)
{
    for (int i = 0; i < slaveList->length(); i++)
    if (slaveList->at(i).getIndex() == slaveindex)
    {
        outputslavemodel = slaveList->at(i);
        return true;
    }
    return false;
}

void NetworkConfigList::setCalibraitionFile(string new_calibration_file)
{
    this->calibration_file = new_calibration_file;
}

string NetworkConfigList::getCalibrationFile()
{
    return calibration_file;
}

void NetworkConfigList::clear()
{
    this->slaveList->clear();
}

void NetworkConfigList::showList()
{
    qDebug() << "***********NetworkConfigList*******size::" << slaveList->size() << "*******";
    for (int i = 0; i < slaveList->length(); i++)
        qDebug() << slaveList->at(i).getHostAddress().c_str();
}


//@zengwang 2015年10月13日
QString NetworkConfigList::findNASIPByCamID(QString tmpcamid)
{
	 if (slaveList->length() == 0)
    {
        qDebug() << tr("网络配置文件未加载");
        return "";
    }

    for (int i = 0; i < slaveList->length(); i++)
		if (QString(slaveList->at(i).box1.boxindex).compare(tmpcamid) == 0
			|| QString(slaveList->at(i).box2.boxindex).compare(tmpcamid) == 0)
		 {
				//qDebug() << "find!" << cameraindex;
				return QString(slaveList->at(i).getBackupNasIP().c_str());
		 }
    return "";
}