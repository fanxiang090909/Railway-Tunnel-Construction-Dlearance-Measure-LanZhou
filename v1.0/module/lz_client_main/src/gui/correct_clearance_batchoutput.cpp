#include "correct_clearance_batchoutput.h"
#include "ui_correct_clearance_batchoutput.h"
#include <QMessageBox>

/**
 * 图形修正界面——EXCEL批量输出
 * @author 范翔
 * @version 1.0.0
 * @date 2015-10-07
 */
CorrectClearanceBatchOutputWidget::CorrectClearanceBatchOutputWidget(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CorrectClearanceBatchOutputWidget)
{
    ui->setupUi(this);

    connect(ui->cancelButton,SIGNAL(clicked()),this,SLOT(close()));

    connect(ui->exportButton, SIGNAL(clicked()), this, SLOT(exportAll()));

    group = new QButtonGroup(this);
    group->addButton(ui->radioButton_500mm);
    group->addButton(ui->radioButton_1000mm);
    group->addButton(ui->radioButton_2000mm);
    group->addButton(ui->radioButton_5000mm);
    group->addButton(ui->radioButton_10000mm);

    group->setId(ui->radioButton_500mm, 0);
    group->setId(ui->radioButton_1000mm, 1);
    group->setId(ui->radioButton_2000mm, 2);
    group->setId(ui->radioButton_5000mm, 3);
    group->setId(ui->radioButton_10000mm, 4);

    ui->progressBar->setVisible(false);
}

CorrectClearanceBatchOutputWidget::~CorrectClearanceBatchOutputWidget()
{
    delete ui;
}

void CorrectClearanceBatchOutputWidget::setMinUnit(float minUnit)
{
    this->interframe_mile = minUnit;
}

void CorrectClearanceBatchOutputWidget::initProgressBar(long long start, long long end)
{
    ui->progressBar->setMinimum(start);
    ui->progressBar->setMaximum(end);
    ui->progressBar->setValue(start);
    ui->progressBar->setVisible(true);
    this->update();
}

void CorrectClearanceBatchOutputWidget::setProgressBar(long long current)
{
    ui->progressBar->setValue(current);
    this->update();
}

void CorrectClearanceBatchOutputWidget::endProgressBar()
{
    ui->progressBar->setValue(ui->progressBar->maximum());
    this->update();
}

void CorrectClearanceBatchOutputWidget::exportAll()
{
    // 外部做判断，并选择文件夹
    /*int result = QMessageBox::warning(this,tr("警告"), tr("确认导出全部？"),
                                        QMessageBox::Yes | QMessageBox::No);
    if (result == QMessageBox::No)
        return;
    else if (result == QMessageBox::Yes)*/
    {
        int tmp;
        switch(group->checkedId())
        {
            case 0: tmp = 500; break; 
            case 1: tmp = 1000; break; 
            case 2: tmp = 2000; break; 
            case 3: tmp = 5000; break; 
            case 4: tmp = 10000; break; 
            default: tmp = 500; break;

        }
        emit exportAll(tmp);
    }
}