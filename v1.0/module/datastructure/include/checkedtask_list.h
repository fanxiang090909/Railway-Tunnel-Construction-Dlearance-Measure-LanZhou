#ifndef CHECKED_TASK_LIST_H
#define CHECKED_TASK_LIST_H

#include <QObject>

#include "checkedtask.h"
#include "realtask.h"

#include <QDir>
#include <QMutex>
#include <QFileInfoList>

/**
 * 校正任务队列
 * @author xiongxue
 * @author fanxiang
 * @version 1.0.0
 * @date 2014-1-16
 */
class CheckedTaskList : public QObject
{
    Q_OBJECT
public:

    /**
     * singleton单例设计模式，公有静态方法返回单一实例
     * @return 单一实例
     */
    //static CheckedTaskList* getCheckedTaskListInstance();//公有静态方法返回单一实例
    explicit CheckedTaskList(QObject *parent = 0);
    ~CheckedTaskList();

    /**
     * 返回list
     * 常类型是指使用类型修饰符const说明的类型，常类型的变量或对象的值是不能被更新的。
     */
    const QList<CheckedTunnelTaskModel>* list();//存Tunnelcheck.xml文件的内容

    bool pushback(CheckedTunnelTaskModel& slavetunnelcheckModel);

    /**
     * 清空整个list
     */
    void clear();

    QList<CheckedTunnelTaskModel>::iterator begin();
    QList<CheckedTunnelTaskModel>::iterator end();

    /**
     * 得到CheckedTunnelTaskModel
     * @param tunnelid 输入的隧道ID
     * @param task 输出的找到值
     */
    bool getCheckedTunnelModel(int tunnelid, CheckedTunnelTaskModel & task);
    CheckedTunnelTaskModel & getCheckedTunnelModel(int tunnelid, bool & ret);

    /**
     * (在文件未被校正前的版本)得到CheckedTunnelTaskModel
     * @param tunnelid 输入的隧道ID
     * @param startseqno 输入的起始seqno采集序列号
     * @param task 输出的找到值
     */
    bool getCheckedTunnelModel(int tunnelid, int startseqno, CheckedTunnelTaskModel & task);
    CheckedTunnelTaskModel & getCheckedTunnelModel(int tunnelid, int startseqno, bool & ret);

    /**
     * 得到CheckedTunnelTaskModel
     * @param tunnelid 输入的隧道ID
     * @param masterfc 输入的起始seqno采集序列号
     * @param task 输出的找到值
     */
    bool getCheckedTunnelModel(int tunnelid, __int64 masterfc, CheckedTunnelTaskModel & task);

    /**
     * 中间插入一个校正隧道元素到校正队列
     * @param i 插入位置
     * @param t 要插入的校正隧道元素
     */
    void insertAt(int i, const CheckedTunnelTaskModel & t);

    /**
     * 从隧道校正队列中删除某一项校正隧道元素
     * @param i 删除位置
     */
    void deletelistone(int i);

    /**
     * qDebug把整个校正队列list显示到控制台
     */
    void showList();

    /**
     * 获得隧道ID
     */
    int getTunnelIDByTunnelName(string tunnelname);

    /**
     * 检查是否所有主控文件都已计算备份
     */
    bool checkAllCalcuBackup();

    /**
     * 重置计算位置
     */
    bool resetCalcuPos();

    /**
     * 重新设置车厢正反
     * @param tunnelid 隧道ID
     * @param direction true正向 false 反向
     */
    bool setTunnelCarriageDirection(int tunnelid, bool direction);

private:
    //explicit CheckedTaskList(QObject *parent = 0);

    // singleton设计模式，静态私有实例变量
    //static CheckedTaskList * correctedTaskListInstance;

    QList<CheckedTunnelTaskModel> *slavetunnelcheckList;

    QMutex mutex;
};

#endif // CHECKED_TASK_LIST_H
