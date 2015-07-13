#ifndef CREATE_PROJECT_WIDGET_H
#define CREATE_PROJECT_WIDGET_H

#include <QWidget>
#include <QTextBrowser>

#include "lz_working_enum.h"
#include "lz_project_access.h"

namespace Ui {
    class CreateProjectWidget;
}

/**
 * 主控机创建当日采集工程界面类声明
 * @author 范翔
 * @version 1.0.0
 * @date 2014-05-07
 */
class CreateProjectWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * 界面构造函数
     * @param curwidgettype WorkingStatus类型的变量，是否要选择即将操作的状态，若是（采集、计算&备份）若否，不显示选择控件
     */
    explicit CreateProjectWidget(WorkingStatus curwidgettype, QWidget *parent = 0);

    ~CreateProjectWidget();

    /**
     * 设置为计算备份界面，不显示创建工程按钮
     */
    void setToCalculateStyle();

signals:
    // @see MasterSetting类的getProjectFilename
    void sendproject_file_name(QString);

public slots:
    void setWidgetEnabled(bool ifenabled);

private slots:
    /**
     * 界面消息提示，接受MasterProgram的signalToGUI信号
     */
    void appendMsg(WorkingStatus, QString);

    /**
     * 导入工程文件
     */
    void importProj();
    
    /**
     * 创建新的工程文件及对应目录
     */
    void createProj();

    /**
     * 配置当前线路操作@author 范翔（@see MainWidget::configCurentOperation()）
     */
    bool configCurrentOperation();

    /**
     * 线路配置文件同步
     */
    void fileSynchronization();

    /**
     * 操作cambobox切换槽函数
     * 因为删除选择时，不用同步
     */
    void operationChanged(int);

private:
    Ui::CreateProjectWidget *ui;
	
    // 当前用户名（创建工程时记录）
    QString currentusername;
    // 即将开始的配置线路
    WorkingStatus workingstatus;

    /**
     * 当前操作类
     */
    LzProjectClass projectclass;

};

#endif // CREATE_PROJECT_WIDGET_H
