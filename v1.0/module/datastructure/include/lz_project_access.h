#ifndef LZ_PROJECT_ACCESS_H
#define LZ_PROJECT_ACCESS_H

#include <QList>
#include <QMutex>
#include <QDebug>
#include <QStringList>

#include "projectmodel.h"
#include "plantask_list.h"
#include "realtask_list.h"
#include "checkedtask_list.h"

/**
 * 兰州工程类别
 * Main 主类别（用于用户交互、文件校正、客户端等操作）
 * Collect 采集类别
 * Calculate 计算类别
 * Backup 备份类别
 */
enum LzProjectClass {
    Main = 0,
    Collect = 1,
    Calculate = 2,
    Backup = 3
};

/**
 * 兰州任务队列配置文件类型
 * List_Plan 计划任务队列
 * List_Real 实际记录队列
 * List_Checked 校正任务队列
 */
enum LzListType {
    List_Plan = 0,
    List_Real = 1,
    List_Checked = 2
};

enum ConfigFileType {
    Config_Network = 0,
    Config_Calibration = 1,
    Config_FenZhong = 2,
    Config_Plan = 3,
    Config_Real = 4,
    Config_Checked = 5
};

/**
 * 兰州任务工程访问类声明
 * 包含信息：工程入口文件及模型
 *           任务列表（PlanTaskList，RealTaskList，CheckedTaskList）访问类
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2014-10-28
 */
class LzProjectAccess : public QObject
{
    Q_OBJECT

public:

    /**
     * singleton单例设计模式，公有静态方法返回单一实例
     * @return 单一实例
     */
    static LzProjectAccess* getLzProjectAccessInstance();//公有静态方法返回单一实例

    virtual ~LzProjectAccess();
    
    /**
     * 设置工程存放路径
     */
    void setProjectPath(LzProjectClass clasz, QString newpath);
    /**
     * @return 当前采集工程目录路径
     */
    QString & getProjectPath(LzProjectClass clasz);

    /**
     * 采集线路工程文件的配置加载
     */
    bool setProjectName(LzProjectClass clasz, QString filename);
    /**
     * @return 当前采集工程文件名
     */
    QString & getProjectFilename(LzProjectClass clasz);
    /**
     * @return 当前采集工程模型
     */
    ProjectModel & getProjectModel(LzProjectClass clasz);

    /**
     * qDebug显示四个projectModel
     */
    void showProjectModel();

    PlanTaskList & getLzPlanList(LzProjectClass clasz);
    RealTaskList & getLzRealList(LzProjectClass clasz);
    CheckedTaskList & getLzCheckedList(LzProjectClass clasz);

    /**
     * 得到配置文件是否存在引用
     */
    bool & getConfigBoolean(LzProjectClass clasz, ConfigFileType configtype);

private:

    bool & getInitProjectOk(LzProjectClass clasz);

    /**
     * 构造函数
     */
    explicit LzProjectAccess(QObject * parent = 0);
    
    // singleton设计模式，静态私有实例变量
    static LzProjectAccess * lzProjectAccessInstance;

    // 主程序（用户操作、交互程序）
    /**
     * 当前采集工程模型是否初始化
     */
    bool main_initprojectok;
    /**
     * 当前采集工程模型
     */
    ProjectModel main_ProjectModel;
    /**
     * 工程入口文件路径变量
     */
    QString main_projectpath;
    /**
     * 工程入口文件名变量
     */
    QString main_projectfilename;
    // 主程序（用户操作、交互程序）所用list
    PlanTaskList main_planlist;
    RealTaskList main_reallist;
    CheckedTaskList main_checkedlist;
    bool main_networkconfigok;
    bool main_calibrationok;
    bool main_fenzhongok;
    bool main_plantaskok;
    bool main_realtaskok;
    bool main_checkedtaskok;


    // 采集程序所用list
    bool collect_initprojectok;
    ProjectModel collect_ProjectModel;
    QString collect_projectpath;
    QString collect_projectfilename;
    PlanTaskList collect_planlist;
    RealTaskList collect_reallist;
    CheckedTaskList collect_checkedlist;
    // 可操作状态标志，用于功能初始化状态检查
    bool collect_networkconfigok;
    bool collect_calibrationok;
    bool collect_fenzhongok;
    bool collect_plantaskok;
    bool collect_realtaskok;
    bool collect_checkedtaskok;


    // 计算程序所用list
    bool calcu_initprojectok;
    ProjectModel calcu_ProjectModel;
    QString calcu_projectpath;
    QString calcu_projectfilename;
    PlanTaskList calcu_planlist;
    RealTaskList calcu_reallist;
    CheckedTaskList calcu_checkedlist;
    // 可操作状态标志，用于功能初始化状态检查
    bool calcu_networkconfigok;
    bool calcu_calibrationok;
    bool calcu_fenzhongok;
    bool calcu_plantaskok;
    bool calcu_realtaskok;
    bool calcu_checkedtaskok;

    // 备份程序所用list
    bool backup_initprojectok;
    ProjectModel backup_ProjectModel;
    QString backup_projectpath;
    QString backup_projectfilename;
    PlanTaskList backup_planlist;
    RealTaskList backup_reallist;
    CheckedTaskList backup_checkedlist;
    // 可操作状态标志，用于功能初始化状态检查
    bool backup_networkconfigok;
    bool backup_calibrationok;
    bool backup_fenzhongok;
    bool backup_plantaskok;
    bool backup_realtaskok;
    bool backup_checkedtaskok;

};

#endif // LZ_PROJECT_ACCESS_H
