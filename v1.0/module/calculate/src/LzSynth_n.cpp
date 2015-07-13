#include "LzSynth_n.h"
#include "output_heights_list.h"
#include "daoclearanceoutput.h"
#include <QDebug>

/**
 * 单隧道隧道综合计算类实现
 * @author 范翔
 * @date 2014-3-25
 * @version 1.0.0
 */
LzNTunnelsSynthesis::LzNTunnelsSynthesis()
{
    hasinit = false;
}

LzNTunnelsSynthesis::~LzNTunnelsSynthesis()
{
}

bool LzNTunnelsSynthesis::initSynthesis(std::list<_int64> init_task_clearance_ids)
{
    this->task_clearance_ids = init_task_clearance_ids;

    if (OutputHeightsList::getOutputHeightsListInstance()->getCurrentHeightsVersion() == 0)
    {
        hasinit = false;
        std::cout << "heights init false";
        return false;
    }

    hasinit = true;
    return true;
}

// 综合函数
void LzNTunnelsSynthesis::synthesis(ClearanceData& straightdata, ClearanceData& leftdata,
                    ClearanceData& rightdata,
                    int & straightnum, int & leftnum, int & rightnum)
{
    if (!hasinit)
        return;

    straightnum = 0;
    leftnum = 0;
    rightnum = 0;

    // 初始化时检查需要初始化几个表格
    // 【需求相关】 不管左（右）曲线段数量有多少，始终输出一个左（右）曲线限界表格
    
    // 如果没有左转右转曲线，则一定包含直线段
    straightdata.initMaps();	// 初始化高度，不可缺少
    leftdata.initMaps();	// 初始化高度，不可缺少
    rightdata.initMaps();	// 初始化高度，不可缺少

    qDebug() << "size:" << task_clearance_ids.size();

    // 临时存储当前帧的高度数据
    std::list<_int64>::iterator it = task_clearance_ids.begin();
    while (it != task_clearance_ids.end())
    {
        for (int j = 0; j < 3; j++) // 3个OutputType均试一下
        {
            ClearanceType tmptype;

            switch(j)
            {
                case 0: tmptype = Straight_Smallest; break;
                case 1: tmptype = LeftCurve_Smallest; break;
                case 2: tmptype = RightCurve_Smallest; break;
                default: break;
            }

            ClearanceData tempdata;
            tempdata.initMaps(); // 初始化高度，不可缺少

            /**********读数据库限界值**********/
            bool ret = ClearanceOutputDAO::getClearanceOutputDAOInstance()->DBDataToClearanceData(tempdata, (*it), tmptype);
            qDebug() << "DBDataToClearanceData" << ret << ", clearance_outputid=" << (*it);

            // 临时变量findInterMile()函数引用
            tmptype = tempdata.getClearanceType();

            if (ret)
            {
                switch (tmptype)
                {
                    case Straight_Smallest:	tempdata.updateToClearanceData(straightdata); straightnum++; break;
                    case LeftCurve_Smallest:    tempdata.updateToClearanceData(leftdata); leftnum++; break;
                    case RightCurve_Smallest:   tempdata.updateToClearanceData(rightdata); rightnum++; break;
                    default:qDebug() << "can not find TYPE in task_tunnel_id" << (*it); break;
                }
                //straightdata.showMaps();
            }
            /*********************************/
        }
        it++;
    }
}
