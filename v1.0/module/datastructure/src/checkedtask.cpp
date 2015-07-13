#include "checkedtask.h"

#include <QDebug>

using namespace std;

/**
 * 生成装载实际与计划隧道校正文件的内存
 * @author xiongxue
 * @author 范翔
 * @date 2014.1.16
 */
CheckedTunnelTaskModel::CheckedTunnelTaskModel()
{
    realList = new std::list<CheckedItem>();
}

// 复制构造函数
CheckedTunnelTaskModel::CheckedTunnelTaskModel(const CheckedTunnelTaskModel & real)
{
    this->planTask = real.planTask;
    this->calcuItem = real.calcuItem;
    this->realList = real.realList;
    this->realList = new std::list<CheckedItem>();
    std::list<CheckedItem> * tmplist = real.realList;
    std::list<CheckedItem>::iterator it = tmplist->begin();
    while ( it != tmplist->end())
    {
        CheckedItem tmp = (*it);
        this->realList->push_back(tmp);
        it++;
    }
};

// 重载赋值号
CheckedTunnelTaskModel & CheckedTunnelTaskModel::operator=(const CheckedTunnelTaskModel &real)
{
    if (this == &real)
        return *this;

    this->planTask = real.planTask;
    this->calcuItem = real.calcuItem;
    this->realList = real.realList;
    this->realList = new std::list<CheckedItem>();
    std::list<CheckedItem> * tmplist = real.realList;
    std::list<CheckedItem>::iterator it = tmplist->begin();
    while ( it != tmplist->end())
    {
        CheckedItem tmp = (*it);
        this->realList->push_back(tmp);
        it++;
    }
    return *this;
};

// 友元函数不用写“CheckedTunnelTaskModel::”
bool operator==(const CheckedTunnelTaskModel &real1, const CheckedTunnelTaskModel &real2)
{
    if (real1.planTask.tunnelnum == real2.planTask.tunnelnum && real1.planTask.datetime == real2.planTask.datetime &&
        real1.calcuItem.cal_filename_prefix == real2.calcuItem.cal_filename_prefix)
        return true;
    else
        return false;
};

CheckedTunnelTaskModel::~CheckedTunnelTaskModel()
{
    //qDebug() << "before delete";
    //showCheckedTaskModel();
    std::list<CheckedItem>::iterator it = realList->begin();
    while (it != realList->end())
    {
        it = realList->erase(it);
    }
    //qDebug() << "after delete";
    //showCheckedTaskModel();
    delete realList;
}

void CheckedTunnelTaskModel::setPlanTask(PlanTask ptask) { this->planTask = ptask; }
void CheckedTunnelTaskModel::setCalcuItem(CalcuFileItem citem) { this->calcuItem = citem; }

std::list<CheckedItem> *CheckedTunnelTaskModel::getRealList() { return realList; }
void CheckedTunnelTaskModel::pushback(CheckedItem& real) { realList->push_back(real); }
void CheckedTunnelTaskModel::insertAt(int i, CheckedItem& real)
{
    std::list<CheckedItem>::iterator it = begin();
    int pos = 0;
    while (it != end() && pos < i)
    {
        it++;
        pos++;
    }
    realList->insert(it, real);
}

void CheckedTunnelTaskModel::remove(CheckedItem& real) {realList->remove(real);}
void CheckedTunnelTaskModel::removeAt(int i)
{
    std::list<CheckedItem>::iterator it = begin();
    int pos = 0;
    while (it != end() && pos < i)
    {
        it++;
        pos++;
    }
    it = realList->erase(it);
}

/**
 * qDebug查看对象
 */
void CheckedTunnelTaskModel::showCheckedTaskModel()
{
    qDebug() << "*****showCheckedTaskModel***** plan:" << planTask.tunnelnum << QString::fromLocal8Bit(planTask.tunnelname.c_str()) << getRealList()->size();
    _int64 checkedstart;
    _int64 checkedend;
    std::list<CheckedItem>::iterator it = getRealList()->begin();
    if (it != getRealList()->end())
    {
        checkedstart = it->start_frame_A1;
        checkedend = it->end_frame_A1;

        while (it != getRealList()->end())
        {
            qDebug() << "\t\t" << it->seqno << it->tunnel_id << QString::fromLocal8Bit(it->tunnel_name.c_str()) << checkedstart << checkedend;

            if (it->start_frame_A1 < checkedstart)
                checkedstart = it->start_frame_A1;
            if (it->end_frame_A1 > checkedend)
                checkedend = it->end_frame_A1;
            it++;
        }
    }
}

std::list<CheckedItem>::iterator CheckedTunnelTaskModel::begin() { return realList->begin(); }
std::list<CheckedItem>::iterator CheckedTunnelTaskModel::end() { return realList->end(); }
std::list<CheckedItem>::reverse_iterator CheckedTunnelTaskModel::rbegin() { return realList->rbegin(); }
std::list<CheckedItem>::reverse_iterator CheckedTunnelTaskModel::rend() { return realList->rend(); }