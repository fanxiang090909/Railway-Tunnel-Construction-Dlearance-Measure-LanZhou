#ifndef INPUT_BRIDGE_CLEARANCE_H
#define INPUT_BRIDGE_CLEARANCE_H

#include <QWidget>
#include <QTableWidgetItem>
#include "clearance_item.h"
#include "daoclearanceoutput.h"
#include "daotasktunnel.h"

namespace Ui {
    class InputBridgeWidget;
}

/**
 * 桥梁限界输入界面类声明
 *
 * @author 熊雪
 * @author 范翔
 * @version 1.0.0
 * @date 20140615
 */
class InputBridgeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit InputBridgeWidget(QWidget *parent = 0);
    ~InputBridgeWidget();

private:
    Ui::InputBridgeWidget *ui;

    /**
     * 判断是否进行了编辑
     */
    bool hasedit;
    bool maketable;

    /**
     * 某一个隧道的历史所有隧道任务模型，排序
     */
    QSqlQueryModel * bridgeCollectModel;

    /**
     * 高度值的总数，也是限界表的行数
     */
    int heightlength;

    /**
     * 对应的桥梁（隧道）ID
     */
    int tunnelid;

    /**
     * 采集桥梁（隧道）ID
     */
    _int64 tasktunnelid;

    /**
     * 桥梁限界数据
     */
    ClearanceData datas;
    ClearanceData datal;
    ClearanceData datar;
    int hass, hasl, hasr;

    /**
     * 加载桥梁限界数据
     */
    void loadBridgeClearanceData();
    
    /**
     * 初始化，清零
     */
    void initBridgeClearanceData();

    /**
     * 更新桥梁限界数据（只更新内部数据，不修改表头）
     */
    void updateBridgeClearanceData();

public slots:
    /**
     * 界面转换公有槽函数，接收ManageTunnelWidget发出的编辑桥梁限界的信号
     * @param tunnelid 隧道ID
     */
    void slotBridgeToEdit(int newtunnelid, QString newtunnelname);

private slots:
    /**
     * 完成输入槽函数
     * (1) 保存桥梁限界值
     * (2) 发送转换界面信号
     */
    void finishEdit();

    /**
     * 修改限界值的槽函数
     */
    void saveItemChanged(QTableWidgetItem* item);
    void minheightChanged(QString);
    void canBeModified();
    void canSave(bool can);
    void saveChanges();

signals:
    /**
     * 界面转换信号，返回到ManageTunnelWidget界面
     */
    void signalEndEdit();
};

#endif // INPUT_BRIDGE_CLEARANCE_H
