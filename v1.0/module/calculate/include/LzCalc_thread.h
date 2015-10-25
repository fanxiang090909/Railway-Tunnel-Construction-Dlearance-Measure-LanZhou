#ifndef LZCALCULATE_THREAD_H
#define LZCALCULATE_THREAD_H

#include <datastructure.h>
#include <filemanagers.h>

#include <QThread>
#include <QMutex>
#include <QObject>
#include <QDebug>

// 计算类
#include "slavemodel.h"
#include "LzCalculator.h"
#include "icpPointToPoint.h"
#include "Fusion.h"
#include "R_T_Calc.h"   // 旧版本RT计算
#include "Lz_RT_Calc.h" // 新版本RT计算
#include "xmlsynthesisheightsfileloader.h"
#include "output_heights_list.h"
#include "fileoperation.h"

#include "lz_logger.h"

using namespace std;

/**
 * 兰州计算线程类别
 *    一般从机双目计算（双线程）
 *    RT从机双目计算（双线程版本）
 *    RT从机双目计算（单线程版本）
 *    主控融合计算线程
 */
enum LzCalcThreadType
{
    LzCalcThread_GeneralSlave = 0,      // 一般从机双目计算（双线程）
    LzCalcThread_RTSlave_V2 = 1,        // RT从机双目计算（双线程版本）
    LzCalcThread_RTSlave_V1 = 2,        // RT从机双目计算（单线程版本）
    LzCalcThread_Fuse = 3               // 主控融合计算线程
};

/**
 * 计算线程类
 * @author 范翔 唐书航
 * @version 1.0.0
 * @date 2014-08-05
 */
class LzCalculateThread : public QThread 
{
    Q_OBJECT

public:

    LzCalculateThread(QObject * parent = 0, int inithreadid = 1);

    virtual ~LzCalculateThread();

    /**
     * 中断线程、外部调用
     */
    virtual void suspend();

    // 得到线程编号
    int getID();

    bool getHasInit();

    QString getOutputFileName();

    /** 
     * 日志类初始化
     */
    bool initLogger(string filename, string username);
    /**
     * 日志记录
     */
    void log(QString msg);

protected:

    virtual void run();

    bool hasinit;

    string projectpath;

    // 输入标定配置文件（QR），20151025加
    string inputcalibfile;

    string outputfile; 

    DataHead datahead;

    // 线程编号
    int threadid;
    // 当前计算到的fc号
    __int64 currentcalcfc;

    // 是否暂停
    bool ifsuspend;

    // 是否是从上次暂停计算位置继续计算
    bool isfrominterruptfile;
    // if (isfrominterruptfile == true) 上次终止位置
    qint64 interruptedfc;

    LzLogger * logger;
    bool hasinitlog;

signals:
    /**
     * 线程运行结束消息
     * @param threadid 线程编号（单线程时为1）
     * @param isok -1 暂停计算成功。需要参数endfc
     *             0 正常计算结束
     *             1 找不到SlaveModel中对应的cameragroupindex
     *             2 得不到有效的CheckedTunnelTaskModel
     *             5 计算结果文件未打开（主要针对从原来暂停文件中恢复计算的情况）
     *             6 其他异常，（融合高度配置文件未加载）
     *             7 原始图像流式文件不存在
     *             8 原采集数据dat文件不能retrieve到起始帧
     * @param tunnelid 线程所计算的隧道ID
     * @param filename 线程所计算的任务文件名
     * @param endfc 计算的终止帧号
     */
    void finish(int threadid, int isok, int tunnelid, QString filename, qint64 endfc);

    /**
     * 向LzCalcuQueue转发信号
     * @param threadid 线程编号（单线程时为1）
     * @param tmpfc 当前计算帧号
     */
    void statusShow(int threadid, qint64 tmpfc, int tunnelid, QString filename);
};

/**
 * 双目计算线程类
 * 由1个相机盒子中2个相机拍摄的图片计算激光打出的三维点集合坐标，结果为三维点集矩阵
 * @author 范翔
 * @version 1.0.0
 * @date 2014-08-05
 */
class LzCalculateTVThread : public LzCalculateThread
{

private:
    SlaveModel slavemodel;

    string cameragroupid;
    
    std::list<CheckedItem> todocalfilelist;

    int tunnelid;

public:

    LzCalculateTVThread(QObject * parent = 0, int inithreadid = 1);

    virtual ~LzCalculateTVThread();

    /**
     * 计算函数(对应一条隧道的一组相机）
     * @param const std::list<CheckedItem> & todofilelist 计算范围——这条隧道对应的实际采集文件及帧有效范围，
     *									见module/datastructure/checkedtask.h
     * @param string projectpath 工程路径 
     * @param string cameragroupindex
     * @param SlaveModel & slavemodel
     * @param string outputfile输出第x组相机的文件名
     * @param DataHead & outputdatahead 输出文件头
     */
    void init(string projectpath, int newtunnelid, std::list<CheckedItem> & todofilelist, string cameragroupindex, 
                                        SlaveModel & initmodel, string outputfile, DataHead & outputdatahead,
                                        bool isinterrupt, qint64 interruptfc);

protected:

    void run();

};

/**
 * 【双线程计算】双目RT计算线程类
 * 由车底4个相机拍摄的图片计算车厢相对于分中坐标系下的的RT
 * @author 范翔 唐书航
 * @version 1.0.0
 * @date 2015-05-26
 */
class LzCalculateTVRTThread_V2 : public LzCalculateThread
{
private:
    SlaveModel slavemodel;

    string cameragroupid;

    string inputcalibmat;

    std::list<CheckedItem> todocalfilelist;
    
    int tunnelid;

public:

    LzCalculateTVRTThread_V2(QObject * parent = 0, int inithreadid = 1);

    virtual ~LzCalculateTVRTThread_V2();

    /**
     * 计算函数(对应一条隧道的一组相机） 
     * @param const std::list<CheckedItem> & todofilelist 计算范围——这条隧道对应的实际采集文件及帧有效范围，
     *									见module/datastructure/checkedtask.h
     * @param string projectpath 工程路径 
     * @param SlaveModel & slavemodel [注意!][slavemodel的box1,box2的顺序必须为先Q后R]
     * @param inputcalibmat 分中标定基础三维点——固定值，不用传递参数
     * @param string outputfile输出第x组相机的文件名 R, T 分别两个不同的文件
     * @param DataHead & outputdatahead 输出文件头
     */
    void init(string projectpath, int newtunnelid, std::list<CheckedItem> & todofilelist, string cameragroupindex, SlaveModel & initmodel, string outputfile, DataHead & outputdatahead,
                                        bool isinterrupt, qint64 newinterruptfc);

protected:

    void run();
};

/**
 * 【单线程计算】双目RT计算线程类
 * 由车底4个相机拍摄的图片计算车厢相对于分中坐标系下的的RT
 * @author 范翔 唐书航
 * @version 1.0.0
 * @date 2014-08-05
 */
class LzCalculateTVRTThread : public LzCalculateThread
{
private:
    SlaveModel slavemodel;

    string inputcalibmat;
    string outputTFile;

    std::list<CheckedItem> todocalfilelist;
    
    int tunnelid;

public:

    LzCalculateTVRTThread(QObject * parent = 0, int inithreadid = 1);

    virtual ~LzCalculateTVRTThread();

    /**
     * 计算函数(对应一条隧道的一组相机） 
     * @param const std::list<CheckedItem> & todofilelist 计算范围——这条隧道对应的实际采集文件及帧有效范围，
     *									见module/datastructure/checkedtask.h
     * @param string projectpath 工程路径 
     * @param SlaveModel & slavemodel [注意!][slavemodel的box1,box2的顺序必须为先Q后R]
     * @param inputcalibmat 分中标定基础三维点——固定值，不用传递参数
     * @param string outputfile输出第x组相机的文件名 R, T 分别两个不同的文件
     * @param DataHead & outputdatahead 输出文件头
     */
    void init(string projectpath, int newtunnelid, std::list<CheckedItem> & todofilelist, SlaveModel & initmodel, string outputRFile, string outputTFile, DataHead & outputdatahead,
              bool isinterrupt, qint64 interruptfc);

protected:

    void run();

    //void Comput_RT(Mat pnts3D_1, Mat pnts3D_2, Vector<Point3f> calibed_pnts3d, Mat &RR, Mat &TT);
    void Comput_RT(Point2d& Q_p, Point2d& R_p, Point2d& std_Q, Point2d& std_R,Mat &RT);
    void loadcalib_pnts(Vector<Point3f> &calibed_pnts3d, Mat calib_mat);
};

/**
 * 主控融合计算线程类
 * 由车底4个相机拍摄的图片计算车厢相对于分中坐标系下的的RT
 * @author 范翔 唐书航
 * @version 1.0.0
 * @date 2014-08-05
 */
class LzCalculateFuseThread : public LzCalculateThread
{
    Q_OBJECT

private:
    // 融合计算类
    Fusion * f;

    vector<string> inputfilenames;

    string outputextractheightsfile;
    /**
     * 钢轨QR标定文件
     */
    string QRrailcalibfile;

    /**
     * 校正文件
     */
    string rectifysyninput;

    /**
     * 是否使用校正文件
     */
    bool userectifyfactor;

    /**
     * 是否使用校正文件中的 安全系数因子
     */
    bool usesafetyfactor;

    __int64 start_num;
    __int64 frame_cunt;
public:

    LzCalculateFuseThread(QObject * parent = 0, int inithreadid = 1);

    virtual ~LzCalculateFuseThread();

    /**
     * 计算函数(对应一条隧道的一组相机） 
     * @param std::vector<string> inputfilenames
     * @param string projectpath 工程路径 
     * @param string outputfile输出第x组相机的文件名
     * @param DataHead & outputdatahead 输出文件头
     */
    void init(string projectpath, std::vector<string> inputfilenames, string QRrailcalibfile, string rectifysyninput, __int64 startnum, __int64 endnum, string outputfusefile, string outputextractheightsfile, DataHead & outputdatahead, 
              bool isinterrupt, qint64 interruptfc, bool userectifyfactor, bool usesafetyfactor);

    /**
     * 中断线程、外部调用
     */
    virtual void suspend();

protected:

    void run();

private slots:
    /**
     * @param tmpfc 当前计算帧号
     * @param ifsuspend 是否暂停
     */
    void receiveFuseStatus(qint64, bool);

};


#endif // LZCALCULATE_THREAD_H