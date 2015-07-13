#include "slavemodel.h"

/**
 * 从机硬件配置模型类实现
 * 包括从机的IP地址、挂载的四台相机编号及SN号
 * 相机盒子号
 * 注：端口号暂时没有用
 * @author 范翔
 * @version 1.0.0
 * @date 2013-11
 */
SlaveModel::SlaveModel(int initSlaveIndex)
{
    slaveIndex = initSlaveIndex;
}

SlaveModel::SlaveModel(int initSlaveIndex, string initHostAddress, int initPort, bool initIsRT, string initnasip)
{
    slaveIndex = initSlaveIndex;
    hostAddress = initHostAddress;
    port = initPort;
    isRT = initIsRT;
    backup_nasip = initnasip;
}

SlaveModel::~SlaveModel()
{
}

const int SlaveModel::getIndex() const
{
    return slaveIndex;
}

const string SlaveModel::getHostAddress() const
{
    return hostAddress;
}

const int SlaveModel::getPort() const
{
    return port;
}

const bool SlaveModel::getIsRT() const
{
    return isRT;
}

const string SlaveModel::getBackupNasIP() const
{
    return backup_nasip;
}

bool SlaveModel::setBox1(CameraPair newbox1)
{
    this->box1 = newbox1;
    return true;
}

bool SlaveModel::setBox2(CameraPair newbox2)
{
    this->box2 = newbox2;
    return true;
}
