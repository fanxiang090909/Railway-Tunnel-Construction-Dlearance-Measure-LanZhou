目录说明：

icon 图标目录

Startup 开机程序自启动批处理文件

copyexe.bat
- 在module中各个VS 工程已经编译好release版本的前提下，直接拷贝exe到此目录下的Release文件夹中，同时解压Release_dll.rar到Release目录中。

copy_config.bat
- 拷贝4个txt配置文件到Release目录下

copy_config_test.bat
- 拷贝4个txt配置文件到Release目录下

主要可运行程序说明：
1) 客户端程序（火车和办公室均可使用）：lanzhouclient2013.exe
2) 主控程序（火车上）
   2.1) 全部程序（包括采集计算备份）：lz_master_gui.exe
   2.2) 缩减版本（只包括采集部分）：lz_master_monitor.exe
3) 从控程序（火车上）
   3.1) 从控主程序：lz_slave.exe
   3.2) 从控控制程序-客户端：slave_mini.exe
4) 办公室服务器程序、从控控制程序-服务器（办公室、火车）：lzservertest.exe
5) 标定程序（集成双目、分中标定）
   5.1) 双目标定采集程序：LzGrasp.exe
   5.2) 标定计算程序：Lz_Calibration.exe
6) ToolBox小工具
   6.1) 文件查看-采集原始图像文件查看	：viewacquisitionfile.exe
   6.2) 文件查看-中间计算结果、融合计算结果文件查看、断面精度误差对比：viewcalcumatfile.exe
   6.3) 文件查看-双目三维点+原图查看：view3dtvmatfile.exe
   6.4) 单机计算（数据都在本机，不是火车上的网络分布式计算方式）：calcu_noslave.exe
每个可运行程序在初始化时首先都会需要自己的初始化配置文件。如果不存在或信息有误，则打开不成功。
另外初始化配置文件中明确指明了项目的数据路径，个别程序还需要从数据路径中读取项目数据目录中system下的配置文件（二级配置文件），如果二级配置中不存在或读取有误，程序也可能启动失败。


可运行程序与初始化配置文件对应关系：
在程序初始化时，首先检查是否包含初始化配置文件，并将配置文件的信息读入，若出现错误，则程序打开失败。
每个可运行exe程序都会对应一个初始化配置文件，对应关系如下：
lanzhouclient2013.exe      -> client_datastore_path.txt
lz_master_gui.exe          -> master_datastore_path.txt
lz_master_monitor.exe      -> master_datastore_path.txt
lzservertest.exe           -> server_datastore_path.txt
lz_slave.exe               -> slave_datastore_path.txt
slave_mini.exe             -> slave_datastore_path.txt
viewacquisitionfile.exe    -> slave_datastore_path.txt
viewcalcumatfile.exe       -> slave_datastore_path.txt
view3dtvmatfile.exe        -> slave_datastore_path.txt
calcu_noslave.exe          -> master_datastore_path.txt


配置文件说明：
1)	client_datastore_path.txt 客户端程序配置文件
只包含1行：表示本机存储项目数据目录
2)	master_datastore_path.txt主控程序配置文件
包含2行：
第一行：表示本机存储项目数据目录；
第二行：表示默认的0.5m/帧模式下的，相邻两帧的里程间隔。
3)	server_datastore_path.txt办公室服务器程序、从控控制程序-服务器配置文件
包含2行：
第一行：表示本机存储项目数据目录；
第二行：表示NAS上存储主控机数据文件的NAS IP地址。
        若NAS在火车上备份时有NAS1，和NAS2。
默认NAS1上存储主控数据、从控1-4的数据；NAS2上存储从控5-9的数据。
4)	slave_datastore_path.txt
包含4行：
第一行：表示本机存储项目数据目录；
第二行：本机负责连接主控机的本机端口IP地址（因为本机网络端口可能有很多）；
第三行：从控程序目录（专门用于从控控制程序-客户端：slave_mini.exe）；
第四行：从控程序打开批处理目录（专门用于从控控制程序-客户端：slave_mini.exe）。