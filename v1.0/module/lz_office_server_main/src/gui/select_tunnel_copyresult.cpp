#include "select_tunnel_copyresult.h"
#include "ui_select_tunnel_copyresult.h"

#include <QDebug>
#include <QFileDialog>
#include <QSqlRelationalDelegate>
#include <QMessageBox>
#include <QPalette>
#include <QTime>
#include <QDateTime>

#include "QStandardItemModel"
#include "QTableWidgetItem"
#include <QTextCodec>

#include "setting_server.h"
#include "serverprogram.h"

#include "daotask.h"
#include "daotasktunnel.h"

#include "lz_project_access.h"

#include "realtask_list.h"
#include "checkedtask_list.h"
#include "xmlrealtaskfileloader.h"
#include "xmlcheckedtaskfileloader.h"

#include "fileoperation_nascopyfilter.h"

/**
 * 可修正综合隧道选择（图形修正、隧道综合之前）界面类定义
 *
 * @author 范翔
 * @version 1.0.0
 * @date 20140222
 */
CopyAvaliableTunnelWidget::CopyAvaliableTunnelWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CopyAvaliableTunnelWidget)
{
    ui->setupUi(this);
    QTextCodec::setCodecForTr(QTextCodec::codecForLocale());
    //imageopen =NULL;

    currentRow = -1;

    projectname = "";

    loadAvaliableLines();

    connect(ui->refreshButton, SIGNAL(clicked()), this, SLOT(refreshAvalibleLinesData()));

    /* 线路及隧道数据列表更新信号槽 */
    connect(ui->linesView, SIGNAL(clicked(QModelIndex)), this, SLOT(updateCheckedTunnelsView(const QModelIndex &)));

    // 拷贝按钮
    connect(ui->copyOneButton, SIGNAL(clicked()), this, SLOT(copyOneProject()));
    connect(ui->copyAllButton, SIGNAL(clicked()), this, SLOT(copyAllProject()));

}

CopyAvaliableTunnelWidget::~CopyAvaliableTunnelWidget()
{
    delete ui;
}

void CopyAvaliableTunnelWidget::refreshAvalibleLinesData()
{
    // 等待一段时间后查看
    loadAvaliableLines();
}

void CopyAvaliableTunnelWidget::loadAvaliableLines()
{
    linesModel = ServerSetting::getSettingInstance()->getEditableLineNames();
    if (linesModel != NULL)
    {
        ui->linesView->setModel(linesModel);
        ui->linesView->setAutoScroll(true);
        ui->linesView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        // 【标记1】初始设置选中第一行，后边需要根据这个信息得到当前线路名称
        QModelIndex index = linesModel->index(0, 0);
        ui->linesView->setCurrentIndex(index);

        if (linesModel->rowCount() > 0)
            currentRow = 0;
        else
            currentRow = -1;

    }
    else
        ui->textEdit->setText(tr("当前没有可编辑的线路任务！"));
}

void CopyAvaliableTunnelWidget::updateCheckedTunnelsView(const QModelIndex &index)
{
    if (index.isValid())
    {
        projectname = index.data().toString();
        currentRow = index.row();

        qDebug() << projectname;
        if (projectname.compare("") != 0)
        {
            QString projectfilename = projectname + ".proj";
            LzProjectAccess::getLzProjectAccessInstance()->setProjectName(LzProjectClass::Main, ServerSetting::getSettingInstance()->getNASAddress() + "/" + projectname + "/" + projectfilename);
            LzProjectAccess::getLzProjectAccessInstance()->setProjectPath(LzProjectClass::Main, ServerSetting::getSettingInstance()->getNASAddress() + "/" + projectname);

            bool ret = loadCheckedTaskTunnelData();
          	if (ret)
            {
                QString checkedFile = LzProjectAccess::getLzProjectAccessInstance()->getProjectPath(LzProjectClass::Main) + "/" + LzProjectAccess::getLzProjectAccessInstance()->getProjectModel(LzProjectClass::Main).getCheckedFilename();
                ui->textEdit->setText(QObject::tr("打开校正隧道文件%1成功。").arg(checkedFile));
            }
        }
    }
    else
    {
        currentRow = -1;
        ui->textEdit->setText(QObject::tr("没有指定校正隧道文件，请刷新后重试！"));
    }
}

bool CopyAvaliableTunnelWidget::loadCheckedTaskTunnelData()//添加实际隧道文件
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
    ui->actualTasksWidget->setHorizontalHeaderItem(CHECKEDTASK_ENDFRAME, new QTableWidgetItem(QObject::tr("终止帧数")));
    ui->actualTasksWidget->setHorizontalHeaderItem(CHECKEDTASK_STARTFRAME, new QTableWidgetItem(QObject::tr("起始帧数")));
    ui->actualTasksWidget->setHorizontalHeaderItem(CHECKEDTASK_ENDFRAME, new QTableWidgetItem(QObject::tr("终止帧数")));
    ui->actualTasksWidget->setHorizontalHeaderItem(CHECKEDTASK_TOTALFRAME, new QTableWidgetItem(QObject::tr("总帧数")));

    //将actualTasksWidget设置为整行选中的话，则点击每个单元格都是整行选中，如果设置为选中单个目标的话，则每次只能点击一个单元格，任何选择的内容都不设置的话，则两者都可以选择。
    ui->actualTasksWidget->setSelectionBehavior(QAbstractItemView::SelectRows);  //整行选中的方式
    ui->actualTasksWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);   //表格禁止编辑

    return updateCheckedTaskWidget();
}

bool CopyAvaliableTunnelWidget::updateCheckedTaskWidget()
{
    // 引用赋值
    CheckedTaskList & clist = LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Main);

    //从实际隧道文件的的xml中解析实际隧道数据和校正之后的数据到listcollect中
	QString checkedFile = LzProjectAccess::getLzProjectAccessInstance()->getProjectPath(LzProjectClass::Main) + "/" + LzProjectAccess::getLzProjectAccessInstance()->getProjectModel(LzProjectClass::Main).getCheckedFilename();
    XMLCheckedTaskFileLoader * checktask = new XMLCheckedTaskFileLoader(tr(checkedFile.toLocal8Bit().data()));
    bool ret = checktask->loadFile(clist);//生成listtask
    delete checktask;
    //bool ret = false;
    if(ret == false)
    {
        qDebug() << tr("不能加载校正任务文件--- checkedtaskfile");
        ui->textEdit->setText(tr("不能加载校正任务文件--- checkedtaskfile"));
        return false;
    }

    clist.showList();

    // 加载校正任务记录到widget中
    QList<CheckedTunnelTaskModel>::iterator checkedTaskIterator = clist.begin();
    while (checkedTaskIterator != clist.end())
    {
        //qDebug() << "gui show checked" << checkedTaskIterator->planTask.tunnelnum << QString::fromLocal8Bit(checkedTaskIterator->planTask.tunnelname.c_str());

        //动态设置QTabwidget的行数
        int row = ui->actualTasksWidget->rowCount();
        ui->actualTasksWidget->setRowCount(row + 1);
        //qDebug()<<"row:"<<row;
        ui->actualTasksWidget->setItem(row, CHECKEDTASK_TUNNELID, new QTableWidgetItem(QString("%1").arg(checkedTaskIterator->planTask.tunnelnum)));
        ui->actualTasksWidget->setItem(row, CHECKEDTASK_TUNNELNAME, new QTableWidgetItem(QString::fromLocal8Bit(checkedTaskIterator->planTask.tunnelname.c_str())));

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

        // qDebug() << checkedstart << checkedend;
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

        checkedTaskIterator++;
    }

    ui->actualTasksWidget->resizeColumnsToContents();
    ui->actualTasksWidget->horizontalHeader()->setStretchLastSection(false);
    return true;
}

void CopyAvaliableTunnelWidget::copyOneProject()
{
    if (currentRow < 0)
    {
        ui->textEdit->setText(QObject::tr("请先选择NAS上的一个工程。"));
        return;
    }

    QString frompath = LzProjectAccess::getLzProjectAccessInstance()->getProjectPath(LzProjectClass::Main);
    QString topath = ServerSetting::getSettingInstance()->getParentPath() + "/" + projectname;

    qDebug() << "from:" << frompath << ", to:" << topath;

    LocalFileOperation_NASCopyFilter::copyDirectoryFiles(frompath, topath, true);
}

void CopyAvaliableTunnelWidget::copyAllProject()
{
    if (linesModel->rowCount() < 0)
    {
        ui->textEdit->setText(QObject::tr("当前NAS上没人任何工程需要拷贝。"));
        return;
    }

    for (int i = 0; i < linesModel->rowCount(); i++)
    {
        QString projname = linesModel->index(i, 0).data().toString();
        QString frompath = ServerSetting::getSettingInstance()->getNASAddress() + "/" + projname;
        QString topath = ServerSetting::getSettingInstance()->getParentPath() + "/" + projname;

        qDebug() << "from:" << frompath << ", to:" << topath;
        LocalFileOperation_NASCopyFilter::copyDirectoryFiles(frompath, topath, true);
    }
}