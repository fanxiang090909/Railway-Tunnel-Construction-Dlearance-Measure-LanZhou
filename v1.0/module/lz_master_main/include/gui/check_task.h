#ifndef CHECK_TASK_H
#define CHECK_TASK_H

#include <QWidget>
#include <QStandardItemModel>
#include <string>

#include "xmltaskfileloader.h"
#include "xmlrealtaskfileloader.h"
#include "xmlcheckedtaskfileloader.h"
#include "realtask.h"
#include "realtask_list.h"
#include "checkedtask.h"
#include "checkedtask_list.h"
#include "status.h"
#include "imageviewer_master.h"
#include <QString>
#include "setvalidframes.h"
#include "projectmodel.h"

#include "lz_logger.h"

using namespace std;

namespace Ui {
    class CheckTaskWidget;
}

/**
 * 主控机界面-计划任务与实际采集任务校对界面类声明
 * @author 熊雪
 * @author 范翔
 * @date 2014.1
 * @version 1.0.0
 */
class CheckTaskWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CheckTaskWidget(QWidget *parent = 0);
    
    ~CheckTaskWidget();

private:

    /**
     * 配置后面操作时QFileDialog（文件选择对话框）的打开路径
     */
    void loadConfigOpenCloseDir();

    /** 
     * 加载计划隧道文件，并显示
     */
    void loadPlanTaskTunnelData();

    /** 
     * 更新界面上的计划隧道列表
     */
    void updatePlanTaskWidget();
    
    /**
     * 添加实际采集“隧道”记录文件，并显示
     */
    void loadCheckedTaskTunnelData();
    
    /** 
     * 更新界面上的实际采集“隧道”列表
     */
    void updateCheckedTaskWidget();

    /**
     * 设置当前文件名
     * @param 工程入口文件名
     * @return 加载并解析成功，存入currentProjectModel中
     */
    bool setCurrentName(QString filename);

    /** 
     * 日志类初始化
     */
    bool initLogger(string filename, string username);
    /**
     * 日志记录
     */
    void log(QString msg);

private:
    Ui::CheckTaskWidget *ui;

    enum {
        PLANTASK_TUNNELID = 0,
        PLANTASK_TUNNELNAME = 1,
        PLANTASK_DATE = 2,
        PLANTASK_TUNNELLENGTH = 3,
        PLANTASK_STARTMILEAGE = 4,
        PLANTASK_ENDMILEAGE = 5
    };

    enum {
        CHECKEDTASK_TUNNELID = 0,
        CHECKEDTASK_TUNNELNAME = 1,
        CHECKEDTASK_TIME = 2,
        CHECKEDTASK_SEQNO_TOTAL = 3,
        CHECKEDTASK_START_MILE = 4,
        CHECKEDTASK_END_MILE = 5,
        CHECKEDTASK_STARTFRAME = 6,
        CHECKEDTASK_ENDFRAME = 7,
        CHECKEDTASK_TOTALFRAME = 8,
        CHECKEDTASK_CHECKIMAGE = 9,
        CHECKEDTASK_CHECKVALIDFRAMES = 10
    };

    int currentRow;
    QString openFileDir;
    QString saveFileDir;
    //Imagemainwin *imageopen;
    MasterImageViewer *finalimage;
    SetValidFrames *openvalidframes;

    // 标记实际采集显示列表是由real文件获得还是由checked文件获得
    bool ischeckedfile;

    // 设置有效帧范围的所选帧数
    int valid_row;

    QStandardItemModel *plantask_model;

    /**
     * 当前采集工程及文件名配置
     */
    QString path;

    /**
     * 当前工程入口文件设置时加载的当前操作工程模型
     */
    ProjectModel currentProjectModel;
    /**
     * 当前采集工程路径
     */
    QString projectPath;

    // 日志类
    LzLogger * logger;

    bool hasinitlog;

public slots:
    
    void getframesfromsetvalidframes(long long newstartframes, long long newendframes);
    
    void getframesfromsetvalidframes_createtmppro(long long newstartframes, long long newendframes);
    
    void openPlanTaskFile();

    /* 对实际隧道任务的修正操作，前移后移，删除 */
    /**
     * 校正-删除实际“隧道”，对CheckedTaskList直接操作(@see CheckedTaskList)
     * Step1 从currentRow往后的所有表项的planTask往依次向前移
     * Step2 删除节点
     * 然后存入CheckedTaskList对应的XML中
     */
    void removeTaskTunnel();
    /**
     * 校正-合并两个以上实际“隧道”，对CheckedTaskList直接操作(@see CheckedTaskList)
     * Step1 往topRow为编号的CheckedTaskList的元素后方插入topRow+1到bottomRow的所有
     * Step2 bottomRow+1至结束的所有表项的planTask往后错(count-1)个，即list中序号为topRow+1往后的所有元素
     * Step3 删除多余
     * 然后存入CheckedTaskList对应的XML中
     */
    void combineTaskTunnel();
    /**
     * 校正-分离一个实际“隧道”成为两个实际“隧道”，对CheckedTaskList直接操作(@see CheckedTaskList)
     * Step1 往currentRow为编号的CheckedTaskList的元素后中是否包含num帧号
     * Step2 从currentRow+1为编号planTask名称向后移动
     * 然后存入CheckedTaskList对应的XML中
     */
    void separateTaskTunnel();

    /**
     * 在ischeckedfile == false时首先生成checkedfile
     */
    void resetCheckedFile();

    /**
     * 设置分离删除合并按钮是否可点击
     */
    void setOptButtonEnable();

    void openimageviewer();
    void opensetvalidframes();

    // 文件察看
    //void on_pushButton_clicked();
};

#endif // CHECK_TASK_H
