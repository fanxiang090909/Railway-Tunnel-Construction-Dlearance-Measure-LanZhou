#include "manage_tunnel.h"
#include "ui_manage_tunnel.h"

#include <QtSql/QSqlRelationalTableModel>
#include <QtSql/QSqlRelationalDelegate>
#include <QSqlRecord>
#include <QSqlTableModel>
#include <QDebug>
#include <QComboBox>

#include "daoline.h"
#include "daotunnel.h"
#include "daocurve.h"

/**
 * 隧道管理界面类实现
 * 继承自QWidget
 * @author fanxiang
 * @version 1.0.0
 * @date 2013-08-18
 */
ManageTunnelWidget::ManageTunnelWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ManageTunnelWidget)
{
    ui->setupUi(this);

    mapper = new QDataWidgetMapper(this);

    linesModel = NULL;
    tunnelsModel = NULL;
    curvesModel = NULL;

    loadLinesData();
    loadTunnelData();

    /* 开始选中查找模式为学号模式 */
    ui->radioButton->click();

    /* 开始不能修改 */
    currentRow = 0;
    changed = false;
    saveType = Save_Modify;
    cannotModify();

    /* 线路及隧道数据列表更新信号槽 */
    connect(ui->linesView, SIGNAL(clicked(QModelIndex)), this, SLOT(updateTunnelsView(const QModelIndex &)));
    // 【作废】updateCurvesView已在setCurrentMapper中调用
    //connect(ui->tunnelsView, SIGNAL(clicked(QModelIndex)), this, SLOT(updateCurvesView(const QModelIndex &)));

    /* 添加删除隧道槽 */
    connect(ui->newTunnelButton, SIGNAL(clicked()), this, SLOT(newTunnel()));
    connect(ui->deleteTunnelButton, SIGNAL(clicked()), this, SLOT(deleteTunnel()));
    /* 添加删除隧道对应曲线槽 */
    //connect(ui->curvesView, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(changeCurveData()));
    connect(ui->newCurveButton, SIGNAL(clicked()), this, SLOT(newCurve()));
    connect(ui->deleteCurveButton, SIGNAL(clicked()), this, SLOT(deleteCurve()));

    /* 刷新线路隧道 */
    connect(ui->refreshButton, SIGNAL(clicked()), this, SLOT(refreshLinesView()));

    /* 具体隧道信息信号槽 */
    connect(ui->tunnelsView, SIGNAL(clicked(QModelIndex)), this, SLOT(showTunnelDetail(const QModelIndex &)));
    //connect(ui->stumajorcombobox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(updateStuClasses(const QString &)));

    /* 修改隧道信息信号槽 */
    connect(ui->lineEdit_tunnelid, SIGNAL(textChanged(QString)), this, SLOT(canSave()));
    connect(ui->lineEdit_name, SIGNAL(textChanged(QString)), this, SLOT(canSave()));
    connect(ui->lineEdit_IDstd, SIGNAL(textChanged(QString)), this, SLOT(canSave()));
    connect(ui->lineEdit_startPoint, SIGNAL(textChanged(QString)), this, SLOT(canSave()));
    connect(ui->lineEdit_endPoint, SIGNAL(textChanged(QString)), this, SLOT(canSave()));
    connect(ui->combobox_isNewLine, SIGNAL(currentIndexChanged(int)), this, SLOT(canSave()));
    connect(ui->combobox_lineType, SIGNAL(currentIndexChanged(int)), this, SLOT(canSave()));
    connect(ui->radioButton_isDoubleLine, SIGNAL(clicked()), this, SLOT(canSave()));
    connect(ui->radioButton_isSingleLine, SIGNAL(clicked()), this, SLOT(canSave()));
    connect(ui->radioButton_isUpLink, SIGNAL(clicked()), this, SLOT(canSave()));
    connect(ui->radioButton_isDownLink, SIGNAL(clicked()), this, SLOT(canSave()));
    connect(ui->radioButton_isBridge, SIGNAL(clicked()), this, SLOT(canSave()));
    connect(ui->radioButton_notBridge, SIGNAL(clicked()), this, SLOT(canSave()));

    // 隧道中的曲线界面双击修改后，也可点击保存按钮
    connect(ui->curvesView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(canSave()));

    connect(ui->firstButton, SIGNAL(clicked()), this, SLOT(toFirst()));
    connect(ui->previousButton, SIGNAL(clicked()), this, SLOT(toPrevious()));
    connect(ui->nextButton, SIGNAL(clicked()), this, SLOT(toNext()));
    connect(ui->lastButton, SIGNAL(clicked()), this, SLOT(toLast()));

    connect(ui->modifyButton, SIGNAL(clicked()), this, SLOT(canModify()));

    /* 查找隧道槽 */
    connect(ui->findButton, SIGNAL(clicked()), this, SLOT(findTunnel()));
    connect(ui->findEdit,SIGNAL(textEdited(QString)),this,SLOT(showAllLines()));
    /* 编辑桥梁槽 */
    connect(ui->editBridgeClearanceButton, SIGNAL(clicked()), this, SLOT(editBridgeClearance()));
}

ManageTunnelWidget::~ManageTunnelWidget()
{
    delete mapper;
    delete ui;
}

void ManageTunnelWidget::loadLinesData()
{
    linesModel = LineDAO::getLineDAOInstance()->getLineNames();
    ui->linesView->setModel(linesModel);
    ui->linesView->setAutoScroll(true);
    ui->linesView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // 【标记1】初始设置选中第一行，后边需要根据这个信息得到当前线路名称
    if (linesModel->rowCount() <= 0)
        return;
    QModelIndex index = linesModel->index(0, 0);
    ui->linesView->setCurrentIndex(index);
}

void ManageTunnelWidget::loadTunnelData()
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
    if (tunnelsModel == NULL)
    {
        qDebug() << QObject::tr("加载隧道不成功");
        return;
    }
    ui->tunnelsView->setModel(tunnelsModel);
    //ui->tunnelsView->setItemDelegate(new QSqlRelationalDelegate(this));
    ui->tunnelsView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tunnelsView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tunnelsView->resizeColumnsToContents();
    ui->tunnelsView->horizontalHeader()->setStretchLastSection(true);
    /* 隐藏tunnelsModel模型中的列 */
    ui->tunnelsView->hideColumn(Tunnel_is_new_std);
    ui->tunnelsView->hideColumn(Tunnel_line_type_std);
    ui->tunnelsView->hideColumn(Tunnel_is_valid);
    ui->tunnelsView->hideColumn(Tunnel_is_double_line);
    ui->tunnelsView->hideColumn(Tunnel_is_downlink);
    // 【隐藏ID】@author 范翔20140313
    ui->tunnelsView->hideColumn(Tunnel_ID);
    ui->tunnelsView->hideColumn(Tunnel_is_bridge);

    if (tunnelsModel->rowCount() > 0)
    {
        currentRow = 0;
        ui->tunnelsView->selectRow(0);
    }
    else
        currentRow = -1;

    // 【注意！】要设置mapper的提交模式，不能设置为AutoSubmit
    mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    mapper->setModel(TunnelDAO::getTunnelDAOInstance()->getTunnels(lineid));
    //mapper->setItemDelegate(new QSqlRelationalDelegate(this));
    mapper->addMapping(ui->lineEdit_tunnelid, Tunnel_ID);
    mapper->addMapping(ui->lineEdit_name, Tunnel_name_std);
    mapper->addMapping(ui->lineEdit_IDstd, Tunnel_ID_std);
    mapper->addMapping(ui->lineEdit_startPoint, Tunnel_start_point_std);
    mapper->addMapping(ui->lineEdit_endPoint, Tunnel_end_point_std);

    QModelIndex tunnelindex = ui->tunnelsView->model()->index(currentRow, 0);;
    if (!tunnelindex.isValid())
    {
        qDebug() << QObject::tr("加载隧道不成功");
        return;
    }
    updateCurvesView(tunnelindex);

    // 【作废】当curveView是TableView时用
    /*ui->curvesView->setModel(curvesModel);
    ui->curvesView->setItemDelegate(new QSqlRelationalDelegate(this));
    ui->curvesView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->curvesView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->curvesView->resizeColumnsToContents();
    ui->curvesView->horizontalHeader()->setStretchLastSection(true);

    // 隐藏curvesModel模型中的列
    ui->curvesView->hideColumn(Curve_ID);
    ui->curvesView->hideColumn(Curve_is_straight_std);
    ui->curvesView->hideColumn(Curve_is_valid);
    */

    /****************当curveView是TableWidget时：BEGIN设置curvesView表格******************/
    ui->curvesView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->curvesView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    int columnCount = curvesModel->columnCount();
    int hideColumns = 2;
    QString coltitlestr;
    /* 创建curvesView表格、表头 */
    ui->curvesView->setColumnCount(columnCount - hideColumns);
    // 不把模型中的是否有效位填充
    for (int i = 0, j = 0; j < columnCount - hideColumns; i++)
    {
        // 去掉隧道编号
        if (i == Curve_tunnel_ID || i == Curve_is_valid)
            continue; // j不增，而i增
        coltitlestr = curvesModel->headerData(i, Qt::Horizontal).toString();
        QTableWidgetItem * columntablewidgetitem = new QTableWidgetItem();
        columntablewidgetitem->setText(coltitlestr);
        ui->curvesView->setHorizontalHeaderItem(j, columntablewidgetitem);
        j++;
    }
    ui->curvesView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->curvesView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->curvesView->resizeColumnsToContents();
    /****************END设置curvesView表格******************/

    // 其中含有mapper->toFirst();
    this->toFirst();
}

/**
 * 刷新线路及隧道列表
 */
void ManageTunnelWidget::refreshLinesView()
{
    loadLinesData();
    loadTunnelData();
}

void ManageTunnelWidget::updateTunnelsView(const QModelIndex &index)
{
    currentlineid = -1;
    if (index.isValid())
    {
        QString linename = index.data().toString();
        int lineid = LineDAO::getLineDAOInstance()->getLineID(linename);
        currentlineid = lineid;
        if (lineid == -1)
        {
            return;
        }

        tunnelsModel = TunnelDAO::getTunnelDAOInstance()->getTunnels(lineid);
        if (tunnelsModel->rowCount() <= 0)
        {
            ui->lineEdit_tunnelid->clear();
            ui->lineEdit_IDstd->clear();
            ui->lineEdit_name->clear();
            ui->lineEdit_startPoint->clear();
            ui->lineEdit_endPoint->clear();
            ui->combobox_isNewLine->setCurrentIndex(0);
            ui->combobox_lineType->setCurrentIndex(0);
            ui->radioButton_isDoubleLine->setChecked(false);
            ui->radioButton_isSingleLine->setChecked(false);
            ui->radioButton_isDownLink->setChecked(false);
            ui->radioButton_isUpLink->setChecked(false);
            ui->radioButton_isBridge->setChecked(false);
            ui->radioButton_notBridge->setChecked(false);

            ui->editBridgeClearanceButton->setVisible(false);

            // 如果没有指定tunnel，清楚curveWidget中的数据
            // 先去掉原来的内容
            int tempk = ui->curvesView->rowCount();
            for (int k = 0; k < tempk; k++)
                ui->curvesView->removeRow(0);

            changed = false;
            saveType = Save_Modify;
            cannotModify();
            currentRow = -1;
        }
        else
        {
            currentRow = 0;
            setCurrentMapper(currentRow);
        }
    }
    return;
}

void ManageTunnelWidget::updateCurvesView(const QModelIndex &index)
{
    if (index.isValid())
    {
        QSqlRecord record = tunnelsModel->record(index.row());
        int tunnelid = record.value(0).toInt();
        //qDebug() << QObject::tr("update curvesView") << tunnelid;
        curvesModel = CurveDAO::getCurveDAOInstance()->getCurves(tunnelid);

        if (curvesModel == NULL)
        {
            qDebug() << QObject::tr("加载弯道不成功");
            return;
        }

        /*****************fanxiang*****************/
        // 先去掉原来的内容
        //ui->curvesView->clear();//此句不删除表格行数
        int tempk = ui->curvesView->rowCount();
        for (int k = 0; k < tempk; k++)
            ui->curvesView->removeRow(0);
        ui->curvesView->setRowCount(0);

        /* 填充表格内容 */
        QString itemstr;

        int columnCount = curvesModel->columnCount();
        int hideColumns = 2;
        for (int k = 0; k < curvesModel->rowCount(); k++)
        {
            int row = ui->curvesView->rowCount();
            //qDebug() << "cureves view row=" << row;
            ui->curvesView->setRowCount(row+1);

            // 表项
            bool static isStraight = true;
            bool static isLeft = true;
            for (int i = 0, j = 0; j < columnCount - hideColumns; i++)
            {
                record = curvesModel->record(k);
                itemstr = record.value(i).toString();
                QTableWidgetItem * tablewidgetitem = new QTableWidgetItem();

                // 去掉不显示数据（曲线编号，对应隧道和是否有效）
                if (i == Curve_tunnel_ID || i == Curve_is_valid)
                    continue;
                if (i == Curve_is_straight_std)
                    if (itemstr.compare("0") == 0)
                        isStraight = false;
                    else
                        isStraight = true;
                if (i == Curve_is_left_std)
                    if (itemstr.compare("0") == 0)
                        isLeft = false;
                    else
                        isLeft = true;
                if (i == Curve_is_straight_std)
                {
                    QComboBox *combo = new QComboBox();
                    combo->addItem(QObject::tr("是"));
                    combo->addItem(QObject::tr("否"));
                    if (itemstr.compare("0") == 0)
                        combo->setCurrentIndex(1); // 否
                    else if (itemstr.compare("1") == 0)
                        combo->setCurrentIndex(0); // 是
                    ui->curvesView->setCellWidget(k, j, combo);
                    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(changeCurveData()));
                }
                if (i == Curve_is_left_std)
                {
                    QComboBox *combo = new QComboBox();
                    combo->addItem(QObject::tr("左转"));
                    combo->addItem(QObject::tr("右转"));
                    if (itemstr.compare("0") == 0)
                        combo->setCurrentIndex(1); // 否
                    else if (itemstr.compare("1") == 0)
                        combo->setCurrentIndex(0); // 是
                    ui->curvesView->setCellWidget(k, j, combo);
                    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(changeCurveData()));
                }
                tablewidgetitem->setText(itemstr);
                ui->curvesView->setItem(k, j, tablewidgetitem);
                j++; // 非常关键
            }
            //qDebug() << k << itemstr;
        }
        // 开始不能编辑，是为行选择
        ui->curvesView->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->curvesView->setEnabled(false);
    }
    return;
}

void ManageTunnelWidget::on_saveButton_clicked()
{
    /* 如果隧道名称、隧道编号、起始里程、终止里程为空 */
    if (ui->lineEdit_name->text().length() == 0 ||
            ui->lineEdit_IDstd->text().length() == 0  ||
            ui->lineEdit_startPoint->text().length() == 0 ||
            ui->lineEdit_endPoint->text().length() == 0)
    {
        int result = QMessageBox::warning(this,tr("警告"), tr("隧道名称、隧道编号、起始里程、终止里程不能为空。继续则不保存，是否继续？"),
                                                  QMessageBox::Yes | QMessageBox::No);
        if (result == QMessageBox::No)
            return;
        if (result == QMessageBox::Yes)
        {
            // 不保存，直接重新加载详细框即可
            showTunnelDetail(ui->tunnelsView->currentIndex());
            changed = false;
            /* 重新设置为不可编辑 */
            cannotModify();
            saveType = Save_Modify;
            return;
        }
    }

    // 获得线路id
    int lineid;
    // 【标记1】得到当前线路名称
    QModelIndex lineindex = ui->linesView->currentIndex();
    if (!lineindex.isValid())
        ui->linesView->setCurrentIndex(lineindex);
    else
    {
        if (ui->linesView->model() == NULL || ui->linesView->model()->data(lineindex) == NULL)
        {
            qDebug() << "invalid";
            return;
        }
        QString linename = ui->linesView->model()->data(lineindex).toString();
        lineid = LineDAO::getLineDAOInstance()->getLineID(linename);
        qDebug() << linename << lineid;
    }
    // 获得输入隧道信息数据
    int isNewLine = -1;
    int lineType = -1;
    bool isDoubleLine;
    bool isDownLink;
    bool isBridge;

    int temp = ui->combobox_isNewLine->currentIndex();
    if (temp == 1) // 新线
        isNewLine = 1;
    else if (temp == 2) // 旧线
        isNewLine = 0;
    temp = ui->combobox_lineType->currentIndex();
    if (temp == 1)
        lineType = 1; // 内燃牵引
    else if (temp == 2)
        lineType = 0; // 电力牵引
    isDoubleLine = ui->radioButton_isDoubleLine->isChecked();
    isDownLink = ui->radioButton_isDownLink->isChecked();
    isBridge = ui->radioButton_isBridge->isChecked();

    // 更新曲线
    saveCurvesData();

    if (saveType == Save_Add)
    {
        TunnelDAO::getTunnelDAOInstance()->addTunnel(ui->lineEdit_name->text(), true,
                                                        lineid, ui->lineEdit_IDstd->text(),
                                                        ui->lineEdit_startPoint->text().toLongLong(),
                                                        ui->lineEdit_endPoint->text().toLongLong(),
                                                        isNewLine, lineType, isDoubleLine, isDownLink, isBridge);
    }
    else if (saveType == Save_Modify)
    {
        TunnelDAO::getTunnelDAOInstance()->updateTunnel(ui->lineEdit_tunnelid->text().toInt(),
                                                        ui->lineEdit_name->text(), true,
                                                        lineid, ui->lineEdit_IDstd->text(),
                                                        ui->lineEdit_startPoint->text().toLongLong(),
                                                        ui->lineEdit_endPoint->text().toLongLong(),
                                                        isNewLine, lineType, isDoubleLine, isDownLink);
    }

    /* 若保存或更新，更新左视图 */
    // @author 范翔注释，每次不及时修改左视图
    QModelIndex tunnelindex = ui->tunnelsView->currentIndex();
    updateTunnelsView(ui->linesView->currentIndex());
    if (tunnelindex.isValid())
    {
        ui->tunnelsView->setCurrentIndex(tunnelindex);
        updateCurvesView(tunnelindex);
    }

    changed = false;
    saveType = Save_Modify;
    cannotModify();
}

void ManageTunnelWidget::showTunnelDetail(const QModelIndex& index)
{
    if (!index.isValid())
        return;

    // 改变当前行号
    currentRow = index.row();

    /* 判断是否已对某些信息修改 */
    if (changed)
    {
        int result = QMessageBox::warning(this,tr("警告"), tr("确认放弃修改？放弃后修改的内容将丢失！！！点击“No”取消。"),
                                          QMessageBox::Yes | QMessageBox::No);
        if (result == QMessageBox::No)
            return;
        else if (result == QMessageBox::Yes)
        {
            changed = false;
            cannotModify();
            // 只重显示detail框，之前将changed置为false
            showTunnelDetail(index);
            /* 重新设置为不可编辑 */
            saveType = Save_Modify;
        }
    }

    // 该方法调用mapper->setCurrentIndex(currentRow);
    setCurrentMapper(currentRow);
}

void ManageTunnelWidget::newTunnel()
{
    if (changed == true)
    {
        QMessageBox::warning(this,tr("提示"), tr("请先保存再添加隧道！"));
        return;
    }

    qDebug() << tr("添加");
    ui->lineEdit_tunnelid->clear();
    ui->lineEdit_IDstd->clear();
    ui->lineEdit_name->clear();
    ui->lineEdit_startPoint->clear();
    ui->lineEdit_endPoint->clear();

    // 从线路属性中得出
    int defaultIsNew = LineDAO::getLineDAOInstance()->getIsNewLineStd(currentlineid);
    if (defaultIsNew == 0)
        defaultIsNew = 2;
    else if (defaultIsNew == 1)
        defaultIsNew = 1;
    else
        defaultIsNew = 0;

    int defaultLineType = LineDAO::getLineDAOInstance()->getLineTypeStd(currentlineid);
    if (defaultLineType == 0)
        defaultLineType = 2;
    else if (defaultLineType == 1)
        defaultLineType = 1;
    else
        defaultLineType = 0;

    ui->combobox_isNewLine->setCurrentIndex(defaultIsNew);
    ui->combobox_lineType->setCurrentIndex(defaultLineType);
    ui->radioButton_isDoubleLine->setChecked(false);
    ui->radioButton_isSingleLine->setChecked(false);
    ui->radioButton_isDownLink->setChecked(false);
    ui->radioButton_isUpLink->setChecked(false);
    ui->saveButton->setText(tr("添加"));
    canModify();
    ui->groupBox_isBridge->setEnabled(true);

    changed = false;
    saveType = Save_Add;
}

void ManageTunnelWidget::deleteTunnel()
{
    if (changed == true)
    {
        QMessageBox::warning(this,tr("提示"), tr("请先保存再删除隧道！"));
        return;
    }

    QModelIndex index = ui->tunnelsView->currentIndex();

    if (currentRow < 0)
    {
        QMessageBox::warning(this,tr("提示"), tr("请先选择一行！"));
        return;
    }
    int r = QMessageBox::warning(this,tr("提示"), tr("您将删除该隧道，确定删除？"),
                                 QMessageBox::Yes | QMessageBox::No);
    if (r == QMessageBox::No)
    {
        return;
    }

    /*********************调用TunnelDAO的删除操作remove**********************/
    // 调用TunnelDAO的removeTunnel方法删除
    int row = index.row();
    index = ui->tunnelsView->model()->index(row, Tunnel_ID);
    int tunnelid = ui->tunnelsView->model()->data(index).toInt();
    // qDebug() << tunnelid;
    TunnelDAO::getTunnelDAOInstance()->removeTunnel(tunnelid);

    // 删除时更新左视图
    updateTunnelsView(ui->linesView->currentIndex());

    // qDebug() << tr("删除");
    this->toPrevious();
}

// 更新曲线
void ManageTunnelWidget::saveCurvesData()
{
    static _int64 curveid = -1;
    static bool curveUpdate = true;
    static bool isLeft = true;
    static bool isStraight = true;
    //int columnCount = ui->curvesView->columnCount();
    //int hideColumns = 2;
    for (int k = 0; k < ui->curvesView->rowCount(); k++)
    {
        // 【注意！】很重要，否则空指针异常
        if (ui->curvesView->item(k, 1) == NULL ||
                    ui->curvesView->item(k, 4) == NULL ||
                    ui->curvesView->item(k, 5) == NULL)
        {
            QMessageBox::warning(this,tr("警告"), tr("曲线半径，直缓点里程、缓直点里程不能为空"));
            return;
        }
        curveid = -1;
        curveUpdate = true;
        isLeft = true;
        isStraight = true;
        // 去掉不显示数据（曲线编号，对应隧道和是否有效）
        if (ui->curvesView->item(k, 0) == NULL)
            curveUpdate = false;
        else
            curveid = ui->curvesView->item(k, 0)->text().toLongLong();
        //qDebug() << k << " " << 0 << " " << curveid;
        QComboBox *combo = (QComboBox*)ui->curvesView->cellWidget(k, 2);
		if (combo->currentText().compare(QString(tr("否"))) == 0) // ?会不会有问题？
        {
            qDebug() << combo->currentText();
            isStraight = false;
        }
        combo = (QComboBox*)ui->curvesView->cellWidget(k, 3);
        if (combo->currentText().compare(QString(tr("右转"))) == 0)
            isLeft = false;

        // 更新curve
        if (curveUpdate)
            CurveDAO::getCurveDAOInstance()->updateCurve(curveid, ui->lineEdit_tunnelid->text().toInt(),
                                                            ui->curvesView->item(k, 1)->text().toInt(),
                                                            ui->curvesView->item(k, 4)->text().toLongLong(),
                                                            ui->curvesView->item(k, 5)->text().toLongLong(),
                                                            isStraight, isLeft, true);
        else
            CurveDAO::getCurveDAOInstance()->addCurve(ui->lineEdit_tunnelid->text().toInt(),
                                                            ui->curvesView->item(k, 1)->text().toInt(),
                                                            ui->curvesView->item(k, 4)->text().toLongLong(),
                                                            ui->curvesView->item(k, 5)->text().toLongLong(),
                                                            isStraight, isLeft, true);
    }
    //qDebug() << tr("更新曲线");
}

void ManageTunnelWidget::newCurve()
{
    if (saveType == Save_Add_PropertyClass)
    {
        saveCurvesData();
        QModelIndex tunnelindex = ui->tunnelsView->currentIndex();
        if (tunnelindex.isValid() == false)
        {
            QMessageBox::warning(this,tr("提示"), tr("请先选择一条隧道！"));
            return;
        }
        updateCurvesView(tunnelindex);

        canModify();
    }

    int curvesRow = ui->curvesView->rowCount();
    ui->curvesView->insertRow(curvesRow);
    //qDebug() << tr("添加曲线");
    int columnCount = curvesModel->columnCount();
    int hideColumns = 2;

    for (int i = 0, j = 0; j < columnCount - hideColumns; i++)
    {
        // 去掉不显示数据（曲线编号，对应隧道和是否有效）
        if (i == Curve_ID)
        {
            QLabel *label = new QLabel("");
            ui->curvesView->setCellWidget(curvesRow, j, label);
            //ui->curvesView->cellWidget(curvesRow, j)->setEnabled(false);
        }
        if (i == Curve_tunnel_ID || i == Curve_is_valid)
            continue;
        else if (i == Curve_is_straight_std)
        {
            QComboBox *combo = new QComboBox();
            combo->addItem(QObject::tr("是"));
            combo->addItem(QObject::tr("否"));
            combo->setCurrentIndex(1);
            // combobox的信号槽
            connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(canSave()));
            ui->curvesView->setCellWidget(curvesRow, j, combo);
        }
        else if (i == Curve_is_left_std)
        {
            QComboBox *combo = new QComboBox();
            combo->addItem(QObject::tr("左转"));
            combo->addItem(QObject::tr("右转"));
            // combobox的信号槽
            connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(canSave()));
            ui->curvesView->setCellWidget(curvesRow, j, combo);
        }
        j++;
    }
    canSave();
    saveType = Save_Add_PropertyClass;
}

void ManageTunnelWidget::deleteCurve()
{
    if (changed == true)
    {
        QMessageBox::warning(this,tr("提示"), tr("请先保存再删除曲线！"));
        return;
    }

    int row = ui->curvesView->currentIndex().row();
    if (row < 0)
    {
        QMessageBox::warning(this,tr("提示"), tr("请先选择一行！"));
        return;
    }

    /*************************fanxiang*****************************/
    // 调用CurveDAO的removeCurve方法删除
    _int64 curveid = -1;
    // 去掉不显示数据（曲线编号，对应隧道和是否有效）
    if (ui->curvesView->item(row, 0) == NULL)
    {
        QMessageBox::warning(this,tr("提示"), tr("该曲线段尚未保存，不能删除!"));
        return;
    }
    else
    {
        int r = QMessageBox::warning(this,tr("提示"), tr("您将删除该曲线段，确定删除？"),
                                     QMessageBox::Yes | QMessageBox::No);
        if (r == QMessageBox::No)
        {
            return;
        }
        curveid = ui->curvesView->item(row, 0)->text().toLongLong();
        CurveDAO::getCurveDAOInstance()->removeCurve(curveid);
    }

    // 删除时更新曲线列表curvesView视图
    QModelIndex tunnelindex = ui->tunnelsView->currentIndex();
    if (!tunnelindex.isValid())
    {
        QModelIndex index = ui->tunnelsView->model()->index(0, 0);
        ui->tunnelsView->setCurrentIndex(index);
    }
    updateCurvesView(tunnelindex);
    changed = false;
    /* 重新设置为不可编辑 */
    cannotModify();
    saveType = Save_Modify;
    //qDebug() << tr("删除曲线");
}

/**
 * 监听曲线表格更改信号
 */
void ManageTunnelWidget::changeCurveData()
{
    saveType = Save_Add_PropertyClass;
    canSave();
}

void ManageTunnelWidget::canModify()
{
    ui->lineEdit_tunnelid->setEnabled(false);// 在数据库中ID自增策略，无需手动输入
    ui->lineEdit_name->setEnabled(true);
    ui->lineEdit_IDstd->setEnabled(true);
    ui->lineEdit_startPoint->setEnabled(true);
    ui->lineEdit_endPoint->setEnabled(true);
    ui->combobox_isNewLine->setEnabled(true);
    ui->combobox_lineType->setEnabled(true);
    ui->groupBox_doubleLine->setEnabled(true);
    ui->groupBox_downLink->setEnabled(true);
    ui->groupBox_isBridge->setEnabled(false);

    ui->newCurveButton->setEnabled(true);
    ui->deleteCurveButton->setEnabled(true);
    ui->modifyButton->setEnabled(false);

    // 双击即可编辑弯道
    ui->curvesView->setEditTriggers(QAbstractItemView::DoubleClicked);
    // 可以编辑，是表格元素选择
    ui->curvesView->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui->curvesView->setEnabled(true);
    for (int i = 0; i < ui->curvesView->rowCount(); i++)
        ui->curvesView->item(i, 0)->setFlags(Qt::NoItemFlags);
}

void ManageTunnelWidget::cannotModify()
{
    ui->saveButton->setText(tr("保存"));

    ui->lineEdit_tunnelid->setEnabled(false);
    ui->lineEdit_name->setEnabled(false);
    ui->lineEdit_IDstd->setEnabled(false);
    ui->lineEdit_startPoint->setEnabled(false);
    ui->lineEdit_endPoint->setEnabled(false);
    ui->combobox_isNewLine->setEnabled(false);
    ui->combobox_lineType->setEnabled(false);
    ui->groupBox_doubleLine->setEnabled(false);
    ui->groupBox_downLink->setEnabled(false);
    ui->groupBox_isBridge->setEnabled(false);

    /* 也不能保存 */
    ui->saveButton->setEnabled(false);
    ui->modifyButton->setEnabled(true);
    ui->newCurveButton->setEnabled(false);
    ui->deleteCurveButton->setEnabled(false);

    // 开始不能编辑，是为行选择
    ui->curvesView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->curvesView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->curvesView->setEnabled(false);
}

void ManageTunnelWidget::canSave()
{
    if (ui->modifyButton->isEnabled() == false)
    {
        changed = true;
        ui->saveButton->setEnabled(true);
    }
}

/* 用于mapper的四个按钮，调用mapper的toFirst、toNext、toLast、ToPrevious方法 */
void ManageTunnelWidget::toFirst()
{
    //mapper->toFirst();
    if (currentRow >= 0)
        currentRow = 0;
    setCurrentMapper(currentRow);
}

void ManageTunnelWidget::toNext()
{
    //mapper->toNext();
    if (currentRow != tunnelsModel->rowCount() - 1)
        currentRow++;
    setCurrentMapper(currentRow);
}

void ManageTunnelWidget::toLast()
{
    //mapper->toLast();
    currentRow = tunnelsModel->rowCount() - 1;
    setCurrentMapper(currentRow);
}

void ManageTunnelWidget::toPrevious()
{
    //mapper->toPrevious();
    if (currentRow >= 0)
        currentRow--;
    setCurrentMapper(currentRow);
}

/* 设置隧道详细框当前匹配的内容显示出来，调用mapper的setCurrentIndex */
void ManageTunnelWidget::setCurrentMapper(int currentRow)
{
    // 没有任何
    if (currentRow < 0)
        return;

    mapper->setCurrentIndex(currentRow);
    ui->tunnelsView->selectRow(currentRow);

    // 相应隧道信息
    int isNewLine = tunnelsModel->index(currentRow, Tunnel_is_new_std).data().toInt();
    int lineType = tunnelsModel->index(currentRow, Tunnel_line_type_std).data().toInt();
    bool isDoubleLine = tunnelsModel->index(currentRow, Tunnel_is_double_line).data().toBool();
    bool isDownLink = tunnelsModel->index(currentRow, Tunnel_is_downlink).data().toBool();
    bool isBridge = tunnelsModel->index(currentRow, Tunnel_is_bridge).data().toBool();
    //qDebug() << isNewLine << lineType << isDoubleLine << isDownLink;

    if (isNewLine == 1) // 新线
        ui->combobox_isNewLine->setCurrentIndex(1);
    else if (isNewLine == 0) // 旧线
        ui->combobox_isNewLine->setCurrentIndex(2);
    else
        ui->combobox_isNewLine->setCurrentIndex(0);

    if (lineType == 1) // 内燃牵引
        ui->combobox_lineType->setCurrentIndex(1);
    else if (lineType == 0) // 电力牵引
        ui->combobox_lineType->setCurrentIndex(2);
    else
        ui->combobox_lineType->setCurrentIndex(0);

    ui->radioButton_isDoubleLine->setChecked(isDoubleLine);
    ui->radioButton_isSingleLine->setChecked(!isDoubleLine);
    ui->radioButton_isDownLink->setChecked(isDownLink);
    ui->radioButton_isUpLink->setChecked(!isDownLink);
    ui->radioButton_isBridge->setChecked(isBridge);
    ui->radioButton_notBridge->setChecked(!isBridge);

    if (isBridge)
        ui->editBridgeClearanceButton->setVisible(true);
    else
        ui->editBridgeClearanceButton->setVisible(false);

    /* 加载隧道对应的多条曲线段 */
    QModelIndex tunnelindex = ui->tunnelsView->currentIndex();
    if (!tunnelindex.isValid())
    {
        QModelIndex index = ui->tunnelsView->model()->index(0, 0);
        ui->tunnelsView->setCurrentIndex(index);
    }
    updateCurvesView(tunnelindex);

    changed = false;
    /* 重新设置为不可编辑 */
    cannotModify();
    ui->modifyButton->setEnabled(true);
}

void ManageTunnelWidget::findTunnel()
{
    if(ui->radioButton_2->isChecked())
    {
        // 只有显示全部线路的时候才能查找
        if(ui->findEdit->text().compare("") == 0)//如果线路名称为空的话
        {
            QMessageBox::warning(NULL,tr("提示"),tr("温馨提示：线路名称不能为空。"));
            return;
        }
        int row1 = linesModel->rowCount();//获得QListView控件中的行数
        int k = 0;
        for(k = 0; k < row1; k++)
        {
            QModelIndex index = ui->linesView->model()->index(k,0);
            QString linename = index.data().toString();//这两行代码获取ListView中某行某列的值
            QString query=ui->findEdit->text();
            //qDebug()<<query;
            //qDebug()<<index;
            if(linename.compare(query) == 0)
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
        for(int i = 0; i < k; i++)
            ui->linesView->setRowHidden(i,true);
        for(int i = k + 1; i < row1; i++)
            ui->linesView->setRowHidden(i,true);
        if(k == row1)
            QMessageBox::warning(NULL,tr("提示"),tr("温馨提示：未查找到此条线路。"));
        //要跳出for循环，才能确定已经查找完所有数据
    }
    if (ui->radioButton->isChecked())
    {
        if (ui->findEdit->text().compare("") == 0)//如果线路名称为空的话
        {
            QMessageBox::warning(NULL,tr("提示"),tr("温馨提示：隧道名称不能为空。"));
            return;
        }
        int row1 = tunnelsModel->rowCount();//获得QListView控件中的行数
        qDebug()<<row1;
        int k = 0;
        for(k = 0; k < row1; k++)
        {
           QModelIndex index = ui->tunnelsView->model()->index(k,1);
           QString tunnelname = index.data().toString();//这两行代码获取TableView中某行某列的值
           qDebug()<<QString(tr("%1")).arg(tunnelname);
           QString query=ui->findEdit->text();
           qDebug()<<QString(tr("%1")).arg(query);
           if(tunnelname.compare(query) == 0)
           {
               ui->tunnelsView->setCurrentIndex(index);
               break;
           }
           else
           {
               continue;
           }
       }
        qDebug()<<"k:"<<k;
       for(int i = 0; i < k; i++)
           ui->tunnelsView->setRowHidden(i,true);
       for(int i = k + 1; i < row1; i++)
           ui->tunnelsView->setRowHidden(i,true);
       if(k == row1)
           QMessageBox::warning(NULL,tr("提示"),tr("温馨提示：未查找到此条隧道。"));
       //要跳出for循环，才能确定已经查找完所有数据
    }
}

void ManageTunnelWidget::showAllLines()
{
    // 显示所有线路
    int row1 = linesModel->rowCount();
    for(int i = 0; i < row1; i++)
    {
        ui->linesView->setRowHidden(i, false);
    }
    ui->linesView->show();
    // 显示所有隧道
    int row2 = tunnelsModel->rowCount();
    for(int i = 0; i < row2; i++)
    {
        ui->tunnelsView->setRowHidden(i, false);
    }
    ui->tunnelsView->show();
}

void ManageTunnelWidget::findLine()
{
    QString finddes = ui->findEdit->text();
    int findindex = 0;
    if (ui->radioButton_2->isChecked())
    {
        findindex = 1;
    }
    int iffind = 0;
    if (findindex == 0) {
        QString tunnelid = finddes;
        qDebug() << tunnelid;

        if (iffind == 0)
        {
            QMessageBox::warning(this,tr("提示"),
                           tr("该线路中找不到此隧道: 隧道序号") + tunnelid, tr("确定"));
            ui->findEdit->clear();
        }
    }
    else if (findindex == 1)
    {
        QString tunnelname = finddes;
        for (int row = 0; row < tunnelsModel->rowCount(); ++row) {
            if (true) {
                iffind = 1;
            }
        }
        if (iffind == 0)
        {
            QMessageBox::warning(this,tr("提示"),
                           tr("该线路中找不到此隧道: 隧道名称") + tunnelname, tr("确定"));
            ui->findEdit->clear();
        }
    }

    ui->tunnelsView->horizontalHeader()->setVisible(tunnelsModel->rowCount() > 0);
}

/* 编辑桥梁限界槽 */
void ManageTunnelWidget::editBridgeClearance()
{
    int tunnelid = ui->lineEdit_tunnelid->text().toInt();
    QString tunnelname = ui->lineEdit_name->text();
    emit signalBridgeToEdit(tunnelid, tunnelname);
}
