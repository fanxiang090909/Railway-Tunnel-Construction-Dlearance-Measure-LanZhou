#include "calcu_noslave.h"
#include "ui_calcu_noslave.h"

#include "tabwidgetmanager.h"
#include "create_project.h"

#include "status_calcu_noslave.h"

#include <QTextCodec>
#include <QMessageBox>
#include <QDateTime>

#include "status.h"
#include "projectmodel.h"
#include "network_config_list.h"

#include <QFileDialog>
#include <QCloseEvent>

/**
 * 主控监控页面tabwidget界面类实现
 * @author fanxiang
 * @version 1.0.0
 * @date 2014-04-17
 */

/**
 * 构造函数传入WorkingStatus status初始化界面
 * @param newsinglewidgetapp 是否为单一界面程序（采集程序有可能为单一界面程序，其结束方式与复界面程序不用） @see closeEvent方法
 * @param status 工作状态，@see status.h
 * Collecting = 1 采集时
 * Calculating_Backuping = 5 计算备份同时进行
 */
CalcuNoSlaveWidget::CalcuNoSlaveWidget(bool newsinglewidgetapp, WorkingStatus initstatus, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CalcuNoSlaveWidget)
{
    ui->setupUi(this);

    // 初始化结束方式
    singlewidgetapp = newsinglewidgetapp;

    // 初始化工作状态
    workingstatus = initstatus;

    // 初始时采集工程文件名为空
    projectfilename = "";

    // 默认用户不知道是谁
    currentusername = "?";

    ///CREATE PROJECT界面
    connect(ui->importProjButton, SIGNAL(clicked()), this, SLOT(importProj()));

    // 在主界面连接主控底层服务信号槽
    //connect(MasterProgram::getMasterProgramInstance(), SIGNAL(msgGUICollect(WorkingStatus, bool)), this, SLOT(msgToGUICreateProjectWidget(WorkingStatus, bool)));
    //connect(MasterProgram::getMasterProgramInstance(), SIGNAL(appendMsg(WorkingStatus, QString)), this, SLOT(appendMsg(WorkingStatus, QString)));
    //connect(MasterProgram::getMasterProgramInstance(), SIGNAL(appendError(WorkingStatus, QString)), this, SLOT(appendError(WorkingStatus, QString)));

    lzFuseCalcQueue = new LzFuseCalcQueue(this);

    // 融合计算状态提示，消息转发
    connect(lzFuseCalcQueue, SIGNAL(signalParsedMsgToSlave(QString)), this, SLOT(appendMsg(QString)));
    connect(lzFuseCalcQueue, SIGNAL(signalCalcuBakcupTask(WorkingStatus, QString, int, QString, QString)), this, SLOT(changeSlaveCameraTask_slot(WorkingStatus, QString, int, QString, QString)));
    connect(lzFuseCalcQueue, SIGNAL(calcubackupProgressingBar(WorkingStatus, QString, int, int, int, bool)), this, SLOT(changecalcubackupProgressingBar_slot(WorkingStatus, QString, int, int, int, bool)));
    // 融合计算单一任务消息转发-当前计算帧号
    connect(lzFuseCalcQueue, SIGNAL(signalCalcuBackupTaskFC(WorkingStatus, QString, int, QString, long long)), this, SLOT(changeSlaveCameraTaskFC_slot(WorkingStatus, QString, int, QString, long long)));
    // 融合计算单一任务消息转发-任务初始化（起始帧、终止帧）
    connect(lzFuseCalcQueue, SIGNAL(signalCalcuBackupTaskFC_init(WorkingStatus, QString, int, QString, long long, long long)), this, SLOT(changeSlaveCameraTaskFC_init_slot(WorkingStatus, QString, int, QString, long long, long long)));

    lzCalcQueue = new LzCalcQueue(this);

    ////TODO
    connect(lzCalcQueue, SIGNAL(signalParsedMsgToSlave(QString)), this, SLOT(sendParsedMsgToGUI(QString)));
    connect(lzCalcQueue, SIGNAL(signalMsgToMaster(QString)), this, SLOT(sendMsgToMaster(QString)));

    //QObject::connect(lzCalcQueue, SIGNAL(myStart(int, bool, int, QString, qint64, qint64)), this, SLOT(receiveThreadStart(int, bool, int, QString, qint64, qint64)));
    //QObject::connect(lzCalcQueue, SIGNAL(finish(int, int, int, QString)), this, SLOT(receiveThreadFinish(int, int, int, QString)));
    //QObject::connect(lzCalcQueue, SIGNAL(fcupdate(WorkingStatus, int, QString, qint64)), this, SLOT(receivefcupdate(WorkingStatus, int, QString, qint64)));
    //connect(lzCalcQueue, SIGNAL(signalFileToMaster(QString)), this, SLOT(sendFileToMaster(QString)));

    firstInitNetwork = true;

    //connect(MasterProgram::getMasterProgramInstance(), SIGNAL(turnOffAllHardware()), this, SLOT(turnOffLights()));
    //connect(MasterProgram::getMasterProgramInstance(), SIGNAL(makeGUIActive(bool)), this, SLOT(getCalcuBackupGUIActive(bool)));

    // 初始时关闭所有提示灯
    turnOffLights();

    this->setWindowTitle(QObject::tr("铁路隧道建筑限界测量系统-本机计算程序（采集数据在一台机器上）"));
    QIcon icon;
    icon.addFile(QString::fromUtf8(":image/icon.png"), QSize(), QIcon::Normal, QIcon::Off);
    this->setWindowIcon(icon);

    if (workingstatus == Calculating) // 计算
    {
        // ui->gridLayout_5是整个Window的最外层布局Layout。可从.ui文件中找出
        CalcuStatusNoSlaveWidget * calcuWidget = new CalcuStatusNoSlaveWidget(ui->widget);
        QGridLayout *layout3 = new QGridLayout();
        layout3->addWidget(calcuWidget);
        ui->widget->setLayout(layout3);

        // @author范翔 作废
        // 可以导入已有、不能创建新工程
        connect(this, SIGNAL(sendproject_file_name(QString)),calcuWidget,SLOT(updateCheckedTunnelView()));
        connect(this, SIGNAL(changeSlaveCameraTask(WorkingStatus, QString, int, QString, QString)), calcuWidget, SLOT(changeSlaveTask(WorkingStatus, QString, int, QString, QString)));
        // 从控计算总进度
        connect(this, SIGNAL(calcubackupProgressingBar(WorkingStatus, QString, int, int, int, bool)), calcuWidget, SLOT(changecalcubackupProgressingBar(WorkingStatus, QString, int, int, int, bool)));
        // 每个从控的每个线程每个任务
        // 每个从控的每个线程每个任务-进度初始化
        connect(this, SIGNAL(changeSlaveCameraTaskFC_init(WorkingStatus, QString, int, QString, long long, long long)), calcuWidget, SLOT(changeSlaveCameraTaskFC_init(WorkingStatus, QString, int, QString, long long, long long)));
        // 每个从控的每个线程每个任务-当前任务帧号反馈
        connect(this, SIGNAL(changeSlaveCameraTaskFC(WorkingStatus, QString, int, QString, long long)), calcuWidget, SLOT(changeSlaveCameraTaskFC(WorkingStatus, QString, int, QString, long long)));

        connect(this, SIGNAL(makeGUIActive(bool)), calcuWidget, SLOT(setOptButtonEnable(bool)));


        /////////////NEW
        connect(calcuWidget, SIGNAL(startcalcu(int, int)), this, SLOT(startcalcu2(int, int)));

        connect(ui->operationConfigButton, SIGNAL(clicked()), this, SLOT(configCurrentOperation()));

        // projectclass
        projectclass = LzProjectClass::Calculate;
        projectclassstr = "calcu";
    }
   
    // 桌面系统图标相关，防止误操作关闭，可后台运行
    createActions();
    createTrayIcon();

    setIcon();
    connect(trayIcon, SIGNAL(messageClicked()), this, SLOT(messageClicked()));
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

    trayIcon->show();

    // 初始时确认开始按钮不可点击
    ui->operationConfigButton->setEnabled(false);
}

CalcuNoSlaveWidget::~CalcuNoSlaveWidget()
{
    delete ui;

    delete lzFuseCalcQueue;

    delete lzCalcQueue;
}

void CalcuNoSlaveWidget::appendMsg(QString msg)
{
    QString currentDate = QTime::currentTime().toString("[hh:mm:ss]");
    ui->statusArea->append(QString("%1%2").arg(currentDate).arg(msg));
}

void CalcuNoSlaveWidget::appendError(QString msg)
{
    QString currentDate = QTime::currentTime().toString("[hh:mm:ss]");
    ui->statusArea->append(QString("%1%2").arg(currentDate).arg(msg));
}

void CalcuNoSlaveWidget::changeSlaveCameraTask_slot(WorkingStatus status, QString cameraindex, int threadid, QString task, QString remark)
{
    if (status == Collecting)
        emit changeSlaveCameraTask(status, cameraindex, threadid, task, "");
    else if (status == Calculating || status == Backuping)
        emit changeSlaveCameraTask(status, cameraindex, threadid, task, remark);
}

void CalcuNoSlaveWidget::changeSlaveCameraTaskFC_init_slot(WorkingStatus status, QString index, int threadid, QString file, long long startfc, long long endfc)
{
    if (status == Calculating)
        emit changeSlaveCameraTaskFC_init(status, index, threadid, file, startfc, endfc);
}

void CalcuNoSlaveWidget::changeSlaveCameraTaskFC_slot(WorkingStatus status, QString cameraindex,  int threadid, QString task, long long currentfc)
{
    if (status == Calculating)
        emit changeSlaveCameraTaskFC(status, cameraindex, threadid, task, currentfc);
}

void CalcuNoSlaveWidget::getCalcuBackupGUIActive(bool act)
{
    emit makeGUIActive(act);
}

void CalcuNoSlaveWidget::changecalcubackupProgressingBar_slot(WorkingStatus status, QString slaveidstr, int threadid, int totalnum, int donenum, bool isupdate)
{
    emit calcubackupProgressingBar(status, slaveidstr, threadid, totalnum, donenum, isupdate);
}

void CalcuNoSlaveWidget::setSelectProject(bool iftrue)
{
    ui->importProjButton->setEnabled(iftrue);
    ui->comboBox->setEnabled(iftrue);
}

void CalcuNoSlaveWidget::startcalcu2(int type, int tunnelid)
{
    if (type == 0)
        calculate_beginStartAll();
    else if (type == 1)
        calculate_beginStartOneTunnel(tunnelid);
    else if (type == 2)
        calculate_Fuse_beginAll();
    else if (type == 3)
        calculate_Fuse_beginOneTunnel(tunnelid);
	else if (type == 4)
		;//calculate_ExtractHeight_betinAll();
	else if (type == 5)
		calculate_ExtractHeight_beginOneTunnel(tunnelid);
}


// 开始时关闭所有提示灯槽函数
void CalcuNoSlaveWidget::turnOffLights()
{
    emit turnOffAllHardware();
}

// 界面传递参数-工程文件
void CalcuNoSlaveWidget::slotprojectfilename(QString newfilename)
{
    projectfilename = newfilename;

    emit turnOffAllHardware();

    // 初始时确认开始按钮不可点击
    ui->operationConfigButton->setEnabled(true);

    emit sendproject_file_name(newfilename);
}

void CalcuNoSlaveWidget::configCurrentOperation()
{
    QString statusStr;
    if (workingstatus == Collecting) // 采集
    {
        statusStr = tr("采集");
    }
    else if (workingstatus == Calculating) // 计算
    {
        statusStr = tr("计算");
    }
    else if (workingstatus == Backuping) // 备份
    {
        statusStr = tr("备份");
    }

    if (projectfilename.compare("") != 0)
    {
        // 解析project filename 成为两段，linename和datetime
        QString linename = projectfilename.left(projectfilename.length() - 14);
        QString datetime11 = projectfilename.right(13).left(8);
        qDebug() << "linename=" << linename << ",datatime=" << datetime11;

        QMessageBox msgBox;
        QIcon icon;
        icon.addFile(QString::fromUtf8(":image/icon.png"), QSize(), QIcon::Normal, QIcon::Off);
        msgBox.setWindowIcon(icon);
        msgBox.setWindowTitle(tr("确认检查配置"));
        msgBox.setInformativeText(tr("您要确定设置工作模式为%1线路的%2吗").arg(projectfilename).arg(statusStr));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        int ret = msgBox.exec();
        switch (ret)
        {
            case QMessageBox::Yes:
                if (workingstatus == Calculating) // 计算
                {
                    // 参数lineid暂时不用。设为-1
                    //MasterProgram::getMasterProgramInstance()->workingMode_Setting(projectfilename, -1, linename, datetime11, Calculating, true);
                    bool ret2 = changeMode_newProjectConfig(Calculate, projectfilename);
                    emit makeGUIActive(ret2);
                    setSelectProject(ret2);
                }
                break;
            default:break;
        }
    }
    else
    {
        QMessageBox::warning(this,tr("提示"), tr("您需要在设置工作模式首先设置文件名"));
        return;
    }
}

void CalcuNoSlaveWidget::configCurrentOperation2()
{
    if (projectfilename.compare("") != 0)
    {
        // 解析project filename 成为两段，linename和datetime
        QString linename = projectfilename.left(projectfilename.length() - 14);
        QString datetime11 = projectfilename.right(13).left(8);
        qDebug() << "linename=" << linename << ",datatime=" << datetime11;
        
        if (workingstatus == Calculating) // 计算
        {
            // 参数lineid暂时不用。设为-1
            ;//TODO
            //MasterProgram::getMasterProgramInstance()->workingMode_Setting(projectfilename, -1, linename, datetime11, Calculating, false);
        }
    }
}

// 桌面系统图标相关，防止误操作关闭，可后台运行
//! [2]
void CalcuNoSlaveWidget::closeEvent(QCloseEvent *event)
{
    if (trayIcon->isVisible()) {
        QMessageBox::information(this, tr("提示"),
                                 tr("程序将在后台运行，终止程序请点击桌面右下小图标，选择退出！"));
        hide();
        event->ignore();
    }
}
//! [2]

//! [3]
void CalcuNoSlaveWidget::setIcon()
{
    if (workingstatus == Collecting)
    {
        QIcon icon = QIcon(":/image/collectstatus1.png");
        trayIcon->setIcon(icon);
        setWindowIcon(icon);
        trayIcon->setToolTip(tr("铁路隧道建筑限界测量系统--采集监控"));
        setWindowTitle(tr("铁路隧道建筑限界测量系统--采集监控"));
    }
    else if (workingstatus == Calculating)// 计算
    {
        QIcon icon = QIcon(":/image/Calculatorstatus1.png");
        trayIcon->setIcon(icon);
        setWindowIcon(icon);
        trayIcon->setToolTip(tr("铁路隧道建筑限界测量系统--计算监控"));
        setWindowTitle(tr("铁路隧道建筑限界测量系统--计算监控"));
    }
    else if (workingstatus == Backuping)// 备份
    {
        QIcon icon = QIcon(":/image//up_alt.png");
        trayIcon->setIcon(icon);
        setWindowIcon(icon);
        trayIcon->setToolTip(tr("铁路隧道建筑限界测量系统--备份监控"));
        setWindowTitle(tr("铁路隧道建筑限界测量系统--备份监控"));
    }
}
//! [3]

//! [4]
void CalcuNoSlaveWidget::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        //iconComboBox->setCurrentIndex((iconComboBox->currentIndex() + 1)
        //                              % iconComboBox->count());
        break;
    case QSystemTrayIcon::MiddleClick:
        showMessage();
        break;
    default:
        ;
    }
}
//! [4]

//! [5]
void CalcuNoSlaveWidget::showMessage()
{
    QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon(0);
    trayIcon->showMessage("aaa", "bbb", icon, 1000);
}
//! [5]

//! [6]
void CalcuNoSlaveWidget::messageClicked()
{
    if (workingstatus == Collecting)
    {
         QMessageBox::information(0, tr("提示"),
                                 tr("Sorry, I already gave what help I could.\n"
                                    "Maybe you should try asking a human?"));
    }
    else // 其他状态、计算或备份
    {
         QMessageBox::information(0, tr("提示"),
                                tr("Sorry, I already gave what help I could.\n"
                                   "Maybe you should try asking a human?"));
    }
}
//! [6]

void CalcuNoSlaveWidget::createActions()
{
    minimizeAction = new QAction(tr("最小化"), this);
    connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));

    maximizeAction = new QAction(tr("最大化"), this);
    connect(maximizeAction, SIGNAL(triggered()), this, SLOT(showMaximized()));

    restoreAction = new QAction(tr("显示"), this);
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

    if (singlewidgetapp)
    {
        quitAction = new QAction(tr("退出"), this);
        connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    }
    else
    {
        quitAction = new QAction(tr("隐藏"), this);
        connect(quitAction, SIGNAL(triggered()), this, SLOT(hide()));
    }
}

void CalcuNoSlaveWidget::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(maximizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
}

////////////////////////////////////////////////
///////////////////主控函数复制////////////////
/*****************关于计算*********************/
/**
 * 开始计算
 */
void CalcuNoSlaveWidget::calculate_beginStartAll()
{
    // 给每个从机的开始地方都不一样
    // 2001,filename,seqno=x,tunnelid=x,cameratgroup_index=x,restart=true,restart_frame=x

    SlaveModel tmpsm(1.0);
    QString tmpfilename = "";
    QString date = LzProjectAccess::getLzProjectAccessInstance()->getProjectFilename(LzProjectClass::Calculate).right(13).left(8);
    QString tmpnameprefix = "";
    qDebug() << "date:" << date;
    int tmptunnelid = -1;
        
    bool ret2 = false;
    QString tmpboxindex;
    int tmphasbackup;
    __int64 tmpinterruptfc;
    int tmphasbackupnum = 0;
    int totalhasbackupnum = LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Calculate).list()->size() * 2;

    for (int i = 1; i < 10; i++)
    {
        tmphasbackupnum = 0;

        QList<CheckedTunnelTaskModel>::iterator it = LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Calculate).begin();
        while (it != LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Calculate).end())
        {
            tmpnameprefix = (*it).planTask.tunnelname.c_str() + QString("_") + date;
            tmptunnelid = (*it).planTask.tunnelnum;

            ret2 = NetworkConfigList::getNetworkConfigListInstance()->findSlaveModelBySlaveIndex(i, tmpsm);
            if (!ret2)
            {
                break;
            }
            else
            {
                // @author 范翔 @date 20150527 注释掉，因为RT计算变成两线程，与其他普通从控计算任务一致
                /*if (tmpsm.getIsRT())
                {
                    tmpboxindex = QString("%1").arg(tmpsm.box1.boxindex);
                    ret2 = (*it).calcuItem.getHasBackupCalc(tmpboxindex.toStdString(), tmphasbackup, tmpinterruptfc);
                    if (ret2)
                    {
                        tmpfilename = tmpnameprefix +  QString("_RT") + ".mdat";
                        if (tmphasbackup == 0)
                            calculate_StartOneFile(i, tmpboxindex, tmpfilename, tmptunnelid, false, 0);
                        else if (tmphasbackup == 1)
                            calculate_StartOneFile(i, tmpboxindex, tmpfilename, tmptunnelid, true, tmpinterruptfc);
                        else
                            ;//tmphasbackupnum++;
                    }
                }
                else*/
                {
                    tmpboxindex = QString("%1").arg(tmpsm.box1.boxindex);
                    ret2 = (*it).calcuItem.getHasBackupCalc(tmpboxindex.toStdString(), tmphasbackup, tmpinterruptfc);
                    if (ret2)
                    {
                        tmpfilename = tmpnameprefix +  QString("_%1").arg(tmpboxindex) + ".mdat";
                        if (tmphasbackup == 0)
                            calculate_StartOneFile(i, tmpboxindex, tmpfilename, tmptunnelid, false, 0);
                        else if (tmphasbackup == 1)
                            calculate_StartOneFile(i, tmpboxindex, tmpfilename, tmptunnelid, true, tmpinterruptfc);
                        else
                            ;//tmphasbackupnum++;
                    }
                    tmpboxindex = QString("%1").arg(tmpsm.box2.boxindex);
                    ret2 = (*it).calcuItem.getHasBackupCalc(tmpboxindex.toStdString(), tmphasbackup, tmpinterruptfc);
                    if (ret2)
                    {
                        tmpfilename = tmpnameprefix +  QString("_%1").arg(tmpboxindex) + ".mdat";
                        if (tmphasbackup == 0)
                            calculate_StartOneFile(i, tmpboxindex, tmpfilename, tmptunnelid, false, 0);
                        else if (tmphasbackup == 1)
                            calculate_StartOneFile(i, tmpboxindex, tmpfilename, tmptunnelid, true, tmpinterruptfc);
                        else
                            ;//tmphasbackupnum++;
                    }
                }
            }
            it++;
        }
        emit calcubackupProgressingBar(WorkingStatus::Calculating, QString("%1").arg(i), 1, totalhasbackupnum, tmphasbackupnum, false);
    }
    appendMsg(QObject::tr("开始计算"));
}

void CalcuNoSlaveWidget::calculate_beginStartOneTunnel(int tocalcutunnelid)
{
    SlaveModel tmpsm(1.0);
    QString tmpfilename = "";
    QString date = LzProjectAccess::getLzProjectAccessInstance()->getProjectFilename(LzProjectClass::Calculate).right(13).left(8);
    QString tmpnameprefix = "";
    qDebug() << "date:" << date;
    int tmptunnelid = -1;
        
    bool ret = false, ret2 = false;
    QString tmpboxindex;
    int tmphasbackup;
    __int64 tmpinterruptfc;
    int tmphasbackupnum = 0;
    int totalhasbackupnum = 2; // 计算一条隧道，每个从机两个任务

    int i = ui->comboBox->currentText().toInt();
    //for (int i = 1; i < 10; i++)
    //{
        tmphasbackupnum = 0;

        CheckedTunnelTaskModel tmpmodel;
        ret = LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Calculate).getCheckedTunnelModel(tocalcutunnelid, tmpmodel);
        if (ret)
        {
            tmpnameprefix = tmpmodel.planTask.tunnelname.c_str() + QString("_") + date;
            tmptunnelid = tmpmodel.planTask.tunnelnum;

            ret2 = NetworkConfigList::getNetworkConfigListInstance()->findSlaveModelBySlaveIndex(i, tmpsm);
            if (!ret2)
            {
                ret = false;
                //break;
            }
            else
            {
                // @author 范翔 @date 20150527 注释掉，因为RT计算变成两线程，与其他普通从控计算任务一致
                /*if (tmpsm.getIsRT())
                {
                    tmpboxindex = QString("%1").arg(tmpsm.box1.boxindex);
                    ret2 = tmpmodel.calcuItem.getHasBackupCalc(tmpboxindex.toStdString(), tmphasbackup, tmpinterruptfc);
                    if (ret2)
                    {
                        tmpfilename = tmpnameprefix +  QString("_RT") + ".mdat";
                        if (tmphasbackup == 0)
                            calculate_StartOneFile(i, tmpboxindex, tmpfilename, tmptunnelid, false, 0);
                        else if (tmphasbackup == 1)
                            calculate_StartOneFile(i, tmpboxindex, tmpfilename, tmptunnelid, true, tmpinterruptfc);
                        else
                            ;//tmphasbackupnum++;
                    }
                }
                else*/
                {
                    tmpboxindex = QString("%1").arg(tmpsm.box1.boxindex);
                    ret2 = tmpmodel.calcuItem.getHasBackupCalc(tmpboxindex.toStdString(), tmphasbackup, tmpinterruptfc);
                    if (ret2)
                    {
                        tmpfilename = tmpnameprefix +  QString("_%1").arg(tmpboxindex) + ".mdat";
                        if (tmphasbackup == 0)
                            calculate_StartOneFile(i, tmpboxindex, tmpfilename, tmptunnelid, false, 0);
                        else if (tmphasbackup == 1)
                            calculate_StartOneFile(i, tmpboxindex, tmpfilename, tmptunnelid, true, tmpinterruptfc);
                        else
                            ;//tmphasbackupnum++;
                    }
                    tmpboxindex = QString("%1").arg(tmpsm.box2.boxindex);
                    ret2 = tmpmodel.calcuItem.getHasBackupCalc(tmpboxindex.toStdString(), tmphasbackup, tmpinterruptfc);
                    if (ret2)
                    {
                        tmpfilename = tmpnameprefix +  QString("_%1").arg(tmpboxindex) + ".mdat";
                        if (tmphasbackup == 0)
                            calculate_StartOneFile(i, tmpboxindex, tmpfilename, tmptunnelid, false, 0);
                        else if (tmphasbackup == 1)
                            calculate_StartOneFile(i, tmpboxindex, tmpfilename, tmptunnelid, true, tmpinterruptfc);
                        else
                            ;//tmphasbackupnum++;
                    }
                }
            }
        }
        emit calcubackupProgressingBar(WorkingStatus::Calculating, QString("%1").arg(i), 1, totalhasbackupnum, tmphasbackupnum, false);
    //}
    
    appendMsg(QObject::tr("开始计算"));
}

void CalcuNoSlaveWidget::calculate_StartOneFile(int slaveid, QString cameragroupindex, QString filename, int tunnelid, bool isinterrupted, qint64 interruptfc)
{
    SlaveModel tmpSlaveModel(1.0);
    bool ret = NetworkConfigList::getNetworkConfigListInstance()->findSlaveModelBySlaveIndex(slaveid, tmpSlaveModel);
    if (ret)
    {
        ToDoMsg todomsg;
        QString isinterruptedstr = "false";
        if (isinterrupted)
            isinterruptedstr = "true";

        // 2001,filename=xxx,tunnelid=xx,cameragroup_index=xx,isinterrupted=false(true),interruptfc=xx
        todomsg.msg = QString("2001,filename=%1,tunnelid=%2,cameragroup_index=%3,isinterrupted=%4,interruptfc=%5").arg(filename).arg(tunnelid).arg(cameragroupindex).arg(isinterruptedstr).arg(interruptfc);
        lzCalcQueue->pushBack(todomsg);
        
        ////TODO
        //multiThreadTcpServer->sendMessageToOneSlave(tmpSlaveModel.getHostAddress().c_str(), QString("2001,filename=%1,tunnelid=%2,cameragroup_index=%3").arg(filename).arg(tunnelid).arg(cameragroupindex));
    }
}

/**
 * 暂停计算
 */
void CalcuNoSlaveWidget::calculate_Stop()
{
    lzCalcQueue->suspend();
    appendMsg(QObject::tr("[暂停计算"));
}

/**
 * 融合计算之前的数据检查
 */
void CalcuNoSlaveWidget::calculate_Fuse_checkisready()
{
    ////TODO
}

/**
 * 开始融合计算和提高度计算
 * 前提是在双目及车底RT计算的中间结果文件全部返回之后
 */
void CalcuNoSlaveWidget::calculate_Fuse_beginAll()
{
    QString projectname = LzProjectAccess::getLzProjectAccessInstance()->getProjectFilename(LzProjectClass::Calculate);
    QString date = projectname.right(13).left(8);
    ToDoMsg todomsg;

    int tmphasbackup;
    __int64 tmpinterruptfc;
    int tmphasbackupnum = 0;
    int totalhasbackupnum = LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Calculate).list()->size();

    bool ret2 = false;
    QList<CheckedTunnelTaskModel>::iterator it = LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Calculate).begin();
    while (it != LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Calculate).end())
    {

        ret2 = (*it).calcuItem.getHasBackupCalc("fuse", tmphasbackup, tmpinterruptfc);
        if (ret2)
        {
            int tunnelid = (*it).planTask.tunnelnum;
            QString filename = QObject::tr((*it).planTask.tunnelname.c_str()) + "_" + date;
            if (tmphasbackup == 1)
                todomsg.msg = QString("-30,%1,%2,%3,%4").arg(tunnelid).arg(filename).arg(tmphasbackup).arg(tmpinterruptfc);
            else
                todomsg.msg = QString("-30,%1,%2,%3,%4").arg(tunnelid).arg(filename).arg(tmphasbackup).arg(0);
            lzFuseCalcQueue->pushBack(todomsg);
        }
        it++;
    }
    emit calcubackupProgressingBar(WorkingStatus::Calculating, QString("%1").arg(0), 1, totalhasbackupnum, tmphasbackupnum, false);
}

void CalcuNoSlaveWidget::calculate_Fuse_beginOneTunnel(int tunnelid)
{
    QString projectname = LzProjectAccess::getLzProjectAccessInstance()->getProjectFilename(LzProjectClass::Calculate);
    QString date = projectname.right(13).left(8);
    ToDoMsg todomsg;

    int tmphasbackup;
    __int64 tmpinterruptfc;
    int tmphasbackupnum = 0;
    int totalhasbackupnum = LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Calculate).list()->size();

    bool ret2 = false;
    QList<CheckedTunnelTaskModel>::iterator it = LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Calculate).begin();
    while (it != LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Calculate).end())
    {

        ret2 = (*it).calcuItem.getHasBackupCalc("fuse", tmphasbackup, tmpinterruptfc);
        if (ret2)
        {

            if (tunnelid == (*it).planTask.tunnelnum)
            {
                QString filename = QObject::tr((*it).planTask.tunnelname.c_str()) + "_" + date;
                if (tmphasbackup == 1)
                    todomsg.msg = QString("-30,%1,%2,%3,%4").arg(tunnelid).arg(filename).arg(tmphasbackup).arg(tmpinterruptfc);
                else
                    todomsg.msg = QString("-30,%1,%2,%3,%4").arg(tunnelid).arg(filename).arg(tmphasbackup).arg(0);

                lzFuseCalcQueue->pushBack(todomsg);
                break;
            }
        }
        it++;
    }
    emit calcubackupProgressingBar(WorkingStatus::Calculating, QString("%1").arg(0), 1, totalhasbackupnum, tmphasbackupnum, false);
}

void CalcuNoSlaveWidget::calculate_ExtractHeight_beginOneTunnel(int tunnelid)
{
    QString projectname = LzProjectAccess::getLzProjectAccessInstance()->getProjectFilename(LzProjectClass::Calculate);
    QString date = projectname.right(13).left(8);
    ToDoMsg todomsg;

    int tmphasbackup;
    __int64 tmpinterruptfc;
    int tmphasbackupnum = 0;
    int totalhasbackupnum = LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Calculate).list()->size();

    bool ret2 = false;
    QList<CheckedTunnelTaskModel>::iterator it = LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Calculate).begin();
    while (it != LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Calculate).end())
    {

        ret2 = (*it).calcuItem.getHasBackupCalc("fuse", tmphasbackup, tmpinterruptfc);
        if (ret2)
        {

            if (tunnelid == (*it).planTask.tunnelnum)
            {
                QString filename = QObject::tr((*it).planTask.tunnelname.c_str()) + "_" + date;
                //if (tmphasbackup == 1)
                    todomsg.msg = QString("-30,%1,%2,%3,%4").arg(tunnelid).arg(filename).arg(tmphasbackup).arg(tmpinterruptfc);
                //else
                    todomsg.msg = QString("-30,%1,%2,%3,%4").arg(tunnelid).arg(filename).arg(tmphasbackup).arg(0);
                    QString projectpath = LzProjectAccess::getLzProjectAccessInstance()->getProjectPath(Calculate);

                    qDebug() << projectpath + "/fuse_calcu/" + filename + ".fdat";

                    qDebug() << projectpath + "/syn_calcu/" + filename + ".syn";


                //lzFuseCalcQueue->pushBack(todomsg);
                break;
            }
        }
        it++;
    }

}

void CalcuNoSlaveWidget::calculate_Fuse_stop()
{
    lzFuseCalcQueue->suspend();
}

////////////////////////////////////////////////
//////////////从控函数复制//////////////////////
/**
 * 切换工作模式-采集、备份时新工程配置
 * 设置文件接收路径
 *     工程路径
 *     工程文件.proj解析加载
 * @author 范翔 @date 20140509
 */
bool CalcuNoSlaveWidget::changeMode_newProjectConfig(LzProjectClass projectclass, QString newfilename)
{
    // @author 范翔 @date 20141122 从控只执行，不判断！
    /*
    if (lzAcquizQueue->getIsAcquizing() == true)
        return false;
    */

    // 按当前时间创建空目录，等待存储
    QString linename = newfilename.left(newfilename.length() - 14);
    QString datetime11 = newfilename.right(13).left(8);
    qDebug() << "linename=" << linename << ",datatime=" << datetime11;
    QString newname = linename + "_" + datetime11;

    // 创建的新的工程目录 【参考】设计-每个工程目录结构.txt
    QString newprojectfilepath = SlaveSetting::getSettingInstance()->getParentPath() + "/" + newname;

    // 如果没有该目录，创建新目录
    bool ret = SlaveSetting::getSettingInstance()->createSlaveProjectDir(newprojectfilepath);

    // 设置工作工程文件名
    ret = LzProjectAccess::getLzProjectAccessInstance()->setProjectName(projectclass, newprojectfilepath + "/" + newfilename);
    
    LzProjectAccess::getLzProjectAccessInstance()->getConfigBoolean(projectclass, ConfigFileType::Config_Network) = false;
    LzProjectAccess::getLzProjectAccessInstance()->getConfigBoolean(projectclass, ConfigFileType::Config_Calibration) = false;
    LzProjectAccess::getLzProjectAccessInstance()->getConfigBoolean(projectclass, ConfigFileType::Config_FenZhong) = true;  // @author 范翔 每组相机倒到分中坐标系的配置文件与双目标定配置文件二合一
    LzProjectAccess::getLzProjectAccessInstance()->getConfigBoolean(projectclass, ConfigFileType::Config_Plan) = false;
    LzProjectAccess::getLzProjectAccessInstance()->getConfigBoolean(projectclass, ConfigFileType::Config_Real) = false;
    LzProjectAccess::getLzProjectAccessInstance()->getConfigBoolean(projectclass, ConfigFileType::Config_Checked) = false;

    QString newprojectdir = SlaveSetting::getSettingInstance()->getParentPath() + "/" + newfilename.left(newfilename.length() - 5);

    // @author 范翔,初始化融合计算队列
    lzFuseCalcQueue->initCalc();

    if (ret)
    {
        appendMsg(QString("配置本次线路工程成功%1").arg(newprojectdir));
        ret = checkedok(Calculating, newname, linename, datetime11);
    }
    else
    {
        appendMsg(QString("配置本次线路工程失败%1").arg(newprojectdir));
    }
    
    return ret;
}

/**
 * 每次启动都要向主控申请task文件（如果是上次备份又开始则不需要申请）
 * @author 冯明晨
 * @author 熊雪改
 */
bool CalcuNoSlaveWidget::init_TaskFile(QString filename)
{
    // @author 范翔 @date 20141122 从控只执行，不判断！
    /*if (lzAcquizQueue->getIsAcquizing() == true)
    {
        qDebug() << QString("正在采集,不能重新加载计划任务配置文件");
        appendMsg(QString("正在采集,不能重新加载计划任务配置文件"));
        return false;
    }*/
    bool & plantaskok = LzProjectAccess::getLzProjectAccessInstance()->getConfigBoolean(LzProjectClass::Collect, ConfigFileType::Config_Plan);

    plantaskok = false;
    QFile file3(filename);//file3代表任务文件
    if(file3.exists())
    {
        // @fanxiang 改
        // 【注意！】QString转cchar不能用QString().toStdString().c_str()，否则可能会出问题
        QByteArray tempstr = filename.toLocal8Bit();
        XMLTaskFileLoader * taskFileLoader = new XMLTaskFileLoader(tempstr);
        bool ret = taskFileLoader->loadFile(LzProjectAccess::getLzProjectAccessInstance()->getLzPlanList(LzProjectClass::Collect));
        delete taskFileLoader;
        if (ret)
        {
            appendMsg(QString("任务文件解析完毕！"));
            plantaskok = true;
            file3.close();
            return true;
        }
        qDebug() << "load task file:" << ret;
    }

    file3.close();
    appendMsg(QString("计划任务文件不存在或解析失败，向主控申请任务文件。。。"));
    return false;
}

/**
 * 加载网络从控及相机SNID硬件，双目视觉标定文件名称配置文件
 */
bool CalcuNoSlaveWidget::init_NetworkCameraHardwareFile(LzProjectClass projectclass, QString filename)
{
    // @author 范翔 @date 20141122 从控只执行，不判断！
    /*if (lzAcquizQueue->getIsAcquizing() == true)
    {
        qDebug() << QString("正在采集,不能重新加载网络硬件配置文件");
        appendMsg(QString("正在采集,不能重新加载网络硬件配置文件"));
        return false;
    }*/
    bool & networkok = LzProjectAccess::getLzProjectAccessInstance()->getConfigBoolean(projectclass, ConfigFileType::Config_Network);
    networkok = false;
    // @author fanxiang注文件解析
    qDebug() << "init_NetworkCameraHardwareFile" << filename;
    QFile file0(filename);//file0代表index和camid的文件
    if(file0.exists())
    {
        file0.close();
        qDebug() << QObject::tr("index文件存在,正在解析...");//在控制台上显示
        // @author fanxiang注文件解析

        XMLNetworkFileLoader* ff2 = new XMLNetworkFileLoader(filename);
        bool ret = ff2->loadFile();
        delete ff2;
        if (ret)
        {
            //NetworkConfigList::getNetworkConfigListInstance()->showList();

            int index = ui->comboBox->currentText().toInt();

            ret = NetworkConfigList::getNetworkConfigListInstance()->findSlaveModelBySlaveIndex(index,
                                                                      SlaveSetting::getSettingInstance()->getCurrentSlaveModel());
            //SlaveSetting::getSettingInstance()->setHasModel(ret);

            SlaveModel & slaveModel = SlaveSetting::getSettingInstance()->getCurrentSlaveModel();
            // nasip
            SlaveSetting::getSettingInstance()->setNASAccessIP(slaveModel.getBackupNasIP().c_str());

            lzCalcQueue->initSlaveModel(slaveModel);
            lzCalcQueue->initCalc();

            if (ret)
            {
				appendMsg(QString("从机%1的NetworkCameraHardwareConfig文件解析成功%2！").arg(index).arg(filename));
				emit showSlaveID();
                return true;
            }
			else
                appendMsg(QString("网络硬件配置文件不存在或解析失败%1,可能文件中不存在从机%2").arg(filename).arg(index));
        }
    }
    file0.close();
    return false;
}

/**
 * 接收校正文件
 */
bool CalcuNoSlaveWidget::loadRealTaskFile(LzProjectClass projectclass, QString filename)
{
    // @author 范翔 @date 20141122 从控只执行，不判断！
    /*if (lzAcquizQueue->getIsAcquizing() == true)
    {
        //qDebug() << QString("正在采集,不能重新加载实际采集记录配置文件");
        appendMsg(QString("正在采集,不能重新加载实际采集记录配置文件"));
        return false;
    }*/

    bool & realtaskok = LzProjectAccess::getLzProjectAccessInstance()->getConfigBoolean(projectclass, ConfigFileType::Config_Real);
    realtaskok = false;
    // @author fanxiang注文件解析
    QFile file0(filename);//file0代表index和camid的文件
    if(file0.exists())
    {
        file0.close();
        XMLRealTaskFileLoader * ff3 = new XMLRealTaskFileLoader(filename);
        bool ret = ff3->loadFile(LzProjectAccess::getLzProjectAccessInstance()->getLzRealList(projectclass));
        delete ff3;
        if (ret)
        {
            appendMsg(QString("校正任务checked_task文件解析成功%1！").arg(filename));
            realtaskok = true;
            return true;
        }
    }
    file0.close();
    appendMsg(QString("实际采集记录real_task文件不存在或解析失败%1！").arg(filename));
    return false;
}

/**
 * 接收校正文件
 */
bool CalcuNoSlaveWidget::loadCorrectedTaskFile(LzProjectClass projectclass, QString filename)
{
    // @author 范翔 @date 20141122 从控只执行，不判断！
    /*if (lzAcquizQueue->getIsAcquizing() == true)
    {
        qDebug() << QString("正在采集,不能重新加载采集任务校正配置文件");
        appendMsg(QString("正在采集,不能重新加载采集任务校正配置文件"));
        return false;
    }*/
    bool & checkedtaskok = LzProjectAccess::getLzProjectAccessInstance()->getConfigBoolean(projectclass, ConfigFileType::Config_Checked);
    checkedtaskok = false;
    // @author fanxiang注文件解析
    QFile file0(filename);//file0代表index和camid的文件
    if(file0.exists())
    {
        file0.close();
        XMLCheckedTaskFileLoader * ff3 = new XMLCheckedTaskFileLoader(filename);
        bool ret = ff3->loadFile(LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(projectclass));
        delete ff3;
        if (ret)
        {
            appendMsg(QString("校正任务checked_task文件解析成功%1！").arg(filename));
            checkedtaskok = true;
            return true;
        }
    }
    file0.close();
    appendMsg(QString("校正任务checked_task文件不存在或解析失败%1！").arg(filename));
    return false;
}

/**
 * 加载分中硬件配置文件
 */
bool CalcuNoSlaveWidget::init_FenZhongFile(QString projectpath, SlaveModel & sm)
{
    // @author 范翔 @date 20141122 从控只执行，不判断！
    /*if (lzAcquizQueue->getIsAcquizing() == true)
    {
        qDebug() << QString("正在采集,不能重新加载轨平面分中坐标系标定参数文件");
        appendMsg(QString("正在采集,不能重新加载轨平面分中坐标系标定参数文件"));
        return false;
    }*/
    bool & fenzhongok = LzProjectAccess::getLzProjectAccessInstance()->getConfigBoolean(LzProjectClass::Calculate, ConfigFileType::Config_FenZhong);
    fenzhongok = true;
    QFile file1(projectpath + "/calcu_calibration/" + sm.box1.box_fenzhong_calib_file.c_str());//file1代表分中参数的文件
    if (!file1.exists())
    {
        fenzhongok = false;
        file1.close();
    }
    QFile file2(projectpath + "/calcu_calibration/" + sm.box2.box_fenzhong_calib_file.c_str());
    if (!file2.exists())
    {
        fenzhongok = false;
        file2.close();
    }
    file1.close();
    file2.close();

    if (fenzhongok)
    {
        appendMsg(QString("轨平面分中坐标系标定参数文件存在！"));
        fenzhongok = true;
        return true;
    }
    else
    {
        appendMsg(QString("轨平面分中坐标系标定参数文件不存在！%1").arg(projectpath + "/calcu_calibration/" + sm.box1.box_fenzhong_calib_file.c_str() + ", " + projectpath + "/calcu_calibration/" + sm.box2.box_fenzhong_calib_file.c_str()));
        return false;
    }
    return false;
}

/**
 * 加载双目视觉标定配置文件
 */
bool CalcuNoSlaveWidget::init_CameraCalibrationFile(QString projectpath, SlaveModel & sm)
{
    // @author 范翔 @date 20141122 从控只执行，不判断！
    /*if (lzAcquizQueue->getIsAcquizing() == true)
    {
        qDebug() << QString("正在采集,不能重新加载双目相机标定配置文件");
        appendMsg(QString("正在采集,不能重新加载双目相机标定配置文件"));
        return false;
    }*/
    bool & calibrationok = LzProjectAccess::getLzProjectAccessInstance()->getConfigBoolean(LzProjectClass::Calculate, ConfigFileType::Config_Calibration);
    calibrationok = true;
    QFile file1(projectpath + "/calcu_calibration/" + sm.box1.box_camera_calib_file.c_str());//file1代表分中参数的文件
    if (!file1.exists())
    {
        calibrationok = false;
        file1.close();
    }
    QFile file2(projectpath + "/calcu_calibration/" + sm.box2.box_camera_calib_file.c_str());
    if (!file2.exists())
    {
        calibrationok = false;
        file2.close();
    }
    file1.close();
    file2.close();

    if (calibrationok)
    {
        appendMsg(QString("相机标定配置文件存在！%1").arg(projectpath + "/calcu_calibration/" + sm.box1.box_camera_calib_file.c_str() + ", " + projectpath + "/calcu_calibration/" + sm.box2.box_camera_calib_file.c_str()));
        calibrationok = true;
        return true;
    }
    else
    {
        appendMsg(QString("相机标定配置文件不存在！%1").arg(projectpath + "/calcu_calibration/" + sm.box1.box_camera_calib_file.c_str() + ", " + projectpath + "/calcu_calibration/" + sm.box2.box_camera_calib_file.c_str()));
        return false;
    }
    return false;
}

/**
 * 初始化完成
 * @param WorkingStatus function 相当于一个枚举，1表示是否可以开始采集，(2计算, 3备份--作废) 4表示是否可以开始计算备份
 * @param QString filename 要解析的任务文件，当采集时需要计划文件"xxxx_20140211.plan"
 *                                        当计算备份时需要校正文件"xxxx_20140211.check"
 * @param QString linename, QString date 暂时没用，与filename冲突
 */
bool CalcuNoSlaveWidget::checkedok(WorkingStatus function, QString filename, QString linename, QString date)
{
    // @author 范翔 @date 20141122 从控只执行，不判断！
    /*if (lzAcquizQueue->getIsAcquizing() == true)
    {
        qDebug() << QString("正在采集,不能重新配置采集任务");
        appendMsg(QString("正在采集,不能重新配置采集任务"));
        return;
    }*/
    
    LzProjectClass projectclass;
    switch (function)
    {
        case Collecting : projectclass = LzProjectClass::Collect; break;
        case Calculating : projectclass = LzProjectClass::Calculate; break;
        case Backuping: projectclass = LzProjectClass::Backup; break;
        case Calculating_Backuping : projectclass = LzProjectClass::Calculate; break;
        default: projectclass = LzProjectClass::Main; break;
    }

    bool ret = false;
    QString projectfilename = LzProjectAccess::getLzProjectAccessInstance()->getProjectFilename(projectclass);
    QString projectdirname = SlaveSetting::getSettingInstance()->getParentPath() + "/" + projectfilename.left(projectfilename.length() - 5);
    filename = projectdirname + "/" + filename;
    //qDebug() << filename;

    bool & networkok = LzProjectAccess::getLzProjectAccessInstance()->getConfigBoolean(projectclass, ConfigFileType::Config_Network);
    bool & plantaskok = LzProjectAccess::getLzProjectAccessInstance()->getConfigBoolean(projectclass, ConfigFileType::Config_Plan);
    bool & realtaskok = LzProjectAccess::getLzProjectAccessInstance()->getConfigBoolean(projectclass, ConfigFileType::Config_Real);
    bool & checkedtaskok = LzProjectAccess::getLzProjectAccessInstance()->getConfigBoolean(projectclass, ConfigFileType::Config_Checked);

    if (!networkok)
        networkok = init_NetworkCameraHardwareFile(projectclass, projectdirname + "/network_config.xml");

    if (!networkok)
        return false;

    ProjectModel currentProjectModel = LzProjectAccess::getLzProjectAccessInstance()->getProjectModel(projectclass);

    if (function == WorkingStatus::Calculating) // 计算
    {
        checkedtaskok = false;
        if (!checkedtaskok)
        {
            // 检查实际采集和校正任务文件
            QString realfilename = projectdirname + "/" + currentProjectModel.getRealFilename();
            ret = loadRealTaskFile(LzProjectClass::Calculate, realfilename);
            if (ret == false)
                return false;

            QString chekedfilename = projectdirname + "/" + currentProjectModel.getCheckedFilename();
            ret = loadCorrectedTaskFile(LzProjectClass::Calculate, chekedfilename);
            if (ret == false)
                return false;

            SlaveModel & slaveModel = SlaveSetting::getSettingInstance()->getCurrentSlaveModel();
            ret = init_CameraCalibrationFile(projectdirname, slaveModel);
            if (ret == false)
                return false;
            
            // @author 范翔 每组相机倒到分中坐标系的配置文件与双目标定配置文件二合一
            //ret = init_FenZhongFile(projectdirname, slaveModel);
            //if (ret == false)
            //    return;

        }

        emit initGroupBox(Calculating);
    }
    return true;
}

// 导入工程文件
void CalcuNoSlaveWidget::importProj()
{
    QString openFileDir = SlaveSetting::getSettingInstance()->getParentPath();
    QString fileName;
    fileName = QFileDialog::getOpenFileName(this, tr("导入已有工程"), openFileDir, tr("工程文件 (*.proj)"));
    if (!fileName.isEmpty())
    {
        bool ret = LzProjectAccess::getLzProjectAccessInstance()->setProjectName(projectclass, fileName);
        if (ret == false)
        {
            ui->currentProjectNameLabel->setText(tr("导入工程文件错误"));
            return;
        }
        QString projectfilename = LzProjectAccess::getLzProjectAccessInstance()->getProjectFilename(projectclass);
        ui->currentProjectNameLabel->setText(tr("%1").arg(projectfilename));
        
        emit sendproject_file_name(projectfilename);
        this->projectfilename = projectfilename;
        // 初始时确认开始按钮不可点击
        ui->operationConfigButton->setEnabled(true);
    }
    else
    {
        ui->currentProjectNameLabel->setText(tr("请选择一个采集工程配置文件。"));
    }
}

void CalcuNoSlaveWidget::receiveThreadStart(int threadid, bool isok, int tunnelid, QString filename, qint64 beginfc, qint64 endfc)
{
    emit changeSlaveCameraTaskFC_init(Calculating, "1", threadid, filename, beginfc, endfc);
}

void CalcuNoSlaveWidget::receiveThreadFinish(int threadid, int isok, int tunnelid, QString filename)
{
    emit changeSlaveCameraTask(Calculating, "1", threadid, filename, QString("%1").arg(isok));
}

/*void CalcuNoSlaveWidget::receiveStatusShow(int threadid, qint64 tmpfc, int tunnelid, QString filename)
{
    emit changeSlaveCameraTaskFC(Calculating, "1", threadid, filename, tmpfc);
}*/

void CalcuNoSlaveWidget::receivefcupdate(WorkingStatus status, int threadid, QString filename, qint64 tmpfc)
{
    emit changeSlaveCameraTaskFC(Calculating, "1", threadid, filename, tmpfc);
}

void CalcuNoSlaveWidget::sendParsedMsgToGUI(QString msg)
{
    appendMsg(msg);
}

void CalcuNoSlaveWidget::sendMsgToMaster(QString msg)
{
    QStringList strList = msg.split(",", QString::SkipEmptyParts);
    if (strList.length() < 1)
    {
        qDebug() << tr("解析字符出错") << msg;
        return;
    }
    int msgid = strList.value(0).toInt();
    int slaveid = ui->comboBox->currentText().toInt();

    QString filename = "";
    QString camera_groupindex = "";
    int tunnelid = -1;
    int threadid = -1;
    if (msgid != 2100 && msgid != 2402)
    {
        if (strList.length() < 5 || !strList.value(1).startsWith("threadid") || !strList.value(2).startsWith("filename"))
        {
            qDebug() << tr("解析字符出错") << msg;
            return;
        }
        threadid = strList.value(1).mid(9).toInt();
        filename = strList.value(2).mid(9);
    }
    switch(msgid)
    {
        case 2002:
        {
            // 2002,threadid=x,filename=xxx,tunnelid=x,camera_groupindex=xx,return=true,beginfc=x,endfc=x
            if (strList.length() < 8 || !strList.value(4).startsWith("camera_groupindex") || !strList.value(5).startsWith("return") || !strList.value(6).startsWith("beginfc") || !strList.value(7).startsWith("endfc"))
            {
                qDebug() << tr("解析字符出错") << msg;
                return;
            }
            tunnelid = strList.value(3).mid(9).toInt();
            camera_groupindex = strList.value(4).mid(18);
            QString returnVal = strList.value(5).mid(7);
            qint64 beginfc = strList.value(6).mid(8).toLongLong();
            qint64 endfc = strList.value(7).mid(6).toLongLong();

            if (returnVal.compare("true") == 0)
            {
                emit changeSlaveCameraTask(WorkingStatus::Calculating, QString("%1").arg(slaveid), threadid, filename, "");
                // 从控每个线程任务状态初始消息
                emit changeSlaveCameraTaskFC_init(WorkingStatus::Calculating, QString("%1").arg(slaveid), threadid, filename, beginfc, endfc);
                    
                appendMsg(QObject::tr("[从控%1] 线程%2正在计算隧道文件%3，相机组号为%4，起始帧号%5，终止帧号%6").arg(slaveid).arg(threadid).arg(filename).arg(camera_groupindex).arg(beginfc).arg(endfc));
            }
            else
                appendError(QObject::tr("[从控%1] 线程%2无法计算隧道文件%3，相机组号为%4，起始帧号%5，终止帧号%6").arg(slaveid).arg(threadid).arg(filename).arg(camera_groupindex).arg(beginfc).arg(endfc));
            break;
        }
        case 2010: // 2010,threadid=x,filename=xxx,tunnelid=x,camera_groupindex=xx
        {
            if (strList.length() < 5 || !strList.value(4).startsWith("camera_groupindex")
                        || !strList.value(3).startsWith("tunnelid"))
            {
                qDebug() << tr("解析字符出错") << msg;
                return;
            }
            tunnelid = strList.value(3).mid(9).toInt();
            camera_groupindex = strList.value(4).mid(18);

            //calculate_FeedbackPerTunnel_CameraGroup_R(threadid, filename, slaveid, tunnelid, camera_groupindex);

            emit changeSlaveCameraTask(WorkingStatus::Calculating, QString("%1").arg(slaveid), threadid, filename, "ok");
            appendMsg(QObject::tr("[从控%1] 计算完隧道%2，第%3组相机数据").arg(slaveid).arg(filename).arg(camera_groupindex));
            break;
        }
        case 2020:
        {
            // 2020,threadid=x,filename=xxx,tunnelid=x,camera_groupindex=x,currentfc=x
            if (strList.length() < 6 || !strList.value(4).startsWith("camera_groupindex")
                    || !strList.value(3).startsWith("tunnelid") || !strList.value(5).startsWith("currentfc"))
            {
                qDebug() << tr("解析字符出错") << msg;
                return;
            }
            tunnelid = strList.value(3).mid(9).toInt();
            camera_groupindex = strList.value(4).mid(18);
            long long currentfc = strList.value(5).mid(10).toLongLong();
            emit changeSlaveCameraTaskFC(WorkingStatus::Calculating, QString("%1").arg(slaveid), threadid, filename, currentfc);

            break;
        }
        case 2200:
        {
            // 2200,threadid=x,filename=xxx,tunnelid=x,camera_groupindex=x,errortype=x
            if (strList.length() < 6 || !strList.value(4).startsWith("camera_groupindex")
                    || !strList.value(3).startsWith("tunnelid") || !strList.value(5).startsWith("errortype"))
            {
                qDebug() << tr("解析字符出错") << msg;
                return;
            }
            tunnelid = strList.value(3).mid(9).toInt();
            camera_groupindex = strList.value(4).mid(18);
            QString error = strList.value(5).mid(10);
            appendError(QObject::tr("[从控%1] 计算隧道%2出错!!错误类型%3。第%4组相机数据")
                                .arg(slaveid).arg(filename).arg(error).arg(camera_groupindex));
            emit changeSlaveCameraTask(WorkingStatus::Calculating, QString("%1").arg(slaveid), threadid, filename, QObject::tr("出错%1").arg(error));

            // 异常处理
            break;
        }
        case 2100:
        {
            appendMsg(QObject::tr("[从控%1] 全部任务计算完成").arg(slaveid));
            break;
        }
        case 2402:
        {
            // 确认暂停计算2402,return=true(false)
            if (strList.length() < 2 || !strList.value(1).startsWith("return"))
            {
                qDebug() << tr("解析字符出错") << msg;
                return;
            }
            QString retstr = strList.value(2).mid(7);

            if (retstr.compare("false") == 0)
            {
                appendMsg(QObject::tr("[从控%1] 暂停计算失败，正在计算中，请稍后"));
            }
            else if (retstr.compare("true") == 0)
            {
                appendMsg(QObject::tr("[从控%1] 暂停计算成功"));
            }
            break;
        }
        default:break;
    }
}