#ifndef IMAGEMAINWIN_H
#define IMAGEMAINWIN_H

#include <QWidget>
#include <QTimer>
#include <QImage>
#include <QMutex>

#include "slavemodel.h"
#include "serverthread.h"
#include "multithreadtcpserver.h"
#include "status.h"


#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class QPushButton;
class QLabel;


namespace Ui {
    class Imagemainwin;
}

class Imagemainwin : public QWidget
{
    Q_OBJECT

public:
   // explicit Imagemainwin(QWidget *parent = 0);
    Imagemainwin(QWidget *p =NULL,Qt::WindowFlags f=0);
    ~Imagemainwin();
private:
    QMutex mutex;
    // 本身作为主控机的tcpserver服务，有listen方法
    MultiThreadTcpServer * multiThreadTcpServer;
signals:
    void sendparameters(QString,int,_int64,int);
public slots:
    void openFiles();
    void deleteThread();
    void showError(const QString filename);
    void addThumbnail(QImage);
    void viewframe(QString,QString);
    void askforimgae();
    void showseqno(int);





private slots:

    void createNextfiveThumbnail();//下五帧
    void createPreviewfiveThumbnail();//上五帧

private:

    void createThumbnail(const QString& filename);//显示最初的图像
    //QWidget* previewwidget;
    int waitseconds;
    static int sendseqno;


private:
    Ui::Imagemainwin *ui;
};

#endif // IMAGEMAINWIN_H
