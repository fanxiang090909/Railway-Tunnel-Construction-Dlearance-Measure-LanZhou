#include "projectmodel.h"

/**
 * 工程模型数据结构类实现
 * @author 范翔
 * @version 1.0.0
 * @date 2014-3-27
 */
ProjectModel::ProjectModel(float initversion)
{
    version = initversion;
    if (version == 1.0)
    {
        plantaskfile = "";
        realtaskfile = "";
        checkedtaskfile = "";

        pulzerecorderfile = "";

        createdate = "";
        realcollectdate = "";
        lastcheckdate = "";
        endcalculatedate = "";
        lasteditdate = "";

        createuser = "";
        lastcheckuser = "";
    }
}

ProjectModel::ProjectModel(const ProjectModel & real)
{
    this->version = real.getVersion();
    this->plantaskfile = real.getPlanFilename();
    this->realtaskfile = real.getRealFilename();
    this->checkedtaskfile = real.getCheckedFilename();
    this->pulzerecorderfile = real.getPulzeRecorderFilename();
    this->createdate = real.getEndCalculateDate();
    this->realcollectdate = real.getRealCollectDate();
    this->lastcheckdate = real.getLastCheckDate();
    this->endcalculatedate = real.getEndCalculateDate();
    this->lasteditdate = real.getLastEditDate();
    this->createuser = real.getCreateUser();
    this->lastcheckuser = real.getLastCheckUser();
}

// 重载赋值号
ProjectModel & ProjectModel::operator=(const ProjectModel &real)
{
    this->version = real.getVersion();
    this->plantaskfile = real.getPlanFilename();
    this->realtaskfile = real.getRealFilename();
    this->checkedtaskfile = real.getCheckedFilename();
    this->pulzerecorderfile = real.getPulzeRecorderFilename();
    this->createdate = real.getEndCalculateDate();
    this->realcollectdate = real.getRealCollectDate();
    this->lastcheckdate = real.getLastCheckDate();
    this->endcalculatedate = real.getEndCalculateDate();
    this->lasteditdate = real.getLastEditDate();
    this->createuser = real.getCreateUser();
    this->lastcheckuser = real.getLastCheckUser();
    return *this;
}

// 重载==号
bool operator==(const ProjectModel &real1, const ProjectModel &real2)
{
    if (real1.getVersion() == real2.getVersion() && real1.getPlanFilename().compare(real2.getPlanFilename()) == 0
             && real1.getRealFilename().compare(real2.getRealFilename()) == 0 && real1.getCheckedFilename().compare(real2.getCheckedFilename()) == 0)
        return true;
    else
        return false;
}

ProjectModel::~ProjectModel()
{
}

// setter & getter
const QString ProjectModel::getPlanFilename() const { return plantaskfile; }
void ProjectModel::setPlanFilename(QString filename) { this->plantaskfile = filename; }
const QString ProjectModel::getRealFilename() const { return realtaskfile; }
void ProjectModel::setRealFilename(QString filename) { this->realtaskfile = filename; }
const QString ProjectModel::getCheckedFilename() const { return checkedtaskfile; }
void ProjectModel::setCheckedFilename(QString filename) { this->checkedtaskfile = filename; }

const QString ProjectModel::getPulzeRecorderFilename() const { return pulzerecorderfile; }
void ProjectModel::setPulzeRecorderFilename(QString filename) { this->pulzerecorderfile = filename; }

const QString ProjectModel::getCreateDate() const { return createdate; }
void ProjectModel::setCreateDate(QString date) { this->createdate = date; }
const QString ProjectModel::getRealCollectDate() const { return realcollectdate; }
void ProjectModel::setRealCollectDate(QString date) { this->realcollectdate = date; }
const QString ProjectModel::getLastCheckDate() const {return lastcheckdate; }
void ProjectModel::setLastCheckDate(QString date) { this->lastcheckdate = date; }
const QString ProjectModel::getEndCalculateDate() const { return endcalculatedate; }
void ProjectModel::setEndCalculateDate(QString date) { this->endcalculatedate = date; }
const QString ProjectModel::getLastEditDate() const { return lasteditdate; }
void ProjectModel::setLastEditDate(QString date) { this->lasteditdate = date; }

const QString ProjectModel::getCreateUser() const { return createuser; }
void ProjectModel::setCreateUser(QString user) { this->createuser = user; }
const QString ProjectModel::getLastCheckUser() const {return lastcheckuser; }
void ProjectModel::setLastCheckUser(QString user) { this->lastcheckuser = user; }

const float ProjectModel::getVersion() const { return version; }
