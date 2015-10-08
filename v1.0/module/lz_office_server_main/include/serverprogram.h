#ifndef SERVERPROGRAM_H
#define SERVERPROGRAM_H

#include "serverthread_office.h"
#include "multithreadtcpserver_office.h"
#include "setting_server.h"
#include "editing_list.h"

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

/**
 * 办公室服务器程序驱动类声明
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2014-2-11
 */
class ServerProgram : public QObject
{
    Q_OBJECT
private:

    // singleton设计模式，静态私有实例变量
    static ServerProgram * serverProgramInstance;

    // 本身作为服务器机的tcpserver服务，有listen方法
    OfficeMultiThreadTcpServer * multiThreadTcpServer;

    // 接受到从控机tcp连接的socket列表
    QList<QTcpSocket *> tcpReceivedSocketList;

    // 凡是操作Status的列表，都要加锁解锁
    QMutex mutex;

 public:

    /**
     * singleton设计模式，公有静态方法返回单一实例
     * @return 单一实例
     */
    static ServerProgram* getServerProgramInstance();

    ~ServerProgram();

    /**
     * 返回多线程服务器类指针
     */
    OfficeMultiThreadTcpServer * getMultiThreadTcpServer();

    /*****************关于初始化*********************/
    void init();

    /*****************图形修正*********************/
    /**
     * 收到客户端发来的申请
     * @param toip 客户端IP地址
     * @param item 一个正在修正项的信息，@see EditingItem 包括工程名“线路_日期”、 隧道ID、 隧道名、 开始编辑日期
     */
    void checkIfModiiedByOther(QString toip, EditingItem item);

    /**
     * 服务器保存隧道图形修正完成反馈给客户端
     * @param toip 客户端IP地址
     * @param projectname 工程名“线路_日期”
     * @param tunnelid 隧道ID
     * @param tunnelname 隧道名
     * @param date 隧道采集日期
     */
    void receiveFileFeedback(QString toip, QString projectname, int tunnelid, QString tunnelname, QString date);

    void returnDirectoryFiles(QString toip, QString filename);

    void sendCalcuResultData(QString toip, QString filename);

    void sendRawImagesPackage(QString toip, QString filename);

    /*****************关于计算*********************/

    /**
     * 重启slave程序
     */
    void resetSlaveProgram();
    void terminateSlaveProgram();

private:
    explicit ServerProgram(QObject *parent = 0);

signals:

    // 给上层界面的控制台输出提供消息
    void signalMsgToGUI(const QString &msg);
    void signalErrorToGUI(const QString &msg);

    /**
     * 更新当前连接IP列表
     * @param ip ip地址
     * @param addorremove 增加到iplist还是从iplist中删除，true为增加，false为删除
     */
    void updateIPListSignal(QString ip, bool addorremove);

    /**
     * 更新正在编辑工程列表
     */
    void updateEditingProjectListSignal();

private slots:

    /**
     * !!现在这个函数多余了2013-09-24fan
     * ||如果需要应该在该类的init函数中去掉注释的connect两句话
     * 检测到server的msg，向上层抛出，这里不做处理
     */
    void displayMsg(QString msg);

    /**
     * 解析消息字符串
     * @param msg 自定义的消息，这里解析从从控机传到服务器机的消息
     * @see 《服务器从控程序流程及消息设计.ppt》版本v1.0
     */
    void parseMsg(QString msg);

};

#endif // SERVERPROGRAM_H
