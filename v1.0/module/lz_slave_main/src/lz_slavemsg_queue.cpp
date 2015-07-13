#include "lz_slavemsg_queue.h"

/**
 * 从控机消息队列类设计
 * 顺序执行接受的消息任务，防止冲突
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2014-7-31
 */
LzSlaveMsgQueue::LzSlaveMsgQueue(QObject * parent) : LzMsgQueue(parent), slaveModel(1.0)
{
    hasinitslavemodel = false;
}

/**
 * 析构函数，删除线程数组中的实例线程
 */
LzSlaveMsgQueue::~LzSlaveMsgQueue()
{
}

/**
 * 暂停
 */
void LzSlaveMsgQueue::suspend()
{
    LzMsgQueue::suspend();
}

/**
 * 初始化从机模型
 */
void LzSlaveMsgQueue::initSlaveModel(SlaveModel & model)
{
    slaveModel = model;
    hasinitslavemodel = true;
}

/**
 * 得到从机模型
 * @return SlaveModel 引用
 */
SlaveModel & LzSlaveMsgQueue::getSlaveModel()
{
    return slaveModel;
}

/**
 * 是否初始化从机模型
 * @return true 已经初始化
 */
bool LzSlaveMsgQueue::hasInitSlaveModel()
{
    return hasinitslavemodel;
}