#include "form_mini.h"
#include "ui_form_mini.h"

#include <QTextCodec>
#include <QDebug>
#include <QMessageBox>
#include <QCloseEvent>
#include <QStyle>
#include <QFileDialog>

#include "setting_slave_mini.h"
#include "lz_working_enum.h"

using namespace std;

FormMini::FormMini(SlaveMiniProgram * cv, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormMini)
{
    ui->setupUi(this);

    this->cvice = cv;

    // 显示IP地址、从机号
    ui->ip->setText(SlaveMiniSetting::getSettingInstance()->getMySlaveIP());
    showSlaveID();
    connect(cv, SIGNAL(showSlaveID()), this, SLOT(showSlaveID()));

    /*******************************/
    QTextCodec::setCodecForTr(QTextCodec::codecForName("GBK")); //当QString变量中含有中文时，需要增加这两行代码
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
    connect(cv, SIGNAL(signalMsgToGUI(QString)), this, SLOT(appendMsg(QString)), Qt::DirectConnection);
    connect(cv, SIGNAL(signalErrorToGUI(QString)), this, SLOT(appendMsg(QString)), Qt::DirectConnection);
	
    // 桌面系统图标相关，防止误操作关闭，可后台运行
    createActions();
    createTrayIcon();

    setIcon();
    connect(trayIcon, SIGNAL(messageClicked()), this, SLOT(messageClicked()));
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

    trayIcon->show();

    connect(ui->sendFileButton, SIGNAL(clicked()), this, SLOT(sendFileButton_clicked()));

    connect(ui->terminateSlaveButton, SIGNAL(clicked()), this, SLOT(terminateSlave()));

    connect(ui->restartSlaveButton, SIGNAL(clicked()), this, SLOT(restartSlave()));

    connect(ui->shutdownButton, SIGNAL(clicked()), this, SLOT(shutdownSlave()));

}

FormMini::~FormMini()
{
    delete ui;
}

void FormMini::showSlaveID()
{
    int slaveid = SlaveMiniSetting::getSettingInstance()->getCurrentSlaveModel().getIndex();
    ui->slaveid->setText(QString("%1").arg(slaveid));
}

/*******************************/
void FormMini::appendMsg(QString msg)
{
    QString currentDate = QTime::currentTime().toString("[hh:mm:ss]");
    ui->statusArea->append(QString("%1%2").arg(currentDate).arg(msg));
    repaint();
}

void FormMini::appendError(QString msg)
{
    QString currentDate = QTime::currentTime().toString("[hh:mm:ss]");
    ui->statusArea->append(QString("%1%2").arg(currentDate).arg(msg));
    repaint();
}

// 桌面系统图标相关，防止误操作关闭，可后台运行
//! [2]
void FormMini::closeEvent(QCloseEvent *event)
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
void FormMini::setIcon()
{
    QIcon icon = QIcon(style()->standardIcon(QStyle::SP_MessageBoxInformation));
    trayIcon->setIcon(icon);
    setWindowIcon(icon);
    trayIcon->setToolTip(tr("铁路隧道建筑限界测量系统--从控控制小程序"));
    setWindowTitle(tr("铁路隧道建筑限界测量系统--从控控制小程序"));
}
//! [3]

//! [4]
void FormMini::iconActivated(QSystemTrayIcon::ActivationReason reason)
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
void FormMini::showMessage()
{
    QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon(0);
    trayIcon->showMessage("aaa", "bbb", icon, 1000);
}
//! [5]

//! [6]
void FormMini::messageClicked()
{
    QMessageBox::information(0, tr("提示"),
                                tr("Sorry, I already gave what help I could.\n"
                                   "Maybe you should try asking a human?"));
}
//! [6]

void FormMini::createActions()
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

void FormMini::createTrayIcon()
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

void FormMini::sendFileButton_clicked()
{
    QString openFileDir = SlaveMiniSetting::getSettingInstance()->getParentPath();
    QString filename = QFileDialog::getOpenFileName(this, tr("发送文件"), openFileDir, "(*.*)");
    cvice->send_file(filename);
}

void FormMini::terminateSlave()
{
    cvice->terminateSlave();
}

void FormMini::restartSlave()
{
    cvice->restartSlaveProgram();
}

void FormMini::shutdownSlave()
{
    cvice->shutdown();
}
