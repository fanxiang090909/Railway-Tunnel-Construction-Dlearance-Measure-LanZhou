/**
 * 主要程序说明及程序模块说明
 * @author 范翔
 * @date 2015-05-26
 * @version 1.0.0
 */
开发环境：Windows（x64）：VS2012（MSVC110） + Qt4.8.0 + Boost + OpenCV2.4.9
开发环境所有安装包见服务器 \\10.13.29.236\Public Resource\0.科研项目\Lz兰州铁路2012\_开发环境+目标机安装运行环境

主要程序说明：
1）客户端程序（火车和办公室均可使用）
	module\lz_client_main\test\gui\nmake\lanzhouclient2013.sln

2) 主控程序（火车上）
   2.1) 全部程序（包括采集计算备份）：
	module\lz_master_main\test\lz_master_gui\nmake\lz_master_gui.sln
   2.2) 缩减版本（只包括采集部分）
	module\lz_master_main\test\monitor\nmake\lz_master_monitor.sln

3）从控程序（火车上）
   3.1）从控主程序
   	module\lz_slave_main\test\nmake\slave_main\lz_slave.sln
   3.2）从控控制程序-客户端
   	module\lz_slave_main\test\nmake\slave_mini\slave_mini.sln


4) 办公室服务器程序、从控控制程序-服务器（办公室、火车）
	module\lz_office_server_main\test\nmake\lzservertest.sln

5）标定程序（集成双目、分中标定）
   5.1）双目标定采集程序
        module\acquistion\test\LzGrasp\nmake\LzGrasp.sln
   5.2）标定计算程序
	module\lz_calibration\test\Lz_Calibration.sln

6) ToolBox小工具
   6.1) 文件查看-采集原始图像文件查看
	module\filemanagers\test\viewacquisitionfile\nmake\viewacquisitionfile.sln
   6.2) 文件查看-中间计算结果、融合计算结果文件查看、断面精度误差对比
	module\filemanagers\test\viewcalcumatfile\nmake\viewcalcumatfile.sln
   6.3) 文件查看-双目三维点+原图查看
	module\filemanagers\test\view3dtvfile\nmake\view3dtvmatfile.sln
   6.4) 单机计算（数据都在本机，不是火车上的网络分布式计算方式）
	module\lz_master_main\test\calcu_noslave\nmake\calcu_noslave.sln


模块说明
acquisition - 采集
calculate - 计算框架、预处理、双目计算、融合计算、综合计算
compress - 压缩（采集图片压缩）、传输文件压缩解压缩
datastructure - 基础数据类型：计划任务队列，实际采集任务队列，校正任务队列等
filemanagers - 流式文件读写管理；QtUI：图像二维点三维点基础显示界面
filetransport - 网络文件传输：文件发送接收
lz_calibration - 标定
lz_client_main - 客户端相关程序：业务逻辑和QtUI
lz_db - 数据库访问
lz_exception - 异常
lz_logger - 日志
lz_master_main - 主控相关程序：业务逻辑和QtUI
lz_office_server_main 办公室服务器相关程序：业务逻辑和QtUI
lz_output - 图表输出：html和excel
lz_slave_main - 从控相关程序：业务逻辑和QtUI
network_cs - 网络消息传输
pulsehandle - 硬件接口卡相关：集成车轮脉冲和进出洞检测
xmlloader - XML配置文件读写