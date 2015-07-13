#ifndef MANAGE_TUNNEL_H
#define MANAGE_TUNNEL_H

#include "daotunnel.h"
#include "daocurve.h"
#include "manage_line.h"

#include <QWidget>
#include <QTableView>
#include <QtSql/QSqlRelationalTableModel>
#include <QDataWidgetMapper>
#include <QMessageBox>

namespace Ui {
    class ManageTunnelWidget;
}

/**
 * 隧道管理界面类声明
 * 继承自QWidget
 * @author fanxiang
 * @version 1.0.0
 * @date 2013-08-18
 */
class ManageTunnelWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ManageTunnelWidget(QWidget *parent = 0);
    ~ManageTunnelWidget();

    void loadLinesData();
    void loadTunnelData();

private:
    Ui::ManageTunnelWidget *ui;

    QStringListModel * linesModel;
    QSqlRelationalTableModel * tunnelsModel;
    QSqlRelationalTableModel * curvesModel;

    QDataWidgetMapper *mapper;

    SaveType saveType;
    int currentRow;
    bool changed;

    int currentlineid;

private slots:
    //void updateClasses(const QString &majorname);
    
    /**
     * 刷新线路及隧道列表
     */
    void refreshLinesView();

    /** 
     * 更新隧道视图
     */
    void updateTunnelsView(const QModelIndex &index);
    void updateCurvesView(const QModelIndex &index);

    void showTunnelDetail(const QModelIndex &);

    /**
     * 点击新建隧道按钮后
     * 调用数据库TunnelDAO的方法添加隧道
     */
    void newTunnel();
    /**
     * 点击删除隧道按钮后
     * 调用数据库TunnelDAO的方法删除隧道
     */
    void deleteTunnel();
    /**
     * 点击新建曲线按钮后
     * 调用数据库CurveDAO的方法添加隧道对应的曲线
     */
    void newCurve();
    /**
     * 点击删除曲线按钮后
     * 调用数据库CurveDAO的方法删除隧道对应的曲线
     */
    void deleteCurve();
    
    /**
     * 保存曲线数据
     */
    void saveCurvesData();

    /**
     * 监听曲线表格更改信号
     */
    void changeCurveData();

    /* 查找框输入 */
    void findTunnel();
    void findLine();
    void showAllLines();

    /**
     * 编辑桥梁限界槽
     */
    void editBridgeClearance();

    /**
     * 点击saveTunnel按钮后的槽函数
     * 若修改调用mapper自带的数据库update方法
     * TunnelDAO中定义的提交方法，save
     */
    void on_saveButton_clicked();

    /**
     * 设置界面控件（输入框、下拉框）为可编辑、可选择状态
     * 右侧的详细信息
     */
    void canModify();
    /**
     * 设置界面控件（输入框、下拉框）为不可编辑、不可选择状态
     * 右侧的详细信息
     */
    void cannotModify();
    /**
     * 设置保存按钮控件可否点击
     */
    void canSave();

    /**
     * 用于mapper的四个按钮，调用mapper的toFirst、toNext、toLast、toPrevious方法
     */
    void toFirst();
    void toNext();
    void toLast();
    void toPrevious();

    /**
     * 设置隧道详细框当前匹配的内容显示出来，调用mapper的setCurrentIndex
     */
    void setCurrentMapper(int currentRow);

signals:
    /**
     * 发送界面转换到编辑桥梁限界界面信号
     */
    void signalBridgeToEdit(int tunnelid, QString tunnelname);
};

#endif // MANAGE_TUNNEL_H
