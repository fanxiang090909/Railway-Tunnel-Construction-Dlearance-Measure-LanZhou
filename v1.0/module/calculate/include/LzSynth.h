#ifndef LZ_SYNTH_H
#define LZ_SYNTH_H

#include "clearance_item.h"
#include "tunneldatamodel.h"
#include "checkedtask.h"

#include "LzReduction.h"
#include <QObject>

using namespace std;

/**
 * 单隧道隧道综合计算类声明
 * 调用步骤
 * （1）调用initSynthesis
 * （2）调用synthesis
 * @author 范翔
 * @date 2014-5-30
 * @version 1.0.0
 */
class LzSynthesis: public QObject
{
    Q_OBJECT

public:
    // 构造函数
    explicit LzSynthesis(QObject *object = 0);

    ~LzSynthesis();
    
    /**
     * 隧道综合初始化，设置两个结构，检查输出高度配置文件（output_heights.xml）是否加载
     * @param initialTunnelheight_syn_name 输入隧道提高度结果文件名（含路径）
     * @param initialTunnelData 输入数据库中查出的该隧道模型, 包含有无左曲线、有曲线、直线及其曲线里程起止信息、 @see TunnelDataModel
     * @param initialCheckedTunnelModel 输入的校对后的隧道信息模型，包含对应的计划隧道信息，实际采集的“隧道”文件信息，可能不只一条（误触发多采集） @see CheckedTunnelTaskModel
	 * @param leftrightvalid 0代表都有效，1代表左侧有效，2代表右侧有效，要配合newCarriageDirection来看左侧右侧
     * @return true 检查输出高度配置文件（output_heights.xml）是否加载
     */
    bool initSynthesis(string initialTunnelheight_syn_name, TunnelDataModel * initialTunnelData,
                       CheckedTunnelTaskModel * initialCheckedTunnelModel, float framedistance, bool newCarriageDirection, long long startframeno = -1, long long endframeno = -1, int leftrightvalid = 0);
    /**
     * 综合函数
     * @param straightdata 返回值：直线段断面数据
     * @param leftdata 返回值：左曲线线段断面数据
     * @param rightdata 返回值：右曲线线段断面数据
     * @param hasstraight 返回值：是否含有直线断面
     * @param hasleft 返回值：是否含有左曲线线段断面
     * @param hasright 返回值：是否含有右曲线线段断面
     */
    void synthesis(ClearanceData& straightdata, ClearanceData& leftdata,
                    ClearanceData& rightdata,
                    bool & hasstraight, bool & hasleft, bool & hasright);

signals:
    void initfc(long long startfc, long long endfc);
    void currentfc(long long currentfc);

private:
    /**
     * 是否初始化成功
     */

    bool hasinit;

    /**
     * 车厢正反
     */
    bool carriageDireciton;

	/**
	 * 当前计算中使用的起始帧终止帧
	 */
	long long startframeno;
	long long endframeno;

	int leftrightvalid;



	//@zengwang 2015年10月7号添加
	//isAccord用来标记采集方向与出表方向是否一致，当isAccord为0时，表示一致，仅出左边数据，当isAccord为1时，表示不一致，出右边数据
	int isAccord;



    /**
     * 帧间隔
     */
    float currentFrameDistance;

    /**
     * 隧道提高度结果文件名（含路径）
     */
    string tunnelheight_syn_name;

    /**
     * 需要的这条隧道的数据库模型（含有几条曲线等信息）
     */
    TunnelDataModel * currentTunnelData;

    /**
     * 实际采集计算得到的融合结果文件位置信息，帧数-里程等
     */
    CheckedTunnelTaskModel * currentCheckedTunnelModel;
};

#endif // LZ_SYNTH_H
