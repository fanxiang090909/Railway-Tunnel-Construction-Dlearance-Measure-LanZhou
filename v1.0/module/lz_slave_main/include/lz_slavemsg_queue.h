#ifndef LZ_SLAVEMSG_QUEUE_H
#define LZ_SLAVEMSG_QUEUE_H

#include "slavemodel.h"
#include "lz_msg_queue.h"
#include "lz_project_access.h"

/**
 * 从控机消息队列类设计
 * 顺序执行接受的消息任务，防止冲突
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2014-7-31
 */
class LzSlaveMsgQueue : public LzMsgQueue
{
    Q_OBJECT

public:
    /**
     * 构造函数
     */
    LzSlaveMsgQueue(QObject * parent = 0);

    virtual ~LzSlaveMsgQueue();
    
    /**
     * 暂停
     */
    virtual void suspend();

    /**
     * 初始化从机模型
     */
    void initSlaveModel(SlaveModel & model);
    /**
     * 得到从机模型
     * @return SlaveModel 引用
     */
    SlaveModel & getSlaveModel();
    /**
     * 是否初始化从机模型
     * @return true 已经初始化
     */
    bool hasInitSlaveModel();

protected:

    SlaveModel slaveModel;

private:

    bool hasinitslavemodel;

signals:
    /**
     * 已解析好的的消息发送给slave，进而供界面显示
     * @see LzMsgQueue的signalParsedMsgToSlave信号
     */
    //void signalParsedMsgToSlave(QString parsedMsg);

    /**
     * 要转发给Master的消息，经上层转发
     */
    void signalMsgToMaster(QString msgToMaster);

    /**
     * 要转发给Master的文件，经上层转发
     */
    void signalFileToMaster(QString fileToMaster);
};

#endif // LZ_SLAVEMSG_QUEUE_H
