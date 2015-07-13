每个工程目录结构_主控，客户端（由程序目录下的master_datastore_path.txt或client_datastore_path.txt指定）

【注】：工程创建只在采集时创建，即可理解为采集日期

system 【系统当前配置目录】
	|--- network_server_ip.txt【备份网络存储NAS服务器IP，数据库主机IP，办公室监控服务器IP（客户端才有）】
	|--- network_config.xml
	|--- output_heights.xml
calcu_calibration【双目、分中计算参数配置文件】
	|--- Calibration_A_标定日期.xml
	|--- Calibration_B_标定日期.xml
	|
	|--- ...
	|--- FZCalibration_A_标定日期.xml
	|--- FZCalibration_B_标定日期.xml
	|
	|--- ...
template_output【输出图表模板excel和html】
	|--- MultiTunnelsTemplate.html
	|--- MultiTunnelsTemplate.xls
	|--- SingleSectionTemplate.html
	|--- SingleSectionTemplate.xls
	|--- SingleTunnelTemplate.html
	|--- SingleTunnelTemplate.xls
plan_tasks 【计划任务配置目录】
	|--- ...
"线路_采集日期"【顶级目录】（若多条线路："线路_线路_采集日期"）
	|--- 线路_采集日期.proj（工程目录入口）
	|--- 线路_计划配置日期.plan（计划采集隧道配置，注：此工程目录下的所有目录中的日期只可能这个文件的日期不一致）
	|--- 线路_采集日期.real（实际采集文件）
	|--- 线路_采集日期_pulze_recorder.dat（主控记录脉冲和帧号的匹配数据）
	|--- 线路_采集日期.checked（校正文件，其文件名中日期是采集日期）
	|--- 线路_采集日期_checked.log（校正文件操作日志）
	|--- 线路_采集日期_filetransport.log（备份文件传输日志）
	|--- 线路_采集日期_master_collect.log（主控采集工作日志）
	|--- 线路_采集日期_master_calcu.log（主控计算工作日志）
	|--- 线路_采集日期_master_backup.log（主控备份工作日志）
	|--- network_config.xml
	|--- output_heights.xml
	|--- collect【采集实际文件目录】
	|	|--- 采集序列号_隧道名_采集日期_相机编号.dat
	|	|			（采集序列号就是实际采集的1,2,3,4...防止误触发多采集隧道标识）
	|	|			（采集日期是工程采集日期，不一定是该隧道的实际采集日期）
	|	|			（相机编号是A1、A2、B1、B2...R1、R2）
	|	|--- 采集序列号_隧道名_采集日期_相机编号.dat
	|	|--- 采集序列号_隧道名_采集日期_相机编号.dat
	|	|
	|	|--- ...
	|--- calcu_calibration【双目、分中计算参数配置文件】
	|	|--- Calibration_A_标定日期.xml
	|	|--- Calibration_B_标定日期.xml
	|	|
	|	|--- ...
	|	|--- FZCalibration_A_标定日期.xml
	|	|--- FZCalibration_B_标定日期.xml
	|	|
	|	|--- ...
	|--- mid_calcu【中间计算结果（双目计算）】
	|	|--- 隧道名_采集日期_相机编号.mdat
	|	|--- 隧道名_采集日期_相机编号.mdat
	|	|
	|	|--- ...
	|--- fuse_calcu【融合计算结果】
	|	|--- 隧道名_采集日期.fdat
	|	|--- 隧道名_采集日期.fdat
	|	|
	|	|--- ...
	|--- syn_data【综合计算输入数据】
	|	|--- 隧道名_采集日期.syn
	|	|--- 隧道名_采集日期_correct.log（两两对应，一个.syn就最多有一个与之对应的.log）
	|	|--- 隧道名_采集日期.syn
	|	|--- 隧道名_采集日期_correct.log（两两对应，一个.syn就最多有一个与之对应的.log）
	|	|
	|	|--- ...
	|--- slave_undo【从机未完成作业状态记录】（最终应该为空，程序处理结束会删）
	|	|--- 线路_采集日期_slave1_undo.txt
	|	|--- 线路_采集日期_slave2_undo.txt
	|	|---
	|	|--- ...
	|--- output【输出Excel html限界图表】
	|	|---隧道名_采集日期.html
	|	|---隧道名_采集日期.html
	|	|
	|	|--- ...
	|--- tmp_img【查看原图临时数据】（最终可删）
	|	|--- 采集序列号_隧道名_采集日期_相机编号【临时img目录】（名称形式见【实际采集文件目录】）   
	|	|	|--- 帧号.jpg
	|	|	|--- 帧号.jpg
	|	|	|
	|	|	|--- ...
	|	|--- 采集序列号_隧道名_采集日期_相机编号【临时img目录】
	|	|	|--- 帧号.jpg
	|	|	|--- 帧号.jpg
	|	|	|
	|	|	|--- ...
	|	|
	|	|--- ...