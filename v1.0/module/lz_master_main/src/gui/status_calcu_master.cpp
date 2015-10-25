#include "status_calcu_master.h"

#include "ui_status_calcu.h"
#include "status_calcu.h"

#include "masterprogram.h"
#include <QMessageBox>

/**
 * 计算状态显示主控界面
 * @author 范翔
 * @version 1.0.0
 * @date 2014-12-06
 */

CalcuMasterWidget::CalcuMasterWidget(QWidget *parent) : CalcuWidget(parent)
{
}

CalcuMasterWidget::~CalcuMasterWidget()
{
}

void CalcuMasterWidget::saveResetFile()
{
    CalcuWidget::saveResetFile();
}

void CalcuMasterWidget::on_stopCalcuButton_clicked()
{
    CalcuWidget::on_stopCalcuButton_clicked();
    MasterProgram::getMasterProgramInstance()->calculate_Stop();
}

void CalcuMasterWidget::on_stopFuseButton_clicked()
{
    CalcuWidget::on_stopFuseButton_clicked();
    MasterProgram::getMasterProgramInstance()->calculate_Fuse_stop();
}

void CalcuMasterWidget::calculate_beginStartAll()
{
    MasterProgram::getMasterProgramInstance()->calculate_beginStartAll();
}

void CalcuMasterWidget::calculate_beginStartOneTunnel(int tunnelid)
{
    MasterProgram::getMasterProgramInstance()->calculate_beginStartOneTunnel(tunnelid);
}

void CalcuMasterWidget::fuse_beginStartAll()
{
    MasterProgram::getMasterProgramInstance()->calculate_Fuse_beginStartAll(useerrrectifyfactor, usesavetyfactor);
}

void CalcuMasterWidget::fuse_beginStartOneTunnel(int tunnelid)
{
    MasterProgram::getMasterProgramInstance()->calculate_Fuse_beginStartOneTunnel(tunnelid, useerrrectifyfactor, usesavetyfactor);
}

void CalcuMasterWidget::extract_beginStartAll()
{
    MasterProgram::getMasterProgramInstance()->calculate_ExtractHeight_beginStartAll(useerrrectifyfactor, usesavetyfactor);
}

void CalcuMasterWidget::extract_beginStartOneTunnel(int tunnelid)
{
    MasterProgram::getMasterProgramInstance()->calculate_ExtractHeight_beginStartOneTunnel(tunnelid, useerrrectifyfactor, usesavetyfactor);
}