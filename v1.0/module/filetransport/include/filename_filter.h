#ifndef FILENAME_FILTER_H
#define FILENAME_FILTER_H

#include <QObject>


/**
 * 文件传输使用，接收文件根据文件名，改变存放路径
 * @param recfilename 接收文件名
 * @param directory 引用，存放路径，如果不符合条件，路径不变，否则添加子目录
 * @return true 找到存放路径成功
 * @see FileReceiver
 * @see 【参考】 设计-每个工程目录结构.txt
 * @author 范翔
 * @date 2014-05-07
 */
bool static FILTER_FILENAME(QString recfilename, QString & directory)
{

    // parent path
    if (recfilename.endsWith(".dat")) // 采集文件
         directory = directory + "/collect";
    else if (recfilename.endsWith(".mdat")) // 中间计算结果文件
         directory = directory + "/mid_calcu";
    else if (recfilename.endsWith(".fdat")) // 融合计算结果
         directory = directory + "/fuse_calcu";
    else if (recfilename.endsWith(".syn") || recfilename.endsWith("_correct.log"))
         directory = directory + "/syn_data";
    else if (recfilename.endsWith(".log"))
         directory = directory;
    else if (recfilename.endsWith(".rar") || recfilename.contains("image"))
         directory = directory + "/tmp_img";
    else if (recfilename.endsWith(".xml") && (!recfilename.startsWith("network_config")) && (!recfilename.startsWith("output_height")))
         directory = directory + "/calcu_calibration";
    else
        return false;
    return true;
}

#endif // FILENAME_FILTER_H