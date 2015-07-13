#include "triggersetting.h"
#include "ui_triggersetting.h"
#include <QMessageBox>

#include "masterprogram.h"

/**
 * 采集状态监控界面,设置采集触发状态
 * @author 范翔
 * @version 1.0.0
 * @date 2014-10-23
 */
TriggerSettingWidget::TriggerSettingWidget(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TriggerSettingWidget)
{
    ui->setupUi(this);

    cameraCollectMode = LzCameraCollectingMode::Lz_Camera_HardwareTrigger;
    ui->comboBox_3->setCurrentIndex(cameraCollectMode);

    defaultcomboindex = LzCollectingMode::Lz_Collecting_Manual_DistanceMode;

    distanceMode = LzCollectHardwareTriggerDistanceMode::Lz_HardwareTrigger_500mm;
    noDistanceMode = LzCollectHardwareTriggerNoDistanceMode::Lz_Collecting_FreeMode_30Hz;

    // 默认方式
    ui->comboBox->setCurrentIndex(defaultcomboindex);
    defaultcomboindex_2 = distanceMode;
    ui->comboBox_2->setCurrentIndex(defaultcomboindex_2);

    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(comboBox1currentIndexChanged(int)));
    connect(ui->comboBox_2, SIGNAL(currentIndexChanged(int)), this, SLOT(comboBox2currentIndexChanged(int)));

    changingComboBox2 = false;

    connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(setframes()));
}

TriggerSettingWidget::~TriggerSettingWidget()
{
    delete ui;
}

void TriggerSettingWidget::comboBox1currentIndexChanged(int index)
{
    // 手动进出洞、自动进出洞、自由触发模式
    defaultcomboindex = (LzCollectingMode)index;
    changingComboBox2 = true;
    switch (defaultcomboindex)
    {
    case Lz_Collecting_Manual_DistanceMode:
        ui->comboBox_2->clear();
        ui->comboBox_2->insertItems(0, QStringList()
            << QApplication::translate("TriggerSettingWidget", "0.5m", 0, QApplication::UnicodeUTF8)
            << QApplication::translate("TriggerSettingWidget", "0.75m", 0, QApplication::UnicodeUTF8)
            << QApplication::translate("TriggerSettingWidget", "1.0m", 0, QApplication::UnicodeUTF8)
            << QApplication::translate("TriggerSettingWidget", "1.25m", 0, QApplication::UnicodeUTF8)
            );
        ui->comboBox_2->setEnabled(true);
        break;
    case Lz_Collecting_Automatic_DistanceMode:
        ui->comboBox_2->clear();
        ui->comboBox_2->insertItems(0, QStringList()
            << QApplication::translate("TriggerSettingWidget", "0.5m", 0, QApplication::UnicodeUTF8)
            << QApplication::translate("TriggerSettingWidget", "0.75m", 0, QApplication::UnicodeUTF8)
            << QApplication::translate("TriggerSettingWidget", "1.0m", 0, QApplication::UnicodeUTF8)
            << QApplication::translate("TriggerSettingWidget", "1.25m", 0, QApplication::UnicodeUTF8)
            );
        ui->comboBox_2->setEnabled(true);
        break;
    case Lz_Collecting_FreeMode_NoDistanceMode:
        ui->comboBox_2->clear();
        ui->comboBox_2->insertItems(0, QStringList()
            << QApplication::translate("TriggerSettingWidget", "30Hz", 0, QApplication::UnicodeUTF8)
            << QApplication::translate("TriggerSettingWidget", "50Hz", 0, QApplication::UnicodeUTF8)
            << QApplication::translate("TriggerSettingWidget", "40Hz", 0, QApplication::UnicodeUTF8)
            << QApplication::translate("TriggerSettingWidget", "60Hz", 0, QApplication::UnicodeUTF8)
            );
        ui->comboBox_2->setEnabled(true);
        break;
    default:
        break;
    }
    changingComboBox2 = false;
}

void TriggerSettingWidget::comboBox2currentIndexChanged(int index)
{
    // 正在软件修改，不是人为选择
    //if (changingComboBox2)
    //    return;
    
    switch (defaultcomboindex)
    {
    case Lz_Collecting_Manual_DistanceMode:
        distanceMode = (LzCollectHardwareTriggerDistanceMode) ui->comboBox_2->currentIndex();
        defaultcomboindex_2 = distanceMode;
        break;
    case Lz_Collecting_Automatic_DistanceMode:
        distanceMode = (LzCollectHardwareTriggerDistanceMode) ui->comboBox_2->currentIndex();
        defaultcomboindex_2 = distanceMode;
        break;
    case Lz_Collecting_FreeMode_NoDistanceMode:
        noDistanceMode = (LzCollectHardwareTriggerNoDistanceMode) ui->comboBox_2->currentIndex();
        defaultcomboindex_2 = noDistanceMode;
        break;
    default:
        break;
    }
}

void TriggerSettingWidget::setframes()
{
    if ((ui->lineEdit->text() == ""))
    {
        QMessageBox::warning(this, tr("提示"), tr("请先输入曝光时间！"),QMessageBox::Yes | QMessageBox::No);
        return;
    }
    int exposureTime = ui->lineEdit->text().toInt();
    if (exposureTime <= 50 || exposureTime >= 10000)
    {
        QMessageBox::warning(this, tr("提示"), tr("曝光时间不在范围50-10000中，请重新输入！"), QMessageBox::Yes | QMessageBox::No);
        return;
    }
    int triggermode = ui->comboBox->currentIndex();
    int distancemode = ui->comboBox_3->currentIndex();

    defaultcomboindex_2 = ui->comboBox_2->currentIndex();

    MasterProgram::getMasterProgramInstance()->setTriggerModeAndExposureTime((LzCameraCollectingMode)triggermode, exposureTime, (LzCollectingMode)defaultcomboindex, defaultcomboindex_2);
    this->close();
}

void TriggerSettingWidget::on_pushButton_3_clicked()//重置按钮
{
    ui->lineEdit->setText("");
    ui->comboBox_3->setCurrentIndex(cameraCollectMode);
    ui->comboBox->setCurrentIndex(defaultcomboindex);
    ui->comboBox_2->setCurrentIndex(defaultcomboindex_2);

}

void TriggerSettingWidget::on_pushButton_2_clicked()//取消按钮
{
    this->close();
}
