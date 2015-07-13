#ifndef SETVALIDFRAMES_H
#define SETVALIDFRAMES_H

#include <QDialog>

namespace Ui {
    class SetValidFrames;
}

/**
 * 采集文件信息校对界面——设置有效帧数界面管理类声明
 * @author 熊雪
 * @version 1.0.0
 * @date 2014-01-20
 */
class SetValidFrames : public QDialog
{
    Q_OBJECT

public:
    explicit SetValidFrames(QWidget *parent = 0);
    ~SetValidFrames();


public slots:
    /** 
     * 是否显示下方的Widget
     */
    void showWidget(bool ifshow);

    void setShowWidgetDisVisible(bool);

private slots:
    void setframes();
    void on_pushButton_3_clicked();
    //void on_pushButton_2_clicked();

    /**
     * 生成临时工程槽函数
     */
    void createTempProject();

signals:
    void sendframestocheck_task(long long ,long long);

    void sendframestocheck_task_createtmppro(long long, long long);

private:
    Ui::SetValidFrames *ui;
};

#endif // SETVALIDFRAMES_H
