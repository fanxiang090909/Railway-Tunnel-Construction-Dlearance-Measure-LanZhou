#include "imageviewer_master.h"

#include "ui_imageviewer.h"

#include "masterprogram.h"
#include "setting_master.h"

/**
 * 采集文件信息校对界面——原图浏览界面类定义
 * @author 熊雪
 * @author 范翔
 * @version 1.0.0
 * @date 2014-08-19
 */
MasterImageViewer::MasterImageViewer(QWidget *parent) : ImageViewer(parent)
{
    // 如果是图形修正界面看原图，隐藏播放
    ui->player_groupBox->setVisible(true);
    ui->askRawImagesWidget->setVisible(true);
}

MasterImageViewer::~MasterImageViewer()
{
}

bool MasterImageViewer::askforimgae(__int64 startnum, int numofasked)
{
    if (!hasinitmodel)
        return false;

    bool findstart = false;
    int tmpseqno;
    __int64 tmpmasterstartfc = -1;
    __int64 tmpmasterendfc = -1;
    std::list<CheckedItem>::iterator it = currentCheckedModel.begin();
    while (it != currentCheckedModel.end())
    {
        // 还没有找到起始时找起始帧所在文件
        if (!findstart && startnum >= (*it).start_frame_master && startnum < (*it).end_frame_master)
        {
            tmpseqno = (*it).seqno;
            tmpmasterstartfc = (*it).start_frame_master;
            tmpmasterendfc = (*it).end_frame_master;
            findstart = true;
            break;
        }
        it++;
    }

    if (!findstart)
        return false;

    // 如果存储在多个文件中（校正时做了合并），分成两次申请图像，本次申请头一个文件的
    if (tmpmasterendfc <= startnum + numofasked)
        numofasked = tmpmasterendfc - startnum + 1;

    bool ret = true;

    ProjectModel currentProjectModel = LzProjectAccess::getLzProjectAccessInstance()->getProjectModel(LzProjectClass::Main);
    QString realfilename = LzProjectAccess::getLzProjectAccessInstance()->getProjectPath(LzProjectClass::Main) + "/" + currentProjectModel.getRealFilename();

    QString tmpcamid;
    __int64 tmpstartfc;
    __int64 tmpendfc;
    bool tmpret = false;

    for (int i = 0; i < 5; i++)
    {
        switch (i)
        {
            case 0:  tmpcamid = ui->cam1->currentText(); tmpret = (*it).getStartEndVal(tmpcamid.toLocal8Bit().constData(), tmpstartfc, tmpendfc); break;
            case 1:  tmpcamid = ui->cam2->currentText(); tmpret = (*it).getStartEndVal(tmpcamid.toLocal8Bit().constData(), tmpstartfc, tmpendfc); break;
            case 2:  tmpcamid = ui->cam3->currentText(); tmpret = (*it).getStartEndVal(tmpcamid.toLocal8Bit().constData(), tmpstartfc, tmpendfc); break;
            case 3:  tmpcamid = ui->cam4->currentText(); tmpret = (*it).getStartEndVal(tmpcamid.toLocal8Bit().constData(), tmpstartfc, tmpendfc); break;
            case 4:  tmpcamid = ui->cam5->currentText(); tmpret = (*it).getStartEndVal(tmpcamid.toLocal8Bit().constData(), tmpstartfc, tmpendfc); break;
            default: break;
        }

        if (tmpcamid.trimmed().compare("") == 0)
            continue;

        if (!tmpret)
            continue;

        // 暂定自己设置的这5台相机
        if (tmpcamid.trimmed().compare("") != 0)
        {
            // 换算每个相机的startfc = startnum - tmpmasterstartfc + tmpstartfc
            qDebug() << "startnum = " << startnum << ", tmpmasterstartfc = " << tmpmasterstartfc << ", tmpstartfc = " << tmpstartfc << ", tempendfc = " << tmpendfc << ", startfc = (startnum - tmpmasterstartfc + tmpstartfc) = " << startnum - tmpmasterstartfc + tmpstartfc;
            tmpret = MasterProgram::getMasterProgramInstance()->askForRawImages(realfilename, tmpcamid, tmpseqno, currenttunnelid, startnum - tmpmasterstartfc + tmpstartfc, numofasked);
            if (tmpret)
            {
                ui->statusArea->append(QObject::tr("图像申请命令实际文件%1，相机%2的从帧号%3一共%4帧申请发送成功。").arg(realfilename).arg(tmpcamid).arg(startnum).arg(numofasked));
                continue;
            }
        }
        ret = false;
        ui->statusArea->append(QObject::tr("图像申请命令实际文件%1，相机%2的从帧号%3一共%4帧申请发送失败！！").arg(realfilename).arg(tmpcamid).arg(startnum).arg(numofasked));
    }
    return ret;
}