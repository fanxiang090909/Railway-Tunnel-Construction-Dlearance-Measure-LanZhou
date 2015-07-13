/**
 * 兰州项目采集、计算选择过滤器
 *		说明：根据参数：@param carriage_direction 【采集时】 注明的车厢正反
 *					 @param is_normal  【采集时】 注明此次拍摄的双线正常/非正常信息，（单线正序/逆序信息）
 *					 @parma is_double_line 【隧道属性】 是否为双线做出判断
 *                   @param is_down_link   【隧道属性】是否下行
 *			 判断结果：采集时采集相对车厢的哪一侧相机
 *					计算时，哪一侧数据有效（或者只依据采集时的有用数据）
 *					综合时“实际”里程位置，而不是面向终点采集时相对于隧道口的里程
 *
 *			 参考文档 svn://10.13.29.236:3693/lanzhou2013/项目组例会/兰州需求座谈会20130530=31.doc
 *					svn://lanzhou2013/个人/范翔\数据库设计v1.0/关于单双线、正常非正常拍摄图示
 * @author 范翔
 * @version 1.0.0
 * @date 2014-04-05
 */
#ifndef LZ_FILTER_H
#define LZ_FILTER_H

#include <QDebug>

/**
 * 计算当前断面的控制点位置（“实际”里程）
 * 根据采集的is_normal信息  和   数据库提供的起始终止里程信息，算出当前断面的控制点位置（“实际”里程）
 * @param is_normal       【采集时】注明此次拍摄的双线正常/非正常信息，（单线正序/逆序信息）
 * @param filteredmileage 【采集时】注明内部里程，隧道口为0
 * @param is_double_line  【隧道属性】是否双线
 * @param is_down_link    【隧道属性】是否下行
 * @param tunnelStartPoint【隧道属性】隧道的起始里程
 * @param tunnelEndPoint  【隧道属性】隧道的终止里程
 * @param filteredmileage  【返回】过滤后的里程，直接修改
 * @return true 过滤成功，else 识别结果发现有可能有错误（不在tunnelStartPoint与tunnelEndPoint范围内）
 */
static bool Filter_TunnelMileage_InterToActual(bool is_normal, bool is_double_line, bool is_down_link, __int64 tunnelStartPoint, __int64 tunnelEndPoint, double & filteredmileage)
{
    qDebug() << "****Filter_TunnelMileage_InterToActual:*****";
	qDebug() << "is_normal=" << is_normal << ", is_double_line=" << is_double_line << ", is_down_link=" << is_down_link << ", mileage=" << filteredmileage;
    qDebug() << "this tunnel startpoint=" << tunnelStartPoint << ", endpoint=" << tunnelEndPoint;
    // 若单线正常（正序），双线下行正常、双线上行非正常，mileage+隧道起始里程为当前里程
	if ((!is_double_line && is_normal) || (is_double_line && is_down_link && is_normal) || (is_double_line && !is_down_link && !is_normal))
    {
        // 里程表示
        filteredmileage = tunnelStartPoint + filteredmileage;

        // 隧道距小里程隧道口表示
        //filteredmileage = filteredmileage;
    }
    else // 若单线非正常（逆序），双线下行非正常、双线上行正常，隧道终止里程-mileage为当前里程
    {
        // 里程表示
		filteredmileage = tunnelEndPoint - filteredmileage;

        // 隧道距小里程隧道口表示
        //filteredmileage = tunnelEndPoint - tunnelStartPoint - filteredmileage;
    }

    qDebug() << "the filtered mileage=" << filteredmileage;
    // 里程表示
    if (filteredmileage <= tunnelEndPoint && tunnelStartPoint <= filteredmileage)
        return true;
    else
        return false;
    //return true;
}

// 【注意！】相对于车厢正向的相机组左侧右侧开启模式
enum CollectDirectionMode
{
    Collect_All_Mode = 0,
    Collect_L_Mode = 1,  // 面对车厢正向的左侧（相机配置文件中应包含这个信息）
    Collect_R_Mode = 2,  // 面对车厢正向的右侧
    Collect_C_Mode = 3	 // 左右重叠区域，中部，应该不返回这种模型，不过在配置文件中可按此设置
};

/**
 * 采集应被打开的相机组号
 * 根据采集的车厢正反carriage_direction信息  和  数据库中的隧道是否双线是否下行信息，算出采集应被打开的相机组号
 * @param carriage_direction  【采集时】注明此次拍摄车厢正反
 * @param is_normal           【采集时】注明此次拍摄的双线正常/非正常信息，（单线正序/逆序信息）
 * @param is_double_line      【隧道属性】是否双线
 * @param is_down_link        【隧道属性】是否下行		——		此处不需要
 * @return CollectDirectionMode 相机组有效模式，@see CollectDirectionMode，三种之一Collect_All_Mode，Collect_L_Mode，Collect_R_Mode
 */
static CollectDirectionMode Filter_CollectCameraGroupsOpen_CollectDirectionMode(bool carriage_direction, bool is_normal, bool is_double_line, bool is_down_link)
{
    qDebug() << "****Filter_CollectCameraGroupsOpen_CollectDirectionMode:*****";
    qDebug() << "carriage_direction=" << carriage_direction << ", is_normal=" << is_normal << ", is_double_line=" << is_double_line << ", is_down_link=" << is_down_link;
	CollectDirectionMode ret;
	// 若为单线隧道，相机全部打开
    if (!is_double_line)
    {
		ret = Collect_All_Mode;
    }
    else // 若为双线
    {
 	    // 暂不考虑车厢正反，true为面向终点的左侧数据有效，false为面向终点的右侧数据有效
		bool tmpDirection; 
		if (is_normal) // 双线下行正常，双线上行正常时，面向终点的左侧数据有效
			tmpDirection = true;
		else			// 双线下行【非】正常，双线上行【非】正常时，面向终点的右侧数据有效
			tmpDirection = false;
		// 考虑车厢正反
		if ((carriage_direction && tmpDirection) || (!carriage_direction && !tmpDirection))
			ret = Collect_L_Mode;
		else
			ret = Collect_R_Mode;
	}
    qDebug() << "the filtered CollectDirectionMode=" << ret << "(0-all,1-leftmode,2-rightmode";
	return ret;
}

/**
 * 融合计算结果的左右与车行面对终点的左右方向一致性判断
 * 根据采集的车厢正反carriage_direction信息  和  数据库中的隧道是否双线是否下行信息，算出融合计算结果的左右与车行面对终点的左右方向一致性判断
 * @param carriage_direction  【采集时】注明此次拍摄车厢正反
 * @param is_normal           【采集时】注明此次拍摄的双线正常/非正常信息，（单线正序/逆序信息）
 * @param is_double_line      【隧道属性】是否双线
 * @param is_down_link        【隧道属性】是否下行
 * @param camera_direction_mode CollectDirectionMode (ref引用，也是作为filter结果) 相机组有效模式，@see CollectDirectionMode
 * @return true 左右一致（车厢正向，面向生活区），else 左右相反（车厢反向，背对生活区）
 */
static bool Filter_Fuse_LeftRightCoordinate(bool carriage_direction/*, bool is_normal, bool is_double_line, bool is_down_link*/)
{
    //qDebug() << "****Filter_Fuse_LeftRightCoordinate:*****";
    //qDebug() << "carriage_direction=" << carriage_direction << ", is_normal=" << is_normal << ", is_double_line=" << is_double_line << ", is_down_link=" << is_down_link;
    return carriage_direction;
}

#endif // LZ_FILTER_H