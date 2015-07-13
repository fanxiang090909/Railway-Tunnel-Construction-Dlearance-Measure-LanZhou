#ifndef MULTIFRAMECORRECT_H
#define MULTIFRAMECORRECT_H

#include <QDialog>

namespace Ui {
    class MultiframeCorrect;
}

/**
 * 图形修正-多帧修正对话框界面类声明
 *
 * @author 熊雪
 * @version 1.0.0
 * @date 20140322
 */
class MultiframeCorrect : public QDialog
{
    Q_OBJECT

public:
    explicit MultiframeCorrect(QWidget *parent = 0);
    ~MultiframeCorrect();

private:
    Ui::MultiframeCorrect *ui;
};

#endif // MULTIFRAMECORRECT_H
