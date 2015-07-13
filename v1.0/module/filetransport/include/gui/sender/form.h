#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include "filelistsender.h"

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
    void on_startButton_clicked();
    void on_susButton_clicked();

    /**** TO paste to widget two of masterGUI program ****/

private:
    Ui::Form *ui;
    FileListSender *filesender;
};

#endif // FORM_H
