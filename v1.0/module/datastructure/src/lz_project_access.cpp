#include "lz_project_access.h"

#include "xmlrealtaskfileloader.h"
#include "xmlprojectfileloader.h"
#include "xmlnetworkfileloader.h"

/**
 * 兰州任务工程访问类声明
 * 包含信息：工程入口文件及模型
 *           任务列表（PlanTaskList，RealTaskList，CheckedTaskList）访问类
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2014-10-28
 */

// singleton设计模式，静态私有实例变量
LzProjectAccess * LzProjectAccess::lzProjectAccessInstance = NULL;

/**
 * singleton设计模式，公有静态方法返回单一实例
 * @return 单一实例
 */
LzProjectAccess* LzProjectAccess::getLzProjectAccessInstance()
{
    if (lzProjectAccessInstance == NULL)
    {
        lzProjectAccessInstance = new LzProjectAccess();
    }
    return lzProjectAccessInstance;
}

// singleton单一设计模式，构造函数私有
LzProjectAccess::LzProjectAccess(QObject *parent) :
    QObject(parent), main_ProjectModel(1.0), collect_ProjectModel(1.0), 
                    calcu_ProjectModel(1.0), backup_ProjectModel(1.0)
{
    main_networkconfigok = false;
    main_fenzhongok = true;             // @author 范翔 每组相机倒到分中坐标系的配置文件与双目标定配置文件二合一
    main_calibrationok = false;
    main_plantaskok = false;
    main_realtaskok = false;
    main_checkedtaskok = false;

    // 文件状态标志
    collect_networkconfigok = false;
    collect_fenzhongok = true;             // @author 范翔 每组相机倒到分中坐标系的配置文件与双目标定配置文件二合一
    collect_calibrationok = false;
    collect_plantaskok = false;
    collect_realtaskok = false;
    collect_checkedtaskok = false;

    // 文件状态标志
    calcu_networkconfigok = false;
    calcu_fenzhongok = true;             // @author 范翔 每组相机倒到分中坐标系的配置文件与双目标定配置文件二合一
    calcu_calibrationok = false;
    calcu_plantaskok = false;
    calcu_realtaskok = false;
    calcu_checkedtaskok = false;

    // 文件状态标志
    backup_networkconfigok = false;
    backup_fenzhongok = true;             // @author 范翔 每组相机倒到分中坐标系的配置文件与双目标定配置文件二合一
    backup_calibrationok = false;
    backup_plantaskok = false;
    backup_realtaskok = false;
    backup_checkedtaskok = false;
}

LzProjectAccess::~LzProjectAccess()
{
    
}

PlanTaskList & LzProjectAccess::getLzPlanList(LzProjectClass clasz)
{
    switch (clasz)
    {
        case LzProjectClass::Main :        return main_planlist;
        case LzProjectClass::Collect :     return collect_planlist;
        case LzProjectClass::Calculate :   return calcu_planlist;
        case LzProjectClass::Backup :      return backup_planlist;
        default:                           return main_planlist;
    }
}

RealTaskList & LzProjectAccess::getLzRealList(LzProjectClass clasz)
{
    switch (clasz)
    {
        case LzProjectClass::Main :        return main_reallist;
        case LzProjectClass::Collect :     return collect_reallist;
        case LzProjectClass::Calculate :   return calcu_reallist;
        case LzProjectClass::Backup :      return backup_reallist;
        default:                           return main_reallist;
    }
}

CheckedTaskList & LzProjectAccess::getLzCheckedList(LzProjectClass clasz)
{
    switch (clasz)
    {
        case LzProjectClass::Main :        return main_checkedlist;
        case LzProjectClass::Collect :     return collect_checkedlist;
        case LzProjectClass::Calculate :   return calcu_checkedlist;
        case LzProjectClass::Backup :      return backup_checkedlist;
        default:                           return main_checkedlist;
    }
}

/**
 * 得到配置文件是否存在引用
 */
bool & LzProjectAccess::getConfigBoolean(LzProjectClass clasz, ConfigFileType configtype)
{
    switch (clasz)
    {
        case LzProjectClass::Main :       
        {
            switch (configtype)
            {
                case Config_Network:       return main_networkconfigok;
                case Config_Calibration:   return main_calibrationok;
                case Config_FenZhong:      return main_fenzhongok;
                case Config_Plan:          return main_plantaskok;
                case Config_Real:          return main_realtaskok;
                case Config_Checked:       return main_checkedtaskok;
                default:                   return main_initprojectok;
            }
        }
        case LzProjectClass::Collect :
        {
            switch (configtype)
            {
                case Config_Network:       return collect_networkconfigok;
                case Config_Calibration:   return collect_calibrationok;
                case Config_FenZhong:      return collect_fenzhongok;
                case Config_Plan:          return collect_plantaskok;
                case Config_Real:          return collect_realtaskok;
                case Config_Checked:       return collect_checkedtaskok;
                default:                   return main_initprojectok;
            }
        }
        case LzProjectClass::Calculate :
        {
            switch (configtype)
            {
                case Config_Network:       return calcu_networkconfigok;
                case Config_Calibration:   return calcu_calibrationok;
                case Config_FenZhong:      return calcu_fenzhongok;
                case Config_Plan:          return calcu_plantaskok;
                case Config_Real:          return calcu_realtaskok;
                case Config_Checked:       return calcu_checkedtaskok;
                default:                   return main_initprojectok;
            }
        }
        case LzProjectClass::Backup :
        {
            switch (configtype)
            {
                case Config_Network:       return backup_networkconfigok;
                case Config_Calibration:   return backup_calibrationok;
                case Config_FenZhong:      return backup_fenzhongok;
                case Config_Plan:          return backup_plantaskok;
                case Config_Real:          return backup_realtaskok;
                case Config_Checked:       return backup_checkedtaskok;
                default:                   return main_initprojectok;
            }
        }
        default:                           return main_initprojectok;
    }
}

bool & LzProjectAccess::getInitProjectOk(LzProjectClass clasz)
{
    switch (clasz)
    {
        case LzProjectClass::Main :        return main_initprojectok;
        case LzProjectClass::Collect :     return collect_initprojectok;
        case LzProjectClass::Calculate :   return calcu_initprojectok;
        case LzProjectClass::Backup :      return backup_initprojectok;
        default:                           return main_initprojectok;
    }
}

/**
 * 采集线路工程文件的配置加载
 */
bool LzProjectAccess::setProjectName(LzProjectClass clasz, QString newfilename)
{
    bool & initprojectok = getInitProjectOk(clasz);
    ProjectModel & currentProjectModel = getProjectModel(clasz);
    QString & projectfilename = getProjectFilename(clasz);
    QString & projectpath = getProjectPath(clasz);

    if (!newfilename.endsWith(".proj"))
    {
        initprojectok = false;
        return false;
    }
    XMLProjectFileLoader * profileLoder = new XMLProjectFileLoader(newfilename);
    initprojectok = profileLoder->loadFile(currentProjectModel);
    qDebug() << "project loader:" << initprojectok << ", projectfilename:" << newfilename;
    delete profileLoder;

    if (initprojectok == false)
    {
        projectfilename = "";
        return false;
    }

    // 保存文件名
    projectfilename = QFileInfo(newfilename).fileName();
    projectpath = QFileInfo(newfilename).path();

    qDebug() << projectpath << currentProjectModel.getPlanFilename() << currentProjectModel.getRealCollectDate() << currentProjectModel.getCheckedFilename();
    
    // TODELETE
    //showProjectModel();
    
    return true;
}

QString & LzProjectAccess::getProjectFilename(LzProjectClass clasz)
{
    switch (clasz)
    {
        case LzProjectClass::Main :        return main_projectfilename;
        case LzProjectClass::Collect :     return collect_projectfilename;
        case LzProjectClass::Calculate :   return calcu_projectfilename;
        case LzProjectClass::Backup :      return backup_projectfilename;
        default:                           return main_projectfilename;
    }
}

ProjectModel & LzProjectAccess::getProjectModel(LzProjectClass clasz)
{
    switch (clasz)
    {
        case LzProjectClass::Main :        return main_ProjectModel;
        case LzProjectClass::Collect :     return collect_ProjectModel;
        case LzProjectClass::Calculate :   return calcu_ProjectModel;
        case LzProjectClass::Backup :      return backup_ProjectModel;
        default:                           return main_ProjectModel;
    }
}

/**
 * qDebug显示四个projectModel
 */
void LzProjectAccess::showProjectModel()
{
    QString path;
    QString filename;
    ProjectModel project(1.0);
    LzProjectClass tmp;
    for (int i = 0; i < 4; i++)
    {
        switch (i)
        {
        case 0: tmp = LzProjectClass::Main; qDebug() << "***main project"; break;
        case 1: tmp = LzProjectClass::Collect; qDebug() << "***collect project"; break;
        case 2: tmp = LzProjectClass::Calculate; qDebug() << "***calcu project"; break;
        case 3: tmp = LzProjectClass::Backup; qDebug() << "***backup project"; break;
        }
        path = getProjectPath(tmp);
        project = getProjectModel(tmp);
        filename = getProjectFilename(tmp);
        qDebug() << "\tpath:" << path << ", filename:" << filename << ", project:" << project.getPlanFilename() << project.getRealFilename() << project.getCheckedFilename();
    }
}

/**
 * 设置工程存放路径
 */
void LzProjectAccess::setProjectPath(LzProjectClass clasz, QString newpath) 
{
    QString & currentprojpath = getProjectPath(clasz);
    currentprojpath = newpath;
}

QString & LzProjectAccess::getProjectPath(LzProjectClass clasz) 
{
    switch (clasz)
    {
        case LzProjectClass::Main :        return main_projectpath;
        case LzProjectClass::Collect :     return collect_projectpath;
        case LzProjectClass::Calculate :   return calcu_projectpath;
        case LzProjectClass::Backup :      return backup_projectpath;
        default:                           return main_projectpath;
    }
}
