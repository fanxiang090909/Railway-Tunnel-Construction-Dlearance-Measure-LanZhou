#ifndef WIDGETHEADER_H
#define WIDGETHEADER_H

#include <QWidget>

namespace Ui {
    class WidgetHeader;
}

class WidgetHeader : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetHeader(QWidget *parent = 0);
    ~WidgetHeader();

private:
    Ui::WidgetHeader *ui;
};

#endif // WIDGETHEADER_H
