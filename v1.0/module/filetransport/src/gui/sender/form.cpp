#include "form.h"
#include "ui_form.h"

#include <QTextCodec>
#include <QTime>
#include "filesender.h"

Form::Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);

    filesender = new FileListSender();
}

Form::~Form()
{
    delete filesender;
}

void Form::on_startButton_clicked(){
    qDebug() <<"start chuanshu";

}

void Form::on_susButton_clicked(){

}
