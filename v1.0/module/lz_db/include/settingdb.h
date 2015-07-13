#ifndef SETTINGDB_H
#define SETTINGDB_H

#include <QObject>

/**
 * 数据库连接等各项参数配置类声明
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2014-1-4
 */
class DatabaseSetting : public QObject
{
    Q_OBJECT
private:

    // singleton设计模式，静态私有实例变量
    static DatabaseSetting * dbSettingInstance;

public:

    /**
     * singleton设计模式，公有静态方法返回单一实例
     * @return 单一实例
     */
    static DatabaseSetting * getDBSettingInstance();

    ~DatabaseSetting();

    void setIPAddress(QString newip);
    QString getIPAddress();
    int getPort();
    QString getUsername();
    QString getPasswd();
    QString getDBName();

private:
    explicit DatabaseSetting(QObject *parent = 0);

    QString ipaddress;
    int port;
    QString username;
    QString passwd;
    QString dbname;
signals:

public slots:

};

#endif // SETTINGDB_H
