#include "select_tunnel.h"
#include "ui_select_tunnel.h"

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

#include "setting_client.h"
#include "clientprogram.h"

#include "daotask.h"
#include "daotasktunnel.h"

#include "lz_project_access.h"
#include "checkedtask_list.h"
#include "xmlcheckedtaskfileloader.h"

/**
 * 可修正综合隧道选择（图形修正、隧道综合之前）界面类定义
 *
 * @author 范翔
 * @version 1.0.0
 * @date 20140222
 */
SelectAvaliableTunnelWidget::SelectAvaliableTunnelWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SelectAvaliableTunnelWidget)
{
    ui->setupUi(this);
    QTextCodec::setCodecForTr(QTextCodec::codecForLocale());
    //imageopen =NULL;

    openvalidframes=NULL;

    projectname = "";

    loadAvaliableLines();

    connect(ui->refreshButton, SIGNAL(clicked()), this, SLOT(refreshAvalibleLinesData()));

    /* 线路及隧道数据列表更新信号槽 */
    connect(ui->linesView, SIGNAL(clicked(QModelIndex)), this, SLOT(updateCheckedTunnelsView(const QModelIndex &)));

	// 显示client的message
	connect(ClientProgram::getClientProgramInstance(), SIGNAL(signalMsgToGUI(QString)), this, SLOT(appendMsg(QString)));

    // 日志类
    logger = NULL;
    hasinitlog = false;
}

SelectAvaliableTunnelWidget::~SelectAvaliableTunnelWidget()
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

    if (openvalidframes != NULL)
        delete openvalidframes;
}

// 显示clientprogram的消息
void SelectAvaliableTunnelWidget::appendMsg(QString newmsg)
{
    ui->textEdit->setText(newmsg);
}

void SelectAvaliableTunnelWidget::refreshAvalibleLinesData()
{
    ClientProgram::getClientProgramInstance()->askForAvalibleDirectory();
    
    // 等待一段时间后查看
    loadAvaliableLines();
}

void SelectAvaliableTunnelWidget::loadAvaliableLines()
{
    linesModel = ClientSetting::getSettingInstance()->getEditableLineNames();
    if (linesModel != NULL)
    {
        ui->linesView->setModel(linesModel);
        ui->linesView->setAutoScroll(true);
        ui->linesView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        // 【标记1】初始设置选中第一行，后边需要根据这个信息得到当前线路名称
        QModelIndex index = linesModel->index(0, 0);
        ui->linesView->setCurrentIndex(index);
    }
    else
        ui->textEdit->setText(tr("当前没有可编辑的线路任务！"));
}

void SelectAvaliableTunnelWidget::updateCheckedTunnelsView(const QModelIndex &index)
{
    if (index.isValid())
    {
        projectname = index.data().toString();
        qDebug() << projectname;
        if (projectname.compare("") != 0)
        {
            QString projectfilename = projectname + ".proj";
            LzProjectAccess::getLzProjectAccessInstance()->setProjectName(LzProjectClass::Main, ClientSetting::getSettingInstance()->getParentPath() + "/" + projectname + "/" + projectfilename);
            LzProjectAccess::getLzProjectAccessInstance()->setProjectPath(LzProjectClass::Main, ClientSetting::getSettingInstance()->getParentPath() + "/" + projectname);

            bool ret = loadCheckedTaskTunnelData();
          	if (ret)
			{
				QString checkedFile = LzProjectAccess::getLzProjectAccessInstance()->getProjectPath(LzProjectClass::Main) + "/" + LzProjectAccess::getLzProjectAccessInstance()->getProjectModel(LzProjectClass::Main).getCheckedFilename();
				ui->textEdit->setText(QObject::tr("打开校正隧道文件%1成功。").arg(checkedFile));
			}
        }
    }
    else
        ui->textEdit->setText(QObject::tr("没有指定校正隧道文件，请刷新后重试！"));
}

bool SelectAvaliableTunnelWidget::loadCheckedTaskTunnelData()//添加实际隧道文件
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
    ui->actualTasksWidget->setHorizontalHeaderItem(CHECKEDTASK_STARTMILE, new QTableWidgetItem(QObject::tr("起始里程")));
    ui->actualTasksWidget->setHorizontalHeaderItem(CHECKEDTASK_ENDMILE, new QTableWidgetItem(QObject::tr("终止里程")));
    ui->actualTasksWidget->setHorizontalHeaderItem(CHECKEDTASK_STARTFRAME, new QTableWidgetItem(QObject::tr("起始帧数")));
    ui->actualTasksWidget->setHorizontalHeaderItem(CHECKEDTASK_ENDFRAME, new QTableWidgetItem(QObject::tr("终止帧数")));
    ui->actualTasksWidget->setHorizontalHeaderItem(CHECKEDTASK_TOTALFRAME, new QTableWidgetItem(QObject::tr("总帧数")));
    ui->actualTasksWidget->setHorizontalHeaderItem(CHECKEDTASK_APPLYEDIT, new QTableWidgetItem(QObject::tr("申请修正")));
    ui->actualTasksWidget->setHorizontalHeaderItem(CHECKEDTASK_CHECKVALIDFRAMES, new QTableWidgetItem(QObject::tr("设置有效帧范围")));
    //ui->actualTasksWidget->hideColumn(CHECKEDTASK_TUNNELID);

    //将actualTasksWidget设置为整行选中的话，则点击每个单元格都是整行选中，如果设置为选中单个目标的话，则每次只能点击一个单元格，任何选择的内容都不设置的话，则两者都可以选择。
    ui->actualTasksWidget->setSelectionBehavior(QAbstractItemView::SelectRows);  //整行选中的方式
    ui->actualTasksWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);   //表格禁止编辑

    return updateCheckedTaskWidget();
}

bool SelectAvaliableTunnelWidget::updateCheckedTaskWidget()
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
        ui->actualTasksWidget->setItem(row, CHECKEDTASK_STARTMILE, new QTableWidgetItem(QString("%1").arg(milestart)));
        ui->actualTasksWidget->setItem(row, CHECKEDTASK_ENDMILE, new QTableWidgetItem(QString("%1").arg(mileend)));
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

        //把新建按钮放在循环之外，则只能最后一行显示按钮，因为按钮是new的，所以只有一个，所以需要放在循环中，建一个存一个。
        QPushButton * imageviewerbutton = new QPushButton();
        imageviewerbutton->setText(tr("申请编辑"));
        //设置按钮信号槽函数
        connect(imageviewerbutton, SIGNAL(clicked()), this, SLOT(askforEditableTunnel()));
        ui->actualTasksWidget->setCellWidget(row, CHECKEDTASK_APPLYEDIT, imageviewerbutton);

        // 设置按钮
        QPushButton * valid_frames_button =new QPushButton();
        valid_frames_button->setText(tr("设置有效帧范围"));
        // 设置按钮信号槽函数
        ui->actualTasksWidget->setCellWidget(row,CHECKEDTASK_CHECKVALIDFRAMES,valid_frames_button);
        connect(valid_frames_button,SIGNAL(clicked()),this,SLOT(opensetvalidframes()));


        checkedTaskIterator++;
    }

    ui->actualTasksWidget->resizeColumnsToContents();
    ui->actualTasksWidget->horizontalHeader()->setStretchLastSection(false);
	return true;
}

void SelectAvaliableTunnelWidget::askforEditableTunnel()
{
    //@author 熊雪
    //下面这几行代码可以在不选中行的情况下点击原图按钮获得正确的当前行，而不是-1
    QPushButton *pbt = dynamic_cast<QPushButton*>(this->sender());
    if(pbt == 0)
    return;
    int x = pbt->frameGeometry().x();
    int y = pbt->frameGeometry().y();
    QModelIndex index = ui->actualTasksWidget->indexAt(QPoint(x,y));
    int row = index.row();//按钮所在的当前行
    if(row == -1)
    {
        return;
    }
    qDebug() << "sellect row:" << row;
    
    // 选中某一行的隧道
    int tunnelid = ui->actualTasksWidget->item(row, CHECKEDTASK_TUNNELID)->text().toInt();
    // 记录当前所选择隧道
    tunnelname = ui->actualTasksWidget->item(row, CHECKEDTASK_TUNNELNAME)->text();
    ClientSetting::getSettingInstance()->setCurrentEditingTunnel(tunnelname);

    ui->actualTasksWidget->selectRow(row);

    QString date;
    if (projectname.compare("") != 0)
    {
        QStringList strList = projectname.split("_", QString::SkipEmptyParts);
        if (strList.length() < 2)
        {
            qDebug() << tr("解析字符出错") << projectname;
            return;
        }
        date = strList.value(1);
    }
    QString username = ClientSetting::getSettingInstance()->getCurrentUser();
    ClientProgram::getClientProgramInstance()->applyForModifiy(projectname, tunnelid, tunnelname, date, username);
}

// 转到综合按钮槽函数
void SelectAvaliableTunnelWidget::on_nextWidgetButton_clicked()
{
    int currow = ui->actualTasksWidget->currentRow();

    if (currow < 0)
    {
    	QMessageBox::warning(this,tr("提示"), tr("请先选择一条隧道！"));
        return;
    }
    // 记录当前所选择隧道
    tunnelname = ui->actualTasksWidget->item(currow, CHECKEDTASK_TUNNELNAME)->text();
	long long startframeno = ui->actualTasksWidget->item(currow, CHECKEDTASK_STARTFRAME)->text().toLongLong();
	long long endframeno = ui->actualTasksWidget->item(currow, CHECKEDTASK_ENDFRAME)->text().toLongLong();

    ClientSetting::getSettingInstance()->setCurrentEditingTunnel(tunnelname);
    //QString projectfilename = LzProjectAccess::getLzProjectAccessInstance()->getProjectFilename(LzProjectClass::Main);
    ClientSetting::getSettingInstance()->setCurrentEditingProject(projectname);

    if (!ClientSetting::getSettingInstance()->getCanEditing())
    {
        QMessageBox::warning(this, tr("提示"), tr("当前隧道还未从服务器申请到修正权限，不能修正！"));
        return;
    }

    string tunnelname1(tunnelname.toLocal8Bit().constData());
    
    // 引用赋值
    CheckedTaskList & clist = LzProjectAccess::getLzProjectAccessInstance()->getLzCheckedList(LzProjectClass::Main);
    int tunnelid = clist.getTunnelIDByTunnelName(tunnelname1);
    bool traindirection = true;

	bool isNormal = true;

    CheckedTunnelTaskModel & tmp = clist.getCheckedTunnelModel(tunnelid, traindirection);

    if (traindirection)
		traindirection = tmp.planTask.traindirection;
    else
        traindirection = true; // 默认正向车厢

	if(isNormal)
		isNormal = tmp.planTask.isnormal;
	else
		isNormal = true;   //默认为正常行驶



    // 设置可编辑的基础隧道信息到全局空间
    bool ret = ClientSetting::getSettingInstance()->setCorrectTunnelDataModel(tunnelid);
    if (ret == false)
    {
        QMessageBox::warning(this, tr("提示"), QObject::tr("当前隧道ID: %1 的隧道%2没有在数据库中，不能找到隧道模型！").arg(tunnelid).arg(tunnelname1.c_str()));
    }

    QStringList strlist = projectname.split("_");
    int tmpsize = strlist.length();

    QString filename = tunnelname + "_" + strlist.at(tmpsize - 1).left( strlist.at(tmpsize - 1).length()) + ".syn";
    qDebug() << "find tunnelid:" << tunnelid << tunnelname << filename;

    QString signalfilename = LzProjectAccess::getLzProjectAccessInstance()->getProjectPath(LzProjectClass::Main) + "/syn_data/" + filename;

    emit signalSelectedTunnelToEdit(tunnelid, signalfilename, traindirection, isNormal, startframeno, endframeno);
}

/** 
 * 日志类初始化
 */
bool SelectAvaliableTunnelWidget::initLogger(string filename, string username)
{
    if (logger != NULL)
    {
        if (logger->isLogging())
        {
            logger->log(string("关闭工程-客户端计算完后综合文件帧校正,") + string("用户") + username);
            logger->close();
        }
        delete logger;
    }

    logger = new LzLogger(LzLogger::MasterOpt);
    logger->setFilename(filename);
    if (logger->open() == 0)
    {
        hasinitlog = true;
        logger->log(string("开启工程-客户端计算完后综合文件帧校正") + string("用户") + username);
        return true;
    }
    else
    {
        hasinitlog = false;
        return false;
    }
}

void SelectAvaliableTunnelWidget::log(QString msg)
{
    if (hasinitlog)
        logger->log(msg.toLocal8Bit().constData());
}

void SelectAvaliableTunnelWidget::opensetvalidframes()
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
    openvalidframes->showWidget(false);
    openvalidframes->setShowWidgetDisVisible(false);
    connect(openvalidframes, SIGNAL(sendframestocheck_task(long long ,long long)), this, SLOT(getframesfromsetvalidframes(long long ,long long)));

    //initLogger(, ClientSetting::getSettingInstance()->getCurrentUser().toLocal8Bit().constData());
}

void SelectAvaliableTunnelWidget::getframesfromsetvalidframes(long long  newstartframes, long long newendframes)
{
    // 得到数据要记录到日志中
    int tunnelid = ui->actualTasksWidget->item(valid_row, CHECKEDTASK_TUNNELID)->data(Qt::DisplayRole).toInt();
    QString tunnelname = ui->actualTasksWidget->item(valid_row, CHECKEDTASK_TUNNELNAME)->data(Qt::DisplayRole).toString();
    QString seqnototal = ui->actualTasksWidget->item(valid_row, CHECKEDTASK_SEQNO_TOTAL)->data(Qt::DisplayRole).toString();
    int startseqno = 1;
    startseqno = QStringList(seqnototal.split("-")).at(0).toInt();

    // 从实际隧道文件的的xml中解析实际隧道数据和校正之后的数据到listcollect中

    QStringList strlist = projectname.split("_");
    int tmpsize = strlist.length();

    QString filenameprefix = tunnelname + "_" + strlist.at(tmpsize - 1).left( strlist.at(tmpsize - 1).length());
    qDebug() << "find tunnelid:" << tunnelid << tunnelname << filenameprefix;

    QString logfile = LzProjectAccess::getLzProjectAccessInstance()->getProjectPath(LzProjectClass::Main) + "/syn_data/" + filenameprefix + "_correct.log";
	QString checkedFilename = LzProjectAccess::getLzProjectAccessInstance()->getProjectModel(LzProjectClass::Main).getCheckedFilename();
    QString checkedFile = LzProjectAccess::getLzProjectAccessInstance()->getProjectPath(LzProjectClass::Main)  + "/" + checkedFilename;

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
            
            //ischeckedfile = true;
            // 更新界面
            loadCheckedTaskTunnelData();
        }
    }
    else
        // 记录到日志中
        QMessageBox::warning(this, QObject::tr("错误"), QObject::tr("设置有效帧范围无效：帧范围无效！"));

}