#include "LzCalc.h"

#include "LzCalc_ExtractHeight.h"
#include <QDateTime>

LzSlaveCalculate::LzSlaveCalculate(LzCalcThreadType initcalcthreadtype, int initnumofthreads, QObject * parent) : calcuthreadtype(initcalcthreadtype), maxnumofthreads(initnumofthreads), 
                            slavemodel(1.0), QObject(parent)
{
    // 如果是从机RT计算单线程版本或融合计算，最大线程数为1，其他为2
    if (initcalcthreadtype == LzCalcThreadType::LzCalcThread_RTSlave_V1 || initcalcthreadtype == LzCalcThreadType::LzCalcThread_Fuse)
        maxnumofthreads = 1;
    hasinitslavemodel = false;
    hasinitcheckedtask = false;
    currentnumofthreads = 0;
    currentThreads = QList<LzCalculateThread* >();
}

LzSlaveCalculate::~LzSlaveCalculate()
{

}

/**
 * 得到允许最大线程数
 */
int LzSlaveCalculate::getNumOfMaxThreads()
{
    return maxnumofthreads;
}

/**
 * 得到当前线程数
 */
int LzSlaveCalculate::getNumOfCurrentThreads()
{
    int ret;
    nummutex.lock();
    ret = currentnumofthreads;
    nummutex.unlock();
    return ret;
}

bool LzSlaveCalculate::hasInitSlaveModel() { return hasinitslavemodel; }
bool LzSlaveCalculate::hasInitCheckedTask() { return hasinitcheckedtask; }

bool LzSlaveCalculate::initProjectPath(QString path)
{
    QByteArray pathba = path.toLocal8Bit();
    this->projectpath = pathba.constData();
    return true;
}

bool LzSlaveCalculate::initChekedTaskFile(QString path, QString filename)
{
    XMLCheckedTaskFileLoader * fileLoader = new XMLCheckedTaskFileLoader(path + "/" + filename);
    hasinitcheckedtask = fileLoader->loadFile(LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Calculate));
    delete fileLoader;
    QByteArray pathba = path.toLocal8Bit();
    this->projectpath = pathba.constData();
    return hasinitcheckedtask;
}

bool LzSlaveCalculate::initSlaveModel(SlaveModel & initmodel)
{
    slavemodel = initmodel;
    hasinitslavemodel = true;
    return hasinitslavemodel;
}

bool LzSlaveCalculate::startNewTwoViewCalc(QString cameragroupindex, int tunnelid, QString filename, bool isinterrupt, qint64 interruptedfc)
{
    if (hasinitslavemodel && getNumOfCurrentThreads() < maxnumofthreads)
    {
        // 准备计算初始化参数
        CheckedTunnelTaskModel task;
        bool ret = LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Calculate).getCheckedTunnelModel(tunnelid, task);

        if (!ret)
        {
            // 得不到有效的CheckedTunnelTaskModel，结束计算
            finish(0, 2, tunnelid, filename, 0);
            return true;
        }
        QByteArray camgroupid = cameragroupindex.toLocal8Bit();
        QByteArray outputfile = filename.toLocal8Bit();
        // 写文件头
        DataHead head;
        head.carriage_direction = task.planTask.traindirection;	// 正向 【用】
        head.is_normal = task.planTask.isnormal;				// 正常拍摄（正序）【用】
        head.start_mileage = task.planTask.startdistance;		// 隧道起始里程 【用于check与数据库中的信息是否发生改变】
        head.end_mileage = task.planTask.enddistance;			// 隧道终止里程 【用于check与数据库中的信息是否发生改变】
        head.is_double_line = task.planTask.doubleline;		    // 是否双线 【用于check与数据库中的信息是否发生改变】
        head.is_downlink = task.planTask.downstream;
        head.is_newline = task.planTask.newline;
        head.interval_pulse_num = task.planTask.pulsepermeter;
        head.tunnel_id = task.planTask.tunnelnum;				// 隧道ID 【用】
        strcpy(head.tunnel_name, task.planTask.tunnelname.c_str());
        strcpy(head.line_name, task.planTask.linename.c_str());
        strcpy(head.camera_index, camgroupid.constData());
        QDateTime lastTime = QDateTime::currentDateTime();
        strcpy(head.datetime, lastTime.toString().toLocal8Bit().constData());

        // 如果是双目计算（双线程）
        if (calcuthreadtype == LzCalcThreadType::LzCalcThread_GeneralSlave || calcuthreadtype == LzCalcThreadType::LzCalcThread_RTSlave_V2)
        {
            // 找起始、终止帧
            qint64 start_num_store = 0;
            qint64 end_num_store = 0;
            std::list<CheckedItem>::iterator todoit = task.getRealList()->begin();
            bool firsttime = true;
            while (todoit != task.getRealList()->end())
            {
                if (firsttime)
                {
                    start_num_store = (*todoit).start_frame_master;
                    firsttime = false;
                }
                end_num_store = (*todoit).end_frame_master;
                todoit++;
            }

            if (calcuthreadtype == LzCalcThreadType::LzCalcThread_GeneralSlave)
            {
                int toinsertid = 1;
                // 记录线程数量
                nummutex.lock();
                QList<LzCalculateThread *>::iterator it = currentThreads.begin();
                while (it != currentThreads.end())
                {
                    if ((*it)->getID() == toinsertid)
                        toinsertid++;
                    it++;
                }
                LzCalculateTVThread * athread = new LzCalculateTVThread(0, toinsertid);
                currentnumofthreads++;
                currentThreads.push_back(athread);
                nummutex.unlock();
            
                // 告知上层开始计算
                emit myStart(toinsertid, true, tunnelid, filename, start_num_store, end_num_store);

                // 初始化计算参数
                athread->init(projectpath, tunnelid, (*(task.getRealList())), camgroupid.constData(), slavemodel, outputfile.constData(), head, isinterrupt, interruptedfc);

                // 信号槽，接收到计算结束信号
                QObject::connect(athread, SIGNAL(finish(int, int, int, QString, qint64)), this, SLOT(receiveThreadFinish(int, int, int, QString, qint64)));
                QObject::connect(athread, SIGNAL(statusShow(int, qint64, int, QString)), this, SLOT(receiveStatusShow(int, qint64, int, QString)));
                // 线程开始运行
                athread->start();
            }
            else if (calcuthreadtype == LzCalcThreadType::LzCalcThread_RTSlave_V2)
            {
                int toinsertid = 1;
                // 记录线程数量
                nummutex.lock();
                QList<LzCalculateThread *>::iterator it = currentThreads.begin();
                while (it != currentThreads.end())
                {
                    if ((*it)->getID() == toinsertid)
                        toinsertid++;
                    it++;
                }
                LzCalculateTVRTThread_V2 * athread = new LzCalculateTVRTThread_V2(0, toinsertid);
                currentnumofthreads++;
                currentThreads.push_back(athread);
                nummutex.unlock();
            
                // 告知上层开始计算
                emit myStart(toinsertid, true, tunnelid, filename, start_num_store, end_num_store);

                // 初始化计算参数
                athread->init(projectpath, tunnelid, (*(task.getRealList())), camgroupid.constData(), slavemodel, outputfile.constData(), head, isinterrupt, interruptedfc);

                // 信号槽，接收到计算结束信号
                QObject::connect(athread, SIGNAL(finish(int, int, int, QString, qint64)), this, SLOT(receiveThreadFinish(int, int, int, QString, qint64)));
                QObject::connect(athread, SIGNAL(statusShow(int, qint64, int, QString)), this, SLOT(receiveStatusShow(int, qint64, int, QString)));
                // 线程开始运行
                athread->start();
            }
            return true;
        }
        else if (calcuthreadtype == LzCalcThreadType::LzCalcThread_RTSlave_V1) // 如果是双目计算（单线程）
        {            
            int toinsertid = 1;
            // 记录线程数量
            nummutex.lock();
            QList<LzCalculateThread *>::iterator it = currentThreads.begin();
            while (it != currentThreads.end())
            {
                if ((*it)->getID() == toinsertid)
                    toinsertid++;
                it++;
            }
            LzCalculateTVRTThread * athread = new LzCalculateTVRTThread(0, toinsertid);
            currentnumofthreads++;
            currentThreads.push_back(athread);
            nummutex.unlock();

            // 找起始、终止帧
            qint64 start_num_store = 0;
            qint64 end_num_store = 0;
            std::list<CheckedItem>::iterator todoit = task.getRealList()->begin();
            bool firsttime = true;
            while (todoit != task.getRealList()->end())
            {
                if (firsttime)
                {
                    start_num_store = (*todoit).start_frame_master;
                    firsttime = false;
                }
                end_num_store = (*todoit).end_frame_master;
                todoit++;
            }
            // 告知上层开始计算
            emit myStart(toinsertid, true, tunnelid, filename, start_num_store, end_num_store);

            QString outptuRTfile = filename + "_RT.mdat";
            QByteArray outputRT = outptuRTfile.toLocal8Bit();

            // 初始化计算参数
            athread->init(projectpath, tunnelid, (*(task.getRealList())), slavemodel, outputfile.constData(), outputRT.constData(), head, isinterrupt, interruptedfc);
            // 信号槽，接收到计算结束信号
            QObject::connect(athread, SIGNAL(finish(int, int, int, QString, qint64)), this, SLOT(receiveThreadFinish(int, int, int, QString, qint64)));
            QObject::connect(athread, SIGNAL(statusShow(int, qint64, int, QString)), this, SLOT(receiveStatusShow(int, qint64, int, QString)));
            // 线程开始运行
            athread->start();

            return true;
        }
        else
            return false;
    }
    else
    {
        emit finish(0, 4, tunnelid, filename, 0);
        return false;
    }
}

/**
 * 开始融合计算
 */
bool LzSlaveCalculate::startFuseCalc(int tunnelid, QString filename, bool isinterrupt, qint64 interruptedfc, QString parentpath, QString railQRcalifile, QString heightsrectifyfile ,bool userectifyfactor, bool usesafetyfactor)
{
    if (getNumOfCurrentThreads() >= maxnumofthreads)
    {
        // TODO Fuse只是单线程 threadid = 1
        emit finish(0, 4, tunnelid, filename, 0);
        return false;
    }
    else
    {
        QString path = LzProjectAccess::getLzProjectAccessInstance()->getProjectPath(Calculate);
        initProjectPath(path);

        CheckedTunnelTaskModel task;
        bool ret = LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Calculate).getCheckedTunnelModel(tunnelid, task);
        if (!ret)
        {
            // Fuse只是单线程 threadid = 1
            emit finish(1, 2, tunnelid, filename, 0);
            return true;
        }

        QByteArray outputfile = filename.toLocal8Bit();
        // 写文件头
        DataHead head;
        head.carriage_direction = task.planTask.traindirection;	// 正向 【用】
        head.is_normal = task.planTask.isnormal;				// 正常拍摄（正序）【用】
        head.start_mileage = task.planTask.startdistance;		// 隧道起始里程 【用于check与数据库中的信息是否发生改变】
        head.end_mileage = task.planTask.enddistance;			// 隧道终止里程 【用于check与数据库中的信息是否发生改变】
        head.is_double_line = task.planTask.doubleline;		    // 是否双线 【用于check与数据库中的信息是否发生改变】
        head.is_downlink = task.planTask.downstream;
        head.is_newline = task.planTask.newline;
        head.interval_pulse_num = task.planTask.pulsepermeter;
        head.tunnel_id = task.planTask.tunnelnum;				// 隧道ID 【用】
        strcpy(head.tunnel_name, task.planTask.tunnelname.c_str());
        strcpy(head.line_name, task.planTask.linename.c_str());
        strcpy(head.camera_index, "SYN\0");
        QDateTime lastTime = QDateTime::currentDateTime();
        strcpy(head.datetime, lastTime.toString().toLocal8Bit().constData());

        __int64 startnum = -1;
        __int64 endnum = 0;
        std::list<CheckedItem>::iterator it = task.getRealList()->begin();
        while (it != task.getRealList()->end())
        {
            if (startnum == -1) // 首个元素
                startnum = it->start_frame_master;
            endnum = it->end_frame_master;
            it++;
        }

        LzCalculateFuseThread * athread = new LzCalculateFuseThread();
        // 记录线程数量
        nummutex.lock();
        currentnumofthreads++;
        currentThreads.push_back(athread);
        nummutex.unlock();

        // 找起始、终止帧
        qint64 start_num_store = 0;
        qint64 end_num_store = 0;
        std::list<CheckedItem>::iterator todoit = task.getRealList()->begin();
        bool firsttime = true;
        while (todoit != task.getRealList()->end())
        {
            if (firsttime)
            {
                start_num_store = (*todoit).start_frame_master;
                firsttime = false;
            }
            end_num_store = (*todoit).end_frame_master;
            todoit++;
        }

        // 告知上层开始计算
        emit myStart(1, true, tunnelid, filename, start_num_store, end_num_store);

        QString outputfusefile = filename + ".fdat";
        QString outptuheightfile = filename + ".syn";
        
        qDebug() << "output file:" << outputfusefile << "," << outptuheightfile;

        QByteArray outputf = outputfusefile.toLocal8Bit();
        QByteArray outputh = outptuheightfile.toLocal8Bit();

        std::vector<string> inputfilenames;
        filename = path + "/mid_calcu/" + filename;

        QByteArray tmpba = QString(filename + "_A.mdat").toLocal8Bit();
        inputfilenames.push_back(tmpba.constData());
        tmpba = QString(filename + "_B.mdat").toLocal8Bit();
        inputfilenames.push_back(tmpba.constData());
        tmpba = QString(filename + "_C.mdat").toLocal8Bit();
        inputfilenames.push_back(tmpba.constData());
        tmpba = QString(filename + "_D.mdat").toLocal8Bit();
        inputfilenames.push_back(tmpba.constData());
        tmpba = QString(filename + "_E.mdat").toLocal8Bit();
        inputfilenames.push_back(tmpba.constData());
        tmpba = QString(filename + "_F.mdat").toLocal8Bit();
        inputfilenames.push_back(tmpba.constData());
        tmpba = QString(filename + "_G.mdat").toLocal8Bit();
        inputfilenames.push_back(tmpba.constData());
        tmpba = QString(filename + "_H.mdat").toLocal8Bit();
        inputfilenames.push_back(tmpba.constData());
        tmpba = QString(filename + "_I.mdat").toLocal8Bit();
        inputfilenames.push_back(tmpba.constData());
        tmpba = QString(filename + "_J.mdat").toLocal8Bit();
        inputfilenames.push_back(tmpba.constData());
        tmpba = QString(filename + "_K.mdat").toLocal8Bit();
        inputfilenames.push_back(tmpba.constData());
        tmpba = QString(filename + "_L.mdat").toLocal8Bit();
        inputfilenames.push_back(tmpba.constData());
        tmpba = QString(filename + "_M.mdat").toLocal8Bit();
        inputfilenames.push_back(tmpba.constData());
        tmpba = QString(filename + "_N.mdat").toLocal8Bit();
        inputfilenames.push_back(tmpba.constData());
        tmpba = QString(filename + "_O.mdat").toLocal8Bit();
        inputfilenames.push_back(tmpba.constData());
        tmpba = QString(filename + "_P.mdat").toLocal8Bit();
        inputfilenames.push_back(tmpba.constData());
        tmpba = QString(filename + "_Q.mdat").toLocal8Bit();
        inputfilenames.push_back(tmpba.constData());
        tmpba = QString(filename + "_R.mdat").toLocal8Bit();
        inputfilenames.push_back(tmpba.constData());

        // 车底计算出的 R矩阵文件和T矩阵文件
        tmpba = QString(filename + "_RT.mdat").toLocal8Bit();
        inputfilenames.push_back(tmpba.constData());

        // 初始化计算参数
        athread->init(projectpath, inputfilenames, railQRcalifile.toLocal8Bit().constData(), heightsrectifyfile.toLocal8Bit().constData(), startnum, endnum, outputf.constData(), outputh.constData(), head, isinterrupt, interruptedfc, userectifyfactor, usesafetyfactor);
        // 信号槽，接收到计算结束信号
        QObject::connect(athread, SIGNAL(finish(int, int, int, QString, qint64)), this, SLOT(receiveThreadFinish(int, int, int, QString, qint64)));
        QObject::connect(athread, SIGNAL(statusShow(int, qint64, int, QString)), this, SLOT(receiveStatusShow(int, qint64, int, QString)));
        // 线程开始运行
        athread->start();

        return true;
    }
}

/**
 * 开始提取高度+RT校正计算
 */
bool LzSlaveCalculate::startExtractHeightCalc(int tunnelid, QString filename, bool isinterrupt, qint64 interruptedfc, QString parentpath, QString railQRcalifile, QString heightsrectifyfile, bool userectifyfactor, bool usesafetyfactor)
{
    LzCalculate_ExtractHeight calcExtractHeight;

    QString projectpath = LzProjectAccess::getLzProjectAccessInstance()->getProjectPath(Calculate);

    QString fdat_nort = projectpath + "/fuse_calcu/" + filename + ".fdat";
    QString mdat_Q = projectpath + "/mid_calcu/" + filename + ".mdat";
    QString mdat_R = projectpath + "/mid_calcu/" + filename + ".mdat";

    QString rectifyfile = projectpath + "/calcu_calibration/" + heightsrectifyfile;
    QString QRcalibfie = projectpath + "/calcu_calibration/" + railQRcalifile;
    QString syndat = projectpath + "/syn_data/" + filename + ".syn";
    
    std::list<int> Item = OutputHeightsList::getOutputHeightsListInstance()->list();

    // 初始化计算参数
    calcExtractHeight.init(Item, fdat_nort.toLocal8Bit().constData(), mdat_Q.toLocal8Bit().constData(), mdat_R.toLocal8Bit().constData(),
                                                    rectifyfile.toLocal8Bit().constData(), syndat.toLocal8Bit().constData(), userectifyfactor, usesafetyfactor);
    
    // 计算
    int ret = calcExtractHeight.run();

    if (ret == 0)
        return true;
    else
        return false;
}


/**
 * 当前所有线程暂停计算
 */
bool LzSlaveCalculate::suspendAllThreads()
{
    // 记录线程数量
    nummutex.lock();
    QList<LzCalculateThread *>::iterator it = currentThreads.begin();
    while (it != currentThreads.end())
    {
        (*it)->suspend();
        it++;
    }
    nummutex.unlock();
    return true;
}

void LzSlaveCalculate::receiveThreadFinish(int threadid, int isok, int tunnelid, QString filename, qint64 endfc)
{
    // 删除线程对象内存
    int numThreads = currentnumofthreads;
    if (numThreads == 0)
        qDebug() << "error remove slave calcu thread, no claculate task:" << filename;
    for (int i = 0; i < numThreads; i++)
    {
        if ((currentThreads.at(i) == NULL) || (currentThreads.at(i) != NULL && currentThreads.at(i)->getOutputFileName().compare(filename) == 0))
        {
            // 计数更新
            nummutex.lock();
            //if (currentThreads.at(i) != NULL)
                //delete currentThreads.at(i);
            currentThreads.removeAt(i);
            currentnumofthreads--;
            nummutex.unlock();
            qDebug() << "remove calcu task ok " << filename;
            break;
        }
    }
    // 告知上层
    emit finish(threadid, isok, tunnelid, filename, endfc);
}

/**
 * 向LzCalcuQueue转发信号
 * @param tmpfc 当前计算帧号
 */
void LzSlaveCalculate::receiveStatusShow(int threadid, qint64 tmpfc, int tunnelid, QString filename)
{
    emit statusShow(threadid, tmpfc, tunnelid, filename);
}