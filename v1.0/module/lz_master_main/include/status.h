#ifndef STATUS_H
#define STATUS_H

#define COLLECT_CAMERA_NUM 36  //宏定义
#define CALCULATE_CAMERA_GROUP_NUM 12

#include <QObject>
#include <QMutex>

#include <iostream>
#include <list>
#include <string>
#include <vector>

#include "lz_working_enum.h"

#include "plantask_list.h"
#include "xmltaskfileloader.h"

#ifndef MILEINTERVAL
#define MILEINTERVAL 0.5
#endif 

using namespace std;

/**
 * 单台从机4相机实时帧号记录
 */
struct SlaveCollectFrameCounterRecord
{
    _int64 startfc_box1cameraref;
    _int64 startfc_box1camera;
    _int64 startfc_box2cameraref;
    _int64 startfc_box2camera;
    _int64 endfc_box1cameraref;
    _int64 endfc_box1camera;
    _int64 endfc_box2cameraref;
    _int64 endfc_box2camera;
};

/**
 * 实际采集任务信息的状态，链表元素
 */
struct CollectRecord {
    int seqno;                  // 序列号，每次从1开始
    int record_tunnelid;        // 记录的tunnelid号
    string record_tunnelname;
    string record_time;
    int returnnum;              // n个从控机最终返回n次，每次returnnum++
    // 校正之后设置是否有效位，之前都为false
    bool isvalid;               // 是否有效
    int corrected_tunnelid;     // 若isvalid为false，此处无用，经校正后的隧道ID
    _int64 start_framecounter;  // 若isvalid为false，此处无用，否则表示各个文件的可取交集里程范围
    _int64 end_framecounter;    // 若isvalid为false，此处无用，否则表示各个文件的可取交集里程范围
    float start_mile;
    float end_mile;
};

/**
 * 单台从机采集（采集文件备份）记录项信息结构体
 */
struct SlaveCollectRecord {
    int seqno;
    // 每个从控机连接的4个相机的索引号和状态
    // camerabackup，false还未备份，true已经备份
    string index_camera1;
    bool backup_camera1;
    string index_camera2;
    bool backup_camera2;
    string index_camera3;
    bool backup_camera3;
    string index_camera4;
    bool backup_camera4;
    _int64 start_framecounter_camera1;
    _int64 end_framecounter_camera1;
    _int64 start_framecounter_camera2;
    _int64 end_framecounter_camera2;
    _int64 start_framecounter_camera3;
    _int64 end_framecounter_camera3;
    _int64 start_framecounter_camera4;
    _int64 end_framecounter_camera4;
};

/**
 * 单台从机计算（计算结果文件备份）记录项信息结构体
 */
/*struct SlaveCalculateRecord {
    int corrected_tunnelid;
    // groupstatus 0还未计算，1已经双目（中间）计算完成，2双目（中间）计算结果已经备份
    string group1index;
    int group1status;
    string group2index;
    int group2status;
};*/

/**
 * 从机未完成工作项结构体（采集计算备份未完成）
 */
/*struct Uncompleted {
    // 未完成状态记录
    int seqno;
    WorkingStatus status; // 计算或是备份未完成
    int filetype;         // 计算区分（中间计算结果、融合计算结果）；备份时需要（区分原始数据，中间计算结果）（也可省略，只根据index大小来判断）
    string index;
    _int64 frameStart;    // 没有必要workingStatus typeUncompleted;
};*/

/**
 * 行车状态类声明
 * @author fanxiang
 * @version 1.0.0
 * @date 2013-10-13
 */
class Status : public QObject
{
    Q_OBJECT
public:

    /**
     * singleton设计模式，公有静态方法返回单一实例
     * @return 单一实例
     */
    static Status* getStatusInstance();

    ~Status();
    
    /**
     * 得到从机当前记录帧号结构体
     * @param ret 返回值是否成功，引用
     */
    SlaveCollectFrameCounterRecord & getSlaveCollectFrameCounterRecord(int slaveid, bool & ret);

    // 对status 当前fc和mile的操作锁
    QMutex fcrecord_mutex;
    _int64 startfc_master;
    _int64 endfc_master;
    float startmile_master;
    float endmile_master;
    QString collecting_filenameprefix_master;

    // 主控当前执行到的采集状态
    QMutex master_status_mutex;
    MasterCollectingStatus master_collect_status;
    int num_master_collect_status;

    /**
     * 增加采集记录（主控端）
     * 此函数由主控主动调用
     */
    bool addToCollectList_master(_int64 start_framecounter, _int64 end_framecounter, float start_mile, float end_mile);

    /**
     * 增加采集记录(或更新9台采集机的returnnum值，最大为Setting::SLAVE_MAX)
     * 此函数在接收从控返回消息时调用
     * 内部对list增加或更新操作，
     * 若tunnelid最新，则增加list元素；
     * 若已有tunnnelid，则更新数据，一般在多从控返回时，vector中的对应元素的returnnum++
     * @param slaveid 从机号
     * @param tunnelid 隧道号
     * @param tunnelname 隧道名称
     * @param seqno 采集序列号
     * @param cameraindex 相机索引号A1。。。
     * //@param remark 隧道号备注号（根据 @parma tunnelid 和 remark 作为标识确定从控中对应的文件）
     * @return true 操作成功，false失败
     */
    bool addToCollectList(int slaveid, int tunnelid, QString tunnelname, int seqno, QString cameraindex, _int64 start_framecounter, _int64 end_framecounter);

    /*******初始化CalcuList********/
    // 【注意！】 记录计算状态时必须
    //bool addTaskTunnelToAllCalculatedList();

    /**
     * 更新计算状态
     * 【前提】计算状态记录链表已经初始化了
     */
    //bool recordCalculate(QString filename, int slaveid, int tunnelid, string groupindex);

    /**
     * 更新备份状态
     * 【注意！】在函数内部根据index的长度判断是备份的A、B、A1、A2...哪个文件（计算中间or实际采集）
     * @param filetype 1是原始图像数据，2是中间计算结果数据
     * @param seqno  当计算中间文件根据tunnelid标识
     *                 实际采集文件根据seqno标识
     */
    //bool recordBackup(QString filename, int slaveid, int seqno, int tunnelid, string index);

    /**
     * 增加到各自slaveid的断点记录list中
     */
    //bool addToUncompletedList(QString filename, int newslaveid, int type, int newseqno, WorkingStatus newstatus, string newindex, _int64 newframestart);

    /**
     * 删除采集隧道链表中全部元素
     */
    bool deleteAllCollectList();

    /**
     * 删除slaveRecordList全部元素
     */
    bool deleteAllSlaveRecordList();

    /**
     * 删除list中已知tunnelid和remark的元素
     * 调用STL list的erace(list<xxx>::iterator方法)it1 = mylist.erase (it1);
     */
    int deleteFromCollectList(int tunnelid, int remark);

    /**
     * 保存采集时所记录的信息至RealTaskList，并写入文件
     * @see RealTaskList
     * @see XMLRealTaskFileLoader
     */
    bool saveCollectedInfoToRealFile();

    /**
     * 删除uncompletedList中的头元素，并返回，pop
     * @param slaveid 输入从机索引号
     * @param outputUncompleted 输出从机slaveid的未完成项
     * @return true 找到并成功pop出
     */
    //bool popFromUncompletedList(int slaveid, Uncompleted & outputUncompleted);

    /**
     * 返回实际采集的list，方便遍历显示等界面或其他操作
     */
    list<CollectRecord> & getCollectList();
    list<CollectRecord>::iterator collectListBegin();
    list<CollectRecord>::iterator collectListEnd();

    /**
     * 返回从控机的备份
     * @param slaveid 从机索引号
     * @param ret 输出返回值 true为存在slaveid
     * @return 返回从控机的备份记录队列
     */
    list<SlaveCollectRecord> & getSlaveRecordList(int slaveid, bool & ret);

    /**
     * 返回从控机的计算记录队列
     * @param slaveid 从机索引号
     * @param ret 输出返回值 true为存在slaveid
     * @return 从控机的计算记录队列
     */
    //list<SlaveCalculateRecord> & getSlaveCalcuList(int slaveid, bool & ret);

    /**
     * 返回从控机的计算和备份中未完成节点
     * @param slaveid 从机索引号
     * @param ret 输出返回值 true为存在slaveid
     * @return 从控机的计算和备份中未完成节点队列
     */
    //list<Uncompleted> & getUncompletedList(int slaveid, bool & ret);

    /**
     * 返回主控的总工作状态
     * 计算&备份   或   采集
     */
    WorkingStatus getWorkingStatus();

    /**
     * 初始化工作状态，工作状态归零
     */
    void initWorkingStatus();
    /**
     * 更新工作状态
     * @return true 可以更新到新状态（currentStatusNum==0）
     *         false 不能更新到新工作状态（currentStatusNum!=0）
     */
    bool setWorkingStatus(WorkingStatus newStatus);
    /**
     * 从机进入工作状态
     * 有如下工作状态可进入：Preparing（网络连接），Collecting（采集），Backuping（备份），Calculating（计算）
     */
    void slaveIntoStatus(WorkingStatus newStatus, int slaveid);
    /**
     * 从机离开当前工作状态
     * 有如下工作状态：Preparing（网络连接），Collecting（采集），Backuping（备份），Calculating（计算）
     */
    void slaveOutfromStatus(WorkingStatus newStatus, int slaveid);
    /**
     * 得到当前状态下的从机数量
     */
    int getStatusSlaveNum(WorkingStatus newStatus);

    /**
     * 初始化采集，主机的PlanTaskList信息
     */
    bool initCollectStatus();

    /**
     * 初始化计算备份状态
     * 加载已有文件的数据到各个结构中
     */
    bool initCalculateBackupStatus();

    /**
     * 返回从控机的是否处在某状态中的引用, 若slaveid=0返回的是主机是否在该状态中的引用
     * @param tofind 查找的状态
     * @param slaveid 从机索引号
     * @param ret 返回值true为存在slaveid
     * @return 返回从机slaveid是否在该状态中
     */
    bool & getSlaveWorkingStatus(WorkingStatus tofind, int slaveid, bool & ret);

    /**
     * 指定九个文件位置的读取未完成工作（计算备份断点）
     * uncompleted_slave_1.txt   ...   uncompleted_slave_9.txt
     */
    //bool saveUncompletedToFiles();
    //bool loadUncompletedFromFiles();

private:

    explicit Status(QObject *parent = 0);

    // singleton设计模式，静态私有实例变量
    static Status* statusInstance;

    /**
     * 得到从机状态数值引用，只有Collecting，Calculating，和Backuping状态可以有值，其他均不做记录
     * @param stat 状态
     * @param ret 是否成功找到返回值的引用
     * @return 返回从机slaveid是否在该状态中
     */
    int & getSlaveNumWorkingStatus(WorkingStatus, bool & ret);

    // 主机的隧道记录
    QList<PlanTask>::iterator planTaskIterator;
    int seq;
    // 对存储队列的操作锁
    QMutex seqmutex;

    // 相机数量36路
    //static int COLLECT_CAMERA_NUM;

    // 计算时候相机组数。16组
    //static int CALCULATE_CAMERA_GROUP_NUM;

    /**
     * 返回list中tunnelid中已知
     */
    list<CollectRecord>::iterator findCollectListIterator(int tunnelid, int remark);

    /**
     * 返回list中seqno已知
     */
    list<CollectRecord>::reverse_iterator findCollectListIterator(int seqno);

public:

    vector<SlaveCollectFrameCounterRecord> slaveFrameCounterRecord;

    // 实际采集任务的状态链表
    list<CollectRecord> collectList;
    // 对存储队列的操作锁
    QMutex collectListmutex;

    // 用于记录每个从控机的原始图像备份状态
    list<SlaveCollectRecord> slave1RecordList;
    list<SlaveCollectRecord> slave2RecordList;
    list<SlaveCollectRecord> slave3RecordList;
    list<SlaveCollectRecord> slave4RecordList;
    list<SlaveCollectRecord> slave5RecordList;
    list<SlaveCollectRecord> slave6RecordList;
    list<SlaveCollectRecord> slave7RecordList;
    list<SlaveCollectRecord> slave8RecordList;
    list<SlaveCollectRecord> slave9RecordList;

    // 用于记录每个从控机的双目（中间）计算、备份状态
    /*list<SlaveCalculateRecord> slave1CalcuList;
    list<SlaveCalculateRecord> slave2CalcuList;
    list<SlaveCalculateRecord> slave3CalcuList;
    list<SlaveCalculateRecord> slave4CalcuList;
    list<SlaveCalculateRecord> slave5CalcuList;
    list<SlaveCalculateRecord> slave6CalcuList;
    list<SlaveCalculateRecord> slave7CalcuList;
    list<SlaveCalculateRecord> slave8CalcuList;
    list<SlaveCalculateRecord> slave9CalcuList;

    // 用于记录是否有未完成工作（传到一半的）
    // 因为考虑每个slave上边线程数不一样，可能中断计算或备份时，断点比较多
    list<Uncompleted> slave1UncompletedList;
    list<Uncompleted> slave2UncompletedList;
    list<Uncompleted> slave3UncompletedList;
    list<Uncompleted> slave4UncompletedList;
    list<Uncompleted> slave5UncompletedList;
    list<Uncompleted> slave6UncompletedList;
    list<Uncompleted> slave7UncompletedList;
    list<Uncompleted> slave8UncompletedList;
    list<Uncompleted> slave9UncompletedList;
    list<Uncompleted> masterUncompletedList;*/
    
    /**
     * 当前工作状态
     */
    WorkingStatus workingStatus;
    // 当前连接的从机数量
    int slavenum_connected;
    bool slave1_connected;
    bool slave2_connected;
    bool slave3_connected;
    bool slave4_connected;
    bool slave5_connected;
    bool slave6_connected;
    bool slave7_connected;
    bool slave8_connected;
    bool slave9_connected;

    /**
     * 正常当前工作状态的从机数量
     */
    int slavenum_in_collect;
    bool master_in_collect;
    // 记录每个从机的工作状态
    bool slave1_in_collect;
    bool slave2_in_collect;
    bool slave3_in_collect;
    bool slave4_in_collect;
    bool slave5_in_collect;
    bool slave6_in_collect;
    bool slave7_in_collect;
    bool slave8_in_collect;
    bool slave9_in_collect;

    /**
     * 正常当前工作状态的从机数量
     */
    int slavenum_in_calcu;
    bool master_in_calcu;
    // 记录每个从机的工作状态
    bool slave1_in_calcu;
    bool slave2_in_calcu;
    bool slave3_in_calcu;
    bool slave4_in_calcu;
    bool slave5_in_calcu;
    bool slave6_in_calcu;
    bool slave7_in_calcu;
    bool slave8_in_calcu;
    bool slave9_in_calcu;

    /**
     * 正常当前工作状态的从机数量
     */
    int slavenum_in_backup;
    bool master_in_backup;
    // 记录每个从机的工作状态
    bool slave1_in_backup;
    bool slave2_in_backup;
    bool slave3_in_backup;
    bool slave4_in_backup;
    bool slave5_in_backup;
    bool slave6_in_backup;
    bool slave7_in_backup;
    bool slave8_in_backup;
    bool slave9_in_backup;

signals:

    void errortogui(QString);

};

#endif // STATUS_H
