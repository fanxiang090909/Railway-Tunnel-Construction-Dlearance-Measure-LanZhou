#ifndef CREATE_PLAN_TASK_H
#define CREATE_PLAN_TASK_H

#include <QWidget>
#include <QStringListModel>
#include <QSqlRelationalTableModel>
#include <tinyxml.h>
#include <tinystr.h>
#include <QString>
#include <QListView>
#include "setting_master.h"
#include "plantask_list.h"
#include "checkedtask_list.h"
#include "xmltaskfileloader.h"
#include "xmlcheckedtaskfileloader.h"

namespace Ui {
    class PlanTaskWidget;
}

/**
 * 隧道任务分配界面类声明
 * @author 熊雪
 * @author 范翔
 * @version 1.0.0
 * @date 2013-9-22
 */
class PlanTaskWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PlanTaskWidget(QWidget *parent = 0);
    ~PlanTaskWidget();

    /**
     * 加载左侧QListView的线路数据
     */
    void loadLinesData();
    /**
     * 加载中间QTableView的隧道数据
     */
    void loadTunnelData();
    /**
     * 加载右侧QTableWidget的隧道任务表头数据
     */
    void prepareTaskTunnelsView();


private:
    Ui::PlanTaskWidget *ui;

    int static TABLE_COLUMN_COUNT;

    /**
     *  线路listView中用的数据访问对象模型
     */
    QStringListModel * linesModel;
    /**
     *  线路tableWidget中用的数据访问对象模型
     */
    QSqlRelationalTableModel * tunnelsModel;

    /**
     * plan task xml文件读写
     */
    XMLTaskFileLoader *task;

    /**
     * checked task xml文件读写
     */
    XMLCheckedTaskFileLoader *tunnelcheck;

    int currentRow;
    QString openFileDir;

    /**
     * 更新采集日期
     */
    void updateDateTime();
private slots:

    /**
     * 点击线路的lineView更新隧道的TableView槽函数
     * @param index lineView中的一个索引
     */
    void updateTunnelsView(const QModelIndex &index);

    /* 私有槽函数 */
    /* 选择隧道添加至隧道任务中，增加，删除隧道任务 */
    /**
     * 增加计划任务隧道
     */
    void addTaskTunnel();
    /**
     * 移除计划任务隧道
     */
    void removeTaskTunnel();
    /**
     * 增加计划任务隧道
     * @param index tableWidget中的一个索引
     */
    void addTaskTunnel(const QModelIndex &);
    /**
     * 移除计划任务隧道
     * @param index tableWidget中的一个索引
     */
    void removeTaskTunnel(const QModelIndex &);
    /**
     * 增加计划任务隧道，将选中的tableView中的项全部添加到tableWidget中
     */
    void addAllSellectedTaskTunnels();
    /**
     * 移除计划任务隧道，将选中的tableWidget中的项全部移除
     */
    void removeAllSellectedTaskTunnels();
    /**
     * 清空计划任务隧道list
     */
    void removeAllTaskTunnels();

    /* 隧道任务顺序操作翻转前移后移 */
    /**
     * 计划任务的翻转，只在界面TableWidget中修改，不操作PlanTaskList
     */
    void turnTaskTunnelsOrder();
    /**
     * 计划任务的前移，只在界面TableWidget中修改，不操作PlanTaskList
     */
    void turnToPreviousTaskTunnel();
    /**
     * 计划任务的后移，只在界面TableWidget中修改，不操作PlanTaskList
     */
    void turnToNextTaskTunnel();

    /**
     * 打开浏览xml任务文件
     */
    //void openTaskFile();
    /**
     * 新建计划任务文件
     */
    void makeTaskFile();

    void on_searchButton_clicked();
    void on_showAllLinesButton_clicked();
    //void on_assginTaskButton_clicked();

    void showAllLines();
    //void openxmlFile();
signals:
    //void send_datatocheck_task(QString);


};

#endif // CREATE_PLAN_TASK_H
