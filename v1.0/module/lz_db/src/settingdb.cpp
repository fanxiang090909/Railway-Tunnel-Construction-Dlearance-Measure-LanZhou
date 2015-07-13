#include "settingdb.h"

/**
 * 数据库连接等各项参数配置类实现
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2014-1-4
 */
// singleton设计模式，静态私有实例变量
DatabaseSetting * DatabaseSetting::dbSettingInstance = NULL;

/**
 * singleton设计模式，公有静态方法返回单一实例
 * @return 单一实例
 */
DatabaseSetting * DatabaseSetting::getDBSettingInstance()
{
    if (dbSettingInstance == NULL)
    {
        dbSettingInstance = new DatabaseSetting();
    }
    return dbSettingInstance;
}

// singleton单一设计模式，构造函数私有
DatabaseSetting::DatabaseSetting(QObject *parent) :
    QObject(parent)
{
    ipaddress = "10.13.29.217";
    port = 3306;
    username = "root";
    passwd = "root";
    dbname = "lanzhou2013";
}

DatabaseSetting::~DatabaseSetting()
{
}

void DatabaseSetting::setIPAddress(QString newip) {ipaddress = newip;}
QString DatabaseSetting::getIPAddress() {return ipaddress;}
int DatabaseSetting::getPort() {return port;}
QString DatabaseSetting::getUsername() {return username;}
QString DatabaseSetting::getPasswd() {return passwd;}
QString DatabaseSetting::getDBName() {return dbname;}
