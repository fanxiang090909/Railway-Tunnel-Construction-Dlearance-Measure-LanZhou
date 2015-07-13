#ifndef SLAVEMODEL_H
#define SLAVEMODEL_H

#include <string>
#include <list>

using namespace std;

/**
 * 所挂载相机盒子信息结构
 * 索引为boxindex 从A到R
 */
struct CameraPair
{
    char boxindex;
    string box_camera_calib_file;
    string box_fenzhong_calib_file;
    string camera_ref;  // 参考相机
    string camera_ref_sn;
    string camera_ref_file;
    string camera;      // 非参考相机
    string camera_sn;
    string camera_file;
};

/**
 * 从机硬件配置模型类声明
 * 包括从机的IP地址、挂载的四台相机编号及SN号
 * 相机盒子号
 * 注：端口号暂时没有用
 * @author 范翔
 * @version 1.0.0
 * @date 2013-11
 */
class SlaveModel
{
private:
    int slaveIndex;
    string hostAddress;
    int port;
    bool isRT;
    string backup_nasip;
public:
    CameraPair box1;    // 所挂载相机盒子
    CameraPair box2;

public:
    SlaveModel(int initSlaveIndex);
    SlaveModel(int initSlaveIndex, string initHostAddress, int initPort, bool initrt, string initnasip);

    // 复制构造函数
    SlaveModel(const SlaveModel & real)
    {
        this->slaveIndex = real.getIndex();
        this->port = real.getPort();
        this->hostAddress = real.getHostAddress();
        this->backup_nasip = real.getBackupNasIP();
        this->isRT = real.getIsRT();
        this->box1 = real.box1;
        this->box2 = real.box2;
    }
    // 重载赋值号
    SlaveModel & operator=(const SlaveModel &real)
    {
        if (this == &real)
            return *this;
        this->slaveIndex = real.getIndex();
        this->port = real.getPort();
        this->hostAddress = real.getHostAddress();
        this->backup_nasip = real.getBackupNasIP();
        this->isRT = real.getIsRT();
        this->box1 = real.box1;
        this->box2 = real.box2;
        return *this;
    }
    // 重载==号
    friend bool operator==(const SlaveModel &real1, const SlaveModel &real2)
    {
        if (real1.getIndex() == real2.getIndex() && real1.getHostAddress() == real2.getHostAddress())
            return true;
        else
            return false;
    }

    bool setBox1(CameraPair newbox1);
    bool setBox2(CameraPair newbox2);

    ~SlaveModel();
    const int getIndex() const;
    const string getHostAddress() const;
    const int getPort() const;
    const bool getIsRT() const;
    const string getBackupNasIP() const;
};

#endif // SLAVEMODEL_H
