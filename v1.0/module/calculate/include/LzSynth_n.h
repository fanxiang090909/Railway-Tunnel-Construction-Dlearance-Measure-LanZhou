#ifndef LZ_SYNTH_NTUNNELS_H
#define LZ_SYNTH_NTUNNELS_H

#include "clearance_item.h"
#include "tunneldatamodel.h"
#include "checkedtask.h"

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
    bool initSynthesis(std::list<_int64> init_task_clearance_ids);
    /**
     * 综合函数
     */
    void synthesis(ClearanceData& straightdata, ClearanceData& leftdata,
                    ClearanceData& rightdata,
                    int & straightnum, int & leftnum, int & rightnum);

private:
    /**
     * 是否初始化成功
     */
    bool hasinit;

    std::list<_int64> task_clearance_ids;
};

#endif // LZ_SYNTH_NTUNNELS_H
