#ifndef SERVERSETTING_H
#define SERVERSETTING_H

#include <QObject>
#include "projectmodel.h"

/**
 * 服务器办公室程序参数配置设置类声明
 * @author fanxiang
 * @version 1.0.0
 * @date 2014-2-11
 */
class ServerSetting : public QObject
{
    Q_OBJECT
public:

    /**
     * singleton单例设计模式，公有静态方法返回单一实例
     * @return 单一实例
     */
    static ServerSetting* getSettingInstance();//公有静态方法返回单一实例

    ~ServerSetting();

    /**
     * 设置顶级存放路径
     */
    void setParentPath(QString newpath);
    /**
     * @return 顶级存放路径
     */
    QString getParentPath();

    /**
     * 设置工程存放路径
     */
    void setProjectPath(QString newpath);
    /**
     * @return 当前采集工程目录路径
     */
    QString getProjectPath();

    /**
     * 采集线路工程文件的配置加载
     */
    bool setProjectName(QString filename);
    /**
     * @return 当前采集工程文件名
     */
    QString getProjectFilename();
    /**
     * @return 当前采集工程模型
     */
    ProjectModel & getProjectModel();


private:
    explicit ServerSetting(QObject *parent = 0);

    // singleton设计模式，静态私有实例变量
    static ServerSetting * settingInstance;

    /**
     * 当前采集工程模型是否初始化
     */
    bool initprojectok;
    /**
     * 当前采集工程模型
     */
    ProjectModel currentProjectModel;
    /**
     * 工程入口文件路径变量
     */
    QString projectpath;
    /**
     * 工程入口文件名变量
     */
    QString projectfilename;

    /**
     * 顶级存储目录变量
     */
    QString parentpath;

};

#endif // SERVERSETTING_H
