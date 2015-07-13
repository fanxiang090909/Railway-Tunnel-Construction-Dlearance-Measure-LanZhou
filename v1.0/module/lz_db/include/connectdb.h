#ifndef CONNECTDB_H
#define CONNECTDB_H

#include <QSqlDatabase>
#include <QMessageBox>
#include <QDebug>

/**
 * 隧道基础数据库连接
 *
 * @author fanxiang
 * @version 1.0.0
 */

static bool CREATE_MYSQL_CONNECTION(QString ipaddress)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName(ipaddress);
    db.setPort(3306);
    db.setDatabaseName("lanzhou2013");
    db.setUserName("root");
    db.setPassword("root");

    if(!db.open())
    {
        qDebug() << QObject::tr("连接数据库错误");
        QString a = "";
        QStringList strlist = QSqlDatabase::drivers();
        foreach(QString driver, strlist)
            a += driver + ",";
        QMessageBox::critical(0, QObject::tr("连接数据库错误"),
                                 QObject::tr("无法连接到数据库:%1:3306//lanzhou2013，请检查数据库设置，驱动：%2！").arg(ipaddress).arg(a),
                              QMessageBox::Ok);
        return false;
    }

    return true;
}

static bool CREATE_SQLITE_CONNECTION(QString &filename)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(filename);

    if(!db.open())
    {
        qDebug() << filename;
        qDebug() << QObject::tr("连接数据库错误");
        QMessageBox::critical(0, QObject::tr("连接数据库错误"),
                              QObject::tr("无法连接到数据库，请检查数据库设置！"), QMessageBox::Ok);
        return false;
    }

    return true;
}

#endif // CONNECTDB_H
