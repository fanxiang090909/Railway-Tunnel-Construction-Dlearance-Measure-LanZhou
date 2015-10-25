#ifndef CLEARANCE_MULTI_TUNNELS_H
#define CLEARANCE_MULTI_TUNNELS_H

#include <QStringListModel>
#include <QObject>
#include "clearance_item.h"
#include "lz_working_enum.h"
#include "lz_output_enum.h"
#include <list>

using namespace std;

/**
 * 多个隧道区段综合结果数据模型
 * @author fanxiang
 * @version 1.0.0
 * @date 2014-6-11
 */
class ClearanceMultiTunnels
{

public:
    /**
     * 构造函数
     */
    ClearanceMultiTunnels();

    /**
     * 析构函数
     */
    ~ClearanceMultiTunnels();
    
    /**
     * 正在编辑修正使用的待填入限界表格，直线部分
     */
    ClearanceData & getClearanceStraightData();
    /**
     * 正在编辑修正使用的待填入限界表格，左转曲线部分
     */
    ClearanceData & getClearanceLeftData();
    /**
     * 正在编辑修正使用的待填入限界表格，右转曲线部分
     */
    ClearanceData & getClearanceRightData();

    /**
     * 初始化上面三个ClearanceData
     */
    void initClearanceDatas(std::list<_int64> inttasktunnelids, QString templatepath, OutputClearanceImageType type = OutType_B_DianLi);

    /**
     * 调用多隧道区段综合计算类
     */
    bool synthesis();

    int getNumOfStraight();
    int getNumOfLeft();
    int getNumOfRight();

    /**
     * 得到超限隧道数量
     */
    int getNumOfOutOfClearanceTunnels();
    int getNumOfOutOfClearanceBridges();

    bool getHasInit();

    /**
     * 包含综合隧道信息的List模型，用于在View中显示
     */
    QStringListModel * getTunnelsNames();

    // 得到区段综合中的隧道数量
    int getTunnelsNum();
    // 得到区段综合中的桥梁数量
    int getBridgesNum();

    /**
     * 格外手动输入到表格项-最大外轨超高
     */
    void setWaiGuiChaoGao(QString newwaiguichaogao) { waiguichaogao = newwaiguichaogao; }
    
    /**
     * 格外手动输入到表格项-最低接触网高度
     */
    void setJieChuWangGaoDu(QString newjiechuwanggaodu) { jiechuwanggaodu = newjiechuwanggaodu; }

    QString getWaiGuiChaoGao() { return waiguichaogao; }

    QString getJieChuWangGaoDu() { return jiechuwanggaodu; }

    QString getLineName();

    QString getEndStationName();

    /**
     * 最小曲线半径
     */
    int getMinRadius();

    /**
     * 最低中心净高
     */
    int getMinHeight();

public:

    std::list<_int64> tasktunnelids;
    
private:
    /**
     * 包含综合隧道信息的List模型，用于在View中显示
     */
    QStringListModel * tunnelsModel;
    bool hasloadnames;

    /**
     * 正在使用的待填入限界表格，直线部分
     */
    ClearanceData straightdata;
    /**
     * 正在使用的待填入限界表格，左转曲线部分
     */
    ClearanceData leftdata;
    /**
     * 正在使用的待填入限界表格，右转曲线部分
     */
    ClearanceData rightdata;

    int numofstraight;
    int numofleft;
    int numofright;

    bool hasinit;

    /**
     * 限界类型，记录超限桥隧数量
     */
    OutputClearanceImageType tunnelsType;
    int numofOutOfClearanceTunnels;
    int numofOutOfClearanceBridges;
    // 超限桥隧限界值模板路径
    QString templatepath;

    /**
     * 格外手动输入到表格项-最大外轨超高
     */
    QString waiguichaogao;
    
    /**
     * 格外手动输入到表格项-最低接触网高度
     */
    QString jiechuwanggaodu;
};

#endif // CLEARANCE_MULTI_TUNNELS_H
