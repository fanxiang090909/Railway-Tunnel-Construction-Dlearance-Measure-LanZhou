#include "widgetone.h"
#include "ui_widgetone.h"

#include <QDebug>

#include "manage_line.h"
#include "manage_tunnel.h"

WidgetOne::WidgetOne(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetOne)
{
    ui->setupUi(this);

    ManageLineWidget * widgetone = new ManageLineWidget(ui->tabLine);
    ManageTunnelWidget * widgettwo = new ManageTunnelWidget(ui->tabTunnel);

    QGridLayout *layout1 = new QGridLayout();
    layout1->addWidget(widgetone);
    ui->tabLine->setLayout(layout1);


    QGridLayout *layout2 = new QGridLayout();
    layout2->addWidget(widgettwo);
    ui->tabTunnel->setLayout(layout2);

    //connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(changeTabWidge(int)));
    // 切换界面信号槽
    connect(widgettwo, SIGNAL(signalBridgeToEdit(int, QString)), this, SLOT(slotBridgeToEdit(int, QString)));
}

WidgetOne::~WidgetOne()
{
    delete ui;
}

void WidgetOne::changeTabWidge(int i)
{
    if (i == 0)
    {
        qDebug() << "0";
    }
    else
    {
        qDebug() << "1";
    }
}

void WidgetOne::slotBridgeToEdit(int tunnelid, QString tunnelname)
{
    emit signalBridgeToEdit(tunnelid, tunnelname);
}

