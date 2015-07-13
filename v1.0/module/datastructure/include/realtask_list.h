#ifndef REALTASK_LIST_H
#define REALTASK_LIST_H

#include <QObject>
#include <QMutex>
#include "realtask.h"

/**
 * 实际采集存储list结构类声明
 * list中每个元素是RealTask
 * @see RealTask
 * @author xiongxue
 * @date 2013-11-13
 * @version 1.0.0
 */
class RealTaskList : public QObject
{
    Q_OBJECT
public:
    /**
     * singleton单例设计模式，公有静态方法返回单一实例
     * @return 单一实例
     */
    //static RealTaskList* getRealTaskListInstance();
    explicit RealTaskList(QObject *parent = 0);
    ~RealTaskList();
    
    /**
     * 返回list
     */
    const QList<RealTask>* list();
    bool pushBack(RealTask realTask);
    
    /**
     * 清空整个list
     */
    void clear();

    QList<RealTask>::iterator begin();
    QList<RealTask>::iterator end();

    /**
     * 从list中找到某个RealTask
     * @param seq 输入的实际采集顺序号
     * @param outputrealtask 输出的实际任务结构体
     * @return true 找到
     */
    bool getRealTaskBySeqno(int seq, RealTask & outputrealtask);
    RealTask & getRealTaskBySeqno(int seq, bool & ret);

    /**
     * 检查是否所有原从机文件都已备份
     */
    bool checkAllBackup();

private:
    //explicit RealTaskList(QObject *parent = 0);
    //static RealTaskList * realTaskListInstance;

    QList<RealTask> *slavetaskList;

    QMutex mutex;
};

#endif // REALTASK_LIST_H
