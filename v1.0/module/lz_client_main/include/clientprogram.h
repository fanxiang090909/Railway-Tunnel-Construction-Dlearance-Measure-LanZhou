#ifndef CLIENTPROGRAM_H
#define CLIENTPROGRAM_H

// 莫名其妙，非要加这个
#include <QtNetWork>
#include <QTcpServer>
#include <QList>
#include <QFile>

#include "client.h"

/**
 * 办公室底层网络消息处理类声明
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2013-11-23
 */
class ClientProgram : public QObject
{
    Q_OBJECT
public:
    /**
     * singleton设计模式，公有静态方法返回单一实例
     * @return 单一实例
     */
    static ClientProgram* getClientProgramInstance();

    ~ClientProgram();

    /**********功能函数********************/
    /**
     * 申请图形修正
     * @param projectname 
     * @param tunnelid
     * @param tunnelname
     * @param date
     * @param username
     */
    void applyForModifiy(QString projectname, int tunnelid, QString tunnelname, QString date, QString username);
    /**
     * 完成图形修正
     * @param projectname 
     * @param tunnelid
     * @param tunnelname
     * @param date
     * @param username
     */
    void finishModify(QString projectname, int tunnelid, QString tunnelname, QString date, QString username);
    /**
     * 查看服务器可修正隧道目录
     */
    void askForAvalibleDirectory();
    /**
     * 已经获得服务器可修正隧道目录
     */
    void hasAvalibleDirectory();

    /**
     * 希望查看原始图像 
     * 工程projectname下文件seqno，cameraIndex从startFramecounter到(startFramecounter+frameNum)的帧图片
     * @param projectname
     * @param cameraIndex
     * @param seqno
     * @param date
     * @param startFramecounter 
     * @param frameNum 帧数
     */
    void askForRawImages(QString projectname, QString cameraIndex, int seqno, _int64 startFramecounter, int frameNum);


    /***********网络发送文件、消息服务调用接口******************/
    void send_file(QString filename);

    /**
     * 初始化【1】IP配置	
     * 创建client实例
     * @return true 初始化成功
     */
    bool initServerIP();
    /**
     * 初始化【2】网络连接
     * @return true 初始化成功
     */
    bool initConnect();
    /**
     * @return true 已初始化IP配置	
     */
    bool hasInitServerIP();
    /**
     * @return true 已初始化网络连接	
     */
    bool hasInitConnect();

private:
    explicit ClientProgram(QObject *parent = 0);

    static ClientProgram * clientProgramInstance;

    /*******当做一个客户端，发送文件及消息*****/
    Client * client;

    /**
     * 是否初始化服务器IP设置的标志
     */
    bool isinitserverip;
    /**
     * 是否连接网络的服务器标志
     */
    bool isinitconnect;
public slots:
    /**
     * 解析消息字符串
     * @param msg 自定义的消息，这里解析从从控机传到主控机的消息
     * @see 《兰州项目网络消息设计v2.0.ppt》版本v2.0
     */
    void parseMsg(QString msg);

    /******************************/

signals:
    // @author fanxiang
    void signalMsgToGUI(const QString& str);
    void signalErrorToGUI(const QString& str);
};

#endif // CLIENTPROGRAM_H
