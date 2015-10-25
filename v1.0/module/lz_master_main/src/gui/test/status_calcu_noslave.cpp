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
	// 范翔定义新按钮，从融合fuse结果计算syn提取高度结果
	ui->startExtractHeightButton->setVisible(true);
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
    emit startcalcu(0, 0, false, false);
}

void  CalcuStatusNoSlaveWidget::calculate_beginStartOneTunnel(int tunnelid)
{
    emit startcalcu(1, tunnelid, false, false);
}

void CalcuStatusNoSlaveWidget::fuse_beginStartAll()
{
    emit startcalcu(2, 0, useerrrectifyfactor, usesavetyfactor);
}

void CalcuStatusNoSlaveWidget::fuse_beginStartOneTunnel(int tunnelid)
{
    emit startcalcu(3, tunnelid, useerrrectifyfactor, usesavetyfactor);
}

void CalcuStatusNoSlaveWidget::extract_beginStartAll()
{
	emit startcalcu(4, 0, useerrrectifyfactor, usesavetyfactor);
}

void CalcuStatusNoSlaveWidget::extract_beginStartOneTunnel(int tunnelid)
{
	emit startcalcu(5, tunnelid, useerrrectifyfactor, usesavetyfactor);
}