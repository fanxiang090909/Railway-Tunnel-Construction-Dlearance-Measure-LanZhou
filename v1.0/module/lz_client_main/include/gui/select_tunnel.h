#ifndef SELECT_TUNNEL_H
#define SELECT_TUNNEL_H

#include <QWidget>
#include "tinystr.h"
#include "tinyxml.h"
#include "xmltaskfileloader.h"
#include <QStringListModel>

#include "setvalidframes.h"
#include "lz_logger.h"

using namespace std;

namespace Ui {
    class SelectAvaliableTunnelWidget;
}

/**
 * 可修正综合隧道选择（图形修正、隧道综合之前）界面类声明
 *
 * @author 范翔
 * @version 1.0.0
 * @date 20140222
 */
class SelectAvaliableTunnelWidget : public QWidget
{
    Q_OBJECT

public:
    // 默认链接数据库
    explicit SelectAvaliableTunnelWidget(bool initisconnectingdb = true, QWidget *parent = 0);
    ~SelectAvaliableTunnelWidget();

    // 刷新可编辑隧道

    // 设置有效帧范围的所选帧数
    int valid_row;

    /** 
     * 日志类初始化
     */
    bool initLogger(string filename, string username);
    /**
     * 日志记录
     */
    void log(QString msg);

signals:
    void sendsignal(QString,QString);
    void sendseqno(int);
    void sendstartnumber(int);
    void sendtunnelid(int);
    void signalSelectedTunnelToEdit(int, QString, bool, bool, bool, double, long long, long long);

private:
    Ui::SelectAvaliableTunnelWidget *ui;

    QStringListModel * linesModel;

    int currentRow;
    QString openFileDir;
    QString saveFileDir;

    /**
     * 记录当前选中的projectname（不含后缀名）
     */
    QString projectname;
    /**
     * 记录当前选中的tunnelname
     */
    QString tunnelname;

    
    SetValidFrames *openvalidframes;

    // 日志类
    LzLogger * logger;

    bool hasinitlog;

    enum {
        CHECKEDTASK_TUNNELID = 0,
        CHECKEDTASK_TUNNELNAME = 1,
        CHECKEDTASK_TIME = 2,
        CHECKEDTASK_SEQNO_TOTAL = 3,
        CHECKEDTASK_STARTMILE = 4,
        CHECKEDTASK_ENDMILE = 5,
        CHECKEDTASK_STARTFRAME = 6,
        CHECKEDTASK_ENDFRAME = 7,
        CHECKEDTASK_TOTALFRAME = 8,
        CHECKEDTASK_APPLYEDIT = 9,
        CHECKEDTASK_CHECKVALIDFRAMES = 10
    };

    /**
     * @author 范翔 20151103 为兼容3d查看小工具添加
     * 是否初始链接数据库
     */
    bool isconnectingdb;

public:
    //添加实际隧道文件

    void loadAvaliableLines();

    bool loadCheckedTaskTunnelData();
    bool updateCheckedTaskWidget();

private slots:
    void refreshAvalibleLinesData();

    void updateCheckedTunnelsView(const QModelIndex &);
    // 单击按钮响应槽函数
    void askforEditableTunnel();
    // 转到综合按钮槽函数
    void on_nextWidgetButton_clicked();

    // 显示clientprogram的消息
    void appendMsg(QString);

    
    void opensetvalidframes();

    void getframesfromsetvalidframes(long long  newstartframes, long long newendframes);

};

#endif // SELECT_TUNNEL_H
