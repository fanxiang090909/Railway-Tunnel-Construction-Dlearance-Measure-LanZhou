/**
 *	clearrance_item.h
 *	兰州隧道限界检测项目
 *
 *	数据类型：断面数据、限界数据
 *              断面数据用于界面绘图显示（图形修正、图表输出）
 *              限界数据用于综合计算（单隧道综合、区段隧道桥梁综合）
 *
 *	作者: FanX
 *      版本: alpha v1.0.0
 *	日期：2014.3.14
 */

#ifndef CLEARANCE_ITEM_H
#define CLEARANCE_ITEM_H

#include <iostream>
#include <string>
#include <map>

using namespace std;
// 声明
class ClearanceData;

/**
 * 最小限界类型枚举
 * 左转、右转曲线、直线最小限界
 * 左转、右转曲线、直线次小限界
 */
enum ClearanceType {
    Straight_Smallest = 0, // 最小限界
    LeftCurve_Smallest = 1,
    RightCurve_Smallest = 2,
    Unknown_Smallest = 3,
    Straight_2ndSmallest = 4, // 次小限界
    LeftCurve_2ndSmallest = 5,
    RightCurve_2ndSmallest = 6,
    Unknown_2ndSmallest = 7
};

enum CurveType {
    Curve_Straight = 0, // 最小限界
    Curve_Left = 1,
    Curve_Right = 2
};

/**
 * 断面数据项
 * 作为SectionData中的map填充对象
 * @see SectionData
 * @author 范翔
 * @version 1.0.0
 * @date 20140325
 */
struct item
{
    float left;
    float right;
};

/**
 * 断面指定高度数据集
 * 包括一个限界数据项map
 * @see item
 * @author 范翔
 * @version 1.0.0
 * @date 20140325
 */
class SectionData
{

public:
    SectionData();

    // 复制构造函数
    SectionData(const SectionData & real);
    // 重载赋值号
    SectionData & operator=(const SectionData &real);
    // 【有问题，item的==号问题】重载==号
    //friend bool operator==(const SectionData &real1, const SectionData &real2);

    /**
     * 左右数据对调，方便限界图形预览（符合人眼感觉）
     */
    void swapLeftAndRight();

    /**
     * 插入map单侧限界值
     * @param height 高度map中的键
     * @param val 限界值
     * @param isleft 该限界值为左值？右值
     */
    bool updateToMapVals(int height, float val, bool isleft);
    
    /**
     * 插入map单侧限界值
     * @param height 高度map中的键
     * @param val 限界值
     * @param isleft 该限界值为左值？右值
     * @param oldval 要返回的原值
     */
    bool updateToMapVals(int height, float val, bool isleft, float & oldval);
   
    /**
     * 插入map两侧限界值
     * @param height 高度map中的键
     * @param leftVal 左限界值
     * @param rightVal 右限界值
     */
    bool updateToMapVals(int height, float leftVal, float rightVal);

    /**
     * 初始化map（长度已知，从配置文件中读出）
     */
    bool initMaps();

    /**
     * 重置maps，不删除map，只把maps中的item.left和item.right清零，记录下一帧的
     */
    bool resetMaps();

    /**
     * 得到maps
     * @return 断面map，键为高度，值为item类型@see item，即左右限界值
     */
    std::map<int, item>& getMaps();

    /**
     * 显示输出map的值
     */
    bool showMaps();

    /**
     * 更新限界值
     * @param clearancedata 当前限界ClearanceData数据指针
     */
    bool updateToClearanceData(ClearanceData & clearancedata);

    /** 
     * 设置该断面所在里程
     */
    void setMile(double newmile);

    /** 
     * 获得该断面所在里程
     */
    double getMile();

    /** 
     * 设置该断面所在曲线半径（if 曲线类型为type）
     */
    void setRadius(float newradius);

    /** 
     * 获得该断面所在曲线半径（if 曲线类型为type）
     */
    float getRadius();
    
    /**
     * 设置中心线净高
     */
    void setCenterHeight(float newcenterheight);

    /**
     * 获得中心线净高
     */
    float getCenterHeight();

    /** 
     * 设置该断面所在曲线类型
     * 直线0，左转曲线-1，右转曲线+1
     */
    void setType(CurveType newtype);
    /** 
     * 获得该断面所在曲线类型
     */
    CurveType getType();

private:

    /**
     * 中心线上方净高
     */
    float centerHeight;

    /**
     * 里程数
     */
    double mileCount;
    /**
     * 相机帧号
     */
    __int64 frameCounter;

    /**
     * 左曲线？右曲线？直线
     */
    CurveType type;
    
    /**
     * 曲线半径（只在type为左曲线和右曲线时使用）
     */
    float curveRadius;

    /**
     * 右边点  左边点 （包含高度信息）
     * 【注意！】左边右边都以标准输出表格的左右为准
     *          即单线下行，与双线非正常方向
     *          面向终点的左边数据点和右边数据点
     */
    std::map<int, item> vals;

    // 是否已经初始化标记
    bool hasinit;
};


/**
 * 限界数据项，比断面数据多位置信息
 * 作为ClearanceData中的map填充对象
 * @see ClearanceData
 * @author 范翔
 * @version 1.0.0
 * @date 20140325
 */
struct ClearanceItem
{
    // 左限界值
    float leftval;
    // 右限界值
    float rightval;

    // 左限界曲线半径（只在type为曲线时使用）
    float leftradius;
    // 右限界曲线半径（只在type为曲线时使用）
    float rightradius;

    // 左限界位置
    double leftpos;
    // 左限界隧道（区段综合时使用）
    int lefttunnelid;
    // 右限界位置
    double rightpos;
    // 右限界隧道（区段综合时使用） 
    int righttunnelid;
};

/**
 * 限界数据
 * 包括一个限界数据项map
 * @see ClearanceItem
 * @author 范翔
 * @version 1.0.0
 * @date 20140325
 */
class ClearanceData
{
public:
    ClearanceData();
    // 复制构造函数
    ClearanceData(const ClearanceData & real);
    // 重载赋值号
    ClearanceData & operator=(const ClearanceData &real);

    /**
     * 插入map单侧限界值
     * @param height 高度map中的键
     * @param val 限界值
     * @param isleft 该限界值为左值？右值
     */
    bool updateToMapVals(int height, float val, double pos, int ttunnelid, float radius, bool isleft);

    /**
     * 插入map两侧限界值
     * @param height 高度map中的键
     * @param leftVal 左限界值
     * @param rightVal 右限界值
     */
    bool updateToMapVals(int height, float leftVal, float rightVal, double leftpos, double rightpos, 
							int lefttunnelid, int righttunnelid, float leftradius, float rightradius);

    /**
     * 初始化map（长度已知，从配置文件中读出）
     */
    bool initMaps();

    /**
     * 重置maps，不删除map，只把maps中的item.left和item.right清零，记录下一帧的
     */
    bool resetMaps();

    /**
     * 得到maps
     * @return 限界数据map，键为高度，值为ClearanceItem类型@see ClearanceItem，即左右限界值，加左右限界位置、半径等信息
     */
    std::map<int, ClearanceItem>& getMaps();

    /**
     * 显示输出map的值
     */
    bool showMaps();

    /**
     * 更新线路中心线上方最低净高
     */
    void updateToMinCenterHeight(float val, double pos);
    void updateToMinCenterHeight(float val, double pos, int tunnelid);

    /**
     * 线路中心线上方最低净高
     */
    float getMinCenterHeight();
    /**
     * 线路中心线上方最低净高
     */
    void setMinCenterHeight(float newval);

    /**
     * 线路中心线上方最低净高所在位置
     */
    double getMinCenterHeightPos();
    /**
     * 线路中心线上方最低净高所在位置
     */
    void setMinCenterHeightPos(double newpos);

    /**
     * 线路中心线上方最低净高所在隧道ID
     */
    int getMinCenterHeightTunnelID();
    /**
     * 线路中心线上方最低净高所在隧道ID
     */
    void setMinCenterHeightTunnelID(int newtunnelid);

    /**
     * 更新限界值
     * @param clearancedata 当前限界ClearanceData数据指针
     */
    bool updateToClearanceData(ClearanceData & clearancedata);

    /**
     * 得到曲线类型
     */
    const ClearanceType getClearanceType() const;

    /**
     * 设置曲线类型
     */
    void setType(ClearanceType curvetype);

private:

    /**
     * 线路中心线上方最低净高
     */
    float minCenterHeight;

    /**
     * 线路中心线上方最低净高所在位置
     */
    double minCenterHeightPos;

    /**
     * 线路中心线上方最低净高所在隧道ID
     */
    int minCenterHeightTunnelID;

    /**
     * 当前隧道号
     */
    int tunnelid;

    /**
     * 当前曲线类型
     */
    ClearanceType clearanceType;

    /**
     * 右边点  左边点 （包含高度信息）
     * 【注意！】左边右边都以标准输出表格的左右为准
     *         即单线下行，与双线非正常方向
     *          面向终点的左边数据点和右边数据点
     */
    std::map<int, ClearanceItem> vals;

    // 是否已经初始化标记
    bool hasinit;
};

#endif // CLEARANCE_ITEM_H
