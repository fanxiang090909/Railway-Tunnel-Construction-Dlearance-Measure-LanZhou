#ifndef DAOTASKTUNNEL_H
#define DAOTASKTUNNEL_H

#include <QObject>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlRelationalTableModel>
#include <QStringListModel>

enum
{
    TaskTunnel_ID = 0,
    TaskTunnel_Task_ID = 1,
    TaskTunnel_Tunnel_ID = 2,
    TaskTunnel_Is_Normal = 3,
    TaskTunnel_State = 4
};

/**
 * 任务条目-对应隧道数据访问对象类声明
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2013-8-11
 */
class TaskTunnelDAO : public QObject
{
    Q_OBJECT
private:
    // singleton设计模式，静态私有实例变量
    static TaskTunnelDAO * taskTunnelDAOInstance;

    //QSqlTableModel * taskTunnelsModel;

    /* 一次采集任务的所有隧道采集数据 */
    QStringListModel * taskTunnelListModel;
    QSqlQueryModel * taskTunnelsModel;
    // 包含隧道长度的QSqlQueryModel
    QSqlQueryModel * taskTunnelsLengthModel;

    /**
     * 一条线路上最近一次采集的隧道数据
     */
    QSqlQueryModel * taskTunnelsOneLineRecently;

    /**
     * 一个隧道的历史所有采集数据
     */
    QSqlQueryModel * taskTunnels_OneTunnelModel;

public:
    /**
     * singleton设计模式，公有静态方法返回单一实例
     * @return 单一实例
     */
    static TaskTunnelDAO * getTaskTunnelDAOInstance();

    ~TaskTunnelDAO();

    /**
     * 得到task_tunnel_id，如果有，返回查找后得到task_tunnel_id，如果没有，创建新的task_tunnel_id
     * @param tunnelid 隧道ID
     * @param collectdate 采集时间
     */
    _int64 getTaskTunnelID(int tunnelid, QString collectdate);

    /**
     * 某一次采集任务中的所有隧道任务模型，排序
     * @param taskid  任务ID
     * @return QSqlQueryModel * 类型
     */
    QSqlQueryModel * getTaskTunnels(_int64 taskid);

    /**
     * 某一次采集任务中的所有隧道任务模型，排序
     * 【注！】与上一方法返回格式不同
     * 与getTaskTunnelsOneLineRecently返回格式完全一样 @see QSqlQueryModel * getTaskTunnelsOneLineRecently(int )
     * @param taskid  任务ID
     * @return QSqlQueryModel * 类型
     */
    QSqlQueryModel * getTaskTunnels_2(_int64 taskid);

    /**
     * 得到一条线路上最近一次采集的隧道数据
     * 与getTaskTunnels_2返回格式完全一样 @see QSqlQueryModel * getTaskTunnels_2(_int64 )
     * @param lineid 线路ID
     */
    QSqlQueryModel * getTaskTunnelsOneLineRecently(int lineid);

    /**
     * 某一个隧道的历史所有隧道任务模型，排序
     * getTaskTunnels方法参数含义不同
     * @param tunnelid  隧道ID
     */
    QSqlQueryModel * getTaskTunnels_OneTunnel(int tunnelid);

    /**
     * 某一次采集任务的所有任务隧道隧道名称，排序
     */
    QStringListModel * getTaskTunnelNames(_int64 taskid);

    /**
     * 得到某个tasktunnelid对应的隧道名称和采集时间
     * @param newtasktunnelid 
     * @param outputtunnelid 输出的隧道ID
     * @param outputtunnelname 输出的隧道名，从数据库中找出
     * @param output 输出的采集时间，从数据库中找出
     */
    bool getTaskTunnelInfo(_int64 newtasktunnelid, int &outputtunnelid, QString & outputtunnelname, QString & outputdate, QString & outputlinename);

    /**
     * 是否有该taskTunnelid对应的隧道任务数据库记录
     * @return true有，false没有
     */
    bool hasTaskTunnelID(_int64 taskTunnelid);

    /**
     * 创建隧道任务，没有tasktunnelid参数，因为加入数据库时采取主键自增策略
     * @param taskid
     * @param tunnelid 隧道ID
     * @param interframe_mile 帧间隔里程数，一般0.5m，0.75m，1m，1.25m一帧
     * @param carriageDirection 车厢正反
     * @param isNormal 正常拍摄、非正常拍摄
     * @param state 状态
     * @return 0 创建成功
     *         1 创建不成功 已经存在该隧道任务
     *         2 创建不成功 taskid不存在
     *         3 创建不成功 tunnelid不存在
     */
    int addTaskTunnel(_int64 taskid, int tunnelid, double interframe_mile, bool carriageDirection, bool isNormal, int state);

    /**
     * 更新隧道任务
     * @param taskid
     * @param tunnelid 隧道ID
     * @param interframe_mile 帧间隔里程数，一般0.5m，0.75m，1m，1.25m一帧
     * @param carriageDirection 车厢正反
     * @param isNormal 正常拍摄、非正常拍摄
     * @param state 状态
     * @return 0 更新成功
     *         1 更新不成功，当前taskTunnelid不存在
     *         2 更新不成功，taskid不存在
     *         3 更新不成功，tunnelid不存在
     */
    int updateTaskTunnel(_int64 taskTunnelid, _int64 taskid, int tunnelid, double interframe_mile, bool carriageDirection, bool isNormal, int state);

private:

    // singleton单一设计模式，构造函数私有
    explicit TaskTunnelDAO(QObject *parent = 0);

    // 是否有这条隧道任务，return >0 taskTunnelid，-1没有
    _int64 getTaskTunnel_P(int tunnelid, QString date);

    // 数据库中创建隧道任务，没有taskTunnelid参数，因为加入数据库时采取主键自增策略
    void createTaskTunnel_P(_int64 taskid, int tunnelid, double interframe_mile, bool carriageDirection, bool isNormal, int state);

    // 数据库中修改task_tunnel
    void updateTaskTunnel_P(_int64 taskTunnelid, _int64 taskid, int tunnelid, double interframe_mile, bool carriageDirection, bool isNormal, int state);

    // 数据库中删除task_tunnel
    void deleteTaskTunnel_P(_int64 taskTunnelid);

signals:

public slots:

};

#endif // DAOTASKTUNNEL_H
