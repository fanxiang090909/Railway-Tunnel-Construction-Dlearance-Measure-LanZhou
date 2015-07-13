#include "status_calcu_noslave.h"

#include "ui_status_calcu.h"
#include "status_calcu.h"

/**
 * 计算状态显示主控界面
 * @author 范翔
 * @version 1.0.0
 * @date 2014-12-06
 */

CalcuStatusNoSlaveWidget::CalcuStatusNoSlaveWidget(QWidget *parent) : CalcuWidget(parent)
{
}

CalcuStatusNoSlaveWidget::~CalcuStatusNoSlaveWidget()
{
}

void CalcuStatusNoSlaveWidget::on_stopCalcuButton_clicked()
{
    CalcuWidget::on_stopCalcuButton_clicked();

}

void CalcuStatusNoSlaveWidget::on_stopFuseButton_clicked()
{
    CalcuWidget::on_stopFuseButton_clicked();

}

void CalcuStatusNoSlaveWidget::calculate_beginStartAll()
{
    emit startcalcu(0, 0);
}

void  CalcuStatusNoSlaveWidget::calculate_beginStartOneTunnel(int tunnelid)
{
    emit startcalcu(1, tunnelid);
}

void CalcuStatusNoSlaveWidget::fuse_beginStartAll()
{
    emit startcalcu(2, 0);
}

void CalcuStatusNoSlaveWidget::fuse_beginStartOneTunnel(int tunnelid)
{
   emit startcalcu(3, tunnelid);
}