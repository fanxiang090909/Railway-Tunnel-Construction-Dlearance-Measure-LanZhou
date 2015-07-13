#ifndef SETTING_SLAVE_H
#define SETTING_SLAVE_H

#include <QObject>
#include "slavemodel.h"
#include "projectmodel.h"
#include <QDir>
#include <string>
using namespace std;

/**
 * 从控机参数配置程序声明
 * 计划隧道文件和相机参数配置文件解析
 * @author xiongxue
 * @author fanxiang
 * @date 2013-12-30
 */
class SlaveSetting : public QObject
{
    Q_OBJECT
public:
    enum WorkingState 
    {
        Preparing = 0,
        Collecting = 1,
        Calculating = 2,
        Backuping = 3
    };

    /**
     * singleton单例设计模式，公有静态方法返回单一实例
	 * @return 单一实例
     */
    static SlaveSetting* getSettingInstance();
    ~SlaveSetting();

    /**
     * 得到当前从机的连接相相机数量，常理应该为4
     */
    int getCurrentSalveCameraNumber();
    /**
     * 得到当前的从机模型，一个引用值
     * @see SlaveModel 类
     */
    SlaveModel & getCurrentSlaveModel();

    /************从控机存储顶级目录**************/
    QString getParentPath();
    void setParentPath(QString newpath);

    /************本从控机IP地址获取**************/
    QString getMySlaveIP();
	void setMySlaveIP(QString newip);

    /*******从控机模型标志的重置set和获取get方法********/
    void setHasModel(bool has);
    bool hasModel();

    /********主控IP地址，默认为""（空字符串）**********/
    QString getMasterIP();
    void setMasterIP(QString ip);
    void setHasMasterIP(bool has);
    bool hasMasterIP();
    /**
     * 设置NAS访问IP地址
     */
    void setNASAccessIP(QString newip);
    bool getHasNasAccessIP();
    /**
     * 获得NAS访问IP地址
     */
    QString getNASAccessIP();

    /**
     * 创建新的工程在此从机的存储路径
     * 【参考】设计-每个工程目录结构-从控.txt
     */
    bool createSlaveProjectDir(QString projectfullfilepath);

    QString getRarAddressname();

private:
    explicit SlaveSetting(QObject *parent = 0);
    static SlaveSetting* settingInstance;

    // 从控存储顶级目录 
    QString parentpath;

    // 获取从机IP地址
    QString myslaveip;
    // 从机相关硬件参数
    SlaveModel myslavemodel;
    bool hasSlaveModel;
    // 存放主控机IP地址
    QString masterip;
    bool hasmip; 
    // NAS访问IP地址
    QString nasaccessip;
    bool hasnasip;

    //压缩命令Rar.exe存放目录
    QString Rar_address;

    WorkingState workingState;
signals:

public slots:

};

#endif // SETTING_SLAVE_H
