#ifndef CAMERASNID_H
#define CAMERASNID_H

#include <QObject>
#include "slavemodel.h"

/**
 * 相机sn与id对应声明
 * @author xiongxue
 * @date 2013-11-13
 * @version 1.0.0
 */
class NetworkConfigList : public QObject
{
    Q_OBJECT
public:
    /**
     * singleton单例设计模式，公有静态方法返回单一实例
     * @return 单一实例
     */
    static NetworkConfigList* getNetworkConfigListInstance();

    ~NetworkConfigList();

    /**
     * 返回list
     */
    const QList<SlaveModel>* listsnid();
    
    /**
     * 新元素插入末尾
     * @param slavesnidModel 新元素
     */
    bool pushBackToSlaveListsnid(SlaveModel slavesnidModel);

    /**
     * 根据IP地址获得从机index号
     * @return slaveid号
     */
    int findSlaveId(QString ipaddress);

    /**
     * 查找相机所在从机的IP地址
     * @return ip地址
     */
    QString findIPAddressHasCamera(QString cameraindex);

    /**
     * 根据IP地址查找从机并返回
     * @param ip 输入ip地址
     * @param slavemodel output从机模型
     * @return true 找到
     */
    bool findSlave(QString ip, SlaveModel & slavemodel);

    /**
     * 根据从机号获得从机模型
     * @param slaveindex 输入从机模型索引号
     * @param outputslavemodel 输出找到的从机模型
     * @return true list中找到了该从机slaveindex
     */
    bool findSlaveModelBySlaveIndex(int slaveindex, SlaveModel & outputslavemodel);

    /**
     * 设置标定文件
     * @param 标定文件名
     */
    void setCalibraitionFile(string new_calibration_file);

    /**
     * 得到标定文件名
     */
    string getCalibrationFile();

	//@zengwang 2015年10月13日
	//根据相机号寻找对应的Nas的IP地址
	QString findNASIPByCamID(QString tmpcamid);


    void setMasterIP(QString newip)
    {
        this->master_ip = newip;
    }

    QString getMasterIP()
    {
        return master_ip;
    }

    void setMasterBackupNasIP(QString newip)
    {
        this->master_backup_nasip = newip;
    }

    QString getMasterBackupNasIP()
    {
        return master_backup_nasip;
    }

    void setDBServerIP(QString newip)
    {
        this->db_server_ip = newip;
    }

    QString getDBServerIP()
    {
        return db_server_ip;
    }

    void clear();

    void showList();
private:
    /**
     * 构造函数私有，单一设计模式
     */
    explicit NetworkConfigList(QObject *parent = 0);
    static NetworkConfigList * networkconfiglistInstance;

    QList<SlaveModel> * slaveList;
    /**
     * 标定文件名
     */
    string calibration_file;

    QString master_ip;
    QString master_backup_nasip;
    QString db_server_ip;

};

#endif // CAMERASNID_H
