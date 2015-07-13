#ifndef HARDWARE_CONFIG_H
#define HARDWARE_CONFIG_H

#include <QWidget>
#include <QtNetwork>
#include <QLabel>

namespace Ui {
    class HardwareConfigWidget;
}

/**
 * 主控机界面-硬件（网络、相机）参数配置设置界面类声明
 * @author 熊雪
 * @2013.10
 * @version 1.0.0
 */
class HardwareConfigWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HardwareConfigWidget(QWidget *parent = 0);
    ~HardwareConfigWidget();

    void loadNetworkConfigData();

private:

    enum networkformCOL
    {
        SLAVE_NO = 0,
        SLAVE_IP = 1,
        CAMERA1_NAME = 2,
        CAMERA1_SN = 3,
        CAMERA2_NAME = 4,
        CAMERA2_SN = 5,
        CAMERA_CALIB_XML = 6,
        FENZHONG_CALIB_XML = 7
    };

    Ui::HardwareConfigWidget *ui;

    QString fileName;//保存文件路径

    QLabel *statusLabel;//状态栏中的label
    // QStatusBar *statusBar;

private slots:

    // 硬件参数做完修改后
    void okHardwareConfig();

    // 更换硬件配置tablewidget的每个单元格更改时槽函数
    // 在tablewidget中打开某个配置文件
    void changeHardwareSetting(int, int);

    // 选择要发送的文件
    void selectFile();

    // 发送文件
    void send();
    void sendone();

    /**
     * 解析消息字符串
     * @param msg 自定义的消息，这里解析从从控机传到主控机的消息
     * @see 《主控从控程序流程及消息设计.ppt》版本v1.0
     */
    void parseMsg(QString msg);
};

#endif // HARDWARE_CONFIG_H
