#include "hardware_config.h"
#include "ui_hardware_config.h"
#include <QFileDialog>
//#include <QStatusBar>
#include <QTabWidget>
#include <QDebug>
#include <QFile>
#include <QTableWidget>
#include <QTablewidgetItem>
#include <QDebug>
#include <QMessageBox>

#include "xmlnetworkfileloader.h"
#include "slavemodel.h"
#include "network_config_list.h"
#include "masterprogram.h"
#include "setting_master.h"

/**
 * 主控机界面-硬件（网络、相机）参数配置设置界面类定义
 * @author 熊雪
 * @2013.10
 * @version 1.0.0
 */
HardwareConfigWidget::HardwareConfigWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HardwareConfigWidget)
{
    ui->setupUi(this);

    ui->hostLineEdit->setVisible(false);
    ui->portLineEdit->setVisible(false);

    loadNetworkConfigData();

    // qDebug()<<this->size();//获得当前界面的大小

    //状态栏的标签
    //    statusLabel =new QLabel();
    //    statusLabel->setMinimumSize(statusLabel->sizeHint());
    //    statusBar = new QStatusBar();
    //    statusBar->addWidget(statusLabel);
    //    statusBar->setStyleSheet(QString("QStatusBar::item{border:0px}"));

    //上一行代码表示当没有任何提示时，状态栏会有一个短短的竖线，其实，这是QLabel的边框。当没有内容显示时，QLabel只显示出自己的一个边框。
    //但是，很多情况下我们并不希望有这条竖线，于是，我们对statusBar()进行如下设置，把QStatusBar的子组件的border设置为0，也就是没有边框.
    //主要进行几个变量的初始化和几个信号和槽函数的关联

    connect(ui->ID,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(changeHardwareSetting(int, int)));

    connect(ui->okConfigButton, SIGNAL(clicked()), this, SLOT(okHardwareConfig()));

    connect(ui->selectSendFileButton, SIGNAL(clicked()), this, SLOT(selectFile()));

    //cellDoubleClicked(int,int)双击某一项，cellClicked(int,int)表示单击某一项
    //ui->ID->setSelectionBehavior(QAbstractItemView::SelectRows);//表示控件中按行选择
    ui->ID->setSelectionBehavior(QAbstractItemView::SelectItems);//表示控件中按单个元素选中选择
    ui->ID->hideColumn(FENZHONG_CALIB_XML);
    ui->ID->resizeColumnsToContents();
    ui->ID->horizontalHeader()->setStretchLastSection(true);

    connect(MasterProgram::getMasterProgramInstance()->getMultiThreadTcpServer(),
            SIGNAL(signalMsgToMaster(QString)), this, SLOT(parseMsg(QString)));
    connect(MasterProgram::getMasterProgramInstance()->getMultiThreadTcpServer(),
            SIGNAL(signalErrorToMaster(QString)), this, SLOT(parseMsg(QString)));

    // 文件发送按钮
    ui->sendOneConfigFileButton->setEnabled(false);
    ui->sendConfigFilesButton->setEnabled(false);
    // 文件发送按钮信号槽
    connect(ui->sendOneConfigFileButton, SIGNAL(clicked()), this, SLOT(sendone()));
    connect(ui->sendConfigFilesButton, SIGNAL(clicked()), this, SLOT(send()));

    // 保存配置文件按钮
    ui->okConfigButton->setEnabled(false);
}

HardwareConfigWidget::~HardwareConfigWidget()
{
    delete ui;
}

void HardwareConfigWidget::loadNetworkConfigData()
{
    int row = ui->ID->rowCount();
    //qDebug() << "widgetone row=" << row;
    if (NetworkConfigList::getNetworkConfigListInstance()->listsnid() == NULL || NetworkConfigList::getNetworkConfigListInstance()->listsnid()->size() == 0)
    {
        if (NetworkConfigList::getNetworkConfigListInstance()->listsnid()->length() == 0)
            qDebug() << tr("相机 Serial Number ID未加载");
        qDebug() << tr("相机 Serial Number ID未加载");
        return;
    }
    SlaveModel tmp = SlaveModel(0);
    // 将解析出的networkconfig.xml文件中的ip和端口值填入表格中
    for(int i=0;i<row;i++)
    {
        if(i%2==0)
        {
            tmp = NetworkConfigList::getNetworkConfigListInstance()->listsnid()->at(i/2);
            ui->ID->setItem(i,SLAVE_NO,new QTableWidgetItem(QString("%1").arg(tmp.getIndex())));
            ui->ID->setItem(i,SLAVE_IP,new QTableWidgetItem(tmp.getHostAddress().c_str()));
        }
        else
        {
            tmp = NetworkConfigList::getNetworkConfigListInstance()->listsnid()->at((i-1)/2);
            ui->ID->setItem(i,SLAVE_NO,new QTableWidgetItem(QString("%1").arg(tmp.getIndex())));
            ui->ID->setItem(i,SLAVE_IP,new QTableWidgetItem(tmp.getHostAddress().c_str()));
        }
    }
    //将解析出networkconfig.xml文件中的对应的xml文件名的值填入表格中
    for(int i=0;i<row/2;i++)
    {
        if(i%2==0) // 偶数
        {
            if(i/2==0)
                tmp = NetworkConfigList::getNetworkConfigListInstance()->listsnid()->at(i);
            else
                tmp = NetworkConfigList::getNetworkConfigListInstance()->listsnid()->at(i/2);
            ui->ID->setItem(i,CAMERA1_NAME,new QTableWidgetItem(tmp.box1.camera_ref.c_str()));
            ui->ID->setItem(i,CAMERA1_SN,new QTableWidgetItem(tmp.box1.camera_ref_sn.c_str()));
            ui->ID->setItem(i,CAMERA2_NAME,new QTableWidgetItem(tmp.box1.camera.c_str()));
            ui->ID->setItem(i,CAMERA2_SN,new QTableWidgetItem(tmp.box1.camera_sn.c_str()));
            ui->ID->setItem(i,CAMERA_CALIB_XML, new QTableWidgetItem(tmp.box1.box_camera_calib_file.c_str()));
            ui->ID->setItem(i,FENZHONG_CALIB_XML,new QTableWidgetItem(tmp.box1.box_fenzhong_calib_file.c_str()));
        }
        else
        {
            if(i/1==1)
                tmp = NetworkConfigList::getNetworkConfigListInstance()->listsnid()->at(i-1);
            else
                tmp = NetworkConfigList::getNetworkConfigListInstance()->listsnid()->at((i-1)/2);
            ui->ID->setItem(i,CAMERA1_NAME,new QTableWidgetItem(tmp.box2.camera_ref.c_str()));
            ui->ID->setItem(i,CAMERA1_SN,new QTableWidgetItem(tmp.box2.camera_ref_sn.c_str()));
            ui->ID->setItem(i,CAMERA2_NAME,new QTableWidgetItem(tmp.box2.camera.c_str()));
            ui->ID->setItem(i,CAMERA2_SN,new QTableWidgetItem(tmp.box2.camera_sn.c_str()));
            ui->ID->setItem(i,CAMERA_CALIB_XML, new QTableWidgetItem(tmp.box2.box_camera_calib_file.c_str()));
            ui->ID->setItem(i,FENZHONG_CALIB_XML,new QTableWidgetItem(tmp.box2.box_fenzhong_calib_file.c_str()));
        }
    }
    for (int i = row / 2; i < row; i++)
    {
        if (i % 2 != 0)
        {
            tmp = NetworkConfigList::getNetworkConfigListInstance()->listsnid()->at((i-1)/2);
            ui->ID->setItem(i,CAMERA1_NAME,new QTableWidgetItem(tmp.box2.camera_ref.c_str()));
            ui->ID->setItem(i,CAMERA1_SN,new QTableWidgetItem(tmp.box2.camera_ref_sn.c_str()));
            ui->ID->setItem(i,CAMERA2_NAME,new QTableWidgetItem(tmp.box2.camera.c_str()));
            ui->ID->setItem(i,CAMERA2_SN,new QTableWidgetItem(tmp.box2.camera_sn.c_str()));
            ui->ID->setItem(i,CAMERA_CALIB_XML, new QTableWidgetItem(tmp.box2.box_camera_calib_file.c_str()));
            ui->ID->setItem(i,FENZHONG_CALIB_XML,new QTableWidgetItem(tmp.box2.box_fenzhong_calib_file.c_str()));
        }
        else
        {
            tmp = NetworkConfigList::getNetworkConfigListInstance()->listsnid()->at(i/2);
            ui->ID->setItem(i,CAMERA1_NAME,new QTableWidgetItem(tmp.box1.camera_ref.c_str()));
            ui->ID->setItem(i,CAMERA1_SN,new QTableWidgetItem(tmp.box1.camera_ref_sn.c_str()));
            ui->ID->setItem(i,CAMERA2_NAME,new QTableWidgetItem(tmp.box1.camera.c_str()));
            ui->ID->setItem(i,CAMERA2_SN,new QTableWidgetItem(tmp.box1.camera_sn.c_str()));
            ui->ID->setItem(i,CAMERA_CALIB_XML, new QTableWidgetItem(tmp.box1.box_camera_calib_file.c_str()));
            ui->ID->setItem(i,FENZHONG_CALIB_XML,new QTableWidgetItem(tmp.box1.box_fenzhong_calib_file.c_str()));
        }
    }
    for(int i=0;i<row;i++)
    {
        //设置tablewidget的第0列和第1列不能编辑
        ui->ID->item(i,SLAVE_NO)->setFlags(Qt::ItemIsSelectable);
        ui->ID->item(i,SLAVE_IP)->setFlags(Qt::ItemIsSelectable);
        ui->ID->item(i,CAMERA1_NAME)->setFlags(Qt::ItemIsSelectable);
        ui->ID->item(i,CAMERA2_NAME)->setFlags(Qt::ItemIsSelectable);
    }
}

class SleeperThread : public QThread
{
public:
    static void msleep(unsigned _int64 msecs)
    {
        QThread::msleep(msecs);
    }
};

// 实现选择文件函数
void HardwareConfigWidget::selectFile()//打开文件
{
    QString file_name;
    QString openFileDir = MasterSetting::getSettingInstance()->getParentPath();
    fileName = QFileDialog::getOpenFileName(this, tr("发送文件"), openFileDir, "(*.*)"); 
    QFileInfo fi = QFileInfo(fileName);
    file_name = fi.fileName();
    if(!fileName.isEmpty())
    {
        ui->sendConfigFilesButton->setEnabled(true);
        ui->sendOneConfigFileButton->setEnabled(true);
        ui->sendConfigFilesButton->setStatusTip(QObject::tr("打开文件%1成功!").arg(fileName));
        //  ui->clientStatusLabel->setText(tr("打开参数配置文件文件%1成功!").arg(fileName));
        ui->status->setText(tr("打开参数配置文件文件成功!"));
        ui->filelineEdit->setText(tr("%1").arg(fileName));
    }
}
//该函数将在下面的"打开配置文件"按钮单击事件槽函数中调用
//void HardwareConfigWidget::openwenjian()
//{
//     emit clicked01("01");//参数文件打开成功
//}

void HardwareConfigWidget::changeHardwareSetting(int srow, int scloume)
{
    if (scloume != FENZHONG_CALIB_XML && scloume != CAMERA_CALIB_XML)
    {
        ui->status->setText("");
        return;
    }
    QString file_name;
    QString openFileDir = MasterSetting::getSettingInstance()->getParentPath();
    file_name = QFileDialog::getOpenFileName(this, tr("发送文件"), openFileDir, "(*.*)"); 
    qDebug()<<fileName;
    QFileInfo fi = QFileInfo(fileName);
    file_name =fi.fileName();
    //qDebug()<<file_name;
    if(!fileName.isEmpty())
    {
        ui->sendOneConfigFileButton->setEnabled(true);
        ui->status->setText(tr("打开参数配置文件成功!"));
        ui->ID->item(ui->ID->currentRow(),ui->ID->currentColumn())->setText(file_name);
    }
    else
    {
        ui->status->setTextColor(Qt::red);
        ui->status->setText(tr("打开参数配置文件失败,请重试！"));
    }
}

// 硬件参数做完修改后
void HardwareConfigWidget::okHardwareConfig()
{
    // 拷贝硬件参数文件
    QString openFileDir = MasterSetting::getSettingInstance()->getParentPath() + "/system/hardware_config.xml";
    QString projpath = LzProjectAccess::getLzProjectAccessInstance()->getProjectPath(LzProjectClass::Main);
    bool ret2 = QFile::copy(projpath, openFileDir);
    if (ret2 == false)
    {
        ui->status->setText(tr("拷贝硬件参数配置文件错误，目录中已有该文件%1").arg(openFileDir));
        return;
    }
}

// 实现连接函数
void HardwareConfigWidget::send()//链接到服务器，执行发送，调用QTabWidget中内容循环发送
{
    MasterProgram::getMasterProgramInstance()->getMultiThreadTcpServer()->sendFileToSlaves(fileName);
}

void HardwareConfigWidget::sendone()//链接到服务器，执行发送，调用QTabWidget中内容循环发送
{
    QModelIndex index = ui->ID->currentIndex();
    if (!index.isValid())
    {
        QMessageBox::warning(this,tr("提示"), tr("请先选择一行！"));
        return;
    }
    int i=ui->ID->currentRow();//获得当前行的索引
    //ui->pushButton_1->setEnabled(false);
    //初始化已发送字节为0
    QString slaveip = ui->ID->item(i,1)->text();//让hostLineEdit分别显示QTabWidget中的内容
    int slaveid = NetworkConfigList::getNetworkConfigListInstance()->findSlaveId(slaveip);
    //ui->portLineEdit->setText(ui->ID->item(0,1)->text());//让portLineEdit分别显示QTabWidget中的内容
    ui->hostLineEdit->setText(slaveip);//让hostLineEdit分别显示QTabWidget中的内容
    //ui->portLineEdit->setText(ui->ID->item(i,1)->text());//让portLineEdit分别显示QTabWidget中的内容
    ui->status->setText(tr("正在连接从控机%1中....，准备发送文件%2").arg(slaveid).arg(fileName));

    MasterProgram::getMasterProgramInstance()->getMultiThreadTcpServer()->sendFileToOneSlave(slaveip,fileName);
}

/**
 * 文件传输相关解析消息字符串
 * @param msg 自定义的消息，这里解析从从控机传到主控机的消息
 */
void HardwareConfigWidget::parseMsg(QString msg)
{
    QStringList strList = msg.split(",", QString::SkipEmptyParts);
    if (strList.length() < 2)
    {
        qDebug() << tr("解析字符出错") << msg;
        return;
    }
    int slaveid = strList.value(0).toInt();
    int msgid = strList.value(1).toInt();

    //qDebug() << msgid;
    if (msgid <= 0) // 主控自己通知
    {
        QString filename;
        int type = 0; // 文件接收还是发送，0是接收，1是发送
        if (msgid >= -11 && msgid <= -9)
        {
            if (strList.length() < 4)
            {
                qDebug() << QObject::tr("解析字符出错") << msg;
                return;
            }
            else
            {
                type = strList.at(2).toInt();
                filename = strList.at(3);
            }
        }
        switch(msgid)
        {
            case -9:
            {
                if (type == 0)
                    ui->status->setText(QObject::tr("[主控]  接收从控%1的文件%2成功").arg(slaveid).arg(filename));
                else
                    ui->status->setText(QObject::tr("[主控]  发送至从控%1的文件%2成功").arg(slaveid).arg(filename));
                break;
            }
            case -10:
            {
                if (type == 0)
                    ui->status->setText(QObject::tr("[主控]  接收从控%1的文件%2时打开文件失败").arg(slaveid).arg(filename));
                else
                    ui->status->setText(QObject::tr("[主控]  发送至从控%1的文件%2时打开文件失败").arg(slaveid).arg(filename));
                break;
            }
            case -11:
            {
                if (strList.length() < 5)
                {
                    qDebug() << QObject::tr("解析字符出错") << msg;
                    return;
                }
                QString error = strList.at(4);
                if (type == 0)
                    ui->status->setText(QObject::tr("[主控]  接收从控%1的文件%2出错%3").arg(slaveid).arg(filename).arg(error));
                else
                    ui->status->setText(QObject::tr("[主控]  发送至从控%1的文件%2出错%3").arg(slaveid).arg(filename).arg(error));
                break;
            }
            default:;
        }
    }
}
