#include "input_bridge_clearance.h"
#include "ui_input_bridge_clearance.h"
#include <QTableWidgetItem>
#include <QDebug>
#include <QHeaderView>
#include <QSqlRecord>
#include <QMessageBox>

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

    bool ret = data.initMaps();
    if (ret == false)
    {
        qDebug() << "can init clearance value map:";
        return;
    }

    heightlength = data.getMaps().size();
    ui->tableWidget->setColumnCount(7);
    ui->tableWidget->setRowCount(heightlength);
    ui->tableWidget->verticalHeader()->setVisible(false); //隐藏行表头
    ui->tableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem(QObject::tr("轨面高度")));
    ui->tableWidget->setHorizontalHeaderItem(1, new QTableWidgetItem(QObject::tr("尺寸-左侧")));
    ui->tableWidget->setHorizontalHeaderItem(2, new QTableWidgetItem(QObject::tr("尺寸-右侧")));
    ui->tableWidget->setHorizontalHeaderItem(3, new QTableWidgetItem(QObject::tr("曲线半径-左侧")));
    ui->tableWidget->setHorizontalHeaderItem(4, new QTableWidgetItem(QObject::tr("曲线半径-右侧")));
    ui->tableWidget->setHorizontalHeaderItem(5, new QTableWidgetItem(QObject::tr("控制点-左侧")));
    ui->tableWidget->setHorizontalHeaderItem(6, new QTableWidgetItem(QObject::tr("控制点-右侧")));
    std::map <int,ClearanceItem>::reverse_iterator it = data.getMaps().rbegin();
    int i = 0;
    while (it != data.getMaps().rend())
    {
        std::pair<int,ClearanceItem> pair = (*it);
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(QString("%1").arg(pair.first)));
        it++;
        i++;
    }
    for(int i = 0; i < heightlength; i++)
    {
        ui->tableWidget->item(i, 0)->setFlags(Qt::NoItemFlags);
    }
    maketable = false;
}

/**
 * 更新桥梁限界数据（只更新内部数据，不修改表头）
 */
void InputBridgeWidget::updateBridgeClearanceData()
{
    maketable = true;
    std::map <int,ClearanceItem>::reverse_iterator it = data.getMaps().rbegin();
    int i = 0;
    while (it != data.getMaps().rend())
    {
        std::pair<int,ClearanceItem> pair = (*it);
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(QString("%1").arg(pair.second.leftval)));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(QString("%1").arg(pair.second.rightval)));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(QString("%1").arg(pair.second.leftradius)));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(QString("%1").arg(pair.second.rightradius)));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(QString("%1").arg(pair.second.leftpos)));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(QString("%1").arg(pair.second.rightpos)));
        it++;
        i++;
    }
    maketable = false;
}

void InputBridgeWidget::saveItemChanged(QTableWidgetItem* item)
{
    /* 如果正在见表, 不执行槽函数 */
    if (maketable)
    {
        return;
    }

    //
    QMessageBox::warning(this,tr("提示"), tr("此处尚不能输入桥梁限界，敬请期待！"),
                                              QMessageBox::Yes | QMessageBox::No);
    /*QString itemstr = item->text();

    QString stuid = record.value(0).toString();

    bool ok;
    float itemscore = itemstr.toFloat(&ok);
    // 判断是否在成绩项中，是否为float单精度型 
    if (ok == false || item->column() < 2)
    {
        QMessageBox::warning(this,tr("错误"), tr("不能修改") + title + tr("为小数"));
        // 不做修改
        maketable = 0;
        item->setText(record.value(item->column()).toString());
        maketable = 1;
        return;
    }

    //qDebug() << stuid + QString("_") + title + QString("_") + itemstr +  QString("_") + QString::number(year);
    modifiedItems << stuid + QString("_") + title + QString("_") + itemstr + QString("_") + QString::number(year);
    ui->saveButton->setEnabled(true);
    ui->cancelButton->setEnabled(true);*/
}

void InputBridgeWidget::canBeModified()
{
    ui->tableWidget->setEditTriggers(QAbstractItemView::AllEditTriggers);
    ui->modifyButton->setEnabled(false);
}

void InputBridgeWidget::saveChanges()
{
    /*if (modifiedItems.size() == 0)
    {
        QMessageBox::warning(this,tr("提示"), tr("没有做任何修改"));
    }
    else
    {
        bool modifysuccess = true;
        QString wrongmessage = "";
        QString stritem;
        for (int i = 0; i < modifiedItems.size(); i++)
        {
            wrongmessage += QObject::tr("不能修改:\n");
            stritem = modifiedItems.at(i);
            int result = data.updateToMapVals();
            if (result != 0)
            {
                modifysuccess = false;
                wrongmessage += ", " + stritem + "\n";
            }
        }
        if (modifysuccess == false)
            QMessageBox::warning(this,tr("错误"), wrongmessage);
        else
            QMessageBox::warning(this,tr("提示"), tr("修改成功！"));
    }
    modifiedItems.clear();
    // qDebug() << QObject::tr("modifiedItems已清空");*/
    ui->saveButton->setEnabled(false);
    ui->modifyButton->setEnabled(true);

}

/*void InputBridgeWidget::cancelModify()
{
    modifiedItems.clear();
    ui->saveButton->setEnabled(false);
    ui->modifyButton->setEnabled(true);
    updateBridgeClearanceData();
}*/

/**
 * 界面转换公有槽函数，接收ManageTunnelWidget发出的编辑桥梁限界的信号
 * @param tunnelid 隧道ID
 */
void InputBridgeWidget::slotBridgeToEdit(int newtunnelid, QString newtunnelname)
{
    this->tunnelid = newtunnelid;
    qDebug() << "slot in Widget inputBridgeClearance" << tunnelid;

    bridgeCollectModel = TaskTunnelDAO::getTaskTunnelDAOInstance()->getTaskTunnelsOneLineRecently(tunnelid);

    QString collectdate;
    bool ret = false;
    if (bridgeCollectModel->rowCount() == 0)
    {
        ret = true;
        // 得到采集隧道ID
        tasktunnelid = bridgeCollectModel->record(0).value(0).toLongLong();
        // 采集时间
        collectdate = bridgeCollectModel->record(0).value(2).toString();

        ret = ClearanceOutputDAO::getClearanceOutputDAOInstance()->DBDataToClearanceData(data, tasktunnelid, ClearanceType::Straight_Smallest);
    }
    if (ret)
    {
        // 更新限界值并显示到界面上
        updateBridgeClearanceData();
        ui->dateEdit->setDate(QDate::fromString(collectdate));
        ui->status->setText(tr("加载桥梁%1限界成功").arg(newtunnelname));
    }
    else
    {
        ui->dateEdit->setDate(QDate::currentDate());
        ui->status->setText(tr("加载已有桥梁%1限界失败，请输入最新限界！").arg(newtunnelname));
    }
    ui->tunnelname->setText(newtunnelname);
    // 初始没有做任何编辑
    hasedit = false;
}

void InputBridgeWidget::finishEdit()
{
    // 保存桥梁限界值

    // 返回基础桥隧管理界面
    emit signalEndEdit();
}
