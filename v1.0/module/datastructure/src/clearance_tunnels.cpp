#include "clearance_tunnels.h"

#include "LzSynth_n.h"
#include "daoline.h"
#include "daotunnel.h"
#include "daotasktunnel.h"

/**
 * 多个隧道区段综合结果数据模型
 * @author fanxiang
 * @version 1.0.0
 * @date 2014-6-11
 */
ClearanceMultiTunnels::ClearanceMultiTunnels() : tunnelsType(OutputClearanceImageType::OutType_B_DianLi)
{
    hasinit = false;
    hasloadnames = false;
    tunnelsModel = new QStringListModel();

    // 超限隧道类型及数量初始为0
    templatepath = "";
    tunnelsType = OutputClearanceImageType::OutType_B_DianLi;
    numofOutOfClearanceTunnels = 0;
    numofOutOfClearanceBridges = 0;

    tasktunnelids = std::list<_int64>();
}

ClearanceMultiTunnels::~ClearanceMultiTunnels() 
{
    delete tunnelsModel;
    tasktunnelids.clear();
}

void ClearanceMultiTunnels::initClearanceDatas(std::list<_int64> inittasktunnelids,  QString inittemplatepath, OutputClearanceImageType type)
{
    // 限界数据初始化
    straightdata.initMaps();
    leftdata.initMaps();
    rightdata.initMaps();

    tasktunnelids = inittasktunnelids;

    // 超限隧道类型及数量初始为0
    templatepath = inittemplatepath;
    tunnelsType = type;
    numofOutOfClearanceTunnels = 0;
    numofOutOfClearanceBridges = 0;

    hasinit = true;
}

/**
 * 调用多隧道区段综合计算类
 */
bool ClearanceMultiTunnels::synthesis()
{
    LzNTunnelsSynthesis ntunnelsSynthesis;
    ntunnelsSynthesis.initSynthesis(tasktunnelids, templatepath.toLocal8Bit().constData(), tunnelsType);
    ntunnelsSynthesis.synthesis(straightdata, leftdata, rightdata, numofstraight, numofleft, numofright, numofOutOfClearanceTunnels, numofOutOfClearanceBridges);
    if (numofleft == 0 && numofstraight == 0 && numofright == 0)
        return false;
    else
        return true;
}

/**
 * 正在使用的待填入限界表格，直线部分
 */
ClearanceData & ClearanceMultiTunnels::getClearanceStraightData() { return straightdata; }
/**
 * 正在使用的待填入限界表格，左转曲线部分
 */
ClearanceData & ClearanceMultiTunnels::getClearanceLeftData() { return leftdata; }
/**
 * 正在使用的待填入限界表格，右转曲线部分
 */
ClearanceData & ClearanceMultiTunnels::getClearanceRightData() { return rightdata; }

int ClearanceMultiTunnels::getNumOfStraight() { return numofstraight; }
int ClearanceMultiTunnels::getNumOfLeft() { return numofleft; }
int ClearanceMultiTunnels::getNumOfRight() { return numofright; }

/**
 * 得到超限隧道数量
 */
int ClearanceMultiTunnels::getNumOfOutOfClearanceTunnels() { return numofOutOfClearanceTunnels; }
int ClearanceMultiTunnels::getNumOfOutOfClearanceBridges() { return numofOutOfClearanceBridges; }

bool ClearanceMultiTunnels::getHasInit() { return hasinit; }

/**
 * 包含综合隧道信息的List模型，用于在View中显示
 */
QStringListModel * ClearanceMultiTunnels::getTunnelsNames()
{
    // 如果已经加载
    //if (hasloadnames)
    //    return tunnelsModel;

    QString tmptunnelname;
    QString tmpdate;
    QString tmplinename;
    int tmptunnelid;
    QStringList tmplist;
    std::list<_int64>::iterator it = tasktunnelids.begin();
    bool ret;
    while (it != tasktunnelids.end())
    {
        ret = TaskTunnelDAO::getTaskTunnelDAOInstance()->getTaskTunnelInfo((*it), tmptunnelid, tmptunnelname, tmpdate, tmplinename);
        if (!ret)
        {
            tunnelsModel->setStringList(tmplist);
            hasloadnames = true;
            return tunnelsModel;
        }
        tmplist << QObject::tr("%1-%2-%3").arg(tmplinename).arg(tmptunnelname).arg(tmpdate);
        it++;
    }
    tunnelsModel->setStringList(tmplist);
    hasloadnames = true;
    return tunnelsModel;
}

// 得到区段综合中的隧道数量
int ClearanceMultiTunnels::getTunnelsNum()
{
    QString tmptunnelname;
    QString tmpdate;
    QString tmplinename;
    int tmptunnelid;
    std::list<_int64>::iterator it = tasktunnelids.begin();
    bool ret;
    int times = 0;
    while (it != tasktunnelids.end())
    {
        ret = TaskTunnelDAO::getTaskTunnelDAOInstance()->getTaskTunnelInfo((*it), tmptunnelid, tmptunnelname, tmpdate, tmplinename);
        if (ret)
        {
            bool ret1 = TunnelDAO::getTunnelDAOInstance()->getTunnelIsBriage(tmptunnelid);
            if (!ret1)
                times++;
        }
        it++;
    }
    return times;
}

// 得到区段综合中的桥梁数量
int ClearanceMultiTunnels::getBridgesNum()
{
    QString tmptunnelname;
    QString tmpdate;
    QString tmplinename;
    int tmptunnelid;
    std::list<_int64>::iterator it = tasktunnelids.begin();
    bool ret;
    int times = 0;
    while (it != tasktunnelids.end())
    {
        ret = TaskTunnelDAO::getTaskTunnelDAOInstance()->getTaskTunnelInfo((*it), tmptunnelid, tmptunnelname, tmpdate, tmplinename);
        if (ret)
        {
            bool ret1 = TunnelDAO::getTunnelDAOInstance()->getTunnelIsBriage(tmptunnelid);
            if (!ret1)
                times++;
        }
        it++;
    }
    return times;
}

QString ClearanceMultiTunnels::getLineName()
{
    QString tmptunnelname;
    QString tmpdate;
    QString tmplinename;
    QString lastlinename = "";
    int tmptunnelid;
    QString retname = "";
    std::list<_int64>::iterator it = tasktunnelids.begin();
    bool ret;
    while (it != tasktunnelids.end())
    {
        ret = TaskTunnelDAO::getTaskTunnelDAOInstance()->getTaskTunnelInfo((*it), tmptunnelid, tmptunnelname, tmpdate, tmplinename);
        if (ret)
        {
            if (lastlinename.compare("") == 0)
            {
                retname += tmplinename;
                lastlinename = tmplinename;
            }
            else if (lastlinename.compare(tmplinename) != 0)
            {
                retname += "-" + tmplinename;
                lastlinename = tmplinename;
            }
        }
        it++;
    }

    return retname;
}

QString ClearanceMultiTunnels::getEndStationName()
{
    // 如果已经加载
    if (tunnelsModel->rowCount() > 0)
    {
        QString tunnelname = tunnelsModel->data(tunnelsModel->index(0, 0), Tunnel_name_std).toString();
        int lineid = tunnelsModel->data(tunnelsModel->index(0, 0), Tunnel_line_ID).toInt();
        QString name = LineDAO::getLineDAOInstance()->getEndStationName(lineid);
        return name;
    }
    return "";
}

/**
 * 最小曲线半径
 */
int ClearanceMultiTunnels::getMinRadius()
{
    if (!hasinit)
        return 0;

    // 没有曲线的情况
    if (numofleft == 0 && numofright == 0)
        return 0;

    int minRadius = 9999999; // 最大半径
    if (numofleft > 0)
    {
        int tmp = leftdata.getMinRadius();
        if (tmp < minRadius && tmp > 0)
            minRadius = tmp;
    }

    if (numofright > 0)
    {
        int tmp = rightdata.getMinRadius();
        if (tmp < minRadius && tmp > 0)
            minRadius = tmp;
    }

    if (minRadius < 0 || minRadius >= 9999999)
        return 0;

    return minRadius;}

/**
 * 最低中心净高
 */
int ClearanceMultiTunnels::getMinHeight()
{
    int minheight = -1;
    if (numofstraight)
    {
        if (minheight < 0 || (straightdata.getMinCenterHeight() > 0 && straightdata.getMinCenterHeight() < minheight ))
            minheight = straightdata.getMinCenterHeight();
    }
    if (numofleft)
    {
        if (minheight < 0 || (leftdata.getMinCenterHeight() > 0 && leftdata.getMinCenterHeight() < minheight ))
            minheight = leftdata.getMinCenterHeight();
    }
    if (numofright)
    {
        if (minheight < 0 || (rightdata.getMinCenterHeight() > 0 && rightdata.getMinCenterHeight() < minheight ))
            minheight = rightdata.getMinCenterHeight();
    }


    return minheight;
}