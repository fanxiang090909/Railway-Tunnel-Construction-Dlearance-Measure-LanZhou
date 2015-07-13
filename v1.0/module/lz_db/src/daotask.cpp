#include <QObject>
#include <QDebug>
#include "daotask.h"

/**
 * 任务数据访问对象类实现
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2013-8-10
 */
// singleton设计模式，静态私有实例变量
TaskDAO * TaskDAO::taskDAOInstance = NULL;

/**
 * singleton设计模式，公有静态方法返回单一实例
 * @return 单一实例
 */
TaskDAO * TaskDAO::getTaskDAOInstance()
{
    if (taskDAOInstance == NULL)
    {
        taskDAOInstance = new TaskDAO();
    }
    return taskDAOInstance;
}

// singleton单一设计模式，构造函数私有
TaskDAO::TaskDAO(QObject *parent) :
    QObject(parent)
{
    initPageIndex = -1;
    pageSize = 20;
    recentTasksModel = new QSqlTableModel(this);
    recentTasksModelByLine = new QSqlQueryModel(this);
}

TaskDAO::~TaskDAO()
{
    delete recentTasksModel;
    delete recentTasksModelByLine;
}

/**
 * 返回任务模型，默认每页pageSize个
 * @param 第几页 从0开始递增
 */
QSqlTableModel * TaskDAO::getRecentTasks(int pageStart)
{
    recentTasksModel->setTable("task");
    recentTasksModel->setHeaderData(Task_ID, Qt::Horizontal, tr("任务ID"));
    recentTasksModel->setHeaderData(Task_frame_num, Qt::Horizontal, tr("每米帧间隔"));
    recentTasksModel->setHeaderData(Task_date, Qt::Horizontal, tr("采集日期"));
    recentTasksModel->setHeaderData(Task_carriage_direction, Qt::Horizontal, tr("车厢正反"));
    updateRecentTasksModel(pageStart);
    recentTasksModel->select();
    return recentTasksModel;
}

/**
 * 返回任务模型，默认每页pageSize个
 * @overload  getRecentTasks方法重载
 * @param 第几页 从0开始递增
 * @param 某一年的任务
 */
QSqlQueryModel * TaskDAO::getRecentTasks(int pageStart, int year)
{
    QSqlQuery query;
    query.exec(QString("SELECT t.* FROM task t "
                       "WHERE YEAR(date) = %1 "
                       "ORDER BY date DESC LIMIT %2, %3").arg(year).arg(pageStart * this->pageSize).arg(this->pageSize));
    recentTasksModelByLine->setQuery(query);
    recentTasksModelByLine->setHeaderData(0, Qt::Horizontal, tr("任务（线路）ID"));
    recentTasksModelByLine->setHeaderData(1, Qt::Horizontal, tr("采集帧数"));
    recentTasksModelByLine->setHeaderData(2, Qt::Horizontal, tr("采集日期"));
    recentTasksModelByLine->setHeaderData(3, Qt::Horizontal, tr("车厢正反"));
    return recentTasksModelByLine;
}

/**
 * 返回任务模型，默认每页pageSize个
 * @overload  getRecentTasks方法重载
 * @param 第几页 从0开始递增
 * @param 某一年的任务
 */
QSqlQueryModel * TaskDAO::getRecentTasksByLine(int pageStart, int year)
{
    QSqlQuery query;
    query.exec(QString("SELECT t.*, l.line_id, l.line_name_std FROM task t "
            "JOIN task_tunnel tt ON tt.task_id = t.task_id "
            "JOIN tunnel tu ON tu.tunnel_id = tt.tunnel_id "
            "JOIN railway_line l ON l.line_id = tu.line_id "
            "WHERE YEAR(date) <= %1 "
            "GROUP BY t.task_id "
            "ORDER BY date DESC LIMIT %2, %3").arg(year).arg(pageStart * this->pageSize).arg(this->pageSize));
    recentTasksModelByLine->setQuery(query);

    recentTasksModelByLine->setHeaderData(2, Qt::Horizontal, tr("任务（线路）ID"));
    recentTasksModelByLine->setHeaderData(3, Qt::Horizontal, tr("采集帧数"));
    recentTasksModelByLine->setHeaderData(4, Qt::Horizontal, tr("采集日期"));
    recentTasksModelByLine->setHeaderData(4, Qt::Horizontal, tr("车厢正反"));
    recentTasksModelByLine->setHeaderData(0, Qt::Horizontal, tr("线路ID"));
    recentTasksModelByLine->setHeaderData(1, Qt::Horizontal, tr("线路名称"));

    return recentTasksModelByLine;
}

/**
 * 返回每页数目
 */
int TaskDAO::getPageSize()
{
    return pageSize;
}

/**
 * 设置每页数目
 */
void TaskDAO::setPageSize(int newPageSize)
{
    this->pageSize = newPageSize;
}

// 是否有这条隧道采集任务组task，return >0 taskid，-1没有
__int64 TaskDAO::getTaskID(int tunnelid, QString date)
{
    QSqlQuery query;
    query.exec(QString("SELECT t.task_id FROM task_tunnel tt "
                            "JOIN task t ON t.task_id = tt.task_id "
                            "WHERE tt.tunnel_id = %1 AND t.date = '%2' "
                       ).arg(tunnelid).arg(date));

    while (query.next()) {
        __int64 taskid = query.value(0).toLongLong();
        return taskid;
    }

    qDebug() << "can not find in table task!";
    return -1;
}

/**
 * 是否有该taskid对应的任务记录
 * @return true有，false没有
 */
bool TaskDAO::hasTaskID(__int64 taskid)
{
    QSqlQuery query;
    query.exec(QString("SELECT * FROM task WHERE task_id = %1 "
               ).arg(taskid));

    while (query.next()) {
        return true;
    }

    qDebug() << taskid << "can not find in table task!";
    return false;
}

/**
 * 创建任务，没有taskid参数，因为加入数据库时采取主键自增策略
 * @return 新的taskid > 0
 */
__int64 TaskDAO::addTask(int frameNum, QString date, bool carriageDirection, QString remark)
{
    this->createTask_P(frameNum, date, carriageDirection, remark);
    return this->getNewestTaskID(frameNum, date, carriageDirection, remark);
}

/**
 * 对任务的增删改操作
 * @return 0 成功，1没有这个taskid号的task
 */
int TaskDAO::updateTask(__int64 taskid, int frameNum, QString date, bool carriageDirection, QString remark)
{
    if (!hasTaskID(taskid))
    {
        qDebug() << taskid << "can not find in table task!";
        return 1;
    }
    else
        this->updateTask_P(taskid, frameNum, date, carriageDirection, remark);
    return 0;
}

/**
 * 得到某个任务的采集时间 年-月-日
 */
QString TaskDAO::getTaskDate(__int64 taskid)
{
    QSqlQuery query;
    query.exec(QString("SELECT t.date FROM task t WHERE t.task_id = %1 "
                       ).arg(taskid));

    while (query.next()) {
        QString taskDate = query.value(0).toString();
        return taskDate;
    }

    qDebug() << taskid << "can not find in table task";
    return "";
}

// 更新数据模型
void TaskDAO::updateRecentTasksModel(int pageStart)
{
    QString strFilter  = QString("1 = 1 ORDER BY date DESC LIMIT %1, %2").arg(pageStart * this->pageSize).arg(this->pageSize);
    recentTasksModel->setFilter(strFilter);
}

void TaskDAO::updateRecentTasksModel(int year, int pageStart)
{
    QString strFilter  = QString("YEAR(date) = %1 ORDER BY date DESC LIMIT %2, %3").arg(year).arg(pageStart * this->pageSize).arg(this->pageSize);
    recentTasksModel->setFilter(strFilter);
}

__int64 TaskDAO::getNewestTaskID(int frameNum, QString date, bool carriageDirection, QString remark)
{
    QSqlQuery query;
    query.exec(QString("SELECT * FROM task WHERE frame_num = %1 AND date = '%2' AND carriage_direction = %3 "
               ).arg(frameNum).arg(date).arg(carriageDirection));

    while (query.next()) {
        __int64 taskid = query.value(0).toLongLong();
        return taskid;
    }

    qDebug() << "date:" << date << " can not find in table task!";
    return -1;
}

// 数据库中创建新线路，没有taskid参数，因为加入数据库时采取主键自增策略
bool TaskDAO::createTask_P(int frameNum, QString date, bool carriageDirection, QString remark)
{
    QSqlQuery query;
    bool ret = query.exec(QString("INSERT INTO task (frame_num, date, carriage_direction, task_remark) "
                       "VALUES (%1, '%2', %3, '%4') "
               ).arg(frameNum).arg(date).arg(carriageDirection).arg(remark));
    qDebug() << "createTask_P" << date << ret;
    return ret;

}

// 数据库中修改task
bool TaskDAO::updateTask_P(_int64 taskid, int frameNum, QString date, bool carriageDirection, QString remark)
{
    QSqlQuery query;
    bool ret = query.exec(QString("UPDATE task SET frame_num = %1, "
               "date = %2, carriage_direction = %3 , task_remark = '%4' "
               "WHERE task_id = %4").arg(taskid).arg(frameNum).arg(frameNum).arg(carriageDirection).arg(remark));
    qDebug() << "updateTask_P" << taskid << ret;
    return ret;
}

// 数据库中删除task
bool TaskDAO::deleteTask_P(__int64 taskid)
{
    QSqlQuery query;
    bool ret = query.exec(QString("DELETE FROM task WHERE task_id = %1").arg(taskid));
    qDebug() << "deleteTask_P" << taskid << ret;
    return ret;
}
