#ifndef SELECT_TUNNEL_COPYRESULT_H
#define SELECT_TUNNEL_COPYRESULT_H

#include <QWidget>
#include "tinystr.h"
#include "tinyxml.h"
#include "xmltaskfileloader.h"
#include <QStringListModel>

using namespace std;

namespace Ui {
    class CopyAvaliableTunnelWidget;
}

/**
 * 可修正综合隧道选择（图形修正、隧道综合之前）界面类声明
 *
 * @author 范翔
 * @version 1.0.0
 * @date 20140222
 */
class CopyAvaliableTunnelWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CopyAvaliableTunnelWidget(QWidget *parent = 0);
    ~CopyAvaliableTunnelWidget();

    // 刷新可编辑隧道

    // 设置有效帧范围的所选帧数
    int valid_row;

signals:
    void sendsignal(QString,QString);
    void sendseqno(int);
    void sendstartnumber(int);
    void sendtunnelid(int);

private:
    Ui::CopyAvaliableTunnelWidget *ui;

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

    enum {
        CHECKEDTASK_TUNNELID = 0,
        CHECKEDTASK_TUNNELNAME = 1,
        CHECKEDTASK_TIME = 2,
        CHECKEDTASK_SEQNO_TOTAL = 3,
        CHECKEDTASK_START_MILE = 4,
        CHECKEDTASK_END_MILE = 5,
        CHECKEDTASK_STARTFRAME = 6,
        CHECKEDTASK_ENDFRAME = 7,
        CHECKEDTASK_TOTALFRAME = 8
    };

public:
    //添加实际隧道文件

    void loadAvaliableLines();

    bool loadCheckedTaskTunnelData();
    bool updateCheckedTaskWidget();

private slots:
    void refreshAvalibleLinesData();

    void updateCheckedTunnelsView(const QModelIndex &);

    void copyOneProject();

    void copyAllProject();
};

#endif // SELECT_TUNNEL_COPYRESULT_H
