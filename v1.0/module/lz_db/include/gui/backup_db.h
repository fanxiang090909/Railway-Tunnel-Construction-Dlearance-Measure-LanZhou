#ifndef BACKUP_DB_H
#define BACKUP_DB_H

#include <QWidget>
#include <QDateTime>

namespace Ui {
    class BackupDatabaseWidget;
}

/**
 * 数据库备份界面类声明
 *
 * @author 熊雪 范翔
 * @version 1.0.0
 * @date 20150305
 */
class BackupDatabaseWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BackupDatabaseWidget(QWidget *parent = 0);
    
    ~BackupDatabaseWidget();
    
    /**
     * 获得最新数据库dump文件时间
     */
    QDateTime getNewestDBDumpFileTime();

public slots:

    /**
     * 检查程序打开过程中数据库是否有更新
     */
    void checkIfExport();
    /**
     * 检查程序打开过程中数据库需要导入
     */
    void checkIfImport();

private slots:

    /**
     * 导入将sql文件导入数据库，即更新数据库
     */
    void importSQLFileToDB();

    /**
     * 将数据库导出为sql文件，即备份数据库
     */
    void exportDBToSQLFile();

    /**
     * 接受DumpThread的finish消息
     */
    void receiveFinish(bool, QString, QString);

private:
    Ui::BackupDatabaseWidget *ui;

    /**
     * 最新数据库dump文件名
     */
    QString newestDBDumpFileName;

    QString timestampfilename;
};

#endif // BACKUP_DB_H
