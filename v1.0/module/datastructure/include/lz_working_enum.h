#ifndef LZ_WORKING_EUM_H
#define LZ_WORKING_EUM_H

/**
 * 文件类型
 */
enum FileType {
    File_RAW_valid = 0,
    File_CalcuData_temp = 1,
    File_CalcuData_result = 2
};

/**
 * 工作状态枚举类型
 */
enum WorkingStatus
{
    Preparing = 0,
    Collecting = 1,
    Correcting = 2,            // 校正
    Calculating = 3,
    Backuping = 4,
    Calculating_Backuping = 5, // 计算备份同时进行
    Deleting = 6,              // 删除从控存储空间
};

/**
 * 主控采集状态枚举类型
 */
enum MasterCollectingStatus
{
    Collecting_Ready = 0,
    Collecting_Wrong = 1,
    Collecting_InTunnelValid = 2,   // 预进洞有效
    Collecting_InTunnelNotValid = 3,// 预进洞无效
    Collecting_OutfromTunnel = 4,   // 出洞
};

/**
 * 每个硬件的工作状态
 */
enum HardwareStatus
{
    Hardware_Off = 0,          // 正常关闭（灰色）
    Hardware_OnButFree = 1,    // 正常连接，但未工作（黄色）
    Hardware_Working = 2,      // 正在工作（绿色）
    Hardware_ExceptionOff = 3, // 异常关闭（红色）
    Hardware_NotDetect = 4,    // 检测不到（初始化时）（红色）
    Hardware_Broken = 5        // （红色）
};

/**
 * 硬件类型
 */
enum HardwareType
{
    Slave_Computer_Type = 0,
    Camera_Type = 1,
    Laser_Type = 2
};

/**
 * 兰州相机采集模式
 */
enum LzCameraCollectingMode
{
    Lz_Camera_Continus = 0,
    Lz_Camera_HardwareTrigger = 1,
	Lz_Camera_SoftwareTrigger = 2
};

/**
 * 兰州外触发模式下
 * 采集帧间隔 0.5m 0.75m 1m 1.25m 一帧
 */
enum LzCollectHardwareTriggerDistanceMode 
{
    Lz_HardwareTrigger_500mm = 0,           // default
    Lz_HardwareTrigger_750mm = 1,
    Lz_HardwareTrigger_1000mm = 2,            
    Lz_HardwareTrigger_1250mm = 3
};

/**
 * 集成里程、进出洞检测装置后和郑老师定义的采集模式 手动触发模式 自动检测模式 自由触发模式
 * @author 范翔
 * @date 20150625
 */
enum LzCollectingMode
{
    Lz_Collecting_Manual_DistanceMode = 0,    // 手动触发模式   --default
    Lz_Collecting_Automatic_DistanceMode = 1, // 自动检测模式
    Lz_Collecting_FreeMode_NoDistanceMode = 2 // 自由触发模式
};

/**
 * 兰州自由触发模式下
 * 采集帧率 30Hz、40Hz、50Hz、60Hz
 */
enum LzCollectHardwareTriggerNoDistanceMode 
{
    Lz_Collecting_FreeMode_30Hz = 0,          // --default
    Lz_Collecting_FreeMode_50Hz = 1,
    Lz_Collecting_FreeMode_40Hz = 2,            
    Lz_Collecting_FreeMode_60Hz = 3
};


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

#endif // LZ_WORKING_EUM_H