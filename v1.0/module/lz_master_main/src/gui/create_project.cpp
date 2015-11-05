#include "create_project.h"
#include "ui_create_project.h"

#include <QFileInfo>
#include <QFile>
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include <QDateTime>

#include "fileoperation.h"
#include "projectmodel.h"
#include "xmlprojectfileloader.h"
#include "setting_master.h"
#include "masterprogram.h"

#include "hardware_config.h"

/**
 * 主控机创建当日采集工程界面类定义
 * @author 范翔
 * @version 1.0.0
 * @date 2014-05-07
 */

/**
 * 界面构造函数
 * @param curwidgettype WorkingStatus类型的变量，是否要选择即将操作的状态，若是（采集、计算&备份）若否，不显示选择控件
 */
CreateProjectWidget::CreateProjectWidget(WorkingStatus curwidgettype, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CreateProjectWidget)
{
    ui->setupUi(this);

    // 初始化工作状态
    workingstatus = curwidgettype;

    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(operationChanged(int)));

    connect(ui->importProjButton, SIGNAL(clicked()), this, SLOT(importProj()));
    connect(ui->createProjButton, SIGNAL(clicked()), this, SLOT(createProj()));

    // 发送文件信号槽
    connect(ui->fileSynchronizationButton, SIGNAL(clicked()), this, SLOT(fileSynchronization()));
    
    // 设置project类别
    switch (curwidgettype)
    {
        case Collecting : projectclass = LzProjectClass::Collect; break;
        case Calculating : projectclass = LzProjectClass::Calculate; break;
        case Calculating_Backuping : projectclass = LzProjectClass::Calculate; break;
        case Backuping : projectclass = LzProjectClass::Backup; break;
        default : projectclass = LzProjectClass::Main;
    }

    // 界面显示
    if (curwidgettype == Collecting || curwidgettype == Calculating_Backuping || curwidgettype == Calculating || curwidgettype == Backuping )
    {
        ui->comboBox->setVisible(false);
        ui->widget->setEnabled(false);
    }
    else
    {
        connect(MasterProgram::getMasterProgramInstance(), SIGNAL(signalMsgToGUI(WorkingStatus, QString)), this, SLOT(appendMsg(WorkingStatus, QString)));
        connect(MasterProgram::getMasterProgramInstance(), SIGNAL(signalErrorToGUI(WorkingStatus, QString)), this, SLOT(appendMsg(WorkingStatus, QString)));
    }

    // 默认操作用户
    currentusername = "publicuser";
}

CreateProjectWidget::~CreateProjectWidget()
{
    delete ui;
}

void CreateProjectWidget::setWidgetEnabled(bool ifenabled)
{
    ui->comboBox->setEnabled(ifenabled);
    ui->createProjButton->setEnabled(ifenabled);
    ui->importProjButton->setEnabled(ifenabled);
    ui->fileSynchronizationButton->setEnabled(ifenabled);
}

void CreateProjectWidget::appendMsg(WorkingStatus status, QString msg)
{
    QString currentDate = QTime::currentTime().toString("[hh:mm:ss]");
    ui->textBrowser->append(QString("%1%2").arg(currentDate).arg(msg));
}

// 导入工程文件
void CreateProjectWidget::importProj()
{
    QString openFileDir = MasterSetting::getSettingInstance()->getParentPath();
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
    }
    else
    {
        ui->currentProjectNameLabel->setText(tr("请选择一个采集工程配置文件。"));
    }
}

// 创建新的工程文件及对应目录
void CreateProjectWidget::createProj()
{
    // cambobox放到采集处
    ui->comboBox->setCurrentIndex(0);

    QString planfileName;
    QString directory;
    QString openFileDir = MasterSetting::getSettingInstance()->getParentPath() + "/plan_tasks";
    planfileName = QFileDialog::getOpenFileName(this, tr("创建新工程，选择计划配置文件"), openFileDir, tr("计划文件 (*.plan)"));
    QFileInfo fi = QFileInfo(planfileName);
    qDebug() << "planfileName" << planfileName;
    if(!planfileName.isEmpty())
    {
        // 选择创建目录，默认为刚刚创建的目录
        QFileDialog *fd = new QFileDialog(this, tr("创建新工程，请选择工程存放目录"));
        fd->setFileMode(QFileDialog::DirectoryOnly);
        fd->setDirectory(MasterSetting::getSettingInstance()->getParentPath());
        if(fd->exec() == QFileDialog::Accepted) // ok
        {
            QStringList folders = fd->selectedFiles();
            if(folders.size() <= 0)
            {
                ui->currentProjectNameLabel->setText(tr("未选择新建工程存储目录。"));
                delete fd;
                return;
            }
            directory = folders[0];
            qDebug() << "directory" << directory;
            LzProjectAccess::getLzProjectAccessInstance()->setProjectPath(projectclass, directory);
        }
        else
        {
            ui->currentProjectNameLabel->setText(tr("创建新工程文件错误"));
            delete fd;
            return;
        }
        delete fd;

        // 按当前时间创建空目录，等待存储
        ProjectModel newProject = ProjectModel(1.0);
        planfileName = fi.fileName();
        newProject.setPlanFilename(planfileName);
        
        QDateTime time = QDateTime::currentDateTime();
        qDebug() << "time" << time.toString("yyyy-MM-dd hh:mm:ss") << time.toString("yyyyMMdd");
        newProject.setCreateDate(time.toString("yyyy-MM-dd hh:mm:ss"));
        newProject.setCreateUser(currentusername);

        QString linename = planfileName.left(planfileName.length() - 14);
        QString datetime11 = time.toString("yyyyMMdd");
        QString newname = linename + "_" + datetime11;

        newProject.setRealFilename(newname + ".real");
        newProject.setCheckedFilename(newname + ".checked");

        // 创建的新的工程目录 【参考】设计-每个工程目录结构.txt
        QDir mydir;
        QString newpath = MasterSetting::getSettingInstance()->getParentPath() + "/" + newname;
        QString newdir = newpath;
        if (!mydir.exists(newpath))
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
        newname = newdir + "/" + linename + "_" + datetime11 + ".proj";

        qDebug() << "new name=" << newname;

        // 创建新工程文件.proj
        XMLProjectFileLoader * projloader = new XMLProjectFileLoader(newname);
        bool ret1 = projloader->saveFile(newProject);
        delete projloader;
        if (ret1 == false)
        {
            ui->currentProjectNameLabel->setText(tr("创建新工程文件错误"));
            return;
        }

        ret1 = LzProjectAccess::getLzProjectAccessInstance()->setProjectName(projectclass, newname);
        if (ret1 == false)
        {
            ui->currentProjectNameLabel->setText(tr("新工程文件加载错误"));
            return;
        }

        // 拷贝计划任务文件
        bool ret2 = QFile::copy(fi.absoluteFilePath(), newdir + "/" + planfileName);
        if (ret2 == false)
        {
            ui->currentProjectNameLabel->setText(tr("拷贝计划文件错误，目录中已有该文件%1").arg(planfileName));
            //return;
        }

        // 拷贝网络硬件参数任务文件
        ret2 = QFile::copy(MasterSetting::getSettingInstance()->getParentPath() + "/system/network_config.xml", newdir + "/network_config.xml");
        if (ret2 == false)
        {
            ui->currentProjectNameLabel->setText(tr("拷贝网络硬件参数文件错误，目录中已有该文件"));
            //return;
        }

        // 拷贝高度配置文件
        ret2 = QFile::copy(MasterSetting::getSettingInstance()->getParentPath() + "/system/output_heights.xml", newdir + "/output_heights.xml");
        if (ret2 == false)
        {
            ui->currentProjectNameLabel->setText(tr("拷贝隧道综合提取高度配置文件错误，目录中已有该文件"));
            //return;
        }

        qDebug() << "from: " << MasterSetting::getSettingInstance()->getParentPath() + "/calcu_calibration" << ", to: " << newdir + "/calcu_calibration"; 
        // 拷贝双目标定参数配置文件，QRrail.xml文件，rectify.heights文件
        ret2 = LocalFileOperation::copyDirectoryFiles(MasterSetting::getSettingInstance()->getParentPath() + "/calcu_calibration",
                                            newdir + "/calcu_calibration", true);
        if (ret2 == false)
        {
            ui->currentProjectNameLabel->setText(tr("拷贝双目标定参数配置文件错误，目录中已有该文件"));
            //return;
        }

        // 显示时不显示文件路径
        fi = QFileInfo(newname);
        newname = fi.fileName();
        ui->currentProjectNameLabel->setText(tr("%1").arg(newname));
        emit sendproject_file_name(newname);
    }
    else
    {
        ui->currentProjectNameLabel->setText(tr("请选择一个计划任务配置文件。"));
    }
}

// 配置当前线路操作@author 范翔（@see MainWidget::configCurentOperation()）
bool CreateProjectWidget::configCurrentOperation()
{
    QString statusStr;
    LzProjectClass projectclass;

    int tmpindex = ui->comboBox->currentIndex();
    switch(tmpindex)
    {
        ///TODO 改界面 @author 范翔
        case 0: workingstatus = Collecting; projectclass = LzProjectClass::Collect; break;
        case 1: workingstatus = Calculating; projectclass = LzProjectClass::Calculate; break;
        case 2: workingstatus = Backuping; projectclass = LzProjectClass::Backup; break;
        case 3: workingstatus = Correcting; projectclass = LzProjectClass::Main; break;
        case 4: workingstatus = Deleting; projectclass = LzProjectClass::Main; break;
        default: workingstatus = Preparing; projectclass = LzProjectClass::Main; 
    }    

    QString projectfilename = LzProjectAccess::getLzProjectAccessInstance()->getProjectFilename(projectclass);

    if (workingstatus == Collecting) // 采集
    {
        statusStr = tr("采集");
    }
    else if (workingstatus == Correcting)
    {
        statusStr = tr("校正");

    }
    else if (workingstatus == Calculating_Backuping) // 计算+备份
    {
        statusStr = tr("计算&备份");
    }
    else if (workingstatus == Deleting)
    {
        statusStr = tr("删除从机存储空间");
    }

    if (projectfilename.compare("") != 0)
    {
        // 解析project filename 成为两段，linename和datetime
        QString linename = projectfilename.left(projectfilename.length() - 14);
        QString datetime11 = projectfilename.right(13).left(8);
        qDebug() << "linename=" << linename << ",datatime=" << datetime11;

        QMessageBox msgBox;
        msgBox.setText(tr("提示"));
        msgBox.setInformativeText(tr("您要确定设置工作模式为%1线路的%2吗").arg(projectfilename).arg(statusStr));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        int ret = msgBox.exec();
        switch (ret)
        {
            case QMessageBox::Yes:
                // 参数lineid暂时不用。设为-1
                MasterProgram::getMasterProgramInstance()->workingMode_Setting(projectfilename, -1, linename, datetime11, workingstatus, true);
                return true;
            default:break;
        }
    }
    else
    {
        QMessageBox::warning(this,tr("提示"), tr("您需要在设置工作模式首先设置文件名"));
    }
    qDebug() << QObject::tr("向从控机同步工程失败！");
    appendMsg(WorkingStatus::Preparing, QObject::tr("向从控机同步工程%1失败！").arg(projectfilename));
    return false;
}

// 线路配置文件同步
void CreateProjectWidget::fileSynchronization()
{
    // 先同步工程目录
    if (workingstatus == Preparing)
    {
        bool ret = configCurrentOperation();
        if (!ret)
        {
            return;
        }
    }

    ProjectModel currentProjModel = LzProjectAccess::getLzProjectAccessInstance()->getProjectModel(projectclass);
    QString projectfilename = LzProjectAccess::getLzProjectAccessInstance()->getProjectFilename(projectclass);
    QString projectdirpath = MasterSetting::getSettingInstance()->getParentPath() + "/" + projectfilename.left(projectfilename.length() - 5);

    // 发送工程目录文件
    QString newpath = projectdirpath + "/" +  projectfilename;
    MasterProgram::getMasterProgramInstance()->getMultiThreadTcpServer()->sendFileToSlaves(newpath);

    if (workingstatus == Collecting) // 采集
    {
        // 发送计划任务文件
        newpath = projectdirpath + "/" + currentProjModel.getPlanFilename();
        MasterProgram::getMasterProgramInstance()->init_TaskFile(newpath);

        newpath = MasterSetting::getSettingInstance()->getParentPath() + "/system/network_config.xml";
        MasterProgram::getMasterProgramInstance()->init_NetworkConfigFile(newpath);

    }
    else if (workingstatus == Correcting) // 校正
    {
        // 发送实际采集任务目录文件
        QString newpath = projectdirpath + "/" + currentProjModel.getRealFilename();
        MasterProgram::getMasterProgramInstance()->getMultiThreadTcpServer()->sendFileToSlaves(newpath);

        newpath = MasterSetting::getSettingInstance()->getParentPath() + "/system/network_config.xml";
        MasterProgram::getMasterProgramInstance()->init_NetworkConfigFile(newpath);
    }
    else if (workingstatus == Calculating_Backuping || workingstatus == Calculating || workingstatus == Backuping) // 计算+备份
    {
        if (workingstatus == Calculating_Backuping || workingstatus == Calculating)
        {
            // 发送计算双目标定参数配置文件
            MasterProgram::getMasterProgramInstance()->init_CameraCalibrationParamFile();
        
            // 发送分中外标定参数配置文件
            // @author 范翔 每组相机倒到分中坐标系的RT矩阵配置文件与双目标定配置文件二合一
            //MasterProgram::getMasterProgramInstance()->init_FenZhongCalibParamFile();
        }

        // 发送实际采集任务目录文件
        QString newpath = projectdirpath + "/" + currentProjModel.getRealFilename();
        MasterProgram::getMasterProgramInstance()->getMultiThreadTcpServer()->sendFileToSlaves(newpath);

        // 发送校正任务目录文件
        newpath = projectdirpath + "/" + currentProjModel.getCheckedFilename();
        MasterProgram::getMasterProgramInstance()->getMultiThreadTcpServer()->sendFileToSlaves(newpath);

        newpath = MasterSetting::getSettingInstance()->getParentPath() + "/system/network_config.xml";
        MasterProgram::getMasterProgramInstance()->init_NetworkConfigFile(newpath);
    }

    HardwareConfigWidget *widget3 = new HardwareConfigWidget();
    widget3->show();

}

// 设置为计算备份界面，不显示创建工程按钮
void CreateProjectWidget::setToCalculateStyle()
{
    ui->createProjButton->setVisible(false);
    ui->fileSynchronizationButton->setVisible(true);
    workingstatus = Calculating_Backuping;
}

/**
 * 操作cambobox切换槽函数
 * 因为删除选择时，不用同步
 */
void CreateProjectWidget::operationChanged(int currentindex)
{
    if (currentindex == 3)
        ui->fileSynchronizationButton->setVisible(false);
    else
        ui->fileSynchronizationButton->setVisible(true);
}
