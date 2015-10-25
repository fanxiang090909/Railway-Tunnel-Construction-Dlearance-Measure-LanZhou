#ifndef SETTING_MASTER_H
#define SETTING_MASTER_H

#include <QObject>
#include <QDir>
#include <QFileInfoList>
#include <string>

#include "projectmodel.h"

/**
 * 程序参数配置设置类声明
 * @author 熊雪
 * @author 范翔
 * @version 1.0.0
 * @date 2014-4-15
 */
class MasterSetting : public QObject
{
    Q_OBJECT
public:

    /**
     * singleton单例设计模式，公有静态方法返回单一实例
     * @return 单一实例
     */
    static MasterSetting* getSettingInstance();//公有静态方法返回单一实例

    ~MasterSetting();

    static int getMaxSalveNum();

    /**
     * 设置顶级存放路径
     */
    void setParentPath(QString newpath);
    /**
     * @return 顶级存放路径
     */
    QString getParentPath();

    /********主控底层服务程序参数配置加载***************/
    /**
     * 网络配置加载
     * 相机编号与实际物理ID的配置加载
     * @return true 加载成功
     */
    bool loadNetworkSetting();
    /**
     * 设置网络硬件设备配置文件名
     */
    void setNetworkConfigFilename(QString newfilename);
    /**
     * @return 网络硬件设备配置文件名
     */
    QString getNetworkConfigFilename();

    void setCompressFilename(QString newfilename);
    QString getCompressFilename();
    
    /**
     * 设置NAS访问IP地址
     */
    void setNASAccessIP(QString newip);
    /**
     * 获得NAS访问IP地址
     */
    QString getNASAccessIP();

    /**
     * 设置当前操作用户名
     */
    void setCurrentUser(QString newUser);
    /**
     * @return 当前操作用户名
     */
    QString getCurrentUser();
    
    /**
     * 按里程模式采集下相邻两帧的间隔距离
     */
    double getDefaultDistanceMode();
    void setDefaultDistanceMode(double newDefaultDistanceMode);

    /**
     * 拷贝目录及目录下文件
     * @param fromDir 源目录
     * @param toDir 目标目录
     * @param bCoverifFileExists true 同名时覆盖，flase，同名时返回false，终止拷贝
     */
    bool copyMasterDirectoryFiles(const QDir& fromDir, QStringList & outputfiles);

private:
    explicit MasterSetting(QObject *parent = 0);

    // singleton设计模式，静态私有实例变量
    static MasterSetting * settingInstance;

    /**
     * 静态常量：最大连接数（从控机个数）
     */
    static int SLAVE_MAX;

    /**
     * 网络硬件设备配置文件名
     */
    QString networkConfigFilename;
    /**
     * 压缩RAR.exe程序文件名
     */
    QString compressFilename;

    /**
     * 顶级存储目录变量
     */
    QString parentpath;

    /**
     * 解压图像命令Rar.exe存放路径
     */
    QString Rar_address;
    
    /**
     * NAS访问IP地址
     */
    QString nasaccessip;

    /**
     * 当前操作用户
     */
    QString currentUser;

    /**
     * 按里程模式采集下相邻两帧的间隔距离
     */
    double defaultDistanceMode;
};

#endif // SETTING_MASTER_H
