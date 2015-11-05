#include "check_task.h"
#include "ui_check_task.h"

#include <QDebug>
#include <QFileDialog>
#include <QSqlRelationalDelegate>
#include <QMessageBox>
#include <QPalette>
#include <QTime>
#include <QDateTime>
#include <QTableWidgetItem>
#include <QTextCodec>

#include "daotask.h"
#include "daotasktunnel.h"
#include "imageviewer_master.h"

#include "create_plan_task.h"
#include "file_decompress.h"
#include "setting_master.h"
#include "masterprogram.h"

/**
 * 主控机界面-计划任务与实际采集任务校对界面类实现
 * @author 熊雪
 * @author 范翔
 * @date 2014.1
 * @version 1.0.0
 */
CheckTaskWidget::CheckTaskWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CheckTaskWidget), currentProjectModel(1.0)
{
    ui->setupUi(this);
    QTextCodec::setCodecForTr(QTextCodec::codecForLocale());
    //imageopen =NULL;
    finalimage =NULL;
    openvalidframes=NULL;

    ischeckedfile = true;

    // 设置目录
    path = ".";

    loadConfigOpenCloseDir();

    connect(ui->openFileButton, SIGNAL(clicked()), this, SLOT(openPlanTaskFile()));

    /* 对实际隧道任务的修正操作，前移后移，删除 */
    connect(ui->deleteButton, SIGNAL(clicked()), this, SLOT(removeTaskTunnel()));
    connect(ui->combineButton, SIGNAL(clicked()), this, SLOT(combineTaskTunnel()));
    connect(ui->separate_pushButton, SIGNAL(clicked()), this, SLOT(separateTaskTunnel()));

    /* 重置帧数按钮 */
    connect(ui->resetToRealTaskButton, SIGNAL(clicked()), this, SLOT(resetCheckedFile()));

    // 加载两个表格表头等内容
    plantask_model = new QStandardItemModel();
    // 这样调用的话是初始化界面时，即在界面上显示具体的值，【注意】要在plantask-model new之后
    loadPlanTaskTunnelData();//添加计划隧道文件 在界面上默认显示某个计划task
    loadCheckedTaskTunnelData();//添加实际隧道文件

    // 设置分离删除合并按钮是否可点击
    setOptButtonEnable();

    // 日志类
    logger = NULL;
    hasinitlog = false;
}

CheckTaskWidget::~CheckTaskWidget()
{
    delete ui;

    if (logger != NULL)
    {
        if (logger->isLogging())
        {
            logger->log(string("关闭工程"));
            logger->close();
        }
        delete logger;
    }

    if (finalimage != NULL)
        delete finalimage;
    if(openvalidframes != NULL)
        delete openvalidframes;
    if (plantask_model != NULL)
        delete plantask_model;
}

/** 
 * 日志类初始化
 */
bool CheckTaskWidget::initLogger(string filename, string username)
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

void CheckTaskWidget::log(QString msg)
{
    if (hasinitlog)
        logger->log(msg.toLocal8Bit().constData());
}

//打开图像浏览界面
void CheckTaskWidget::openimageviewer()
{
    // 下面这几行代码可以在不选中行的情况下点击原图按钮获得正确的当前行，而不是-1
    QPushButton *pbt = dynamic_cast<QPushButton*>(this->sender());
    if(pbt == 0)
    return;
    int x = pbt->frameGeometry().x();
    int y = pbt->frameGeometry().y();
    QModelIndex index = ui->actualTasksWidget->indexAt(QPoint(x,y));
    int i = index.row();//按钮所在的当前行
    if (i == -1)
    {
        return;
    }

    /*在不选中行的情况下，直接点击原图浏览按钮，当前行和列返回的都是-1，导致程序崩溃了，所以下面两行代码偶漏洞*/
    QString startframe = ui->actualTasksWidget->item(i, CHECKEDTASK_STARTFRAME)->text();
    long long startframe1 = startframe.toLongLong();
    QString seqnototal = ui->actualTasksWidget->item(i, CHECKEDTASK_SEQNO_TOTAL)->text();
    QStringList strlist = seqnototal.split("-", QString::SkipEmptyParts);
    int seqnostart = strlist.at(0).toInt();
    int tunnelid = ui->actualTasksWidget->item(i, CHECKEDTASK_TUNNELID)->text().toInt();
    QString endframe = ui->actualTasksWidget->item(i, CHECKEDTASK_ENDFRAME)->text();
    QString tunnelname = ui->actualTasksWidget->item(i, CHECKEDTASK_TUNNELNAME)->text();
    qDebug() << "tunnelid:" << tunnelid;
    QString projectpath = LzProjectAccess::getLzProjectAccessInstance()->getProjectPath(LzProjectClass::Main);

    if (finalimage != NULL)
    {
        delete finalimage;
    }
    finalimage = new MasterImageViewer();
    finalimage->setInfo(projectpath, projectpath, tunnelid, seqnostart, tunnelname);
    finalimage->show();
}

void CheckTaskWidget::opensetvalidframes()
{
    QPushButton *pbt = dynamic_cast<QPushButton*>(this->sender());
    if(pbt == 0)
    return;
    int x = pbt->frameGeometry().x();
    int y = pbt->frameGeometry().y();
    QModelIndex index = ui->actualTasksWidget->indexAt(QPoint(x,y));
    int i = index.row();//按钮所在的当前行
    if(i == -1)
    {
        return;
    }
    this->valid_row = i;
    if(openvalidframes != NULL)
    {
        delete openvalidframes;
    }
    openvalidframes = new SetValidFrames();
    openvalidframes->show();
    connect(openvalidframes, SIGNAL(sendframestocheck_task(long long ,long long)), this, SLOT(getframesfromsetvalidframes(long long ,long long)));
    connect(openvalidframes, SIGNAL(sendframestocheck_task_createtmppro(long long ,long long)), this, SLOT(getframesfromsetvalidframes_createtmppro(long long ,long long)));
}

void CheckTaskWidget::getframesfromsetvalidframes(long long  newstartframes, long long newendframes)
{
    // 得到数据要记录到日志中
    int tunnelid = ui->actualTasksWidget->item(valid_row, CHECKEDTASK_TUNNELID)->data(Qt::DisplayRole).toInt();
    QString tunnelname = ui->actualTasksWidget->item(valid_row, CHECKEDTASK_TUNNELNAME)->data(Qt::DisplayRole).toString();
    QString seqnototal = ui->actualTasksWidget->item(valid_row, CHECKEDTASK_SEQNO_TOTAL)->data(Qt::DisplayRole).toString();
    int startseqno = 1;
    startseqno = QStringList(seqnototal.split("-")).at(0).toInt();
    // 从实际隧道文件的的xml中解析实际隧道数据和校正之后的数据到listcollect中
    QString checkedFile = path + "/" + currentProjectModel.getCheckedFilename();
    qDebug() << "checked filename: " << checkedFile << ", tunnelid:" << tunnelid << ", tunnelname:" << tunnelname << ",startseqno:" << startseqno;

    // 引用赋值
    CheckedTaskList & clist = LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Main);
    bool findmodel = false;
    CheckedTunnelTaskModel & task = clist.getCheckedTunnelModel(tunnelid, startseqno, findmodel);
    if (findmodel)
    {
        std::list<CheckedItem>::iterator it = task.begin();
        __int64 tmpdeltanum;
        while (it != task.end())
        {
            if (newstartframes >= (*it).start_frame_master && newstartframes <= (*it).end_frame_master)
            {
                tmpdeltanum = newstartframes - it->start_frame_master;
                it->start_mile = it->start_mile + (it->end_mile - it->start_mile) * (tmpdeltanum + 1) / (it->end_frame_master - it->start_frame_master + 1) ;
                // 主控有效起始帧
                it->start_frame_master = newstartframes;
                it->start_frame_A1 = it->start_frame_A1 + tmpdeltanum;  // 从num开始
                it->start_frame_A2 = it->start_frame_A2 + tmpdeltanum;
                it->start_frame_B1 = it->start_frame_B1 + tmpdeltanum;
                it->start_frame_B2 = it->start_frame_B2 + tmpdeltanum;
                it->start_frame_C1 = it->start_frame_C1 + tmpdeltanum;
                it->start_frame_C2 = it->start_frame_C2 + tmpdeltanum;
                it->start_frame_D1 = it->start_frame_D1 + tmpdeltanum;
                it->start_frame_D2 = it->start_frame_D2 + tmpdeltanum;
                it->start_frame_E1 = it->start_frame_E1 + tmpdeltanum;
                it->start_frame_E2 = it->start_frame_E2 + tmpdeltanum;
                it->start_frame_F1 = it->start_frame_F1 + tmpdeltanum;
                it->start_frame_F2 = it->start_frame_F2 + tmpdeltanum;
                it->start_frame_G1 = it->start_frame_G1 + tmpdeltanum;
                it->start_frame_G2 = it->start_frame_G2 + tmpdeltanum;
                it->start_frame_H1 = it->start_frame_H1 + tmpdeltanum;
                it->start_frame_H2 = it->start_frame_H2 + tmpdeltanum;
                it->start_frame_I1 = it->start_frame_I1 + tmpdeltanum;
                it->start_frame_I2 = it->start_frame_I2 + tmpdeltanum;
                it->start_frame_J1 = it->start_frame_J1 + tmpdeltanum;
                it->start_frame_J2 = it->start_frame_J2 + tmpdeltanum;
                it->start_frame_K1 = it->start_frame_K1 + tmpdeltanum;
                it->start_frame_K2 = it->start_frame_K2 + tmpdeltanum;
                it->start_frame_L1 = it->start_frame_L1 + tmpdeltanum;
                it->start_frame_L2 = it->start_frame_L2 + tmpdeltanum;
                it->start_frame_M1 = it->start_frame_M1 + tmpdeltanum;
                it->start_frame_M2 = it->start_frame_M2 + tmpdeltanum;
                it->start_frame_N1 = it->start_frame_N1 + tmpdeltanum;
                it->start_frame_N2 = it->start_frame_N2 + tmpdeltanum;
                it->start_frame_O1 = it->start_frame_O1 + tmpdeltanum;
                it->start_frame_O2 = it->start_frame_O2 + tmpdeltanum;
                it->start_frame_P1 = it->start_frame_P1 + tmpdeltanum;
                it->start_frame_P2 = it->start_frame_P2 + tmpdeltanum;
                it->start_frame_Q1 = it->start_frame_Q1 + tmpdeltanum;
                it->start_frame_Q2 = it->start_frame_Q2 + tmpdeltanum;
                it->start_frame_R1 = it->start_frame_R1 + tmpdeltanum;
                it->start_frame_R2 = it->start_frame_R2 + tmpdeltanum;

            }
            if (newendframes >= (*it).start_frame_master && newendframes <= (*it).end_frame_master)
            {
                // 主控有效终止帧
                (*it).end_frame_master = newendframes;
                tmpdeltanum = it->end_frame_master  - newendframes;
                float mileperframe = 1; // TODO

                it->end_mile = it->end_mile - (it->end_mile - it->start_mile) * (tmpdeltanum + 1) / (it->end_frame_master - it->start_frame_master + 1) - mileperframe;
                it->end_frame_master = it->end_frame_master - tmpdeltanum;
                it->end_frame_A1 = it->end_frame_A1 - tmpdeltanum; // 截止到 TODO 
                it->end_frame_A2 = it->end_frame_A2 - tmpdeltanum;
                it->end_frame_B1 = it->end_frame_B1 - tmpdeltanum;
                it->end_frame_B2 = it->end_frame_B2 - tmpdeltanum;
                it->end_frame_C1 = it->end_frame_C1 - tmpdeltanum;
                it->end_frame_C2 = it->end_frame_C2 - tmpdeltanum;
                it->end_frame_D1 = it->end_frame_D1 - tmpdeltanum;
                it->end_frame_D2 = it->end_frame_D2 - tmpdeltanum;
                it->end_frame_E1 = it->end_frame_E1 - tmpdeltanum;
                it->end_frame_E2 = it->end_frame_E2 - tmpdeltanum;
                it->end_frame_F1 = it->end_frame_F1 - tmpdeltanum;
                it->end_frame_F2 = it->end_frame_F2 - tmpdeltanum;
                it->end_frame_G1 = it->end_frame_G1 - tmpdeltanum;
                it->end_frame_G2 = it->end_frame_G2 - tmpdeltanum;
                it->end_frame_H1 = it->end_frame_H1 - tmpdeltanum;
                it->end_frame_H2 = it->end_frame_H2 - tmpdeltanum;
                it->end_frame_I1 = it->end_frame_I1 - tmpdeltanum;
                it->end_frame_I2 = it->end_frame_I2 - tmpdeltanum;
                it->end_frame_J1 = it->end_frame_J1 - tmpdeltanum;
                it->end_frame_J2 = it->end_frame_J2 - tmpdeltanum;
                it->end_frame_K1 = it->end_frame_K1 - tmpdeltanum;
                it->end_frame_K2 = it->end_frame_K2 - tmpdeltanum;
                it->end_frame_L1 = it->end_frame_L1 - tmpdeltanum;
                it->end_frame_L2 = it->end_frame_L2 - tmpdeltanum;
                it->end_frame_M1 = it->end_frame_M1 - tmpdeltanum;
                it->end_frame_M2 = it->end_frame_M2 - tmpdeltanum;
                it->end_frame_N1 = it->end_frame_N1 - tmpdeltanum;
                it->end_frame_N2 = it->end_frame_N2 - tmpdeltanum;
                it->end_frame_O1 = it->end_frame_O1 - tmpdeltanum;
                it->end_frame_O2 = it->end_frame_O2 - tmpdeltanum;
                it->end_frame_P1 = it->end_frame_P1 - tmpdeltanum;
                it->end_frame_P2 = it->end_frame_P2 - tmpdeltanum;
                it->end_frame_Q1 = it->end_frame_Q1 - tmpdeltanum;
                it->end_frame_Q2 = it->end_frame_Q2 - tmpdeltanum;
                it->end_frame_R1 = it->end_frame_R1 - tmpdeltanum;
                it->end_frame_R2 = it->end_frame_R2 - tmpdeltanum;
            }
            it++;
        }
        task.calcuItem.cal_framecounter_begin = newstartframes;
        task.calcuItem.cal_framecounter_end = newendframes;
        task.calcuItem.cal_valid_frames_num = newendframes - newstartframes + 1;
        // 存入配置文件
        XMLCheckedTaskFileLoader * ff = new XMLCheckedTaskFileLoader(checkedFile);
        bool ret = ff->saveFile(clist);
        delete ff;
        if (ret == false)
            qDebug() << "save(update) xml file fail";
        else
        {
            // 记录到日志中
            this->log(QObject::tr("设置有效帧范围：实际采集的第%1条隧道，采集隧道名为%2，有效帧范围为%3-%4").arg(seqnototal).arg(tunnelname).arg(newstartframes).arg(newendframes));
            
            ischeckedfile = true;
            // 更新界面
            loadCheckedTaskTunnelData();
        }
    }
    else
        // 记录到日志中
        QMessageBox::warning(this, QObject::tr("错误"), QObject::tr("设置有效帧范围无效：帧范围无效！"));

}

void CheckTaskWidget::getframesfromsetvalidframes_createtmppro(long long  newstartframes, long long newendframes)
{
    QString projectfilename = LzProjectAccess::getLzProjectAccessInstance()->getProjectFilename(LzProjectClass::Main);
    QString linename = projectfilename.left(projectfilename.length() - 14);
    QString datetime11 = projectfilename.right(13).left(8);
    qDebug() << "linename=" << linename << ",datatime=" << datetime11;

    int tunnelid = ui->actualTasksWidget->item(valid_row, CHECKEDTASK_TUNNELID)->data(Qt::DisplayRole).toInt();
    QString tunnelname = ui->actualTasksWidget->item(valid_row, CHECKEDTASK_TUNNELNAME)->data(Qt::DisplayRole).toString();
    QString seqnototal = ui->actualTasksWidget->item(valid_row, CHECKEDTASK_SEQNO_TOTAL)->data(Qt::DisplayRole).toString();
    int startseqno = 1;
    startseqno = QStringList(seqnototal.split("-")).at(0).toInt();

    // 引用赋值
    CheckedTaskList & clist = LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Main);
    bool findmodel = false;
    CheckedTunnelTaskModel & task = clist.getCheckedTunnelModel(tunnelid, startseqno, findmodel);
    if (findmodel)
    {
        std::list<CheckedItem>::iterator it = task.begin();
        __int64 tmpdeltanum;
        while (it != task.end())
        {
            if (newstartframes >= (*it).start_frame_master && newstartframes <= (*it).end_frame_master)
            {
                // 只找一个seqno下（一个原始序号采集文件）的帧，最多200帧
                if (newendframes > min(newstartframes + 200, (*it).end_frame_master))
                    newendframes = min(newstartframes + 200, (*it).end_frame_master);
                
                startseqno = (*it).seqno;

                QMessageBox msgBox;
                msgBox.setText(tr("提示"));
                msgBox.setInformativeText(tr("您要确定要将采集序号为%1隧道%2第%3-第%4帧生成临时工程？").arg(startseqno).arg(tunnelname).arg(newstartframes).arg(newendframes));
                msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                int ret = msgBox.exec();
                switch (ret)
                {
                    case QMessageBox::Yes:
                    {
                        LzProjectClass projectclass = LzProjectClass::Main;
                        QString currentprojectpath = LzProjectAccess::getLzProjectAccessInstance()->getProjectPath(projectclass);
                        QString newprojectdir = MasterSetting::getSettingInstance()->getParentPath() + "/" + projectfilename.left(projectfilename.length() - 5);
                        qDebug() << "new file receiver path" << newprojectdir;

                        QString tmpprojectname = "TEMPDIR_20000101";
                        QString tmpdirpath = MasterSetting::getSettingInstance()->getParentPath() + "/" + tmpprojectname + "/";
                        QDir createfile;
                        bool exist = createfile.exists(tmpdirpath);
                        if (exist)
                            LocalFileOperation::removeDirwithContent(tmpdirpath);
    
                        createfile.mkpath(QFileInfo(tmpdirpath).path());

                        QString newname = linename + "_" + datetime11;

                        // 创建的新的工程目录 【参考】设计-每个工程目录结构.txt
                        QDir mydir;
                        QString newpath = tmpdirpath;
                        QString newdir = newpath;
                        if (true)
                        {
                            mydir.mkpath(newpath);
                            newpath = newdir + "/collect";
                            mydir.mkpath(newpath);
                            newpath = newdir + "/calcu_calibration";
                            mydir.mkpath(newpath);
                            newpath = newdir + "/mid_calcu";
                            mydir.mkpath(newpath);
                            newpath = newdir + "/fuse_calcu";
                            mydir.mkpath(newpath);
                            newpath = newdir + "/syn_data";
                            mydir.mkpath(newpath);
                            newpath = newdir + "/tmp_img";
                            mydir.mkpath(newpath);
                            newpath = newdir + "/output";
                            mydir.mkpath(newpath);
                        }

                        bool ret2 = false;
                        // 拷贝创建新工程文件.proj

                        // 按当前时间创建空目录，等待存储
                        ProjectModel newProject = ProjectModel(1.0);
                        newProject.setPlanFilename(tmpprojectname + ".plan");
                        newProject.setRealFilename(tmpprojectname + ".real");
                        newProject.setCheckedFilename(tmpprojectname + ".checked");
                        QDateTime time = QDateTime::currentDateTime();
                        qDebug() << "time" << time.toString("yyyy-MM-dd hh:mm:ss") << time.toString("yyyyMMdd");
                        newProject.setCreateDate(time.toString("yyyy-MM-dd hh:mm:ss"));
                        newProject.setCreateUser(MasterSetting::getSettingInstance()->getCurrentUser());
                        // 创建新工程文件.proj
                        XMLProjectFileLoader * projloader = new XMLProjectFileLoader(newdir + "/" + tmpprojectname + ".proj");
                        ret2 = projloader->saveFile(newProject);
                        delete projloader;
                        if (ret2 == false)
                        {
                            QMessageBox::warning(this, QObject::tr("错误"), QObject::tr("[主控]  创建临时工程失败！工程入口（.proj）文件拷贝错误"));
                            return;
                        }

                        // 拷贝创建.real文件
                        ret2 = QFile::copy(newprojectdir + "/" + newname + ".real", newdir + "/" + tmpprojectname + ".real");
                        if (ret2 == false)
                        {
                            QMessageBox::warning(this, QObject::tr("错误"), QObject::tr("[主控]  创建临时工程失败！实际采集配置文件（.real）文件拷贝错误"));
                            return;
                        }

                        // 拷贝网络硬件参数任务文件
                        ret2 = QFile::copy(newprojectdir + "/network_config.xml", newdir + "/network_config.xml");
                        if (ret2 == false)
                        {
                            QMessageBox::warning(this, QObject::tr("错误"), QObject::tr("[主控]  创建临时工程失败！拷贝网络硬件参数文件错误，目录中已有该文件"));
                            return;
                        }

                        // 拷贝高度配置文件
                        ret2 = QFile::copy(newprojectdir + "/output_heights.xml", newdir + "/output_heights.xml");
                        if (ret2 == false)
                        {
                            QMessageBox::warning(this, QObject::tr("错误"), QObject::tr("[主控]  创建临时工程失败！拷贝隧道综合提取高度配置文件错误，目录中已有该文件"));
                            return;
                        }

                        qDebug() << "from: " << newprojectdir + "/calcu_calibration" << ", to: " << newdir + "/calcu_calibration"; 
                        // 拷贝双目标定参数配置文件
                        ret2 = LocalFileOperation::copyDirectoryFiles(newprojectdir + "/calcu_calibration",
                                                            newdir + "/calcu_calibration", true);
                        if (ret2 == false)
                        {
                            QMessageBox::warning(this, QObject::tr("错误"), QObject::tr("[主控]  创建临时工程失败！拷贝双目标定参数配置文件错误，目录中已有该文件"));
                            return;
                        }

                        // **创建.check文件
                        // 存入配置文件
                        CheckedTaskList tmplist;
                        CheckedTunnelTaskModel checkedTask = task;
                        CheckedItem item = (*it);
                        tmpdeltanum = newstartframes - item.start_frame_master;
                        item.start_mile = item.start_mile + (item.end_mile - item.start_mile) * (tmpdeltanum + 1) / (item.end_frame_master - item.start_frame_master + 1) ;
                        // 主控有效起始帧
                        item.start_frame_master = newstartframes;
                        item.start_frame_A1 = item.start_frame_A1 + tmpdeltanum;  // 从num开始
                        item.start_frame_A2 = item.start_frame_A2 + tmpdeltanum;
                        item.start_frame_B1 = item.start_frame_B1 + tmpdeltanum;
                        item.start_frame_B2 = item.start_frame_B2 + tmpdeltanum;
                        item.start_frame_C1 = item.start_frame_C1 + tmpdeltanum;
                        item.start_frame_C2 = item.start_frame_C2 + tmpdeltanum;
                        item.start_frame_D1 = item.start_frame_D1 + tmpdeltanum;
                        item.start_frame_D2 = item.start_frame_D2 + tmpdeltanum;
                        item.start_frame_E1 = item.start_frame_E1 + tmpdeltanum;
                        item.start_frame_E2 = item.start_frame_E2 + tmpdeltanum;
                        item.start_frame_F1 = item.start_frame_F1 + tmpdeltanum;
                        item.start_frame_F2 = item.start_frame_F2 + tmpdeltanum;
                        item.start_frame_G1 = item.start_frame_G1 + tmpdeltanum;
                        item.start_frame_G2 = item.start_frame_G2 + tmpdeltanum;
                        item.start_frame_H1 = item.start_frame_H1 + tmpdeltanum;
                        item.start_frame_H2 = item.start_frame_H2 + tmpdeltanum;
                        item.start_frame_I1 = item.start_frame_I1 + tmpdeltanum;
                        item.start_frame_I2 = item.start_frame_I2 + tmpdeltanum;
                        item.start_frame_J1 = item.start_frame_J1 + tmpdeltanum;
                        item.start_frame_J2 = item.start_frame_J2 + tmpdeltanum;
                        item.start_frame_K1 = item.start_frame_K1 + tmpdeltanum;
                        item.start_frame_K2 = item.start_frame_K2 + tmpdeltanum;
                        item.start_frame_L1 = item.start_frame_L1 + tmpdeltanum;
                        item.start_frame_L2 = item.start_frame_L2 + tmpdeltanum;
                        item.start_frame_M1 = item.start_frame_M1 + tmpdeltanum;
                        item.start_frame_M2 = item.start_frame_M2 + tmpdeltanum;
                        item.start_frame_N1 = item.start_frame_N1 + tmpdeltanum;
                        item.start_frame_N2 = item.start_frame_N2 + tmpdeltanum;
                        item.start_frame_O1 = item.start_frame_O1 + tmpdeltanum;
                        item.start_frame_O2 = item.start_frame_O2 + tmpdeltanum;
                        item.start_frame_P1 = item.start_frame_P1 + tmpdeltanum;
                        item.start_frame_P2 = item.start_frame_P2 + tmpdeltanum;
                        item.start_frame_Q1 = item.start_frame_Q1 + tmpdeltanum;
                        item.start_frame_Q2 = item.start_frame_Q2 + tmpdeltanum;
                        item.start_frame_R1 = item.start_frame_R1 + tmpdeltanum;
                        item.start_frame_R2 = item.start_frame_R2 + tmpdeltanum;

                        tmpdeltanum = item.end_frame_master  - newendframes;
                        float mileperframe = 1; // TODO
                        item.valid_frames_num = item.end_frame_master - item.start_frame_master + 1;
                        item.end_mile = item.end_mile - (item.end_mile - item.start_mile) * (tmpdeltanum + 1) / (item.end_frame_master - item.start_frame_master + 1) - mileperframe;
                        item.end_frame_master = item.end_frame_master - tmpdeltanum;
                        item.end_frame_A1 = item.end_frame_A1 - tmpdeltanum; // 截止到 TODO 
                        item.end_frame_A2 = item.end_frame_A2 - tmpdeltanum;
                        item.end_frame_B1 = item.end_frame_B1 - tmpdeltanum;
                        item.end_frame_B2 = item.end_frame_B2 - tmpdeltanum;
                        item.end_frame_C1 = item.end_frame_C1 - tmpdeltanum;
                        item.end_frame_C2 = item.end_frame_C2 - tmpdeltanum;
                        item.end_frame_D1 = item.end_frame_D1 - tmpdeltanum;
                        item.end_frame_D2 = item.end_frame_D2 - tmpdeltanum;
                        item.end_frame_E1 = item.end_frame_E1 - tmpdeltanum;
                        item.end_frame_E2 = item.end_frame_E2 - tmpdeltanum;
                        item.end_frame_F1 = item.end_frame_F1 - tmpdeltanum;
                        item.end_frame_F2 = item.end_frame_F2 - tmpdeltanum;
                        item.end_frame_G1 = item.end_frame_G1 - tmpdeltanum;
                        item.end_frame_G2 = item.end_frame_G2 - tmpdeltanum;
                        item.end_frame_H1 = item.end_frame_H1 - tmpdeltanum;
                        item.end_frame_H2 = item.end_frame_H2 - tmpdeltanum;
                        item.end_frame_I1 = item.end_frame_I1 - tmpdeltanum;
                        item.end_frame_I2 = item.end_frame_I2 - tmpdeltanum;
                        item.end_frame_J1 = item.end_frame_J1 - tmpdeltanum;
                        item.end_frame_J2 = item.end_frame_J2 - tmpdeltanum;
                        item.end_frame_K1 = item.end_frame_K1 - tmpdeltanum;
                        item.end_frame_K2 = item.end_frame_K2 - tmpdeltanum;
                        item.end_frame_L1 = item.end_frame_L1 - tmpdeltanum;
                        item.end_frame_L2 = item.end_frame_L2 - tmpdeltanum;
                        item.end_frame_M1 = item.end_frame_M1 - tmpdeltanum;
                        item.end_frame_M2 = item.end_frame_M2 - tmpdeltanum;
                        item.end_frame_N1 = item.end_frame_N1 - tmpdeltanum;
                        item.end_frame_N2 = item.end_frame_N2 - tmpdeltanum;
                        item.end_frame_O1 = item.end_frame_O1 - tmpdeltanum;
                        item.end_frame_O2 = item.end_frame_O2 - tmpdeltanum;
                        item.end_frame_P1 = item.end_frame_P1 - tmpdeltanum;
                        item.end_frame_P2 = item.end_frame_P2 - tmpdeltanum;
                        item.end_frame_Q1 = item.end_frame_Q1 - tmpdeltanum;
                        item.end_frame_Q2 = item.end_frame_Q2 - tmpdeltanum;
                        item.end_frame_R1 = item.end_frame_R1 - tmpdeltanum;
                        item.end_frame_R2 = item.end_frame_R2 - tmpdeltanum;

                        checkedTask.getRealList()->clear();
                        checkedTask.getRealList()->push_back(item);

                        //qDebug() << "checked" << QString::fromLocal8Bit(planTask.tunnelname.c_str()) << ",item num:" << checkedTask->getRealList()->size();
                        checkedTask.calcuItem.has_backup_calc_A = 0;
                        checkedTask.calcuItem.has_backup_calc_B = 0;
                        checkedTask.calcuItem.has_backup_calc_C = 0;
                        checkedTask.calcuItem.has_backup_calc_D = 0;
                        checkedTask.calcuItem.has_backup_calc_E = 0;
                        checkedTask.calcuItem.has_backup_calc_F = 0;
                        checkedTask.calcuItem.has_backup_calc_G = 0;
                        checkedTask.calcuItem.has_backup_calc_H = 0;
                        checkedTask.calcuItem.has_backup_calc_I = 0;
                        checkedTask.calcuItem.has_backup_calc_J = 0;
                        checkedTask.calcuItem.has_backup_calc_K = 0;
                        checkedTask.calcuItem.has_backup_calc_L = 0;
                        checkedTask.calcuItem.has_backup_calc_M = 0;
                        checkedTask.calcuItem.has_backup_calc_N = 0;
                        checkedTask.calcuItem.has_backup_calc_O = 0;
                        checkedTask.calcuItem.has_backup_calc_P = 0;
                        checkedTask.calcuItem.has_backup_calc_Q = 0;
                        checkedTask.calcuItem.has_backup_calc_R = 0;
                        checkedTask.calcuItem.has_backup_calc_RT = 0;
                        checkedTask.calcuItem.backup_calc_pos_fuse = 0;
                        checkedTask.calcuItem.cal_framecounter_begin = item.start_frame_master;
                        checkedTask.calcuItem.cal_framecounter_end = item.end_frame_master;
                        checkedTask.calcuItem.cal_valid_frames_num = item.valid_frames_num;
                        
                        tmplist.pushback(checkedTask);
                        XMLCheckedTaskFileLoader * ff = new XMLCheckedTaskFileLoader( newdir + "/" + tmpprojectname + ".checked");
                        bool ret = ff->saveFile(tmplist);
                        delete ff;
                        if (ret == false)
                        {
                            QMessageBox::warning(this, QObject::tr("错误"), QObject::tr("[主控]  创建临时工程失败！生成临时工程的（.check）文件失败"));
                            return;
                        }

                        MasterProgram::getMasterProgramInstance()->createTempProject(projectfilename, -1, linename, datetime11, tunnelid, startseqno, newstartframes, newendframes);

                    }
                    case QMessageBox::No:
                        return;
                    default:break;
                }

                break;                
            }
            it++;
        }
    }
    else
        // 记录到日志中
        QMessageBox::warning(this, QObject::tr("错误"), QObject::tr("生成临时工程错误无效：帧范围无效！"));
}

void CheckTaskWidget::loadPlanTaskTunnelData()//添加计划隧道文件
{
    //利用setModel()方法将数据模型与QTableView绑定
    ui->planTasksView->setModel(plantask_model);
    ui->planTasksView->setSelectionBehavior(QAbstractItemView::SelectRows);//按行选择
    ui->planTasksView->setEditTriggers(QAbstractItemView::NoEditTriggers);//不能被编辑
    ui->planTasksView->resizeColumnsToContents();//根据内容调整表格

    //设置表格的某列的宽度值，0/第一列，1/第二列····
    ui->planTasksView->setColumnWidth(2,100);

    updatePlanTaskWidget();
}

void CheckTaskWidget::updatePlanTaskWidget()
{
    plantask_model->clear();
    //添加表头
    //准备数据模型
    plantask_model->setHorizontalHeaderItem(PLANTASK_TUNNELID, new QStandardItem(QObject::tr("隧道ID")));
    plantask_model->setHorizontalHeaderItem(PLANTASK_TUNNELNAME, new QStandardItem(QObject::tr("隧道名称")));
    plantask_model->setHorizontalHeaderItem(PLANTASK_DATE, new QStandardItem(QObject::tr("计划时间")));
    plantask_model->setHorizontalHeaderItem(PLANTASK_TUNNELLENGTH, new QStandardItem(QObject::tr("隧道长度")));
    plantask_model->setHorizontalHeaderItem(PLANTASK_STARTMILEAGE, new QStandardItem(QObject::tr("起始里程")));
    plantask_model->setHorizontalHeaderItem(PLANTASK_ENDMILEAGE, new QStandardItem(QObject::tr("终止里程")));

    // 动态添加行
    for (int i = 0; i < LzProjectAccess::getLzProjectAccessInstance()->getLzPlanList(LzProjectClass::Main).list()->length(); i++)
    {
        PlanTask tmp = LzProjectAccess::getLzProjectAccessInstance()->getLzPlanList(LzProjectClass::Main).list()->at(i);
        plantask_model->setItem(i,PLANTASK_TUNNELID,new QStandardItem(QString("%1").arg(tmp.tunnelnum)));
        plantask_model->setItem(i,PLANTASK_TUNNELNAME,new QStandardItem(QObject::tr(tmp.tunnelname.c_str())));
        plantask_model->setItem(i,PLANTASK_DATE,new QStandardItem(tmp.datetime.c_str()));
        plantask_model->setItem(i,PLANTASK_STARTMILEAGE,new QStandardItem(QString("%1").arg(tmp.startdistance)));
        plantask_model->setItem(i,PLANTASK_ENDMILEAGE,new QStandardItem(QString("%1").arg(tmp.enddistance)));

        _int64 startdistance = tmp.startdistance;
        _int64 enddistance = tmp.enddistance;
        _int64 distance = enddistance-startdistance;
        //qDebug()<<distance;
        //将int类型转换为QString
        /*int a = 63;
        QString s = QString("%1").arg(a);  */
        plantask_model->setItem(i,PLANTASK_TUNNELLENGTH,new QStandardItem(QString("%1").arg(distance)));
    }
}

void CheckTaskWidget::loadCheckedTaskTunnelData()//添加实际隧道文件
{
    ui->actualTasksWidget->clear();
    ui->actualTasksWidget->setRowCount(0);
    ui->actualTasksWidget->setColumnCount(11);

    //添加表头
    //准备数据模型
    ui->actualTasksWidget->setHorizontalHeaderItem(CHECKEDTASK_TUNNELID, new QTableWidgetItem(QObject::tr("隧道ID")));
    ui->actualTasksWidget->setHorizontalHeaderItem(CHECKEDTASK_TUNNELNAME, new QTableWidgetItem(QObject::tr("隧道名称")));
    ui->actualTasksWidget->setHorizontalHeaderItem(CHECKEDTASK_TIME, new QTableWidgetItem(QObject::tr("采集时间")));
    ui->actualTasksWidget->setHorizontalHeaderItem(CHECKEDTASK_SEQNO_TOTAL, new QTableWidgetItem(QObject::tr("采集顺序号")));
    ui->actualTasksWidget->setHorizontalHeaderItem(CHECKEDTASK_START_MILE, new QTableWidgetItem(QObject::tr("起始里程")));
    ui->actualTasksWidget->setHorizontalHeaderItem(CHECKEDTASK_END_MILE, new QTableWidgetItem(QObject::tr("终止里程")));
    ui->actualTasksWidget->setHorizontalHeaderItem(CHECKEDTASK_STARTFRAME, new QTableWidgetItem(QObject::tr("起始帧数")));
    ui->actualTasksWidget->setHorizontalHeaderItem(CHECKEDTASK_ENDFRAME, new QTableWidgetItem(QObject::tr("终止帧数")));
    ui->actualTasksWidget->setHorizontalHeaderItem(CHECKEDTASK_TOTALFRAME, new QTableWidgetItem(QObject::tr("总帧数")));
    ui->actualTasksWidget->setHorizontalHeaderItem(CHECKEDTASK_CHECKIMAGE, new QTableWidgetItem(QObject::tr("原图浏览")));
    ui->actualTasksWidget->setHorizontalHeaderItem(CHECKEDTASK_CHECKVALIDFRAMES, new QTableWidgetItem(QObject::tr("设置有效帧范围")));

    //将actualTasksWidget设置为整行选中的话，则点击每个单元格都是整行选中，如果设置为选中单个目标的话，则每次只能点击一个单元格，任何选择的内容都不设置的话，则两者都可以选择。
    ui->actualTasksWidget->setSelectionBehavior(QAbstractItemView::SelectRows);  //整行选中的方式
    ui->actualTasksWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);   //表格禁止编辑
    
    ui->planTasksView->resizeColumnsToContents();//根据内容调整表格

    updateCheckedTaskWidget();
}

void CheckTaskWidget::updateCheckedTaskWidget()
{
    // 从实际隧道文件的的xml中解析实际隧道数据和校正之后的数据到listcollect中
    QString checkedFile = path + "/" + currentProjectModel.getCheckedFilename();
    QString realFile = path + "/" + currentProjectModel.getRealFilename();
    qDebug() << "checked filename: " << checkedFile << "real filename: " << realFile;

    // 引用赋值
    CheckedTaskList & clist = LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Main);

    XMLCheckedTaskFileLoader * checktask = new XMLCheckedTaskFileLoader(tr(checkedFile.toLocal8Bit().data()));
    bool ret = checktask->loadFile(clist);//生成listtask
    delete checktask;
    if(ret == false)
    {
        qDebug() << tr("不能加载校正任务文件--- checkedtaskfile");
        ui->textEdit->setText(tr("不能加载校正任务文件--- checkedtaskfile"));

        XMLRealTaskFileLoader * realtask = new XMLRealTaskFileLoader(realFile);
        ret = realtask->loadFile(LzProjectAccess::getLzProjectAccessInstance()->getLzRealList(LzProjectClass::Main));//生成listtunnelcheck
        if (ret == false)
        {
            qDebug() << tr("不能加载校正任务文件、实际采集文件--- checkedtaskfile、realtaskfile");
            ui->textEdit->setText(tr("不能加载校正任务文件、实际采集文件--- checkedtaskfile、realtaskfile"));
            return;
        }
        else
        {
            ischeckedfile = false;

            // 将realfile生成checkedfile
            resetCheckedFile();

            return;
        }
    }

    ischeckedfile = true;

    // qDebug
    //clist.showList();
    // 加载校正任务记录到widget中
    QList<CheckedTunnelTaskModel>::iterator checkedTaskIterator = clist.begin();
    while (checkedTaskIterator != clist.end())
    {
        //qDebug() << "gui show checked" << checkedTaskIterator->planTask.tunnelnum << QString::fromLocal8Bit(checkedTaskIterator->planTask.tunnelname.c_str());

        //动态设置QTabwidget的行数
        int row = ui->actualTasksWidget->rowCount();
        ui->actualTasksWidget->setRowCount(row+1);
        //qDebug()<<"row:"<<row;
        ui->actualTasksWidget->setItem(row,CHECKEDTASK_TUNNELID,new QTableWidgetItem(QString("%1").arg(checkedTaskIterator->planTask.tunnelnum)));
        ui->actualTasksWidget->setItem(row,CHECKEDTASK_TUNNELNAME,new QTableWidgetItem(QString::fromLocal8Bit(checkedTaskIterator->planTask.tunnelname.c_str())));

        int seqnostart;
        _int64 checkedstart;
        float milestart;

        int seqnoend;
        _int64 checkedend;
        float mileend;

        QString time;
        std::list<CheckedItem>::iterator it = checkedTaskIterator->begin();
        if (it != checkedTaskIterator->end())
        {
            time = QString(it->collecttime.c_str());
            seqnostart = it->seqno;
            checkedstart = it->start_frame_master;
            milestart = it->start_mile;

            seqnoend = it->seqno;
            checkedend = it->end_frame_master;
            mileend = it->end_mile;
            
            while (it != checkedTaskIterator->end())
            {
                if (it->start_frame_master < checkedstart)
                {
                    seqnostart = it->seqno;
                    checkedstart = it->start_frame_master;
                    milestart = it->start_mile;
                }
                if (it->end_frame_master > checkedend)
                {
                    seqnoend = it->seqno;
                    checkedend = it->end_frame_master;
                    mileend = it->end_mile;
                }
                it++;
            }
        }

        QString seqtotal = "";
        if (seqnostart == seqnoend)
            seqtotal = QString("%1").arg(seqnostart);
        else
            seqtotal = QString("%1-%2").arg(seqnostart).arg(seqnoend);

        ui->actualTasksWidget->setItem(row, CHECKEDTASK_TIME, new QTableWidgetItem(time));
        ui->actualTasksWidget->setItem(row, CHECKEDTASK_SEQNO_TOTAL, new QTableWidgetItem(QString("%1").arg(seqtotal)));
        ui->actualTasksWidget->setItem(row, CHECKEDTASK_START_MILE, new QTableWidgetItem(QString("%1").arg(milestart)));
        ui->actualTasksWidget->setItem(row, CHECKEDTASK_END_MILE, new QTableWidgetItem(QString("%1").arg(mileend)));
        ui->actualTasksWidget->setItem(row, CHECKEDTASK_STARTFRAME, new QTableWidgetItem(QString("%1").arg(checkedstart)));
        ui->actualTasksWidget->setItem(row, CHECKEDTASK_ENDFRAME, new QTableWidgetItem(QString("%1").arg(checkedend)));
        // 终止帧与结束帧相减,长度再加1，即为最终的长度。
        _int64 framecounter;
        if (checkedstart < checkedend)
            framecounter = checkedend - checkedstart + 1;
        else
            framecounter = checkedstart - checkedend + 1;
        QString framecounters = QString::number(framecounter);
        ui->actualTasksWidget->setItem(row, CHECKEDTASK_TOTALFRAME,new QTableWidgetItem(QString("%1").arg(framecounters)));

        //把新建按钮放在循环之外，则只能最后一行显示按钮，因为按钮是new的，所以只有一个，所以需要放在循环中，建一个存一个。
        QPushButton * imageviewerbutton = new QPushButton();
        imageviewerbutton->setText(tr("原图浏览"));
        //设置按钮信号槽函数
        connect(imageviewerbutton,SIGNAL(clicked()),this,SLOT(openimageviewer()));
        ui->actualTasksWidget->setCellWidget(row, CHECKEDTASK_CHECKIMAGE,imageviewerbutton);

        // 设置按钮
        QPushButton * valid_frames_button =new QPushButton();
        valid_frames_button->setText(tr("设置有效帧范围"));
        // 设置按钮信号槽函数
        ui->actualTasksWidget->setCellWidget(row,CHECKEDTASK_CHECKVALIDFRAMES,valid_frames_button);
        connect(valid_frames_button,SIGNAL(clicked()),this,SLOT(opensetvalidframes()));

        //qDebug() << checkedTaskIterator->getRealList()->size() << checkedstart << checkedend;

        checkedTaskIterator++;
    }

    ui->actualTasksWidget->resizeColumnsToContents();
    // 设置分离删除合并按钮是否可点击
    setOptButtonEnable();
}

// 在ischeckedfile == false时首先生成checkedfile
void CheckTaskWidget::resetCheckedFile()
{
    if (ischeckedfile != false)
    {
        QMessageBox msgBox;
        msgBox.setText(tr("警告！！"));
        msgBox.setInformativeText(tr("【注意！】您要确定重新校正文件%1？一旦重置，原计算的结果文件均不能对应，请【谨慎操作】！是否继续？").arg(currentProjectModel.getCheckedFilename()));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        int ret = msgBox.exec();
        switch (ret)
        {
            case QMessageBox::No:
                return;
            default:break;
        }

        this->log(QObject::tr("【重置】校正文件,原先操作均作废！"));
    }
    else
        this->log(QObject::tr("【重置】首次根据实际采集配置生成校正文件"));

    // 重新加载RealTaskList
    QString realFile = path + "/" + currentProjectModel.getRealFilename();
    LzProjectAccess::getLzProjectAccessInstance()->getLzRealList(LzProjectClass::Main).clear();
    XMLRealTaskFileLoader * realtask = new XMLRealTaskFileLoader(realFile);
    bool ret = realtask->loadFile(LzProjectAccess::getLzProjectAccessInstance()->getLzRealList(LzProjectClass::Main));//生成listtunnelcheck
    if (ret == false)
    {
        qDebug() << tr("不能加载实际采集文件--- realtaskfile");
        ui->textEdit->setText(tr("不能加载实际采集文件--- realtaskfile"));
        return;
    }

    // @author 范翔
    ischeckedfile = false;
    LzProjectAccess::getLzProjectAccessInstance()->getLzPlanList(LzProjectClass::Main).showList();
    QString projectdate = LzProjectAccess::getLzProjectAccessInstance()->getProjectFilename(LzProjectClass::Main).right(13).left(8);
    string projectdatestr = projectdate.toLocal8Bit().constData();
    if (ischeckedfile == false)
    {
        // 引用赋值
        CheckedTaskList & clist = LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Main);

        QList<RealTask>::iterator realit = LzProjectAccess::getLzProjectAccessInstance()->getLzRealList(LzProjectClass::Main).begin();
        clist.clear();
        CheckedTunnelTaskModel* tmp;
        int tmptunnelid;
        while (realit != LzProjectAccess::getLzProjectAccessInstance()->getLzRealList(LzProjectClass::Main).end())
        {
            tmp = new CheckedTunnelTaskModel();
            tmptunnelid = realit->tunnelid;
            PlanTask plantmp;
            bool ret = LzProjectAccess::getLzProjectAccessInstance()->getLzPlanList(LzProjectClass::Main).getPlanTask(tmptunnelid, plantmp);
            if (!ret)
                return;
            qDebug() << plantmp.tunnelnum << QObject::tr(plantmp.tunnelname.c_str()) << plantmp.tunnelnum << QObject::tr(plantmp.tunnelname.c_str()) << plantmp.datetime.c_str();
            tmp->setPlanTask(plantmp);
            CalcuFileItem calcuitem;
            calcuitem.cal_filename_prefix = plantmp.tunnelname + "_" + projectdatestr;
            calcuitem.cal_framecounter_begin = realit->start_frame_master;
            calcuitem.cal_framecounter_end = realit->end_frame_master;
            calcuitem.cal_valid_frames_num = calcuitem.cal_framecounter_end - calcuitem.cal_framecounter_begin + 1;
            calcuitem.fuse_calculte_time = "";
            tmp->setCalcuItem(calcuitem);
            CheckedItem * tmpitem = new CheckedItem();
            tmpitem->filename_prefix = plantmp.tunnelname + "_" + projectdatestr;
            tmpitem->seqno = realit->seqno;
            tmpitem->tunnel_name = realit->tunnelname;;
            tmpitem->tunnel_id = realit->tunnelid;;
            tmpitem->collecttime = realit->datetime;
            tmpitem->isvalid = realit->isvalid;
            tmpitem->valid_frames_num = realit->valid_frames;
            tmpitem->status = 0; // 尚未备份

            tmpitem->start_mile = realit->start_mile;
            tmpitem->end_mile = realit->end_mile;
            tmpitem->start_frame_master = realit->start_frame_master;
            tmpitem->end_frame_master = realit->end_frame_master;

            tmpitem->start_frame_A1 = realit->start_frame_A1;
            tmpitem->end_frame_A1 = realit->end_frame_A1;
            tmpitem->start_frame_A2 = realit->start_frame_A2;
            tmpitem->end_frame_A2 = realit->end_frame_A2;
            tmpitem->start_frame_B1 = realit->start_frame_B1;
            tmpitem->end_frame_B1 = realit->end_frame_B1;
            tmpitem->start_frame_B2 = realit->start_frame_B2;
            tmpitem->end_frame_B2 = realit->end_frame_B2;
            tmpitem->start_frame_C1 = realit->start_frame_C1;
            tmpitem->end_frame_C1 = realit->end_frame_C1;
            tmpitem->start_frame_C2 = realit->start_frame_C2;
            tmpitem->end_frame_C2 = realit->end_frame_C2;
            tmpitem->start_frame_D1 = realit->start_frame_D1;
            tmpitem->end_frame_D1 = realit->end_frame_D1;
            tmpitem->start_frame_D2 = realit->start_frame_D2;
            tmpitem->end_frame_D2 = realit->end_frame_D2;
            tmpitem->start_frame_E1 = realit->start_frame_E1;
            tmpitem->end_frame_E1 = realit->end_frame_E1;
            tmpitem->start_frame_E2 = realit->start_frame_E2;
            tmpitem->end_frame_E2 = realit->end_frame_E2;
            tmpitem->start_frame_F1 = realit->start_frame_F1;
            tmpitem->end_frame_F1 = realit->end_frame_F1;
            tmpitem->start_frame_F2 = realit->start_frame_F2;
            tmpitem->end_frame_F2 = realit->end_frame_F2;
            tmpitem->start_frame_G1 = realit->start_frame_G1;
            tmpitem->end_frame_G1 = realit->end_frame_G1;
            tmpitem->start_frame_G2 = realit->start_frame_G2;
            tmpitem->end_frame_G2 = realit->end_frame_G2;
            tmpitem->start_frame_H1 = realit->start_frame_H1;
            tmpitem->end_frame_H1 = realit->end_frame_H1;
            tmpitem->start_frame_H2 = realit->start_frame_H2;
            tmpitem->end_frame_H2 = realit->end_frame_H2;
            tmpitem->start_frame_I1 = realit->start_frame_I1;
            tmpitem->end_frame_I1 = realit->end_frame_I1;
            tmpitem->start_frame_I2 = realit->start_frame_I2;
            tmpitem->end_frame_I2 = realit->end_frame_I2;
            tmpitem->start_frame_J1 = realit->start_frame_J1;
            tmpitem->end_frame_J1 = realit->end_frame_J1;
            tmpitem->start_frame_J2 = realit->start_frame_J2;
            tmpitem->end_frame_J2 = realit->end_frame_J2;
            tmpitem->start_frame_K1 = realit->start_frame_K1;
            tmpitem->end_frame_K1 = realit->end_frame_K1;
            tmpitem->start_frame_K2 = realit->start_frame_K2;
            tmpitem->end_frame_K2 = realit->end_frame_K2;
            tmpitem->start_frame_L1 = realit->start_frame_L1;
            tmpitem->end_frame_L1 = realit->end_frame_L1;
            tmpitem->start_frame_L2 = realit->start_frame_L2;
            tmpitem->end_frame_L2 = realit->end_frame_L2;
            tmpitem->start_frame_M1 = realit->start_frame_M1;
            tmpitem->end_frame_M1 = realit->end_frame_M1;
            tmpitem->start_frame_M2 = realit->start_frame_M2;
            tmpitem->end_frame_M2 = realit->end_frame_M2;
            tmpitem->start_frame_N1 = realit->start_frame_N1;
            tmpitem->end_frame_N1 = realit->end_frame_N1;
            tmpitem->start_frame_N2 = realit->start_frame_N2;
            tmpitem->end_frame_N2 = realit->end_frame_N2;
            tmpitem->start_frame_O1 = realit->start_frame_O1;
            tmpitem->end_frame_O1 = realit->end_frame_O1;
            tmpitem->start_frame_O2 = realit->start_frame_O2;
            tmpitem->end_frame_O2 = realit->end_frame_O2;
            tmpitem->start_frame_P1 = realit->start_frame_P1;
            tmpitem->end_frame_P1 = realit->end_frame_P1;
            tmpitem->start_frame_P2 = realit->start_frame_P2;
            tmpitem->end_frame_P2 = realit->end_frame_P2;
            tmpitem->start_frame_Q1 = realit->start_frame_Q1;
            tmpitem->end_frame_Q1 = realit->end_frame_Q1;
            tmpitem->start_frame_Q2 = realit->start_frame_Q2;
            tmpitem->end_frame_Q2 = realit->end_frame_Q2;
            tmpitem->start_frame_R1 = realit->start_frame_R1;
            tmpitem->end_frame_R1 = realit->end_frame_R1;
            tmpitem->start_frame_R2 = realit->start_frame_R2;
            tmpitem->end_frame_R2 = realit->end_frame_R2;
            tmp->pushback(*tmpitem);
            realit++;
            clist.pushback(*tmp);
        }

        QString checkedFile = path + "/" + currentProjectModel.getCheckedFilename();
        if (checkedFile.compare("") == 0)
        {
            qDebug() << "checked file name not set";
            return;
        }
        XMLCheckedTaskFileLoader * ff = new XMLCheckedTaskFileLoader(checkedFile);
        bool ret = ff->saveFile(clist);
        delete ff;
        if (ret == false)
            qDebug() << "save(update) xml file fail";
        else
        {
            qDebug() << "save(update) xml file success";

            ischeckedfile = true;
            // 更新界面
            loadCheckedTaskTunnelData();
        }
    }
    else
    {
        qDebug() << tr("已经是checkedfile，resetcheckedfile出错！");
    }
}

// 设置当前文件名
bool CheckTaskWidget::setCurrentName(QString filename)
{
    bool ret = LzProjectAccess::getLzProjectAccessInstance()->setProjectName(LzProjectClass::Main, filename);
    if (ret)
        currentProjectModel = LzProjectAccess::getLzProjectAccessInstance()->getProjectModel(LzProjectClass::Main);
    return ret;
}

void CheckTaskWidget::openPlanTaskFile()
{
    // 切换目录
    loadConfigOpenCloseDir();

    QString file_full = QFileDialog::getOpenFileName(this, QObject::tr("打开采集工程文件"), openFileDir, "Document files (*.proj)");

    LzProjectAccess::getLzProjectAccessInstance()->setProjectName(LzProjectClass::Main, file_full);

    //仅获得绝对路径中的文件名
    QFileInfo fi;
    fi = QFileInfo(file_full);
    QString filename = fi.fileName();

    if (filename.endsWith("TEMPDIR_20000101.proj"))
    {
        QMessageBox::warning(this,tr("提示"), tr("TEMPDIR_20000101.proj不能作为工程文件打开！"));
        return;
    }

    if (false == setCurrentName(file_full))
    {
        qDebug() << tr("加载该采集工程文件%1错误") << filename;
        ui->textEdit->setText(tr("加载该采集工程文件%1错误").arg(filename));
        return;
    }

    LzProjectAccess::getLzProjectAccessInstance()->setProjectPath(LzProjectClass::Main, fi.path());
    path = fi.path();
    QString projectname = filename.left(filename.length() - 5);
    QString loggerfilename = path + "/" + projectname + "_checked.log";
    QString currentusername = MasterSetting::getSettingInstance()->getCurrentUser();
    qDebug() << "logger filename = " << loggerfilename << ", currentuser = " << currentusername;
    initLogger(loggerfilename.toLocal8Bit().constData(), currentusername.toLocal8Bit().constData());

    QString projectfilename = LzProjectAccess::getLzProjectAccessInstance()->getProjectFilename(LzProjectClass::Main);
    QString linename = projectfilename.left(projectfilename.length() - 14);
    QString datetime11 = projectfilename.right(13).left(8);
    qDebug() << "linename=" << linename << ",datatime=" << datetime11;

    // 发送实际采集配置文件
    QString realfilename = path + "/" + LzProjectAccess::getLzProjectAccessInstance()->getProjectModel(LzProjectClass::Main).getRealFilename();
    qDebug() << "send realfilename=" << realfilename;
    MasterProgram::getMasterProgramInstance()->getMultiThreadTcpServer()->sendFileToSlaves(realfilename);

    MasterProgram::getMasterProgramInstance()->workingMode_Setting(projectfilename, -1, linename, datetime11, WorkingStatus::Correcting, false);

    file_full = path + "/" + currentProjectModel.getPlanFilename();
    QString checked_filename = path + "/" + currentProjectModel.getCheckedFilename();
    QString real_filename = path + "/" +  currentProjectModel.getRealFilename();
    qDebug() << "filename:" << filename << "plan_filename:" << file_full;
    if (file_full.isNull())
    {
        // 用户取消选择文件
        //        QMessageBox::information(this, QObject::tr("打开计划隧道文件"),
        //                                 QObject::tr("没有指定要打开的数据文件目标"), QMessageBox::Ok | QMessageBox::Cancel);
        ui->textEdit->setTextColor(Qt::red);
        ui->textEdit->setText(QObject::tr("没有指定要打开的数据文件目标"));
    }
    else
    {
        // 发送消息，切换到校正工作模式
        QString projectfilename = LzProjectAccess::getLzProjectAccessInstance()->getProjectFilename(LzProjectClass::Main);
        QString linename = projectfilename.left(projectfilename.length() - 14);
        QString datetime11 = projectfilename.right(13).left(8);
        qDebug() << "linename=" << linename << ",datatime=" << datetime11;
        MasterProgram::getMasterProgramInstance()->workingMode_Setting(projectfilename, -1, linename, datetime11, WorkingStatus::Correcting, true);

        XMLTaskFileLoader *newtask = new XMLTaskFileLoader(tr(file_full.toLocal8Bit().data()));
        bool ret = newtask->loadFile(LzProjectAccess::getLzProjectAccessInstance()->getLzPlanList(LzProjectClass::Main));//计划隧道文件解析之后存入list
        delete newtask;
        if (ret == false)
        {
            qDebug() << tr("加载计划任务文件不成功");
            return;
        }
        else
        {
            // 更新计划任务文件并显示
            updatePlanTaskWidget();
            // 加载工程对应的实际隧道文件并显示
            loadCheckedTaskTunnelData();
            // 按照实际采集的隧道文件生成存储原图的文件夹
            QString filename;
            if (ischeckedfile)
                filename = checked_filename;
            else
                filename = real_filename;
            QFileInfo info_real(tr(filename.toLocal8Bit().data()));

            if(!(info_real.exists()))//实际隧道文件不存在时
            {
                //         ui->label_2->setText(QObject::tr("该计划隧道暂未进行采集。"));
                //         //更改QLabel中字体的颜色
                //         QPalette pe;
                //         pe.setColor(QPalette::WindowText,Qt::red);
                //         ui->label_2->setPalette(pe);
                if(plantask_model->rowCount()==0)//获得QTableview的行数
                {
                    ui->textEdit->setTextColor(Qt::red);
                    ui->textEdit->setText(QObject::tr("打开计划隧道文件失败，且实际隧道文件暂未采集。"));
                }
                else
                {
                    ui->textEdit->setTextColor(Qt::red);
                    if (ischeckedfile)
                        ui->textEdit->setText(QObject::tr("打开计划隧道文件成功，但校正隧道文件不存在。"));
                    else
                        ui->textEdit->setText(QObject::tr("打开计划隧道文件成功，但实际隧道文件未采集。"));
                }
            }
            else//实际隧道文件存在
            {
                if(plantask_model->rowCount()==0)
                {
                    ui->textEdit->setTextColor(Qt::red);
                    ui->textEdit->setText(QObject::tr("打开计划隧道文件失败，实际隧道文件已经采集。"));
                }
                else
                {
                    if (ischeckedfile)
                    {
                        ui->textEdit->setTextColor(Qt::black);
                        ui->textEdit->setText(QObject::tr("打开计划隧道文件成功，实际隧道文件已经采集并校正。"));
                    }
                    else
                    {
                        ui->textEdit->setTextColor(Qt::red);
                        ui->textEdit->setText(QObject::tr("打开计划隧道文件成功，实际隧道文件已经采集，请校正该文件。"));
                    }
                }
            }
        }
    }
}

void CheckTaskWidget::loadConfigOpenCloseDir()
{
    // 当前打开数据文件路径
    QString projectname = MasterSetting::getSettingInstance()->getParentPath();
    openFileDir = projectname;
    // qDebug()<<QDir::currentPath();
    saveFileDir = projectname;
}

/**
 * 校正-删除实际“隧道”，对CheckedTaskList直接操作(@see CheckedTaskList)
 * Step1 从currentRow往后的所有表项的planTask往依次向前移
 * Step2 删除节点
 * 然后存入CheckedTaskList对应的XML中
 */
void CheckTaskWidget::removeTaskTunnel()//删除
{
    QModelIndex index = ui->actualTasksWidget->currentIndex();
    if (!index.isValid())
    {
        QMessageBox::warning(this,tr("提示"), tr("请先选择一行！"));
        return;
    }
    // 得到数据要记录到日志中
    QString tunnelname = ui->actualTasksWidget->item(index.row(), CHECKEDTASK_TUNNELNAME)->data(Qt::DisplayRole).toString();
    QString seqnototal = ui->actualTasksWidget->item(index.row(), CHECKEDTASK_SEQNO_TOTAL)->data(Qt::DisplayRole).toString();

    int i = ui->actualTasksWidget->currentRow();
    int rowCount = ui->actualTasksWidget->rowCount();
    qDebug()<< "to remove ,currentRow:" << i << ", rowCount:" << rowCount;

    // 引用赋值
    CheckedTaskList & clist = LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Main);

    // Step1 从currentRow往后的所有表项的planTask往依次向前移
    QList<CheckedTunnelTaskModel>::iterator it1 = clist.end();
    // 【注意！】iterator从end()开始递减时，end()为空，第一项是iterator--
    it1--;
    int j = rowCount - 1;
    if (it1 >= clist.begin())
    {
        it1--;
        //qDebug() << "it1->planTask.tunnel_id=" << it1->planTask.tunnelnum;
        QList<CheckedTunnelTaskModel>::iterator it2 = clist.end();
        // 【注意！】iterator从end()开始递减时，end()为空，第一项是iterator--
        it2--;
        //qDebug() << "it2->planTask.tunnel_id=" << it2->planTask.tunnelnum;
        while ((j > i) && it1 >= clist.begin())
        {
            qDebug() << "it2->planTask.tunnel_id=" << it2->planTask.tunnelnum << "it1->planTask.tunnel_id=" << it1->planTask.tunnelnum;
            it2->planTask = it1->planTask;
            j--;
            it2--;
            it1--;
        }
        //clist.showList();
    } // 否则选择的是最后一行，直接进入Step2删除

    // Step2 删除节点
    clist.deletelistone(i);
    //clist.showList();

    // 更新XML
    QString checkedFile = path + "/" + currentProjectModel.getCheckedFilename();
    if (checkedFile.compare("") == 0)
    {
        qDebug() << "checked file name not set";
        return;
    }
    XMLCheckedTaskFileLoader * ff = new XMLCheckedTaskFileLoader(checkedFile);
    bool ret = ff->saveFile(clist);
    delete ff;
    if (ret == false)
        qDebug() << "save(update) xml file fail";
    else
        // 记录到日志中
        this->log(QObject::tr("删除实际采集的第%1条隧道，采集隧道名为%2").arg(seqnototal).arg(tunnelname));

    // 更新界面
    loadCheckedTaskTunnelData();
}

/**
 * 校正-合并两个以上实际“隧道”，对CheckedTaskList直接操作(@see CheckedTaskList)
 * Step1 往topRow为编号的CheckedTaskList的元素后方插入topRow+1到bottomRow的所有
 * Step2 bottomRow+1至结束的所有表项的planTask往后错(count-1)个，即list中序号为topRow+1往后的所有元素
 * Step3 删除多余
 * 然后存入CheckedTaskList对应的XML中
 */
void CheckTaskWidget::combineTaskTunnel()//合并
{
    QModelIndex index = ui->actualTasksWidget->currentIndex();
    if (!index.isValid())
    {
        QMessageBox::warning(this,tr("提示"), tr("请先选择需要合并的隧道！"));
        return;
    }

    if (ui->actualTasksWidget->selectedRanges().isEmpty() == false)
    {
        QList<QTableWidgetSelectionRange> ranges = ui->actualTasksWidget->selectedRanges();
        int count = ranges.count();
        bool sellectvalid = true;
        if (count <= 0)
                sellectvalid = false;
        int topRow = ranges.at(0).topRow();
        int bottomRow = ranges.at(0).bottomRow();
        if (count > 1)
        {
            int inttmp1;
            for (int i = 1; i < count; i++)
            {
                inttmp1 = ranges.at(i).topRow();
                if (inttmp1 == bottomRow + 1)
                    bottomRow = ranges.at(i).bottomRow();
                else
                {
                    sellectvalid = false;
                    break;
                }
            }
        }
        qDebug() << "sellectedvalid=" << sellectvalid << "count:" << count << ",topRow:" << topRow << "bottomRow:" << bottomRow;;
        if (!sellectvalid && (bottomRow - topRow <= 1))
        {
            QMessageBox::warning(this, tr("提示"), tr("请选择相邻两行或多行合并！"));
            return;
        }

        // 得到数据要记录到日志中
        QString tunnelnametop = ui->actualTasksWidget->item(topRow, CHECKEDTASK_TUNNELNAME)->data(Qt::DisplayRole).toString();
        QString seqnototaltop = ui->actualTasksWidget->item(topRow, CHECKEDTASK_SEQNO_TOTAL)->data(Qt::DisplayRole).toString();
        QString tunnelnamebottom = ui->actualTasksWidget->item(bottomRow, CHECKEDTASK_TUNNELNAME)->data(Qt::DisplayRole).toString();
        QString seqnototalbottom = ui->actualTasksWidget->item(bottomRow, CHECKEDTASK_SEQNO_TOTAL)->data(Qt::DisplayRole).toString();

        // 引用赋值
        CheckedTaskList & clist = LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Main);

        // Step1 往topRow为编号的CheckedTaskList的元素后方插入topRow+1到bottomRow的所有
        QList<CheckedTunnelTaskModel>::iterator it1 = clist.begin();
        int k = 0;
        while (it1 != clist.end())
        {
            if (k == topRow)
                break;
            it1++;
            k++;
        }
        QList<CheckedTunnelTaskModel>::iterator it2 = clist.begin();
        int j = 0;
        while (it2 != clist.end())
        {
            if (j >= topRow + 1 && j <= bottomRow)
            {
                std::list<CheckedItem>::iterator it = it2->begin();
                while (it != it2->end())
                {
                    CheckedItem *ch = new CheckedItem((*it));
                    it1->pushback(*ch);
                    it++;
                }
            }
            if (j >= bottomRow)
                    break;
            it2++;
            j++;
        }
        //clist.showList();

        // Step2 bottomRow+1至结束的所有表项的planTask往后错(count-1)个，即list中序号为topRow+1往后的所有元素
        it1 = clist.end();
        it1--;
        k = clist.list()->size() - 1;
        int a = k - (bottomRow - topRow);
        while (it1 >= clist.begin())
        {
            if (k == a)
                break;
            k--;
            it1--;
        }
        qDebug() << "it1 pos(=size-1-(bottomRow - topRow))=" << a << ",it1->planTask.tunnel_id=" << it1->planTask.tunnelnum;

        it2 = clist.end();
        // 【注意！】iterator从end()开始递减时，end()为空，第一项是iterator--
        it2--;
        qDebug() << "it2 pos(=size-1)=" << ",it2->planTask.tunnel_id=" << it2->planTask.tunnelnum;
        j = 0;
        while ((j < a - topRow + 1) && it1 >= clist.begin())
        {
            qDebug() << "it2->planTask.tunnel_id=" << it2->planTask.tunnelnum << "it1->planTask.tunnel_id=" << it1->planTask.tunnelnum;

            it2->planTask = it1->planTask;
            j++;
            it2--;
            it1--;
        }
        clist.showList();
        // Step3 删除多余
        for (int i = topRow + 1; i <= bottomRow; i++)
        {
            clist.deletelistone(topRow + 1);
        }
        clist.showList();

        // 更新XML
        QString checkedFile = path + "/" + currentProjectModel.getCheckedFilename();
        if (checkedFile.compare("") == 0)
        {
            qDebug() << "checked file name not set";
            return;
        }
        XMLCheckedTaskFileLoader * ff = new XMLCheckedTaskFileLoader(checkedFile);
        bool ret = ff->saveFile(clist);
        delete ff;
        if (ret == false)
            qDebug() << "save(update) xml file";
        else
            // 记录到日志中
            this->log(QObject::tr("合并实际采集的第%1条（采集隧道名为%2）到第%3条隧道（采集隧道名为%4），").arg(seqnototaltop).arg(tunnelnametop).arg(seqnototalbottom).arg(tunnelnamebottom));

        // 更新界面
        loadCheckedTaskTunnelData();
    }
    else
    {
        QMessageBox::warning(this, tr("提示"), tr("请选择相邻两行或多行！"));
        return;
    }
}

/**
 * 校正-分离一个实际“隧道”成为两个实际“隧道”，对CheckedTaskList直接操作(@see CheckedTaskList)
 * Step1 往currentRow为编号的CheckedTaskList的元素后中是否包含num帧号
 * Step2 从currentRow+1为编号planTask名称向后移动
 * 然后存入CheckedTaskList对应的XML中
 */
void CheckTaskWidget::separateTaskTunnel()//分离
{
    QModelIndex index = ui->actualTasksWidget->currentIndex();
    if (!index.isValid())
    {
        QMessageBox::warning(this,tr("提示"), tr("请先选择一行！"));
        return;
    }

    if(ui->down_lineEdit->text().compare("") == 0)//分离帧号为空
    {
        qDebug()<<1;
        QMessageBox::warning(this,tr("提示"), tr("请先输入分离的帧号!"));
        ui->down_lineEdit->setFocus();
        return;
    }

    // 得到数据要记录到日志中
    QString tunnelname = ui->actualTasksWidget->item(index.row(), CHECKEDTASK_TUNNELNAME)->data(Qt::DisplayRole).toString();
    QString seqnototal = ui->actualTasksWidget->item(index.row(), CHECKEDTASK_SEQNO_TOTAL)->data(Qt::DisplayRole).toString();

    _int64 num = ui->down_lineEdit->text().toLongLong();
    _int64 tmpdeltanum;
    int currentRow = ui->actualTasksWidget->currentRow();

    // 引用赋值
    CheckedTaskList & clist = LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Main);
    PlanTask tmpplan;

    // Step1 往currentRow为编号的CheckedTaskList的元素后中是否包含num帧号
    QList<CheckedTunnelTaskModel>::iterator it1 = clist.begin();
    int k = 0;
    while (it1 != clist.end())
    {
        if (k == currentRow)
        {
            tmpplan = it1->planTask;
            if (currentRow+1 < ui->planTasksView->model()->rowCount() && currentRow == ui->actualTasksWidget->rowCount() - 1)
            {
                tmpplan = LzProjectAccess::getLzProjectAccessInstance()->getLzPlanList(LzProjectClass::Main).list()->at(currentRow+1);
            }
            break;
        }
        it1++;
        k++;
    }
    CheckedTunnelTaskModel * inserttmp;
    inserttmp = new CheckedTunnelTaskModel();
    std::list<CheckedItem>::iterator it = it1->begin();
    bool find = false;
    while (it != it1->end())
    {
        if (find == false && it->start_frame_master < num && it->end_frame_master > num)
        {
            find = true;
            tmpdeltanum = num - it->start_frame_master;
            CheckedItem *ch = new CheckedItem((*it));

            ch->start_mile = it->start_mile + (it->end_mile - it->start_mile) * (tmpdeltanum + 1) / (it->end_frame_master - it->start_frame_master + 1) ;
            ch->start_frame_master = it->start_frame_master + tmpdeltanum + 1;
            ch->start_frame_A1 = it->start_frame_A1 + tmpdeltanum + 1;  // 从num开始
            ch->start_frame_A2 = it->start_frame_A2 + tmpdeltanum + 1;
            ch->start_frame_B1 = it->start_frame_B1 + tmpdeltanum + 1;
            ch->start_frame_B2 = it->start_frame_B2 + tmpdeltanum + 1;
            ch->start_frame_C1 = it->start_frame_C1 + tmpdeltanum + 1;
            ch->start_frame_C2 = it->start_frame_C2 + tmpdeltanum + 1;
            ch->start_frame_D1 = it->start_frame_D1 + tmpdeltanum + 1;
            ch->start_frame_D2 = it->start_frame_D2 + tmpdeltanum + 1;
            ch->start_frame_E1 = it->start_frame_E1 + tmpdeltanum + 1;
            ch->start_frame_E2 = it->start_frame_E2 + tmpdeltanum + 1;
            ch->start_frame_F1 = it->start_frame_F1 + tmpdeltanum + 1;
            ch->start_frame_F2 = it->start_frame_F2 + tmpdeltanum + 1;
            ch->start_frame_G1 = it->start_frame_G1 + tmpdeltanum + 1;
            ch->start_frame_G2 = it->start_frame_G2 + tmpdeltanum + 1;
            ch->start_frame_H1 = it->start_frame_H1 + tmpdeltanum + 1;
            ch->start_frame_H2 = it->start_frame_H2 + tmpdeltanum + 1;
            ch->start_frame_I1 = it->start_frame_I1 + tmpdeltanum + 1;
            ch->start_frame_I2 = it->start_frame_I2 + tmpdeltanum + 1;
            ch->start_frame_J1 = it->start_frame_J1 + tmpdeltanum + 1;
            ch->start_frame_J2 = it->start_frame_J2 + tmpdeltanum + 1;
            ch->start_frame_K1 = it->start_frame_K1 + tmpdeltanum + 1;
            ch->start_frame_K2 = it->start_frame_K2 + tmpdeltanum + 1;
            ch->start_frame_L1 = it->start_frame_L1 + tmpdeltanum + 1;
            ch->start_frame_L2 = it->start_frame_L2 + tmpdeltanum + 1;
            ch->start_frame_M1 = it->start_frame_M1 + tmpdeltanum + 1;
            ch->start_frame_M2 = it->start_frame_M2 + tmpdeltanum + 1;
            ch->start_frame_N1 = it->start_frame_N1 + tmpdeltanum + 1;
            ch->start_frame_N2 = it->start_frame_N2 + tmpdeltanum + 1;
            ch->start_frame_O1 = it->start_frame_O1 + tmpdeltanum + 1;
            ch->start_frame_O2 = it->start_frame_O2 + tmpdeltanum + 1;
            ch->start_frame_P1 = it->start_frame_P1 + tmpdeltanum + 1;
            ch->start_frame_P2 = it->start_frame_P2 + tmpdeltanum + 1;
            ch->start_frame_Q1 = it->start_frame_Q1 + tmpdeltanum + 1;
            ch->start_frame_Q2 = it->start_frame_Q2 + tmpdeltanum + 1;
            ch->start_frame_R1 = it->start_frame_R1 + tmpdeltanum + 1;
            ch->start_frame_R2 = it->start_frame_R2 + tmpdeltanum + 1;

            inserttmp->pushback(*ch);

            // 把当前帧数改写截止到num
            // TODO
            it->end_mile = it->start_mile + (it->end_mile - it->start_mile) * (tmpdeltanum + 1) / (it->end_frame_master - it->start_frame_master + 1) ;
            it->end_frame_master = it->start_frame_master + tmpdeltanum;
            it->end_frame_A1 = it->start_frame_A1 + tmpdeltanum; // 截止到num
            it->end_frame_A2 = it->start_frame_A2 + tmpdeltanum;
            it->end_frame_B1 = it->start_frame_B1 + tmpdeltanum;
            it->end_frame_B2 = it->start_frame_B2 + tmpdeltanum;
            it->end_frame_C1 = it->start_frame_C1 + tmpdeltanum;
            it->end_frame_C2 = it->start_frame_C2 + tmpdeltanum;
            it->end_frame_D1 = it->start_frame_D1 + tmpdeltanum;
            it->end_frame_D2 = it->start_frame_D2 + tmpdeltanum;
            it->end_frame_E1 = it->start_frame_E1 + tmpdeltanum;
            it->end_frame_E2 = it->start_frame_E2 + tmpdeltanum;
            it->end_frame_F1 = it->start_frame_F1 + tmpdeltanum;
            it->end_frame_F2 = it->start_frame_F2 + tmpdeltanum;
            it->end_frame_G1 = it->start_frame_G1 + tmpdeltanum;
            it->end_frame_G2 = it->start_frame_G2 + tmpdeltanum;
            it->end_frame_H1 = it->start_frame_H1 + tmpdeltanum;
            it->end_frame_H2 = it->start_frame_H2 + tmpdeltanum;
            it->end_frame_I1 = it->start_frame_I1 + tmpdeltanum;
            it->end_frame_I2 = it->start_frame_I2 + tmpdeltanum;
            it->end_frame_J1 = it->start_frame_J1 + tmpdeltanum;
            it->end_frame_J2 = it->start_frame_J2 + tmpdeltanum;
            it->end_frame_K1 = it->start_frame_K1 + tmpdeltanum;
            it->end_frame_K2 = it->start_frame_K2 + tmpdeltanum;
            it->end_frame_L1 = it->start_frame_L1 + tmpdeltanum;
            it->end_frame_L2 = it->start_frame_L2 + tmpdeltanum;
            it->end_frame_M1 = it->start_frame_M1 + tmpdeltanum;
            it->end_frame_M2 = it->start_frame_M2 + tmpdeltanum;
            it->end_frame_N1 = it->start_frame_N1 + tmpdeltanum;
            it->end_frame_N2 = it->start_frame_N2 + tmpdeltanum;
            it->end_frame_O1 = it->start_frame_O1 + tmpdeltanum;
            it->end_frame_O2 = it->start_frame_O2 + tmpdeltanum;
            it->end_frame_P1 = it->start_frame_P1 + tmpdeltanum;
            it->end_frame_P2 = it->start_frame_P2 + tmpdeltanum;
            it->end_frame_Q1 = it->start_frame_Q1 + tmpdeltanum;
            it->end_frame_Q2 = it->start_frame_Q2 + tmpdeltanum;
            it->end_frame_R1 = it->start_frame_R1 + tmpdeltanum;
            it->end_frame_R2 = it->start_frame_R2 + tmpdeltanum;
            it++;
            continue;
        }
        if (find) // 从第一个找到的item开始依次加入新的
        {
            CheckedItem *ch = new CheckedItem((*it));
            inserttmp->pushback(*ch);
        }
        it++;
    }
    if (find)
    {
        inserttmp->setPlanTask(tmpplan);
        CalcuFileItem calcuitem;
        calcuitem.cal_filename_prefix = inserttmp->planTask.tunnelname + "_" + inserttmp->planTask.datetime;
        calcuitem.cal_framecounter_begin = -1;
        calcuitem.cal_framecounter_end = -1;
        calcuitem.cal_valid_frames_num = -1;
        calcuitem.fuse_calculte_time = "";
        inserttmp->setCalcuItem(calcuitem);
        clist.insertAt(k + 1, (*inserttmp));
    }
    else
    {
        ui->textEdit->setText(tr("分离帧数无效！请重新输入！"));
        delete inserttmp;
        return;
    }
    //clist.showList();

    // Step2 从currentRow+1为编号planTask名称向后移动
    it1 = clist.begin();
    QList<CheckedTunnelTaskModel>::iterator it2 = clist.begin();
    k = 0;
    while (it1 != clist.end())
    {
        if (k == currentRow + 1)
            break;
        it1++;
        it2++;
        k++;
    }
    if (it2 != clist.end())
    {
        it2++;
        while (it1 != clist.end() && it2 != clist.end())
        {
            qDebug() << "it2->planTask.tunnel_id=" << it2->planTask.tunnelnum << "it1->planTask.tunnel_id=" << it1->planTask.tunnelnum;
            it1->planTask = it2->planTask;
            it2++;
            it1++;
        }
    }

    // 更新XML
    QString checkedFile = path + "/" + currentProjectModel.getCheckedFilename();
    if (checkedFile.compare("") == 0)
    {
        qDebug() << "checked file name not set";
        return;
    }
    XMLCheckedTaskFileLoader * ff = new XMLCheckedTaskFileLoader(checkedFile);
    bool ret = ff->saveFile(clist);
    delete ff;
    if (ret == false)
        qDebug() << "save(update) xml file";
    else
        // 记录到日志中
        this->log(QObject::tr("分离实际采集的第%1条隧道，采集隧道名为%2，从帧号%3处开始分离").arg(seqnototal).arg(tunnelname).arg(num));

    // 更新界面
    loadCheckedTaskTunnelData();
}

// 设置分离删除合并按钮是否可点击
void CheckTaskWidget::setOptButtonEnable()
{
    if (ui->actualTasksWidget->rowCount() >= 1)
    {
        ui->separate_pushButton->setEnabled(true);
        ui->deleteButton->setEnabled(true);
    }
    else
    {
        ui->separate_pushButton->setEnabled(false);
        ui->deleteButton->setEnabled(false);
        ui->combineButton->setEnabled(false);
        return;
    }
    if (ui->actualTasksWidget->rowCount() > 1)
        ui->combineButton->setEnabled(true);
    else
        ui->combineButton->setEnabled(false);
}

/*void CheckTaskWidget::on_pushButton_clicked()
{
    QProcess *process = new QProcess();
    QString filename;
    if (ischeckedfile)
        filename = path + "/" + currentProjectModel.getCheckedFilename();
    else
        filename = path + "/" + currentProjectModel.getRealFilename();
    QFileInfo info = QFileInfo(filename);
    if (info.exists())
        process->start(QString("notepad.exe %1").arg(tr(filename.toLocal8Bit().data())));//非阻塞
    else
        ui->textEdit->setText(tr("没有这个文件%1").arg(tr(filename.toLocal8Bit().data())));
}*/
