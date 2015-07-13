#include "realtask.h"
#include "realtask_list.h"
#include <QDebug>

/**
 * 实际采集存储list结构类定义
 * @author xiongxue
 * @date 2013-11-13
 * @version 1.0.0
 */
//singleton设计模式，静态私有实例变量
/*RealTaskList * RealTaskList::realTaskListInstance = NULL;

RealTaskList* RealTaskList::getRealTaskListInstance()
{
    if (realTaskListInstance == NULL)
    {
        realTaskListInstance = new RealTaskList();
    }
    return realTaskListInstance;
}*/

RealTaskList::RealTaskList(QObject *parent) :
    QObject(parent)
{
    slavetaskList = new QList<RealTask>();
}

RealTaskList::~RealTaskList()
{
    delete slavetaskList;
}

const QList<RealTask>* RealTaskList::list()
{
    return slavetaskList;
}

bool RealTaskList::pushBack(RealTask slavetaskModel)
{
    mutex.lock();
    slavetaskList->push_back(slavetaskModel);
    mutex.unlock();
    return true;
}

void RealTaskList::clear()
{
    mutex.lock();
    slavetaskList->clear();
    mutex.unlock();
}

QList<RealTask>::iterator RealTaskList::begin() {return slavetaskList->begin();}
QList<RealTask>::iterator RealTaskList::end() {return slavetaskList->end();}

/**
 * 从list中找到某个RealTask
 * @param seq 输入的实际采集顺序号
 * @param outputrealtask 输出的实际任务结构体
 * @return true 找到
 */
bool RealTaskList::getRealTaskBySeqno(int seq, RealTask & outputrealtask)
{
    mutex.lock();
    for (int i = 0; i < slavetaskList->length(); i++)
    {
        if (slavetaskList->at(i).seqno == seq)
        {
             outputrealtask = slavetaskList->at(i);
             mutex.unlock();
             return true;
        }
    }
    mutex.unlock();
    return false;
}

RealTask & RealTaskList::getRealTaskBySeqno(int seq, bool & ret)
{
    mutex.lock();
    QList<RealTask>::iterator it = begin();
    while (it != end())
    {
        if ((*it).seqno == seq)
        {
            ret = true;
            mutex.unlock();
            return (*it);
        }
        it++;
    }
    ret = false;
    mutex.unlock();
    return (*it);
}

/**
 * 检查是否所有原从机文件都已备份
 */
bool RealTaskList::checkAllBackup()
{
    mutex.lock();

    for (int i = 0; i < slavetaskList->length(); i++)
    {
        RealTask tmp = slavetaskList->at(i);
        if (tmp.has_backup_A1 && tmp.has_backup_A2 && tmp.has_backup_B1 && tmp.has_backup_B2 && 
            tmp.has_backup_C1 && tmp.has_backup_C2 && tmp.has_backup_D1 && tmp.has_backup_D2 &&
            tmp.has_backup_E1 && tmp.has_backup_E2 && tmp.has_backup_F1 && tmp.has_backup_F2 &&
            tmp.has_backup_G1 && tmp.has_backup_G2 && tmp.has_backup_H1 && tmp.has_backup_H2 &&
            tmp.has_backup_I1 && tmp.has_backup_I2 && tmp.has_backup_J1 && tmp.has_backup_J2 &&
            tmp.has_backup_K1 && tmp.has_backup_K2 && tmp.has_backup_L1 && tmp.has_backup_L2 &&
            tmp.has_backup_M1 && tmp.has_backup_M2 && tmp.has_backup_N1 && tmp.has_backup_N2 &&
            tmp.has_backup_O1 && tmp.has_backup_O2 && tmp.has_backup_P1 && tmp.has_backup_P2 &&
            tmp.has_backup_Q1 && tmp.has_backup_Q2 && tmp.has_backup_R1 && tmp.has_backup_R2)
        {
             continue;
        }
        else
        {
            mutex.unlock();
            return false;
        }
    }
    mutex.unlock();
    return true;
}