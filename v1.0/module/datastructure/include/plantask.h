#ifndef PLANTASK_H
#define PLANTASK_H

#include <string>
#include <list>

using namespace std;

/**
 * 计划任务信息结构体
 * @see PlanTaskList PlanTaskList的成员
 * @author 熊雪 范翔
 * @date 20140120
 * @version 1.0.0
 */
class PlanTask
{
public:
    PlanTask() {
        this->linenum = -1;
        this->linename = "";
        this->tunnelnum = -1;
        this->tunnelname = "";
        this->startdistance = -1;
        this->enddistance = -1;
        this->traindirection = true;
        this->newline = true;
        this->linetype = 0;
        this->doubleline = true;
        this->downstream = true;
        this->pulsepermeter = -1;
        this->isnormal = true;
        this->datetime = "";
    }
    // 复制构造函数
    PlanTask(const PlanTask & real)
    {
        this->linenum = real.linenum;
        this->linename = real.linename;
        this->tunnelnum = real.tunnelnum;
        this->tunnelname = real.tunnelname;
        this->startdistance = real.startdistance;
        this->enddistance = real.enddistance;
        this->traindirection = real.traindirection;
        this->newline = real.newline;
        this->linetype = real.linetype;
        this->doubleline = real.doubleline;
        this->downstream = real.downstream;
        this->pulsepermeter = real.pulsepermeter;
        this->isnormal = real.isnormal;
        this->datetime = real.datetime;
    }
    // 重载赋值号
    PlanTask & operator=(const PlanTask &real)
    {
        if (this == &real)
            return *this;
        this->linenum = real.linenum;
        this->linename = real.linename;
        this->tunnelnum = real.tunnelnum;
        this->tunnelname = real.tunnelname;
        this->startdistance = real.startdistance;
        this->enddistance = real.enddistance;
        this->traindirection = real.traindirection;
        this->newline = real.newline;
        this->linetype = real.linetype;
        this->doubleline = real.doubleline;
        this->downstream = real.downstream;
        this->pulsepermeter = real.pulsepermeter;
        this->isnormal = real.isnormal;
        this->datetime = real.datetime;
        return *this;
    }
    // 重载==号
    friend bool operator==(const PlanTask &real1, const PlanTask &real2)
    {
        if (real1.linenum == real2.linenum && real1.tunnelnum == real2.tunnelnum && real1.datetime.compare(real2.datetime) == 0)
            return true;
        else
            return false;
    }

    int linenum;        //线路序号
    string linename;    //线路名称
    int tunnelnum;      //隧道编号
    string tunnelname;  //隧道名称
    _int64 startdistance; //起始里程
    _int64 enddistance;   //终止里程
    bool traindirection;//车厢方向
    int newline;        //是否为新线
    int linetype;       //线路类型
    bool doubleline;    //是否为双线
    bool downstream;    //是否为下行
    double pulsepermeter;  //每米脉冲数
    bool isnormal;      //是否正常
    string datetime;    //采集日期
};


#endif // PLANTASK_H
