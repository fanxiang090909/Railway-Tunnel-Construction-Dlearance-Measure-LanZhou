/**
 * 兰州项目综合数据后处理——限界值折减
 * 参考文档 \\10.13.29.236\Project Directory\Lz兰州铁路2012\相关铁路标准\铁路建筑限界测量和数据格式-标准版
 *					3.7.4 以轨面为基准的折减公式
 *
 * @author 范翔
 * @version 1.0.0
 * @date 2014-06-12
 */
#ifndef LZ_REDUCTION_H
#define LZ_REDUCTION_H

#include <QDebug>
#include "clearance_item.h"

/**
 * 折减
 * 以轨面为测量基准时，曲线内侧建筑限界折减按 X = B - 40500 / R 计算
 *                     曲线外侧建筑限界折减按 X = B - 44000 / R 计算
 * 该折减不同于以大地为测量基准的折减，详见\\10.13.29.236\Project Directory\Lz兰州铁路2012\相关铁路标准\《铁路建筑限界测量和数据格式-标准版》-3.7.4
 * 式中： X 为折减后建筑限界半宽，单位为毫米（mm）
 *        B 为实测建筑限界半宽，单位为毫米（mm）
 *        R 为曲线半径，单位为米（m）
 *
 * @param curvetype 曲线类型，直线，左转曲线，右转曲线
 * @param val 输入公式中的B，输出折减结果公式中的X
 * @param radius 公式中的R
 * @param isinside 是否是内侧
 * @return true 折减成功
 * @author 范翔
 * @date 2014-4-6
 */
static bool reduction(CurveType curvetype, float & val, int radius, bool isinside)
{
    if (curvetype == CurveType::Curve_Straight)
        return false;

    if (isinside) // X = B - 40500 / R
	    val = val -(40500 * 1.0 / radius);
	else // X = B - 44000 / R
	    val = val -(44000 * 1.0 / radius);
    return true;
}

/**
 * 隧道断面折减
 * @param curvetype 曲线类型，直线0，左转曲线-1，右转曲线+1
 * @param data 输入的隧道断面数据
 * @param radius 曲线半径
 * @param carriagedirection 车厢正反
 * @return true 折减成功
 * @author 范翔
 * @date 2014-6-12
 */
static bool clearanceReduction(SectionData & data, bool carriagedirection)
{
    bool leftisinside;
    // 如果车厢为正，又是左曲线，则data左侧为内侧，data右侧为外侧
    CurveType type = data.getType();
    int radius = data.getRadius();
    if ((type == Curve_Left && carriagedirection) || (type == Curve_Right && !carriagedirection))
        leftisinside = true;
    // 如果车厢为反，又是左转曲线，则data左侧为外侧，data右侧为内侧
    else if ((type == Curve_Left && !carriagedirection) || (type == Curve_Right && carriagedirection)) 
        leftisinside = false;
    else
        return false;

    std::map<int,item>::iterator it = data.getMaps().begin();
    int tempkey;
    while (it != data.getMaps().end())
    {
        std::pair<int,item> pair = (*it);
        reduction(type, (*it).second.left, radius, leftisinside);
        reduction(type, (*it).second.left, radius, !leftisinside);
        it++;
    }
    return true;
}

#endif // LZ_REDUCTION_H