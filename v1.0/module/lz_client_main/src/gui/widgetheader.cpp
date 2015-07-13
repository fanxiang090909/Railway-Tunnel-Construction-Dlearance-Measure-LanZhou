#include "widgetheader.h"
#include "ui_widgetheader.h"

WidgetHeader::WidgetHeader(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetHeader)
{
    ui->setupUi(this);
}

WidgetHeader::~WidgetHeader()
{
    delete ui;
}
