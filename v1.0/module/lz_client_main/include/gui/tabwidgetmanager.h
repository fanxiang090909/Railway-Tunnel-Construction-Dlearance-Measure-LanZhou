#ifndef WIDGETMANAGER_H
#define WIDGETMANAGER_H

#include <QWidget>

namespace Ui {
    class TabWidgetManager;
}

/**
 * 左侧按钮切换页面tabwidget界面类声明
 * @author fanxiang
 * @version 1.0.0
 * @date 2013-08-18
 */
class TabWidgetManager : public QWidget
{
    Q_OBJECT

public:
    explicit TabWidgetManager(QWidget *parent = 0);
    ~TabWidgetManager();

signals:
    void my_close();

public slots:
    void my_close_slot();

private slots:

    void button1Clicked();
    void button2Clicked();
    void button3Clicked();
    void button4Clicked();
    void button5Clicked();
    void button6Clicked();
    void button7Clicked();
    void button8Clicked();

private:
    Ui::TabWidgetManager *ui;

    // 转变TabButton格式
    void setTabButtonStyle(int i);
};

#endif // WIDGETMANAGER_H
