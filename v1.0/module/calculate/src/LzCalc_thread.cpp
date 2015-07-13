#include "LzCalc_thread.h"

/**
 * 计算线程类
 * @author 范翔 唐书航
 * @version 1.0.0
 * @date 2014-08-05
 */
LzCalculateThread::LzCalculateThread(QObject * parent, int initthreadid) : QThread(parent), threadid(initthreadid)
{
    hasinit = false;
    projectpath = "";
    outputfile = "";
    ifsuspend = false;
    currentcalcfc = 0;

    isfrominterruptfile = false;
    interruptedfc = 0;
}

LzCalculateThread::~LzCalculateThread()
{

}

/**
 * 中断线程、外部调用
 */
void LzCalculateThread::suspend()
{
    ifsuspend = true;
}

// 得到线程编号
int LzCalculateThread::getID()
{
    return threadid;
}

bool LzCalculateThread::getHasInit()
{
    return hasinit;
}

QString LzCalculateThread::getOutputFileName()
{
    return QObject::tr(outputfile.c_str());
}

void LzCalculateThread::run()
{

}

/** 
 * 日志类初始化
 */
bool LzCalculateThread::initLogger(string filename, string username)
{
    if (logger != NULL)
    {
        if (logger->isLogging())
        {
            logger->log(string("关闭工程-文件校正,") + string("用户") + username);
            logger->close();
        }
        delete logger;
    }

    logger = new LzLogger(LzLogger::MasterOpt);
    logger->setFilename(filename);
    if (logger->open() == 0)
    {
        hasinitlog = true;
        logger->log(string("开启工程-文件校正") + string("用户") + username);
        return true;
    }
    else
    {
        hasinitlog = false;
        return false;
    }
}

void LzCalculateThread::log(QString msg)
{
    if (hasinitlog)
        logger->log(msg.toLocal8Bit().constData());
}

/**
 * 双目计算线程类
 * 由1个相机盒子中2个相机拍摄的图片计算激光打出的三维点集合坐标，结果为三维点集矩阵
 * @author 范翔
 * @version 1.0.0
 * @date 2014-08-05
 */
LzCalculateTVThread::LzCalculateTVThread(QObject * parent, int inithreadid) : LzCalculateThread(parent, inithreadid), slavemodel(1.0)
{
    logger = NULL;
}

LzCalculateTVThread::~LzCalculateTVThread()
{

}

/**
 * 计算函数(对应一条隧道的一组相机）
 * @param const std::list<CheckedItem> & todofilelist 计算范围——这条隧道对应的实际采集文件及帧有效范围，
 *									见module/datastructure/checkedtask.h
 * @param string projectpath 工程路径 
 * @param int newtunnelid 当前隧道ID
 * @param string cameragroupindex
 * @param SlaveModel & slavemodel
 * @param string outputfile输出第x组相机的文件名
 * @param DataHead & outputdatahead 输出文件头
 */
void LzCalculateTVThread::init(string projectpath, int newtunnelid, std::list<CheckedItem> & todofilelist, string cameragroupindex, SlaveModel & initmodel, string outputfile, DataHead & outputdatahead, bool isinterrupt, qint64 newinterruptfc)
{
    this->projectpath = projectpath;
    this->tunnelid = newtunnelid;
    this->todocalfilelist = todofilelist;
    this->cameragroupid = cameragroupindex;
    this->slavemodel = initmodel;
    this->outputfile = outputfile;
    this->datahead = outputdatahead;

    initLogger(outputfile + ".log", "tangshuhang");

    this->isfrominterruptfile = isinterrupt;
    this->interruptedfc = newinterruptfc;
    ifsuspend = false;
}

void LzCalculateTVThread::run()
{
    qDebug() << QObject::tr("单隧道双目计算线程") << QObject::tr(projectpath.c_str()) << "/mid_calcu/" << QObject::tr(outputfile.c_str());
    std::cout << "单隧道双目计算" << std::endl;

    bool ret = false;
    char a[2];
    a[0] = slavemodel.box1.boxindex;
    a[1] = '\0';
    string camref;
    string cam;
    string calibfile;
    if (cameragroupid.compare(a) == 0)
    {
        camref = slavemodel.box1.camera_ref;
        cam = slavemodel.box1.camera;
        calibfile = projectpath + "/calcu_calibration/" + slavemodel.box1.box_camera_calib_file;
        calibfile = LocalFileOperation::toBackSlashStr(QString::fromLocal8Bit(calibfile.c_str())).toLocal8Bit().constData();
        ret = true;
    }
    a[0] = slavemodel.box2.boxindex;
    if (cameragroupid.compare(a) == 0)
    {
        camref = slavemodel.box2.camera_ref;
        cam = slavemodel.box2.camera;
        calibfile = projectpath + "/calcu_calibration/" + slavemodel.box2.box_camera_calib_file;
        calibfile = LocalFileOperation::toBackSlashStr(QString::fromLocal8Bit(calibfile.c_str())).toLocal8Bit().constData();
        ret = true;
    }
    if (ret == false)
    {
        emit finish(threadid, 1, tunnelid, QString(this->outputfile.c_str()), 0);
        return;
    }
    string output_file = projectpath + "/mid_calcu/" + outputfile;
    qDebug() << "****calcu output file" << output_file.c_str() << "start****";

    LzSerialStorageAcqui * lzacqui_L = new LzSerialStorageAcqui();                               //左相机流式存储类
    lzacqui_L->setting(250, 1024*1024*200, true);
    LzSerialStorageAcqui * lzacqui_R = new LzSerialStorageAcqui();                               //右相机流式存储类
    lzacqui_R->setting(250, 1024*1024*200, true);
    LzSerialStorageMat * lzstore_mat = new LzSerialStorageMat();
    lzstore_mat->setting(250, 1024*1024*50, true);
    // @author范翔加,如果不是从中断位置继续计算,创建文件
    if (!isfrominterruptfile)
        lzstore_mat->createFile(output_file.c_str(), sizeof(DataHead));
    bool isopen = lzstore_mat->openFile(output_file.c_str());
    if (!isopen)
    {
        delete lzacqui_L;
        delete lzacqui_R;
        delete lzstore_mat;
        emit finish(threadid, 5, tunnelid, QString(this->outputfile.c_str()), 0);
        return;
    }

    if (!isfrominterruptfile)
        lzstore_mat->writeHead((char*)&datahead);

    LzCalculator Lz_c;

    std::list<CheckedItem>::iterator todoit = todocalfilelist.begin();
    __int64 start_num_L = 0;         //指明左相机起始帧号
    __int64 start_num_R = 0;         //指明右相机起始帧号
    __int64 cunt_num = 100;          //指明一共多少帧
    __int64 start_num_store = 0;     //指明存储的起始帧号
    __int64 tmp_end_num = 0;

    while (todoit != todocalfilelist.end())
    {
        char seqnostr[6];
        sprintf(seqnostr, "%d", (*todoit).seqno);
        (*todoit).getStartEndVal(camref, start_num_L, tmp_end_num );
        (*todoit).getStartEndVal(cam, start_num_R, tmp_end_num );
        start_num_store = (*todoit).start_frame_master;
        cunt_num = (*todoit).end_frame_master - start_num_store + 1;

        // @author 范翔，如果从中断位置开始计算，比较interruptedfc是否在这个CheckedItem中
        if (isfrominterruptfile)
        {
            if (interruptedfc >= start_num_store && interruptedfc <= start_num_store + cunt_num)
            {
                // 从下一帧开始
                start_num_L = (interruptedfc + 1 - start_num_store) + start_num_L;
                start_num_R = (interruptedfc + 1 - start_num_store) + start_num_R;
                start_num_store = interruptedfc + 1;
                cunt_num = (*todoit).end_frame_master - start_num_store + 1;
                // 下个CheckedItem不再做检查
                isfrominterruptfile = false;
            }
            else
                continue;
        }

        string L_img_file = projectpath + "/collect/" + seqnostr + "_" + (*todoit).filename_prefix + "_" + camref + ".dat";       //左相机文件
        string R_img_file = projectpath + "/collect/" + seqnostr + "_" + (*todoit).filename_prefix + "_" + cam + ".dat";       //右相机文件
        //【注意！】20150120【TmpData文件】
        //string L_img_file = projectpath + "/collect/" + seqnostr + "_" + (*todoit).filename_prefix + "_" + camref + ".tmpdat";       //左相机文件
        //string R_img_file = projectpath + "/collect/" + seqnostr + "_" + (*todoit).filename_prefix + "_" + cam + ".tmpdat";       //右相机文件
        try {

            bool isopenl = lzacqui_L->openFile(L_img_file.c_str());
            bool isopenr = lzacqui_R->openFile(R_img_file.c_str());

            if (!isopenl || !isopenr)
            {
                delete lzacqui_L;
                delete lzacqui_R;
                delete lzstore_mat;
                emit finish(threadid, 7, tunnelid, QString(this->outputfile.c_str()), 0);
                return;
            }

            Lz_c.init(calibfile.c_str());               //载入标定文件
            std::vector<BLOCK_KEY> key_L = lzacqui_L->readKeys();
            std::vector<BLOCK_KEY> key_R = lzacqui_R->readKeys();
            Mat L_img,R_img;//,Full_L_img,Full_R_img;
            Mat _Ledge,_Redge;
            BlockInfo Info;
            Info.key = start_num_store;
            Info.isvalid = true;
			Info.seqindex = start_num_store;
			Info.size = sizeof(BlockInfo);
            currentcalcfc = start_num_store;
            
            bool ifread1 = true;
            bool ifread2 = true;

            ifread1 = lzacqui_L->retrieveFrame(start_num_L);
            ifread2 = lzacqui_R->retrieveFrame(start_num_R);

            if (!ifread1 || !ifread2)
            {
                // 关闭文件
                lzacqui_L->closeFile();
                lzacqui_R->closeFile();
                lzstore_mat->closeFile();
                // 删除文件Handle
                delete lzacqui_L;
                delete lzacqui_R;
                delete lzstore_mat;
                emit finish(threadid, 8, tunnelid, QObject::tr(outputfile.c_str()), Info.key);
                return;            
            }

            for(int i = 0; i<cunt_num; i++)
            {
                ifread1 = lzacqui_L->readFrame(L_img);
                ifread2 = lzacqui_R->readFrame(R_img);

                //【注意！】20150120【TmpData文件】
                //modify 2015.1.20
                //ifread1 = lzacqui_L->readMat(L_img);
                //ifread2 = lzacqui_R->readMat(R_img);
                //imwrite("D:/left.jpg", L_img);
                //imwrite("D:/right.jpg", R_img);
                //waitKey(0);
				//////////@todo

                /*	Full_L_img = Mat::zeros(1280,1024,L_img.type());
				Full_R_img = Mat::zeros(1280,1024,R_img.type());
				L_img.copyTo(Full_L_img.colRange(512,1024));
				R_img.copyTo(Full_R_img.colRange(0,512));*/
                if (ifread1 && ifread2)
                {
                    //Lz_c.loadImgMat(L_img,R_img);
                    Lz_c.calc(L_img,R_img, _Ledge,_Redge);
                }
                else
                    Lz_c.is_valid = false;
                if (Lz_c.is_valid == false)
                    Info.isvalid = false;
                else
                    Info.isvalid = true;
                lzstore_mat->writeMat(Lz_c._rect_Pnts3d, Info, true);                                 //写入Mat还有问题，须改！

                // log
                log(QString("FrameNo %1 readframe1 & readframe2 %2 %3, LzCaculator %4").arg(Info.key).arg(ifread1).arg(ifread2).arg(Lz_c._rect_Pnts3d.cols));

                Lz_c.is_valid = false;
                Lz_c._rect_Pnts3d = Mat::zeros(0,0,0);
                //qDebug() << Info.key;
                currentcalcfc = Info.key;

                emit statusShow(threadid, Info.key, tunnelid, QObject::tr(outputfile.c_str()));

                // 是否终止计算，传递当前计算帧号
                if (ifsuspend)
                {
                    // 关闭文件
                    lzacqui_L->closeFile();
                    lzacqui_R->closeFile();
                    lzstore_mat->closeFile();
                    // 删除文件Handle
                    delete lzacqui_L;
                    delete lzacqui_R;
                    delete lzstore_mat;
                    emit finish(threadid, -1, tunnelid, QObject::tr(outputfile.c_str()), Info.key);
                    return;
                }

                Info.key++;
            }
            lzacqui_L->closeFile();
            lzacqui_R->closeFile();
            lzstore_mat->closeFile();
        }
        catch (LzException & ex)
        {
            delete lzacqui_L;
            delete lzacqui_R;
            delete lzstore_mat;
            emit finish(threadid, 6, tunnelid, QString("双目计算文件%1时异常%2").arg(QObject::tr(outputfile.c_str())).arg(ex.getErrDescribe().c_str()), 0);
            return;
        }

        todoit++;
    }

    delete lzacqui_L;
    delete lzacqui_R;
    delete lzstore_mat;
    qDebug() << "****calcu output file" << output_file.c_str() << "end****";

    emit finish(threadid, 0, tunnelid, QString(this->outputfile.c_str()), currentcalcfc);
}

/**
 * 【双线程计算】双目RT计算线程类
 * 由车底4个相机拍摄的图片计算车厢相对于分中坐标系下的的RT
 * @author 范翔 唐书航
 * @version 1.0.0
 * @date 2015-05-26
 */
LzCalculateTVRTThread_V2::LzCalculateTVRTThread_V2(QObject * parent, int inithreadid) : LzCalculateThread(parent, inithreadid), slavemodel(1.0)
{

}

LzCalculateTVRTThread_V2::~LzCalculateTVRTThread_V2()
{

}

/**
 * 计算函数(对应一条隧道的一组相机） 
 * @param const std::list<CheckedItem> & todofilelist 计算范围——这条隧道对应的实际采集文件及帧有效范围，
 *									见module/datastructure/checkedtask.h
 * @param string projectpath 工程路径 
 * @param int newtunnelid 当前隧道ID
 * @param SlaveModel & slavemodel [注意!][slavemodel的box1,box2的顺序必须为先Q后R]
 * @param string outputfile输出第x组相机的文件名
 * @param DataHead & outputdatahead 输出文件头
 */
void LzCalculateTVRTThread_V2::init(string projectpath, int newtunnelid, std::list<CheckedItem> & todofilelist, string cameragroupindex, SlaveModel & initmodel, string outputfile, DataHead & outputdatahead, bool isinterrupt, qint64 newinterruptfc)
{
    this->projectpath = projectpath;
    this->tunnelid = newtunnelid;
    this->todocalfilelist = todofilelist;
    this->slavemodel = initmodel;
    this->cameragroupid = cameragroupindex;
    this->datahead = outputdatahead;
    this->outputfile = outputfile;

    initLogger(outputfile + ".log", "tangshuhang");

    this->isfrominterruptfile = isinterrupt;
    this->interruptedfc = newinterruptfc;
    ifsuspend = false;
}

void LzCalculateTVRTThread_V2::run()
{
    qDebug() << QObject::tr("单隧道RT双目计算线程-双线程版本") << QObject::tr(projectpath.c_str()) << "/mid_calcu/" << QObject::tr(outputfile.c_str());
    std::cout << "单隧道RT双目计算" << std::endl;

    bool ret = false;
    char a[2];
    a[0] = slavemodel.box1.boxindex;
    a[1] = '\0';
    string camref;
    string cam;
    string calibfile;
    if (cameragroupid.compare(a) == 0)
    {
        camref = slavemodel.box1.camera_ref;
        cam = slavemodel.box1.camera;
        calibfile = projectpath + "/calcu_calibration/" + slavemodel.box1.box_camera_calib_file;
        calibfile = LocalFileOperation::toBackSlashStr(QString::fromLocal8Bit(calibfile.c_str())).toLocal8Bit().constData();
        ret = true;
    }
    a[0] = slavemodel.box2.boxindex;
    if (cameragroupid.compare(a) == 0)
    {
        camref = slavemodel.box2.camera_ref;
        cam = slavemodel.box2.camera;
        calibfile = projectpath + "/calcu_calibration/" + slavemodel.box2.box_camera_calib_file;
        calibfile = LocalFileOperation::toBackSlashStr(QString::fromLocal8Bit(calibfile.c_str())).toLocal8Bit().constData();
        ret = true;
    }
    if (ret == false)
    {
        emit finish(threadid, 1, tunnelid, QString(this->outputfile.c_str()), 0);
        return;
    }

    string Pnts_out_file = projectpath + "/mid_calcu/" + outputfile;
    qDebug() << "****calcu output file" << Pnts_out_file.c_str() << "start****";

    LzSerialStorageAcqui * lzacqui_L = new LzSerialStorageAcqui();                        //组1左相机流式存储类
    lzacqui_L->setting(250, 1024*1024*200, true);
    LzSerialStorageAcqui * lzacqui_R = new LzSerialStorageAcqui();                        //组1右相机流式存储类
    lzacqui_R->setting(250, 1024*1024*200, true);

    bool isopen_pnts = false;

    LzSerialStorageMat * lzstore_Pnts_mat = new LzSerialStorageMat();                     //R相机地面三维点存储类
    lzstore_Pnts_mat->setting(250, 1024*1024*50, true);

    // @author范翔加,如果不是从中断位置继续计算,创建文件
    if (!isfrominterruptfile)
    {
        lzstore_Pnts_mat->createFile(Pnts_out_file.c_str(),sizeof(DataHead));
    }
	isopen_pnts = lzstore_Pnts_mat->openFile(Pnts_out_file.c_str());

    if (!isopen_pnts)
    {
        // 删除文件Handle
        delete lzacqui_L;
        delete lzacqui_R;
        delete lzstore_Pnts_mat;
        emit finish(threadid, 5, tunnelid, QString(this->outputfile.c_str()), 0);
        return;
    }

    if (!isfrominterruptfile)
    {
	    lzstore_Pnts_mat->writeHead((char*)&datahead);
    }

    __int64 start_num_L = 0;         //指明左相机起始帧号
    __int64 start_num_R = 0;         //指明右相机起始帧号
    __int64 cunt_num = 100;          //指明一共多少帧
    __int64 start_num_store = 0;     //指明存储的起始帧号
    __int64 tmp_end_num = 0;

    std::list<CheckedItem>::iterator todoit = todocalfilelist.begin();

    while (todoit != todocalfilelist.end())
    {
        char seqnostr[6];
        sprintf(seqnostr, "%d", (*todoit).seqno);

        (*todoit).getStartEndVal(camref, start_num_L, tmp_end_num );
        (*todoit).getStartEndVal(cam, start_num_R, tmp_end_num );
        start_num_store = (*todoit).start_frame_master;
        cunt_num = (*todoit).end_frame_master - start_num_store + 1;

        // @author 范翔，如果从中断位置开始计算，比较interruptedfc是否在这个CheckedItem中
        if (isfrominterruptfile)
        {
            if (interruptedfc >= start_num_store && interruptedfc <= start_num_store + cunt_num)
            {
                // 从下一帧开始
                start_num_L = (interruptedfc + 1 - start_num_store) + start_num_L;
                start_num_R = (interruptedfc + 1 - start_num_store) + start_num_R;

                start_num_store = interruptedfc + 1;
                cunt_num = (*todoit).end_frame_master - start_num_store + 1;
                // 下个CheckedItem不再做检查
                isfrominterruptfile = false;
            }
            else
                continue;
        }

        string L_img_file = projectpath + "/collect/" + seqnostr + "_" + (*todoit).filename_prefix + "_" + camref + ".dat";       //左相机文件
        string R_img_file = projectpath + "/collect/" + seqnostr + "_" + (*todoit).filename_prefix + "_" + cam + ".dat";       //右相机文件

        // 开始计算
        Vector<Point3f> calibed_pnts3d;

        Lz_RT_Clac Cam_Img;                                                            //生成两组相机计算类 得到两组三维点
        Cam_Img.init(calibfile.c_str());

        try {
            bool isopenl = lzacqui_L->openFile(L_img_file.c_str());
            bool isopenr = lzacqui_R->openFile(R_img_file.c_str());  

            if (!isopenl || !isopenr)
            {
                // 删除文件Handle
                delete lzacqui_L;
                delete lzacqui_R;
                delete lzstore_Pnts_mat;
                emit finish(threadid, 7, tunnelid, QString(this->outputfile.c_str()), 0);
                return;
            }

            std::vector<BLOCK_KEY> key_L = lzacqui_L->readKeys();
            std::vector<BLOCK_KEY> key_R = lzacqui_R->readKeys();

            BlockInfo Info;
            Info.key = start_num_store; 
            Info.seqindex = start_num_store;
            Info.size = sizeof(BlockInfo);

            bool ifread1 = true;
            bool ifread2 = true;

            ifread1 = lzacqui_L->retrieveFrame(start_num_L);
            ifread2 = lzacqui_R->retrieveFrame(start_num_R);

            if (!ifread1 || !ifread2)
            {
                // 关闭文件
                lzacqui_L->closeFile();
                lzacqui_R->closeFile();
                lzstore_Pnts_mat->closeFile();
                // 删除文件Handle
                delete lzacqui_L;
                delete lzacqui_R;
                delete lzstore_Pnts_mat;
                emit finish(threadid, 8, tunnelid, QObject::tr(outputfile.c_str()), Info.key);

                return;            
            }

            Mat L_mat,R_mat;
            Mat L_edge,R_edge;

            currentcalcfc = start_num_store;

            bool tmp = true;
            for(int i=0 ;i<cunt_num; i++)
            {
                Mat RT = Mat(1, 3, CV_64FC4);

                ifread1 = lzacqui_L->readFrame(L_mat);
                ifread2 = lzacqui_R->readFrame(R_mat);
                if (ifread1 && ifread2)
                {
                    Cam_Img.calc(L_mat, R_mat, L_edge, R_edge, 1);
                }
                else
                    tmp = false;
                
                lzstore_Pnts_mat->writeMat(Cam_Img._rect_Pnts3d,Info,true);

                // log
                log(QString("FrameNo %1 readframe1 & readframe2 %2 %3, LzCaculator %4").arg(Info.key).arg(ifread1).arg(ifread2).arg(Cam_Img._rect_Pnts3d.cols));

                currentcalcfc = Info.key;
                emit statusShow(threadid, Info.key, tunnelid, QObject::tr(outputfile.c_str()));

                // 是否终止计算，传递当前计算帧号
                if (ifsuspend)
                {
                    // 关闭文件
                    lzacqui_L->closeFile();
                    lzacqui_R->closeFile();
                    lzstore_Pnts_mat->closeFile();
                    // 删除文件Handle
                    delete lzacqui_L;
                    delete lzacqui_R;
                    delete lzstore_Pnts_mat;
                    emit finish(threadid, -1, tunnelid, QObject::tr(outputfile.c_str()), Info.key);

                    return;
                }
	            Cam_Img.rail_P = Point2d(0,0);
                Info.key++;
            }
            lzacqui_L->closeFile();
            lzacqui_R->closeFile();
	        lzstore_Pnts_mat->closeFile();
        }
        catch (LzException & ex)
        {
            delete lzacqui_L;
            delete lzacqui_R;
            delete lzstore_Pnts_mat;
            emit finish(threadid, 6, tunnelid, QString("双目计算文件%1时异常%2").arg(QObject::tr(outputfile.c_str())).arg(ex.getErrDescribe().c_str()), 0);
            return;
        }

        todoit++;
    }
    
    delete lzacqui_L;
    delete lzacqui_R;
    delete lzstore_Pnts_mat;

    emit finish(threadid, 0, tunnelid, QString(this->outputfile.c_str()), currentcalcfc);
}

/**
 * 【单线程计算】双目RT计算线程类
 * 由车底4个相机拍摄的图片计算车厢相对于分中坐标系下的的RT
 * @author 范翔 唐书航
 * @version 1.0.0
 * @date 2014-08-05
 */
LzCalculateTVRTThread::LzCalculateTVRTThread(QObject * parent, int inithreadid) : LzCalculateThread(parent, inithreadid), slavemodel(1.0)
{

}

LzCalculateTVRTThread::~LzCalculateTVRTThread()
{

}

/**
 * 计算函数(对应一条隧道的一组相机） 
 * @param const std::list<CheckedItem> & todofilelist 计算范围——这条隧道对应的实际采集文件及帧有效范围，
 *									见module/datastructure/checkedtask.h
 * @param string projectpath 工程路径 
 * @param int newtunnelid 当前隧道ID
 * @param SlaveModel & slavemodel [注意!][slavemodel的box1,box2的顺序必须为先Q后R]
 * @param string outputfile输出第x组相机的文件名
 * @param DataHead & outputdatahead 输出文件头
 */
void LzCalculateTVRTThread::init(string projectpath, int newtunnelid, std::list<CheckedItem> & todofilelist, SlaveModel & initmodel, string outputRFile, string outputTFile, DataHead & outputdatahead, bool isinterrupt, qint64 newinterruptfc)
{
    this->projectpath = projectpath;
    this->tunnelid = newtunnelid;
    this->todocalfilelist = todofilelist;
    this->slavemodel = initmodel;
    this->datahead = outputdatahead;
    this->inputcalibmat = inputcalibmat; 
    this->outputfile = outputRFile;
    this->outputTFile = outputTFile;

    this->isfrominterruptfile = isinterrupt;
    this->interruptedfc = newinterruptfc;
    ifsuspend = false;
}

void LzCalculateTVRTThread::run()
{
    qDebug() << QObject::tr("单隧道RT双目计算线程-单线程版本") << QObject::tr(projectpath.c_str()) << "/mid_calcu/" << QObject::tr(outputfile.c_str());
    std::cout << "单隧道RT双目计算" << std::endl;

    bool ret = false;

    string box1camref = slavemodel.box1.camera_ref;
    string box1cam = slavemodel.box1.camera;
    string box2camref = slavemodel.box2.camera_ref;
    string box2cam = slavemodel.box2.camera;
    string calibe_file_Q = projectpath + "/calcu_calibration/" + slavemodel.box1.box_camera_calib_file;
    calibe_file_Q = LocalFileOperation::toBackSlashStr(QString::fromLocal8Bit(calibe_file_Q.c_str())).toLocal8Bit().constData();
    string calibe_file_R = projectpath + "/calcu_calibration/" + slavemodel.box2.box_camera_calib_file;
    calibe_file_R = LocalFileOperation::toBackSlashStr(QString::fromLocal8Bit(calibe_file_R.c_str())).toLocal8Bit().constData();
    //string R_out_file =  projectpath + "/mid_calcu/" + outputfile;                         //R矩阵的输出文件
    //string T_out_file =  projectpath + "/mid_calcu/" + outputTFile;                        //T矩阵的输出文件
    QString tmpoutputname = QString::fromLocal8Bit(outputfile.c_str());
    tmpoutputname = tmpoutputname.left(tmpoutputname.length() - 8);
    QByteArray tmpba = tmpoutputname.toLocal8Bit();
    outputfile = tmpba.constData();
    string RT_out_file = projectpath + "/mid_calcu/" + outputfile + "_RT.mdat";
    string R_Pnts_out_file = projectpath + "/mid_calcu/" + outputfile + "_R.mdat";   //R相机地面三维点的输出文件
	string Q_Pnts_out_file = projectpath + "/mid_calcu/" + outputfile + "_Q.mdat";   //Q相机地面三维点的输出文件
    string QR_Pnts_out_file = projectpath + "/mid_calcu/" + outputfile + "_QR.mdat";
    __int64 snum_1L=0,snum_1R=0,snum_2L=0,snum_2R=0,cunt_num=15,out_snum=0,tmp_end_num=0;  //定义左右1,2相机的起始位置

    LzSerialStorageAcqui * lzacqui_1L = new LzSerialStorageAcqui();                        //组1左相机流式存储类
    lzacqui_1L->setting(250, 1024*1024*200, true);
    LzSerialStorageAcqui * lzacqui_1R = new LzSerialStorageAcqui();                        //组1右相机流式存储类
    lzacqui_1R->setting(250, 1024*1024*200, true);
    LzSerialStorageAcqui * lzacqui_2L = new LzSerialStorageAcqui();                        //组2左相机流式存储类
    lzacqui_2L->setting(250, 1024*1024*200, true);
    LzSerialStorageAcqui * lzacqui_2R = new LzSerialStorageAcqui();                        //组2右相机流式存储类
    lzacqui_2R->setting(250, 1024*1024*200, true);

    string QL_img_file;
    string QR_img_file;
    string RL_img_file;
    string RR_img_file;

    bool isopen = false;
    bool isopen_Rpnts = false;
    bool isopen_Qpnts = false;
    bool isopen_QRpnts = false;
    LzSerialStorageMat * lzstore_RT_mat = new LzSerialStorageMat();
    lzstore_RT_mat->setting(250, 1024*1024, true);
    LzSerialStorageMat * lzstore_RPnts_mat = new LzSerialStorageMat();                     //R相机地面三维点存储类
    lzstore_RPnts_mat->setting(250, 1024*1024*50, true);
    LzSerialStorageMat * lzstore_QPnts_mat = new LzSerialStorageMat(); 
    lzstore_QPnts_mat->setting(250, 1024*1024*50, true);
    LzSerialStorageMat * lzstore_QRPnts_mat = new LzSerialStorageMat(); 
    lzstore_QRPnts_mat->setting(250, 1024*1024*50, true);

    // @author范翔加,如果不是从中断位置继续计算,创建文件
    if (!isfrominterruptfile)
    {
        lzstore_RT_mat->createFile(RT_out_file.c_str(),sizeof(DataHead));
        lzstore_RPnts_mat->createFile(R_Pnts_out_file.c_str(),sizeof(DataHead));
        lzstore_QPnts_mat->createFile(Q_Pnts_out_file.c_str(),sizeof(DataHead));
        lzstore_QRPnts_mat->createFile(QR_Pnts_out_file.c_str(), sizeof(DataHead));
    }
    isopen = lzstore_RT_mat->openFile(RT_out_file.c_str());
	isopen_Rpnts = lzstore_RPnts_mat->openFile(R_Pnts_out_file.c_str());
    isopen_Qpnts = lzstore_QPnts_mat->openFile(Q_Pnts_out_file.c_str());
    isopen_QRpnts = lzstore_QRPnts_mat->openFile(QR_Pnts_out_file.c_str());

    if (!isopen || !isopen_Rpnts || !isopen_Qpnts || !isopen_QRpnts)
    {
        // 删除文件Handle
        delete lzacqui_1L;
        delete lzacqui_1R;
        delete lzacqui_2L;
        delete lzacqui_2R;
        delete lzstore_RT_mat;
        delete lzstore_RPnts_mat;
        delete lzstore_QPnts_mat;
        delete lzstore_QRPnts_mat;
        emit finish(threadid, 5, tunnelid, QString(this->outputfile.c_str()), 0);
        return;
    }

    if (!isfrominterruptfile)
    {
        lzstore_RT_mat->writeHead((char*)&datahead);
	    lzstore_RPnts_mat->writeHead((char*)&datahead);
        lzstore_QPnts_mat->writeHead((char*)&datahead);
        lzstore_QRPnts_mat->writeHead((char*)&datahead);
    }
    //std::cout<<isopen<<" "<<isopen<<std::endl;
    //string calib_pnts_file = projectpath + "/calcu_calibration/calib_pnts_file.dat"; //标准三维点的输入文件
     
    std::list<CheckedItem>::iterator todoit = todocalfilelist.begin();

    while (todoit != todocalfilelist.end())
    {
        char seqnostr[6];
        sprintf(seqnostr, "%d", (*todoit).seqno);

        (*todoit).getStartEndVal(box1camref, snum_1L, tmp_end_num );
        (*todoit).getStartEndVal(box1cam, snum_1R, tmp_end_num );
        (*todoit).getStartEndVal(box2camref, snum_2L, tmp_end_num );
        (*todoit).getStartEndVal(box2cam, snum_2R, tmp_end_num );
        out_snum = (*todoit).start_frame_master;
        cunt_num = (*todoit).end_frame_master - out_snum + 1;

        // @author 范翔，如果从中断位置开始计算，比较interruptedfc是否在这个CheckedItem中
        if (isfrominterruptfile)
        {
            if (interruptedfc >= out_snum && interruptedfc <= out_snum + cunt_num)
            {
                // 从下一帧开始
                snum_1L = (interruptedfc + 1 - out_snum) + snum_1L;
                snum_1R = (interruptedfc + 1 - out_snum) + snum_1R;
                snum_2L = (interruptedfc + 1 - out_snum) + snum_2L;
                snum_2R = (interruptedfc + 1 - out_snum) + snum_2R;

                out_snum = interruptedfc + 1;
                cunt_num = (*todoit).end_frame_master - out_snum + 1;
                // 下个CheckedItem不再做检查
                isfrominterruptfile = false;
            }
            else
                continue;
        }

        QL_img_file = projectpath + "/collect/" + seqnostr + "_" + (*todoit).filename_prefix + "_" + box1camref + ".dat";    //左相机文件
        QR_img_file = projectpath + "/collect/" + seqnostr + "_" + (*todoit).filename_prefix + "_" + box1cam + ".dat";       //右相机文件

        RL_img_file = projectpath + "/collect/" + seqnostr + "_" + (*todoit).filename_prefix + "_" + box2camref + ".dat";    //左相机文件
        RR_img_file = projectpath + "/collect/" + seqnostr + "_" + (*todoit).filename_prefix + "_" + box2cam + ".dat";       //右相机文件

        // 开始计算
        Vector<Point3f> calibed_pnts3d;

        Mat calib_mat;
        Lz_RT_Clac Cam_Q,Cam_R;                                                            //生成两组相机计算类 得到两组三维点
        Cam_Q.init(calibe_file_Q.c_str());
        Cam_R.init(calibe_file_R.c_str());

        try {
            bool isopenQl = lzacqui_1L->openFile(QL_img_file.c_str());
            bool isopenQr = lzacqui_1R->openFile(QR_img_file.c_str());  
            bool isopenRl = lzacqui_2L->openFile(RL_img_file.c_str());
            bool isopenRr = lzacqui_2R->openFile(RR_img_file.c_str());

            if (!isopenQl || !isopenQr || !isopenRl || !isopenRr)
            {
                // 删除文件Handle
                delete lzacqui_1L;
                delete lzacqui_1R;
                delete lzacqui_2L;
                delete lzacqui_2R;
                delete lzstore_RT_mat;
                delete lzstore_RPnts_mat;
                delete lzstore_QPnts_mat;
                delete lzstore_QRPnts_mat;
                emit finish(threadid, 7, tunnelid, QString(this->outputfile.c_str()), 0);
                return;
            }

            std::vector<BLOCK_KEY> key_1L = lzacqui_1L->readKeys();
            std::vector<BLOCK_KEY> key_1R = lzacqui_1R->readKeys();
            std::vector<BLOCK_KEY> key_2L = lzacqui_2L->readKeys();
            std::vector<BLOCK_KEY> key_2R = lzacqui_2R->readKeys();

            bool ifread1 = true;
            bool ifread2 = true;
            bool ifread3 = true;
            bool ifread4 = true;
            ifread1 = lzacqui_1L->retrieveFrame(snum_1L);
            ifread2 = lzacqui_1R->retrieveFrame(snum_1R);
            ifread3 = lzacqui_2L->retrieveFrame(snum_2L);
            ifread4 = lzacqui_2R->retrieveFrame(snum_2R);
            if (!ifread1 || !ifread2 || !ifread3 || !ifread4)
            {
                // 关闭文件
                lzacqui_1L->closeFile();
                lzacqui_1R->closeFile();
                lzacqui_2L->closeFile();
                lzacqui_2R->closeFile();
                lzstore_RT_mat->closeFile();
                lzstore_QPnts_mat->closeFile();
	            lzstore_RPnts_mat->closeFile();
                lzstore_QRPnts_mat->closeFile();
                // 删除文件Handle
                delete lzacqui_1L;
                delete lzacqui_1R;
                delete lzacqui_2L;
                delete lzacqui_2R;
                delete lzstore_RT_mat;
                delete lzstore_RPnts_mat;
                delete lzstore_QPnts_mat;
                delete lzstore_QRPnts_mat;
                emit finish(threadid, 8, tunnelid, QObject::tr(outputfile.c_str()), out_snum);

                return;            
            }

            Mat Q_L_mat,Q_R_mat,R_L_mat,R_R_mat;
            Mat Q_L_edge,Q_R_edge,R_L_edge,R_R_edge;
            //Mat R = Mat(3,3,CV_64FC4);
            //Mat T = Mat(3,1,CV_64FC4);
            currentcalcfc = out_snum;

            BlockInfo Info;
            Info.key = out_snum; 
            Info.seqindex = out_snum;
            Info.size = sizeof(BlockInfo);

            bool tmp1 = true;
            bool tmp2 = true;
            for(int i=0 ;i<cunt_num; i++)
            {
                Mat RT = Mat(1, 3, CV_64FC4);

                ifread1 = lzacqui_1L->readFrame(Q_L_mat);
                ifread2 = lzacqui_1R->readFrame(Q_R_mat);
                if (ifread1 && ifread2)
                {
                    Cam_Q.calc(Q_L_mat,Q_R_mat,Q_L_edge,Q_R_edge,1);
                }
                else
                    tmp1 = false;
                ifread1 = lzacqui_2L->readFrame(R_L_mat);
                ifread2 = lzacqui_2R->readFrame(R_R_mat);
                if (ifread1 && ifread2)
                {
                    Cam_R.calc(R_L_mat,R_R_mat,R_L_edge,R_R_edge,2);
                }
                else
                    tmp2 = false;
                if (tmp1 && tmp2)
                {
                    //Comput_RT(Cam_1.pnts3d,Cam_2.pnts3d,calibed_pnts3d, R, T);
                    //lzstore_T_mat->writeMat(T,Info,true);     
                    if(Cam_Q.rail_P.x!=0&&Cam_Q.rail_P.y!=0&&Cam_R.rail_P.x!=0&&Cam_R.rail_P.y!=0)
		            {
			            Cam_Q.valid_rail_P = Cam_Q.rail_P;
			            Cam_R.valid_rail_P = Cam_Q.rail_P;
			            Comput_RT(Cam_Q.valid_rail_P,Cam_R.valid_rail_P,Cam_Q.std_P,Cam_R.std_P,RT);
                        Info.isvalid = true;
                        lzstore_RT_mat->writeMat(RT,Info,true);

		            }  
		            else if(Cam_Q.valid_rail_P.x!=0&&Cam_Q.valid_rail_P.y!=0&&Cam_R.valid_rail_P.x!=0&&Cam_R.valid_rail_P.y!=0)
		            {
		                Comput_RT(Cam_Q.valid_rail_P,Cam_R.valid_rail_P,Cam_Q.std_P,Cam_R.std_P,RT);
                        Info.isvalid = false;
                        lzstore_RT_mat->writeMat(RT,Info,true);
                    }
                    Mat QR_pnts = Mat::zeros(3, (Cam_Q._rect_Pnts3d.cols + Cam_R._rect_Pnts3d.cols), Cam_Q._rect_Pnts3d.type());
                    Cam_Q._rect_Pnts3d.copyTo(QR_pnts.colRange(0, Cam_Q._rect_Pnts3d.cols));
                    Cam_R._rect_Pnts3d.copyTo(QR_pnts.colRange(Cam_Q._rect_Pnts3d.cols, QR_pnts.cols));
		            lzstore_RPnts_mat->writeMat(Cam_R._rect_Pnts3d,Info,true);
		            lzstore_QPnts_mat->writeMat(Cam_Q._rect_Pnts3d,Info,true);
                    lzstore_QRPnts_mat->writeMat(QR_pnts, Info, true);
                    currentcalcfc = Info.key;
                    emit statusShow(threadid, Info.key, tunnelid, QObject::tr(outputfile.c_str()));

                    // 是否终止计算，传递当前计算帧号
                    if (ifsuspend)
                    {
                        // 关闭文件
                        lzacqui_1L->closeFile();
                        lzacqui_1R->closeFile();
                        lzacqui_2L->closeFile();
                        lzacqui_2R->closeFile();
                        lzstore_RT_mat->closeFile();
                        lzstore_QPnts_mat->closeFile();
	                    lzstore_RPnts_mat->closeFile();
                        lzstore_QRPnts_mat->closeFile();
                        // 删除文件Handle
                        delete lzacqui_1L;
                        delete lzacqui_1R;
                        delete lzacqui_2L;
                        delete lzacqui_2R;
                        delete lzstore_RT_mat;
                        delete lzstore_RPnts_mat;
                        delete lzstore_QPnts_mat;
                        delete lzstore_QRPnts_mat;
                        emit finish(threadid, -1, tunnelid, QObject::tr(outputfile.c_str()), Info.key);

                        return;
                    }
                }
	            Cam_Q.rail_P = Point2d(0,0);
	            Cam_R.rail_P = Point2d(0,0);
                Info.key++;
            }
            lzacqui_1L->closeFile();
            lzacqui_1R->closeFile();
            lzacqui_2L->closeFile();
            lzacqui_2R->closeFile();
            lzstore_RT_mat->closeFile();
	        lzstore_QPnts_mat->closeFile();
	        lzstore_RPnts_mat->closeFile();
            lzstore_QRPnts_mat->closeFile();
        }
        catch (LzException & ex)
        {
            delete lzacqui_1L;
            delete lzacqui_1R;
            delete lzacqui_2L;
            delete lzacqui_2R;
            delete lzstore_RT_mat;
            delete lzstore_RPnts_mat;
            delete lzstore_QPnts_mat;
            delete lzstore_QRPnts_mat;
            emit finish(threadid, 6, tunnelid, QString("双目计算文件%1时异常%2").arg(QObject::tr(outputfile.c_str())).arg(ex.getErrDescribe().c_str()), 0);
            return;
        }

        todoit++;
    }
    
    delete lzacqui_1L;
    delete lzacqui_1R;
    delete lzacqui_2L;
    delete lzacqui_2R;
    delete lzstore_RT_mat;
    delete lzstore_RPnts_mat;
    delete lzstore_QPnts_mat;
    delete lzstore_QRPnts_mat;

    emit finish(threadid, 0, tunnelid, QString(this->outputfile.c_str()), currentcalcfc);
}

void LzCalculateTVRTThread::Comput_RT(Point2d& Q_p, Point2d& R_p, Point2d& std_Q, Point2d& std_R,Mat &RT)
{
	double rail_dist = 1435;                       //定义钢轨间距
	double cur_angle,std_angle;                    //定义当前角度和标准角度
	double theta = 0;
	cur_angle = atan2((Q_p.x-R_p.x),rail_dist)*180/3.14159265;
	std_angle = atan2((std_Q.x-std_R.x),rail_dist)*180/3.14159265;
	theta = cur_angle - std_angle; 
	*RT.ptr<double>(0,0) = std_R.y - R_p.y;         //以左轨R作为旋转平移标准
	*RT.ptr<double>(0,1) = (R_p.y/rail_dist)*(Q_p.x-R_p.x) - (std_Q.x - std_R.x)/2;
	*RT.ptr<double>(0,2) = theta;
	std::cout<<theta<<endl;
}

void LzCalculateTVRTThread::loadcalib_pnts(Vector<Point3f> &calibed_pnts3d, Mat calib_mat)
{
    Point3f point;
    for(int i=0; i<calib_mat.cols; i++)
    {
        point.x = float(*calib_mat.ptr<double>(i,0));
        point.y = float(*calib_mat.ptr<double>(i,1));
        point.z = float(*calib_mat.ptr<double>(i,2));
        calibed_pnts3d.push_back(point);
    }
}

/**
 * 主控融合计算线程类
 * 由车底4个相机拍摄的图片计算车厢相对于分中坐标系下的的RT
 * @author 范翔 唐书航
 * @version 1.0.0
 * @date 2014-08-05
 */
LzCalculateFuseThread::LzCalculateFuseThread(QObject * parent, int inithreadid) : LzCalculateThread(parent, inithreadid)
{
    f = NULL;
}

LzCalculateFuseThread::~LzCalculateFuseThread()
{
    if (f != NULL)
        delete f;
}

/**
 * 计算函数(对应一条隧道） 
 * @param std::vector<string> inputfilenames
 * @param string projectpath 工程路径 
 * @param string outputfile输出第x组相机的文件名
 * @param DataHead & outputdatahead 输出文件头
 */
void LzCalculateFuseThread::init(string projectpath, std::vector<string> inputfilenames, __int64 startnum, __int64 endnum, string outputfile, string outputextractheightsfile, DataHead & outputdatahead, bool isinterrupt, qint64 newinterruptfc)
{
    this->projectpath = projectpath;
    this->inputfilenames = inputfilenames;
    this->start_num = startnum;
    this->frame_cunt = endnum - startnum + 1;
    this->outputfile = outputfile;
    this->datahead = outputdatahead;
    this->outputextractheightsfile = outputextractheightsfile;
    
    this->isfrominterruptfile = isinterrupt;
    this->interruptedfc = newinterruptfc;
    ifsuspend = false;
}

/**
 * 中断线程、外部调用
 */
void LzCalculateFuseThread::suspend()
{
    LzCalculateThread::suspend();
    if (f != NULL)
        f->suspend();
}

void LzCalculateFuseThread::receiveFuseStatus(qint64 currentfc, bool ifsuspend)
{
    emit statusShow(1, currentfc, -1, QObject::tr(outputfile.c_str()));
    if (ifsuspend)
        emit finish(threadid, -1, -1, QString(this->outputfile.c_str()), currentfc);
}

void LzCalculateFuseThread::run()
{
    string heightsfile = projectpath + "/output_heights.xml";

    string out_fusefile = projectpath + "/fuse_calcu/" + outputfile;
    string out_extractfile = projectpath + "/syn_data/" + outputextractheightsfile;

    qDebug() << heightsfile.c_str() << out_fusefile.c_str() << out_extractfile.c_str();

    XMLSynthesisHeightsFileLoader * ff3 = new XMLSynthesisHeightsFileLoader(heightsfile.c_str());
    bool ret = ff3->loadFile();
    delete ff3;
    if (!ret)
        emit finish(threadid, 6, -1, QString(this->outputfile.c_str()), 0);

    //std::cout<<ret<<std::endl;
    if (f == NULL)
        f = new Fusion();
    // @author 范翔改 继承自QObject 可以通过消息槽反馈计算进度
    connect(f, SIGNAL(statusShow(qint64, bool)), this, SLOT(receiveFuseStatus(qint64, bool)));
    vector<string> input_file = inputfilenames;
    list<int> Item = OutputHeightsList::getOutputHeightsListInstance()->list();
    //f->init_test();               //供测试时使用
    f->init(input_file);
    int ret1 = f->fuse(Item,out_fusefile,out_extractfile, isfrominterruptfile, start_num, frame_cunt);
    if (ret1 == 0 || ret1 == 1)
        emit finish(threadid, 0, -1, QString(this->outputfile.c_str()), currentcalcfc);
    else if (ret1 != -1) // 包括无数据，打不开结果文件等情况
        emit finish(threadid, ret1, -1, QString(this->outputfile.c_str()), currentcalcfc);
}
