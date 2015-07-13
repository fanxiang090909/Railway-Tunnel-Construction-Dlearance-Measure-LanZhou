#ifndef PROJECTMODEL_H
#define PROJECTMODEL_H

#include <QObject>

using namespace std;

/**
 * 工程模型数据结构
 * @author 范翔
 * @version 1.0.0
 * @date 2014-3-27
 */
class ProjectModel
{
private:

    QString plantaskfile;
    QString realtaskfile;
    QString checkedtaskfile;

    QString pulzerecorderfile;

    QString createdate;
    QString realcollectdate;
    QString lastcheckdate;
    QString endcalculatedate;
    QString lasteditdate;

    QString createuser;
    QString lastcheckuser;

    float version;

public:
    ProjectModel(float initversion);

    ProjectModel(const ProjectModel & real);
    // 重载赋值号
    ProjectModel & operator=(const ProjectModel &real);
    // 重载==号
    friend bool operator==(const ProjectModel &real1, const ProjectModel &real2);

    ~ProjectModel();

    // setter & getter
    const QString getPlanFilename() const;
    void setPlanFilename(QString filename);
    const QString getRealFilename() const;
    void setRealFilename(QString filename);
    const QString getCheckedFilename() const;
    void setCheckedFilename(QString filename);

    void setPulzeRecorderFilename(QString filename);
    const QString getPulzeRecorderFilename() const;

    const QString getCreateDate() const;
    void setCreateDate(QString date);
    const QString getRealCollectDate() const;
    void setRealCollectDate(QString date);
    const QString getLastCheckDate() const;
    void setLastCheckDate(QString date);
    const QString getEndCalculateDate() const;
    void setEndCalculateDate(QString date);
    const QString getLastEditDate() const;
    void setLastEditDate(QString date);

    const QString getCreateUser() const;
    void setCreateUser(QString user);
    const QString getLastCheckUser() const;
    void setLastCheckUser(QString user);

    const float getVersion() const;
};

#endif // PROJECTMODEL_H
