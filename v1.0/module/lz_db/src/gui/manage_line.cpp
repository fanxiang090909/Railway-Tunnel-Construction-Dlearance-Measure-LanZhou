#include "manage_line.h"
#include "ui_manage_line.h"

#include <QtSql/QSqlRelationalTableModel>
#include <QtSql/QSqlRelationalDelegate>
#include <QDebug>
#include <QMessageBox>

/**
 * 线路管理界面类实现
 * 继承自QWidget
 * @author fanxiang
 * @version 1.0.0
 * @date 2013-08-18
 */
ManageLineWidget::ManageLineWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ManageLineWidget)
{
    ui->setupUi(this);

    linesModel = NULL;
    linesModel = LineDAO::getLineDAOInstance()->getLines();

    ui->linesView->setModel(linesModel);
    //ui->linesView->setItemDelegate(new QSqlRelationalDelegate(this));
    ui->linesView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->linesView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->linesView->resizeColumnsToContents();
    ui->linesView->horizontalHeader()->setStretchLastSection(true);
    ui->linesView->hideColumn(Line_is_valid);
    // 【隐藏ID】@author 范翔20140313
    ui->linesView->hideColumn(Line_ID);
    ui->linesView->hideColumn(Line_Tunnel_is_new_std);
    ui->linesView->hideColumn(Line_Tunnel_line_type_std);

    mapper = new QDataWidgetMapper(this);

    // 【注意！】要设置mapper的提交模式，不能设置为AutoSubmit
    mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    mapper->setModel(linesModel);

    mapper->addMapping(ui->lineEdit_lineiD, Line_ID);
    mapper->addMapping(ui->lineEdit_lineName, Line_name_std);
    mapper->addMapping(ui->lineEdit_startStation, Line_start_station_std);
    mapper->addMapping(ui->lineEdit_endStation, Line_end_station_std);

    if (linesModel->rowCount() > 0)
    {
        currentRow = 0;
        setCurrentMapper(0);
    }
    else
        currentRow = -1;

    /* 初始不能修改具体信息 */
    saveType = Save_Modify;
    cannotModify();
    changed = false;

    /* 添加删除修改线路按钮槽 */
    connect(ui->newLine, SIGNAL(clicked()), this, SLOT(newLine()));
    connect(ui->deleteLine, SIGNAL(clicked()), this, SLOT(deleteLine()));

    /* 刷新线路隧道 */
    connect(ui->refreshButton, SIGNAL(clicked()), this, SLOT(updateLinesView()));

    connect(ui->modifyLine, SIGNAL(clicked()), this, SLOT(canModify()));

    /* 具体线路显示widget信号槽 */
    connect(ui->linesView, SIGNAL(clicked(QModelIndex)), this, SLOT(showLineDetail(const QModelIndex &)));
    connect(ui->lineEdit_lineiD, SIGNAL(textChanged(QString)), this, SLOT(canSave()));
    connect(ui->lineEdit_lineName, SIGNAL(textChanged(QString)), this, SLOT(canSave()));
    connect(ui->lineEdit_startStation, SIGNAL(textChanged(QString)), this, SLOT(canSave()));
    connect(ui->lineEdit_endStation, SIGNAL(textChanged(QString)), this, SLOT(canSave()));
    connect(ui->combobox_isNewLine, SIGNAL(currentIndexChanged(int)), this, SLOT(canSave()));
    connect(ui->combobox_lineType, SIGNAL(currentIndexChanged(int)), this, SLOT(canSave()));

    /* 查找线路槽 */
    connect(ui->findButton, SIGNAL(clicked()), this, SLOT(findLine()));
    connect(ui->findEdit,SIGNAL(textEdited(QString)),this,SLOT(showAllLines()));
}

ManageLineWidget::~ManageLineWidget()
{
    delete mapper;
    delete ui;
}

void ManageLineWidget::updateLinesView()
{
    linesModel = LineDAO::getLineDAOInstance()->getLines();

    if (linesModel->rowCount() > 0)
        currentRow = 0;
    
    // 该方法调用mapper->setCurrentIndex(currentRow);
    setCurrentMapper(currentRow);

    changed = false;
}

/* 设置隧道详细框当前匹配的内容显示出来，调用mapper的setCurrentIndex */
void ManageLineWidget::setCurrentMapper(int currentRow)
{
    // 没有任何
    if (currentRow < 0)
        return;

    mapper->setCurrentIndex(currentRow);

    // 相应隧道信息
    int isNewLine = linesModel->index(currentRow, Line_Tunnel_is_new_std).data().toInt();
    int lineType = linesModel->index(currentRow, Line_Tunnel_line_type_std).data().toInt();

    //qDebug() << isNewLine << lineType;

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

    /* 重新设置为不可编辑 */
    changed = false;
    cannotModify();
}

void ManageLineWidget::showLineDetail(const QModelIndex& index)
{
    // 改变当前行号
    currentRow = index.row();

    /* 判断是否已对某些信息修改 */
    if (changed)
    {
        int result = QMessageBox::warning(this,tr("警告"), tr("请点击保存，否则修改的内容将丢失！！！"),
                                          QMessageBox::Yes | QMessageBox::No);
        if (result == QMessageBox::No)
            return;
        else if (result == QMessageBox::Yes)
        {
            /* 删除显示的当前空行 */
            if (ui->lineEdit_lineName->text().length() == 0 ||
                        ui->lineEdit_startStation->text().length() == 0  ||
                        ui->lineEdit_endStation->text().length() == 0 )
            {
                changed = false;
                cannotModify();
                // 只重显示detail框，之前将changed置为false
                showLineDetail(index);
                saveType = Save_Modify;
            }
        }
    }
    
    setCurrentMapper(currentRow);


}

void ManageLineWidget::newLine()
{
    ui->lineEdit_lineiD->clear();
    ui->lineEdit_lineName->clear();
    ui->lineEdit_startStation->clear();
    ui->lineEdit_endStation->clear();
    ui->combobox_isNewLine->setCurrentIndex(0);
    ui->combobox_lineType->setCurrentIndex(0);

    ui->lineEdit_lineName->setFocus();
    ui->saveLine->setText(tr("添加"));
    saveType = Save_Add;
    canModify();
    changed = false;
}

void ManageLineWidget::deleteLine()
{
    QModelIndex index = ui->linesView->currentIndex();
    if (!index.isValid())
    {
        QMessageBox::warning(this,tr("提示"), tr("请先选择一行！"));
        return;
    }
    int r = QMessageBox::warning(this,tr("提示"), tr("您将删除该线路，确定删除？"),
                                 QMessageBox::Yes | QMessageBox::No);
    if (r == QMessageBox::No)
    {
        return;
    }
    // qDebug() << tr("删除");

    /********************************************************/
    // 调用LineDAO的removeLine方法删除
    int row = index.row();
    index = ui->linesView->model()->index(row, Line_ID);
    int lineid = ui->linesView->model()->data(index).toInt();

    qDebug () << lineid;
    LineDAO::getLineDAOInstance()->removeLine(lineid);

    /* 删除操作，更新左视图 */
    updateLinesView();

    if (row != 0) {
        setCurrentMapper(row - 1);
    } else {
        setCurrentMapper(0);
    }
    setCurrentMapper(0);
}

/**
 * 点击saveLine按钮后的槽函数
 * 若修改调用mapper自带的数据库update方法
 * LineDAO中定义的提交方法，save
 */
void ManageLineWidget::on_saveLine_clicked()
{
    /* 删除显示的当前空行 */
    if (ui->lineEdit_lineName->text().length() == 0 ||
                ui->lineEdit_startStation->text().length() == 0  ||
                ui->lineEdit_endStation->text().length() == 0 )
    {
        int result = QMessageBox::warning(this,tr("警告"),
                                          tr("线路名，起始站名，结束站名不能为空。继续则不保存，是否继续？"),
                                                  QMessageBox::Yes | QMessageBox::No);
        if (result == QMessageBox::No)
            return;
        if (result == QMessageBox::Yes)
        {
            // 不保存，直接重新加载详细框即可
            showLineDetail(ui->linesView->currentIndex());
            changed = false;
            /* 重新设置为不可编辑 */
            cannotModify();
            saveType = Save_Modify;
            return;
        }
    }

    // 获得输入隧道信息数据
    int isNewLine = -1;
    int lineType = -1;

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

    if (saveType == Save_Modify)
        LineDAO::getLineDAOInstance()->updateLine(ui->lineEdit_lineiD->text().toInt(),
                                           ui->lineEdit_lineName->text(), true,
                                           ui->lineEdit_startStation->text(), ui->lineEdit_endStation->text(), isNewLine, lineType);
    else
        LineDAO::getLineDAOInstance()->addLine(ui->lineEdit_lineName->text(), true,
                                           ui->lineEdit_startStation->text(), ui->lineEdit_endStation->text(), isNewLine, lineType);

    /* 若保存或更新，更新左视图 */
    updateLinesView();

    QMessageBox::warning(this,tr("提示"), tr("保存成功！"));
    saveType = Save_Modify;
    cannotModify();
    changed = false;
}

void ManageLineWidget::canModify()
{
    ui->lineEdit_lineiD->setEnabled(false); // 在数据库中ID自增策略，无需手动输入
    ui->lineEdit_lineName->setEnabled(true);
    ui->lineEdit_startStation->setEnabled(true);
    ui->lineEdit_endStation->setEnabled(true);
    ui->combobox_isNewLine->setEnabled(true);
    ui->combobox_lineType->setEnabled(true);

    ui->modifyLine->setEnabled(false);
}

void ManageLineWidget::cannotModify()
{
    ui->saveLine->setText(tr("保存"));

    ui->lineEdit_lineiD->setEnabled(false);
    ui->lineEdit_lineName->setEnabled(false);
    ui->lineEdit_startStation->setEnabled(false);
    ui->lineEdit_endStation->setEnabled(false);
    ui->combobox_isNewLine->setEnabled(false);
    ui->combobox_lineType->setEnabled(false);

    /* 不能保存 */
    ui->saveLine->setEnabled(false);
    ui->modifyLine->setEnabled(true);
}

void ManageLineWidget::canSave()
{
    changed = true;
    ui->saveLine->setEnabled(true);
}

void ManageLineWidget::findLine()
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
        QModelIndex index = ui->linesView->model()->index(k, Line_name_std);
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

void ManageLineWidget::showAllLines()
{
    // 显示所有线路
    int row1 = linesModel->rowCount();
    for(int i = 0; i < row1; i++)
    {
        ui->linesView->setRowHidden(i, false);
    }
    ui->linesView->show();
}