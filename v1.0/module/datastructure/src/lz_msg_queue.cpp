#include "lz_msg_queue.h"

/**
 * 从控机消息队列类设计
 * 顺序执行接受的消息任务，防止冲突
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2014-7-31
 */
LzMsgQueue::LzMsgQueue(QObject * parent) : QObject(parent)
{
    todolist = new QList<ToDoMsg>();
	isinit = true;

    // 设置文件发送和接收类
    isbusymutex.lock();
    isbusy = false;
    isbusymutex.unlock();
}

/**
 * 析构函数，删除线程数组中的实例线程
 */
LzMsgQueue::~LzMsgQueue()
{
    delete todolist;
	isinit = false;
}

/**
 * qDebug打印当前待执行的命令队列
 */
void LzMsgQueue::printToDoList()
{
    qDebug() << "*****todolist****"; 
    for (int i = 0; i < todolist->size(); i++)
    {
        ToDoMsg tmp = todolist->at(i);
        qDebug() << "\t" << tmp.msg;
    }
}

const QList<ToDoMsg>* LzMsgQueue::list()
{
    return todolist;
}

/**
 * 插入到队列尾部
 * @return true 立即执行，false 正有执行其他命令，加入队列稍后执行
 */
bool LzMsgQueue::pushBack(ToDoMsg slavetaskModel)
{
	//qDebug() << "LzMsgQueue" << slavetaskModel.msg;
	// 当程序关闭时,也会调用pushBack
	if (!isinit)
		return false;

    mutex.lock();
    todolist->push_back(slavetaskModel);
    mutex.unlock();

    // TODO TODELETE
    printToDoList();

    isbusymutex.lock();
    if (isbusy == false)
    {
        isbusymutex.unlock();
        startProcessNextMsg();
        return true;
    }
    else
    {
        isbusymutex.unlock();
        return false;
    }
}

/**
 * 暂停
 */
void LzMsgQueue::suspend()
{
    todolist->clear();
}

/**
 * 队列完成后执行
 */
void LzMsgQueue::finish() { }

/**
 * 从队列首部取出，队列中删除队首元素
 * @param ret 返回的ToDoMsg
 */
bool LzMsgQueue::popfront(ToDoMsg & ret)
{
    if (todolist->size() > 0)
    {
        mutex.lock();
        ret = todolist->at(0);
        todolist->removeFirst();
        mutex.unlock();
        return true;
    }
    return false;
}

void LzMsgQueue::clear()
{
    mutex.lock();
    todolist->clear();
    mutex.unlock();
}

/**
 * 开始发送
 */
void LzMsgQueue::startProcessNextMsg()
{
    ToDoMsg toDoMsg;
    // 从队列中取出第一个元素
    if (popfront(toDoMsg))
    {
        printToDoList();

        isbusymutex.lock();
        isbusy = true;
        isbusymutex.unlock();

        bool ret = parseMsg(toDoMsg.msg);

        if (ret == false)
        {
            qDebug() << "cannot parseMsg: " << toDoMsg.msg;
            isbusymutex.lock();
            isbusy = false;
            isbusymutex.unlock();
            return;
        }
    }
    else
        finish();
}

/**
 * 执行完某条任务，检查是否有下一条任务等待执行
 */
void LzMsgQueue::endMsg()
{
    isbusymutex.lock();
    isbusy = false;
    isbusymutex.unlock();

    qDebug() << "end msg ok ";
    isbusymutex.lock();
    if (isbusy == false)
    {
        isbusymutex.unlock();
        // 发送队列中下一个文件
        startProcessNextMsg();
    }
    else
        isbusymutex.unlock();
}