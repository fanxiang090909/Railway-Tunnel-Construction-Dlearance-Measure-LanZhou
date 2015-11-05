#include "lz_acquiz_queue.h"

/**
 * 从控机采集队列类实现
 * 顺序执行接受的消息任务，防止冲突
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2014-7-31
 */
LzAcquizQueue::LzAcquizQueue(QObject * parent) : LzSlaveMsgQueue(parent)
{
    lzacqui = NULL;
    isAcquizing = false;
    hasCamerainit = false;

    // 默认为硬触发采集
    collectionMode = LzCameraCollectingMode::Lz_Camera_HardwareTrigger;

	// 默认曝光时间
	exposureTime = 500;

    this->seq = 0;

    // 采集监控从控显示
    timer = new QTimer();
    timer->setInterval(20000);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimeOut()));
    timer->start();
}

LzAcquizQueue::LzAcquizQueue(LzCameraCollectingMode initmode, QObject * parent) : LzSlaveMsgQueue(parent)
{
    LzAcquizQueue::LzAcquizQueue(parent);

    collectionMode = initmode;
}

LzAcquizQueue::~LzAcquizQueue()
{
    // 如果正在采集
    try {
        if (isAcquizing == true && lzacqui != NULL)
        {
            // 关闭文件
            collect_end_tunnel(false);
            // 关闭相机
            collect_end_line();
        }
    }
    catch (LzException & ex)
    {
        qDebug() << ex.getErrDescribe().c_str();
    }

    if (lzacqui != NULL)
        delete lzacqui;

    if (timer != NULL)
    {
        timer->stop();
        delete timer;
    }
}

bool LzAcquizQueue::getIsAcquizing()
{
    return isAcquizing;
}

/**
 * 任务命令解析
 * 实现父类的纯虚函数
 */
bool LzAcquizQueue::parseMsg(QString msg)
{
    QStringList strList = msg.split(",", QString::SkipEmptyParts);
 
    int msgid = strList.value(0).toInt();
    qDebug() << "msgid= " << msgid;
    if (msgid <= 0)
    {
        switch (msgid)
        {
            case 0: // 网络异常中断时终止采集
            {
                if (isAcquizing)
                {
                    // 关闭文件
                    collect_end_tunnel(false);
                    // 关闭相机
                    collect_end_line();

                    endMsg();
                }
                break;
            }
            case -1101: // 1101的补充消息，关闭上一文件，打开下一文件
            {
                // 关闭上一文件
                collect_end_tunnel(false);

                // 打开下一文件
                collect_start_tunnel();
                // 无需多线程等待结束
                endMsg();
                break;
            }
            case -4202: // 4202的补充消息，开始采集
            {
                collect_start_line();

                collect_start_tunnel();
                // 无需多线程等待结束
                endMsg();
                break;
            }
        }
    }
    else
    {
        switch (msgid)
        {
            case 1003: // 收到软触发采集信号
            {
                collect_one_frame_software_trigger();

                endMsg();

                break;
            }
            case 1101: // 收到出洞信号，相机停止拍摄 1101,slaveid=x.endframe_box1camref=x, endframe_box1cam=x, endframe_box2camref=x, endframe_box2cam=x,numofframes=x, start_mile=x,end_mile=x
            {
                if (strList.length() < 9)
                {
                    qDebug() << tr("解析字符出错") << msg;
                    return false;
                }
                int slaveid = strList.at(1).mid(8).toInt();
                // 四个相机的终止帧号
                _int64 endFrameBox1Camref = strList.at(2).mid(20).toLongLong();
                _int64 endFrameBox1Cam = strList.at(3).mid(17).toLongLong();
                _int64 endFrameBox2Camref = strList.at(4).mid(20).toLongLong();
                _int64 endFrameBox2Cam = strList.at(5).mid(17).toLongLong();
                // 隧道内有效帧数
                int numofframes = strList.at(6).mid(12).toInt();
                // 起始里程
                float startMile = strList.at(7).mid(11).toFloat();
                // 起始里程
                float endMile = strList.at(8).mid(9).toFloat();

                emit signalParsedMsgToSlave(tr("[主控] 命令:出洞，其中传来的有效帧数%1，终止帧号%2,%3,%4,%5，起始里程%6，终止里程%7")
                                                .arg(numofframes).arg(endFrameBox1Camref).arg(endFrameBox1Cam).arg(endFrameBox2Camref).arg(endFrameBox2Cam).arg(startMile).arg(endMile));

                // 出洞信号处理：填回相关信息
                collect_beginOutfromTunnel(endFrameBox1Camref, endFrameBox1Cam, endFrameBox2Camref, endFrameBox2Cam, startMile, endMile);

                // @fanxiang @date 20140803 因为是多线程,分成连个任务,1101和-1101
                // 关闭文件
                //collect_end_tunnel(false);
                // 打开下一文件
                //collect_start_tunnel();

                break;
            }
            case 1103: // 收到预进洞有效、无效信号 1103,isvalid=true(预进洞有效，false预进洞无效),slaveid=x,endframe_box1camref=x, endframe_box1cam=x, endframe_box2camref=x, endframe_box2cam=x. numofframes=x
            {
                if (strList.length() < 8)
                {
                    qDebug() << tr("解析字符出错") << msg;
                    return false;
                }
                QString isvalidstr = strList.at(1).mid(8);
                bool isvalid = false;
                if (isvalidstr.compare("true") == 0)
                    isvalid = true;
                int slaveid = strList.at(2).mid(8).toInt();
                // 四个相机的终止帧号
                _int64 endFrameBox1Camref = strList.at(3).mid(20).toLongLong();
                _int64 endFrameBox1Cam = strList.at(4).mid(17).toLongLong();
                _int64 endFrameBox2Camref = strList.at(5).mid(20).toLongLong();
                _int64 endFrameBox2Cam = strList.at(6).mid(17).toLongLong();
                // 预进洞有效无效帧数
                int numofframes = strList.at(7).mid(12).toInt();
                
                if (isvalid)
                {
                    emit signalParsedMsgToSlave(tr("[主控] 命令:预进洞有效(true)，其中传来的有效帧数%1，终止帧号%2,%3,%4,%5") .arg(numofframes).arg(endFrameBox1Camref).arg(endFrameBox1Cam).arg(endFrameBox2Camref).arg(endFrameBox2Cam));
                    
                    this->collect_CameraStatusFeedback(QString(slaveModel.box1.camera_ref.c_str()), 2, " \0");
                    this->collect_CameraStatusFeedback(QString(slaveModel.box1.camera.c_str()), 2, " \0");
                    this->collect_CameraStatusFeedback(QString(slaveModel.box2.camera_ref.c_str()), 2, " \0");
                    this->collect_CameraStatusFeedback(QString(slaveModel.box2.camera.c_str()), 2, " \0");

                    QString tunnelname = QString::fromLocal8Bit(planTaskIterator->tunnelname.c_str());
                    int seqno = this->seq;

                    QString filenamePrfix;
                    /// TODO 可能出错
                    QString projectname = LzProjectAccess::getLzProjectAccessInstance()->getProjectFilename(LzProjectClass::Collect);
                    QString projectdate = projectname.mid(projectname.length() - 13).left(8);

                    if (planTaskIterator != LzProjectAccess::getLzProjectAccessInstance()->getLzPlanList(LzProjectClass::Collect).end())
                        filenamePrfix = QString("%1_%2_%3").arg(seqno).arg(planTaskIterator->tunnelname.c_str()).arg(projectdate);

                    // 告诉主机从机已进入进洞状态
                    collect_msgFinishIntoTunnel(true, slaveModel.box1.camera_ref.c_str(), 0, 0, filenamePrfix);
                    collect_msgFinishIntoTunnel(true, slaveModel.box1.camera.c_str(), 0, 0, filenamePrfix);
                    collect_msgFinishIntoTunnel(true, slaveModel.box2.camera_ref.c_str(), 0, 0, filenamePrfix);
                    collect_msgFinishIntoTunnel(true, slaveModel.box2.camera.c_str(), 0, 0, filenamePrfix);

                    lzacqui->acqui();
                    
                    endMsg();
                }
                else
                {
                    emit signalParsedMsgToSlave(tr("[主控] 命令:预进洞无效(false)，其中传来的无效帧数%1，终止帧号%2,%3,%4,%5").arg(numofframes).arg(endFrameBox1Camref).arg(endFrameBox1Cam).arg(endFrameBox2Camref).arg(endFrameBox2Cam));

                    if (!hasInitSlaveModel())
                    {
                        emit signalParsedMsgToSlave(QString("[从控] 预进洞无效处理失败，未初始化从机模型SlaveModel"));
                        return false;
                    }

                    collect_beginIntoTunnelNotValid(endFrameBox1Camref, endFrameBox1Cam, endFrameBox2Camref, endFrameBox2Cam);
                }
                break;
            }
            case 1301: // 停止采集当前线路 1301
            {
                emit signalParsedMsgToSlave(tr("[主控] 命令:停止采集当前线路"));
                
                // 出洞信号处理：填回相关信息
                collect_beginOutfromTunnel(100, 100, 100, 100, 0, 0);

                // @fanxiang @date 20140803 因为是多线程,分成连个任务,1301和0
                // 关闭文件
                //collect_end_tunnel(true);
                // 关闭相机
                //collect_end_line();
                break;
            }
            case 1401: // 从机采集复位 1401
            {
                emit signalParsedMsgToSlave(tr("[主控] 命令:从机复位至采集最初状态"));
                
                collect_slave_reset();

                emit signalParsedMsgToSlave(tr("[从控] 执行复位结束"));
                // 给主控返回复位确认
                emit signalMsgToMaster("1402");
                break;
            }
            default: return false; 
        }
    }
    return true;
}

/**
 * 设置触发模式及采集曝光时间
 */
void LzAcquizQueue::setTriggerModeAndExposureTime(LzCameraCollectingMode newtriggermode, int newexposuretime)
{
    this->collectionMode = newtriggermode;
    this->exposureTime = newexposuretime;
    // add 08.22
    this->collectionMode = Lz_Camera_HardwareTrigger;
    // add
}

/**
 * 相机硬件初始化
 * 步骤：
 * （1）配置相机SerialNumber和index(A1A2B1B2...R1R2的对应关系)
 * （2）设置采集模式（连续采集/硬件外同步触发）
 * （3）开启相机
 * @return 是否开启成功，上述步骤如果出错，则捕获异常，通过消息告知主控
 * @author 丁志宇 范翔
 */
bool LzAcquizQueue::cameraInit()
{
    if (!hasInitSlaveModel())
    {
        emit signalParsedMsgToSlave(QString("[从控] 采集类初始化失败，未初始化从机模型SlaveModel"));
        return false;
    }
    // 查找硬件ID号和Serial Number编号
    std::string box1camera_ref = slaveModel.box1.camera_ref; bool hasbox1camera_ref = false;
    std::string box1camera = slaveModel.box1.camera;         bool hasbox1camera = false;
    std::string box2camera_ref = slaveModel.box2.camera_ref; bool hasbox2camera_ref = false;
    std::string box2camera = slaveModel.box2.camera;         bool hasbox2camera = false;

    try {
        
        if (lzacqui == NULL)
            lzacqui = new LzCameraAcquisition(4);

        //connect(lzacqui, SIGNAL(discardfinish( vector<AcquiRet> )), this, SLOT(end_IntoTunnelNotValid( vector<AcquiRet> )));
        //connect(lzacqui, SIGNAL(acquifinish( vector<AcquiRet> )), this, SLOT(end_OutFromTunnel( vector<AcquiRet> )));
        
        connect(lzacqui, SIGNAL(discardfinish(QString, qint64, qint64, QString, qint64, qint64,
                                                    QString, qint64, qint64, QString, qint64, qint64)),
                this, SLOT(end_IntoTunnelNotValid(QString, qint64, qint64, QString, qint64, qint64,
                                                  QString, qint64, qint64, QString, qint64, qint64)));
        connect(lzacqui, SIGNAL(acquifinish(QString, qint64, qint64, QString, qint64, qint64,
                                            QString, qint64, qint64, QString, qint64, qint64)),
                this, SLOT(end_OutFromTunnel(QString, qint64, qint64, QString, qint64, qint64,
                                             QString, qint64, qint64, QString, qint64, qint64)));

        lastTime = QDateTime::currentDateTime();
        lzacqui->init();
        m_time = lastTime.msecsTo(QDateTime::currentDateTime());

        emit signalParsedMsgToSlave(QString("[从控] 采集类初始化完成，用时%1毫秒").arg(m_time));
    }
    catch (LzException & ex)
    {
        m_time = lastTime.msecsTo(QDateTime::currentDateTime());

        // 解析异常消息的Serial Number(SN号)
        QString errmsg(ex.getErrDescribe().c_str());
        QStringList strList = errmsg.split(",", QString::SkipEmptyParts);
        int numOfDevices = strList.value(1).toInt();
        if (strList.length() < 2 + numOfDevices)
        {
            emit signalParsedMsgToSlave(QString("[从控] 采集类初始化失败，用时%1毫秒，异常%2，解析采集类初始化消息错误！").arg(m_time).arg(ex.getErrDescribe().c_str()));
            return false;
        }
        // 检测四个相机是否都有
        QString tmpdevicesn = "";

        for (int i = 0; i < numOfDevices; i++,tmpdevicesn = "")
        {
            tmpdevicesn = strList.value(2+i);
            if (tmpdevicesn.compare(QString(slaveModel.box1.camera_ref_sn.c_str())) == 0)
            {
                hasbox1camera_ref = true;
                continue;
            }
            else if (tmpdevicesn.compare(QString(slaveModel.box1.camera_sn.c_str())) == 0)
            {
                hasbox1camera = true;
                continue;
            }
            else if (tmpdevicesn.compare(QString(slaveModel.box2.camera_ref_sn.c_str())) == 0)
            {
                hasbox2camera_ref = true;
                continue;
            }
            else if (tmpdevicesn.compare(QString(slaveModel.box2.camera_sn.c_str())) == 0)
            {
                hasbox2camera = true;
                continue;
            }
        }
        // 四个相机依次发消息
        std::string tmpcameraid;
        bool tmpcamerastatus;
        for (int i = 0; i < 4; i++)
        {
            tmpcameraid = "";
            tmpcamerastatus = false;
            switch(i)
            {
                case 0: tmpcameraid = box1camera_ref; tmpcamerastatus = hasbox1camera_ref;  break;
                case 1: tmpcameraid = box1camera;     tmpcamerastatus = hasbox1camera;      break;
                case 2: tmpcameraid = box2camera_ref; tmpcamerastatus = hasbox2camera_ref;  break;
                case 3: tmpcameraid = box2camera;     tmpcamerastatus = hasbox2camera;      break;
            }
            if (tmpcamerastatus)
            {
                this->collect_CameraStatusFeedback(QString(tmpcameraid.c_str()), 1, " \0");
                emit signalParsedMsgToSlave(QString("[从控] 采集类初始化，检测到相机%1").arg(tmpcameraid.c_str()));
            }
            else
            {
                this->collect_CameraStatusFeedback(QString(tmpcameraid.c_str()), 4, " \0");
                emit signalParsedMsgToSlave(QString("[从控] 采集类初始化，未检测到相机%1").arg(tmpcameraid.c_str()));
            }
        }

        emit signalParsedMsgToSlave(QString("[从控] 采集类初始化失败，用时%1毫秒，异常%2").arg(m_time).arg(ex.getErrDescribe().c_str()));
        return false;
    }

    int cameraOkNum = 0;

    // 四个相机依次初始化
    std::string tmpcameraid;
    std::string tmpcamerasn;
	bool ifref;
    for (int i = 0; i < 4; i++)
    {
        tmpcameraid = "";
        tmpcamerasn = "";
        switch(i)
        {
            case 0: tmpcameraid = box1camera_ref; tmpcamerasn = slaveModel.box1.camera_ref_sn;  ifref = true; break;
            case 1: tmpcameraid = box1camera;     tmpcamerasn = slaveModel.box1.camera_sn;      ifref = false; break;
            case 2: tmpcameraid = box2camera_ref; tmpcamerasn = slaveModel.box2.camera_ref_sn;  ifref = true; break;
            case 3: tmpcameraid = box2camera;     tmpcamerasn = slaveModel.box2.camera_sn;      ifref = false; break;
        }
        lastTime = QDateTime::currentDateTime();
        // 相机初始化
        try {
            lzacqui->cfgEventHandler(Pylon::String_t(tmpcamerasn.c_str()), ifref, exposureTime);
            m_time = lastTime.msecsTo(QDateTime::currentDateTime());

            cameraOkNum++;
            emit signalParsedMsgToSlave(QString("[从控] 设置相机%1初始化Serial Number完成").arg(tmpcameraid.c_str()));
        }
        catch (LzException & ex)
        {
            emit signalParsedMsgToSlave(QString("[从控] 设置相机%1的Serial Number失败，异常%3").arg(tmpcameraid.c_str()).arg(ex.getErrDescribe().c_str()));
            this->collect_CameraStatusFeedback(QString(tmpcameraid.c_str()), 3, " \0");
        }
    }

    qDebug() << "number of init cameras=" << cameraOkNum;
    if (cameraOkNum < 4)
        return false;

    // 设置硬触发模式（硬触发或连续触发模式）
    if (collectionMode == LzCameraCollectingMode::Lz_Camera_Continus)
        lzacqui->cfgTriggerMode(AcquiMode::Continues);
	else if ( collectionMode == LzCameraCollectingMode::Lz_Camera_HardwareTrigger )
        lzacqui->cfgTriggerMode(AcquiMode::HardwareTrigger);
	else if ( collectionMode == LzCameraCollectingMode::Lz_Camera_SoftwareTrigger )
		lzacqui->cfgTriggerMode(AcquiMode::SoftwareTrigger);
    
    try {
        lastTime = QDateTime::currentDateTime();
        lzacqui->openCamera();
        m_time = lastTime.msecsTo(QDateTime::currentDateTime());

        emit signalParsedMsgToSlave(QString("[从控] 四路相机成功开启,用时%1毫秒").arg(m_time));

        // 告诉界面
        emit startCollect(slaveModel.box1.camera_ref.c_str(), slaveModel.box1.camera.c_str(), slaveModel.box2.camera_ref.c_str(), slaveModel.box2.camera.c_str(), 
                          slaveModel.box1.camera_ref_sn.c_str(), slaveModel.box1.camera_sn.c_str(), slaveModel.box2.camera_ref_sn.c_str(), slaveModel.box2.camera_sn.c_str());

        this->collect_CameraStatusFeedback(QString(slaveModel.box1.camera_ref.c_str()), 1, " \0");
        this->collect_CameraStatusFeedback(QString(slaveModel.box1.camera.c_str()), 1, " \0");
        this->collect_CameraStatusFeedback(QString(slaveModel.box2.camera_ref.c_str()), 1, " \0");
        this->collect_CameraStatusFeedback(QString(slaveModel.box2.camera.c_str()), 1, " \0");
    }
    catch (LzException & ex)
    {
        m_time = lastTime.msecsTo(QDateTime::currentDateTime());
        emit signalParsedMsgToSlave(QString("[从控] 四路相机开启失败,用时%1毫秒，异常%2").arg(m_time).arg(ex.getErrDescribe().c_str()));
        this->collect_CameraStatusFeedback(QString(box1camera_ref.c_str()), 3, "");
        this->collect_CameraStatusFeedback(QString(box1camera.c_str()), 3, "");
        this->collect_CameraStatusFeedback(QString(box2camera_ref.c_str()), 3, "");
        this->collect_CameraStatusFeedback(QString(box2camera.c_str()), 3, "");
        return false;
    }
    return true;
}

/**
 * 开始采集某条线路
 * @author 范翔
 */
bool LzAcquizQueue::collect_start_line()
{
    if (hasCamerainit)
    {
        emit signalParsedMsgToSlave(QString("[从控] 相机正在使用，不能初始化"));
        return false;
    }

    hasCamerainit = this->cameraInit();  //4秒时间

    if (hasCamerainit == false)
    {
        emit signalParsedMsgToSlave(QString("[从控] 相机初始化并开启失败"));
        return false;
    }

    int length = LzProjectAccess::getLzProjectAccessInstance()->getLzPlanList(LzProjectClass::Collect).list()->length();
    if (length == 0)
    {
        qDebug() << "task file has no plan task";
        return false;
    }
    else
    {
        // 一次性设置迭代器指针
        planTaskIterator = LzProjectAccess::getLzProjectAccessInstance()->getLzPlanList(LzProjectClass::Collect).begin();
        emit signalParsedMsgToSlave(QString("[从控] 初始化taskfile成功，正在开始采集!"));
        // 配置后seqno清零
        this->seq = 0;

        return true;
    }
}

/**
 * 开始采集某条隧道
 * @author 范翔
 */
bool LzAcquizQueue::collect_start_tunnel()
{
    if (hasCamerainit)
    {
        // @author 范翔 @date 20141122 从控只执行，不判断！
        /*if (isAcquizing == true)
        {
            emit signalParsedMsgToSlave(QString("[从控] 已经正在采集,不能开始采集,请先停止采集当前隧道(关闭文件)"));
            return false;
        }*/
        lastTime = QDateTime::currentDateTime();

        this->seq++;
        SlaveModel & model = SlaveSetting::getSettingInstance()->getCurrentSlaveModel();
        DataHead dataHead;
        // TODO///////////////////fanxiang
        dataHead.carriage_direction = planTaskIterator->traindirection;
        dataHead.is_double_line = planTaskIterator->doubleline;
        dataHead.is_downlink = planTaskIterator->downstream;
        dataHead.is_newline = planTaskIterator->newline;
        dataHead.is_normal = planTaskIterator->isnormal;
        dataHead.is_rectify = false;
        dataHead.line_id = planTaskIterator->linenum;
        strcpy(dataHead.line_name, planTaskIterator->linename.c_str());
        dataHead.tunnel_id = planTaskIterator->tunnelnum;
        strcpy(dataHead.tunnel_name, planTaskIterator->tunnelname.c_str());
        dataHead.line_type = planTaskIterator->linetype;
        dataHead.start_mileage = planTaskIterator->startdistance;		// 隧道起始里程 【用于check与数据库中的信息是否发生改变】
        dataHead.end_mileage = planTaskIterator->enddistance;	
        dataHead.interval_pulse_num = planTaskIterator->pulsepermeter;
        strcpy(dataHead.datetime, lastTime.toString().toLocal8Bit().constData());
        qDebug() << planTaskIterator->doubleline;//

        // 时间改为采集时间
        //strcpy(dataHead.datetime, planTaskIterator->datetime.c_str());
        QByteArray currentdatetime = QDateTime::currentDateTime().toString("yyyyMMdd").toLocal8Bit();
        strcpy(dataHead.datetime, currentdatetime.constData());

        dataHead.version = 1.0;
        dataHead.seqno = this->seq;
        DataHead datahead1 = dataHead;
        DataHead datahead2 = dataHead;
        DataHead datahead3 = dataHead;
        DataHead datahead4 = dataHead;

        // 存储目录名
        QString projectname = LzProjectAccess::getLzProjectAccessInstance()->getProjectFilename(LzProjectClass::Collect);
        QString projectcollectdir = SlaveSetting::getSettingInstance()->getParentPath() + "/" + projectname.left(projectname.length() - 5) + "/collect/";
        QString projectdate = projectname.mid(projectname.length() - 13).left(8);

        QDir my_dir;
        if(!my_dir.exists(projectcollectdir))//如果目录不存在的话，就建立这个目录
            my_dir.mkpath(projectcollectdir);
        // 含中文QString to char*
        QByteArray tmpba = projectcollectdir.toLocal8Bit();
        std::string sss = tmpba.constData();
        // int 转 cstring
        char seqnostr[6]; // 9999足够大
        sprintf(seqnostr, "%d", this->seq);
        sss += seqnostr;
        sss += "_";
        sss += dataHead.tunnel_name;
        sss += "_";
        tmpba = projectdate.toLocal8Bit();
        sss +=  tmpba.constData(); // 不用dataHead.date，因为没有'\0'
        sss += "_";
        string filename1 = sss + model.box1.camera + ".dat";
        string filename2 = sss + model.box1.camera_ref + ".dat";
        string filename3 = sss + model.box2.camera + ".dat";
        string filename4 = sss + model.box2.camera_ref + ".dat";

        strcpy(datahead1.camera_index, model.box1.camera.c_str());
        strcpy(datahead2.camera_index, model.box1.camera_ref.c_str());
        strcpy(datahead3.camera_index, model.box2.camera.c_str());
        strcpy(datahead4.camera_index, model.box2.camera_ref.c_str());

        static bool ret1 = false;
        static bool ret2 = false;
        static bool ret3 = false;
        static bool ret4 = false;
        try {
            ret1 = lzacqui->assignFileInfo(Pylon::String_t(model.box1.camera_sn.c_str()), filename1.c_str(), &datahead1);
            ret2 = lzacqui->assignFileInfo(Pylon::String_t(model.box1.camera_ref_sn.c_str()), filename2.c_str(), &datahead2);
            ret3 = lzacqui->assignFileInfo(Pylon::String_t(model.box2.camera_sn.c_str()), filename3.c_str(), &datahead3);
            ret4 = lzacqui->assignFileInfo(Pylon::String_t(model.box2.camera_ref_sn.c_str()), filename4.c_str(), &datahead4);
            emit signalParsedMsgToSlave(QString("[从控] 创建文件%1,%2,%3,%4").arg(ret1).arg(ret2).arg(ret3).arg(ret4));

            isAcquizing = true;


            ////TODO TODELETE测试用
            //for (int i = 0; i < 200; i++)
                //this->collect_one_frame_software_trigger();
        }
        catch(LzException & ex)
        {
            m_time = lastTime.msecsTo(QDateTime::currentDateTime());

            isAcquizing = false;
            emit signalParsedMsgToSlave(QString("[从控] 创建文件%1,%2,%3,%4失败，异常%5").arg(ret1).arg(ret2).arg(ret3).arg(ret4).arg(ex.getErrDescribe().c_str()));
            return false;
        }
        //emit signalParsedMsgToSlave(QString("[从控] 开始采集lzacqui->startGrabbing结束"));
        return true;
    }
    else
    {
        emit signalParsedMsgToSlave(QString("[从控] 相机未打开，无法开始采集隧道!"));
        return false;
    }
}

/**
 * 采集某一隧道完成
 * @param ifdelete bool 是否删除这些文件
 * @author 范翔
 */
void LzAcquizQueue::collect_end_tunnel(bool ifdelete)
{
    if (!hasCamerainit)
    {
        emit signalParsedMsgToSlave(QString("[从控] 相机未打开，无法结束采集隧道!"));
        return;
    }

    // @author 范翔 @date 20141122 从控只执行，不判断！
    /*
    if (isAcquizing == false)
    {
        emit signalParsedMsgToSlave(QString("[从控] 没有正在采集,不能停止采集"));
        return;
    }*/

    try{
        lzacqui->closeFile();
        isAcquizing = false;
    }
    catch (LzException &ex)
    {
        emit signalParsedMsgToSlave(QString("[从控] 停止采集失败，异常%1").arg(ex.getErrDescribe().c_str()));
    }

    /**
     * @param tunnelid 隧道ID号（不是铁路规定编号）
     * @param seqno 当次线路采集序列号
     * @param startFrameCounter 起始帧号
     * @param endFrameCounter 终止帧号
     * @param filenamePrfix 存储路径前缀（注：后缀分别是"_A1.raw"...）
     */
    if (planTaskIterator == LzProjectAccess::getLzProjectAccessInstance()->getLzPlanList(LzProjectClass::Collect).end())
    {
        emit signalParsedMsgToSlave(QString("[从控] task list 出界 tunnelid"));
    }

    QString projectname = LzProjectAccess::getLzProjectAccessInstance()->getProjectFilename(LzProjectClass::Collect);
    QString projectdate = projectname.mid(projectname.length() - 13).left(8);

    // 如果不删除
    if (!ifdelete)
    {
        // 计划的下一条隧道
        planTaskIterator++;
        if (planTaskIterator == LzProjectAccess::getLzProjectAccessInstance()->getLzPlanList(LzProjectClass::Collect).end())
        {
            planTaskIterator--;
        }
    }
    else // 删除这几个文件
    {
        // 存储目录名
        QString projectcollectdir = SlaveSetting::getSettingInstance()->getParentPath() + "/" + projectname.left(projectname.length() - 5) + "/collect/";

        // 含中文QString to char*
        QByteArray tmpba = projectcollectdir.toLocal8Bit();
        std::string sss = tmpba.constData();
        // int 转 cstring
        char seqnostr[6]; // 9999足够大
        sprintf(seqnostr, "%d", this->seq);
        sss += seqnostr;
        sss += "_";
        sss += planTaskIterator->tunnelname;
        sss += "_";
        tmpba = projectdate.toLocal8Bit();
        sss +=  tmpba.constData(); // 不用dataHead.date，因为没有'\0'
        sss += "_";
        string filename1 = sss + slaveModel.box1.camera + ".dat";
        string filename2 = sss + slaveModel.box1.camera_ref + ".dat";
        string filename3 = sss + slaveModel.box2.camera + ".dat";
        string filename4 = sss + slaveModel.box2.camera_ref + ".dat";

        QFile f;
        bool ret1 = f.remove(filename1.c_str());
        bool ret2 = f.remove(filename2.c_str());
        bool ret3 = f.remove(filename3.c_str());
        bool ret4 = f.remove(filename4.c_str());
        emit signalParsedMsgToSlave(QString("[从控] 删除文件%1,%2,%3,%4成功%5,%6,%7,%8").arg(filename1.c_str()).arg(filename2.c_str())
                                                                        .arg(filename3.c_str()).arg(filename4.c_str())
                                                                        .arg(ret1).arg(ret2).arg(ret3).arg(ret4));
    }
}

/**
 * 采集当前计划任务线路,关闭相机
 */
void LzAcquizQueue::collect_end_line()
{
    if (!hasCamerainit)
    {
        emit signalParsedMsgToSlave(QString("[从控] 相机未打开，无法完成采集线路!"));
        return;
    }

    // @author 范翔 @date 20141122 从控只执行，不判断！
    /*
    if (isAcquizing == true)
    {
        emit signalParsedMsgToSlave(QString("[从控] 已经正在采集,不能开始采集,请先停止采集当前隧道(关闭文件)"));
        return;
    }*/
    try 
    {
        // 关闭相机
        if (collectionMode == LzCameraCollectingMode::Lz_Camera_Continus)
            lzacqui->stopContinuesGrabbing();//·author 范翔 @date 20140529发现还是关不上文件
        else
            lzacqui->stopGrabbing();

        hasCamerainit = false;

        if (lzacqui != NULL)
            delete lzacqui;
        lzacqui = NULL;

        // 确认告知主控关闭相机
        emit signalMsgToMaster("1302,return=true");
        emit signalParsedMsgToSlave(QString("[从控] 关闭相机成功"));
    }
    catch (LzException & ex)
    {
        emit signalParsedMsgToSlave(QString("[从控]] 关闭相机失败，异常%1").arg(ex.getErrDescribe().c_str()));
        emit signalMsgToMaster("1302,return=false");
    }
}

/**
 * 软同步采集一帧
 */
void LzAcquizQueue::collect_one_frame_software_trigger()
{
    if (hasCamerainit && isAcquizing)
    {
        lzacqui->cameras[0].TriggerSoftware.Execute();
        lzacqui->cameras[1].TriggerSoftware.Execute();
        lzacqui->cameras[2].TriggerSoftware.Execute();
        lzacqui->cameras[3].TriggerSoftware.Execute();
    }
}

/**
 * 从机硬件采集状态复位
 * 如果正在执行任何任务，或出现任何问题，均停止恢复到初始未采集状态
 */
bool LzAcquizQueue::collect_slave_reset()
{
    if (hasCamerainit)
    {
        try 
        {
            // 关闭相机
            if (collectionMode == LzCameraCollectingMode::Lz_Camera_Continus)
                lzacqui->stopContinuesGrabbing();//·author 范翔 @date 20140529发现还是关不上文件
            else
                lzacqui->stopGrabbing();

            hasCamerainit = false;

            if (lzacqui != NULL)
                delete lzacqui;
            lzacqui = NULL;

        }
        catch (LzException & ex)
        {
            ;
        }
    }
    if (isAcquizing)
    {
        // 关闭文件
        collect_end_tunnel(false);
        // 关闭相机
        collect_end_line();

        endMsg();
        isAcquizing = false;
    }
    return true;
}

/**
 * 从控程序得到缓存图片
 */
void LzAcquizQueue::retriveImg(QString tmpsn, unsigned __int64 * tmpfc, Mat & tmpimg, bool & isacquiz)
{
    QByteArray tmpba = tmpsn.toLocal8Bit();
    lzacqui->retriveSample(tmpba.constData(), tmpfc, tmpimg);
    isacquiz = isAcquizing;
}

/**
 * 相机状态反馈采集异常处理，并告知主控机
 * @param working表示工作状态，在主控接收端是一个枚举类型0表示关闭，1表示开启但未在采集
 *					2表示正在工作，3表示异常
 * @author 范翔
 */
void LzAcquizQueue::collect_CameraStatusFeedback(QString cameraindex, int working, QString errortype)
{
    emit signalMsgToMaster(QString("1200,cameraindex=%1,working=%2,errortype=%3").arg(cameraindex).arg(working).arg(errortype));
}

/**
 * 发送采集完某一隧道消息
 * @paran cameraindex 相机索引号A1...
 * @param tunnelid 隧道ID号（不是铁路规定编号）
 * @param tunnelname 隧道名称
 * @param seqno 当次线路采集序列号
 * @param startFrameCounter 起始帧号
 * @param endFrameCounter 终止帧号
 * @param filenamePrfix 存储路径前缀（注：后缀分别是"_A1.raw"...）
 */
void LzAcquizQueue::collect_msgFinishOneTunnel(QString cameraindex, int tunnelid, QString tunnelname, int seqno, _int64 startFrameCounter, _int64 endFrameCounter, QString filenamePrfix)
{
    emit signalMsgToMaster(QString("1102,cameraindex=%1,return=true,tunnelid=%2,tunnelname=%3,seqno=%4,start_framecounter=%5,end_framecounter=%6,filename=%7")
                           .arg(cameraindex).arg(tunnelid).arg(tunnelname).arg(seqno).arg(startFrameCounter).arg(endFrameCounter).arg(filenamePrfix));//停止之后给主控发确认消息
}

/**
 * 发送预进洞有效无效消息
 * 1104,cameraindex=x,isvalid=true（预进洞有效，else预进洞无效）,return=true,startframe=xx,endframe=xx,filename_prefix=xx(预进洞有效时有filename_prefix)
 */
void LzAcquizQueue::collect_msgFinishIntoTunnel(bool isvalid, QString cameraindex, _int64 startFrameCounter, _int64 endFrameCounter, QString filenameprefix)
{
    if (isvalid)
    {
        emit signalMsgToMaster(QString("1104,cameraindex=%1,isvalid=true,return=true,startframe=%2,endframe=%3,filename_prefix=%4").arg(cameraindex).arg(startFrameCounter).arg(endFrameCounter).arg(filenameprefix));
    }
    else
        emit signalMsgToMaster(QString("1104,cameraindex=%1,isvalid=false,return=true,startframe=%2,endframe=%3").arg(cameraindex).arg(startFrameCounter).arg(endFrameCounter));
}

/**
 * 预进洞无效处理函数
 */
void LzAcquizQueue::collect_beginIntoTunnelNotValid(_int64 endfc_box1cameraref, _int64 endfc_box1camera, _int64 endfc_box2cameraref, _int64 endfc_box2camera)
{
    if (hasCamerainit && isAcquizing)
    {
        if (!hasInitSlaveModel())
            return;

        vector<AcquiRet> tmp;
        AcquiRet camera1(slaveModel.box1.camera_ref_sn.c_str(), 0, endfc_box1cameraref);
        AcquiRet camera2(slaveModel.box1.camera_sn.c_str(), 0, endfc_box1camera);
        AcquiRet camera3(slaveModel.box2.camera_ref_sn.c_str(), 0, endfc_box2cameraref);
        AcquiRet camera4(slaveModel.box2.camera_sn.c_str(), 0, endfc_box2camera);
        tmp.push_back(camera1);
        tmp.push_back(camera2);
        tmp.push_back(camera3);
        tmp.push_back(camera4);

        lzacqui->discard(tmp);
    }
}

/**
 * 出洞处理函数
 */
void LzAcquizQueue::collect_beginOutfromTunnel(_int64 endfc_box1cameraref, _int64 endfc_box1camera, _int64 endfc_box2cameraref, _int64 endfc_box2camera,
                                float startmile, float endmile)
{
    if (hasCamerainit && isAcquizing)
    {
        if (!hasInitSlaveModel())
            return;

        vector<AcquiRet> tmp;
        AcquiRet camera1(slaveModel.box1.camera_ref_sn.c_str(), 0, endfc_box1cameraref);
        AcquiRet camera2(slaveModel.box1.camera_sn.c_str(), 0, endfc_box1camera);
        AcquiRet camera3(slaveModel.box2.camera_ref_sn.c_str(), 0, endfc_box2cameraref);
        AcquiRet camera4(slaveModel.box2.camera_sn.c_str(), 0, endfc_box2camera);
        tmp.push_back(camera1);
        tmp.push_back(camera2);
        tmp.push_back(camera3);
        tmp.push_back(camera4);

        lzacqui->exiting(tmp);
    }
}

/**
 * 执行完预进洞无效信号的结束信号槽函数
 */
void LzAcquizQueue::end_IntoTunnelNotValid(QString camera1, qint64 c1beg, qint64 c1end, QString camera2, qint64 c2beg, qint64 c2end,
                                           QString camera3, qint64 c3beg, qint64 c3end, QString camera4, qint64 c4beg, qint64 c4end)
{
    QString cameraindex;
    _int64 beg;
    _int64 end;
    for (int i = 0; i < 4; i++)
    {
        switch (i)
        {
            case 0:  cameraindex = camera1; beg = c1beg; end = c1end; break;
            case 1:  cameraindex = camera2; beg = c2beg; end = c2end; break;
            case 2:  cameraindex = camera3; beg = c3beg; end = c3end; break;
            case 3:  cameraindex = camera4; beg = c4beg; end = c4end; break;       
            default: break;
        }
        if (cameraindex.compare(slaveModel.box1.camera_ref_sn.c_str()) == 0)
            cameraindex = slaveModel.box1.camera_ref.c_str();
        else if (cameraindex.compare(slaveModel.box1.camera_sn.c_str()) == 0)
            cameraindex = slaveModel.box1.camera.c_str();
        else if (cameraindex.compare(slaveModel.box2.camera_ref_sn.c_str()) == 0)
            cameraindex = slaveModel.box2.camera_ref.c_str();
        else if (cameraindex.compare(slaveModel.box2.camera_sn.c_str()) == 0)
            cameraindex = slaveModel.box2.camera.c_str();

        collect_msgFinishIntoTunnel(false, cameraindex, beg, end, "");
    }
    emit signalParsedMsgToSlave(QString("[从控] 预进洞无效处理结束"));
    // 处理结束
    endMsg();
}

/**
 * 执行完出洞信号的结束信号槽函数
 */
void LzAcquizQueue::end_OutFromTunnel(QString camera1, qint64 c1beg, qint64 c1end, QString camera2, qint64 c2beg, qint64 c2end,
                                      QString camera3, qint64 c3beg, qint64 c3end, QString camera4, qint64 c4beg, qint64 c4end)

{
    int tunnelid = planTaskIterator->tunnelnum;
    QString tunnelname = QString::fromLocal8Bit(planTaskIterator->tunnelname.c_str());
    int seqno = this->seq;

    QString filenamePrfix;
    QString projectname = LzProjectAccess::getLzProjectAccessInstance()->getProjectFilename(LzProjectClass::Collect);
    QString projectdate = projectname.mid(projectname.length() - 13).left(8);

    if (planTaskIterator != LzProjectAccess::getLzProjectAccessInstance()->getLzPlanList(LzProjectClass::Collect).end())
        filenamePrfix = QString("%1_%2_%3").arg(seqno).arg(planTaskIterator->tunnelname.c_str()).arg(projectdate);
    
    QString cameraindex;
    _int64 beg;
    _int64 end;
    for (int i = 0; i < 4; i++)
    {
        switch (i)
        {
            case 0:  cameraindex = camera1; beg = c1beg; end = c1end; break;
            case 1:  cameraindex = camera2; beg = c2beg; end = c2end; break;
            case 2:  cameraindex = camera3; beg = c3beg; end = c3end; break;
            case 3:  cameraindex = camera4; beg = c4beg; end = c4end; break;
            default: break;
        }

        if (cameraindex.compare(slaveModel.box1.camera_ref_sn.c_str()) == 0)
            cameraindex = slaveModel.box1.camera_ref.c_str();
        else if (cameraindex.compare(slaveModel.box1.camera_sn.c_str()) == 0)
            cameraindex = slaveModel.box1.camera.c_str();
        else if (cameraindex.compare(slaveModel.box2.camera_ref_sn.c_str()) == 0)
            cameraindex = slaveModel.box2.camera_ref.c_str();
        else if (cameraindex.compare(slaveModel.box2.camera_sn.c_str()) == 0)
            cameraindex = slaveModel.box2.camera.c_str();

        emit signalParsedMsgToSlave(QString("[从控] %1_%2_%3隧道相机%4停止采集结束").arg(seqno).arg(planTaskIterator->tunnelname.c_str()).arg(projectdate).arg(cameraindex));
        collect_msgFinishOneTunnel(cameraindex, tunnelid, tunnelname, seqno, beg, end, filenamePrfix);

        emit changeCollectCameraLight(cameraindex, HardwareStatus::Hardware_OnButFree);

        // 反馈相机状态
        this->collect_CameraStatusFeedback(cameraindex, 1, " \0");
    }
    /*for (int i = 0; i < tmp.size(); i++)
    {
        if (tmp[i].cam.compare(slaveModel.box1.camera_ref_sn.c_str()) == 0)
            cameraindex = slaveModel.box1.camera_ref.c_str();
        else if (tmp[i].cam.compare(slaveModel.box1.camera_sn.c_str()) == 0)
            cameraindex = slaveModel.box1.camera.c_str();
        else if (tmp[i].cam.compare(slaveModel.box2.camera_ref_sn.c_str()) == 0)
            cameraindex = slaveModel.box2.camera_ref.c_str();
        else if (tmp[i].cam.compare(slaveModel.box2.camera_sn.c_str()) == 0)
            cameraindex = slaveModel.box2.camera.c_str();
        emit signalParsedMsgToSlave(QString("[从控] %1_%2_%3隧道相机%4停止采集结束").arg(seqno).arg(planTaskIterator->tunnelname.c_str()).arg(projectdate).arg(cameraindex));
        collect_msgFinishOneTunnel(cameraindex, tunnelid, tunnelname, seqno, tmp[i].beg, tmp[i].end, filenamePrfix);
        // 反馈相机状态
        this->collect_CameraStatusFeedback(cameraindex, 1, " \0");
    }*/
    // 处理结束
    endMsg();
}

/**
 * 从控界面定时监控显示计时器槽函数
 */
void LzAcquizQueue::onTimeOut()
{
    // 取数据的条件
    if (hasCamerainit && hasInitSlaveModel())
    {
        emit fcupdate(Collecting, 0, slaveModel.box1.camera_ref.c_str(), 0);
        emit fcupdate(Collecting, 0, slaveModel.box1.camera.c_str(), 0);
        emit fcupdate(Collecting, 0, slaveModel.box2.camera_ref.c_str(), 0);
        emit fcupdate(Collecting, 0, slaveModel.box2.camera.c_str(), 0);
    }
}

void LzAcquizQueue::collect_feedbackFCToMaster(QString cameraid, long long currentfc)
{
    //if (hasCamerainit && hasInitSlaveModel())
    //    emit signalMsgToMaster(QString("1108,cameraindex=%1,currentframe=%2").arg(cameraid).arg(currentfc));
}
