#ifndef EDITING_LIST_H
#define EDITING_LIST_H

#include <QObject>

/**
 * 正在修正项
 */
struct EditingItem
{
    // 用户名
    QString userid;
    // 隧道ID
    int tunnelid;
    // 隧道名称
    QString tunnelname;
    // 采集工程名
    QString projectname;
    // 采集日期
    QString collectdate;
    // 开始修正时间
    QString begintime;
    // 终止修正时间
    QString endtime;
};

/**
 * 正在修正隧道list类声明
 * @author fanxiang
 * @date 2014-3-27
 * @version 1.0.0
 */
class EditingList : public QObject
{
public:
    /**
     * singleton单例设计模式，公有静态方法返回单一实例
     * @return 单一实例
     */
    static EditingList * getEditingListInstance();

    ~EditingList();

    /**
     * 返回list
     */
    const QList<EditingItem>* list();
    /**
     * 插入到队列尾部
     */
    bool pushBack(EditingItem item);

    /**
     * 删除某一item
     * @param deleteitem 待删除的项，只要projectname和tunnelname对应上，就算是
     * @return true 删除成功
     */
    bool remove(EditingItem deleteitem);

    /**
     * 清空list
     */
    void clear();

    QList<EditingItem>::iterator begin();
    QList<EditingItem>::iterator end();

    /**
     * 检查list中是否存在相同的条目
     * @param newitem 待检查的EditingItem
     * @param itemUser 输出正在修正的用户
     * @param startTime 输出正在修正的隧道的开始申请修正时间
     * @return true 存在
     */
    bool exist(EditingItem newitem, QString & itemUser, QString & startTime);

private:
    explicit EditingList(QObject *parent = 0);
    // singleton设计模式，静态私有实例变量
    static EditingList * editlistInstance;

    QList<EditingItem> *editingList;
};

#endif // EDITING_LIST_H
