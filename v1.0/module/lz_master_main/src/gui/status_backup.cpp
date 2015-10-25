#include "status_backup.h"
#include "ui_status_backup.h"

#include <QTextCodec>
#include "masterprogram.h"
#include "mainwidget.h"
#include <QStandardItemModel>
#include <QProgressBar>
#include <QMessageBox>
#include "xmlcheckedtaskfileloader.h"
#include "xmlrealtaskfileloader.h"
#include "checkedtask.h"
#include "checkedtask_list.h"
#include "realtask_list.h"
#include "realtask.h"

#include "xmlprojectfileloader.h"
#include "setting_master.h"

/**
 * 监控界面——备份界面类定义
 * @author 熊雪
 * @version 1.0.0
 * @date 2014-03-01
 */
BackupWidget::BackupWidget(QWidget *parent) :
    QWidget(parent), currentProjectModel(1.0),
    ui(new Ui::BackupWidget)
{
    ui->setupUi(this);

    // 初始时未导入工程
    hasinintproject = false;

    setOptButtonEnable(false);

    //    ui->label_2->setStyleSheet("color:green");//设置label中文本的颜色
    //    ui->label_2->setStyleSheet("background-color: green");//设置lable的背景颜色
    //    ui->label_3->setStyleSheet("background-color: green");//设置lable的背景颜色

    // TODO暂时主控备份的暂停无效
    ui->stopMasterBackupButton->setVisible(false);

    connect(ui->resetButton, SIGNAL(clicked()), this, SLOT(resetBackupConfigFile()));
}

BackupWidget::~BackupWidget()
{
    delete ui;
}


void BackupWidget::saveResetFile()
{
    QString path = LzProjectAccess::getLzProjectAccessInstance()->getProjectPath(LzProjectClass::Backup);
    QString filename = path + "/" + currentProjectModel.getCheckedFilename();

    XMLCheckedTaskFileLoader * checktask = new XMLCheckedTaskFileLoader(tr(filename.toLocal8Bit().data()));
    bool ret = checktask->saveFile(LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Backup));//生成listtask
    delete checktask;
}

void BackupWidget::resetBackupConfigFile()
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

    LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Backup).resetCalcuPos();

    // 重新覆盖原文件
    saveResetFile();

    updateCheckedTunnelView();
}

void BackupWidget::on_startBackupButton_clicked()
{
    SelectBackupMode(0);
}

void BackupWidget::on_stopBackupButton_clicked()
{
    MasterProgram::getMasterProgramInstance()->backup_Stop();
}

void BackupWidget::on_startMasterBackupButton_clicked()
{
    SelectBackupMode(1);
}

void BackupWidget::on_stopMasterBackupButton_clicked()
{
    //MasterProgram::getMasterProgramInstance()->backup_MasterStop();
}

void BackupWidget::changeSlaveTask(WorkingStatus status, QString index, int threadid, QString msgtask, QString remark)
{
    int slaveid = index.toInt();
    QLabel * tmpremark = NULL;
    QProgressBar * tmpprogressbar = NULL;

    if (status == WorkingStatus::Backuping)
    {
        switch (slaveid)
        {
            case 0: ui->label_back01->setText(msgtask); tmpremark = ui->label_back01_remark; tmpprogressbar = ui->progressBar_back01; break;// 主控
            case 1: ui->label_back11->setText(msgtask); tmpremark = ui->label_back11_remark; tmpprogressbar = ui->progressBar_back11; break;
            case 2: ui->label_back21->setText(msgtask); tmpremark = ui->label_back21_remark; tmpprogressbar = ui->progressBar_back21; break;
            case 3: ui->label_back31->setText(msgtask); tmpremark = ui->label_back31_remark; tmpprogressbar = ui->progressBar_back31; break;
            case 4: ui->label_back41->setText(msgtask); tmpremark = ui->label_back41_remark; tmpprogressbar = ui->progressBar_back41; break;
            case 5: ui->label_back51->setText(msgtask); tmpremark = ui->label_back51_remark; tmpprogressbar = ui->progressBar_back51; break;
            case 6: ui->label_back61->setText(msgtask); tmpremark = ui->label_back61_remark; tmpprogressbar = ui->progressBar_back61; break;
            case 7: ui->label_back71->setText(msgtask); tmpremark = ui->label_back71_remark; tmpprogressbar = ui->progressBar_back71; break;
            case 8: ui->label_back81->setText(msgtask); tmpremark = ui->label_back81_remark; tmpprogressbar = ui->progressBar_back81; break;
            case 9: ui->label_back91->setText(msgtask); tmpremark = ui->label_back91_remark; tmpprogressbar = ui->progressBar_back91; break;
            default: break;
        }
        tmpremark->setText(remark);
        if (remark.compare("ok") == 0)
            tmpprogressbar->setValue(tmpprogressbar->maximum());
    }
}

/**
 * 修改进度条槽函数
 */
void BackupWidget::changecalcubackupProgressingBar(WorkingStatus status, QString slaveidstr, int threadid, int totalnum, int currentnum, bool isupdate)
{
    int slaveid = slaveidstr.toInt();
    if (!isupdate) // 设置初始进度
    {
        if (status == WorkingStatus::Backuping)
        {
            switch (slaveid)
            {
                case 0: ui->progressBar_backmaster->setMaximum(totalnum); ui->progressBar_backmaster->setValue(currentnum); break;// 主控
                case 1: ui->progressBar_back1->setMaximum(totalnum); ui->progressBar_back1->setValue(currentnum); break;
                case 2: ui->progressBar_back2->setMaximum(totalnum); ui->progressBar_back2->setValue(currentnum); break;
                case 3: ui->progressBar_back3->setMaximum(totalnum); ui->progressBar_back3->setValue(currentnum); break;
                case 4: ui->progressBar_back4->setMaximum(totalnum); ui->progressBar_back4->setValue(currentnum); break;
                case 5: ui->progressBar_back5->setMaximum(totalnum); ui->progressBar_back5->setValue(currentnum); break;
                case 6: ui->progressBar_back6->setMaximum(totalnum); ui->progressBar_back6->setValue(currentnum); break;
                case 7: ui->progressBar_back7->setMaximum(totalnum); ui->progressBar_back7->setValue(currentnum); break;
                case 8: ui->progressBar_back8->setMaximum(totalnum); ui->progressBar_back8->setValue(currentnum); break;
                case 9: ui->progressBar_back9->setMaximum(totalnum); ui->progressBar_back9->setValue(currentnum); break;
                default: break;
            }
        }
    }
    else // 更新值
    {
        int tmpval = 0;
        if (status == WorkingStatus::Backuping)
        {
            switch (slaveid)
            {
                case 0: tmpval = ui->progressBar_backmaster->value(); ui->progressBar_backmaster->setValue(++tmpval); break;// 主控
                case 1: tmpval = ui->progressBar_back1->value(); ui->progressBar_back1->setValue(++tmpval); break;
                case 2: tmpval = ui->progressBar_back2->value(); ui->progressBar_back2->setValue(++tmpval); break;
                case 3: tmpval = ui->progressBar_back3->value(); ui->progressBar_back3->setValue(++tmpval); break;
                case 4: tmpval = ui->progressBar_back4->value(); ui->progressBar_back4->setValue(++tmpval); break;
                case 5: tmpval = ui->progressBar_back5->value(); ui->progressBar_back5->setValue(++tmpval); break;
                case 6: tmpval = ui->progressBar_back6->value(); ui->progressBar_back6->setValue(++tmpval); break;
                case 7: tmpval = ui->progressBar_back7->value(); ui->progressBar_back7->setValue(++tmpval); break;
                case 8: tmpval = ui->progressBar_back8->value(); ui->progressBar_back8->setValue(++tmpval); break;
                case 9: tmpval = ui->progressBar_back9->value(); ui->progressBar_back9->setValue(++tmpval); break;
                default: break;
            }
        }
    }
}

void BackupWidget::changeSlaveCameraTaskFC_init(WorkingStatus status, QString index, int threadid, QString msgtask, long long startfc, long long endfc)
{
    if (status == WorkingStatus::Backuping)
    {
        int slaveid = index.toInt();
        QLabel * tmplabelcalcuremark = NULL;
        QLabel * tmplabelcalcutask_thread1 = NULL;
        QProgressBar * tmpprogressbar_thread1 = NULL;

        if (threadid != 0)
        {
            switch (slaveid)
            {
                case 0: tmplabelcalcutask_thread1 = ui->label_back01; tmpprogressbar_thread1 = ui->progressBar_back01; break;// 主控
                case 1: tmplabelcalcutask_thread1 = ui->label_back11; tmpprogressbar_thread1 = ui->progressBar_back11; break;
                case 2: tmplabelcalcutask_thread1 = ui->label_back21; tmpprogressbar_thread1 = ui->progressBar_back21; break;
                case 3: tmplabelcalcutask_thread1 = ui->label_back31; tmpprogressbar_thread1 = ui->progressBar_back31; break;
                case 4: tmplabelcalcutask_thread1 = ui->label_back41; tmpprogressbar_thread1 = ui->progressBar_back41; break;
                case 5: tmplabelcalcutask_thread1 = ui->label_back51; tmpprogressbar_thread1 = ui->progressBar_back51; break;
                case 6: tmplabelcalcutask_thread1 = ui->label_back61; tmpprogressbar_thread1 = ui->progressBar_back61; break;
                case 7: tmplabelcalcutask_thread1 = ui->label_back71; tmpprogressbar_thread1 = ui->progressBar_back71; break;
                case 8: tmplabelcalcutask_thread1 = ui->label_back81; tmpprogressbar_thread1 = ui->progressBar_back81; break;
                case 9: tmplabelcalcutask_thread1 = ui->label_back91; tmpprogressbar_thread1 = ui->progressBar_back91; break;
                default: break;
            }
            tmplabelcalcutask_thread1->setText(msgtask);
            tmpprogressbar_thread1->setMinimum(startfc);
            tmpprogressbar_thread1->setMaximum(endfc);
            tmpprogressbar_thread1->setValue(startfc);
        }
        else
            qDebug() << QObject::tr("无法显示的备份消息：从控%1的备份0线程当前任务%2初始化，备份帧范围%3-%4").arg(slaveid).arg(msgtask).arg(startfc).arg(endfc);

    }
}

void BackupWidget::changeSlaveCameraTaskFC(WorkingStatus status, QString index, int threadid, QString msgtask, long long currentfc)
{
    if (status == WorkingStatus::Backuping)
    {
        int slaveid = index.toInt();
        QLabel * tmplabelcalcuremark = NULL;
        QLabel * tmplabelcalcutask_thread1 = NULL;
        QProgressBar * tmpprogressbar_thread1 = NULL;

        if (threadid != 0)
        {
            switch (slaveid)
            {
                case 0: tmplabelcalcutask_thread1 = ui->label_back01; tmpprogressbar_thread1 = ui->progressBar_back01; break;// 主控
                case 1: tmplabelcalcutask_thread1 = ui->label_back11; tmpprogressbar_thread1 = ui->progressBar_back11; break;
                case 2: tmplabelcalcutask_thread1 = ui->label_back21; tmpprogressbar_thread1 = ui->progressBar_back21; break;
                case 3: tmplabelcalcutask_thread1 = ui->label_back31; tmpprogressbar_thread1 = ui->progressBar_back31; break;
                case 4: tmplabelcalcutask_thread1 = ui->label_back41; tmpprogressbar_thread1 = ui->progressBar_back41; break;
                case 5: tmplabelcalcutask_thread1 = ui->label_back51; tmpprogressbar_thread1 = ui->progressBar_back51; break;
                case 6: tmplabelcalcutask_thread1 = ui->label_back61; tmpprogressbar_thread1 = ui->progressBar_back61; break;
                case 7: tmplabelcalcutask_thread1 = ui->label_back71; tmpprogressbar_thread1 = ui->progressBar_back71; break;
                case 8: tmplabelcalcutask_thread1 = ui->label_back81; tmpprogressbar_thread1 = ui->progressBar_back81; break;
                case 9: tmplabelcalcutask_thread1 = ui->label_back91; tmpprogressbar_thread1 = ui->progressBar_back91; break;
                default: break;
            }
            tmplabelcalcutask_thread1->setText(msgtask);
            tmpprogressbar_thread1->setValue(currentfc);
            
        }
        else
            qDebug() << QObject::tr("无法显示的备份消息：从控%1的备份0线程当前任务%2执行到第%3帧").arg(slaveid).arg(msgtask).arg(currentfc);

    }
}

// 加载计划任务中的隧道列表
void BackupWidget::updateCheckedTunnelView()
{
    currentProjectModel = LzProjectAccess::getLzProjectAccessInstance()->getProjectModel(LzProjectClass::Backup);
 
    if (currentProjectModel == NULL)
    {
        QMessageBox::warning(this, tr("工程导入"), tr("工程入口文件导入失败"));
        hasinintproject = false;
        return;
    }
    hasinintproject = true;

    loadCheckTaskTunnelData();
}

void BackupWidget::loadCheckTaskTunnelData()
{
    ui->actualTasksWidget->clear();
    ui->actualTasksWidget->setRowCount(0);
    ui->actualTasksWidget->setColumnCount(9);

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

    //将actualTasksWidget设置为整行选中的话，则点击每个单元格都是整行选中，如果设置为选中单个目标的话，则每次只能点击一个单元格，任何选择的内容都不设置的话，则两者都可以选择。
    ui->actualTasksWidget->setSelectionBehavior(QAbstractItemView::SelectRows);  //整行选中的方式
    ui->actualTasksWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);   //表格禁止编辑

    updateCheckedTaskWidget();
}

void BackupWidget::updateCheckedTaskWidget()
{
    if (!hasinintproject)
        return;

    QString path = LzProjectAccess::getLzProjectAccessInstance()->getProjectPath(LzProjectClass::Backup);
    QString filename = path + "/" + currentProjectModel.getCheckedFilename();
    QString realFile = path + "/" + currentProjectModel.getRealFilename();

    XMLCheckedTaskFileLoader * checktask = new XMLCheckedTaskFileLoader(tr(filename.toLocal8Bit().data()));
    bool ret = checktask->loadFile(LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Calculate));//生成listtask
    delete checktask;

    if(filename.compare("") == 0 || ret == false)
    {
        QMessageBox::information(this,tr("导入工程错误"),tr("该工程下实际采集文件还未校正%1。").arg(filename),QMessageBox::Yes|QMessageBox::No);
        return;
    }

    // 加载校正任务记录到widget中
    QList<CheckedTunnelTaskModel>::iterator checkedTaskIterator = LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Calculate).begin();
    while (checkedTaskIterator != LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Calculate).end())
    {
        qDebug() << "gui show checked" << checkedTaskIterator->planTask.tunnelnum << QString::fromLocal8Bit(checkedTaskIterator->planTask.tunnelname.c_str());

        // 动态设置QTabwidget的行数
        int row = ui->actualTasksWidget->rowCount();
        ui->actualTasksWidget->setRowCount(row+1);
        // qDebug()<<"row:"<<row;
        ui->actualTasksWidget->setItem(row,0,new QTableWidgetItem(QString("%1").arg(checkedTaskIterator->planTask.tunnelnum)));
        ui->actualTasksWidget->setItem(row,1,new QTableWidgetItem(QString::fromLocal8Bit(checkedTaskIterator->planTask.tunnelname.c_str())));

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
        ui->actualTasksWidget->setItem(row,CHECKEDTASK_TOTALFRAME,new QTableWidgetItem(QString("%1").arg(framecounters)));
        qDebug() << checkedTaskIterator->getRealList()->size() << checkedstart << checkedend;

        checkedTaskIterator++;
    }

    ui->actualTasksWidget->resizeColumnsToContents();
    ui->actualTasksWidget->horizontalHeader()->setStretchLastSection(true);

}

void BackupWidget::setOptButtonEnable(bool initstatus)
{
    isincalculatebackupstatus = initstatus;

    if (initstatus && ui->actualTasksWidget->rowCount() >= 1)
    {
        ui->startBackupButton->setEnabled(true);
        ui->stopBackupButton->setEnabled(false);
        ui->startMasterBackupButton->setEnabled(true);
        ui->stopMasterBackupButton->setEnabled(false);
    }
    else
    {
        ui->startBackupButton->setEnabled(false);
        ui->stopBackupButton->setEnabled(false);
        ui->startMasterBackupButton->setEnabled(false);
        ui->stopMasterBackupButton->setEnabled(false);
    }
}

void BackupWidget::SelectBackupMode(int backuptype)
{
    bool line = ui->line_radioButton->isChecked();
    bool tunnel = ui->tunnel_radioButton->isChecked();
    if ((line == false) && (tunnel == false))
    {
        QMessageBox::information(this,tr("提示"),tr("请先选择备份模式"),QMessageBox::Yes);
        return;
    }
    else
    {
        if (line == true) // 按线路
        {
            if (backuptype == 0)
            {
                MasterProgram::getMasterProgramInstance()->backup_beginStartAll();
                ui->startBackupButton->setEnabled(false);
                ui->stopBackupButton->setEnabled(true);
            }
            else
            {
                MasterProgram::getMasterProgramInstance()->backup_Master_beginStartAll();
                ui->startMasterBackupButton->setEnabled(false);
                ui->stopMasterBackupButton->setEnabled(true);
            }
        }
        else // 按某条隧道
        {
            QModelIndex index = ui->actualTasksWidget->currentIndex();
            if (!index.isValid())
            {
                QMessageBox::warning(this,tr("提示"), tr("请先选择一条隧道！"));
                return;
            }
            int i = ui->actualTasksWidget->currentRow();//获得当前行的索引
            // 下面发送至丛控的代码得改，发送到9个丛控应该先计算哪一条隧道
            int tunnelid = ui->actualTasksWidget->item(i, CHECKEDTASK_TUNNELID)->data(Qt::DisplayRole).toInt();
            if (backuptype == 0)
            {
                QString totalseqnostr = ui->actualTasksWidget->item(i, CHECKEDTASK_SEQNO_TOTAL)->data(Qt::DisplayRole).toString();
                qDebug() << (QString(tr("要求从控备份第%1条隧道，隧道ID为%2，隧道采集序列号范围为%,3")).arg(i).arg(tunnelid).arg(totalseqnostr));
                MasterProgram::getMasterProgramInstance()->backup_beginStartOneTunnel(tunnelid);
                ui->startBackupButton->setEnabled(false);
                ui->stopBackupButton->setEnabled(true);
            }
            else
            {
                MasterProgram::getMasterProgramInstance()->backup_Master_beginStartOneTunnel(tunnelid);
                ui->startMasterBackupButton->setEnabled(false);
                ui->stopMasterBackupButton->setEnabled(true);
            }
        }
    }
}
