#include "status_collect.h"
#include "ui_status_collect.h"
#include <QMessageBox>
#include "masterprogram.h"

/**
 * 监控界面——采集界面类定义
 * @author 熊雪
 * @author 范翔
 * @version 1.0.0
 * @date 2014-03-01
 */
CollectWidget::CollectWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CollectWidget)
{
    ui->setupUi(this);

    // 不查看主控机帧号 @date 20150625
    ui->masterLabel_status->setVisible(false);
    // 设置相机的状态
    // statuscameramessage为从控机发送过来的采集机和备份机的状态信息，如0表示正常，则显示为黄色，1表示异常，则显示为红色。
    
    // 不查看激光器状态
    ui->laser1Label->setVisible(false);
    ui->laser1Label_work->setVisible(false);
    ui->laser2Label->setVisible(false);
    ui->laser2Label_work->setVisible(false);
    ui->laser3Label->setVisible(false);
    ui->laser3Label_work->setVisible(false);
    ui->laser4Label->setVisible(false);
    ui->laser4Label_work->setVisible(false);
    ui->laser5Label->setVisible(false);
    ui->laser5Label_work->setVisible(false);
    ui->laser6Label->setVisible(false);
    ui->laser6Label_work->setVisible(false);
    ui->laser7Label->setVisible(false);
    ui->laser7Label_work->setVisible(false);
    ui->laser8Label->setVisible(false);
    ui->laser8Label_work->setVisible(false);
    ui->laser9Label->setVisible(false);
    ui->laser9Label_work->setVisible(false);

    // 初始时关闭所有硬件
    turnOffHardwares();

    connect(ui->modeSettingButton, SIGNAL(clicked()), this, SLOT(showModeSettingWidget()));

    connect(ui->intoTunnelValidButton, SIGNAL(clicked()), this, SLOT(intoTunnelValid()));
    connect(ui->intoTunnelNotValidButton, SIGNAL(clicked()), this, SLOT(intoTunnelNotValid()));
    connect(ui->outFromTunnelButton, SIGNAL(clicked()), this, SLOT(outFromTunnel()));
    connect(ui->endCollectLineButton, SIGNAL(clicked()), this, SLOT(endCollectLine()));
    
    connect(ui->softwareTriggerOneFrameButton, SIGNAL(clicked()), this, SLOT(softwareTriggerOneFrame()));

    // 相机复位信号槽
    connect(ui->collectResetButton, SIGNAL(clicked()), this, SLOT(hardwareReset()));

    // 初始不让点击
    ui->outFromTunnelButton->setEnabled(false);
    ui->endCollectLineButton->setEnabled(false);
    ui->softwareTriggerOneFrameButton->setEnabled(true);
    ui->softwareTriggerOneFrameButton->setVisible(false);
}

CollectWidget::~CollectWidget()
{
    delete ui;
}

// 关闭所有提示灯
void CollectWidget::turnOffHardwares()
{
    QString img = ":/image/lightgrey.png";

    ui->collectlabel->setPixmap(QPixmap(img));
    //ui->masterLabel_task->setText("");
    //ui->masterLabel_status->setText("");

    // 相机灯变红
    ui->A1Label_work->setPixmap(QPixmap(img));
    ui->A2Label_work->setPixmap(QPixmap(img));
    ui->B1Label_work->setPixmap(QPixmap(img));
    ui->B2Label_work->setPixmap(QPixmap(img));
    ui->C1Label_work->setPixmap(QPixmap(img));
    ui->C2Label_work->setPixmap(QPixmap(img));
    ui->D1Label_work->setPixmap(QPixmap(img));
    ui->D2Label_work->setPixmap(QPixmap(img));
    ui->E1Label_work->setPixmap(QPixmap(img));
    ui->E2Label_work->setPixmap(QPixmap(img));
    ui->F1Label_work->setPixmap(QPixmap(img));
    ui->F2Label_work->setPixmap(QPixmap(img));
    ui->G1Label_work->setPixmap(QPixmap(img));
    ui->G2Label_work->setPixmap(QPixmap(img));
    ui->H1Label_work->setPixmap(QPixmap(img));
    ui->H2Label_work->setPixmap(QPixmap(img));
    ui->I1Label_work->setPixmap(QPixmap(img));
    ui->I2Label_work->setPixmap(QPixmap(img));
    ui->J1Label_work->setPixmap(QPixmap(img));
    ui->J2Label_work->setPixmap(QPixmap(img));
    ui->K1Label_work->setPixmap(QPixmap(img));
    ui->K2Label_work->setPixmap(QPixmap(img));
    ui->L1Label_work->setPixmap(QPixmap(img));
    ui->L2Label_work->setPixmap(QPixmap(img));
    ui->M1Label_work->setPixmap(QPixmap(img));
    ui->M2Label_work->setPixmap(QPixmap(img));
    ui->N1Label_work->setPixmap(QPixmap(img));
    ui->N2Label_work->setPixmap(QPixmap(img));
    ui->O1Label_work->setPixmap(QPixmap(img));
    ui->O2Label_work->setPixmap(QPixmap(img));
    ui->P1Label_work->setPixmap(QPixmap(img));
    ui->P2Label_work->setPixmap(QPixmap(img));
    ui->Q1Label_work->setPixmap(QPixmap(img));
    ui->Q2Label_work->setPixmap(QPixmap(img));
    ui->R1Label_work->setPixmap(QPixmap(img));
    ui->R2Label_work->setPixmap(QPixmap(img));

    ui->A1Label_status->setText("");
    ui->A2Label_status->setText("");
    ui->B1Label_status->setText("");
    ui->B2Label_status->setText("");
    ui->C1Label_status->setText("");
    ui->C2Label_status->setText("");
    ui->D1Label_status->setText("");
    ui->D2Label_status->setText("");
    ui->E1Label_status->setText("");
    ui->E2Label_status->setText("");
    ui->F1Label_status->setText("");
    ui->F2Label_status->setText("");
    ui->G1Label_status->setText("");
    ui->G2Label_status->setText("");
    ui->H1Label_status->setText("");
    ui->H2Label_status->setText("");
    ui->I1Label_status->setText("");
    ui->I2Label_status->setText("");
    ui->J1Label_status->setText("");
    ui->J2Label_status->setText("");
    ui->K1Label_status->setText("");
    ui->K2Label_status->setText("");
    ui->L1Label_status->setText("");
    ui->L2Label_status->setText("");
    ui->M1Label_status->setText("");
    ui->M2Label_status->setText("");
    ui->N1Label_status->setText("");
    ui->N2Label_status->setText("");
    ui->O1Label_status->setText("");
    ui->O2Label_status->setText("");
    ui->P1Label_status->setText("");
    ui->P2Label_status->setText("");
    ui->Q1Label_status->setText("");
    ui->Q2Label_status->setText("");
    ui->R1Label_status->setText("");
    ui->R2Label_status->setText("");

    ui->A1Label_task->setText("");
    ui->A2Label_task->setText("");
    ui->B1Label_task->setText("");
    ui->B2Label_task->setText("");
    ui->C1Label_task->setText("");
    ui->C2Label_task->setText("");
    ui->D1Label_task->setText("");
    ui->D2Label_task->setText("");
    ui->E1Label_task->setText("");
    ui->E2Label_task->setText("");
    ui->F1Label_task->setText("");
    ui->F2Label_task->setText("");
    ui->G1Label_task->setText("");
    ui->G2Label_task->setText("");
    ui->H1Label_task->setText("");
    ui->H2Label_task->setText("");
    ui->I1Label_task->setText("");
    ui->I2Label_task->setText("");
    ui->J1Label_task->setText("");
    ui->J2Label_task->setText("");
    ui->K1Label_task->setText("");
    ui->K2Label_task->setText("");
    ui->L1Label_task->setText("");
    ui->L2Label_task->setText("");
    ui->M1Label_task->setText("");
    ui->M2Label_task->setText("");
    ui->N1Label_task->setText("");
    ui->N2Label_task->setText("");
    ui->O1Label_task->setText("");
    ui->O2Label_task->setText("");
    ui->P1Label_task->setText("");
    ui->P2Label_task->setText("");
    ui->Q1Label_task->setText("");
    ui->Q2Label_task->setText("");
    ui->R1Label_task->setText("");
    ui->R2Label_task->setText("");

    // 激光器灯变红
    ui->laser1Label_work->setPixmap(QPixmap(img));
    ui->laser2Label_work->setPixmap(QPixmap(img));
    ui->laser3Label_work->setPixmap(QPixmap(img));
    ui->laser4Label_work->setPixmap(QPixmap(img));
    ui->laser5Label_work->setPixmap(QPixmap(img));
    ui->laser6Label_work->setPixmap(QPixmap(img));
    ui->laser7Label_work->setPixmap(QPixmap(img));
    ui->laser8Label_work->setPixmap(QPixmap(img));
    ui->laser9Label_work->setPixmap(QPixmap(img));

    // 初始不让点击
    initCollectCtrlButton();
}

void  CollectWidget::initCollectCtrlButton()
{
    // 初始不让点击
    ui->outFromTunnelButton->setEnabled(false);
    ui->endCollectLineButton->setEnabled(false);
    ui->softwareTriggerOneFrameButton->setEnabled(true);
    ui->intoTunnelNotValidButton->setEnabled(true);
    ui->intoTunnelValidButton->setEnabled(true);

    connect(MasterProgram::getMasterProgramInstance(), SIGNAL(InTunnel(bool)), this, SLOT(InTunnel_UpdateUI(bool)));
    connect(MasterProgram::getMasterProgramInstance(), SIGNAL(OutTunnel()), this, SLOT(OutTunnel_UpdateUI()));
}

// 改变相机提示灯
void CollectWidget::changeCamera(QString cameraindex, HardwareStatus status)
{
    QString img;
    switch (status)
    {
        case Hardware_Off: img = ":/image/lightgrey.png";break;
        case Hardware_OnButFree: img = ":/image/lightyellow.png";break;
        case Hardware_ExceptionOff: img = ":/image/lightred.png";break;
        case Hardware_NotDetect: img = ":/image/lightred.png";break;
        case Hardware_Working: img = ":/image/lightgreen.png";break;
        case Hardware_Broken: img = ":/image/lightred.png";break;
        default: img = ":/image/lightred.png";break;
    }
    //qDebug() << cameraindex;
    if (cameraindex.compare("A1") == 0)
        ui->A1Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("A2") == 0)
        ui->A2Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("B1") == 0)
        ui->B1Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("B2") == 0)
        ui->B2Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("C1") == 0)
        ui->C1Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("C2") == 0)
        ui->C2Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("D1") == 0)
        ui->D1Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("D2") == 0)
        ui->D2Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("E1") == 0)
        ui->E1Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("E2") == 0)
        ui->E2Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("F1") == 0)
        ui->F1Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("F2") == 0)
        ui->F2Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("G1") == 0)
        ui->G1Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("G2") == 0)
        ui->G2Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("H1") == 0)
        ui->H1Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("H2") == 0)
        ui->H2Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("I1") == 0)
        ui->I1Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("I2") == 0)
        ui->I2Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("J1") == 0)
        ui->J1Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("J2") == 0)
        ui->J2Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("K1") == 0)
        ui->K1Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("K2") == 0)
        ui->K2Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("L1") == 0)
        ui->L1Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("L2") == 0)
        ui->L2Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("M1") == 0)
        ui->M1Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("M2") == 0)
        ui->M2Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("N1") == 0)
        ui->N1Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("N2") == 0)
        ui->N2Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("O1") == 0)
        ui->O1Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("O2") == 0)
        ui->O2Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("P1") == 0)
        ui->P1Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("P2") == 0)
        ui->P2Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("Q1") == 0)
        ui->Q1Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("Q2") == 0)
        ui->Q2Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("R1") == 0)
        ui->R1Label_work->setPixmap(QPixmap(img));
    else if (cameraindex.compare("R2") == 0)
        ui->R2Label_work->setPixmap(QPixmap(img));
}

void CollectWidget::changeCameraFC(QString cameraindex, qint64 newfc)
{
    if (cameraindex.compare("master") == 0)
        ui->masterLabel_status->setText(QString("%1").arg(newfc));
    else if (cameraindex.compare("A1") == 0)
        ui->A1Label_status->setText(QString("%1").arg(newfc));
    else if (cameraindex.compare("A2") == 0)
        ui->A2Label_status->setText(QString("%1").arg(newfc));
    else if (cameraindex.compare("B1") == 0)
        ui->B1Label_status->setText(QString("%1").arg(newfc));
    else if (cameraindex.compare("B2") == 0)
        ui->B2Label_status->setText(QString("%1").arg(newfc));
    else if (cameraindex.compare("C1") == 0)
        ui->C1Label_status->setText(QString("%1").arg(newfc));
    else if (cameraindex.compare("C2") == 0)
        ui->C2Label_status->setText(QString("%1").arg(newfc));
    else if (cameraindex.compare("D1") == 0)
        ui->D1Label_status->setText(QString("%1").arg(newfc));
    else if (cameraindex.compare("D2") == 0)
        ui->D2Label_status->setText(QString("%1").arg(newfc));
    else if (cameraindex.compare("E1") == 0)
        ui->E1Label_status->setText(QString("%1").arg(newfc));
    else if (cameraindex.compare("E2") == 0)
        ui->E2Label_status->setText(QString("%1").arg(newfc));
    else if (cameraindex.compare("F1") == 0)
        ui->F1Label_status->setText(QString("%1").arg(newfc));
    else if (cameraindex.compare("F2") == 0)
        ui->F2Label_status->setText(QString("%1").arg(newfc));
    else if (cameraindex.compare("G1") == 0)
        ui->G1Label_status->setText(QString("%1").arg(newfc));
    else if (cameraindex.compare("G2") == 0)
        ui->G2Label_status->setText(QString("%1").arg(newfc));
    else if (cameraindex.compare("H1") == 0)
        ui->H1Label_status->setText(QString("%1").arg(newfc));
    else if (cameraindex.compare("H2") == 0)
        ui->H2Label_status->setText(QString("%1").arg(newfc));
    else if (cameraindex.compare("I1") == 0)
        ui->I1Label_status->setText(QString("%1").arg(newfc));
    else if (cameraindex.compare("I2") == 0)
        ui->I2Label_status->setText(QString("%1").arg(newfc));
    else if (cameraindex.compare("J1") == 0)
        ui->J1Label_status->setText(QString("%1").arg(newfc));
    else if (cameraindex.compare("J2") == 0)
        ui->J2Label_status->setText(QString("%1").arg(newfc));
    else if (cameraindex.compare("K1") == 0)
        ui->K1Label_status->setText(QString("%1").arg(newfc));
    else if (cameraindex.compare("K2") == 0)
        ui->K2Label_status->setText(QString("%1").arg(newfc));
    else if (cameraindex.compare("L1") == 0)
        ui->L1Label_status->setText(QString("%1").arg(newfc));
    else if (cameraindex.compare("L2") == 0)
        ui->L2Label_status->setText(QString("%1").arg(newfc));
    else if (cameraindex.compare("M1") == 0)
        ui->M1Label_status->setText(QString("%1").arg(newfc));
    else if (cameraindex.compare("M2") == 0)
        ui->M2Label_status->setText(QString("%1").arg(newfc));
    else if (cameraindex.compare("N1") == 0)
        ui->N1Label_status->setText(QString("%1").arg(newfc));
    else if (cameraindex.compare("N2") == 0)
        ui->N2Label_status->setText(QString("%1").arg(newfc));
    else if (cameraindex.compare("O1") == 0)
        ui->O1Label_status->setText(QString("%1").arg(newfc));
    else if (cameraindex.compare("O2") == 0)
        ui->O2Label_status->setText(QString("%1").arg(newfc));
    else if (cameraindex.compare("P1") == 0)
        ui->P1Label_status->setText(QString("%1").arg(newfc));
    else if (cameraindex.compare("P2") == 0)
        ui->P2Label_status->setText(QString("%1").arg(newfc));
    else if (cameraindex.compare("Q1") == 0)
        ui->Q1Label_status->setText(QString("%1").arg(newfc));
    else if (cameraindex.compare("Q2") == 0)
        ui->Q2Label_status->setText(QString("%1").arg(newfc));
    else if (cameraindex.compare("R1") == 0)
        ui->R1Label_status->setText(QString("%1").arg(newfc));
    else if (cameraindex.compare("R2") == 0)
        ui->R2Label_status->setText(QString("%1").arg(newfc));
}

void CollectWidget::changeCameraTask(WorkingStatus status, QString cameraindex, int threadid, QString newtask, QString remark)
{
    if (cameraindex.compare("master") == 0)
        ui->masterLabel_task->setText(newtask);
    else if (cameraindex.compare("A1") == 0)
        ui->A1Label_task->setText(newtask);
    else if (cameraindex.compare("A2") == 0)
        ui->A2Label_task->setText(newtask);
    else if (cameraindex.compare("B1") == 0)
        ui->B1Label_task->setText(newtask);
    else if (cameraindex.compare("B2") == 0)
        ui->B2Label_task->setText(newtask);
    else if (cameraindex.compare("C1") == 0)
        ui->C1Label_task->setText(newtask);
    else if (cameraindex.compare("C2") == 0)
        ui->C2Label_task->setText(newtask);
    else if (cameraindex.compare("D1") == 0)
        ui->D1Label_task->setText(newtask);
    else if (cameraindex.compare("D2") == 0)
        ui->D2Label_task->setText(newtask);
    else if (cameraindex.compare("E1") == 0)
        ui->E1Label_task->setText(newtask);
    else if (cameraindex.compare("E2") == 0)
        ui->E2Label_task->setText(newtask);
    else if (cameraindex.compare("F1") == 0)
        ui->F1Label_task->setText(newtask);
    else if (cameraindex.compare("F2") == 0)
        ui->F2Label_task->setText(newtask);
    else if (cameraindex.compare("G1") == 0)
        ui->G1Label_task->setText(newtask);
    else if (cameraindex.compare("G2") == 0)
        ui->G2Label_task->setText(newtask);
    else if (cameraindex.compare("H1") == 0)
        ui->H1Label_task->setText(newtask);
    else if (cameraindex.compare("H2") == 0)
        ui->H2Label_task->setText(newtask);
    else if (cameraindex.compare("I1") == 0)
        ui->I1Label_task->setText(newtask);
    else if (cameraindex.compare("I2") == 0)
        ui->I2Label_task->setText(newtask);
    else if (cameraindex.compare("J1") == 0)
        ui->J1Label_task->setText(newtask);
    else if (cameraindex.compare("J2") == 0)
        ui->J2Label_task->setText(newtask);
    else if (cameraindex.compare("K1") == 0)
        ui->K1Label_task->setText(newtask);
    else if (cameraindex.compare("K2") == 0)
        ui->K2Label_task->setText(newtask);
    else if (cameraindex.compare("L1") == 0)
        ui->L1Label_task->setText(newtask);
    else if (cameraindex.compare("L2") == 0)
        ui->L2Label_task->setText(newtask);
    else if (cameraindex.compare("M1") == 0)
        ui->M1Label_task->setText(newtask);
    else if (cameraindex.compare("M2") == 0)
        ui->M2Label_task->setText(newtask);
    else if (cameraindex.compare("N1") == 0)
        ui->N1Label_task->setText(newtask);
    else if (cameraindex.compare("N2") == 0)
        ui->N2Label_task->setText(newtask);
    else if (cameraindex.compare("O1") == 0)
        ui->O1Label_task->setText(newtask);
    else if (cameraindex.compare("O2") == 0)
        ui->O2Label_task->setText(newtask);
    else if (cameraindex.compare("P1") == 0)
        ui->P1Label_task->setText(newtask);
    else if (cameraindex.compare("P2") == 0)
        ui->P2Label_task->setText(newtask);
    else if (cameraindex.compare("Q1") == 0)
        ui->Q1Label_task->setText(newtask);
    else if (cameraindex.compare("Q2") == 0)
        ui->Q2Label_task->setText(newtask);
    else if (cameraindex.compare("R1") == 0)
        ui->R1Label_task->setText(newtask);
    else if (cameraindex.compare("R2") == 0)
        ui->R2Label_task->setText(newtask);
}

// 改变激光器提示灯
void CollectWidget::changeLayser(QString layseridint, HardwareStatus status)
{
    QString img;
    switch (status)
    {
        case Hardware_Off: img = ":/image/lightred.png";break;
        case Hardware_OnButFree: img = ":/image/lightyellow.png";break;
        case Hardware_Working: img = ":/image/lightgreen.png";break;
        case Hardware_Broken: img = ":/image/lightred.png";break;
        default: img = ":/image/lightred.png";break;
    }
    //qDebug() << layseridint;
    if (layseridint.compare("0") == 0) // 主控控制
        ui->collectlabel->setPixmap(QPixmap(img));
    else if (layseridint.compare("laser1") == 0)
        ui->laser1Label_work->setPixmap(QPixmap(img));
    else if (layseridint.compare("laser2") == 0)
        ui->laser2Label_work->setPixmap(QPixmap(img));
    else if (layseridint.compare("laser3") == 0)
        ui->laser3Label_work->setPixmap(QPixmap(img));
    else if (layseridint.compare("laser4") == 0)
        ui->laser4Label_work->setPixmap(QPixmap(img));
    else if (layseridint.compare("laser5") == 0)
        ui->laser5Label_work->setPixmap(QPixmap(img));
    else if (layseridint.compare("laser6") == 0)
        ui->laser6Label_work->setPixmap(QPixmap(img));
    else if (layseridint.compare("laser7") == 0)
        ui->laser7Label_work->setPixmap(QPixmap(img));
    else if (layseridint.compare("laser8") == 0)
        ui->laser8Label_work->setPixmap(QPixmap(img));
    else if (layseridint.compare("laser9") == 0)
        ui->laser9Label_work->setPixmap(QPixmap(img));
}

/**
 * 设置采集触发工作模式,显示界面
 */
void CollectWidget::showModeSettingWidget()
{
    TriggerSettingWidget * settingwindow = new TriggerSettingWidget(this);
    settingwindow->show();
}

// 预进洞有效
void CollectWidget::intoTunnelValid()
{
    MasterProgram::getMasterProgramInstance()->collect_IntoTunnel(true);
}

// 预进洞无效
void CollectWidget::intoTunnelNotValid() 
{
    MasterProgram::getMasterProgramInstance()->collect_IntoTunnel(false);
}

// 出洞
void CollectWidget::outFromTunnel()
{
    MasterProgram::getMasterProgramInstance()->collect_OutFromTunnel();
}

// 对UI界面按钮的更新
// 预进洞有效 预进洞无效
void CollectWidget::InTunnel_UpdateUI(bool isvalid)
{
    if (isvalid)
    {
        ui->intoTunnelValidButton->setEnabled(false);
        ui->intoTunnelNotValidButton->setEnabled(false);
        ui->outFromTunnelButton->setEnabled(true);
        ui->endCollectLineButton->setEnabled(false);
    }
    else
    {
        ui->outFromTunnelButton->setEnabled(false);
        ui->endCollectLineButton->setEnabled(false);
    }
}

// 出洞
void CollectWidget::OutTunnel_UpdateUI()
{
    ui->intoTunnelValidButton->setEnabled(true);
    ui->intoTunnelNotValidButton->setEnabled(true);
    ui->outFromTunnelButton->setEnabled(false);
    ui->endCollectLineButton->setEnabled(true);
}

// 结束线路
void CollectWidget::endCollectLine()
{
    MasterProgram::getMasterProgramInstance()->collect_EndCurrentLine();
    ui->intoTunnelValidButton->setEnabled(false);
    ui->intoTunnelNotValidButton->setEnabled(false);
    ui->outFromTunnelButton->setEnabled(false);
    ui->softwareTriggerOneFrameButton->setEnabled(false);
    ui->endCollectLineButton->setEnabled(false);
}

void CollectWidget::softwareTriggerOneFrame()
{
    if (MasterProgram::getMasterProgramInstance()->getTriggerMode() == Lz_Camera_SoftwareTrigger)
        MasterProgram::getMasterProgramInstance()->collect_SoftwareTriggerOneFrame();
    else
        QMessageBox::warning(this, tr("提示"), tr("正在硬触发模式下，不能软触发！"),QMessageBox::Yes | QMessageBox::No);
}

/**
 * 相机复位
 */
void CollectWidget::hardwareReset()
{
    int ret = QMessageBox::warning(this, tr("提示"), tr("您确认硬件发生异常，各个从机采集复位？"),QMessageBox::Yes | QMessageBox::No);
    if (ret == QMessageBox::Yes)
        MasterProgram::getMasterProgramInstance()->collect_ResetSlaves();
}