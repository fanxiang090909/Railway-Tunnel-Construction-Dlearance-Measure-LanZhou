配置数据目录路径：
数据目录主要在两个地方需要配置。
任何运行程序的计算机一般需要在本地配置数据目录。通常路径为D:/datastore。
这个数据目录路径每次改变时需要在4个程序初始化配置文件（
client_datastore_path.txt、master_datastore_path.txt、server_datastore_path.txt、slave_datastore_path.txt）的第一行做相应的修改。
在程序初始化时，首先检查是否包含初始化配置文件，并将配置文件的信息读入，正确读入后检查配置数据目录下的配置文件并加载。


数据目录内容：
数据目录包含以下内容。在程序初始化时，首先检查是否包含初始化配置文件，并将配置文件的信息读入，正确读入后检查配置数据目录下的配置文件并加载。
通常初始化运行时会检查system目录下的三个文件，如果缺失，会提示错误，个别程序会直接退出。
着重说明system目录下的三个配置文件：
1）	system/network_serverip_client.txt 客户端程序额外配置文件
包含3行：
第一行：单NAS数据模式下的NAS IP地址；
第二行：服务器 IP地址（在办公室有办公室服务器的前提下，火车上的服务器即是主控机，服务器程序即是lzservertest.exe）；
第三行：数据库服务器IP地址（通常为服务器IP地址，即同第二行，或为localhost）。
2）	system/nework_config.xml 火车上的网络通信及标定配置的配置文件
在下一小节详细讲解这个文件。
3）	system/output_heights.xml 输出高度配置文件
客户端、主控均需要。


每个工程目录结构_主控，客户端（由程序目录下的master_datastore_path.txt或client_datastore_path.txt指定）

【注】：工程创建只在采集时创建，即可理解为采集日期

system 【系统当前配置目录】
	|--- network_serverip_client.txt【备份网络存储NAS服务器IP，数据库主机IP，办公室监控服务器IP（客户端才有）】
	|--- network_config.xml
	|--- output_heights.xml
calcu_calibration【双目标定、分中标准轨标定、误差修正参数配置文件】
	|--- heights_标定日期.rectify 
	|--- QRrail_标定日期.xml
	|--- Calibration_A_标定日期.xml
	|--- Calibration_B_标定日期.xml
	|
	|--- ...
template_output【输出图表模板excel和html】
	|--- MultiTunnelsTemplate.html（区段综合模板）
	|--- MultiTunnelsTemplate.xls（区段综合模板）
	|--- SingleSectionTemplate.html（单帧输出模板）
	|--- SingleSectionTemplate.xls（单帧输出模板）
	|--- SingleTunnelTemplate.html（单隧道综合模板）
	|--- SingleTunnelTemplate.xls（单隧道综合模板）
	|--- red.jpg
	|--- blue.jpg
	|--- black.jpg
	|--- BaseClearance_D_NeiRan.sec（双层集装箱_内燃区段_标准限制值）
	|--- BaseClearance_D_DianLi.sec（双层集装箱_电力区段_标准限制值）
	|--- BaseClearance_B_NeiRan.sec（标准限界_内燃区段_标准限制值）
	|--- BaseClearance_B_DianLi.sec（标准限界_电力区段_标准限制值）
plan_tasks 【计划任务配置目录】
	|--- ...
db_backup 【数据库备份目录】
	|--- ...
output 【默认图表输出目录】
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
	|--- calcu_calibration【双目标定、分中标准轨标定、误差修正参数配置文件】
	|	|--- heights_标定日期.rectify 
	|	|--- QRrail_标定日期.xml
	|	|--- Calibration_A_标定日期.xml
	|	|--- Calibration_B_标定日期.xml
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