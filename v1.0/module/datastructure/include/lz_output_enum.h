#ifndef LZ_OUTPUT_EUM_H
#define LZ_OUTPUT_EUM_H

/**
 * 输出限界图形时的底板限界
 * 输出形式 会在限界图中不同。@see \\10.13.29.236\Public Resource\0.科研项目\Lz兰州铁路2012\相关铁路标准\铁路桥隧建筑标准限界
 * DianLi电力牵引，与NeiRan内燃牵引为隧道属性
 * B与D（双层集装箱）为选择属性，可选择B或D输出表格
 */
enum OutputClearanceImageType
{
    OutType_B_NeiRan = 1,  // 隧道建筑限界，内燃牵引
    OutType_B_DianLi = 2,  // 隧道建筑限界，电力牵引
    OutType_D_NeiRan = 3,  // 双层集装箱桥隧建筑限界，内燃牵引
    OutType_D_DianLi = 4,  // 双层集装箱桥隧建筑限界，电力牵引
};

/**
 * 限界表的列头枚举类型
 */
enum LzOutputTableColumn {
    Height = 0,
    Straight_Left_Val = 1,
    Straight_Right_Val = 2,
    Straight_Left_Pos = 3,
    Straight_Right_Pos = 4,
    LeftCurve_Left_Val = 5,
    LeftCurve_Right_Val = 6,
    LeftCurve_Left_Radius = 7,
    LeftCurve_Right_Radius = 8,        
    LeftCurve_Left_Pos = 9,
    LeftCurve_Right_Pos = 10,
    RightCurve_Left_Val = 11,
    RightCurve_Right_Val = 12,
    RightCurve_Left_Radius = 13,
    RightCurve_Right_Radius = 14,        
    RightCurve_Left_Pos = 15,
    RightCurve_Right_Pos = 16,
};


#endif // LZ_OUTPUT_EUM_H