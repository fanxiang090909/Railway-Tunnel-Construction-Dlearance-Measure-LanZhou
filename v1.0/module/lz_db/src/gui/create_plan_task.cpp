#include "create_plan_task.h"
#include "ui_create_plan_task.h"
#include "hardware_config.h"

#include "lz_project_access.h"

#include "daotunnel.h"
#include "daoline.h"

#include <QSqlRelationalDelegate>
#include <QTableWidgetItem>
#include <QSqlRecord>
#include <QMessageBox>
#include <QDebug>
#include <QTime>
#include <QDateTime>
#include <QTableWidget>
#include <QString>
#include <QTextCodec>
#include <QRadioButton>
#include <QListView>
#include <QStringListModel>
#include <QFileDialog>
#include <QFile>
#include <QFileInfo>
#include <QDateTime>
#include <QComboBox>

#include <QDir>
//QString globaldata;//声明一个全局变量用于窗体间传值

/**
 * 隧道任务分配界面类实现
 * @author 熊雪
 * @author 范翔
 * @version 1.0.0
 * @date 2013-9-22
 */
// 隧道任务界面表格列数
int PlanTaskWidget::TABLE_COLUMN_COUNT = 11;

PlanTaskWidget::PlanTaskWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlanTaskWidget)
{
    ui->setupUi(this);
    QTextCodec::setCodecForTr(QTextCodec::codecForName("GBK")); //当QString变量中含有中文时，需要增加这两行代码
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));

    linesModel = NULL;
    tunnelsModel = NULL;
    task =NULL;
    tunnelcheck =NULL;

    currentRow = 0;

    updateDateTime();

    loadLinesData();
    loadTunnelData();

    prepareTaskTunnelsView();

    /* 线路及隧道数据列表更新信号槽 */
    connect(ui->linesView, SIGNAL(clicked(QModelIndex)), this, SLOT(updateTunnelsView(const QModelIndex &)));

    /* 增加移除隧道任务项槽--1单击按钮事件 */
    connect(ui->addTaskTunnelButton, SIGNAL(clicked()), this, SLOT(addTaskTunnel()));
    connect(ui->removeTaskTunnelButton, SIGNAL(clicked()), this, SLOT(removeTaskTunnel()));
    /* 增加移除隧道任务项槽--2双击表格行事件 */
    connect(ui->tunnelsView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(addTaskTunnel(const QModelIndex &)));
    //connect(ui->taskTunnelTableWidget, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(removeTaskTunnel(const QModelIndex &)));

    /* 全部增加，全部移除隧道任务项槽 */
    connect(ui->addAllTaskTunnelsButton, SIGNAL(clicked()), this, SLOT(addAllSellectedTaskTunnels()));
    connect(ui->removeAllTaskTunnelsButton, SIGNAL(clicked()), this, SLOT(removeAllSellectedTaskTunnels()));
    connect(ui->clearbutton, SIGNAL(clicked()), this, SLOT(removeAllTaskTunnels()));

    /* 隧道任务顺序操作，翻转，前移，后移 */
    connect(ui->turnOrderButton, SIGNAL(clicked()), this, SLOT(turnTaskTunnelsOrder()));
    connect(ui->previousButton, SIGNAL(clicked()), this, SLOT(turnToPreviousTaskTunnel()));
    connect(ui->nextButton, SIGNAL(clicked()), this, SLOT(turnToNextTaskTunnel()));

    /* 打开过去的xml文件按钮信号槽 */
    //connect(ui->openTaskFileButton, SIGNAL(clicked()), this, SLOT(openTaskFile()));
    /* 生成xml任务文件 */
    connect(ui->makeTaskFileButton,SIGNAL(clicked()),this,SLOT(makeTaskFile()));
    connect(ui->searchLineEdit,SIGNAL(textEdited(QString)),this,SLOT(showAllLines()));
}

PlanTaskWidget::~PlanTaskWidget()
{
    delete linesModel;
    delete tunnelsModel;
    delete ui;
    delete task;
    delete tunnelcheck;

}

// 更新采集日期
void PlanTaskWidget::updateDateTime()
{
    ui->dateLabel->setText(QDate::currentDate().toString("yyyyMMdd"));
}

/**
 * 加载左侧QListView的线路数据
 */
void PlanTaskWidget::loadLinesData()
{
    linesModel = LineDAO::getLineDAOInstance()->getLineNames();
    if (linesModel == NULL)
        return;
    ui->linesView->setModel(linesModel);
    ui->linesView->setAutoScroll(true);
    ui->linesView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tunnelsView->setSelectionBehavior(QAbstractItemView::SelectRows);
}

/**
 * 加载中间QTableView的隧道数据
 */
void PlanTaskWidget::loadTunnelData()
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

    tunnelsModel = TunnelDAO::getTunnelDAOInstance()->getTunnels(lineid);

    ui->tunnelsView->setModel(tunnelsModel);
    //ui->tunnelsView->setItemDelegate(new QSqlRelationalDelegate(this));//VS2012中加了就错，why
    //ui->studentView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tunnelsView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tunnelsView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tunnelsView->resizeColumnsToContents();

    ui->tunnelsView->horizontalHeader()->setStretchLastSection(true);
    //    ui->taskTunnelTableWidget->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    //    ui->taskTunnelTableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->taskTunnelTableWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->taskTunnelTableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->taskTunnelTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    /* 隐藏tunnelsModel模型中的列 */
    ui->tunnelsView->hideColumn(Tunnel_ID);
    //ui->tunnelsView->hideColumn(Tunnel_ID_std);
    ui->tunnelsView->hideColumn(Tunnel_is_new_std);
    ui->tunnelsView->hideColumn(Tunnel_line_type_std);
    ui->tunnelsView->hideColumn(Tunnel_is_valid);
    ui->tunnelsView->hideColumn(Tunnel_is_double_line);
    ui->tunnelsView->hideColumn(Tunnel_is_downlink);
}

/**
 * 加载右侧QTableWidget的隧道任务表头数据
 */
void PlanTaskWidget::prepareTaskTunnelsView()
{
    if (tunnelsModel == NULL)
        return;

    int columnCount = tunnelsModel->columnCount();
    //int rowCount = integrateScoresModel->rowCount();
    QString coltitlestr;

    /* 创建表格 */
    ui->taskTunnelTableWidget->setColumnCount(TABLE_COLUMN_COUNT);
    //ui->integrate_score_widget->setRowCount(rowCount);
    bool static flag = true;
    // 不把模型中的是否有效位填充
    for (int i = 0; i < columnCount - 1; i++)
    {
        // 去掉隧道编号
        if (flag && i == Tunnel_ID_std)
        {
            i--;
            flag = false;
            continue;
        }
        coltitlestr = tunnelsModel->headerData(i, Qt::Horizontal).toString();
        QTableWidgetItem * columntablewidgetitem = new QTableWidgetItem();
        columntablewidgetitem->setText(coltitlestr);
        ui->taskTunnelTableWidget->setHorizontalHeaderItem(i, columntablewidgetitem);
    }
    flag = true;
    coltitlestr = QObject::tr("是否正常");
    QTableWidgetItem * columntablewidgetitem = new QTableWidgetItem();
    columntablewidgetitem->setText(coltitlestr);
    ui->taskTunnelTableWidget->setHorizontalHeaderItem(TABLE_COLUMN_COUNT - 1, columntablewidgetitem);
}

void PlanTaskWidget::updateTunnelsView(const QModelIndex &index)
{
    if (index.isValid())
    {
        QString linename = index.data().toString();
        int lineid = LineDAO::getLineDAOInstance()->getLineID(linename);
        if (lineid == -1)
        {
            return;
        }

        tunnelsModel = TunnelDAO::getTunnelDAOInstance()->getTunnels(lineid);

        currentRow = 0;
    }
    updateDateTime();
    return;
}

/* 私有槽函数 */
/* 选择隧道添加至隧道任务中，增加，删除隧道任务 */
void PlanTaskWidget::addTaskTunnel()
{
    QModelIndex index = ui->tunnelsView->currentIndex();
    addTaskTunnel(index);
}

void PlanTaskWidget::removeTaskTunnel()
{
    QModelIndex index = ui->taskTunnelTableWidget->currentIndex();
    removeTaskTunnel(index);
}

void PlanTaskWidget::addTaskTunnel(const QModelIndex &index)
{
    int columnCount = tunnelsModel->columnCount();

    if (index.isValid())
    {
        /******TODO*****/
        // 如果已经加入了不能重复再加

        QSqlRecord record = tunnelsModel->record(index.row());

        /* 填充表格内容 */
        QString itemstr;

        int i = ui->taskTunnelTableWidget->rowCount();
        ui->taskTunnelTableWidget->insertRow(i);

        // 表项
        bool static flag = true;
        bool static isDouble = true;
        int static linetype = 0;

        for (int j = 0; j < columnCount - 1; j++)
        {

            itemstr = record.value(j).toString();
            QTableWidgetItem * tablewidgetitem = new QTableWidgetItem();

            // 去掉隧道编号
            if (flag && j == Tunnel_ID)
            {
                j--;
                flag = false;
                continue;
            }
            if (j == Tunnel_is_double_line)
                if (itemstr.compare("0") == 0)
                    isDouble = false; // 单线
                else
                    isDouble = true;  // 双线

            if (j == Tunnel_line_type_std)
                if (itemstr.compare("0") == 0)
                    linetype = 0; // 电力牵引
                else
                    linetype = 1; // 内燃牵引

            // "是"或"否"的显示字串变换
            if (j > 4 && j != 6 && j < columnCount - 2)
            {
                if (itemstr == "0")
                    itemstr = QObject::tr("否");
                else if (itemstr == "1")
                    itemstr = QObject::tr("是");
            }
            else if (j == 6)
            {
                if (linetype == 0)
                    itemstr = QObject::tr("电力牵引");
                else if (linetype == 1)
                    itemstr = QObject::tr("内燃牵引");
            }
            tablewidgetitem->setText(itemstr);
            ui->taskTunnelTableWidget->setItem(i, j, tablewidgetitem);
        }
        flag = true;

        QComboBox *combo = new QComboBox();
        if (isDouble)
        {
            combo->addItem(QObject::tr("正常双线"));
            combo->addItem(QObject::tr("非正常双线"));
        }
        else
        {
            combo->addItem(QObject::tr("正序单线"));
            combo->addItem(QObject::tr("逆序单线"));
        }
        ui->taskTunnelTableWidget->setCellWidget(i, TABLE_COLUMN_COUNT - 1, combo);
    }
    ui->taskTunnelTableWidget->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->taskTunnelTableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->taskTunnelTableWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->taskTunnelTableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    // HorizontalScrollBarPolicy设置为needed和alwayson均可，不要设置成off
}

void PlanTaskWidget::removeTaskTunnel(const QModelIndex &index)
{
    if (index.isValid())
    {
        ui->taskTunnelTableWidget->removeRow(index.row());
    }
    else
    {
        QMessageBox::warning(this, tr("提示"), tr("请选择某行!"));
        return;
    }
}

void PlanTaskWidget::addAllSellectedTaskTunnels()
{
    // @author 范翔注释，改成全部添加
    /*QItemSelectionModel *selections = ui->tunnelsView->selectionModel();
    QModelIndexList selected = selections->selectedRows();

    foreach (QModelIndex index, selected)
    {
        addTaskTunnel(index);
    }*/

    for (int i = 0; i < ui->tunnelsView->model()->rowCount(); i++)
    {
        QModelIndex indexA = ui->tunnelsView->model()->index(i, 0, QModelIndex());
        addTaskTunnel(indexA);
    }
   
}

void PlanTaskWidget::removeAllSellectedTaskTunnels()
{
    // 部分删除选择行判断
    if (ui->taskTunnelTableWidget->selectedRanges().isEmpty() == false)
    {
        QList<QTableWidgetSelectionRange> ranges = ui->taskTunnelTableWidget->selectedRanges();
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
            QMessageBox::warning(this, tr("提示"), tr("请选择相邻两行或多行删除！"));
            return;
        }
        // 删除多余
        for (int i = topRow; i <= bottomRow; i++)
        {
            ui->taskTunnelTableWidget->removeRow(topRow);
        }
    }
    else
    {
        QMessageBox::warning(this, tr("提示"), tr("请选择相邻两行或多行!"));
        return;
    }
}

void PlanTaskWidget::removeAllTaskTunnels()
{
    int l = ui->taskTunnelTableWidget->rowCount();
    for (int i = 0; i < l; i++)
        ui->taskTunnelTableWidget->removeRow(0);
    currentRow = 0;
}

/* 隧道任务顺序操作翻转前移后移 */
void PlanTaskWidget::turnTaskTunnelsOrder()
{
    // 部分翻转选择行判断
    //if (ui->taskTunnelTableWidget->selectedRanges().isEmpty() == false)
    //{
    //@author 范翔改成全部翻转
        /*QList<QTableWidgetSelectionRange> ranges = ui->taskTunnelTableWidget->selectedRanges();
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
        if (!sellectvalid && (bottomRow - topRow < 1))
        {
            QMessageBox::warning(this, tr("提示"), tr("请选择相邻两行或多行翻转！"));
            return;
        }

        // 部分翻转
        //int rowCount = bottomRow - topRow + 1;
        int columnCount = tunnelsModel->columnCount();
        */
        int topRow = 0;
        int bottomRow = ui->taskTunnelTableWidget->rowCount() - 1;

        if (bottomRow - topRow < 1)
        {
            QMessageBox::warning(this, tr("提示"), tr("任务隧道数目小于2个，不能翻转！"));
            return;
        }
        int rowCount = 0;
        int columnCount = tunnelsModel->columnCount();

        //qDebug() << columnCount;//列数
        //qDebug() << rowCount;//总行数
        for (int i = topRow; i < bottomRow; i++)
        {
            ui->taskTunnelTableWidget->insertRow(bottomRow + 1);
            QTableWidgetItem* item;
            QString itemstr = "";

            for (int j = 0; j < columnCount - 1; j++)
            {
                item = ui->taskTunnelTableWidget->item(i, j);
                itemstr = item->text();
                //qDebug() << item->column() << item->row() << itemstr;

                QTableWidgetItem * tablewidgetitem = new QTableWidgetItem();
                tablewidgetitem->setText(itemstr);
                ui->taskTunnelTableWidget->setItem(bottomRow + 1, j, tablewidgetitem);
                // 上面代码时改变前9列的数据，下面的代码时改变第10列的combox数据
                // 使第10列也一样翻转、前移、后移
                QWidget * widget = ui->taskTunnelTableWidget->cellWidget(i,10);//获得widget
                QComboBox *combox = (QComboBox*)widget;//强制转化为QComboBox
                QString itemstr1 = combox->currentText();
                QString itemstr2 = combox->itemText(1);
                QComboBox *combo = new QComboBox();
                combo->addItem(itemstr1);
                combo->addItem(itemstr2);
                ui->taskTunnelTableWidget->setCellWidget(bottomRow + 1, 10, combo);
            }
        }
        for (int i = topRow; i < bottomRow; i++)
        {
            ui->taskTunnelTableWidget->removeRow(topRow);
        }
    //}
    /*else
    {
        QMessageBox::warning(this, tr("提示"), tr("请选择相邻两行或多行!"));
        return;
    }*/
}

void PlanTaskWidget::turnToPreviousTaskTunnel()//前移，即前一个
{
    QModelIndex index = ui->taskTunnelTableWidget->currentIndex();
    if (!index.isValid())
        return;

    int columnCount = ui->taskTunnelTableWidget->columnCount();
    if (index.row() == 0)
    {
        QMessageBox::critical(0, QObject::tr("提示"),
                              QObject::tr("已经是第一项任务，无法前移!"),
                              QMessageBox::Ok);
        return;
    }

    ui->taskTunnelTableWidget->insertRow(index.row() - 1);

    //qDebug() << index.row();

    QTableWidgetItem* item;
    QString itemstr = "";
    for (int j = 0; j <columnCount - 1; j++)
    {
        item = ui->taskTunnelTableWidget->item(index.row() + 1, j);
        itemstr = item->text();
        QTableWidgetItem * tablewidgetitem = new QTableWidgetItem();
        tablewidgetitem->setText(itemstr);
        ui->taskTunnelTableWidget->setItem(index.row() - 1, j, tablewidgetitem);
        QWidget * widget=ui->taskTunnelTableWidget->cellWidget(index.row(),10);//获得widget
        QComboBox *combox=(QComboBox*)widget;//强制转化为QComboBox
        QString itemstr1=combox->currentText();
        QString itemstr2=combox->itemText(1);
        QComboBox *combo1 =new QComboBox();
        combo1->addItem(itemstr1);
        combo1->addItem(itemstr2);
        ui->taskTunnelTableWidget->setCellWidget(index.row() - 1, 10, combo1);
    }
    ui->taskTunnelTableWidget->removeRow(index.row() + 1);

}

void PlanTaskWidget::turnToNextTaskTunnel()//下移，即下一个
{
    QModelIndex index = ui->taskTunnelTableWidget->currentIndex();
    if (!index.isValid())
        return;

    int columnCount = ui->taskTunnelTableWidget->columnCount();
    //qDebug() << columnCount;
    if (index.row() == ui->taskTunnelTableWidget->rowCount() - 1)
    {
        QMessageBox::critical(0, QObject::tr("提示"),
                              QObject::tr("已经是最后一项任务，无法后移!"),
                              QMessageBox::Ok);
        return;
    }

    ui->taskTunnelTableWidget->insertRow(index.row() + 2);

    //qDebug() << index.row();

    QTableWidgetItem* item;
    QString itemstr = "";
    int j = 0;
    for (j = 0; j < columnCount - 1; j++)
    {
        item = ui->taskTunnelTableWidget->item(index.row(), j);
        itemstr = item->text();
        QTableWidgetItem * tablewidgetitem = new QTableWidgetItem();
        tablewidgetitem->setText(itemstr);
        ui->taskTunnelTableWidget->setItem(index.row() + 2, j, tablewidgetitem);
        QWidget * widget=ui->taskTunnelTableWidget->cellWidget(index.row(),10);//获得widget
        QComboBox *combox=(QComboBox*)widget;//强制转化为QComboBox
        QString itemstr1=combox->currentText();
        QString itemstr2=combox->itemText(1);
        QComboBox *combo2 =new QComboBox();
        combo2->addItem(itemstr1);
        combo2->addItem(itemstr2);
        ui->taskTunnelTableWidget->setCellWidget(index.row() + 2, 10, combo2);
    }
    ui->taskTunnelTableWidget->removeRow(index.row());
}

/* 打开浏览xml任务文件 */
/*void PlanTaskWidget::openTaskFile()
{
    openFileDir = MasterSetting::getSettingInstance()->getParentPath() + "/plan_tasks";
    QString filename = QFileDialog::getOpenFileName(this, QObject::tr("打开计划隧道文件"), openFileDir, "Document files (*.plan)");
    //    QFileInfo info(filename);
    //    if(!info.exists())//判断计划隧道任务文件名是否存在，既然打开了就一定存在
    //    {
    //        ui->textEdit->setTextColor(Qt::red);
    //        ui->textEdit->setText(tr("计划隧道文件暂未生成"));
    //    }
    //QProcess::execute(tr("notepad.exe 兰青.xml"));
    if (filename.compare("") == 0)
    {
        ui->textEdit->setTextColor(Qt::red);
        ui->textEdit->setText(tr("请选择一个计划隧道文件任务。"));
    }
    else
    {
        QProcess *process =new QProcess();
        process->start(QString("notepad.exe %1").arg(filename));//非阻塞
    }
    //QProcess::execute("notepad.exe");//容易阻塞
    //直接调用计算机的notepad.exe(记事本程序),直接打开task.xml文件
    // "notepad.exe %1" ,'*把键值设为"notepad.exe %1"，%1指的是要打开的文件
}*/

/**
 * 生成任务文件xml
 * @author xiongxue
 */
/*尽量不要在xml中使用中文*/
void PlanTaskWidget::makeTaskFile()//生成task.xml任务文件
{
    // 如果没有输入脉冲数和配置车厢正反，不能生成
    if (ui->pulseNumEdit->text().trimmed().compare("") == 0 || ui->pulseNumEdit->text().trimmed().toInt() <= 0)
    {
        QMessageBox::critical(0, QObject::tr("提示"),
                              QObject::tr("脉冲没有输入，不能生成文件!"),
                              QMessageBox::Ok);
        return;
    }
    if (ui->radioButton_5->isChecked() == false && ui->radioButton_6->isChecked() == false)
    {
        QMessageBox::critical(0, QObject::tr("提示"),
                              QObject::tr("车厢正反未选择，不能生成文件!"),
                              QMessageBox::Ok);
        return;
    }

    bool ismakefile = false;
    int row=ui->taskTunnelTableWidget->rowCount();//获得taskTunnelTableWidget控件中的行数
    if(row == 0)
    {
        ui->textEdit->setTextColor(Qt::red);
        ui->textEdit->setText(tr("请先选择隧道。"));
        return;
    }

    // 【多条线路一个文件】 @author 范翔添加
    int templineid = -1;
    QString multiline_name = "";

    LzProjectAccess::getLzProjectAccessInstance()->getLzPlanList(LzProjectClass::Main).clear();
    PlanTask tmp;

    //int column=ui->taskTunnelTableWidget->columnCount();////获得taskTunnelTableWidget控件中的列数
    //qDebug()<<"行数"<<row;
    // qDebug()<<"列数"<<column;
    QTextCodec *code = QTextCodec::codecForName("gb2312");
    int i=0;
    for(i=0;i<row;i++)//TabWidget默认的编号是从0开始的
    {
        QString tmpstr = ui->taskTunnelTableWidget->item(i,9)->text();
        // 如果code为0，表示在运行的机器上没有装gb2312字符集。不过一般的中文windows系统中都安装了这一字符集
        if (code)    
            tmp.linename = code->fromUnicode(tmpstr).data();
        else
            tmp.linename = tmpstr.toStdString();//获得TabWidget中某行某列的值

        //qDebug() << QString::fromLocal8Bit(tmp.linename.c_str()) << ui->taskTunnelTableWidget->item(i,9)->text();
        tmp.linenum = LineDAO::getLineDAOInstance()->getLineID(QString(tmpstr));
        tmp.tunnelnum = ui->taskTunnelTableWidget->item(i,0)->text().toInt();
            tmpstr = ui->taskTunnelTableWidget->item(i,1)->text();
        if (code)    
            tmp.tunnelname = code->fromUnicode(tmpstr).data();
        else
            tmp.tunnelname = tmpstr.toStdString();//获得TabWidget中某行某列的值

        tmp.startdistance = ui->taskTunnelTableWidget->item(i,3)->text().toLongLong();
        tmp.enddistance = ui->taskTunnelTableWidget->item(i,4)->text().toLongLong();
        if(ui->radioButton_5->isChecked())//检测表示为正向的radioButton_5按钮是否被选中
            tmp.traindirection = true;
        else//则表示选中反向这个radioButton_6这个按钮
            tmp.traindirection = false;
        if(ui->taskTunnelTableWidget->item(i,5)->text().compare(tr("是")) == 0)//检测表示为正向的radioButton_5按钮是否被选中
            tmp.newline = true;
        else//则表示选中反向这个radioButton_6这个按钮
            tmp.newline = false;
        if (ui->taskTunnelTableWidget->item(i,6)->text().compare(tr("内燃牵引")) == 0) // 内燃牵引
            tmp.linetype = 1;
        else
            tmp.linetype = 0; // 电力牵引
        if(ui->taskTunnelTableWidget->item(i,7)->text().compare(tr("是")) == 0)//检测表示为正向的radioButton_5按钮是否被选中
            tmp.doubleline = true;
        else//则表示选中反向这个radioButton_6这个按钮
            tmp.doubleline = false;
        if(ui->taskTunnelTableWidget->item(i,8)->text().compare(tr("是")) == 0)//检测表示为正向的radioButton_5按钮是否被选中
            tmp.downstream = true;
        else//则表示选中反向这个radioButton_6这个按钮
            tmp.downstream = false;

        tmp.pulsepermeter = ui->pulseNumEdit->text().toInt();

        //从Tablewidget获得QComboBox中的值
        QWidget * widget=ui->taskTunnelTableWidget->cellWidget(i,10);//获得widget
        QComboBox *combox=(QComboBox*)widget;//强制转化为QComboBox
        QString string=combox->currentText();
        if (string.compare(tr("正常双线")) == 0 || string.compare(tr("正序单线")) == 0)
            tmp.isnormal = true;
        else
            tmp.isnormal = false;

        QDateTime time= QDateTime::currentDateTime();//获取系统当前时间
        // QString str =time.toString("yyyy-MM-dd");//设置显示格式 //MM为大写才能显示为正确的月份，若在月份中使用mm则会被误认为是分钟
        QString str = time.toString("yyyyMMdd");//在日期之间加上-，导致了溢出。
        tmp.datetime = str.toAscii().data();
        LzProjectAccess::getLzProjectAccessInstance()->getLzPlanList(LzProjectClass::Main).pushBack(tmp);

        // 【多条线路一个文件】 @author 范翔添加
        if (templineid != tmp.linenum)
        {
            if (templineid == -1)
                multiline_name = QString::fromLocal8Bit(tmp.linename.c_str());
            else
                multiline_name = QString("%1_%2").arg(multiline_name).arg(QString::fromLocal8Bit(tmp.linename.c_str()));
            templineid = tmp.linenum;
        }
    }

    QDir dir = QDir(QString(("%1/plan_tasks/")).arg(MasterSetting::getSettingInstance()->getParentPath()));
    if (!dir.exists())
        dir.mkdir(dir.absolutePath());

    QString taskfilename = QString("%1/plan_tasks/%2_%3.plan").arg(MasterSetting::getSettingInstance()->getParentPath()).arg(multiline_name).arg(tmp.datetime.c_str());
    taskfilename = QFileDialog::getSaveFileName(this, QObject::tr("存储到计划隧道文件"), taskfilename, "Document files (*.plan)");
    //    QFileInfo info(filename);
    //    if(!info.exists())//判断计划隧道任务文件名是否存在，既然打开了就一定存在
    //    {
    //        ui->textEdit->setTextColor(Qt::red);
    //        ui->textEdit->setText(tr("计划隧道文件暂未生成"));
    //    }
    //QProcess::execute(tr("notepad.exe 兰青.xml"));
    if (taskfilename.compare("") == 0)
    {
        ui->textEdit->setTextColor(Qt::red);
        ui->textEdit->setText(tr("请选择一个计划隧道文件任务。"));
    }
 
    //QString taskfilename = QString("%1/plan_tasks/%2_%3.plan").arg(MasterSetting::getSettingInstance()->getParentPath()).arg(multiline_name).arg(tmp.datetime.c_str());
    XMLTaskFileLoader * taskfile = new XMLTaskFileLoader(taskfilename);
    bool ret = taskfile->saveFile(LzProjectAccess::getLzProjectAccessInstance()->getLzPlanList(LzProjectClass::Main));
    if (ret)
        ismakefile=true;

    qDebug() << "save file " << taskfilename << ret;
    delete taskfile;

    if (ismakefile)
    {
        ui->textEdit->setText(QObject::tr("状态提示：成功生成任务文件。%1").arg(taskfilename));
    }
    else
    {
        ui->textEdit->setTextColor(Qt::red);//设置textEdit中字体的颜色，而且还要放在文本设置的前面才行
        ui->textEdit->setText(QObject::tr("状态提示：未成功生成任务文件%1，请检查隧道相关信息是否已选择或设置。").arg(taskfilename));
    }
}

void PlanTaskWidget::on_searchButton_clicked()
{
    //只有显示全部线路的时候才能查找
    if(ui->searchLineEdit->text().compare("") == 0)//如果线路名称为空的话
    {
        ui->textEdit->setText(tr("温馨提示：线路名称不能为空。"));
    }
    int row1 = linesModel->rowCount();//获得QListView控件中的行数
    //qDebug()<<row1;
    int k = 0;
    for(k = 0; k < row1; k++)
    {
        QModelIndex index = ui->linesView->model()->index(k,0);
        QString linename = index.data().toString();//这两行代码获取ListView中某行某列的值
        QString query=ui->searchLineEdit->text();
        //qDebug()<<query;
        //qDebug()<<index;
        if(linename.compare(query) == 0)
        {
            ui->linesView->setCurrentIndex(index);
            //ui->linesView->show();
            //ui->linesView->edit(index);//表示为查找的线路可编辑，按常理来说是不可编辑的
            ui->textEdit->setText(tr("温馨提示：线路查找成功"));
            break;
        }
        else
        {
            continue;
        }
    }
    //qDebug()<<k;
    for(int i = 0; i < k; i++)
        ui->linesView->setRowHidden(i,true);
    for(int i = k + 1; i < row1; i++)
        ui->linesView->setRowHidden(i,true);
    if(k == row1)
        ui->textEdit->setText(tr("温馨提示：未查找到此条线路。"));
    //要跳出for循环，才能确定已经查找完所有数据
}

void PlanTaskWidget::on_showAllLinesButton_clicked()
{
    int row1 = linesModel->rowCount();
    for(int i = 0; i < row1; i++)
    {
        ui->linesView->setRowHidden(i, false);
    }
    ui->linesView->show();
}

void PlanTaskWidget::showAllLines()
{

    int row1 = linesModel->rowCount();
    for(int i = 0; i < row1; i++)
    {
        ui->linesView->setRowHidden(i, false);
    }
    ui->linesView->show();

}

/*void PlanTaskWidget::on_assginTaskButton_clicked()
{
    HardwareConfigWidget *one = new HardwareConfigWidget();
    one->show();
}*/
