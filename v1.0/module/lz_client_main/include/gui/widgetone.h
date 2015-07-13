#ifndef WIDGETONE_H
#define WIDGETONE_H

#include <QWidget>

namespace Ui {
    class WidgetOne;
}

class WidgetOne : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetOne(QWidget *parent = 0);
    ~WidgetOne();

private:
    Ui::WidgetOne *ui;

private slots:
    void changeTabWidge(int i);

    void slotBridgeToEdit(int, QString);
signals:
    void signalBridgeToEdit(int, QString);
};

#endif // WIDGETONE_H
