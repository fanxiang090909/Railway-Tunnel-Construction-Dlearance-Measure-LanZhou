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
ClearanceMultiTunnels::ClearanceMultiTunnels() 
{
    hasinit = false;
    hasloadnames = false;
    tunnelsModel = new QStringListModel();
}

ClearanceMultiTunnels::~ClearanceMultiTunnels() 
{
    delete tunnelsModel;
}

void ClearanceMultiTunnels::initClearanceDatas(std::list<_int64> inittasktunnelids)
{
    // 限界数据初始化
    straightdata.initMaps();
    leftdata.initMaps();
    rightdata.initMaps();

    tasktunnelids = inittasktunnelids;
    hasinit = true;
}

/**
 * 调用多隧道区段综合计算类
 */
bool ClearanceMultiTunnels::synthesis()
{
    LzNTunnelsSynthesis ntunnelsSynthesis;
    ntunnelsSynthesis.initSynthesis(tasktunnelids);
    ntunnelsSynthesis.synthesis(straightdata, leftdata, rightdata, numofstraight, numofleft, numofright);
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

bool ClearanceMultiTunnels::getHasInit() { return hasinit; }

/**
 * 包含综合隧道信息的List模型，用于在View中显示
 */
QStringListModel * ClearanceMultiTunnels::getTunnelsNames()
{
    // 如果已经加载
    if (hasloadnames)
        return tunnelsModel;

    QString tmptunnelname;
    QString tmpdate;
    int tmptunnelid;
    QStringList tmplist;
    std::list<_int64>::iterator it = tasktunnelids.begin();
    bool ret;
    while (it != tasktunnelids.end())
    {
        ret = TaskTunnelDAO::getTaskTunnelDAOInstance()->getTaskTunnelInfo((*it), tmptunnelid, tmptunnelname, tmpdate);
        if (!ret)
            return NULL;
        tmplist << QObject::tr("%1_%2").arg(tmptunnelname).arg(tmpdate);
        it++;
    }
    tunnelsModel->setStringList(tmplist);
    hasloadnames = true;
    return tunnelsModel;
}

QString ClearanceMultiTunnels::getLineName()
{
    // 如果已经加载
    int lasttunnelid = -1;
    QString linename = "";
    if (hasloadnames)
    {
        if (tunnelsModel->rowCount() > 0)
        {
            QString tunnelname = tunnelsModel->data(tunnelsModel->index(0, 0), Tunnel_name_std).toString();
            int lineid = tunnelsModel->data(tunnelsModel->index(0, 0), Tunnel_line_ID).toInt();
            if (lasttunnelid != lineid)
            {
                lasttunnelid = lineid;
            
                //QString linename = LineDAO::getLineDAOInstance()->getLineName(lineid);
            }
            //qDebug() << tunnelname << TunnelDAO::getTunnelDAOInstance()->get
        }
    }
    return "";
}

QString ClearanceMultiTunnels::getEndStationName()
{
    // 如果已经加载
    if (tunnelsModel->rowCount() > 0)
    {
        QString tunnelname = tunnelsModel->data(tunnelsModel->index(0, 0), Tunnel_name_std).toString();
        int lineid = tunnelsModel->data(tunnelsModel->index(0, 0), Tunnel_line_ID).toInt();

    }
    return "";
}


/**
 * 最小曲线半径
 */
int ClearanceMultiTunnels::getMinRadius()
{
    return 0;
}

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