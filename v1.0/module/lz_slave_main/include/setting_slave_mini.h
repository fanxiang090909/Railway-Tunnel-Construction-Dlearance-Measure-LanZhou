#ifndef SETTING_SLAVE_MINI_H
#define SETTING_SLAVE_MINI_H

#include <QObject>
#include "slavemodel.h"
#include "projectmodel.h"
#include <QDir>
#include <string>
using namespace std;

/**
 * 从控机参数配置程序声明
 * 计划隧道文件和相机参数配置文件解析
 * @author fanxiang
 * @date 2015-10-03
 */
class SlaveMiniSetting : public QObject
{
    Q_OBJECT
public:

    /**
     * singleton单例设计模式，公有静态方法返回单一实例
	 * @return 单一实例
     */
    static SlaveMiniSetting* getSettingInstance();
    ~SlaveMiniSetting();

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
    
    // WinRAR exe 地址
    QString getRarAddressname();

    // slave.exe程序的exepath
    QString getSlaveEXEPath();
    void setSlaveEXEPath(QString newpath);

    // slave从机开机自启动bat程序
    QString getSlaveStartupBATPath();
    void setSlaveStartupBATPath(QString newpath);

private:
    explicit SlaveMiniSetting(QObject *parent = 0);
    static SlaveMiniSetting* settingInstance;

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

    //压缩命令Rar.exe存放目录
    QString Rar_address;

    // slave.exe程序的exepath
    QString slaveEXEPath;

    // slave从机开机自启动bat程序
    QString slaveStartupBATPath;
};

#endif // SETTING_SLAVE_MINI_H
