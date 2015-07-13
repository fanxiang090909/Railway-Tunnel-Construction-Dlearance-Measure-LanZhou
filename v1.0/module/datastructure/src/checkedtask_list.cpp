#include "checkedtask_list.h"
#include <QDebug>

#include <string>
#include <QString>

/**
 * 校正任务配置设置类实现
 * @author xiongxue
 * @author fanxiang
 * @version 1.0.0
 * @date 2013-10-13
 */
// singleton设计模式，静态私有实例变量
//CheckedTaskList * CheckedTaskList::correctedTaskListInstance = NULL;

/**
 * singleton设计模式，公有静态方法返回单一实例
 * @return 单一实例
 */
/*CheckedTaskList* CheckedTaskList::getCheckedTaskListInstance()
{
    if (correctedTaskListInstance == NULL)
    {
        correctedTaskListInstance = new CheckedTaskList();
    }
    return correctedTaskListInstance;
}*/

// singleton单一设计模式，构造函数私有
CheckedTaskList::CheckedTaskList(QObject *parent) :
    QObject(parent)
{
    slavetunnelcheckList = new QList<CheckedTunnelTaskModel>();
}

CheckedTaskList::~CheckedTaskList()
{
    QList<CheckedTunnelTaskModel>::iterator it = begin();
    for (int i = 0; i < slavetunnelcheckList->length(); i++)
        it = slavetunnelcheckList->erase(it);
    delete slavetunnelcheckList;
}

/**
 * 返回list
 * 常类型是指使用类型修饰符const说明的类型，常类型的变量或对象的值是不能被更新的。
 */
const QList<CheckedTunnelTaskModel>* CheckedTaskList::list()
{
    return slavetunnelcheckList;
}

/**
 * 得到CheckedTunnelTaskModel
 * @param tunnelid 输入的隧道ID
 * @param task 输出的找到值
 */
bool CheckedTaskList::getCheckedTunnelModel(int tunnelid, CheckedTunnelTaskModel & task)
{
    mutex.lock();
    QList<CheckedTunnelTaskModel>::iterator checkedTaskIterator = begin();
    while (checkedTaskIterator != end())
    {
        qDebug() << "CheckedtTaskModel:plan:" << checkedTaskIterator->planTask.tunnelnum << QString::fromLocal8Bit(checkedTaskIterator->planTask.tunnelname.c_str())
            << checkedTaskIterator->getRealList()->size();
        if (tunnelid == checkedTaskIterator->planTask.tunnelnum)
        {
            // 【注意！】调用了重载的复制构造函数
            task = (*checkedTaskIterator);
            mutex.unlock();
            return true;
        }
        checkedTaskIterator++;
    }
    mutex.unlock();
    return false;
}

/**
 * (在文件未被校正前的版本)得到CheckedTunnelTaskModel
 * @param tunnelid 输入的隧道ID
 * @param startseqno 输入的起始seqno采集序列号
 * @param task 输出的找到值
 */
bool CheckedTaskList::getCheckedTunnelModel(int tunnelid, int startseqno, CheckedTunnelTaskModel & task)
{
    mutex.lock();
    QList<CheckedTunnelTaskModel>::iterator checkedTaskIterator = begin();
    while (checkedTaskIterator != end())
    {
        qDebug() << "CheckedtTaskModel:plan:" << checkedTaskIterator->planTask.tunnelnum << QString::fromLocal8Bit(checkedTaskIterator->planTask.tunnelname.c_str())
                 << checkedTaskIterator->getRealList()->size();
        if (tunnelid == checkedTaskIterator->planTask.tunnelnum)
        {
            if (checkedTaskIterator->getRealList()->size() != 0)
            {
                if (startseqno == (*checkedTaskIterator->begin()).seqno)
                {
                    // 【注意！】调用了重载的复制构造函数
                    task = (*checkedTaskIterator);
                    mutex.unlock();
                    return true;
                }
            }
            else
                break;

        }
        checkedTaskIterator++;
    }
    mutex.unlock();
    return false;
}

CheckedTunnelTaskModel & CheckedTaskList::getCheckedTunnelModel(int tunnelid, int startseqno, bool & ret)
{
    mutex.lock();
    QList<CheckedTunnelTaskModel>::iterator checkedTaskIterator = begin();
    while (checkedTaskIterator != end())
    {
        qDebug() << "CheckedtTaskModel:plan:" << checkedTaskIterator->planTask.tunnelnum << QString::fromLocal8Bit(checkedTaskIterator->planTask.tunnelname.c_str())
            << checkedTaskIterator->getRealList()->size();
        if (tunnelid == checkedTaskIterator->planTask.tunnelnum)
        {
            if (checkedTaskIterator->getRealList()->size() != 0)
            {
                if (startseqno == (*checkedTaskIterator->begin()).seqno)
                {
                    // 【注意！】调用了重载的复制构造函数
                    mutex.unlock();
                    ret = true;
                    return (*checkedTaskIterator);
                }
            }
            else
                break;

        }
        checkedTaskIterator++;
    }
    ret = false;
    mutex.unlock();
    return (*checkedTaskIterator);
}

/**
 * 得到CheckedTunnelTaskModel
 * @param tunnelid 输入的隧道ID
 * @param masterfc 输入的起始seqno采集序列号
 * @param task 输出的找到值
 */
bool CheckedTaskList::getCheckedTunnelModel(int tunnelid, __int64 masterfc, CheckedTunnelTaskModel & task)
{
    mutex.lock();
    QList<CheckedTunnelTaskModel>::iterator checkedTaskIterator = begin();
    while (checkedTaskIterator != end())
    {
        qDebug() << "CheckedtTaskModel:plan:" << checkedTaskIterator->planTask.tunnelnum << QString::fromLocal8Bit(checkedTaskIterator->planTask.tunnelname.c_str())
                 << checkedTaskIterator->getRealList()->size();
        if (tunnelid == checkedTaskIterator->planTask.tunnelnum)
        {

            if (checkedTaskIterator->getRealList()->size() != 0)
            {
                std::list<CheckedItem>::iterator it = checkedTaskIterator->getRealList()->begin();
                while (it != checkedTaskIterator->getRealList()->end())
                {
                    if (masterfc >= it->start_frame_master && masterfc <= it->end_frame_master)
                    {
                        qDebug() << "\t\tfind in seqno" << it->seqno << ", real item start(" << it->start_frame_master << ") < masterfc(" << masterfc << ") < end(" << it->end_frame_master << ")";
                        // 【注意！】调用了重载的复制构造函数
                        task = (*checkedTaskIterator);
                        mutex.unlock();
                        return true;
                    }
                    it++;
                }                
            }
            else
                break;

        }
        checkedTaskIterator++;
    }
    mutex.unlock();
    return false;
}

CheckedTunnelTaskModel & CheckedTaskList::getCheckedTunnelModel(int tunnelid, bool & ret)
{
    mutex.lock();
    QList<CheckedTunnelTaskModel>::iterator checkedTaskIterator = begin();
    while (checkedTaskIterator != end())
    {
        qDebug() << "CheckedtTaskModel:plan:" << checkedTaskIterator->planTask.tunnelnum << QString::fromLocal8Bit(checkedTaskIterator->planTask.tunnelname.c_str())
            << checkedTaskIterator->getRealList()->size();
        if (tunnelid == checkedTaskIterator->planTask.tunnelnum)
        {
            // 【注意！】调用了重载的复制构造函数
            ret = true;
            mutex.unlock();
            return (*checkedTaskIterator);
        }
        checkedTaskIterator++;
    }
    ret = false;
    mutex.unlock();
    return (*checkedTaskIterator);
}

bool CheckedTaskList::pushback(CheckedTunnelTaskModel &correctedTunnelTaskModel)
{
    mutex.lock();
    slavetunnelcheckList->push_back(correctedTunnelTaskModel);
    mutex.unlock();

    return true;
}

/**
 * 清空整个list
 */
void CheckedTaskList::clear()
{
    //slavetunnelcheckList->clear();//删除slaveopentaskList的内容，即清空该list
    mutex.lock();
    QList<CheckedTunnelTaskModel>::iterator it = begin();
    while (it != end())
    {
        it = slavetunnelcheckList->erase(it);
        //showList();
    }
    mutex.unlock();
}

QList<CheckedTunnelTaskModel>::iterator CheckedTaskList::begin() {return slavetunnelcheckList->begin();}
QList<CheckedTunnelTaskModel>::iterator CheckedTaskList::end() {return slavetunnelcheckList->end();}

/**
 * 中间插入一个校正隧道元素到校正队列
 * @param i 插入位置
 * @param t 要插入的校正隧道元素
 */
void CheckedTaskList::insertAt(int i, const CheckedTunnelTaskModel & t)
{
    mutex.lock();
    slavetunnelcheckList->insert(i, t);
    mutex.unlock();
}

/**
 * 从隧道校正队列中删除某一项校正隧道元素
 * @param i 删除位置
 */
void CheckedTaskList::deletelistone(int i)//删除list的某一行
{
    mutex.lock();
    slavetunnelcheckList->removeAt(i);
    mutex.unlock();
}

// qDebug显示到控制台
void CheckedTaskList::showList()
{
    qDebug() << "************************************";
    qDebug() << "CheckedtTaskList:";

    mutex.lock();
    QList<CheckedTunnelTaskModel>::iterator checkedTaskIterator = begin();
    while (checkedTaskIterator != end())
    {
        qDebug() << "CheckedtTaskModel:plan:" << checkedTaskIterator->planTask.tunnelnum << QString::fromLocal8Bit(checkedTaskIterator->planTask.tunnelname.c_str())
                 << checkedTaskIterator->getRealList()->size();
        _int64 checkedstart;
        _int64 checkedend;
        std::list<CheckedItem>::iterator it = checkedTaskIterator->getRealList()->begin();
        if (it != checkedTaskIterator->getRealList()->end())
        {
            checkedstart = it->start_frame_master;
            checkedend = it->end_frame_master;

            while (it != checkedTaskIterator->getRealList()->end())
            {
                qDebug() << "\t\t" << it->seqno << it->tunnel_id << QString::fromLocal8Bit(it->tunnel_name.c_str()) << checkedstart << checkedend;

                if (it->start_frame_master < checkedstart)
                    checkedstart = it->start_frame_master;
                if (it->end_frame_master > checkedend)
                    checkedend = it->end_frame_master;
                it++;
            }
        }
        checkedTaskIterator++;
    }
    mutex.unlock();
    qDebug() << "************************************";
}

/**
 * 获得隧道ID
 */
int CheckedTaskList::getTunnelIDByTunnelName(string tunnelname)
{
    if (slavetunnelcheckList->length() == 0)
    {
        qDebug() << tr("校正任务配置文件未加载");
        return -1;
    }
    mutex.lock();
    for (int i = 0; i < slavetunnelcheckList->length(); i++)
    {
        if (slavetunnelcheckList->at(i).planTask.tunnelname.compare(tunnelname) == 0)
        {
            mutex.unlock();
            return slavetunnelcheckList->at(i).planTask.tunnelnum;
        }
    }
    mutex.unlock();
    return -1;
}

/**
 * 检查是否所有主控文件都已计算备份
 */
bool CheckedTaskList::checkAllCalcuBackup()
{
    mutex.lock();
    for (int i = 0; i < slavetunnelcheckList->length(); i++)
    {
        CheckedTunnelTaskModel tmp = slavetunnelcheckList->at(i);
        if (tmp.calcuItem.has_backup_calc_A == 6 && tmp.calcuItem.has_backup_calc_B == 6 && 
            tmp.calcuItem.has_backup_calc_C == 6 && tmp.calcuItem.has_backup_calc_D == 6 &&
            tmp.calcuItem.has_backup_calc_E == 6 && tmp.calcuItem.has_backup_calc_F == 6 && 
            tmp.calcuItem.has_backup_calc_G == 6 && tmp.calcuItem.has_backup_calc_H == 6 &&
            tmp.calcuItem.has_backup_calc_I == 6 && tmp.calcuItem.has_backup_calc_J == 6 && 
            tmp.calcuItem.has_backup_calc_K == 6 && tmp.calcuItem.has_backup_calc_L == 6 &&
            tmp.calcuItem.has_backup_calc_M == 6 && tmp.calcuItem.has_backup_calc_N == 6 && 
            tmp.calcuItem.has_backup_calc_O == 6 && tmp.calcuItem.has_backup_calc_P == 6 &&
            tmp.calcuItem.has_backup_calc_Q == 6 && tmp.calcuItem.has_backup_calc_R == 6 &&
            tmp.calcuItem.has_backup_calc_RT == 6 && tmp.calcuItem.has_backup_fusecalc == 6)
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

bool CheckedTaskList::resetCalcuPos()
{
    mutex.lock();
    QList<CheckedTunnelTaskModel>::iterator it = slavetunnelcheckList->begin();
    while (it != slavetunnelcheckList->end())
    {
        it->calcuItem.has_backup_calc_A = 0;
        it->calcuItem.has_backup_calc_B = 0; 
        it->calcuItem.has_backup_calc_C = 0; 
        it->calcuItem.has_backup_calc_D = 0;
        it->calcuItem.has_backup_calc_E = 0; 
        it->calcuItem.has_backup_calc_F = 0; 
        it->calcuItem.has_backup_calc_G = 0; 
        it->calcuItem.has_backup_calc_H = 0;
        it->calcuItem.has_backup_calc_I = 0; 
        it->calcuItem.has_backup_calc_J = 0; 
        it->calcuItem.has_backup_calc_K = 0; 
        it->calcuItem.has_backup_calc_L = 0;
        it->calcuItem.has_backup_calc_M = 0; 
        it->calcuItem.has_backup_calc_N = 0; 
        it->calcuItem.has_backup_calc_O = 0; 
        it->calcuItem.has_backup_calc_P = 0;
        it->calcuItem.has_backup_calc_Q = 0; 
        it->calcuItem.has_backup_calc_R = 0; 
        it->calcuItem.has_backup_calc_RT = 0; 
        it->calcuItem.has_backup_fusecalc = 0;
        it++;
    }
    mutex.unlock();
    return true;
}

/**
 * 重新设置车厢正反
 * @param tunnelid 隧道ID
 * @param direction true正向 false 反向
 */
bool CheckedTaskList::setTunnelCarriageDirection(int tunnelid, bool direction)
{
    mutex.lock();
    bool ret = false;
    QList<CheckedTunnelTaskModel>::iterator it = slavetunnelcheckList->begin();
    while (it != slavetunnelcheckList->end())
    {
        if (tunnelid == it->planTask.tunnelnum)
        {
            it->planTask.traindirection = direction;
            ret = true;
        }
        it++;
    }
    mutex.unlock();
    return ret;
}