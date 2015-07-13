/*
*	headdefine.h
*	兰州隧道限界检测项目 数据文件头定义
*
*	此文件头将被添加到所有相关数据文件中，作为文件描述
*	采集文件 -> LzSerialStorAcqui					|	存储对象 frame	(pylon sdk提供采集图像的数据块)
*	计算中间结果文件 -> LzSerialStorCalc			|
*	计算列车相对轨平面旋转 -> LzSerialStorRT		|	存储对象 cv::Mat
*	中间结果融合文件 -> LzSerialStorFuse			|
*	综合结果文件 -> LzSerialStorSynth				|	存储对象 std::map
*
*	作者: Ding (leeleedzy@gmail.com)
*	版本: alpha v1.0
*	日期: 2014.1.13
*
*/

#ifndef LZ_HEADDEFINE_H
#define LZ_HEADDEFINE_H

//流式存储文件头
struct DataHead
{
	unsigned int	tunnel_id;				//隧道ID
	char			tunnel_name[50];		//隧道名称

	double      	start_mileage;			//起始里程
	double       	end_mileage;			//终止里程

	bool			is_rectify;				//文件是否已经矫正
	bool			is_double_line;			//是否双线
	bool			is_normal;				//是否为正常拍摄（双线）/是否为正序（单线）
	bool			is_downlink;			//是否为下行线
	bool			carriage_direction;		//车厢的方向
	bool			is_newline;				//是否为新线/旧线
	unsigned int	line_type;				//线路类型：内燃/牵引/...路段
	_int64	        task_tunnel_id;			//采集任务编号（校对前，校对后均可）
	char			datetime[20];			//采集日期
	unsigned int	line_id;				//线路ID
	char			line_name[50];			//线路名称
	unsigned int	interval_pulse_num;		//此次采集的帧间隔脉冲数，用于换算里程

	unsigned int	frame_num;				//对应图像索引表的元素个数。索引表长度

	char        	camera_index[4];		//相机编号，如果是综合数据，ID处标号是0，表示16组相机的综合数据
	unsigned int	version;				//文件版本号

	unsigned int	seqno;					//采集序号（每次按计划任务采集时实际采集文件不一定完全对应，但是有序号，顺序，方便以后校正）
	char			reservebit[346];		//预留区域			
};

#endif