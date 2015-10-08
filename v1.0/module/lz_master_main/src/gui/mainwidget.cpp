#include "mainwidget.h"
#include "ui_mainwidget.h"

#include "tabwidgetmanager.h"
#include "create_project.h"

#include "status_collect.h"
#include "status_calcu_master.h"
#include "status_backup.h"

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
MainWidget::MainWidget(bool newsinglewidgetapp, WorkingStatus initstatus, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
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

    // 在主界面连接主控底层服务信号槽
    connect(MasterProgram::getMasterProgramInstance(), SIGNAL(makeGUIActive(WorkingStatus, bool)), this, SLOT(msgToGUICreateProjectWidget(WorkingStatus, bool)));
    connect(MasterProgram::getMasterProgramInstance(), SIGNAL(signalMsgToGUI(WorkingStatus, QString)), this, SLOT(appendMsg(WorkingStatus, QString)));
    connect(MasterProgram::getMasterProgramInstance(), SIGNAL(signalErrorToGUI(WorkingStatus, QString)), this, SLOT(appendError(WorkingStatus, QString)));
    // [采集转发消息]
    connect(MasterProgram::getMasterProgramInstance(), SIGNAL(signalToLights(HardwareType, QString, HardwareStatus)), this, SLOT(changeHardwareStatus(HardwareType, QString, HardwareStatus)));
    connect(MasterProgram::getMasterProgramInstance(), SIGNAL(signalToCameraFC(QString, qint64)), this, SLOT(changeCameraFC_slot(QString, qint64)));
    // [采集、计算、备份转发消息]
    connect(MasterProgram::getMasterProgramInstance(), SIGNAL(signalToCameraTask(WorkingStatus, QString, int, QString, QString)), this, SLOT(changeSlaveCameraTask_slot(WorkingStatus, QString, int, QString, QString)));
    // [计算转发消息]
    // 每个从控每个线程每个任务帧号反馈消息转发
    connect(MasterProgram::getMasterProgramInstance(), SIGNAL(signalToCameraTaskFC(WorkingStatus, QString, int, QString, long long)), this, SLOT(changeSlaveCameraTaskFC_slot(WorkingStatus, QString, int, QString, long long)));
    // 每个从控每个线程每个任务初始化界面（起始帧、终止帧）消息转发
    connect(MasterProgram::getMasterProgramInstance(), SIGNAL(signalToSlaveCameraTaskFC_init(WorkingStatus, QString, int, QString, long long, long long)), this, SLOT(changeSlaveCameraTaskFC_init_slot(WorkingStatus, QString, int, QString, long long, long long)));
    // [计算备份转发消息]
    connect(MasterProgram::getMasterProgramInstance(), SIGNAL(calcubackupProgressingBar(WorkingStatus, QString, int, int, int, bool)), this, SLOT(changecalcubackupProgressingBar_slot(WorkingStatus, QString, int, int, int, bool)));
    
    connect(MasterProgram::getMasterProgramInstance(), SIGNAL(turnOffAllHardware()), this, SLOT(turnOffLights()));
    //connect(MasterProgram::getMasterProgramInstance(), SIGNAL(makeGUIActive(bool)), this, SLOT(getCalcuBackupGUIActive(bool)));

    // 重启服务器按钮信号槽
    connect(ui->restartServerButton, SIGNAL(clicked()), this, SLOT(restartServer()));
    // 关闭服务器
    connect(ui->closeServerButton, SIGNAL(clicked()), this, SLOT(closeServer()));
 
    // 初始时关闭所有提示灯
    turnOffLights();
    // 开启网络连接从控计算机提示灯
    initConnectedSlaves();

    /******设置采集机、备份机、相机的状态，正常或者不正常**********/
    //   statuscollectmessage为从控机发送过来的采集机和备份机的状态信息，如0表示正常，则显示为黄色，1表示异常，则显示为红色。
    //     if 采集机异常
    //     ui->slave1Label_net->setPixmap(QPixmap(":/image/lightred.png"));

    this->setWindowTitle(QObject::tr("铁路隧道建筑限界测量系统-主控终端"));
    QIcon icon;
    icon.addFile(QString::fromUtf8(":image/icon.png"), QSize(), QIcon::Normal, QIcon::Off);
    this->setWindowIcon(icon);

    // 功能，设置小图标变灰
    ui->collectstatuslabel->setPixmap(QString::fromUtf8(":image//collectstatus1_2.png"));
    ui->backupstatuslabel->setPixmap(QString::fromUtf8(":image//up_alt_2.png"));
    ui->calculatestatuslabel->setPixmap(QString::fromUtf8(":image//Calculatorstatus1_2.png"));

    // 导入已有工程、创建新工程界面
    CreateProjectWidget * cpwidget = new CreateProjectWidget(workingstatus, ui->createprojwidget);
    connect(cpwidget, SIGNAL(sendproject_file_name(QString)), this, SLOT(slotprojectfilename(QString)));
    connect(this, SIGNAL(setCreateProjectWidgetEnabled(bool)), cpwidget, SLOT(setWidgetEnabled(bool)));

    // @author 范翔 3个界面合二为一之后，关闭重启服务器两个按钮无效
    ui->closeServerButton->setVisible(false);
    ui->restartServerButton->setVisible(false);

    if (workingstatus == Collecting)
    {        
        // Tab界面布局
        // ui->gridLayout_5是整个Window的最外层布局Layout。可从.ui文件中找出
        ui->gridLayout_5->addWidget(ui->page, 4, 0);
        CollectWidget * collectWidget = new CollectWidget(ui->page, MasterSetting::getSettingInstance()->getDefaultDistanceMode());

        QGridLayout *layout5 = new QGridLayout();
        layout5->addWidget(collectWidget);
        ui->page->setLayout(layout5);

        // 得到帧间隔里程
        connect(this, SIGNAL(sendproject_file_name(QString)), collectWidget,SLOT(getDistanceMode(QString)));

        // 界面信号槽
        connect(this, SIGNAL(changeCameraStatus(QString,HardwareStatus)), collectWidget, SLOT(changeCamera(QString, HardwareStatus)));
        connect(this, SIGNAL(changeLayserStatus(QString,HardwareStatus)), collectWidget, SLOT(changeLayser(QString, HardwareStatus)));
        connect(this, SIGNAL(changeCameraFC(QString,qint64)), collectWidget, SLOT(changeCameraFC(QString, qint64)));
        connect(this, SIGNAL(changeSlaveCameraTask(WorkingStatus, QString, int, QString, QString)), collectWidget, SLOT(changeCameraTask(WorkingStatus, QString, int, QString, QString)));
        connect(this, SIGNAL(turnOffAllHardware()), collectWidget, SLOT(turnOffHardwares()), Qt::DirectConnection);

        collectWidget->turnOffHardwares();

        // 【标记1】此处作废
        // 因为有master的单一实例模式+各种对应方法，此处无需接受底层界面发送的消息了，因为底层界面可以直接调用各类主控方法
        //connect(calcuWidget,SIGNAL(clicked111()),this,SLOT(sendToMaster(QString)));

        // 可以导入已有、创建新工程
        connect(ui->operationConfigButton, SIGNAL(clicked()), this, SLOT(configCurrentOperation()));
        connect(ui->operationConfigButton2, SIGNAL(clicked()), this, SLOT(configCurrentOperation2()));

        // 初始时的界面显示
        ui->page->setVisible(true);

        // projectclass
        projectclass = LzProjectClass::Collect;
        projectclassstr = "collect";
    }
    else if (workingstatus == Calculating) // 计算
    {
        // ui->gridLayout_5是整个Window的最外层布局Layout。可从.ui文件中找出
        ui->gridLayout_5->addWidget(ui->page_2, 4, 0);
        CalcuMasterWidget * calcuWidget = new CalcuMasterWidget(ui->page_2);
        QGridLayout *layout3 = new QGridLayout();
        layout3->addWidget(calcuWidget);
        ui->page_2->setLayout(layout3);

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

        connect(ui->operationConfigButton, SIGNAL(clicked()), this, SLOT(configCurrentOperation()));
        connect(ui->operationConfigButton2, SIGNAL(clicked()), this, SLOT(configCurrentOperation2()));

        // 不显示创建新工程按钮
        cpwidget->setToCalculateStyle();

        // 初始时的界面显示
        ui->page_2->setVisible(true);
 
        // projectclass
        projectclass = LzProjectClass::Calculate;
        projectclassstr = "calcu";
    }
    else if (workingstatus == Backuping) // 备份
    {
        // ui->gridLayout_5是整个Window的最外层布局Layout。可从.ui文件中找出
        ui->gridLayout_5->addWidget(ui->page_3, 4, 0);
        BackupWidget * backupWidget = new BackupWidget(ui->page_3);
        QGridLayout *layout3 = new QGridLayout();
        layout3->addWidget(backupWidget);
        ui->page_3->setLayout(layout3);

        // @author范翔 作废
        // 可以导入已有、不能创建新工程
        connect(this, SIGNAL(sendproject_file_name(QString)),backupWidget,SLOT(updateCheckedTunnelView()));
        connect(this, SIGNAL(changeSlaveCameraTask(WorkingStatus, QString, int, QString, QString)), backupWidget, SLOT(changeSlaveTask(WorkingStatus, QString, int, QString, QString)));
        connect(this, SIGNAL(calcubackupProgressingBar(WorkingStatus, QString, int, int, int, bool)), backupWidget, SLOT(changecalcubackupProgressingBar(WorkingStatus, QString, int, int, int, bool)));
        // 每个从控的每个线程每个任务
        // 每个从控的每个线程每个任务-进度初始化
        connect(this, SIGNAL(changeSlaveCameraTaskFC_init(WorkingStatus, QString, int, QString, long long, long long)), backupWidget, SLOT(changeSlaveCameraTaskFC_init(WorkingStatus, QString, int, QString, long long, long long)));
        // 每个从控的每个线程每个任务-当前任务帧号反馈
        connect(this, SIGNAL(changeSlaveCameraTaskFC(WorkingStatus, QString, int, QString, long long)), backupWidget, SLOT(changeSlaveCameraTaskFC(WorkingStatus, QString, int, QString, long long)));

        connect(this, SIGNAL(makeGUIActive(bool)), backupWidget, SLOT(setOptButtonEnable(bool)));

        connect(ui->operationConfigButton, SIGNAL(clicked()), this, SLOT(configCurrentOperation()));
        connect(ui->operationConfigButton2, SIGNAL(clicked()), this, SLOT(configCurrentOperation2()));


        // 不显示创建新工程按钮
        cpwidget->setToCalculateStyle();

        // 初始时的界面显示
        ui->page_3->setVisible(true);

        // projectclass
        projectclass = LzProjectClass::Backup;
        projectclassstr = "backup";
    }

    // 桌面系统图标相关，防止误操作关闭，可后台运行
    createActions();
    createTrayIcon();

    setIcon();
    connect(trayIcon, SIGNAL(messageClicked()), this, SLOT(messageClicked()));
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

    trayIcon->show();

    // 日志类
    logger = NULL;
    hasinitlog = false;

    // 初始时确认开始按钮不可点击
    ui->operationConfigButton->setEnabled(false);
    ui->operationConfigButton2->setEnabled(false);
    emit setCreateProjectWidgetEnabled(true);
}

MainWidget::~MainWidget()
{
    if (logger != NULL)
    {
        if (logger->isLogging())
        {
            logger->log(string("关闭工程"));
            logger->close();
        }
        delete logger;
    }

    delete ui;
}

void MainWidget::appendMsg(WorkingStatus status, QString msg)
{
    //qDebug() << status << msg;
    // 不属于这个界面
    if (status != workingstatus && status != WorkingStatus::Preparing)
    {
        if (status == Calculating_Backuping)
        {
            if (workingstatus == Calculating || workingstatus == Backuping)
                ;
            else
                return;
        }
        else
            return;
    }

    QString currentDate = QTime::currentTime().toString("[hh:mm:ss]");
    ui->statusArea->append(QString("%1%2").arg(currentDate).arg(msg));
    if (hasinitlog)
        logger->log(msg.toLocal8Bit().constData());
}

void MainWidget::appendError(WorkingStatus status, QString msg)
{
    // 不属于这个界面
    if (status != workingstatus && status != WorkingStatus::Preparing)
    {
        if (status == Calculating_Backuping)
        {
            if (workingstatus == Calculating || workingstatus == Backuping)
                ;
            else
                return;
        }
        else
            return;
    }

    QString currentDate = QTime::currentTime().toString("[hh:mm:ss]");
    ui->statusArea->append(QString("%1%2").arg(currentDate).arg(msg));
    if (hasinitlog)
        logger->log(msg.toLocal8Bit().constData());
}

void MainWidget::msgToGUICreateProjectWidget(WorkingStatus status, bool a)
{
    if (status != workingstatus && status != WorkingStatus::Preparing)
    {
        if (status == Calculating_Backuping)
        {
            if (workingstatus == Calculating || workingstatus == Backuping)
            {
                if (a)
                {
                    ui->operationConfigButton2->setEnabled(true);
                    emit setCreateProjectWidgetEnabled(false);
                }
                else
                {
                    ui->operationConfigButton2->setEnabled(false);
                    emit setCreateProjectWidgetEnabled(true);
                }
            }
            else
                return;
        }
        else
            return;
    }
}

void MainWidget::changeHardwareStatus(HardwareType type, QString hardwareidstr, HardwareStatus status)
{
    if (type == Slave_Computer_Type)
    {
        int hardwareid = hardwareidstr.toInt();
        if (status == Hardware_Off)
        {
            QString img = ":/image/lightred.png";
            // 关闭相机灯
            SlaveModel tmpSlaveModel(1.0);
            bool ret = NetworkConfigList::getNetworkConfigListInstance()->findSlaveModelBySlaveIndex(hardwareid, tmpSlaveModel);
            if (!ret)
                return;
            emit changeCameraStatus(tmpSlaveModel.box1.camera_ref.c_str(), Hardware_Off);
            emit changeCameraStatus(tmpSlaveModel.box1.camera.c_str(), Hardware_Off);
            emit changeCameraStatus(tmpSlaveModel.box2.camera_ref.c_str(), Hardware_Off);
            emit changeCameraStatus(tmpSlaveModel.box2.camera.c_str(), Hardware_Off);
            switch (hardwareid)
            {
                case 1:ui->slave1Label_net->setPixmap(QPixmap(img));break;
                case 2:ui->slave2Label_net->setPixmap(QPixmap(img));break;
                case 3:ui->slave3Label_net->setPixmap(QPixmap(img));break;
                case 4:ui->slave4Label_net->setPixmap(QPixmap(img));break;
                case 5:ui->slave5Label_net->setPixmap(QPixmap(img));break;
                case 6:ui->slave6Label_net->setPixmap(QPixmap(img));break;
                case 7:ui->slave7Label_net->setPixmap(QPixmap(img));break;
                case 8:ui->slave8Label_net->setPixmap(QPixmap(img));break;
                case 9:ui->slave9Label_net->setPixmap(QPixmap(img));break;
                default:;
            }
        }
        else if (status == Hardware_OnButFree)
        {
            QString img = ":/image/lightyellow.png";
            switch (hardwareid)
            {
                case 1:ui->slave1Label_net->setPixmap(QPixmap(img));break;
                case 2:ui->slave2Label_net->setPixmap(QPixmap(img));break;
                case 3:ui->slave3Label_net->setPixmap(QPixmap(img));break;
                case 4:ui->slave4Label_net->setPixmap(QPixmap(img));break;
                case 5:ui->slave5Label_net->setPixmap(QPixmap(img));break;
                case 6:ui->slave6Label_net->setPixmap(QPixmap(img));break;
                case 7:ui->slave7Label_net->setPixmap(QPixmap(img));break;
                case 8:ui->slave8Label_net->setPixmap(QPixmap(img));break;
                case 9:ui->slave9Label_net->setPixmap(QPixmap(img));break;
                default:;
            }
        }
        else if (status == Hardware_Working)
        {
            QString img = ":/image/lightgreen.png";
            switch (hardwareid)
            {
                case 1:ui->slave1Label_net->setPixmap(QPixmap(img));break;
                case 2:ui->slave2Label_net->setPixmap(QPixmap(img));break;
                case 3:ui->slave3Label_net->setPixmap(QPixmap(img));break;
                case 4:ui->slave4Label_net->setPixmap(QPixmap(img));break;
                case 5:ui->slave5Label_net->setPixmap(QPixmap(img));break;
                case 6:ui->slave6Label_net->setPixmap(QPixmap(img));break;
                case 7:ui->slave7Label_net->setPixmap(QPixmap(img));break;
                case 8:ui->slave8Label_net->setPixmap(QPixmap(img));break;
                case 9:ui->slave9Label_net->setPixmap(QPixmap(img));break;
                default:;
            }
        }
    }
    else if (type == Camera_Type)
    {
        emit changeCameraStatus(hardwareidstr, status);
    }
    else //if (type == LaserType)
    {
        emit changeLayserStatus(hardwareidstr, status);
    }
}

void MainWidget::changeCameraFC_slot(QString cameraindex, qint64 fc)
{
    emit changeCameraFC(cameraindex, fc);
}

void MainWidget::changeSlaveCameraTask_slot(WorkingStatus status, QString cameraindex, int threadid, QString task, QString remark)
{
    if (status == Collecting)
        emit changeSlaveCameraTask(status, cameraindex, threadid, task, "");
    else if (status == Calculating || status == Backuping)
        emit changeSlaveCameraTask(status, cameraindex, threadid, task, remark);
}

void MainWidget::changeSlaveCameraTaskFC_init_slot(WorkingStatus status, QString index, int threadid, QString file, long long startfc, long long endfc)
{
    if (status == Calculating || status == Backuping)
        emit changeSlaveCameraTaskFC_init(status, index, threadid, file, startfc, endfc);
}

void MainWidget::changeSlaveCameraTaskFC_slot(WorkingStatus status, QString cameraindex,  int threadid, QString task, long long currentfc)
{
    if (status == Calculating || status == Backuping)
        emit changeSlaveCameraTaskFC(status, cameraindex, threadid, task, currentfc);
}

/*void MainWidget::getCalcuBackupGUIActive(bool act)
{
    emit makeGUIActive(act);
}*/

void MainWidget::changecalcubackupProgressingBar_slot(WorkingStatus status, QString slaveidstr, int threadid, int totalnum, int donenum, bool isupdate)
{
    emit calcubackupProgressingBar(status, slaveidstr, threadid, totalnum, donenum, isupdate);
}

// 开始时关闭所有提示灯槽函数
void MainWidget::turnOffLights()
{
    QString img = ":/image/lightred.png";
    ui->slave1Label_net->setPixmap(QPixmap(img));
    ui->slave2Label_net->setPixmap(QPixmap(img));
    ui->slave3Label_net->setPixmap(QPixmap(img));
    ui->slave4Label_net->setPixmap(QPixmap(img));
    ui->slave5Label_net->setPixmap(QPixmap(img));
    ui->slave6Label_net->setPixmap(QPixmap(img));
    ui->slave7Label_net->setPixmap(QPixmap(img));
    ui->slave8Label_net->setPixmap(QPixmap(img));
    ui->slave9Label_net->setPixmap(QPixmap(img));
    emit turnOffAllHardware();
}

// 开启网络连接从控计算机提示提示灯
void MainWidget::initConnectedSlaves()
{
    bool ret;
    bool tmpconnected;
    for (int i = 1; i <= 9; i++)
    {
        tmpconnected = Status::getStatusInstance()->getSlaveWorkingStatus(Preparing, i, ret);
        if (ret)
        {
            if (tmpconnected)
                changeHardwareStatus(Slave_Computer_Type, QString("%1").arg(i), Hardware_OnButFree);
        }
    }
}

// 重启服务器槽函数
void MainWidget::restartServer()
{
    MasterProgram::getMasterProgramInstance()->restartServer();
}

// 关闭服务器
void MainWidget::closeServer()
{
    MasterProgram::getMasterProgramInstance()->closeServer();
}

// 界面传递参数-工程文件
void MainWidget::slotprojectfilename(QString newfilename)
{
    projectfilename = newfilename;

    emit turnOffAllHardware();

    // 初始时确认开始按钮不可点击
    ui->operationConfigButton->setEnabled(true);
    ui->operationConfigButton2->setEnabled(false);
    emit setCreateProjectWidgetEnabled(true);

    emit sendproject_file_name(newfilename);
}

/**
 * 初始化日志输出类
 */
bool MainWidget::initLogger(string filename, string username)
{
    if (logger != NULL)
    {
        if (logger->isLogging())
        {
            logger->log(string("关闭工程"));
            logger->close();
        }
        delete logger;
    }

    logger = new LzLogger(LzLogger::MasterOpt);
    logger->setFilename(filename);
    if (logger->open() == 0)
    {
        hasinitlog = true;
        logger->log(string("开启工程"));
        return true;
    }
    else
    {
        hasinitlog = false;
        return false;
    }
}

void MainWidget::configCurrentOperation()
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

        // 初始化日志类
        QString loggerfilename = LzProjectAccess::getLzProjectAccessInstance()->getProjectPath(projectclass) + "/" + linename + "_" + datetime11 + "_master_";
        loggerfilename += projectclassstr + ".log";
        qDebug() << "loggerfilename = " << loggerfilename;
        initLogger(loggerfilename.toLocal8Bit().constData(), currentusername.toLocal8Bit().constData());

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
                if (workingstatus == Collecting) // 采集
                {
                    ui->page->setVisible(true);
                    ui->page_2->setVisible(false);
                    ui->page_3->setVisible(false);

                    ui->collectstatuslabel->setPixmap(QString::fromUtf8(":image//collectstatus1.png"));
                    ui->backupstatuslabel->setPixmap(QString::fromUtf8(":image//up_alt_2.png"));
                    ui->calculatestatuslabel->setPixmap(QString::fromUtf8(":image//Calculatorstatus1_2.png"));
                    // 参数lineid暂时不用。设为-1
                    MasterProgram::getMasterProgramInstance()->workingMode_Setting(projectfilename, -1, linename, datetime11, Collecting, true);
                }
                else if (workingstatus == Calculating) // 计算
                {
                    ui->page->setVisible(false);
                    ui->page_2->setVisible(true);
                    ui->page_3->setVisible(false);

                    ui->collectstatuslabel->setPixmap(QString::fromUtf8(":image//collectstatus1_2.png"));
                    ui->backupstatuslabel->setPixmap(QString::fromUtf8(":image//up_alt_2.png"));
                    ui->calculatestatuslabel->setPixmap(QString::fromUtf8(":image//Calculatorstatus1.png"));
                    // 参数lineid暂时不用。设为-1
                    MasterProgram::getMasterProgramInstance()->workingMode_Setting(projectfilename, -1, linename, datetime11, Calculating, true);
                }
                else if (workingstatus == Backuping) // 备份
                {
                    ui->page->setVisible(false);
                    ui->page_2->setVisible(false);
                    ui->page_3->setVisible(true);

                    ui->collectstatuslabel->setPixmap(QString::fromUtf8(":image//collectstatus1_2.png"));
                    ui->backupstatuslabel->setPixmap(QString::fromUtf8(":image//up_alt.png"));
                    ui->calculatestatuslabel->setPixmap(QString::fromUtf8(":image//Calculatorstatus1_2.png"));
                    // 参数lineid暂时不用。设为-1
                    MasterProgram::getMasterProgramInstance()->workingMode_Setting(projectfilename, -1, linename, datetime11, Backuping, true);
                }

                ui->operationConfigButton2->setEnabled(true);

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

void MainWidget::configCurrentOperation2()
{
    if (projectfilename.compare("") != 0)
    {
        // 解析project filename 成为两段，linename和datetime
        QString linename = projectfilename.left(projectfilename.length() - 14);
        QString datetime11 = projectfilename.right(13).left(8);
        qDebug() << "linename=" << linename << ",datatime=" << datetime11;
        
        if (workingstatus == Collecting) // 采集
        {
            // 参数lineid暂时不用。设为-1
            MasterProgram::getMasterProgramInstance()->workingMode_Setting(projectfilename, -1, linename, datetime11, Collecting, false);
        }
        else if (workingstatus == Calculating) // 计算
        {
            // 参数lineid暂时不用。设为-1
            MasterProgram::getMasterProgramInstance()->workingMode_Setting(projectfilename, -1, linename, datetime11, Calculating, false);
        }
        else if (workingstatus == Backuping) // 备份
        {
            // 参数lineid暂时不用。设为-1
            MasterProgram::getMasterProgramInstance()->workingMode_Setting(projectfilename, -1, linename, datetime11, Backuping, false);
        }
        emit setCreateProjectWidgetEnabled(false);
        emit makeGUIActive(true);
        return;
    }
    else
    {
        emit setCreateProjectWidgetEnabled(true);
        emit makeGUIActive(false);
    }
}

// 桌面系统图标相关，防止误操作关闭，可后台运行
//! [2]
void MainWidget::closeEvent(QCloseEvent *event)
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
void MainWidget::setIcon()
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
void MainWidget::iconActivated(QSystemTrayIcon::ActivationReason reason)
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
void MainWidget::showMessage()
{
    QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon(0);
    trayIcon->showMessage("aaa", "bbb", icon, 1000);
}
//! [5]

//! [6]
void MainWidget::messageClicked()
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

void MainWidget::createActions()
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

void MainWidget::createTrayIcon()
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
