#ifndef MANAGE_LINE_H
#define MANAGE_LINE_H

#include <QWidget>
#include <QTableView>
#include <QtSql/QSqlRelationalTableModel>
#include <QDataWidgetMapper>
#include <QSqlRecord>

#include "daoline.h"

namespace Ui {
    class ManageLineWidget;
}

/**
 * 保存类型枚举
 */
enum SaveType {
    Save_Add = 0, // 添加线路数据
    Save_Modify = 1, // 修改线路数据
    Save_Add_PropertyClass = 2 // 修改属性类，如隧道的曲线类
};

/**
 * 线路管理界面类声明
 * 继承自QWidget
 * @author fanxiang
 * @version 1.0.0
 * @date 2013-08-18
 */
class ManageLineWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ManageLineWidget(QWidget *parent = 0);
    ~ManageLineWidget();

private slots:
    /**
     * 点击linesView表格视图中的某一行后的槽函数
     * 更新显示右侧界面控件（输入框、下拉框）具体值
     */
    void showLineDetail(const QModelIndex &);

    /**
     * 更新linesView表格视图中的数据
     */
    void updateLinesView();

    /**
     * 点击新建线路按钮后
     * 调用数据库LineDAO的方法添加线路
     */
    void newLine();
    /**
     * 点击删除线路按钮后
     * 调用数据库LineDAO的方法删除线路
     */
    void deleteLine();

    /**
     * 点击saveLine按钮后的槽函数
     * 若修改调用mapper自带的数据库update方法
     * LineDAO中定义的提交方法，save
     */
    void on_saveLine_clicked();

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
     * 查找线路
     */
    void findLine();
    void showAllLines();

    /**
     * 设置隧道详细框当前匹配的内容显示出来，调用mapper的setCurrentIndex
     */
    void setCurrentMapper(int currentRow);

private:
    Ui::ManageLineWidget *ui;

    QDataWidgetMapper *mapper;
    QSqlRelationalTableModel * linesModel;
    int currentRow;

    int changed;
    SaveType saveType;
};

#endif // MANAGE_LINE_H
