#include "plantask_list.h"
#include <QDebug>

/**
 * 计划任务存储list结构类定义
 * @author xiongxue
 * @date 2013-11-13
 * @version 1.0.0
 */
//singleton设计模式，静态私有实例变量
/*PlanTaskList * PlanTaskList::planTaskListInstance = NULL;

PlanTaskList* PlanTaskList::getPlanTaskListInstance()
{
    if (planTaskListInstance == NULL)
    {
        planTaskListInstance = new PlanTaskList();
    }
    return planTaskListInstance;
}*/

PlanTaskList::PlanTaskList(QObject *parent) :
    QObject(parent)
{
    slavetaskList = new QList<PlanTask>();
}

PlanTaskList::~PlanTaskList()
{
    delete slavetaskList;
}

const QList<PlanTask>* PlanTaskList::list()
{
    return slavetaskList;
}

bool PlanTaskList::pushBack(PlanTask slavetaskModel)
{
    slavetaskList->push_back(slavetaskModel);
    return true;
}

void PlanTaskList::clear()
{
    slavetaskList->clear();
}

QList<PlanTask>::iterator PlanTaskList::begin() {return slavetaskList->begin();}
QList<PlanTask>::iterator PlanTaskList::end() {return slavetaskList->end();}

/**
 * 从list中查找plan
 * @param tunnelid 输入隧道ID
 * @param outputplan 输出PlanTask
 * @return true 找到该隧道ID
 */
bool PlanTaskList::getPlanTask(int tunnelid, PlanTask & newplantask)
{
    for(int i=0;i<slavetaskList->length();i++)
    {
        if(slavetaskList->at(i).tunnelnum == tunnelid)
        {
            newplantask = slavetaskList->at(i);
            return true;
        }
    }
    return false;
}

string PlanTaskList::gettunnelname(int tunnelid)
{
    if (slavetaskList->length() == 0)
    {
        qDebug() << tr("计划隧道配置文件未加载");
        return "-1";
    }
    for(int i=0;i<slavetaskList->length();i++)
    {
        if(slavetaskList->at(i).tunnelnum == tunnelid)
        {
            return slavetaskList->at(i).tunnelname;
        }
    }
    return "-1";
}

int PlanTaskList::getlineid(int tunnelid)
{
    if (slavetaskList->length() == 0)
    {
        qDebug() << tr("计划隧道配置文件未加载");
        return -1;
    }
    for(int i=0;i<slavetaskList->length();i++)
    {
        if(slavetaskList->at(i).tunnelnum==tunnelid)
        {
            return slavetaskList->at(i).linenum;
        }
    }
    return -1;
}

string PlanTaskList::getdatetime(int tunnelid)
{
    if (slavetaskList->length() == 0)
    {
        qDebug() << tr("计划隧道配置文件未加载");
        return "-1";
    }
    for(int i=0;i<slavetaskList->length();i++)
    {
        if(slavetaskList->at(i).tunnelnum == tunnelid)
        {
            return slavetaskList->at(i).datetime;
        }
    }
    return "-1";
}

/**
 * qDebug把整个校正队列list显示到控制台
 */
void PlanTaskList::showList()
{
    qDebug() << "************************************";
    qDebug() << "PlanTaskList:";

    QList<PlanTask>::iterator checkedTaskIterator = begin();
    while (checkedTaskIterator != end())
    {
        qDebug() << "PlanTask:" << checkedTaskIterator->tunnelnum << QString::fromLocal8Bit(checkedTaskIterator->tunnelname.c_str());
        checkedTaskIterator++;
    }
    qDebug() << "************************************";
}