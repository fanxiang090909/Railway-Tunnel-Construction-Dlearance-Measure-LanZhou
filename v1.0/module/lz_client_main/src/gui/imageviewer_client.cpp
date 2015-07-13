#include "imageviewer_client.h"

#include "ui_imageviewer.h"

#include "setting_client.h"
#include "lz_project_access.h"
#include "LzSerialStorageAcqui.h"
#include "checkedtask_list.h"
#include <QDebug>

/**
 * 采集文件信息校对界面——原图浏览界面类定义
 * @author 熊雪
 * @author 范翔
 * @version 1.0.0
 * @date 2014-08-19
 */
ClientImageViewer::ClientImageViewer(QWidget *parent) : ImageViewer(parent)
{
    // 如果是图形修正界面看原图，隐藏播放
    ui->player_groupBox->setVisible(true);
    // TODO
    ui->askRawImagesWidget->setVisible(true);
}

ClientImageViewer::~ClientImageViewer()
{
}

bool ClientImageViewer::askforimgae(__int64 startnum, int numofasked)
{
    if (!hasinitmodel)
        return false;

    bool findstart = false;
    int tmpseqno;
    string tmptunnelname; // 实际采集时当成的隧道名，非实际隧道名
    __int64 tmpmasterstartfc;
    __int64 tmpmasterendfc;
    std::list<CheckedItem>::iterator it = currentCheckedModel.begin();
    while (it != currentCheckedModel.end())
    {
        // 还没有找到起始时找起始帧所在文件
        if (!findstart && startnum >= (*it).start_frame_master)
        {
            tmpseqno = (*it).seqno;
            tmptunnelname = (*it).tunnel_name;
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
            // TODO
            tmpret = false;
            //tmpret = MasterProgram::getMasterProgramInstance()->askForRawImages(realfilename, tmpcamid, tmpseqno, currenttunnelid, startnum - tmpmasterstartfc + tmpstartfc, numofasked);
            
            QString projectname = ClientSetting::getSettingInstance()->getCurrentEditingProject();
            QString projectdir = ClientSetting::getSettingInstance()->getParentPath() + "/" + projectname;
            QString projecttmpimgdir = ClientSetting::getSettingInstance()->getClientTmpLocalParentPath() + "/" + projectname;
            QString projectdate = projectname.mid(projectname.length() - 8);

            qDebug() << projectname << projectdir << projecttmpimgdir << projectdate;

            QByteArray tmpstrba = projectdir.toLocal8Bit();
            std::string tmpdir = tmpstrba.constData();
            tmpstrba = tmpstrba = projecttmpimgdir.toLocal8Bit();
            std::string tmpoutdir = tmpstrba.constData();
            tmpstrba = projectdate.toLocal8Bit();
            std::string tmpdate = tmpstrba.constData();
            tmpstrba = tmpcamid.toLocal8Bit();
            std::string tmpcameraid = tmpstrba.constData();
            
            // int 转 cstring
            char seqnostr[6]; // 9999足够大
            sprintf(seqnostr, "%d", tmpseqno);
            
            std::string openfilename = tmpdir + "/collect/" + seqnostr + '_' +  tmptunnelname + "_" + tmpdate + "_" + tmpcameraid + ".dat";
            std::string outputdir = tmpoutdir + "/tmp_img/" + seqnostr + '_' +  tmptunnelname + "_" + tmpdate + "_" + tmpcameraid;
        
            qDebug() << QObject::tr(openfilename.c_str()) << QObject::tr(outputdir.c_str());

            QDir my_dir;
            if (!my_dir.exists(tr(outputdir.c_str())))
            {
		        my_dir.mkpath(tr(outputdir.c_str()));
            }

            BLOCK_KEY startfr = startnum;
            BLOCK_KEY endfr = startfr + numofasked;
            // 查找是否有该线路
            LzSerialStorageAcqui *lzserialimg = new LzSerialStorageAcqui();
            lzserialimg->setting(250, 1024*1024*100, true);

            try {
            
                bool ret = lzserialimg->openFile(openfilename.c_str());

                if (!ret)
                {
                    ui->statusArea->append(QObject::tr("从流式文件%1生成临时小图片失败，该文件打不开！").arg(QObject::tr(openfilename.c_str())));
                    lzserialimg->closeFile();
                    delete lzserialimg;
                    return false;
                }

                std::vector<BLOCK_KEY> keys = lzserialimg->readKeys();
                qDebug() << "file frames vector size:" << keys.size();
                if (keys.size() > 0)
                {
                    //for (int i = 0; i < keys.size(); i++)
                        //qDebug() << keys.at(i);
                    qDebug() << "file frames vector startfc: " << keys.at(0) << ", endfc: " << keys.at(keys.size() - 1);
                    qDebug() << "file be asked frames startfc: " << startnum << ", num: " << numofasked;
                }

                ret = lzserialimg->readFrameSaveAsImage(startfr, endfr, outputdir.c_str(), true, false);

                lzserialimg->closeFile();

                if (ret)
                    ui->statusArea->append(QObject::tr("从流式文件%1生成临时小图片成功").arg(QObject::tr(openfilename.c_str())));
                else
                    ui->statusArea->append(QObject::tr("从流式文件%1生成临时小图片失败！无法读出该文件的%2到%3帧数据，可能不存在该帧").arg(QObject::tr(openfilename.c_str())).arg(startfr).arg(endfr));

                // 更新界面
                receivedepressfinish("");
            }
            catch (LzException & ex)
            {
                ui->statusArea->append(QObject::tr("从流式文件%1生成临时小图片失败").arg(QObject::tr(openfilename.c_str())));
            }
            delete lzserialimg;
            
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