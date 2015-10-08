#ifndef FILE_OPERATION_NASCOPY_FILTER_H
#define FILE_OPERATION_NASCOPY_FILTER_H

#include "fileoperation.h"

/**
 * 本地文件操作类（NAS磁盘挂载后也可使用）
 * @author 范翔
 * @date 2014-08-07
 * @version 1.0.0
 */
class LocalFileOperation_NASCopyFilter : public LocalFileOperation
{

public:
    LocalFileOperation_NASCopyFilter() { };
	
    virtual ~LocalFileOperation_NASCopyFilter() { };

    /**
     * 拷贝目录及目录下文件
     * **** 过滤掉这些文件夹collect, mid_calcu, tmp_img ***
     * @param fromDir 源目录
     * @param toDir 目标目录
     * @param bCoverifFileExists true 同名时覆盖，flase，同名时返回false，终止拷贝
     */
    static bool copyDirectoryFiles(const QDir& fromDir, const QDir & toDir, bool bCoverifFileExists)
    {
        if (!toDir.exists())
        {
             if (!toDir.mkdir(toDir.absolutePath()))
                  return false;
        }

        QFileInfoList fileinfolist = fromDir.entryInfoList();
        foreach (QFileInfo fileinfo, fileinfolist)
        {
             if (fileinfo.fileName().compare(".") == 0 || fileinfo.fileName().compare("..") == 0)
                  continue;

             // 拷贝子目录
             if (fileinfo.isDir())
             {

                  // @author 范翔 过滤掉这些文件夹
                  if (fileinfo.fileName().endsWith("collect") || fileinfo.fileName().endsWith("mid_calcu") || fileinfo.fileName().endsWith("tmp_img"))
                  {
                       qDebug() << "copy filtered dir:"  << fileinfo.fileName();
                       continue;
                  }

                  // 递归调用拷贝
                  if (!copyDirectoryFiles(fileinfo.filePath(), toDir.filePath(fileinfo.fileName()), bCoverifFileExists))
                       return false;
             }
             else // 拷贝子文件
             {
                  if (bCoverifFileExists && toDir.exists(fileinfo.fileName()))
                  {
                       QDir toDir_ = toDir;
                       toDir_.remove(fileinfo.fileName());
                  }
                  if (!filecopy(fileinfo.filePath(), toDir.filePath(fileinfo.fileName())))
                  {
                       return false;
                  }
             }
        }
        return true;
    };

};

#endif // FILE_OPERATION_NASCOPY_FILTER_H