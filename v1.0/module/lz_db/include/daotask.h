#ifndef DAOTASK_H
#define DAOTASK_H

#include <QObject>
#include <QSqlQuery>
#include <QSqlTableModel>

enum {
    Task_ID = 0,
    Task_frame_num = 1,
    Task_date = 2,
    Task_carriage_direction = 3,
    Task_remark = 4, // 采集线路名（如果是多条线路“xx线_xx线”表示出来，方便查找）
    Task_Line_ID = 5,
    Task_Line_Name = 6
};

/**
 * 任务数据访问对象类声明
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2013-8-10
 */
class TaskDAO : public QObject
{
    Q_OBJECT
private:
    // singleton设计模式，静态私有实例变量
    static TaskDAO * taskDAOInstance;

    QSqlTableModel * recentTasksModel;

    QSqlQueryModel * recentTasksModelByLine;

    // 每页数量
    int pageSize;
    int initPageIndex;

public:
    /**
     * singleton设计模式，公有静态方法返回单一实例
     * @return 单一实例
     */
    static TaskDAO * getTaskDAOInstance();

    ~TaskDAO();

    /**
     * 返回任务模型，默认每页pageSize个
     * @param 第几页 从0开始递增
     */
    QSqlTableModel * getRecentTasks(int pageStart);

    /**
     * 返回任务模型，默认每页pageSize个
     * @overload  getRecentTasks方法重载
     * @param 第几页 从0开始递增
     * @param 某一年的任务
     */
    QSqlQueryModel * getRecentTasks(int pageStart, int year);

    /**
     * 返回任务模型，默认每页pageSize个
     * @overload  getRecentTasks方法重载
     * @param 第几页 从0开始递增
     * @param 某一年的任务
     */
    QSqlQueryModel * getRecentTasksByLine(int pageStart, int year);

    /**
     * 返回每页数目
     */
    int getPageSize();

    /**
     * 设置每页数目
     */
    void setPageSize(int newPageSize);

    /**
     * 是否有这条隧道采集任务组task
     * @return >0 taskid，-1没有
     */
    __int64 getTaskID(int tunnelid, QString date);

    /**
     * 是否有该taskid对应的任务记录
     * @return true有，false没有
     */
    bool hasTaskID(__int64 taskid);

    /**
     * 创建任务，没有taskid参数，因为加入数据库时采取主键自增策略
     * @return 新的taskid > 0
     */
    __int64 addTask(int frameNum, QString date, bool carriageDirection, QString remark);

    /**
     * 对任务的增删改操作
     * @return 0 成功，1没有这个taskid号的task
     */
    int updateTask(__int64 taskid, int frameNum, QString date, bool carriageDirection, QString remark);

    /**
     * 得到某个任务的采集时间 年-月-日
     */
    QString getTaskDate(__int64 taskid);

private:

    // singleton单一设计模式，构造函数私有
    explicit TaskDAO(QObject *parent = 0);

    // 更新数据模型
    void updateRecentTasksModel(int pageStart);
    void updateRecentTasksModel(int year, int pageStart);

    __int64 getNewestTaskID(int frameNum, QString date, bool carriageDirection, QString remark);

    // 数据库中创建新线路，没有taskid参数，因为加入数据库时采取主键自增策略
    bool createTask_P(int frameNum, QString date, bool carriageDirection, QString remark);

    // 数据库中修改task
    bool updateTask_P(__int64 taskid, int frameNum, QString date, bool carriageDirection, QString remark);

    // 数据库中删除task
    bool deleteTask_P(__int64 taskid);
    
};

#endif // DAOTASK_H
