#include <QDebug>
#include "daotasktunnel.h"
#include "daotask.h"
#include "daotunnel.h"
#include <QSqlRecord>
/**
 * 任务条目-对应隧道数据访问对象类实现
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2013-8-11
 */
// singleton设计模式，静态私有实例变量
TaskTunnelDAO * TaskTunnelDAO::taskTunnelDAOInstance = NULL;

/**
 * singleton设计模式，公有静态方法返回单一实例
 * @return 单一实例
 */
TaskTunnelDAO * TaskTunnelDAO::getTaskTunnelDAOInstance()
{
    if (taskTunnelDAOInstance == NULL)
    {
        taskTunnelDAOInstance = new TaskTunnelDAO();
    }
    return taskTunnelDAOInstance;
}

// singleton单一设计模式，构造函数私有
TaskTunnelDAO::TaskTunnelDAO(QObject *parent) :
    QObject(parent)
{
    /* 一次采集任务的所有隧道采集数据 */
    taskTunnelListModel = new QStringListModel(this);
    taskTunnelsModel = new QSqlRelationalTableModel(this);
    taskTunnelsLengthModel = new QSqlQueryModel(this);
         
    /* 一条线路上最近一次采集的隧道数据 */
    taskTunnelsOneLineRecently = new QSqlQueryModel(this);

    /* 一个隧道的历史所有采集数据 */
    taskTunnels_OneTunnelModel = new QSqlQueryModel(this);
}

TaskTunnelDAO::~TaskTunnelDAO()
{
    /* 一次采集任务的所有隧道采集数据 */
    delete taskTunnelListModel;
    delete taskTunnelsModel;
    delete taskTunnelsLengthModel;

    /* 一条线路上最近一次采集的隧道数据 */
    delete taskTunnelsOneLineRecently;

    /* 一个隧道的历史所有采集数据 */
    delete taskTunnels_OneTunnelModel;
}

/**
 * 得到task_tunnel_id，如果有，返回查找后得到task_tunnel_id，如果没有，创建新的task_tunnel_id
 * @param tunnelid 隧道ID
 * @param collectdate 采集时间
 */
_int64 TaskTunnelDAO::getTaskTunnelID(int tunnelid, QString collectdate)
{
    return getTaskTunnel_P(tunnelid, collectdate);
}

/**
 * 某一次采集任务的所有任务隧道隧道名称，排序
 */
QStringListModel * TaskTunnelDAO::getTaskTunnelNames(_int64 taskid)
{
    QSqlQuery query;
    query.exec(QObject::tr("SELECT DISTINCT t.tunnel_name_std FROM tunnel t "
                           "JOIN task_tunnel tt ON tt.tunnel_id = t.tunnel_id "
                           "WHERE tt.task_id = %1 ORDER BY task_tunnel_id DESC").arg(taskid));

    QStringList strings;
    while (query.next()) {
        QString line = query.value(0).toString();
        strings << line;
    }
    taskTunnelListModel->setStringList(strings);
    return taskTunnelListModel;
}

/**
 * 得到某个tasktunnelid对应的隧道名称和采集时间
 * @param newtasktunnelid 
 * @param outputtunnelid 输出的隧道ID
 * @param outputtunnelname 输出的隧道名，从数据库中找出
 * @param output 输出的采集时间，从数据库中找出
 */
bool TaskTunnelDAO::getTaskTunnelInfo(_int64 newtasktunnelid, int &outputtunnelid, QString & outputtunnelname, QString & outputdate, QString & outputlinename)
{
    QSqlQuery query;
    query.exec(QObject::tr("SELECT t.tunnel_name_std, DATE_FORMAT(t0.date,'%Y%m%d'), t.tunnel_id, l.line_name_std  "
                           "FROM task_tunnel tt JOIN tunnel t JOIN task t0 JOIN railway_line l "
                           "ON t.tunnel_id = tt.tunnel_id AND t0.task_id = tt.task_id AND l.line_id = t.line_id "
                           "WHERE tt.task_tunnel_id = %1 ").arg(newtasktunnelid));
    QSqlQueryModel tmpmodel;
    tmpmodel.setQuery(query);
    if (tmpmodel.rowCount() != 1)
        return false;
    else
    {
        outputtunnelname = tmpmodel.record(0).value(0).toString();
        outputdate = tmpmodel.record(0).value(1).toString();
        outputtunnelid = tmpmodel.record(0).value(2).toInt();
        outputlinename = tmpmodel.record(0).value(3).toString();
        return true;
    }
}

/**
 * 某一次采集任务中的所有隧道任务模型，排序
 * @param taskid  任务ID
 * @return QSqlQueryModel * 类型
 */
QSqlQueryModel * TaskTunnelDAO::getTaskTunnels(_int64 taskid)
{
    QSqlQuery query;
    query.exec(QObject::tr("SELECT t.tunnel_name_std, t0.date, "
                              "ABS(t.start_point_std-t.end_point_std) AS tunnel_length, "
                              "t.start_point_std, t.end_point_std "
                           "FROM task_tunnel tt JOIN tunnel t JOIN task t0 "
                           "ON t.tunnel_id = tt.tunnel_id AND t0.task_id = tt.task_id "
                           "WHERE tt.task_id = %1 ").arg(taskid));

    taskTunnelsLengthModel->setQuery(query);

    taskTunnelsLengthModel->setHeaderData(0, Qt::Horizontal, tr("隧道名称"));
    taskTunnelsLengthModel->setHeaderData(1, Qt::Horizontal, tr("采集时间"));
    taskTunnelsLengthModel->setHeaderData(2, Qt::Horizontal, tr("隧道长度"));
    taskTunnelsLengthModel->setHeaderData(3, Qt::Horizontal, tr("起始里程"));
    taskTunnelsLengthModel->setHeaderData(4, Qt::Horizontal, tr("终止里程"));

    return taskTunnelsLengthModel;
}

/**
 * 某一次采集任务中的所有隧道任务模型，排序
 * 【注！】与上一方法返回格式不同
 * 与getTaskTunnelsOneLineRecently返回格式完全一样 @see QSqlQueryModel * getTaskTunnelsOneLineRecently(int )
 * @param taskid  任务ID
 * @return QSqlQueryModel * 类型
 */
QSqlQueryModel * TaskTunnelDAO::getTaskTunnels_2(_int64 taskid)
{
    QSqlQuery query;
    query.exec(QObject::tr("SELECT t.*, t0.date, "
                              "ABS(t.start_point_std-t.end_point_std) AS tunnel_length, "
                              "tt.task_tunnel_id "
                           "FROM task_tunnel tt JOIN tunnel t JOIN task t0 "
                           "ON t.tunnel_id = tt.tunnel_id AND t0.task_id = tt.task_id "
                           "WHERE tt.task_id = %1 ").arg(taskid));

    taskTunnelsModel->setQuery(query);
    taskTunnelsModel->setHeaderData(Tunnel_ID, Qt::Horizontal, tr("隧道ID"));
    taskTunnelsModel->setHeaderData(Tunnel_name_std, Qt::Horizontal, tr("隧道名称"));
    taskTunnelsModel->setHeaderData(Tunnel_ID_std, Qt::Horizontal, tr("隧道编号"));
    taskTunnelsModel->setHeaderData(Tunnel_start_point_std, Qt::Horizontal, tr("起始里程"));
    taskTunnelsModel->setHeaderData(Tunnel_end_point_std, Qt::Horizontal, tr("终止里程"));
    taskTunnelsModel->setHeaderData(Tunnel_is_new_std, Qt::Horizontal, tr("是否为新线"));
    taskTunnelsModel->setHeaderData(Tunnel_line_type_std, Qt::Horizontal, tr("线路类型"));
    taskTunnelsModel->setHeaderData(Tunnel_is_double_line, Qt::Horizontal, tr("是否为双线"));
    taskTunnelsModel->setHeaderData(Tunnel_is_downlink, Qt::Horizontal, tr("是否为下行(双线)"));
    taskTunnelsModel->setHeaderData(Tunnel_line_ID, Qt::Horizontal, tr("所在线路"));
    taskTunnelsModel->setHeaderData(Tunnel_is_bridge, Qt::Horizontal, tr("是否为桥梁"));
    taskTunnelsModel->setHeaderData(Tunnel_is_valid, Qt::Horizontal, tr("是否被删除作废"));

    taskTunnelsModel->setHeaderData(12, Qt::Horizontal, tr("采集时间"));
    taskTunnelsModel->setHeaderData(13, Qt::Horizontal, tr("隧道长度"));
    taskTunnelsModel->setHeaderData(14, Qt::Horizontal, tr("采集工程隧道ID"));

    return taskTunnelsModel;
}

/**
 * 得到一条线路上最近一次采集的隧道数据
 * 与getTaskTunnels_2返回格式完全一样 @see QSqlQueryModel * getTaskTunnels_2(_int64 )
 * @param lineid 线路ID
 */
QSqlQueryModel * TaskTunnelDAO::getTaskTunnelsOneLineRecently(int lineid)
{
    QSqlQuery query;
    query.exec(QObject::tr("SELECT t.*, t0.date, "
                              "ABS(t.start_point_std-t.end_point_std) AS tunnel_length, "
                              "tt.task_tunnel_id  "
                            "FROM task_tunnel tt JOIN tunnel t JOIN task t0 "
                            "ON t.tunnel_id = tt.tunnel_id AND t0.task_id = tt.task_id "
                            "WHERE t.line_id = %1 AND t0.date = "
                            "( "
                            "		SELECT MAX(t1.date) "
                            "		FROM task_tunnel tt1 JOIN task t1 "
                            "		ON tt1.task_id = t1.task_id "
                            "		WHERE tt1.tunnel_id = tt.tunnel_id "
                            ")").arg(lineid));
    taskTunnelsOneLineRecently->setQuery(query);
    taskTunnelsOneLineRecently->setHeaderData(Tunnel_ID, Qt::Horizontal, tr("隧道ID"));
    taskTunnelsOneLineRecently->setHeaderData(Tunnel_name_std, Qt::Horizontal, tr("隧道名称"));
    taskTunnelsOneLineRecently->setHeaderData(Tunnel_ID_std, Qt::Horizontal, tr("隧道编号"));
    taskTunnelsOneLineRecently->setHeaderData(Tunnel_start_point_std, Qt::Horizontal, tr("起始里程"));
    taskTunnelsOneLineRecently->setHeaderData(Tunnel_end_point_std, Qt::Horizontal, tr("终止里程"));
    taskTunnelsOneLineRecently->setHeaderData(Tunnel_is_new_std, Qt::Horizontal, tr("是否为新线"));
    taskTunnelsOneLineRecently->setHeaderData(Tunnel_line_type_std, Qt::Horizontal, tr("线路类型"));
    taskTunnelsOneLineRecently->setHeaderData(Tunnel_is_double_line, Qt::Horizontal, tr("是否为双线"));
    taskTunnelsOneLineRecently->setHeaderData(Tunnel_is_downlink, Qt::Horizontal, tr("是否为下行(双线)"));
    taskTunnelsOneLineRecently->setHeaderData(Tunnel_line_ID, Qt::Horizontal, tr("所在线路"));
    taskTunnelsOneLineRecently->setHeaderData(Tunnel_is_bridge, Qt::Horizontal, tr("是否为桥梁"));
    taskTunnelsOneLineRecently->setHeaderData(Tunnel_is_valid, Qt::Horizontal, tr("是否被删除作废"));

    taskTunnelsOneLineRecently->setHeaderData(12, Qt::Horizontal, tr("采集时间"));
    taskTunnelsOneLineRecently->setHeaderData(13, Qt::Horizontal, tr("隧道长度"));
    taskTunnelsOneLineRecently->setHeaderData(14, Qt::Horizontal, tr("采集工程隧道ID"));

    return taskTunnelsOneLineRecently;
}

/**
 * 某一个隧道的历史所有隧道任务模型，排序
 * getTaskTunnels方法参数含义不同
 * @param tunnelid  隧道ID
 */
QSqlQueryModel * TaskTunnelDAO::getTaskTunnels_OneTunnel(int tunnelid)
{

    /*
        【放弃使用】QRelationalTableMode * 类型的 taskTunnels_OneTunnelModel，因为【速度超级慢】
        taskTunnels_OneTunnelModel->setTable("task_tunnel");

        taskTunnels_OneTunnelModel->setRelation(TaskTunnel_Task_ID,
            QSqlRelation("task", "task_id", "date"));
        taskTunnels_OneTunnelModel->setRelation(TaskTunnel_Tunnel_ID,
            QSqlRelation("tunnel", "tunnel_id", "tunnel_name_std"));
        taskTunnels_OneTunnelModel->setFilter(QObject::tr("task_tunnel.tunnel_id = %1").arg(tunnelid));

        taskTunnels_OneTunnelModel->setSort(TaskTunnel_Task_ID, Qt::AscendingOrder);
    */
    QSqlQuery query;
    query.exec(QObject::tr("SELECT tt.task_tunnel_id, t.tunnel_id, t0.date, t.tunnel_name_std, tt.is_normal, tt.state "
                            "FROM task_tunnel tt JOIN tunnel t JOIN task t0 "
                            "ON t.tunnel_id = tt.tunnel_id AND t0.task_id = tt.task_id "
                            "WHERE t.tunnel_id = %1").arg(tunnelid));
    taskTunnels_OneTunnelModel->setQuery(query);
    taskTunnels_OneTunnelModel->setHeaderData(TaskTunnel_ID, Qt::Horizontal, tr("采集任务隧道ID"));
    taskTunnels_OneTunnelModel->setHeaderData(1, Qt::Horizontal, tr("隧道ID"));
    taskTunnels_OneTunnelModel->setHeaderData(2, Qt::Horizontal, tr("采集时间"));
    taskTunnels_OneTunnelModel->setHeaderData(3, Qt::Horizontal, tr("隧道名称"));
    taskTunnels_OneTunnelModel->setHeaderData(4, Qt::Horizontal, tr("是否正常拍摄(单线是否正序)"));
    taskTunnels_OneTunnelModel->setHeaderData(5, Qt::Horizontal, tr("操作状态"));

    return taskTunnels_OneTunnelModel;
}

/**
 * 是否有该taskTunnelid对应的隧道任务数据库记录
 * @return true有，false没有
 */
bool TaskTunnelDAO::hasTaskTunnelID(_int64 taskTunnelid)
{
    QSqlQuery query;
    query.exec(QString("SELECT * FROM task_tunnel WHERE task_tunnel_id = %1 "
               ).arg(taskTunnelid));

    while (query.next()) {
        return true;
    }

    qDebug() << taskTunnelid << "can not find in table task_tunnel!";
    return false;
}

/**
 * 创建隧道任务，没有taskTunnelid参数，因为加入数据库时采取主键自增策略
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
int TaskTunnelDAO::addTaskTunnel(_int64 taskid, int tunnelid, double interframe_mile, bool carriageDirection, bool isNormal, int state)
{
    int ret = 0;
    QString date = TaskDAO::getTaskDAOInstance()->getTaskDate(taskid);
    if (date.compare("") == 0)
    {
        ret = 2;
        return ret;
    }
    if (!TunnelDAO::getTunnelDAOInstance()->hasTunnelID(tunnelid))
    {
        ret = 3;
        return ret;
    }
    if (getTaskTunnel_P(tunnelid, date) > 0)
    {
        ret = 1;
        return ret;
    }
    createTaskTunnel_P(taskid, tunnelid, interframe_mile, carriageDirection, isNormal, state);
    return ret;
}

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
int TaskTunnelDAO::updateTaskTunnel(_int64 taskTunnelid, _int64 taskid, int tunnelid, double interframe_mile, bool carriageDirection, bool isNormal, int state)
{
    int ret = 0;
    if (!hasTaskTunnelID(taskTunnelid))
    {
        ret = 1;
        return ret;
    }
    QString date = (TaskDAO::getTaskDAOInstance())->getTaskDate(taskid);
    if (date.compare("") == 0)
    {
        ret = 2;
        return ret;
    }
    if (!TunnelDAO::getTunnelDAOInstance()->hasTunnelID(tunnelid))
    {
        ret = 3;
        return ret;
    }
    updateTaskTunnel_P(taskTunnelid, taskid, tunnelid, interframe_mile, carriageDirection, isNormal, state);
    return ret;
}

// 是否有这条隧道任务，return >0 taskTunnelid，-1没有
_int64 TaskTunnelDAO::getTaskTunnel_P(int tunnelid, QString date)
{
    QSqlQuery query;
    query.exec(QString("SELECT tt.* FROM task_tunnel tt "
                            "JOIN task t ON t.task_id = tt.task_id "
                            "WHERE tt.tunnel_id = %1 AND t.date = '%2' "
                       ).arg(tunnelid).arg(date));

    while (query.next()) {
        _int64 taskTunnelid = query.value(0).toLongLong();
        return taskTunnelid;
    }

    qDebug() << "can not find in table task_tunnel!";
    return -1;
}

// 数据库中创建新隧道任务，没有taskTunnelid参数，因为加入数据库时采取主键自增策略
void TaskTunnelDAO::createTaskTunnel_P(_int64 taskid, int tunnelid, double interframe_mile, bool carriageDirection, bool isNormal, int state)
{
    QSqlQuery query;
    query.exec(QString("INSERT INTO task_tunnel (task_id, tunnel_id, interframe_mile, carriage_direction, is_normal, state) "
                       "VALUES (%1, %2, %3, %4, %5, %6) "
                       ).arg(taskid).arg(tunnelid).arg(interframe_mile).arg(carriageDirection).arg(isNormal).arg(state));
    qDebug() << "createTaskTunnel_P, task_id" << taskid << ", tunnelid: " << tunnelid <<  ", interframe_mile" << interframe_mile
        << ", carriageDirection" << carriageDirection << ", isnormal: " << isNormal << ", state: " << state;

}

// 数据库中修改task_tunnel
void TaskTunnelDAO::updateTaskTunnel_P(_int64 taskTunnelid, _int64 taskid, int tunnelid, double interframe_mile, bool carriageDirection, bool isNormal, int state)
{
    QSqlQuery query;
    query.exec(QString("UPDATE task_tunnel SET task_id = %1, tunnel_id = %2, interframe_mile = %3, carriage_direction = %4, "
                       "is_normal = %5, state = %6 "
                       "WHERE task_tunnel_id = %7")
                       .arg(taskid).arg(tunnelid).arg(interframe_mile).arg(carriageDirection).arg(isNormal).arg(state).arg(taskTunnelid));
    qDebug() << "updateTaskTunnel_P" << taskTunnelid;

}

// 数据库中删除task_tunnel
void TaskTunnelDAO::deleteTaskTunnel_P(_int64 taskTunnelid)
{
    QSqlQuery query;
    query.exec(QString("DELETE FROM task_tunnel WHERE task_tunnel_id = %1").arg(taskTunnelid));
    qDebug() << "deleteTaskTunnel_P" << taskTunnelid;

}
