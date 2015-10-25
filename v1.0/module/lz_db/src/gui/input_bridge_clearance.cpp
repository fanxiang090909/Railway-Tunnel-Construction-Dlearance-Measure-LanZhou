#include "input_bridge_clearance.h"
#include "ui_input_bridge_clearance.h"

#include <QTableWidgetItem>
#include <QDebug>
#include <QHeaderView>
#include <QSqlRecord>
#include <QMessageBox>

#include "daotask.h"
#include "daotasktunnel.h"
#include "lz_output.h"

/**
 * 桥梁限界输入界面类定义
 *
 * @author 熊雪
 * @author 范翔
 * @version 1.0.0
 * @date 20140615
 */
InputBridgeWidget::InputBridgeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InputBridgeWidget)
{
    ui->setupUi(this);

    //data.initMaps();
    bridgeCollectModel = NULL;

    tunnelid = -1;

    tasktunnelid = -1;

    connect(ui->finishButton, SIGNAL(clicked()), this, SLOT(finishEdit()));

    // 初始没有做任何编辑
    hasedit = false;

    // 加载桥梁限界表表头数据
    loadBridgeClearanceData();

    // 关于编辑限界值、保存限界值的信号槽
    connect(ui->tableWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(saveItemChanged(QTableWidgetItem*)));
    connect(ui->lineEdit_minHeight,SIGNAL(textEdited(QString)),this,SLOT(minheightChanged(QString)));
    connect(ui->modifyButton, SIGNAL(clicked()), this, SLOT(canBeModified()));
    connect(ui->saveButton, SIGNAL(clicked()), this, SLOT(saveChanges()));
    //connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(cancelModify()));
}

InputBridgeWidget::~InputBridgeWidget()
{
    delete ui;
}

void InputBridgeWidget::loadBridgeClearanceData()
{
    maketable = true;

    bool ret = datas.initMaps();
    datal.initMaps();
    datar.initMaps();
    if (ret == false)
    {
        qDebug() << "can init clearance value map:";
        return;
    }

    heightlength = datas.getMaps().size();
    ui->tableWidget->setColumnCount(16);
    ui->tableWidget->setRowCount(heightlength);
    //ui->tableWidget->setHorizontalHeaderItem(LzOutputTableColumn::Height, new QTableWidgetItem(QObject::tr("轨面高度")));
    ui->tableWidget->setHorizontalHeaderItem(Straight_Left_Val - 1, new QTableWidgetItem(QObject::tr("直线尺寸-左侧")));
    ui->tableWidget->setHorizontalHeaderItem(Straight_Right_Val - 1, new QTableWidgetItem(QObject::tr("直线尺寸-右侧")));
    ui->tableWidget->setHorizontalHeaderItem(Straight_Left_Pos - 1, new QTableWidgetItem(QObject::tr("直线控制点-左侧")));
    ui->tableWidget->setHorizontalHeaderItem(Straight_Right_Pos - 1, new QTableWidgetItem(QObject::tr("直线控制点-右侧")));
    ui->tableWidget->setHorizontalHeaderItem(LeftCurve_Left_Val - 1, new QTableWidgetItem(QObject::tr("左转尺寸-左侧")));
    ui->tableWidget->setHorizontalHeaderItem(LeftCurve_Right_Val - 1, new QTableWidgetItem(QObject::tr("左转尺寸-右侧")));
    ui->tableWidget->setHorizontalHeaderItem(LeftCurve_Left_Radius - 1, new QTableWidgetItem(QObject::tr("左转半径-左侧")));
    ui->tableWidget->setHorizontalHeaderItem(LeftCurve_Right_Radius - 1, new QTableWidgetItem(QObject::tr("左转半径-右侧")));
    ui->tableWidget->setHorizontalHeaderItem(LeftCurve_Left_Pos - 1, new QTableWidgetItem(QObject::tr("左转控制点-左侧")));
    ui->tableWidget->setHorizontalHeaderItem(LeftCurve_Right_Pos - 1, new QTableWidgetItem(QObject::tr("左转控制点-右侧")));
    ui->tableWidget->setHorizontalHeaderItem(RightCurve_Left_Val - 1, new QTableWidgetItem(QObject::tr("右转尺寸-左侧")));
    ui->tableWidget->setHorizontalHeaderItem(RightCurve_Right_Val - 1, new QTableWidgetItem(QObject::tr("右转尺寸-右侧")));
    ui->tableWidget->setHorizontalHeaderItem(RightCurve_Left_Radius - 1, new QTableWidgetItem(QObject::tr("右转半径-左侧")));
    ui->tableWidget->setHorizontalHeaderItem(RightCurve_Right_Radius - 1, new QTableWidgetItem(QObject::tr("右转半径-右侧")));
    ui->tableWidget->setHorizontalHeaderItem(RightCurve_Left_Pos - 1, new QTableWidgetItem(QObject::tr("右转控制点-左侧")));
    ui->tableWidget->setHorizontalHeaderItem(RightCurve_Right_Pos - 1, new QTableWidgetItem(QObject::tr("右转控制点-右侧")));
    
    std::map <int,ClearanceItem>::reverse_iterator it = datas.getMaps().rbegin();
    int i = 0;
    while (it != datas.getMaps().rend())
    {
        std::pair<int,ClearanceItem> pair = (*it);
        ui->tableWidget->setVerticalHeaderItem(i, new QTableWidgetItem(QString("%1").arg(pair.first)));
        it++;
        i++;
    }
    maketable = false;
}

void InputBridgeWidget::initBridgeClearanceData()
{
    maketable = true;

    ui->lineEdit_minHeight->setText(QString(""));

    // map反向遍历，正向为高度从小到大，插入图表是需要高度从大到小
    std::map<int, ClearanceItem>::reverse_iterator its = datas.getMaps().rbegin();

    int k = 0;
    while (its != datas.getMaps().rend())
    {
        std::pair<int, ClearanceItem> pairs = (*its);

        ui->tableWidget->setItem(k, Straight_Left_Val - 1, new QTableWidgetItem(QString("")));
        ui->tableWidget->setItem(k, Straight_Right_Val - 1, new QTableWidgetItem(QString("")));
        ui->tableWidget->setItem(k, Straight_Left_Pos - 1, new QTableWidgetItem(QString("")));
        ui->tableWidget->setItem(k, Straight_Right_Pos - 1, new QTableWidgetItem(QString("")));
        ui->tableWidget->setItem(k, LeftCurve_Left_Val - 1, new QTableWidgetItem(QString("")));
        ui->tableWidget->setItem(k, LeftCurve_Right_Val - 1, new QTableWidgetItem(QString("")));
        ui->tableWidget->setItem(k, LeftCurve_Left_Radius - 1, new QTableWidgetItem(QString("")));
        ui->tableWidget->setItem(k, LeftCurve_Right_Radius - 1, new QTableWidgetItem(QString("")));
        ui->tableWidget->setItem(k, LeftCurve_Left_Pos - 1, new QTableWidgetItem(QString("")));
        ui->tableWidget->setItem(k, LeftCurve_Right_Pos - 1, new QTableWidgetItem(QString("")));
        ui->tableWidget->setItem(k, RightCurve_Left_Val - 1, new QTableWidgetItem(QString("")));
        ui->tableWidget->setItem(k, RightCurve_Right_Val - 1, new QTableWidgetItem(QString("")));
        ui->tableWidget->setItem(k, RightCurve_Left_Radius - 1, new QTableWidgetItem(QString("")));
        ui->tableWidget->setItem(k, RightCurve_Right_Radius - 1, new QTableWidgetItem(QString("")));
        ui->tableWidget->setItem(k, RightCurve_Left_Pos - 1, new QTableWidgetItem(QString("")));
        ui->tableWidget->setItem(k, RightCurve_Right_Pos - 1, new QTableWidgetItem(QString("")));

        its++;
        k++;
    }
    maketable = false;
}

/**
 * 更新桥梁限界数据（只更新内部数据，不修改表头）
 */
void InputBridgeWidget::updateBridgeClearanceData()
{
    maketable = true;

    ui->lineEdit_minHeight->setText(QString("%1").arg(datas.getMinCenterHeight()));

    // 临时变量
    hass = 1, hasl = 1, hasr = 1;

    // 若是单隧道综合结果输出
    /*hass = datas.getHasStraight();
    hasl = datas.getHasLeft();
    hasr = datas.getHasRight();*/
     
    if (!hass && !hasl && !hasr)
        return;

    // map反向遍历，正向为高度从小到大，插入图表是需要高度从大到小
    std::map<int, ClearanceItem>::reverse_iterator its = datas.getMaps().rbegin();
    std::map<int, ClearanceItem>::reverse_iterator itl = datal.getMaps().rbegin();
    std::map<int, ClearanceItem>::reverse_iterator itr = datar.getMaps().rbegin();
    int k = 0;
    while (its != datas.getMaps().rend())
    {
        std::pair<int, ClearanceItem> pairs = (*its);
        std::pair<int, ClearanceItem> pairl = (*itl);
        std::pair<int, ClearanceItem> pairr = (*itr);

        if (hass)
        {
            ui->tableWidget->setItem(k, Straight_Left_Val - 1, new QTableWidgetItem(QString("%1").arg(pairs.second.leftval)));
            ui->tableWidget->setItem(k, Straight_Right_Val - 1, new QTableWidgetItem(QString("%1").arg(pairs.second.rightval)));
            ui->tableWidget->setItem(k, Straight_Left_Pos - 1, new QTableWidgetItem(QString("%1").arg(pairs.second.leftpos)));
            ui->tableWidget->setItem(k, Straight_Right_Pos - 1, new QTableWidgetItem(QString("%1").arg(pairs.second.rightpos)));
        }

        if (hasl)
        {
            ui->tableWidget->setItem(k, LeftCurve_Left_Val - 1, new QTableWidgetItem(QString("%1").arg(pairl.second.leftval)));
            ui->tableWidget->setItem(k, LeftCurve_Right_Val - 1, new QTableWidgetItem(QString("%1").arg(pairl.second.rightval)));
            ui->tableWidget->setItem(k, LeftCurve_Left_Radius - 1, new QTableWidgetItem(QString("%1").arg(pairl.second.leftradius)));
            ui->tableWidget->setItem(k, LeftCurve_Right_Radius - 1, new QTableWidgetItem(QString("%1").arg(pairl.second.rightradius)));
            ui->tableWidget->setItem(k, LeftCurve_Left_Pos - 1, new QTableWidgetItem(QString("%1").arg(pairl.second.leftpos)));
            ui->tableWidget->setItem(k, LeftCurve_Right_Pos - 1, new QTableWidgetItem(QString("%1").arg(pairl.second.rightpos)));
        }

        if (hasr)
        {
            ui->tableWidget->setItem(k, RightCurve_Left_Val - 1, new QTableWidgetItem(QString("%1").arg(pairr.second.leftval)));
            ui->tableWidget->setItem(k, RightCurve_Right_Val - 1, new QTableWidgetItem(QString("%1").arg(pairr.second.rightval)));
            ui->tableWidget->setItem(k, RightCurve_Left_Radius - 1, new QTableWidgetItem(QString("%1").arg(pairr.second.leftradius)));
            ui->tableWidget->setItem(k, RightCurve_Right_Radius - 1, new QTableWidgetItem(QString("%1").arg(pairr.second.rightradius)));
            ui->tableWidget->setItem(k, RightCurve_Left_Pos - 1, new QTableWidgetItem(QString("%1").arg(pairr.second.leftpos)));
            ui->tableWidget->setItem(k, RightCurve_Right_Pos - 1, new QTableWidgetItem(QString("%1").arg(pairr.second.rightpos)));
        }

        its++;
        itl++;
        itr++;
        k++;
    }
    maketable = false;
}

void InputBridgeWidget::minheightChanged(QString a)
{
    if (maketable)
    {
        return;
    }
    hasedit = true;
}

void InputBridgeWidget::saveItemChanged(QTableWidgetItem* item)
{
    /* 如果正在建表, 不执行槽函数 */
    if (maketable)
    {
        return;
    }

    int row = item->row();
    int column = item->column();
    
    int key = ui->tableWidget->verticalHeaderItem(row)->data(Qt::DisplayRole).toInt();
    int value = item->text().toInt();
    if (value == 0)
    {
        QMessageBox::StandardButton rb = QMessageBox::question(NULL,tr("警告"),tr("您输入的是无效数字，应输入>0的整数"),QMessageBox::Yes | QMessageBox::No);

        if (rb == QMessageBox::Yes || rb == QMessageBox::No)        
            return;
    }
    //
    //qDebug() << "row:" << row << ", verticalHeaderItem:" << ui->tableWidget->verticalHeaderItem(row)->data(Qt::DisplayRole).toString();
    //qDebug() << "column:" << column << ", horizontalHeaderItem:" << ui->tableWidget->horizontalHeaderItem(column)->data(Qt::DisplayRole).toString();
    //qDebug() << item->text().toInt() << " " << item->text();

    switch (column)
    {
        case Straight_Left_Val - 1:        datas.getMaps().at(key).leftval = value;    break;
        case Straight_Right_Val - 1:       datas.getMaps().at(key).rightval = value;   break;  
        case Straight_Left_Pos - 1:        datas.getMaps().at(key).leftpos = value;    break; 
        case Straight_Right_Pos - 1:       datas.getMaps().at(key).rightpos = value;   break; 
        case LeftCurve_Left_Val - 1:       datal.getMaps().at(key).leftval = value;    break;
        case LeftCurve_Right_Val - 1:      datal.getMaps().at(key).rightval = value;   break;  
        case LeftCurve_Left_Radius - 1:    datal.getMaps().at(key).leftradius = value; break;
        case LeftCurve_Right_Radius - 1:   datal.getMaps().at(key).rightradius = value;break;
        case LeftCurve_Left_Pos - 1:       datal.getMaps().at(key).leftpos = value;    break; 
        case LeftCurve_Right_Pos - 1:      datal.getMaps().at(key).rightpos = value;   break; 
        case RightCurve_Left_Val - 1:      datar.getMaps().at(key).leftval = value;    break;
        case RightCurve_Right_Val - 1:     datar.getMaps().at(key).rightval = value;   break;  
        case RightCurve_Left_Radius - 1:   datar.getMaps().at(key).leftradius = value; break;
        case RightCurve_Right_Radius - 1:  datar.getMaps().at(key).rightradius = value;break;
        case RightCurve_Left_Pos - 1:      datar.getMaps().at(key).leftpos = value;    break; 
        case RightCurve_Right_Pos - 1:     datar.getMaps().at(key).rightpos = value;   break; 
        default: break;
    }
    hasedit = true;
}

void InputBridgeWidget::canBeModified()
{
    hasedit = false;
    canSave(true);
}

void InputBridgeWidget::canSave(bool can)
{
    if (can)
    {
        ui->tableWidget->setEditTriggers(QAbstractItemView::AllEditTriggers);
        ui->lineEdit_minHeight->setEnabled(true);
    }
    else
    {
        ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->lineEdit_minHeight->setEnabled(false);
    }
    ui->modifyButton->setEnabled(!can);
    ui->saveButton->setEnabled(can);
}

void InputBridgeWidget::saveChanges()
{
    QString currentcollectdate = ui->dateEdit->dateTime().toString("yyyyMMdd");
    __int64 taskid = TaskDAO::getTaskDAOInstance()->getTaskID(tunnelid, currentcollectdate);
    QString tunnelname = ui->tunnelname->text();
    qDebug() << "collectdate:" << currentcollectdate << ", tunnelid:" << tunnelid << ", tunnelname:" << tunnelname << ", taskid:" << taskid << ", tasktunnelid:" << tasktunnelid;

    if (tasktunnelid < 0)
    {
        ui->statusArea->append(QObject::tr("首次输入桥梁采集任务组(任务工程)，创建新桥梁采集任务组。。。"));
        // 没有这个任务组
        if (taskid < 0)
            taskid = TaskDAO::getTaskDAOInstance()->addTask(1, currentcollectdate, true, tunnelname+currentcollectdate);

        if (taskid < 0)
        {
            ui->statusArea->append(QObject::tr("创建新桥梁采集任务组失败！重新保存桥梁任务%1失败，数据库中无法添加采集日期为%2的任务隧道组").arg(tunnelname).arg(currentcollectdate));
            return;
        }
        else
        {
            ui->statusArea->append(QObject::tr("创建新桥梁采集任务组成功！"));
        }

        ui->statusArea->append(QObject::tr("创建新桥梁采集任务（工程中的隧道任务）。。。"));
        // 有当天采集的任务组，但没有这个隧道任务
        if (TaskTunnelDAO::getTaskTunnelDAOInstance()->addTaskTunnel(taskid, tunnelid, 0.5, true, true, 0) == 0)
        {
            tasktunnelid = TaskTunnelDAO::getTaskTunnelDAOInstance()->getTaskTunnelID(tunnelid, currentcollectdate);
            ui->statusArea->append(QObject::tr("创建新桥梁采集任务（工程中的隧道任务）成功，tasktunnelid=%1！").arg(tasktunnelid));
        }
        else
        {
            ui->statusArea->append(QObject::tr("创建新桥梁采集任务（工程中的隧道任务）失败！重新保存桥梁采集任务%1失败，数据库中采集日期为%2的任务组存在，但无法添加采集隧道").arg(tunnelname).arg(currentcollectdate));
            return;
        }
    }
        
    if (tasktunnelid >= 0)
    {
        int ret0 = TaskTunnelDAO::getTaskTunnelDAOInstance()->updateTaskTunnel(tasktunnelid, taskid, tunnelid, 0.5, true, true, 0);
        int height = ui->lineEdit_minHeight->text().toInt();
        datas.setMinCenterHeight(height);
        datal.setMinCenterHeight(height);
        datar.setMinCenterHeight(height);
        int ret1 = ClearanceOutputDAO::getClearanceOutputDAOInstance()->clearanceDataToDBData(datas, tasktunnelid, ClearanceType::Straight_Smallest);
        int ret2 = ClearanceOutputDAO::getClearanceOutputDAOInstance()->clearanceDataToDBData(datal, tasktunnelid, ClearanceType::LeftCurve_Smallest);
        int ret3 = ClearanceOutputDAO::getClearanceOutputDAOInstance()->clearanceDataToDBData(datar, tasktunnelid, ClearanceType::RightCurve_Smallest);

        qDebug() << "clearanceDataToDBData" << ret1 << ret2 << ret3;
        //straightdata.showMaps();

        ui->statusArea->append(QObject::tr("重新保存桥梁采集任务%1并添加到数据库完成").arg(tunnelname));
    }
    else 
    {
        ui->statusArea->append(QObject::tr("重新保存桥梁采集任务%1失败，桥梁采集任务（工程中的隧道任务）找不到ID为%2的tasktunnel").arg(tunnelname).arg(tasktunnelid));
    }

    ui->saveButton->setEnabled(false);
    ui->modifyButton->setEnabled(true);
    hasedit = false;
    canSave(false);
}

/**
 * 界面转换公有槽函数，接收ManageTunnelWidget发出的编辑桥梁限界的信号
 * @param tunnelid 隧道ID
 */
void InputBridgeWidget::slotBridgeToEdit(int newtunnelid, QString newtunnelname)
{
    this->tunnelid = newtunnelid;
    qDebug() << "slot in Widget inputBridgeClearance" << tunnelid;

    bridgeCollectModel = TaskTunnelDAO::getTaskTunnelDAOInstance()->getTaskTunnels_OneTunnel(tunnelid);

    QString collectdate;
    bool ret = false;
    if (bridgeCollectModel->rowCount() > 0)
    {
        ret = true;
        // 得到采集隧道ID
        tasktunnelid = bridgeCollectModel->record(0).value(0).toLongLong();
        // 采集时间
        collectdate = bridgeCollectModel->record(0).value(2).toString();

        ret = ClearanceOutputDAO::getClearanceOutputDAOInstance()->DBDataToClearanceData(datas, tasktunnelid, ClearanceType::Straight_Smallest);
        ret = ClearanceOutputDAO::getClearanceOutputDAOInstance()->DBDataToClearanceData(datal, tasktunnelid, ClearanceType::LeftCurve_Smallest);
        ret = ClearanceOutputDAO::getClearanceOutputDAOInstance()->DBDataToClearanceData(datar, tasktunnelid, ClearanceType::RightCurve_Smallest);
    }
    else
    {
        ret = false;
        // 得到采集隧道ID
        tasktunnelid = -1;
        // 采集时间
        collectdate = QDate::currentDate().toString("yyyyMMdd");
    }
    if (ret)
    {
        // 更新限界值并显示到界面上
        updateBridgeClearanceData();
        ui->dateEdit->setDate(QDate::fromString(collectdate));
        ui->statusArea->append(tr("加载桥梁%1限界成功").arg(newtunnelname));
    }
    else
    {
        initBridgeClearanceData();
        ui->dateEdit->setDate(QDate::currentDate());
        ui->statusArea->append(tr("加载已有桥梁%1限界失败，请输入最新限界！").arg(newtunnelname));
    }
    ui->tunnelname->setText(newtunnelname);
    // 初始没有做任何编辑
    hasedit = false;
    canSave(false);
}

void InputBridgeWidget::finishEdit()
{
    // 保存桥梁限界值
    if (hasedit)
    {
        QMessageBox::StandardButton rb = QMessageBox::warning(NULL,tr("警告"),tr("您所做的编辑还没有保存，确认退出？点击“No”返回"),QMessageBox::Yes | QMessageBox::No);

        if (QMessageBox::No)        
            return;
    }

    // 返回基础桥隧管理界面
    emit signalEndEdit();
}
