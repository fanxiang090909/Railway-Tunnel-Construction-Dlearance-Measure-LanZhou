#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include "masterprogram.h"

namespace Ui {
    class Form;
}

class Form : public QWidget
{
    Q_OBJECT

public:
    explicit Form(QWidget *parent = 0);
    ~Form();


private slots:
    void on_pushButton_clicked();

    /**** TO paste to widget two of masterGUI program ****/
    void appendMsg(QString msg);
    void appendError(QString msg);

    void changeHardwareStatus(HardwareType, QString, HardwareStatus);


    /**** TO paste to widget two of masterGUI program ****/
    /**
     * 解析消息字符串
     */
    //void parseMsg(QString msg);
    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_9_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_10_clicked();

    void on_pushButton_11_clicked();

    void on_pushButton_12_clicked();

private:
    Ui::Form *ui;

    MasterProgram *m;

};

#endif // FORM_H
