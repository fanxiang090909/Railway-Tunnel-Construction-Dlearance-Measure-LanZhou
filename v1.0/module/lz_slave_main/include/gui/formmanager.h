#ifndef FORMMANAGER_H
#define FORMMANAGER_H

#include <QObject>
#include "form.h"
#include "slaveprogram.h"

/**
 * 从控机界面显示管理类声明
 * @author fanxiang
 * @version 1.0.0
 * @date 20140112
 */
class FormManager : public QObject
{
    Q_OBJECT
public:
    explicit FormManager(QObject *parent = 0);
    ~FormManager();

	void setSlave(SlaveProgram* slave);
signals:

public slots:
    // 是否显示
    void showForm(bool showornot);
private:
    Form * f;

};

#endif // FORMMANAGER_H
