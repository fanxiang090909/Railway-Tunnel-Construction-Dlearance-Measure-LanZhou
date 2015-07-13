#ifndef CLEARANCE_SINGLE_TUNNEL_H
#define CLEARANCE_SINGLE_TUNNEL_H

#include "clearance_item.h"
#include <QObject>

/**
 * 单隧道综合结果数据模型
 * @author fanxiang
 * @version 1.0.0
 * @date 2014-6-11
 */
class ClearanceSingleTunnel
{
public:
    /**
     * 构造函数
     */
    ClearanceSingleTunnel();

    /**
     * 析构函数
     */
    ~ClearanceSingleTunnel();
    
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
    void initClearanceDatas(_int64 inttasktunnelid);

    /**
     * 调用单隧道综合计算类，加载综合计算结果数据
     */
    bool loadsynthesisdata();

    bool getHasStraight();
    bool getHasLeft();
    bool getHasRight();

    bool getHasInit();
    
    /**
     * 得到采集隧道信息
     * @return “隧道名_采集时间”,没有找到返回""
     */
    QString getTaskTunnelInfo();

    /**
     * 格外手动输入到表格项-起讫站名
     */
    void setQiQiZhanMing(QString newqiqizhanming) { qiqizhanming = newqiqizhanming; }
        
    /**
     * 格外手动输入到表格项-最大外轨超高
     */
    void setWaiGuiChaoGao(QString newwaiguichaogao) { waiguichaogao = newwaiguichaogao; }
    
    /**
     * 格外手动输入到表格项-最低接触网高度
     */
    void setJieChuWangGaoDu(QString newjiechuwanggaodu) { jiechuwanggaodu = newjiechuwanggaodu; }

    QString getQiQiZhanMing() { return qiqizhanming; }

    QString getWaiGuiChaoGao() { return waiguichaogao; }

    QString getJieChuWangGaoDu() { return jiechuwanggaodu; }

private:

    _int64 tasktunnelid;

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

    bool hasstraight;
    bool hasleft;
    bool hasright;

    bool hasinit;

    // 手动输入项初值 起讫站名，最大外轨超高，最低接触网高度
    /**
     * 格外手动输入到表格项-起讫站名
     */
    QString qiqizhanming;

    /**
     * 格外手动输入到表格项-最大外轨超高
     */
    QString waiguichaogao;
    
    /**
     * 格外手动输入到表格项-最低接触网高度
     */
    QString jiechuwanggaodu;
};

#endif // CLEARANCE_SINGLE_TUNNEL_H
