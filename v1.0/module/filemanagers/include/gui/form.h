#ifndef FORM_H
#define FORM_H

#include <QWidget>

#include <QDir>
#include <QString>
#include <QFile>
#include <QFileInfo>
#include <QTime>

#include "headdefine.h"
#include "LzSerialStorageAcqui.h"

namespace Ui {
    class Form;
}

class Form : public QWidget
{
    Q_OBJECT

public:
    explicit Form(QWidget *parent = 0);
    ~Form();

    void setParentPath(QString path);
    
private slots:

    void openFile();
    void loadFile();
    void depressFile();
    void setDepressDir();

    void imgEnhance(bool);

    /**** TO paste to widget two of masterGUI program ****/
    void appendMsg(QString msg);
    void appendError(QString msg);

    /**
     * 设置主控fc对应的五组相机fc
     */
    bool setCurrentFCs(__int64 master_fc);
    
    /**
     * 五组原图显示函数
     */
    bool updateImages();

    // 转到帧的相关槽函数
    void frameSearch();
    void frameSearchEditToggled();

public slots:
    
    /**
     * 起始五帧
     */
    void viewBeginFiveImages();
    /**
     * 末尾五帧
     */
    void viewEndFiveImages();
    /**
     * 下五帧
     */
    void viewNextFiveFrameImages();
    /**
     * 上五帧
     */
    void viewLastFiveFrameImages();

private:
    Ui::Form *ui;

    // 计算时间间隔使用
    QDateTime lastTime;
    int m_time;

    /**
     * 数据目录
     */
    QString tmpopendir;
    // 流式文件绝对路径 
    QString file_full;
    // 解压目录
    QString depressdir;

    // 是否做直方图均衡化
    bool ifimgenhance;

    DataHead datahead;

    // 四组fc，实际文件号对应
    __int64 current_fc;
    __int64 & getTmpCurrentfc(int i);

    bool tmphasfc1;
    bool tmphasfc2;
    bool tmphasfc3;
    bool tmphasfc4;
    bool tmphasfc5;
    bool tmphasfc6;
    bool tmphasfc7;
    bool tmphasfc8;
    bool tmphasfc9;
    bool  & getTmphasfc(int i);

    // 第一次点击framesearchInput
    bool firsttime;

    // 
    bool viewdirection;

    // 起始终止帧
    BLOCK_KEY startfr;
    BLOCK_KEY endfr;
};

#endif // FORM_H
