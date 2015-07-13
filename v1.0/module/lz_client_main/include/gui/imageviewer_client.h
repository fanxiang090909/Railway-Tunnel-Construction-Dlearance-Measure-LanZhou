#ifndef CLIENT_IMAGEVIEWER_H
#define CLIENT_IMAGEVIEWER_H

#include "imageviewer.h"

namespace Ui {
    class ClientImageViewer;
}

/**
 * 采集文件信息校对界面——原图浏览界面类声明
 * @author 熊雪
 * @author 范翔
 * @version 1.0.0
 * @date 2014-08-19
 */
class ClientImageViewer : public ImageViewer
{
    Q_OBJECT

public:
    explicit ClientImageViewer(QWidget *parent = 0);
    ~ClientImageViewer();

protected:

    virtual bool askforimgae(__int64 startnum, int numofasked);

};

#endif // CLIENT_IMAGEVIEWER_H
