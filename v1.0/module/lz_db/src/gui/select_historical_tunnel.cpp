#include "select_historical_tunnel.h"
#include "ui_select_historical_tunnel.h"

#include <QtSql/QSqlRelationalTableModel>
#include <QtSql/QSqlRelationalDelegate>
#include <QSqlRecord>
#include <QSqlTableModel>
#include <QDebug>
#include <QComboBox>
#include <QTableWidgetItem>
#include <QDateTime>
#include "daoline.h"
#include "daotunnel.h"
#include "daocurve.h"
#include "daotask.h"
#include "daotasktunnel.h"
#include "daoclearanceoutput.h"
#include "tunneldatamodel.h"
#include "LzSynth_n.h"

/**
 * 历史隧道选择输出选择（隧道限界图表输出之前）界面类定义
 *
 * @author 熊雪 范翔
 * @version 1.0.0
 * @date 20140322
 */
SelectHistoricalTunnelDataWidget::SelectHistoricalTunnelDataWidget(QWidget *parent) :
    QWidget(parent),
    singleTunnelModel(ClientSetting::getSettingInstance()->getSingleTunnelModel()), 
    multiTunnelsModel(ClientSetting::getSettingInstance()->getMultiTunnelsModel()), 
    ui(new Ui::SelectHistoricalTunnelDataWidget)
{
    ui->setupUi(this);

    linesModel = NULL;
    taskModel = NULL;
    taskTunnelsModel = NULL;
    oneTunnelHistoricalModel = NULL;

    // 开始不能修改
    currentRow = -1;
    currentHistorySelectRow = -1;

    // ui->singlePage->layout()->addWidget(ui->singelpagewidget);
    // 【注意】，此处添加widget为了是窗口布局随大窗口缩放而改变
    // 【注意！】ui->stackedGridLayout为自己在Ui设计器中命名的Layout，该Layout中包含QStackedWidget
    ui->stackedGridLayout->addWidget(ui->singlePage, 1, 0, 1, 1);
    ui->stackedGridLayout->addWidget(ui->multiPage, 1, 0, 1, 1);

    // 初始设置按时间方式选择隧道输出
    watchMode = By_Date;

    // 变换隧道查看模式信号槽 【注】toggled信号可知道被点击或未被点击（只有两个RadioButton的情况下）
    connect(ui->radioButton_byDate, SIGNAL(toggled(bool)), this, SLOT(changeSelectionMode(bool)));

    //ui->linesView->setItemDelegate(new QSqlRelationalDelegate(this));
    ui->linesView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->linesView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->linesView->resizeColumnsToContents();
    ui->linesView->horizontalHeader()->setStretchLastSection(true);
    ui->linesView->verticalHeader()->hide();
    ui->linesView->horizontalHeader()->hide();

    //ui->tunnelsView->setItemDelegate(new QSqlRelationalDelegate(this));
    ui->tunnelsView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tunnelsView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tunnelsView->resizeColumnsToContents();
    ui->tunnelsView->horizontalHeader()->setStretchLastSection(true);
    //loadTunnelData();

    // 线路及隧道数据列表更新信号槽
    connect(ui->linesView, SIGNAL(clicked(QModelIndex)), this, SLOT(updateTunnelsView(const QModelIndex &)));

    // 设置按单隧道方式选择输出信号槽 【注】toggled信号可知道被点击或未被点击（只有两个RadioButton的情况下）
    connect(ui->radioButton_singleMode, SIGNAL(toggled(bool)), this, SLOT(changeSingleMultiMode(bool)));
    // 初始设置按单隧道方式选择输出
    ui->singlePage->setVisible(true);
    ui->multiPage->setVisible(false);
    singleMultiMode = Single_Mode;
    ClientSetting::getSettingInstance()->setSingleMultiMode(Single_Mode);

    ///// 如果是Single_Mode模式下按如下设置 /////
    //ui->historyTunnelTaskView->setItemDelegate(new QSqlRelationalDelegate(this));
    ui->historyTunnelTaskView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->historyTunnelTaskView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->historyTunnelTaskView->resizeColumnsToContents();
    ui->historyTunnelTaskView->horizontalHeader()->setStretchLastSection(true);
    connect(ui->historyTunnelTaskView, SIGNAL(clicked(QModelIndex)), this, SLOT(updateSelectHistoryRow(const QModelIndex &)));
    // 单隧道选择方式下：具体隧道信息信号槽
    connect(ui->tunnelsView, SIGNAL(clicked(QModelIndex)), this, SLOT(showTunnelDetail(const QModelIndex &)));
    // 具体隧道信息信号槽。最前，最后，上一个，下一个
    connect(ui->firstButton, SIGNAL(clicked()), this, SLOT(toFirst()));
    connect(ui->previousButton, SIGNAL(clicked()), this, SLOT(toPrevious()));
    connect(ui->nextButton, SIGNAL(clicked()), this, SLOT(toNext()));
    connect(ui->lastButton, SIGNAL(clicked()), this, SLOT(toLast()));

    ///// 如果是Multi_Mode模式下按如下设置 /////
    initMultiTunnelsTableHeader();
    connect(ui->addToMultiTunnelsButton, SIGNAL(clicked()), this, SLOT(addMultiTaskTunnels()));
    connect(ui->removeFromMultiTunnelsButton, SIGNAL(clicked()), this, SLOT(removeMultiTaskTunnels()));

    // 默认预览查看的是直线建筑限界
    selectClearanceType = Curve_Straight;
    // 加载限界数据准备输出槽函数
    connect(ui->synthesisButton, SIGNAL(clicked()), this, SLOT(loadClearanceDataToOutput()));
    // 表格输出信号槽
    connect(ui->outputButton, SIGNAL(clicked()), this, SLOT(toOutputWidget()));
    // 没有做出选择综合隧道不能切换到预览界面
    canGoNextPage = false;
    ui->outputButton->setEnabled(false);

    // 查找隧道槽
    connect(ui->findEdit, SIGNAL(textChanged(QString)), this, SLOT(checkFindEdit()));
    connect(ui->findButton, SIGNAL(clicked()), this, SLOT(findTunnel()));
    connect(ui->findEdit, SIGNAL(textEdited(QString)), this, SLOT(showAllLines()));

    // 加载数据
    if (watchMode == By_Date)
        loadRecentTaskData();
    else
        loadLinesData();
    loadTunnelData();
}

SelectHistoricalTunnelDataWidget::~SelectHistoricalTunnelDataWidget()
{
    delete ui;
}

/**
 * 变换隧道输出选择方式
 */
void SelectHistoricalTunnelDataWidget::changeSingleMultiMode(bool checked)
{
    // 如果点击的是单隧道输出方式
    if (checked)
    {
        ui->singlePage->setVisible(true);
        ui->multiPage->setVisible(false);
        singleMultiMode = Single_Mode;
        ClientSetting::getSettingInstance()->setSingleMultiMode(Single_Mode);
    }
    else    // 点击的是多隧道综合输出方式
    {
        ui->singlePage->setVisible(false);
        ui->multiPage->setVisible(true);
        singleMultiMode = Multi_Mode;
        ClientSetting::getSettingInstance()->setSingleMultiMode(Multi_Mode);
    }
    clearClearanceDataInfo();
}

/**
 * 变换隧道查看模式
 */
void SelectHistoricalTunnelDataWidget::changeSelectionMode(bool mode)
{
    if (mode)
    {
        watchMode = By_Date;
        loadRecentTaskData();
    }
    else
    {
        watchMode = By_Line;
        loadLinesData();
    }
    loadTunnelData();
    // qDebug() << "change watch mode:" << watchMode << mode;
}

/**
 * 当watchMode == By_Line时按经典的选择线路模式加载
 */
void SelectHistoricalTunnelDataWidget::loadLinesData()
{
    linesModel = LineDAO::getLineDAOInstance()->getLineNames();
    ui->linesView->setModel(linesModel);
    // 【标记1】初始设置选中第一行，后边需要根据这个信息得到当前线路名称
    QModelIndex index = linesModel->index(0, 0);
    ui->linesView->setCurrentIndex(index);
}

/**
 * 当watchMode == By_Date时按经典的选择线路模式加载
 */
void SelectHistoricalTunnelDataWidget::loadRecentTaskData()
{
    int currentYear = QDateTime::currentDateTime().toString("yyyy").toInt();
    qDebug() << "loadRecentTaskData" << currentYear;
    taskModel = TaskDAO::getTaskDAOInstance()->getRecentTasksByLine(0, currentYear);
    ui->linesView->setModel(taskModel);
    // 【标记1】初始设置选中第一行，后边需要根据这个信息得到当前线路名称
    QModelIndex index = taskModel->index(0, 0);
    ui->linesView->setCurrentIndex(index);
    ui->linesView->hideColumn(Task_ID);
    ui->linesView->hideColumn(Task_frame_num);
    ui->linesView->hideColumn(Task_carriage_direction);
    ui->linesView->hideColumn(Task_remark);
    ui->linesView->hideColumn(Task_Line_ID);
}

/**
 * 当watchMode == By_Line或By_Date时
 * 界面中间的隧道视图更新
 */
void SelectHistoricalTunnelDataWidget::loadTunnelData()
{
    if (watchMode == By_Date)
    {
        QModelIndex index = ui->linesView->model()->index(0, 0);
        if (!index.isValid())
        {
            qDebug() << QObject::tr("加载历史任务不成功");
            return;
        }
        _int64 taskid = -1;
        taskid = (_int64) ui->linesView->model()->data(index).toLongLong();
        taskTunnelsModel = TaskTunnelDAO::getTaskTunnelDAOInstance()->getTaskTunnels_2(taskid);
    }
    else if (watchMode = By_Line)
    {
        QModelIndex index = ui->linesView->model()->index(0, 0);
        if (!index.isValid())
        {
            qDebug() << QObject::tr("加载线路不成功");
            return;
        }
        QString linename = index.data().toString();
        int lineid = LineDAO::getLineDAOInstance()->getLineID(linename);
        if (lineid == -1)
        {
            return;
        }
        taskTunnelsModel = TaskTunnelDAO::getTaskTunnelDAOInstance()->getTaskTunnelsOneLineRecently(lineid);
    }

    if (taskTunnelsModel == NULL)
    {
        qDebug() << QObject::tr("加载隧道不成功");
        currentRow = -1;
        return;
    }
    ui->tunnelsView->setModel(taskTunnelsModel);
    // 隐藏隧道ID等信息
    ui->tunnelsView->hideColumn(Tunnel_ID);
    ui->tunnelsView->hideColumn(Tunnel_is_bridge);
    ui->tunnelsView->hideColumn(Tunnel_is_double_line);
    ui->tunnelsView->hideColumn(Tunnel_is_downlink);
    ui->tunnelsView->hideColumn(Tunnel_is_new_std);
    ui->tunnelsView->hideColumn(Tunnel_is_valid);
    ui->tunnelsView->hideColumn(Tunnel_line_ID);
    ui->tunnelsView->hideColumn(Tunnel_line_type_std);
    ui->tunnelsView->hideColumn(14);

    currentRow = 0;
    ui->tunnelsView->selectRow(0);

    this->toFirst();
}

/**
 * 隧道列表视图更新显示，选择不同线路视图时的槽函数
 */
void SelectHistoricalTunnelDataWidget::updateTunnelsView(const QModelIndex &index)
{
    if (index.isValid())
    {
        if (watchMode == By_Date)
        {
            QModelIndex index2 = ui->linesView->model()->index(index.row(), 0);
            _int64 taskid = -1;
            taskid = (_int64) ui->linesView->model()->data(index2).toLongLong();
            taskTunnelsModel = TaskTunnelDAO::getTaskTunnelDAOInstance()->getTaskTunnels_2(taskid);
        }
        else if (watchMode == By_Line)
        {
            QString linename = index.data().toString();
            int lineid = LineDAO::getLineDAOInstance()->getLineID(linename);
            if (lineid == -1)
            {
                return;
            }
            taskTunnelsModel = TaskTunnelDAO::getTaskTunnelDAOInstance()->getTaskTunnelsOneLineRecently(lineid);
        }
        if (taskTunnelsModel->rowCount() == 0)
        {
            ui->lineEdit_linename->clear();
            ui->lineEdit_startStation->clear();
            ui->lineEdit_endStation->clear();
            ui->lineEdit_IDstd->clear();
            ui->lineEdit_name->clear();
            ui->lineEdit_startPoint->clear();
            ui->lineEdit_endPoint->clear();
            ui->lineEdit_isNew->clear();
            ui->lineEdit_lineType->clear();
            ui->lineEdit_isDoubleLine->clear();
            ui->lineEdit_isDownLink->clear();
            // 如果没有指定tunnel，清楚curveWidget中的数据
            // 先去掉原来的内容
            ui->lineEdit_leftCurvesNum->clear();
            ui->lineEdit_rightCurvesNum->clear();
            ui->lineEdit_staightsNum->clear();
            currentRow = -1;
        }
        else
        {
            currentRow = 0;
            if (singleMultiMode == Single_Mode)
                setCurrentMapper(currentRow);
        }
    }
    return;
}

/**
 * 当singleMultiMode == Single_Mode时，点击某一条隧道后的槽函数
 * 查看单隧道具体信息，将该隧道历史采集任务显示到界面中，以及隧道基本信息，含几条曲线等
 */
void SelectHistoricalTunnelDataWidget::showTunnelDetail(const QModelIndex& index)
{
    if (singleMultiMode != Single_Mode)
        return;

    if (!index.isValid())
        return;
    else
    {
        // 改变当前行号
        currentRow = index.row();
        // 该方法调用mapper->setCurrentIndex(currentRow);
        setCurrentMapper(currentRow);
    }
}

/**
 * 当singleMultiMode == Single_Mode时，
 * 用于mapper的四个按钮，调用mapper的toFirst、toNext、toLast、toPrevious方法
 */
void SelectHistoricalTunnelDataWidget::toFirst()
{
    if (singleMultiMode != Single_Mode)
        return;
    
    if (currentRow >= 0)
        currentRow = 0;
    setCurrentMapper(currentRow);
}

void SelectHistoricalTunnelDataWidget::toNext()
{
    if (singleMultiMode != Single_Mode)
        return;

    if (currentRow != ui->tunnelsView->model()->rowCount() - 1)
        currentRow++;
    setCurrentMapper(currentRow);
}

void SelectHistoricalTunnelDataWidget::toLast()
{
    if (singleMultiMode != Single_Mode)
        return;

    currentRow = ui->tunnelsView->model()->rowCount() - 1;
    setCurrentMapper(currentRow);
}

void SelectHistoricalTunnelDataWidget::toPrevious()
{
    if (singleMultiMode != Single_Mode)
        return;

    if (currentRow >= 0)
        currentRow--;
    setCurrentMapper(currentRow);
}

/**
 * 当singleMultiMode == Single_Mode时，
 * 设置隧道详细框当前匹配的内容显示出来，调用mapper的setCurrentIndex
 */
void SelectHistoricalTunnelDataWidget::setCurrentMapper(int currentRow)
{
    if (singleMultiMode != Single_Mode)
        return;

    if (currentRow < 0)
        return;

    ui->tunnelsView->selectRow(currentRow);

    // 相应隧道信息
    QSqlRecord record = taskTunnelsModel->record(currentRow);

    int tmptunnelid = record.value(Tunnel_ID).toInt();
    QString tmptunnelname = record.value(Tunnel_name_std).toString();

    bool ret = ClientSetting::getSettingInstance()->setOutputTunnelDataModel(tmptunnelid);
    if (!ret)
    {
        QMessageBox::warning(this, tr("提示"), tr("当前隧道未找到对应模型，隧道id为%1").arg(tmptunnelid).arg(tmptunnelname));
        return;
    }

    // 加载单隧道信息到相关Label中
    TunnelDataModel *tunnelDataModel = ClientSetting::getSettingInstance()->getOutputTunnelDataModel(ret);
    if (ret)
    {
        ui->lineEdit_linename->setText(QString("%1").arg(tunnelDataModel->getLinename().c_str()));
        ui->lineEdit_startStation->setText(QString("%1").arg(tunnelDataModel->getLineStartStation().c_str()));
        ui->lineEdit_endStation->setText(QString("%1").arg(tunnelDataModel->getLineEndStation().c_str()));
        ui->lineEdit_name->setText(QString(tr("%1")).arg(QString::fromLocal8Bit(tunnelDataModel->getName().c_str())));
        ui->lineEdit_IDstd->setText(QString(tr("%1")).arg(QString::fromLocal8Bit(tunnelDataModel->getIdStd().c_str())));
        ui->lineEdit_startPoint->setText(QString("%1").arg(tunnelDataModel->getStartPoint()));
        ui->lineEdit_endPoint->setText(QString("%1").arg(tunnelDataModel->getEndPoint()));

        int isNewLine = tunnelDataModel->getIsNew();
        int lineType = tunnelDataModel->getLineType();
        bool isDoubleLine = tunnelDataModel->getIsDoubleLine();
        bool isDownLink = tunnelDataModel->getIsDownlink();
        //qDebug() << "isNewLine:" << isNewLine << ", lineType:" << lineType << ", isDoubleLine:" << isDoubleLine << ", isDownLink:" << isDownLink;

        if (isNewLine == 1) // 新线
            ui->lineEdit_isNew->setText(tr("新线"));
        else if (isNewLine == 0) // 旧线
            ui->lineEdit_isNew->setText(tr("旧线"));
        else
            ui->lineEdit_isNew->setText(tr(""));

        if (lineType == 1) // 内燃牵引
            ui->lineEdit_lineType->setText(tr("内燃牵引"));
        else if (lineType == 0) // 电力牵引
            ui->lineEdit_lineType->setText(tr("电力牵引"));
        else
            ui->lineEdit_lineType->setText(tr(""));

        if (isDoubleLine == true) // 双线
            ui->lineEdit_isDoubleLine->setText(tr("双线"));
        else if (isDoubleLine == false) //
            ui->lineEdit_isDoubleLine->setText(tr("单线"));
        else
            ui->lineEdit_isDoubleLine->setText(tr(""));

        if (isDownLink == true) // 下行
            ui->lineEdit_isDownLink->setText(tr("下行"));
        else if (isDownLink == false) // 上行
            ui->lineEdit_isDownLink->setText(tr("上行"));
        else
            ui->lineEdit_isDownLink->setText(tr(""));
    }
    else
    {
        QMessageBox::warning(this, tr("提示"), tr("当前隧道未找到对应模型，隧道id为%1").arg(tmptunnelid).arg(tmptunnelname));
        return;
    }

    /**********加载该条隧道的历史采集任务***********/
    int tunnelid = record.value(Tunnel_ID).toInt();

    // 历史隧道采集任务
    oneTunnelHistoricalModel = TaskTunnelDAO::getTaskTunnelDAOInstance()->getTaskTunnels_OneTunnel(tunnelid);
    if (oneTunnelHistoricalModel != NULL)
    {
        ui->historyTunnelTaskView->setModel(oneTunnelHistoricalModel);
        // 隐藏采集任务隧道ID task_tunnel_id，隧道ID tunnel_id
        ui->historyTunnelTaskView->hideColumn(TaskTunnel_ID);
        ui->historyTunnelTaskView->hideColumn(1);
        if (oneTunnelHistoricalModel->rowCount() > 0) // 当前没有数据
        {
            ui->historyTunnelTaskView->selectRow(0);
            currentHistorySelectRow = 0;
        }
        else
            currentHistorySelectRow = -1;
    }

    // 清空显示
    clearClearanceDataInfo();
    canGoNextPage = false;
}

/**
 * 更新历史选择隧道行号
 */
void SelectHistoricalTunnelDataWidget::updateSelectHistoryRow(const QModelIndex & index)
{
    currentHistorySelectRow = index.row();
}

/**
 * 初始化多隧道综合表格表头信息
 */
void SelectHistoricalTunnelDataWidget::initMultiTunnelsTableHeader()
{
    ui->multiTunnelsTableWidget->setColumnCount(15);
    ui->multiTunnelsTableWidget->setHorizontalHeaderItem(Tunnel_ID, new QTableWidgetItem(QObject::tr("隧道ID")));
    ui->multiTunnelsTableWidget->setHorizontalHeaderItem(Tunnel_name_std, new QTableWidgetItem(QObject::tr("隧道名称")));
    ui->multiTunnelsTableWidget->setHorizontalHeaderItem(Tunnel_ID_std, new QTableWidgetItem(QObject::tr("隧道编号")));
    ui->multiTunnelsTableWidget->setHorizontalHeaderItem(Tunnel_start_point_std, new QTableWidgetItem(QObject::tr("起始里程")));
    ui->multiTunnelsTableWidget->setHorizontalHeaderItem(Tunnel_end_point_std, new QTableWidgetItem(QObject::tr("终止里程")));
    ui->multiTunnelsTableWidget->setHorizontalHeaderItem(Tunnel_is_new_std, new QTableWidgetItem(QObject::tr("是否为新线")));
    ui->multiTunnelsTableWidget->setHorizontalHeaderItem(Tunnel_line_type_std, new QTableWidgetItem(QObject::tr("线路类型")));
    ui->multiTunnelsTableWidget->setHorizontalHeaderItem(Tunnel_is_double_line, new QTableWidgetItem(QObject::tr("是否为双线")));
    ui->multiTunnelsTableWidget->setHorizontalHeaderItem(Tunnel_is_downlink, new QTableWidgetItem(QObject::tr("是否为下行(双线)")));
    ui->multiTunnelsTableWidget->setHorizontalHeaderItem(Tunnel_line_ID, new QTableWidgetItem(QObject::tr("所在线路")));
    ui->multiTunnelsTableWidget->setHorizontalHeaderItem(Tunnel_is_bridge, new QTableWidgetItem(QObject::tr("是否为桥梁")));
    ui->multiTunnelsTableWidget->setHorizontalHeaderItem(Tunnel_is_valid, new QTableWidgetItem(QObject::tr("是否被删除作废")));
    ui->multiTunnelsTableWidget->setHorizontalHeaderItem(12, new QTableWidgetItem(QObject::tr("采集时间")));
    ui->multiTunnelsTableWidget->setHorizontalHeaderItem(13, new QTableWidgetItem(QObject::tr("隧道长度")));
    ui->multiTunnelsTableWidget->setHorizontalHeaderItem(14, new QTableWidgetItem(QObject::tr("采集工程隧道ID")));

    ui->multiTunnelsTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->multiTunnelsTableWidget->hideColumn(0);

    ui->strightButton->setVisible(false);
    ui->leftButton->setVisible(false);
    ui->rightButton->setVisible(false);
}

/**
 * 当singleMultiMode == Multi_Mode，即区段选择综合时，添加多条隧道
 */
void SelectHistoricalTunnelDataWidget::addMultiTaskTunnels()
{
    if (singleMultiMode != Multi_Mode)
        return;

    QItemSelectionModel *selections = ui->tunnelsView->selectionModel();
    QModelIndexList selected = selections->selectedRows();
    foreach (QModelIndex index, selected)
    {
        qDebug() << index.row();
        //addTaskTunnel(index);
    }
    if (selected.size() == 0)
    {
        QMessageBox::warning(this, tr("提示"), tr("请从左侧先选择一条或多条隧道加入多隧道综合列表！"));
    }
    QSqlRecord tmprecord;
    int columnCount = 0;

    foreach (QModelIndex index, selected)
    {
        if (index.isValid())
        {

            tmprecord = taskTunnelsModel->record(index.row());
            columnCount = taskTunnelsModel->columnCount();

            ////TODO
            // 如果已经加入了不能重复再加

            // 填充表格内容
            QString itemstr;
            int i = ui->multiTunnelsTableWidget->rowCount();
            ui->multiTunnelsTableWidget->insertRow(i);
            for (int j = 0; j < columnCount; j++)
            {

                itemstr = tmprecord.value(j).toString();
                QTableWidgetItem * tablewidgetitem = new QTableWidgetItem();
                tablewidgetitem->setText(itemstr);
                ui->multiTunnelsTableWidget->setItem(i, j, tablewidgetitem);
            }
        }
    }
}

/**
 * 当singleMultiMode == Multi_Mode，即区段选择综合时，删除多条隧道
 */
void SelectHistoricalTunnelDataWidget::removeMultiTaskTunnels()
{
    if (singleMultiMode != Multi_Mode)
        return;
       
    if (ui->multiTunnelsTableWidget->selectedRanges().isEmpty() == false)
    {
        // 部分删除选择行判断
        QList<QTableWidgetSelectionRange> ranges = ui->multiTunnelsTableWidget->selectedRanges();
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
        //qDebug() << "sellectedvalid=" << sellectvalid << "count:" << count << ",topRow:" << topRow << "bottomRow:" << bottomRow;;
        if (!sellectvalid && (bottomRow - topRow <= 1))
        {
            QMessageBox::warning(this, tr("提示"), tr("请选择相邻两行或多行删除！"));
            return;
        }
        // 删除多余
        for (int i = topRow; i <= bottomRow; i++)
        {
            ui->multiTunnelsTableWidget->removeRow(topRow);
        }
    }
    else
    {
        QMessageBox::warning(this, tr("提示"), tr("请选择某行!"));
        return;
    }
}

/**
 * 界面上的预览直线段、左转曲线段、右转曲线段按钮及数字的信号槽
 */
void SelectHistoricalTunnelDataWidget::loadClearanceDataToOutput()
{
    if (singleMultiMode == Single_Mode) // 单隧道综合
    {
        // 如果还未选择历史隧道隧道，提示
        if (currentHistorySelectRow < 0)
        {
            QMessageBox::warning(this, tr("提示"), tr("当前未选择任何历史隧道，请先选择！"));
            return;
        }

        int tunnelid = oneTunnelHistoricalModel->index(currentHistorySelectRow, 1).data().toInt();
        // 隧道基本信息
        ClientSetting::getSettingInstance()->setOutputTunnelDataModel(tunnelid);
        bool ret;
        TunnelDataModel * tunnelDataModel = ClientSetting::getSettingInstance()->getOutputTunnelDataModel(ret);
        if (!ret || tunnelDataModel == NULL)
        {
            // 清空显示
            clearClearanceDataInfo();
            return;
        }

        // 左转曲线，右转曲线，直线段是否可以显示
        int tmpNum = tunnelDataModel->getNumberOfStrights();
        ui->lineEdit_staightsNum->setText(QString("%1").arg(tmpNum));
        if (tmpNum > 0)
            ui->strightButton->setVisible(true);
        else
            ui->strightButton->setVisible(false);
        tmpNum = tunnelDataModel->getNumberOfLeftCurves();
        ui->lineEdit_leftCurvesNum->setText(QString("%1").arg(tmpNum));
        if (tmpNum > 0)
            ui->leftButton->setVisible(true);
        else
            ui->leftButton->setVisible(false);
        tmpNum = tunnelDataModel->getNumberOfRightCurves();
        ui->lineEdit_rightCurvesNum->setText(QString("%1").arg(tmpNum));
        if (tmpNum > 0)
            ui->rightButton->setVisible(true);
        else
            ui->rightButton->setVisible(false);

        // qDebug() << tunnelDataModel->getNumberOfLeftCurves() << tunnelDataModel->getNumberOfRightCurves() << tunnelDataModel->getNumberOfStrights();

        // 加载综合数据
        _int64 tasktunnelid = oneTunnelHistoricalModel->index(currentHistorySelectRow, 0).data().toLongLong(); // task_tunnel_id
        qDebug() << tasktunnelid << oneTunnelHistoricalModel->index(currentHistorySelectRow, 2).data().toString() << oneTunnelHistoricalModel->index(currentHistorySelectRow, 3).data().toString() << oneTunnelHistoricalModel->index(currentHistorySelectRow, 4).data().toString();

        
        // 选择并加载数据后可以输出预览
        singleTunnelModel.initClearanceDatas(tasktunnelid);
        ret = singleTunnelModel.loadsynthesisdata();
        if (!ret)
        {
            ui->synthesisInfoLabel->setText(tr("加载数据库综合结果失败"));
            canGoNextPage = false;
        }
        else
        {
            ui->synthesisInfoLabel->setText(tr("加载数据库综合结果成功"));
            canGoNextPage = true;
            ui->outputButton->setEnabled(true);
        }
    }
    else // 多隧道综合
    {
        // 加载综合数据
        // @author 范翔，此处如果用debug调试可能出现错误，release则没有问题
        std::list<_int64> tasktunnelids;
        QTableWidgetItem* item;
        _int64 itemlong = 0;
        for (int i = 0; i < ui->multiTunnelsTableWidget->rowCount(); i++)
        {
            itemlong = ui->multiTunnelsTableWidget->item(i, 14)->text().toLongLong();
            tasktunnelids.push_back(itemlong);
        }
        // 如果多隧道数量不够，提示
        if (tasktunnelids.size() < 1)
        {
            QMessageBox::warning(this, tr("提示"), tr("当前未选择待综合的多条隧道，请先选择！"));
            return;
        }

        multiTunnelsModel.initClearanceDatas(tasktunnelids);
        bool ret = multiTunnelsModel.synthesis();

        int numofstraight = multiTunnelsModel.getNumOfStraight();
        int numofleft = multiTunnelsModel.getNumOfLeft();
        int numofright = multiTunnelsModel.getNumOfRight();

        // 左转曲线，右转曲线，直线段是否可以显示
        ui->lineEdit_staightsNum->setText(QString("%1").arg(numofstraight));
        if (numofstraight > 0)
            ui->strightButton->setVisible(true);
        else
            ui->strightButton->setVisible(false);
        ui->lineEdit_leftCurvesNum->setText(QString("%1").arg(numofleft));
        if (numofleft > 0)
            ui->leftButton->setVisible(true);
        else
            ui->leftButton->setVisible(false);
        ui->lineEdit_rightCurvesNum->setText(QString("%1").arg(numofright));
        if (numofright > 0)
            ui->rightButton->setVisible(true);
        else
            ui->rightButton->setVisible(false);
        
        // 选择并加载数据后可以输出预览
        if (!ret)
        {
            ui->synthesisInfoLabel->setText(tr("多隧道综合失败"));
            canGoNextPage = false;
        }
        else
        {
            ui->synthesisInfoLabel->setText(tr("多隧道综合成功"));
            canGoNextPage = true;
            ui->outputButton->setEnabled(true);
        }
    }
}

void SelectHistoricalTunnelDataWidget::clearClearanceDataInfo()
{
    ui->lineEdit_staightsNum->setText("");
    ui->lineEdit_leftCurvesNum->setText("");
    ui->lineEdit_rightCurvesNum->setText("");
    ui->strightButton->setVisible(false);
    ui->leftButton->setVisible(false);
    ui->rightButton->setVisible(false);
    ui->synthesisInfoLabel->setText(QObject::tr("隧道数据未加载"));
    canGoNextPage = false;
    ui->outputButton->setEnabled(false);
}

/**
 * 点击跳转页面后跳转到OutputWidget页面，同时传参数
 */
void SelectHistoricalTunnelDataWidget::toOutputWidget()
{
    if (!canGoNextPage)
    {
        QMessageBox::warning(this, tr("提示"), tr("没有做出选择综合隧道不能切换到预览界面!"));
        return;
    }

    if (singleMultiMode == Single_Mode)
        emit updateOutput(Single_Mode, selectClearanceType);
    else
        emit updateOutput(Multi_Mode, selectClearanceType);
}

void SelectHistoricalTunnelDataWidget::checkFindEdit()
{
    // 哪种查找模式
    int findindex = 0;
    if (ui->radioButton_2->isChecked())
    {
        findindex = 1;
    }
    if (findindex == 0) 
    {
        QRegExp regExp1("[0-9]{0,10}");
        ui->findEdit->setValidator(new QRegExpValidator(regExp1, this));
    }
    else if (findindex == 1)
    {
        ui->findEdit->setValidator(0);
    }
}

void SelectHistoricalTunnelDataWidget::findTunnel()
{
    // 只有显示全部线路的时候才能查找
    if(ui->findEdit->text().compare("") == 0)//如果线路名称为空的话
    {
        QMessageBox::warning(NULL,tr("提示"),tr("温馨提示：隧道名称不能为空。"));
        return;
    }
    int row1 = linesModel->rowCount();//获得QListView控件中的行数
    // qDebug()<<row1;
    int k = 0;
    for (k = 0; k < row1; k++)
    {
        QModelIndex index = ui->linesView->model()->index(k,0);
        QString linename = index.data().toString();//这两行代码获取ListView中某行某列的值
        QString query = ui->findEdit->text();
        //qDebug()<<query;
        //qDebug()<<index;
        if (linename.compare(query) == 0)
        {
            ui->linesView->setCurrentIndex(index);
            //ui->linesView->show();
            //ui->linesView->edit(index);//表示为查找的线路可编辑，按常理来说是不可编辑的
            break;
        }
        else
        {
            continue;
        }
    }
    //qDebug()<<k;
    for (int i = 0; i < k; i++)
        ui->linesView->setRowHidden(i ,true);
    for (int i = k + 1; i < row1; i++)
        ui->linesView->setRowHidden(i, true);
    if (k == row1)
        QMessageBox::warning(NULL,tr("提示"),tr("温馨提示：未查找到此条线路。"));
    // 要跳出for循环，才能确定已经查找完所有数据
}

void SelectHistoricalTunnelDataWidget::showAllLines()
{
    if(ui->findEdit->text().compare("") == 0)
    {
        int row1 = linesModel->rowCount();
        for(int i = 0; i < row1; i++)
        {
            ui->linesView->setRowHidden(i, false);
        }
        ui->linesView->show();
    }
}

void SelectHistoricalTunnelDataWidget::findLine()
{
    // 只有显示全部线路的时候才能查找
    if(ui->findEdit->text().compare("") == 0)//如果线路名称为空的话
    {
        QMessageBox::warning(NULL,tr("提示"),tr("温馨提示：线路名称不能为空。"));
        return;
    }
    int row1 = linesModel->rowCount();//获得QListView控件中的行数
    // qDebug()<<row1;
    int k = 0;
    for (k = 0; k < row1; k++)
    {
        QModelIndex index = ui->linesView->model()->index(k,0);
        QString linename = index.data().toString();//这两行代码获取ListView中某行某列的值
        QString query = ui->findEdit->text();
        //qDebug()<<query;
        //qDebug()<<index;
        if (linename.compare(query) == 0)
        {
            ui->linesView->setCurrentIndex(index);
            //ui->linesView->show();
            //ui->linesView->edit(index);//表示为查找的线路可编辑，按常理来说是不可编辑的
            break;
        }
        else
        {
            continue;
        }
    }
    //qDebug()<<k;
    for (int i = 0; i < k; i++)
        ui->linesView->setRowHidden(i ,true);
    for (int i = k + 1; i < row1; i++)
        ui->linesView->setRowHidden(i, true);
    if (k == row1)
        QMessageBox::warning(NULL,tr("提示"),tr("温馨提示：未查找到此条线路。"));
    // 要跳出for循环，才能确定已经查找完所有数据
}
