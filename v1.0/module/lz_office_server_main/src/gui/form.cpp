#include "form.h"
#include "ui_form.h"

#include <QTextCodec>
#include <QMessageBox>
#include <QCloseEvent>
#include <QStyle>
#include <QFileDialog>
#include "serverprogram.h"
#include "editing_list.h"

Form::Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);

    copywidget = NULL;

    /*******************************/

    // 【注意】顺序：先有masterProgramInstance，才能连接信号槽
    ServerProgram::getServerProgramInstance();

    connect(ServerProgram::getServerProgramInstance(),
            SIGNAL(signalMsgToGUI(QString)), this, SLOT(appendMsg(QString)));
    connect(ServerProgram::getServerProgramInstance(),
            SIGNAL(signalErrorToGUI(QString)), this, SLOT(appendError(QString)));

    // connect函数必在ServerProgram::getServerProgramInstance()之后

    ServerProgram::getServerProgramInstance()->init();

    /*******************************/

    connect(ServerProgram::getServerProgramInstance(), SIGNAL(updateEditingProjectListSignal()), this, SLOT(updateEditingProjectListView()));
    connect(ServerProgram::getServerProgramInstance(), SIGNAL(updateIPListSignal(QString, bool)), this, SLOT(updateIPListView(QString, bool)));

    // 桌面系统图标相关，防止误操作关闭，可后台运行
    createActions();
    createTrayIcon();

    setIcon();
    connect(trayIcon, SIGNAL(messageClicked()), this, SLOT(messageClicked()));
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

    trayIcon->show();

    // 默认发送的clientip地址
    toclientip = "10.13.29.34";

    iplistmodel = new QStringListModel(this);
    ui->iplistView->setModel(iplistmodel);

    editinglistmodel = new QStringListModel(this);
    ui->editingProjectListView->setModel(editinglistmodel);

    // 测试按钮不可见
    ui->pushButton->setVisible(false);
    ui->pushButton_2->setVisible(false);
    ui->pushButton_3->setVisible(false);
    ui->pushButton_4->setVisible(false);
    ui->pushButton_5->setVisible(false);
    ui->pushButton_6->setVisible(false);
    ui->pushButton_7->setVisible(false);

    connect(ui->saveProjDataButton, SIGNAL(clicked()), this, SLOT(saveProjData()));
    connect(ui->resetSlaveProgramButton, SIGNAL(clicked()), this, SLOT(resetSlaveProgram()));
    connect(ui->terminateSlaveProgramButton, SIGNAL(clicked()), this, SLOT(terminateSlaveProgram()));
    connect(ui->shutdownAllButton, SIGNAL(clicked()), this, SLOT(shotdownALlSlaves()));
    connect(ui->updateSlaveExeFileButton, SIGNAL(clicked()), this, SLOT(updateSlaveExeFile()));
}

Form::~Form()
{
    delete ui;
    delete iplistmodel;
    delete editinglistmodel;

    if (copywidget != NULL)
        delete copywidget;
}

/**
 * 更新当前连接IP列表
 * @param ip ip地址
 * @param addorremove 增加到iplist还是从iplist中删除，true为增加，false为删除
 */
void Form::updateIPListView(QString ip, bool addorremove)
{
    if (addorremove)
    {
        for (int i = 0; i < iplist.length(); i++)
            if (iplist.at(i).compare(ip) == 0)
                return;
        iplist << ip;
    }
    else
    {
        for (int i = 0; i < iplist.length(); i++)
            if (iplist.at(i).compare(ip) == 0)
            {
                iplist.removeAt(i);
                break;
            }
    }
    iplistmodel->setStringList(iplist);
}

/**
 * 更新正在编辑工程列表
 */
void Form::updateEditingProjectListView()
{
    QStringList strings;
    for (int i = 0; i < EditingList::getEditingListInstance()->list()->size(); i++)
    {
        EditingItem tmp = EditingList::getEditingListInstance()->list()->at(i);
        QString tmpstr = QObject::tr("%1_%2_%3").arg(tmp.projectname).arg(tmp.tunnelname).arg(tmp.userid);
        strings << tmpstr;
    }
    editinglistmodel->setStringList(strings);
}

void Form::on_pushButton_clicked()
{
    EditingItem item;
    item.projectname = QString(tr("兰青_20140122"));
    item.tunnelid = -1;
    item.tunnelname = QString(tr("实验室隧道"));
    item.collectdate = "20140122";
    item.userid = "fanxiang";

    ServerProgram::getServerProgramInstance()->checkIfModiiedByOther(toclientip, item);
}

/*******************************/
void Form::appendMsg(QString msg)
{
    QString currentDate = QTime::currentTime().toString("[hh:mm:ss]");
    ui->statusArea->append(QString("%1%2").arg(currentDate).arg(msg));
}

void Form::appendError(QString msg)
{
    QString currentDate = QTime::currentTime().toString("[hh:mm:ss]");
    ui->statusArea->append(QString("%1%2").arg(currentDate).arg(msg));
}

void Form::on_pushButton_2_clicked()
{
    ServerProgram::getServerProgramInstance()->receiveFileFeedback(toclientip, QString(tr("兰青_20140122")), -1,  QString(tr("实验室隧道")),"20140122");
}

void Form::on_pushButton_3_clicked()
{
    ServerProgram::getServerProgramInstance()->returnDirectoryFiles(toclientip, QString("123456.ppt"));
}

void Form::on_pushButton_4_clicked()
{
    ServerProgram::getServerProgramInstance()->sendCalcuResultData(toclientip, QString(tr("多视图第一次作业_贺尧_2012201757.zip")));
}

void Form::on_pushButton_5_clicked()
{
    ServerProgram::getServerProgramInstance()->sendRawImagesPackage(toclientip, QString(tr("多视图第一次作业_贺尧_2012201757.zip")));
}

void Form::on_pushButton_6_clicked()
{
    // 未选中任何行
    if (!ui->iplistView->currentIndex().isValid())
    {
        QMessageBox::warning(this, tr("提示"), tr("请在IP地址处先选择一栏"));
        return;
    }

    QString openFileDir = ServerSetting::getSettingInstance()->getParentPath();
    QString filename = QFileDialog::getOpenFileName(this, tr("发送文件"), openFileDir, "(*.*)");

    ServerProgram::getServerProgramInstance()->getMultiThreadTcpServer()->sendFileToOneSlave(toclientip, filename);
}

void Form::on_pushButton_7_clicked()
{
    // @author范翔 测试连续多发
    for (int i = 0; i < 1000; i++)
    {
        //qDebug() << i;
        for (int j = 0; j < iplist.length(); j++)
            ServerProgram::getServerProgramInstance()->receiveFileFeedback(iplist.at(j), QString(tr("兰青_20140122")), -1,  QString(tr("实验室隧道%1").arg(i)),"20140122");
    }
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
    trayIcon->setToolTip(tr("铁路隧道建筑限界测量系统--办公室服务器程序"));
    setWindowTitle(tr("铁路隧道建筑限界测量系统--办公室服务器程序"));
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

void Form::on_iplistView_clicked(const QModelIndex &index)
{
    toclientip = index.data().toString();
}

/**
 * 存储工程数据
 */
void Form::saveProjData()
{
    if (copywidget != NULL)
        delete copywidget;

    copywidget = new CopyAvaliableTunnelWidget();
    copywidget->show();
}

/**
 * 重启从控程序
 */
void Form::resetSlaveProgram()
{
    int result = QMessageBox::warning(this,tr("警告"), tr("【请慎重！！】重启所有从控程序，是否继续？"),
        QMessageBox::Yes | QMessageBox::No);
    if (result == QMessageBox::No)
        return;
    if (result == QMessageBox::Yes)
        ServerProgram::getServerProgramInstance()->resetSlaveProgram();
}

void Form::terminateSlaveProgram()
{
    int result = QMessageBox::warning(this,tr("警告"), tr("【请慎重！！】关闭所有从控程序，确认所有从控的当前任务确定已经状态都已保存，是否继续？"),
        QMessageBox::Yes | QMessageBox::No);
    if (result == QMessageBox::No)
        return;
    if (result == QMessageBox::Yes)
        ServerProgram::getServerProgramInstance()->terminateSlaveProgram();
}

void Form::shotdownALlSlaves()
{
    int result = QMessageBox::warning(this,tr("警告"), tr("【请慎重！！】关闭全部从控机，确认所有从控的当前任务确定已经状态都已保存，是否继续？"),
        QMessageBox::Yes | QMessageBox::No);
    if (result == QMessageBox::No)
        return;
    if (result == QMessageBox::Yes)
        ServerProgram::getServerProgramInstance()->shutdownAllSlaves();
}

/**
 * 从控程序更新
 */
void Form::updateSlaveExeFile()
{
    int ret = QMessageBox::warning(this, tr("提示"), tr("【请慎重！！】更新从控的lz_slave.exe Release版程序之前，请确认准备好Release版的lz_slave.exe程序，且目前所有从控程序都已关闭，确认则继续？"), QMessageBox::Yes | QMessageBox::No);
    if (ret == QMessageBox::No)
        return;

    if (iplistmodel->rowCount() < 9)
    {
        int result = QMessageBox::warning(this, tr("提示"), tr("当前链接从控不够九台，是否继续？"), QMessageBox::Yes | QMessageBox::No);
        if (result == QMessageBox::No)
            return;
    }

    QString openFileDir = ServerSetting::getSettingInstance()->getParentPath();
    QString filename = QFileDialog::getOpenFileName(this, tr("发送兰州从控lz_slave.exe程序文件，注意发送Release版"), openFileDir, "(*.exe)");
    if (!filename.endsWith("lz_slave.exe"))
    {
        QMessageBox::warning(this, tr("提示"), tr("不是兰州从控程序，文件名应该是“lz_slave.exe”，请重新选择文件"));
        return;
    }

    for (int j = 0; j < iplist.length(); j++)
        ServerProgram::getServerProgramInstance()->getMultiThreadTcpServer()->sendFileToOneSlave(iplist.at(j), filename);
}