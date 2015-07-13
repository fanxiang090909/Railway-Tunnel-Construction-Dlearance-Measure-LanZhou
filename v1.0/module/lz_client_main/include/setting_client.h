#ifndef SETTING_CLIENT_H
#define SETTING_CLIENT_H

#include <QObject>
#include <QStringListModel>
#include "tunneldatamodel.h"
#include "projectmodel.h"
#include "clearance_tunnel.h"
#include "clearance_tunnels.h"

/**
 * 选择单隧道输出图表还是多隧道综合输出
 */
enum SingleOrMultiSelectionMode
{
    Single_Mode = 0,    // 单隧道选择综合结果
    Multi_Mode = 1      // 多隧道选择，综合出结果
};

/**
 * 办公室操作终端的程序参数设置类声明
 * @author fanxiang
 * @version 1.0.0
 * @date 2013-11-17
 */
class ClientSetting : public QObject
{
    Q_OBJECT
public:

    /**
     * singleton单例设计模式，公有静态方法返回单一实例
     * @return 单一实例
     */
    static ClientSetting* getSettingInstance();

    ~ClientSetting();

    /**
     * 办公室服务器IP地址
     */
    void setServerIP(QString newip);
    QString getServerIP();
    /**
     * 办公室数据库服务器IP地址
     */
    void setDBServerIP(QString newip);
    QString getDBServerIP();
    /**
     * 设置NAS访问IP地址
     */
    void setNASAccessIP(QString newip);
    /**
     * 获得NAS访问IP地址
     */
    QString getNASAccessIP();

    /**
     * 设置NAS数据存放路径
     */
    void setParentPath(QString newpath);
    QString getParentPath();

    /**
     * 设置客户端本地临时数据存储路径
     */
    void setClientTmpLocalParentPath(QString newpath);
    QString getClientTmpLocalParentPath();

    /**
     * 设置当前正在编辑隧道名
     */
    void setCurrentEditingTunnel(QString newTunnelName);
    /**
     * @return 当前正在编辑隧道名
     */
    QString getCurrentEditingTunnel();
    /**
     * 设置当前正在编辑工程名
     */
    void setCurrentEditingProject(QString newPrjectName);
    /**
     * @return 当前正在编辑工程名
     */
    QString getCurrentEditingProject();
    /**
     * 设置当前操作用户名
     */
    void setCurrentUser(QString newUser);
    /**
     * @return 当前操作用户名
     */
    QString getCurrentUser();
    /**
     * @return true 当前修正隧道可以被修正
     */
    bool getCanEditing();
    /**
     * 设置当前操作隧道是否可以被修正
     * @param projectname 正在编辑工程名
     * @param tunnelname 正在编辑隧道名
     * @param newCanEditing 输入是否可以被修正
     * @return true 设置成功,false不能设置成功,原因projectname、tunnelname与currentProject、currentTunnel不符
     */
    bool setCanEditing(QString projectname, QString tunnelname, bool newCanEditing);

    /**
     * @return 输出Excel模板文件路径
     */
    QString getOutExcelTemplatePath();

    bool setOutputTunnelDataModel(int tunnelid);

    bool setCorrectTunnelDataModel(int tunnelid);

    /**
     * 返回输出时用的TunnelDataModel
     * @param ret 返回值引用，是否已加载成功该模型
     * @return 返回模型指针
     */
    TunnelDataModel * getOutputTunnelDataModel(bool & ret);

    /**
     * 返回修正时用的TunnelDataModel
     * @param ret 返回值引用，是否已加载成功该模型
     * @return 返回模型指针
     */
    TunnelDataModel * getCorrectTunnelDataModel(bool & ret);

    /**
     * 单隧道限界结果模型
     */
    ClearanceSingleTunnel & getSingleTunnelModel();

    /**
     * 多隧道综合限界结果模型
     */
    ClearanceMultiTunnels & getMultiTunnelsModel();

    /**
     * 设置输出模式单隧道输出图表还是多隧道
     */
    bool setSingleMultiMode(SingleOrMultiSelectionMode newMode);
    /**
     * 得到输出模式单隧道输出图表还是多隧道
     */
    SingleOrMultiSelectionMode getSingleMultiMode();
    /**
     * 包含目录名的List模型，用于在View中显示
     */
    QStringListModel * getEditableLineNames();
    
private:
    explicit ClientSetting(QObject *parent = 0);

    // singleton设计模式，静态私有实例变量
    static ClientSetting * settingClientInstance;

    /**
     * 输出时用的TunnelDataModel
     */
    TunnelDataModel * outputTunnelDataModel;
    bool hasinitoutputtunneldatamodel;
    /**
     * 修正时用的TunnelDataModel
     */
    TunnelDataModel * correctTunnelDataModel;
    bool hasinitcorrecttunneldatamodel;

    /**
     * 单隧道限界结果模型
     */
    ClearanceSingleTunnel singleTunnelModel;

    /**
     * 多隧道区段综合限界结果模型
     */
    ClearanceMultiTunnels multiTunnelsModel;

    /**
     * 当前是做区段综合还是单个隧道综合，默认应该是单个隧道综合
     */
    SingleOrMultiSelectionMode singleMode;

    /**
     * 包含目录名的List模型，用于在View中显示
     */
    QStringListModel * lineNamesModel;

    /**
     * 办公室服务器ip地址
     */
    QString serverAddress;
    /**
     * 数据库服务器ip地址
     */
    QString dbserverAddress;

    /**
     * NAS存储顶级目录路径
     */
    QString parentpath;
    
    /**
     * 客户端缓存顶级目录路径
     */
    QString clientTmpLocalParentPath;

    /**
     * 当前修正工程
     */
    QString currentEditingProject;
    /**
     * 当前修正隧道
     */
    QString currentEditingTunnel;
    /**
     * 当前操作用户
     */
    QString currentUser;
    /**
     * 当前修正隧道是否可以被修正
     */
    bool canEditing;

    /**
     * 输出excel用的excel模板路径
     */
    QString templatexlspath;

    /**
     * NAS访问IP地址
     */
    QString nasaccessip;

};

#endif // SETTING_CLIENT_H
