#include "status_calcu.h"
#include "ui_status_calcu.h"

#include <QTextCodec>
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

/**
 * 监控界面——计算界面类定义
 * @author 熊雪
 * @version 1.0.0
 * @date 2014-03-01
 */
CalcuWidget::CalcuWidget(QWidget *parent) :
    QWidget(parent), currentProjectModel(1.0),
    ui(new Ui::CalcuWidget)
{
    ui->setupUi(this);

    // 初始时未导入工程
    hasinintproject = false;

    setOptButtonEnable(false);
	
	// 范翔定义新按钮，从融合fuse结果计算syn提取高度结果
	ui->startExtractHeightButton->setVisible(false);

    QObject::connect(ui->resetButton, SIGNAL(clicked()), this, SLOT(resetCalcuConfigFile()));
}

CalcuWidget::~CalcuWidget()
{
    delete ui;
}

void CalcuWidget::saveResetFile()
{

    QMessageBox msgBox;
    msgBox.setText(tr("提示"));
    msgBox.setInformativeText(tr("我还没写好，敬请期待！"));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    int ret = msgBox.exec();

    // void CheckTaskWidget::resetCheckedFile()
    /*

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
    }*/
}

void CalcuWidget::resetCalcuConfigFile()
{
    LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Calculate).resetCalcuPos();

    // 重新覆盖原文件
    saveResetFile();
}

void CalcuWidget::on_startCalcuButton_clicked()
{
    SelectCalcuMode(0);
}

void CalcuWidget::on_startFuseButton_clicked()
{
    SelectCalcuMode(1);
}

void CalcuWidget::on_stopCalcuButton_clicked()
{
    ui->stopCalcuButton->setEnabled(false);
}

void CalcuWidget::on_stopFuseButton_clicked()
{
    ui->stopFuseButton->setEnabled(false);
}

void CalcuWidget::on_startExtractHeightButton_clicked()
{
	SelectCalcuMode(2);
}

void CalcuWidget::changeSlaveTask(WorkingStatus status, QString index, int threadid, QString msgtask, QString remark)
{
    if (status == WorkingStatus::Calculating)
    {
        int slaveid = index.toInt();
        QLabel * tmplabelcalcuremark = NULL;
        QLabel * tmplabelcalcuremark_thread1 = NULL;
        QLabel * tmplabelcalcutask_thread1 = NULL;
        QLabel * tmplabelcalcuremark_thread2 = NULL;
        QLabel * tmplabelcalcutask_thread2 = NULL;
        QProgressBar * tmpprogressbar_thread1 = NULL;
        QProgressBar * tmpprogressbar_thread2 = NULL;

        if (threadid != 0)
        {
            if (remark.compare("ok") == 0)
            {
                switch (slaveid)
                {
                    case 0: tmpprogressbar_thread1 = ui->progressBar_cal01; break;
                    case 1: tmpprogressbar_thread1 = ui->progressBar_cal11; tmpprogressbar_thread2 = ui->progressBar_cal12; break;
                    case 2: tmpprogressbar_thread1 = ui->progressBar_cal21; tmpprogressbar_thread2 = ui->progressBar_cal22; break;
                    case 3: tmpprogressbar_thread1 = ui->progressBar_cal31; tmpprogressbar_thread2 = ui->progressBar_cal32; break;
                    case 4: tmpprogressbar_thread1 = ui->progressBar_cal41; tmpprogressbar_thread2 = ui->progressBar_cal42; break;
                    case 5: tmpprogressbar_thread1 = ui->progressBar_cal51; tmpprogressbar_thread2 = ui->progressBar_cal52; break;
                    case 6: tmpprogressbar_thread1 = ui->progressBar_cal61; tmpprogressbar_thread2 = ui->progressBar_cal62; break;
                    case 7: tmpprogressbar_thread1 = ui->progressBar_cal71; tmpprogressbar_thread2 = ui->progressBar_cal72; break;
                    case 8: tmpprogressbar_thread1 = ui->progressBar_cal81; tmpprogressbar_thread2 = ui->progressBar_cal82; break;
                    case 9: tmpprogressbar_thread1 = ui->progressBar_cal91; tmpprogressbar_thread2 = ui->progressBar_cal92; break;
                    default: break;
                }
            }

            switch (slaveid)
            {
                case 0: tmplabelcalcutask_thread1 = ui->label_calmaster; tmplabelcalcuremark_thread1 = ui->label_calmaster_remark; break;// 主控
                case 1: tmplabelcalcutask_thread1 = ui->label_cal11; tmplabelcalcuremark_thread1 = ui->label_cal11_remark; tmplabelcalcutask_thread2 = ui->label_cal12; tmplabelcalcuremark_thread2 = ui->label_cal12_remark; break;
                case 2: tmplabelcalcutask_thread1 = ui->label_cal21; tmplabelcalcuremark_thread1 = ui->label_cal21_remark; tmplabelcalcutask_thread2 = ui->label_cal22; tmplabelcalcuremark_thread2 = ui->label_cal22_remark; break;
                case 3: tmplabelcalcutask_thread1 = ui->label_cal31; tmplabelcalcuremark_thread1 = ui->label_cal31_remark; tmplabelcalcutask_thread2 = ui->label_cal32; tmplabelcalcuremark_thread2 = ui->label_cal32_remark; break;
                case 4: tmplabelcalcutask_thread1 = ui->label_cal41; tmplabelcalcuremark_thread1 = ui->label_cal41_remark; tmplabelcalcutask_thread2 = ui->label_cal42; tmplabelcalcuremark_thread2 = ui->label_cal42_remark; break;
                case 5: tmplabelcalcutask_thread1 = ui->label_cal51; tmplabelcalcuremark_thread1 = ui->label_cal51_remark; tmplabelcalcutask_thread2 = ui->label_cal52; tmplabelcalcuremark_thread2 = ui->label_cal52_remark; break;
                case 6: tmplabelcalcutask_thread1 = ui->label_cal61; tmplabelcalcuremark_thread1 = ui->label_cal61_remark; tmplabelcalcutask_thread2 = ui->label_cal62; tmplabelcalcuremark_thread2 = ui->label_cal62_remark; break;
                case 7: tmplabelcalcutask_thread1 = ui->label_cal71; tmplabelcalcuremark_thread1 = ui->label_cal71_remark; tmplabelcalcutask_thread2 = ui->label_cal72; tmplabelcalcuremark_thread2 = ui->label_cal72_remark; break;
                case 8: tmplabelcalcutask_thread1 = ui->label_cal81; tmplabelcalcuremark_thread1 = ui->label_cal81_remark; tmplabelcalcutask_thread2 = ui->label_cal82; tmplabelcalcuremark_thread2 = ui->label_cal82_remark; break;
                case 9: tmplabelcalcutask_thread1 = ui->label_cal91; tmplabelcalcuremark_thread1 = ui->label_cal91_remark; tmplabelcalcutask_thread2 = ui->label_cal92; tmplabelcalcuremark_thread2 = ui->label_cal92_remark; break;
                default: break;
            }
            if (threadid == 1)
            {
                tmplabelcalcutask_thread1->setText(msgtask);
                tmplabelcalcuremark_thread1->setText(remark);
                if (remark.compare("ok") == 0)
                    tmpprogressbar_thread1->setValue(tmpprogressbar_thread1->maximum());
            }
            else if (threadid == 2)
            {
                tmplabelcalcutask_thread2->setText(msgtask);
                tmplabelcalcuremark_thread2->setText(remark);
                if (remark.compare("ok") == 0)
                    tmpprogressbar_thread2->setValue(tmpprogressbar_thread2->maximum());
            }
        }
        else
            qDebug() << QObject::tr("无法显示的计算消息：从控%1的计算0线程当前任务%2出现提示%3").arg(slaveid).arg(msgtask).arg(remark);

    }
}

void CalcuWidget::changeSlaveCameraTaskFC_init(WorkingStatus status, QString index, int threadid, QString msgtask, long long startfc, long long endfc)
{
    if (status == WorkingStatus::Calculating)
    {
        int slaveid = index.toInt();
        QLabel * tmplabelcalcuremark = NULL;
        QLabel * tmplabelcalcutask_thread1 = NULL;
        QLabel * tmplabelcalcutask_thread2 = NULL;
        QProgressBar * tmpprogressbar_thread1 = NULL;
        QProgressBar * tmpprogressbar_thread2 = NULL;

        if (threadid != 0)
        {
            switch (slaveid)
            {
                case 0: tmplabelcalcutask_thread1 = ui->label_calmaster; tmpprogressbar_thread1 = ui->progressBar_cal01; break;// 主控
                case 1: tmplabelcalcutask_thread1 = ui->label_cal11; tmpprogressbar_thread1 = ui->progressBar_cal11; tmplabelcalcutask_thread2 = ui->label_cal12; tmpprogressbar_thread2 = ui->progressBar_cal12; break;
                case 2: tmplabelcalcutask_thread1 = ui->label_cal21; tmpprogressbar_thread1 = ui->progressBar_cal21; tmplabelcalcutask_thread2 = ui->label_cal22; tmpprogressbar_thread2 = ui->progressBar_cal22; break;
                case 3: tmplabelcalcutask_thread1 = ui->label_cal31; tmpprogressbar_thread1 = ui->progressBar_cal31; tmplabelcalcutask_thread2 = ui->label_cal32; tmpprogressbar_thread2 = ui->progressBar_cal32; break;
                case 4: tmplabelcalcutask_thread1 = ui->label_cal41; tmpprogressbar_thread1 = ui->progressBar_cal41; tmplabelcalcutask_thread2 = ui->label_cal42; tmpprogressbar_thread2 = ui->progressBar_cal42; break;
                case 5: tmplabelcalcutask_thread1 = ui->label_cal51; tmpprogressbar_thread1 = ui->progressBar_cal51; tmplabelcalcutask_thread2 = ui->label_cal52; tmpprogressbar_thread2 = ui->progressBar_cal52; break;
                case 6: tmplabelcalcutask_thread1 = ui->label_cal61; tmpprogressbar_thread1 = ui->progressBar_cal61; tmplabelcalcutask_thread2 = ui->label_cal62; tmpprogressbar_thread2 = ui->progressBar_cal62; break;
                case 7: tmplabelcalcutask_thread1 = ui->label_cal71; tmpprogressbar_thread1 = ui->progressBar_cal71; tmplabelcalcutask_thread2 = ui->label_cal72; tmpprogressbar_thread2 = ui->progressBar_cal72; break;
                case 8: tmplabelcalcutask_thread1 = ui->label_cal81; tmpprogressbar_thread1 = ui->progressBar_cal81; tmplabelcalcutask_thread2 = ui->label_cal82; tmpprogressbar_thread2 = ui->progressBar_cal82; break;
                case 9: tmplabelcalcutask_thread1 = ui->label_cal91; tmpprogressbar_thread1 = ui->progressBar_cal91; tmplabelcalcutask_thread2 = ui->label_cal92; tmpprogressbar_thread2 = ui->progressBar_cal92; break;
                default: break;
            }
            if (threadid == 1)
            {
                tmplabelcalcutask_thread1->setText(msgtask);
                tmpprogressbar_thread1->setMinimum(startfc);
                tmpprogressbar_thread1->setMaximum(endfc);
                tmpprogressbar_thread1->setValue(startfc);
            }
            else if (threadid == 2)
            {
                tmplabelcalcutask_thread2->setText(msgtask);
                tmpprogressbar_thread2->setMinimum(startfc);
                tmpprogressbar_thread2->setMaximum(endfc);
                tmpprogressbar_thread2->setValue(startfc);
            }
        }
        else
            qDebug() << QObject::tr("无法显示的计算消息：从控%1的计算0线程当前任务%2初始化，计算帧范围%3-%4").arg(slaveid).arg(msgtask).arg(startfc).arg(endfc);

    }
}

void CalcuWidget::changeSlaveCameraTaskFC(WorkingStatus status, QString index, int threadid, QString msgtask, long long currentfc)
{
    if (status == WorkingStatus::Calculating)
    {
        int slaveid = index.toInt();
        QLabel * tmplabelcalcuremark = NULL;
        QLabel * tmplabelcalcutask_thread1 = NULL;
        QLabel * tmplabelcalcutask_thread2 = NULL;
        QProgressBar * tmpprogressbar_thread1 = NULL;
        QProgressBar * tmpprogressbar_thread2 = NULL;

        if (threadid != 0)
        {
            switch (slaveid)
            {
                case 0: tmplabelcalcutask_thread1 = ui->label_calmaster; tmpprogressbar_thread1 = ui->progressBar_cal01; break;// 主控
                case 1: tmplabelcalcutask_thread1 = ui->label_cal11; tmpprogressbar_thread1 = ui->progressBar_cal11; tmplabelcalcutask_thread2 = ui->label_cal12; tmpprogressbar_thread2 = ui->progressBar_cal12; break;
                case 2: tmplabelcalcutask_thread1 = ui->label_cal21; tmpprogressbar_thread1 = ui->progressBar_cal21; tmplabelcalcutask_thread2 = ui->label_cal22; tmpprogressbar_thread2 = ui->progressBar_cal22; break;
                case 3: tmplabelcalcutask_thread1 = ui->label_cal31; tmpprogressbar_thread1 = ui->progressBar_cal31; tmplabelcalcutask_thread2 = ui->label_cal32; tmpprogressbar_thread2 = ui->progressBar_cal32; break;
                case 4: tmplabelcalcutask_thread1 = ui->label_cal41; tmpprogressbar_thread1 = ui->progressBar_cal41; tmplabelcalcutask_thread2 = ui->label_cal42; tmpprogressbar_thread2 = ui->progressBar_cal42; break;
                case 5: tmplabelcalcutask_thread1 = ui->label_cal51; tmpprogressbar_thread1 = ui->progressBar_cal51; tmplabelcalcutask_thread2 = ui->label_cal52; tmpprogressbar_thread2 = ui->progressBar_cal52; break;
                case 6: tmplabelcalcutask_thread1 = ui->label_cal61; tmpprogressbar_thread1 = ui->progressBar_cal61; tmplabelcalcutask_thread2 = ui->label_cal62; tmpprogressbar_thread2 = ui->progressBar_cal62; break;
                case 7: tmplabelcalcutask_thread1 = ui->label_cal71; tmpprogressbar_thread1 = ui->progressBar_cal71; tmplabelcalcutask_thread2 = ui->label_cal72; tmpprogressbar_thread2 = ui->progressBar_cal72; break;
                case 8: tmplabelcalcutask_thread1 = ui->label_cal81; tmpprogressbar_thread1 = ui->progressBar_cal81; tmplabelcalcutask_thread2 = ui->label_cal82; tmpprogressbar_thread2 = ui->progressBar_cal82; break;
                case 9: tmplabelcalcutask_thread1 = ui->label_cal91; tmpprogressbar_thread1 = ui->progressBar_cal91; tmplabelcalcutask_thread2 = ui->label_cal92; tmpprogressbar_thread2 = ui->progressBar_cal92; break;
                default: break;
            }
            if (threadid == 1)
            {
                tmplabelcalcutask_thread1->setText(msgtask);
                tmpprogressbar_thread1->setValue(currentfc);
            }
            else if (threadid == 2)
            {
                tmplabelcalcutask_thread2->setText(msgtask);
                tmpprogressbar_thread2->setValue(currentfc);
            }
        }
        else
            qDebug() << QObject::tr("无法显示的计算消息：从控%1的计算0线程当前任务%2执行到第%3帧").arg(slaveid).arg(msgtask).arg(currentfc);

    }
}

/**
 * 修改进度条槽函数
 */
void CalcuWidget::changecalcubackupProgressingBar(WorkingStatus status, QString slaveidstr, int threadid, int totalnum, int currentnum, bool isupdate)
{
    int slaveid = slaveidstr.toInt();
    if (!isupdate) // 设置初始进度
    {
        if (status == WorkingStatus::Calculating)
        {
            switch (slaveid)
            {
                case 0: ui->progressBar_calmaster->setMaximum(totalnum); ui->progressBar_calmaster->setValue(currentnum); break;// 主控
                case 1: ui->progressBar_cal1->setMaximum(totalnum); ui->progressBar_cal1->setValue(currentnum); break;
                case 2: ui->progressBar_cal2->setMaximum(totalnum); ui->progressBar_cal2->setValue(currentnum); break;
                case 3: ui->progressBar_cal3->setMaximum(totalnum); ui->progressBar_cal3->setValue(currentnum); break;
                case 4: ui->progressBar_cal4->setMaximum(totalnum); ui->progressBar_cal4->setValue(currentnum); break;
                case 5: ui->progressBar_cal5->setMaximum(totalnum); ui->progressBar_cal5->setValue(currentnum); break;
                case 6: ui->progressBar_cal6->setMaximum(totalnum); ui->progressBar_cal6->setValue(currentnum); break;
                case 7: ui->progressBar_cal7->setMaximum(totalnum); ui->progressBar_cal7->setValue(currentnum); break;
                case 8: ui->progressBar_cal8->setMaximum(totalnum); ui->progressBar_cal8->setValue(currentnum); break;
                case 9: ui->progressBar_cal9->setMaximum(totalnum); ui->progressBar_cal9->setValue(currentnum); break;
                default: break;
            }
        }
    }
    else // 更新值
    {
        int tmpval = 0;
        if (status == WorkingStatus::Calculating)
        {
            switch (slaveid)
            {
                case 0: tmpval = ui->progressBar_calmaster->value(); ui->progressBar_calmaster->setValue(++tmpval); break;// 主控
                case 1: tmpval = ui->progressBar_cal1->value(); ui->progressBar_cal1->setValue(++tmpval); break;
                case 2: tmpval = ui->progressBar_cal2->value(); ui->progressBar_cal2->setValue(++tmpval); break;
                case 3: tmpval = ui->progressBar_cal3->value(); ui->progressBar_cal3->setValue(++tmpval); break;
                case 4: tmpval = ui->progressBar_cal4->value(); ui->progressBar_cal4->setValue(++tmpval); break;
                case 5: tmpval = ui->progressBar_cal5->value(); ui->progressBar_cal5->setValue(++tmpval); break;
                case 6: tmpval = ui->progressBar_cal6->value(); ui->progressBar_cal6->setValue(++tmpval); break;
                case 7: tmpval = ui->progressBar_cal7->value(); ui->progressBar_cal7->setValue(++tmpval); break;
                case 8: tmpval = ui->progressBar_cal8->value(); ui->progressBar_cal8->setValue(++tmpval); break;
                case 9: tmpval = ui->progressBar_cal9->value(); ui->progressBar_cal9->setValue(++tmpval); break;
                default: break;
            }
        }
    }
}

// 加载计划任务中的隧道列表
void CalcuWidget::updateCheckedTunnelView()
{
    currentProjectModel = LzProjectAccess::getLzProjectAccessInstance()->getProjectModel(LzProjectClass::Calculate);
 
    if (currentProjectModel == NULL)
    {
        QMessageBox::warning(this, tr("工程导入"), tr("工程入口文件导入失败"));
        hasinintproject = false;
        return;
    }
    hasinintproject = true;

    loadCheckTaskTunnelData();
}

void CalcuWidget::loadCheckTaskTunnelData()
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

void CalcuWidget::updateCheckedTaskWidget()
{
    if (!hasinintproject)
        return;

    QString path = LzProjectAccess::getLzProjectAccessInstance()->getProjectPath(LzProjectClass::Calculate);
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
    XMLRealTaskFileLoader * realtask = new XMLRealTaskFileLoader(tr(realFile.toLocal8Bit().data()));
    ret = realtask->loadFile(LzProjectAccess::getLzProjectAccessInstance()->getLzRealList(LzProjectClass::Calculate));//生成listtunnelcheck
    delete realtask;
    if (realFile.compare("") == 0 || ret == false)
    {
        QMessageBox::information(this,tr("导入工程错误"),tr("该工程下实际采集文件还未生成%1。").arg(realFile),QMessageBox::Yes|QMessageBox::No);
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

void CalcuWidget::setOptButtonEnable(bool initstatus)
{
    isincalculatebackupstatus = initstatus;

    if (initstatus && ui->actualTasksWidget->rowCount() >= 1)
    {
        ui->startCalcuButton->setEnabled(true);
        ui->stopCalcuButton->setEnabled(false);
        ui->startFuseButton->setEnabled(true);
        ui->stopFuseButton->setEnabled(false);
    }
    else
    {
        ui->startCalcuButton->setEnabled(false);
        ui->stopCalcuButton->setEnabled(false);
        ui->startFuseButton->setEnabled(false);
        ui->stopFuseButton->setEnabled(false);
    }
}

void CalcuWidget::SelectCalcuMode(int calcutype)
{
    bool line = ui->line_radioButton->isChecked();
    bool tunnel = ui->tunnel_radioButton->isChecked();
    if ((line == false) && (tunnel == false))
    {
        QMessageBox::information(this,tr("提示"),tr("请先选择计算模式"),QMessageBox::Yes);
        return;
    }
    else
    {
        if (line == true) // 按线路
        {
            if (calcutype == 0)
            {
                calculate_beginStartAll();
                ui->startCalcuButton->setEnabled(false);
                ui->stopCalcuButton->setEnabled(true);
            }
            else if (calcutype == 1)
            {
                fuse_beginStartAll();
                ui->startFuseButton->setEnabled(false);
                ui->stopFuseButton->setEnabled(true);
            }
			else if (calcutype == 2)
			{
			    extract_beginStartAll();
                //ui->startFuseButton->setEnabled(false);
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
            if (calcutype == 0)
            {
                qDebug() << (QString(tr("要求从控计算第%1条隧道，隧道ID为%2")).arg(i).arg(tunnelid));
                calculate_beginStartOneTunnel(tunnelid);
                ui->startCalcuButton->setEnabled(false);
                ui->stopCalcuButton->setEnabled(true);
            }
            else if (calcutype == 1)
            {
                qDebug() << (QString(tr("要求主控融合计算第%1条隧道，隧道ID为%2")).arg(i).arg(tunnelid));
                fuse_beginStartOneTunnel(tunnelid);
                ui->startFuseButton->setEnabled(false);
                ui->stopFuseButton->setEnabled(true);
            }
			else if (calcutype == 2)
			{
				qDebug() << (QString(tr("要求主控提高度计算第%1条隧道，隧道ID为%2")).arg(i).arg(tunnelid));
                extract_beginStartOneTunnel(tunnelid);
			}
        }
    }
}
