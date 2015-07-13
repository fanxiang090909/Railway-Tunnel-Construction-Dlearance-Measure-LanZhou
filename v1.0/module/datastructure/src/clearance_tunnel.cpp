﻿#include "clearance_tunnel.h"

#include "daoclearanceoutput.h"
#include "daotasktunnel.h"

/**
 * 单隧道综合结果数据模型
 * @author fanxiang
 * @version 1.0.0
 * @date 2014-6-11
 */
ClearanceSingleTunnel::ClearanceSingleTunnel() 
{
    hasinit = false;
}

ClearanceSingleTunnel::~ClearanceSingleTunnel() {}

void ClearanceSingleTunnel::initClearanceDatas(_int64 inttasktunnelid)
{
    // 限界数据初始化
    straightdata.initMaps();
    leftdata.initMaps();
    rightdata.initMaps();

    tasktunnelid = inttasktunnelid;
    hasinit = true;
}

/**
 * 正在使用的待填入限界表格，直线部分
 */
ClearanceData & ClearanceSingleTunnel::getClearanceStraightData() { return straightdata; }
/**
 * 正在使用的待填入限界表格，左转曲线部分
 */
ClearanceData & ClearanceSingleTunnel::getClearanceLeftData() { return leftdata; }
/**
 * 正在使用的待填入限界表格，右转曲线部分
 */
ClearanceData & ClearanceSingleTunnel::getClearanceRightData() { return rightdata; }

/**
 * 调用单隧道综合计算类
 */
bool ClearanceSingleTunnel::loadsynthesisdata()
{
    if (!hasinit)
        return false;

    hasstraight = ClearanceOutputDAO::getClearanceOutputDAOInstance()->DBDataToClearanceData(straightdata, tasktunnelid, ClearanceType::Straight_Smallest);
    hasleft = ClearanceOutputDAO::getClearanceOutputDAOInstance()->DBDataToClearanceData(leftdata, tasktunnelid, ClearanceType::LeftCurve_Smallest);
    hasright = ClearanceOutputDAO::getClearanceOutputDAOInstance()->DBDataToClearanceData(rightdata, tasktunnelid, ClearanceType::RightCurve_Smallest);

    straightdata.showMaps();
    leftdata.showMaps();
    rightdata.showMaps();

    if (!hasstraight && !hasleft && !hasright)
        return false;
    else
        return true;
}

bool ClearanceSingleTunnel::getHasStraight() { return hasstraight; }
bool ClearanceSingleTunnel::getHasLeft() { return hasleft; }
bool ClearanceSingleTunnel::getHasRight() { return hasright; }

bool ClearanceSingleTunnel::getHasInit() { return hasinit; }

/**
 * 得到采集隧道信息
 * @return “隧道名_采集时间”,没有找到返回""
 */
QString ClearanceSingleTunnel::getTaskTunnelInfo()
{
    if (!hasinit)
        return "";
    QString tunnelname, date;
    int tunnelid;
    TaskTunnelDAO::getTaskTunnelDAOInstance()->getTaskTunnelInfo(tasktunnelid, tunnelid, tunnelname, date);
    return tunnelname + "_" + date;
}

/**
 * 最小曲线半径
 */
int ClearanceSingleTunnel::getMinRadius()
{
    return 0;
}

/**
 * 最低中心净高
 */
int ClearanceSingleTunnel::getMinHeight()
{
    int minheight = -1;
    if (hasstraight)
    {
        if (minheight < 0 || (straightdata.getMinCenterHeight() > 0 && straightdata.getMinCenterHeight() < minheight ))
            minheight = straightdata.getMinCenterHeight();
    }
    if (hasleft)
    {
        if (minheight < 0 || (leftdata.getMinCenterHeight() > 0 && leftdata.getMinCenterHeight() < minheight ))
            minheight = leftdata.getMinCenterHeight();
    }
    if (hasright)
    {
        if (minheight < 0 || (rightdata.getMinCenterHeight() > 0 && rightdata.getMinCenterHeight() < minheight ))
            minheight = rightdata.getMinCenterHeight();
    }

    return minheight;
}