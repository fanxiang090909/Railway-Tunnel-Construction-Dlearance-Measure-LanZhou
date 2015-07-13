#include "multiframecorrect.h"
#include "ui_multiframecorrect.h"

/**
 * 图形修正-多帧修正对话框界面类定义
 *
 * @author 熊雪
 * @version 1.0.0
 * @date 20140322
 */
MultiframeCorrect::MultiframeCorrect(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MultiframeCorrect)
{
    ui->setupUi(this);
}

MultiframeCorrect::~MultiframeCorrect()
{
    delete ui;
}
