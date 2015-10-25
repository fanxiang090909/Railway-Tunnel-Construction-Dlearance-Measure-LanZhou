#ifndef LZ_SYNTH_NTUNNELS_H
#define LZ_SYNTH_NTUNNELS_H

#include "clearance_item.h"
#include "tunneldatamodel.h"
#include "checkedtask.h"
#include "lz_output_enum.h"

/**
 * 单隧道隧道综合计算类声明
 * @author 范翔
 * @date 2014-3-25
 * @version 1.0.0
 */
class LzNTunnelsSynthesis
{
public:
    /**
     * 构造函数
     */
    LzNTunnelsSynthesis();

    ~LzNTunnelsSynthesis();
    
    /**
     * 隧道综合初始化
     */
    bool initSynthesis(std::list<_int64> init_task_clearance_ids, std::string templatepath, OutputClearanceImageType type = OutputClearanceImageType::OutType_B_DianLi);

    /**
     * 综合函数
     * @param straightnum 输出值 直线数量
     * @param leftnum 输出值 左转曲线数量
     * @param rightnum 输出值 右转曲线数量
     * @param numofOutOfClearance 输出值 超限桥隧数量
     */
    void synthesis(ClearanceData& straightdata, ClearanceData& leftdata,
                    ClearanceData& rightdata,
                    int & straightnum, int & leftnum, int & rightnum, int & numofOutOfClearanceTunnels, int & numofOutOfClearanceBridges);

    /**
     * 加载基准限界数据
     */
    bool loadBaseClearanceTemplateData(OutputClearanceImageType type);

private:
    /**
     * 是否初始化成功
     */
    bool hasinit;

    std::list<_int64> task_clearance_ids;

    std::string templatepath;

    OutputClearanceImageType outputtype;

    /**
     * 基准限界尺寸
     */
    SectionData baseClearandeData;
    // 是否初始化基础限界尺寸
    bool hasinitBaseClearanceData;
};

#endif // LZ_SYNTH_NTUNNELS_H
