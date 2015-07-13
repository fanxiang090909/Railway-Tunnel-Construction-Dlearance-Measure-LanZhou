#ifndef LZ_MSG_QUEUE_H
#define LZ_MSG_QUEUE_H

#include <QList>
#include <QMutex>
#include <QDebug>
#include <QStringList>

#include "slavemodel.h"

/**
 * 待执行消息
 */
struct ToDoMsg
{
    QString msg; // 未执行的消息
};

/**
 * 从控机消息队列类设计
 * 顺序执行接受的消息任务，防止冲突
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2014-7-31
 */
class LzMsgQueue : public QObject
{
    Q_OBJECT

public:
    /**
     * 构造函数
     */
    LzMsgQueue(QObject * parent = 0);

    virtual ~LzMsgQueue();

    /**
     * qDebug打印当前待发送的文件队列
     */
    void printToDoList();

    /**
     * 返回list
     */
    const QList<ToDoMsg>* list();

    /**
     * 插入到队列尾部
     * @return true 立即执行，false 正有其他任务正在执行，加入队列稍后执行
     */
    bool pushBack(ToDoMsg item);

    /**
     * 暂停
     */
    virtual void suspend();

protected:

    /**
     * 队列完成后执行
     */
    virtual void finish();

private:

    /**
     * 待发送的文件记录
     */
    QList<ToDoMsg>* todolist;
	bool isinit;

    // 是否正在发送
    QMutex isbusymutex;
    bool isbusy;
    
    // 队列操作
    QMutex mutex;

    /**
     * 从队列首部取出，队列中删除队首元素
     * @param ret 返回的ToDoMsg
     */
    bool popfront(ToDoMsg & ret);

    /**
     * 清空list
     */
    void clear();
        
    /**
     * 开始执行下一条
     */
    virtual void startProcessNextMsg();

    /**
     * 任务命令解析
     * 纯虚函数
     */
    virtual bool parseMsg(QString newmsg) = 0;

protected slots:
    
    /**
     * 执行完某条任务，检查是否有下一条任务等待执行
     */
    virtual void endMsg();
    
signals:

    /**
     * 已解析好的的消息发送给slave，进而供界面显示
     */
    void signalParsedMsgToSlave(QString parsedMsg);
};

#endif // LZ_MSG_QUEUE_H
