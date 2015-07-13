#include "form.h"
#include "ui_form.h"

#include <QTextCodec>
#include <QDebug>
#include <QMessageBox>
#include <QCloseEvent>
#include <QStyle>
#include <QFileDialog>

#include "setting_slave.h"
#include "lz_working_enum.h"

#include "MatToQImage.h"

using namespace std;

const int TN_WIDTH = 64;
const int TN_HEIGHT = 160;

Form::Form(SlaveProgram * cv, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);

    this->cvice = cv;

    // 显示IP地址、从机号
    ui->ip->setText(SlaveSetting::getSettingInstance()->getMySlaveIP());
    showSlaveID();
    connect(cv, SIGNAL(showSlaveID()), this, SLOT(showSlaveID()));

    ui->box1camref_img->setFixedSize(TN_WIDTH, TN_HEIGHT);
    ui->box1cam_img->setFixedSize(TN_WIDTH, TN_HEIGHT);
    ui->box2camref_img->setFixedSize(TN_WIDTH, TN_HEIGHT);
    ui->box2cam_img->setFixedSize(TN_WIDTH, TN_HEIGHT);

    /*******************************/
    QTextCodec::setCodecForTr(QTextCodec::codecForName("GBK")); //当QString变量中含有中文时，需要增加这两行代码
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
    connect(cv, SIGNAL(signalMsgToGUI(QString)), this, SLOT(appendMsg(QString)), Qt::DirectConnection);
    connect(cv, SIGNAL(signalErrorToGUI(QString)), this, SLOT(appendMsg(QString)), Qt::DirectConnection);
	
    // 采集状态 计算状态 备份状态
    connect(cv->getLzAcquizQueue(), SIGNAL(startCollect(QString, QString, QString, QString, QString, QString, QString, QString)), this, SLOT(startCollect(QString, QString, QString, QString, QString, QString, QString, QString)));
    connect(cv->getLzAcquizQueue(), SIGNAL(fcupdate(WorkingStatus, int, QString, long long)), this, SLOT(fcupdate(WorkingStatus, int, QString, long long)));
    connect(cv->getLzCalcQueue(), SIGNAL(fcupdate(WorkingStatus, int, QString, long long)), this, SLOT(fcupdate(WorkingStatus, int, QString, long long)));
    connect(cv->getLzBackupQueue(), SIGNAL(fcupdate(WorkingStatus, int, QString, long long)), this, SLOT(fcupdate(WorkingStatus, int, QString, long long)));

    connect(cv, SIGNAL(initGroupBox(WorkingStatus)), this, SLOT(initGroupBox(WorkingStatus)));

    // 桌面系统图标相关，防止误操作关闭，可后台运行
    createActions();
    createTrayIcon();

    setIcon();
    connect(trayIcon, SIGNAL(messageClicked()), this, SLOT(messageClicked()));
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

    trayIcon->show();

    turnoffalllight();

    // @author 范翔 20141031 隐藏不必要的按钮，以防乱按
    ui->groupBox->setVisible(false);
    ui->groupBox_3->setVisible(false);
    ui->pushButton_25->setVisible(false);
    ui->pushButton_26->setVisible(false);
}

Form::~Form()
{
    delete ui;
}

void Form::showSlaveID()
{
    int slaveid = SlaveSetting::getSettingInstance()->getCurrentSlaveModel().getIndex();
    ui->slaveid->setText(QString("%1").arg(slaveid));
}

void Form::turnoffalllight()
{
    // 初始灰色灯
    ui->box1camref_work->setPixmap(QPixmap(":/light/lightgrey.png"));
    ui->box1cam_work->setPixmap(QPixmap(":/light/lightgrey.png"));
    ui->box2camref_work->setPixmap(QPixmap(":/light/lightgrey.png"));
    ui->box2cam_work->setPixmap(QPixmap(":/light/lightgrey.png"));
}

void Form::changeCollectCameraLight(QString camid, HardwareStatus status)
{
    QString img;
    switch (status)
    {
        case Hardware_Off: img = ":/light/lightgrey.png";break;
        case Hardware_OnButFree: img = ":/light/lightyellow.png";break;
        case Hardware_ExceptionOff: img = ":/light/lightred.png";break;
        case Hardware_NotDetect: img = ":/light/lightred.png";break;
        case Hardware_Working: img = ":/light/lightgreen.png";break;
        case Hardware_Broken: img = ":/light/lightred.png";break;
        default: img = ":/light/lightred.png";break;
    }
    //qDebug() << cameraindex;
    QLabel * tmpcamwork;
    QString tmpcamid;
    for (int i = 0; i < 4; i++)
    {
        switch (i)
        {
            case 0: tmpcamid = ui->box1camref->text(); tmpcamwork = ui->box1camref_work; break;
            case 1: tmpcamid = ui->box1cam->text(); tmpcamwork = ui->box1cam_work; break;
            case 2: tmpcamid = ui->box2camref->text(); tmpcamwork = ui->box2camref_work; break;
            case 3: tmpcamid = ui->box2cam->text(); tmpcamwork = ui->box2cam_work; break;
        }
        if (tmpcamid.compare(camid) == 0) // 相机号对应
        {
            tmpcamwork->setPixmap(QPixmap(img));
            break;
        }
    }
}

void Form::showshow()
{
    this->repaint();
}

// 采集计算状态
void Form::startCollect(QString box1camref, QString box1cam, QString box2camref, QString box2cam, QString box1camref_sn, QString box1cam_sn, QString box2camref_sn, QString box2cam_sn)
{
    ui->box1camref->setText(box1camref);
    ui->box1cam->setText(box1cam);
    ui->box2camref->setText(box2camref);
    ui->box2cam->setText(box2cam);
    
    ui->box1camref_sn->setText(box1camref_sn);
    ui->box1cam_sn->setText(box1cam_sn);
    ui->box2camref_sn->setText(box2camref_sn);
    ui->box2cam_sn->setText(box2cam_sn);
}

void Form::fcupdate(WorkingStatus status, int threadid, QString camid, long long newfc)
{
    if (status == Collecting)
    {
        QLabel * tmpcamimg = NULL;
        QLabel * tmpcamfc = NULL;
        QString tmpcamid;
        QString tmpcamsn;

        unsigned __int64 tmpfc;
        cv::Mat tmpimgmat;

        for (int i = 0; i < 4; i++)
        {
            switch (i)
            {
                case 0: tmpcamid = ui->box1camref->text(); tmpcamsn = ui->box1camref_sn->text(); tmpcamimg = ui->box1camref_img; tmpcamfc = ui->box1camref_fc; break;
                case 1: tmpcamid = ui->box1cam->text(); tmpcamsn = ui->box1cam_sn->text(); tmpcamimg = ui->box1cam_img; tmpcamfc = ui->box1cam_fc; break;
                case 2: tmpcamid = ui->box2camref->text(); tmpcamsn = ui->box2camref_sn->text(); tmpcamimg = ui->box2camref_img; tmpcamfc = ui->box2camref_fc; break;
                case 3: tmpcamid = ui->box2cam->text(); tmpcamsn = ui->box2cam_sn->text(); tmpcamimg = ui->box2cam_img; tmpcamfc = ui->box2cam_fc; break;
            }
            if (tmpcamid.compare(camid) == 0) // 相机号对应
            {
                cvice->getLzAcquizQueue()->retriveImg(tmpcamsn, &tmpfc, tmpimgmat);
                
                // 帧显示
                // 缩放
                float scales = 0.3; //缩放比例
                cv::Size size = cv::Size(tmpimgmat.size().width * scales, tmpimgmat.size().height * scales);
                cv::resize(tmpimgmat, tmpimgmat, size);
                QImage qimg = MatToQImage(tmpimgmat);
                tmpcamimg->setPixmap(QPixmap::fromImage(qimg));

                tmpcamfc->setText(QString("%1").arg(tmpfc));

                // 强制更新界面绘图
                this->repaint();
            }
        }
    }
    else if (status == Calculating)
    {
        QLabel * tmpcamgroupid = NULL;
        QLabel * tmpcamfc = NULL;
        switch (threadid)
        {
            case 1: tmpcamgroupid = ui->calcuthread1task; tmpcamfc = ui->calcuthread1fn; break;
            case 2: tmpcamgroupid = ui->calcuthread2task; tmpcamfc = ui->calcuthread2fn; break;
            default: qDebug() << "not show thread id:" << threadid << camid;
        }
        tmpcamgroupid->setText(QString("%1").arg(camid));
        tmpcamfc->setText(QString("%1").arg(newfc));
    }
    else if (status == Backuping)
    {
        QLabel * tmpcamgroupid = NULL;
        QLabel * tmpcamfc = NULL;
        switch (threadid)
        {
            case 1: tmpcamgroupid = ui->backupthread1task; tmpcamfc = ui->backupthread1fn; break;
            default: qDebug() << "not show thread id:" << threadid << camid;
        }
        tmpcamgroupid->setText(QString("%1").arg(camid));
        tmpcamfc->setText(QString("%1").arg(newfc));
    }
}

void Form::initGroupBox(WorkingStatus status)
{
    if (status == Collecting)
    {
        ui->box1camref->setText("");
        ui->box1cam->setText("");
        ui->box2camref->setText("");
        ui->box2cam->setText("");
    
        ui->box1camref_sn->setText("");
        ui->box1cam_sn->setText("");
        ui->box2camref_sn->setText("");
        ui->box2cam_sn->setText("");

        ui->box1camref_task->setText("");
        ui->box1cam_task->setText("");
        ui->box2camref_task->setText("");
        ui->box2cam_task->setText("");

        ui->box1camref_img->setText("");
        ui->box1cam_img->setText("");
        ui->box2camref_img->setText("");
        ui->box2cam_img->setText("");
    }
    else if (status == Calculating)
    {
        ui->calcuthread1task->setText("");
        ui->calcuthread2task->setText("");
        ui->calcuthread1fn->setText("");
        ui->calcuthread2fn->setText("");
    }
}

void Form::on_pushButton_clicked()
{
    cvice->getLzAcquizQueue()->collect_CameraStatusFeedback("A1", 3, "Broken!!!");
}

/*******************************/
void Form::appendMsg(QString msg)
{
    QString currentDate = QTime::currentTime().toString("[hh:mm:ss]");
    ui->statusArea->append(QString("%1%2").arg(currentDate).arg(msg));
    repaint();
}

void Form::appendError(QString msg)
{
    QString currentDate = QTime::currentTime().toString("[hh:mm:ss]");
    ui->statusArea->append(QString("%1%2").arg(currentDate).arg(msg));
    repaint();
}

void Form::on_pushButton_2_clicked()
{
    QString cameraindex = "A1";
    int tunnelid = 207;
    QString tunnelname = tr("青石关");
    int seqno = 2;
    _int64 startFrameCounter = 12345;
    _int64 endFrameCounter = 23456;
    QString filenamePrfix = tr("青石关_20131124");
    cvice->getLzAcquizQueue()->collect_msgFinishOneTunnel(cameraindex, tunnelid, tunnelname, seqno, startFrameCounter, endFrameCounter, filenamePrfix);
}

void Form::on_pushButton_3_clicked()
{
    SlaveModel sm(1.0);
    sm.box1.box_camera_calib_file = "A_20140807.xml";
    sm.box2.box_camera_calib_file = "B_20140807.xml";
    cvice->init_CameraCalibrationFile("E:/" , sm);
}

void Form::on_pushButton_4_clicked()
{
    cvice->sendMsgToMaster("0140");
}

void Form::on_pushButton_5_clicked()
{
    cvice->init_TaskFile("task_20140109.xml");
}

void Form::on_pushButton_6_clicked()
{
    cvice->sendMsgToMaster("0110");
}

void Form::on_pushButton_7_clicked()
{
    cvice->init_NetworkCameraHardwareFile(LzProjectClass::Main, "network_config.xml");
}

void Form::on_pushButton_11_clicked()
{
    cvice->loadCorrectedTaskFile(LzProjectClass::Main, "real_task.xml");
}

void Form::on_pushButton_8_clicked()
{
    cvice->sendMsgToMaster("0120");
}

void Form::on_pushButton_9_clicked()
{
    SlaveModel sm(1.0);
    sm.box1.box_camera_calib_file = "A_20140807.xml";
    sm.box2.box_camera_calib_file = "B_20140807.xml";
    cvice->init_FenZhongFile("E:/" , sm);
}

void Form::on_pushButton_10_clicked()
{
    cvice->sendMsgToMaster("0130");
}

void Form::on_pushButton_12_clicked()
{
    cvice->sendMsgToMaster("0200");
}

void Form::on_pushButton_13_clicked()
{
    on_pushButton_33_clicked();
}

void Form::on_pushButton_14_clicked()
{
    QString filename = QString(QObject::tr("checked_兰新下行_20140115.xml"));
    int tunnelid = 207;
    QString cameraGroupIndex = "A";
    cvice->getLzCalcQueue()->calculate_feedbackToMaster(0, filename, tunnelid, cameraGroupIndex);
}

void Form::on_pushButton_15_clicked()
{
    QString filename = QString(QObject::tr("checked_兰新下行_20140115.xml"));
    int tunnelid = 207;
    QString cameraGroupIndex = "A";
    QString errortype= "errorrrrr!";
    cvice->getLzCalcQueue()->calculate_handleError(0, filename, tunnelid, cameraGroupIndex, errortype);
}

void Form::on_pushButton_16_clicked()
{
    int tunnelid = 207;
    QString cameraGroupIndex = "A";
    cvice->sendMsgToMaster(QString("2402,tunnelid=%1,camera_groupindex=%2,restart=false").arg(tunnelid).arg(cameraGroupIndex));
}

void Form::on_pushButton_17_clicked()
{
    int tunnelid = 207;
    QString cameraGroupIndex = "A";
    _int64 restartFrameCounter = 123456;
    cvice->sendMsgToMaster(QString("2402,tunnelid=%1,camera_groupindex=%2,restart=true,restartframe=%3").arg(tunnelid).arg(cameraGroupIndex).arg(restartFrameCounter));
}

void Form::on_pushButton_18_clicked()
{
    cvice->getLzCalcQueue()->calculate_finishAll();
}

void Form::on_pushButton_19_clicked()
{
    QString filename = QString(QObject::tr("checked_兰新下行_20140115.xml"));
    int seqno = 1;
    cvice->getLzBackupQueue()->backup_start(filename, -1, false, 0);
}

void Form::on_pushButton_20_clicked()
{
    QString filename = QString(QObject::tr("checked_兰新下行_20140115.xml"));
    int seqno = 1;
    QString filetype="raw";
    QString cameraindex = "A1";
    cvice->getLzBackupQueue()->backup_feedbackEndToMaster(filename, -1);
}

void Form::on_pushButton_21_clicked()
{
	QString filename = QString(QObject::tr("checked_兰新下行_20140115.xml"));
    int seqno = 1;
    QString filetype = "raw";
    QString cameraindex = "A2";
    QString errortype= "errorrrrr!";
    cvice->getLzBackupQueue()->backup_handleError(filename, -1, errortype);
}

void Form::on_pushButton_22_clicked()
{
    int seqno = 1;
    QString filetype="raw";
    QString cameraindex = "A1";
    cvice->sendMsgToMaster(QString("3402,seqno=%1,filetype=%2,index=%3,restart=false").arg(seqno).arg(filetype).arg(cameraindex));
}

void Form::on_pushButton_23_clicked()
{
    int seqno = 1;
    QString filetype="raw";
    QString cameraindex = "A1";
    _int64 restartFrame = 1234567;
    cvice->sendMsgToMaster(QString("3402,seqno=%1,filetype=%2,index=%3,restart=true,restart_frame=%4").arg(seqno).arg(filetype).arg(cameraindex).arg(restartFrame));
}

void Form::on_pushButton_24_clicked()
{
    QString filename = QString(QObject::tr("checked_兰新下行_20140115.xml"));
    cvice->getLzBackupQueue()->backup_finishAll();
}

// 发送中间计算结果图像文件
void Form::on_pushButton_25_clicked()
{    
    QString filename = tr("D:/范翔.7z");

    //QString filename = "F/file.7z";
    cvice->send_file(filename);
}

// 发送原始图像文件
void Form::on_pushButton_26_clicked()
{
    QString realname = "real_xx_20140228.xml";
    QString cameraindex = "A1";
    int tunnelid=246;
	int seqno=1;
    _int64 startFrameCounter = 20;
    int frameNum = 50;
    int lineid = 54;
    string datetime_string = "20140109";
    QString datetime = QString::fromLocal8Bit(datetime_string.c_str());
    cvice->getLzCorrectQueue()->send_RARFile(realname,seqno, cameraindex, startFrameCounter, frameNum, lineid, datetime, tunnelid);
}

void Form::on_pushButton_27_clicked()
{
    QString linename = tr("侯月下行");
    QString date = "20140115";
    lastTime = QDateTime::currentDateTime();
    cvice->checkedok(WorkingStatus::Collecting, QString("侯月下行_20140115.proj"), linename, date);
    m_time = lastTime.msecsTo(QDateTime::currentDateTime());
    qDebug() << QString("计划线路配置完成%1").arg(m_time);
    appendMsg(QString("计划线路配置完成%1").arg(m_time));
}


void Form::on_pushButton_32_clicked()
{
    lastTime = QDateTime::currentDateTime();
    bool ret = cvice->getLzAcquizQueue()->collect_start_line();
    m_time = lastTime.msecsTo(QDateTime::currentDateTime());
    if (ret)
    {
        qDebug() << QString("相机配置完成并开启%1").arg(m_time);
        appendMsg(QString("相机配置及完成并开启%1").arg(m_time));
    }
    else
    {
        qDebug() << QString("相机配置失败%1").arg(m_time);
        appendMsg(QString("相机配置失败%1").arg(m_time));
    }
}

void Form::on_pushButton_28_clicked()
{
    cvice->getLzAcquizQueue()->collect_start_tunnel();
    qDebug() << QString("开始采集下一条隧道");
    appendMsg(QString("开始采集下一条隧道"));
}

void Form::on_pushButton_29_clicked()
{
    cvice->getLzAcquizQueue()->collect_end_tunnel(false);
    qDebug() << QString("结束采集当前隧道");
    appendMsg(QString("结束采集当前隧道"));
}

void Form::on_pushButton_30_clicked()
{
    cvice->getLzAcquizQueue()->collect_end_line();
    qDebug() << QString("结束采集当前线路,关闭相机");
    appendMsg(QString("结束采集当前线路,关闭相机"));
}

void Form::on_pushButton_31_clicked()
{
    QString linename = tr("侯月下行");
    QString date = "20140115";
    lastTime = QDateTime::currentDateTime();
    cvice->checkedok(WorkingStatus::Calculating_Backuping, QString("侯月下行_20140115.proj"), linename, date);
    m_time = QDateTime::currentDateTime().msecsTo(lastTime);;
    qDebug() << QString("计算备份初始化线路配置完成%1").arg(m_time);
    appendMsg(QString("计算备份初始化线路线路配置完成%1").arg(m_time));
}

void Form::on_pushButton_33_clicked()
{
    QString filename = QString(QObject::tr("checked_兰新下行_20140115.xml"));
    int tunnelid = 216;
    cvice->getLzCalcQueue()->calculate_start(filename, tunnelid, "A", false, 0);
    qDebug() << QString("开始计算");
    appendMsg(QString("开始计算"));
}

void Form::on_pushButton_34_clicked()
{
    QString openFileDir = SlaveSetting::getSettingInstance()->getParentPath();
    QString filename = QFileDialog::getOpenFileName(this, tr("发送文件"), openFileDir, "(*.*)");
    cvice->send_file(filename);
}

void Form::on_pushButton_35_clicked()
{
    QString openFileDir = SlaveSetting::getSettingInstance()->getParentPath();
    QString filename = QFileDialog::getOpenFileName(this, tr("发送文件"), openFileDir, "(*.*)");
    QString file = QFileInfo(filename).fileName();
    bool ret = QFile::copy(filename, "\\\\10.13.29.222\\LanZhou\\" + file);
    qDebug() << ret;
}

// 桌面系统图标相关，防止误操作关闭，可后台运行
//! [2]
void Form::closeEvent(QCloseEvent *event)
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
void Form::setIcon()
{
    QIcon icon = QIcon(style()->standardIcon(QStyle::SP_MessageBoxInformation));
    trayIcon->setIcon(icon);
    setWindowIcon(icon);
    trayIcon->setToolTip(tr("铁路隧道建筑限界测量系统--从控机程序"));
    setWindowTitle(tr("铁路隧道建筑限界测量系统--从控机程序"));
}
//! [3]

//! [4]
void Form::iconActivated(QSystemTrayIcon::ActivationReason reason)
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
void Form::showMessage()
{
    QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon(0);
    trayIcon->showMessage("aaa", "bbb", icon, 1000);
}
//! [5]

//! [6]
void Form::messageClicked()
{
    QMessageBox::information(0, tr("提示"),
                                tr("Sorry, I already gave what help I could.\n"
                                   "Maybe you should try asking a human?"));
}
//! [6]

void Form::createActions()
{
    minimizeAction = new QAction(tr("最小化"), this);
    connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));

    maximizeAction = new QAction(tr("最大化"), this);
    connect(maximizeAction, SIGNAL(triggered()), this, SLOT(showMaximized()));

    restoreAction = new QAction(tr("显示"), this);
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

    quitAction = new QAction(tr("退出"), this);
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
}

void Form::createTrayIcon()
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

