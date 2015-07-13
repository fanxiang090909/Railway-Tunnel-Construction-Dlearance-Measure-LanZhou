#ifndef PLANTASK_LIST_H
#define PLANTASK_LIST_H

#include <QObject>
#include "plantask.h"

/**
 * 计划任务存储list结构类声明
 * @author xiongxue
 * @date 2013-11-13
 * @version 1.0.0
 */
class PlanTaskList : public QObject
{
    Q_OBJECT
public:
    /**
     * singleton单例设计模式，公有静态方法返回单一实例
     * @return 单一实例
     */
    //static PlanTaskList* getPlanTaskListInstance();
    explicit PlanTaskList(QObject *parent = 0);
    ~PlanTaskList();
    /**
     * @return list
     */
    const QList<PlanTask>* list();
    /**
     * 插入到list末尾
     * @return true 插入成功
     */
    bool pushBack(PlanTask planTask);
    /**
     * 清空整个list
     */
    void clear();

    QList<PlanTask>::iterator begin();
    QList<PlanTask>::iterator end();

    /*******功能********/
    /**
     * 从list中查找plan
     * @param tunnelid 输入隧道ID
     * @param outputplan 输出PlanTask
     * @return true 找到该隧道ID
     */
    bool getPlanTask(int tunnelid, PlanTask & outputplan);
    /**
     * 获得隧道名称
     */
    string gettunnelname(int tunnelid);
    /**
     * 获得线路id
     */
    int getlineid(int tunnelid);
    /**
     * 获得隧道对应的时间
     */
    string getdatetime(int tunnelid);

    /**
     * qDebug把整个校正队列list显示到控制台
     */
    void showList();

private:
    //explicit PlanTaskList(QObject *parent = 0);
    //static PlanTaskList * planTaskListInstance;

    QList<PlanTask> *slavetaskList;
};

#endif // PLANTASK_LIST_H
