#include "lz_correct_queue.h"

#include <QProcess>
#include <QObject>

#include "setting_slave.h"

#include "lz_project_access.h"

#include "xmltaskfileloader.h"
#include "xmlrealtaskfileloader.h"
#include "plantask_list.h"
#include "realtask_list.h"

#include "LzSerialStorageAcqui.h"

#include "fileoperation.h"

/**
 * 从控机查看原图队列类实现
 * 顺序执行接受的消息任务，防止冲突
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2014-7-31
 */
LzCorrectQueue::LzCorrectQueue(QObject * parent) : LzSlaveMsgQueue(parent)
{
}

LzCorrectQueue::~LzCorrectQueue()
{
}

/**
 * 队列完成后执行
 */
void LzCorrectQueue::finish() 
{
}

/**
 * 任务命令解析
 * 实现父类的纯虚函数
 */
bool LzCorrectQueue::parseMsg(QString msg)
{
    QStringList strList = msg.split(",", QString::SkipEmptyParts);
 
    int msgid = strList.value(0).toInt();
    qDebug() << "msgid= " << msgid;

    if (msgid > 4000)
    {
        switch (msgid)
        {
            case 4101: // 浏览原图计算4101,realfile=“xxxx”,camera_index=x,seqno=x,tunnelid=x,start_framecounter=xx,frame_num=xx(frame_num表示start与end相差距离)
            {   
                if (strList.length() < 6)
                {
                    qDebug() << tr("解析字符出错") << msg;
                    return false;
                }
                // @author xiongxue改
                // int tunnelid=strList.value(3).toInt();//这样是直接传tunnelid过来，而不加前面的tunnelid
                QString realfilename = strList.at(1).mid(9);
				QString cameraindex = strList.at(2).mid(13);
				int seqno = strList.at(3).mid(6).toInt();

                int tunnelid=strList.at(4).mid(9).toInt();//mid表示过滤掉前面的tunnelid
                _int64 startFrameCounter = strList.at(5).mid(19).toLongLong();
                int frameNum = strList.at(6).mid(10).toInt();

                // 时间：隐含信息从队列中读出（是否合理？）
                int lineid = LzProjectAccess::getLzProjectAccessInstance()->getLzPlanList(LzProjectClass::Main).getlineid(tunnelid);
                string datetime_string = LzProjectAccess::getLzProjectAccessInstance()->getLzPlanList(LzProjectClass::Main).getdatetime(tunnelid);
                QString datetime = QString::fromLocal8Bit(datetime_string.c_str());

                emit signalParsedMsgToSlave(QObject::tr("[主控] 命令:申请隧道Id%1,相机号%2,原始图像数据seqno为%3的文件，帧号从%4开始%5帧").arg(tunnelid).arg(cameraindex).arg(seqno).arg(startFrameCounter).arg(frameNum));

                // 压缩文件并发送
                send_RARFile(realfilename, seqno, cameraindex, startFrameCounter, frameNum, lineid, datetime, tunnelid);

                // 压缩完成
                endMsg();

                break;
            }
            case 4301: // 生成临时工程 4301,filename="xxx",lineid=xx,linename="xxx",date=xxx,tunnelid=xx,seqno=x,start_framecounter=xx,frame_num=xx(frame_num表示start与end相差距离)
            {
                if (strList.length() < 9)
                {
                    qDebug() << tr("解析字符出错") << msg;
                    return false;
                }

                // @author xiongxue改
                // int tunnelid=strList.value(3).toInt();//这样是直接传tunnelid过来，而不加前面的tunnelid
                QString filename = strList.at(1).mid(9);
                int lineid = strList.at(2).mid(7).toInt();
                QString linename = strList.at(3).mid(9);
                QString datetime = strList.at(4).mid(5);
                int tunnelid = strList.at(5).mid(9).toInt();//mid表示过滤掉前面的tunnelid
				int seqno = strList.at(6).mid(6).toInt();

                __int64 startFrameCounter = strList.at(7).mid(19).toLongLong();
                int frameNum = strList.at(8).mid(10).toInt();

                emit signalParsedMsgToSlave(QObject::tr("[主控] 命令:生成临时工程隧道Id%1,seqno为%2的隧道，帧号从%3开始%4帧").arg(tunnelid).arg(seqno).arg(startFrameCounter).arg(frameNum));

                // 分析路径
                QString projectname = LzProjectAccess::getLzProjectAccessInstance()->getProjectFilename(LzProjectClass::Main);
                QString projecttmpimgdir = SlaveSetting::getSettingInstance()->getParentPath() + "/" + projectname.left(projectname.length() - 5);
                QString projectdate = projectname.mid(projectname.length() - 13).left(8);

                LocalFileOperation::removeDirwithContent(QString(SlaveSetting::getSettingInstance()->getParentPath() + "/TEMPDIR_20000101/"));

                // 如果没有该目录，创建新目录
                bool ret = SlaveSetting::getSettingInstance()->createSlaveProjectDir(QString(SlaveSetting::getSettingInstance()->getParentPath() + "/TEMPDIR_20000101/"));

                // 设置工作工程文件名
                ret = LzProjectAccess::getLzProjectAccessInstance()->setProjectName(LzProjectClass::Main, SlaveSetting::getSettingInstance()->getParentPath() + "/TEMPDIR_20000101/TEMPDIR_20000101.proj");

                // 为了得到得到隧道名
                RealTask tmptask;
                ret = LzProjectAccess::getLzProjectAccessInstance()->getLzRealList(LzProjectClass::Main).getRealTaskBySeqno(seqno, tmptask);

                QString fromdir = projecttmpimgdir + "/collect/" + QString("%1").arg(seqno) + "_" + QString::fromLocal8Bit(tmptask.tunnelname.c_str()) + "_" + projectdate + "_";
                QString todir =  SlaveSetting::getSettingInstance()->getParentPath() + "/TEMPDIR_20000101/collect/" + QString("%1").arg(seqno) + "_" + QString::fromLocal8Bit(tmptask.tunnelname.c_str()) + "_" + projectdate + "_";
                if (ret)
                {
                    SlaveModel slaveModel = SlaveSetting::getSettingInstance()->getCurrentSlaveModel();
                    QString fromfilename = fromdir + slaveModel.box1.camera_ref.c_str() + ".dat";
                    QString tofilename = todir + slaveModel.box1.camera_ref.c_str() + ".dat";
                    ret = createNewDir(fromfilename, tofilename, startFrameCounter, startFrameCounter+frameNum);
                    if (ret != 0)
                    {
                        emit signalParsedMsgToSlave(QObject::tr("[从控] 生成临时工程失败，错误原因：拷贝.dat文件到新路径失败%1，从路径%2到%3").arg(ret).arg(fromfilename).arg(tofilename));
                        // 完成
                        endMsg();
                        break;
                    }
                    fromfilename = fromdir + slaveModel.box1.camera.c_str() + ".dat";
                    tofilename = todir + slaveModel.box1.camera.c_str() + ".dat";
                    createNewDir(fromfilename, tofilename, startFrameCounter, startFrameCounter+frameNum);
                    
                    fromfilename = fromdir + slaveModel.box2.camera_ref.c_str() + ".dat";
                    tofilename = todir + slaveModel.box2.camera_ref.c_str() + ".dat";
                    createNewDir(fromfilename, tofilename, startFrameCounter, startFrameCounter+frameNum);

                    fromfilename = fromdir + slaveModel.box2.camera.c_str() + ".dat";
                    tofilename = todir + slaveModel.box2.camera.c_str() + ".dat";
                    createNewDir(fromfilename, tofilename, startFrameCounter, startFrameCounter+frameNum);

                }
                else
                    emit signalParsedMsgToSlave(QObject::tr("[从控] 生成临时工程失败，未从RealList找到序号为%1的文件").arg(seqno));

                // 完成
                endMsg();
                break;
            }
            default: return false;
        }
    }

    return true;
}

/**
 * 发送图像文件压缩包
 * @param 压缩文件查找时需要帧的位置及压缩帧数
 * @author 熊雪
 */
void LzCorrectQueue::send_RARFile(QString realfilename, int seqno, QString cameraindex, _int64 startFrameCounter, int frameNum, int lineid, QString datetime, int tunnelid)
{
    QString projectname = LzProjectAccess::getLzProjectAccessInstance()->getProjectFilename(LzProjectClass::Main);
    QString projecttmpimgdir = SlaveSetting::getSettingInstance()->getParentPath() + "/" + projectname.left(projectname.length() - 5);
    QString projectdate = projectname.mid(projectname.length() - 13).left(8);

    // 为了得到得到隧道名
    RealTask tmptask;
    bool ret = LzProjectAccess::getLzProjectAccessInstance()->getLzRealList(LzProjectClass::Main).getRealTaskBySeqno(seqno, tmptask);
    if (!ret)
        return;
    QByteArray tmpstrba = projecttmpimgdir.toLocal8Bit();
    std::string tmpdir = tmpstrba.constData();
    tmpstrba = projectdate.toLocal8Bit();
    std::string tmpdate = tmpstrba.constData();
    tmpstrba = cameraindex.toLocal8Bit();
    std::string tmpcameraid = tmpstrba.constData();
    // int 转 cstring
    char seqnostr[6]; // 9999足够大
    sprintf(seqnostr, "%d", seqno);

    std::string openfilename = tmpdir + "/collect/" + seqnostr + '_' +  tmptask.tunnelname + "_" + tmpdate + "_" + tmpcameraid + ".dat";
    std::string outputdir = tmpdir + "/tmp_img/" + seqnostr + '_' +  tmptask.tunnelname + "_" + tmpdate + "_" + tmpcameraid;
        
	QDir my_dir;
	QString tmpimgdir = QObject::tr(outputdir.c_str());
	// 或者下面这两行，也是在指定目录下生成文件夹

	//如果目录不存在的话，就建立这个目录
	if (!my_dir.exists(tmpimgdir))
    {
		my_dir.mkpath(tmpimgdir);
        qDebug() << QObject::tr("[从控] 源图像存储目录成功生成%1。").arg(tmpimgdir);
        emit signalParsedMsgToSlave(QObject::tr("[从控] 源图像存储目录成功生成%1。").arg(tmpimgdir));
    }
    else
    {
        // 删除文件其中的内容，不能删除一个单独的文件
        bool ret;//只能删除文件夹及其中的内容，不能删除一个单独的文件
        ret = LocalFileOperation::removeDirwithContent(tmpimgdir);
        if (!ret)
        {
            qDebug() << QObject::tr("[从控] 删除原压缩包文件%1不成功").arg(tmpimgdir);
            emit signalParsedMsgToSlave(QObject::tr("[从控] 删除原压缩包文件%1不成功").arg(tmpimgdir));
        }
        else
        {
            my_dir.mkpath(tmpimgdir);
            qDebug() << QObject::tr("[从控] 源图像存储目录成功生成%1。").arg(tmpimgdir);
            emit signalParsedMsgToSlave(QObject::tr("[从控] 源图像存储目录成功生成%1。").arg(tmpimgdir));
        }
    }

    qDebug() << QObject::tr(openfilename.c_str());
    BLOCK_KEY startfr = startFrameCounter;
    BLOCK_KEY endfr = startfr + frameNum;
    // 查找是否有该线路
    LzSerialStorageAcqui *lzserialimg = new LzSerialStorageAcqui();
    lzserialimg->setting(250, 1024*1024*100, true);

    try {
            
        bool ret = lzserialimg->openFile(openfilename.c_str());
        if (!ret)
        {
            emit signalParsedMsgToSlave(QObject::tr("[从控] 从流式文件%1生成临时小图片失败，文件打不开！").arg(QString::fromLocal8Bit(openfilename.c_str())));
            lzserialimg->closeFile();
            delete lzserialimg;
            return;
        }

        std::vector<BLOCK_KEY> keys = lzserialimg->readKeys();
        qDebug() << "file frames vector size:" << keys.size();
        if (keys.size() > 0)
        {
            //for (int i = 0; i < keys.size(); i++)
                //qDebug() << keys.at(i);
            qDebug() << "file frames vector startfc: " << keys.at(0) << ", endfc: " << keys.at(keys.size() - 1);
            qDebug() << "file be asked frames startfc: " << startFrameCounter << ", num: " << frameNum;
        }

        ret = lzserialimg->readFrameSaveAsImage(startfr, endfr, outputdir.c_str(), true, false);

        lzserialimg->closeFile();
        
        if (ret)
            emit signalParsedMsgToSlave(QObject::tr("[从控] 从流式文件%1生成临时小图片成功").arg(tmpimgdir));
        else
            emit signalParsedMsgToSlave(QObject::tr("[从控] 从流式文件%1生成临时小图片失败！无法读出该文件的%2到%3帧数据，可能不存在该帧").arg(tmpimgdir).arg(startfr).arg(endfr));

    }
    catch (LzException & ex)
    {
        emit signalParsedMsgToSlave(QObject::tr("[从控] 从流式文件%1生成临时小图片失败").arg(tmpimgdir));
    }
    delete lzserialimg;

    // 文件压缩，compress用的是QProcess的excute
	QString  Rar_address;
    QProcess pro(0);
    Rar_address = SlaveSetting::getSettingInstance()->getRarAddressname();
    qDebug() << "rar system address" << Rar_address;

    QStringList compress_parameter;
    compress_parameter.append("a");
    compress_parameter.append("-ep1");
    compress_parameter.append("-r");
    compress_parameter.append("-ibck");
    compress_parameter.append("-o+");
    QString rarfilename = QString("%1").arg(tmpimgdir + ".rar");
    compress_parameter.append(rarfilename);
    QString rardirname = QString("%1").arg(QObject::tr(outputdir.c_str()));
    compress_parameter.append(rardirname);
    qDebug() <<"rarfilename:" << rarfilename << "rardirname:" << rardirname;

    int bret = pro.execute(Rar_address,compress_parameter);
    emit signalParsedMsgToSlave(QObject::tr("主控申请的原图像文件压缩成功。%1").arg(rarfilename));

    emit signalFileToMaster(rarfilename);
}

int LzCorrectQueue::createNewDir(QString sourceDir, QString toDir, long long newstartfc, long long newendfc)
{
    toDir.replace("\\", "/");
    if (sourceDir == toDir)
    {
        return -2;
    }
    if (!QFile::exists(sourceDir))
    {
        return -3;
    }
    QDir createfile;
    bool exist = createfile.exists(toDir);
    if (exist)
    {
        createfile.remove(toDir);
    }
    else
    {
        if (!createfile.mkpath(QFileInfo(toDir).path()))
        {
            return -4;
        }
    }

    // 文件拷贝
    LzSerialStorageAcqui * lzinput = new LzSerialStorageAcqui();                               //左相机流式存储类
    LzSerialStorageAcqui * lzoutput = new LzSerialStorageAcqui(); 
    string input_file = sourceDir.toLocal8Bit().constData();
    string output_file = toDir.toLocal8Bit().constData();
        
    // @author范翔加,如果不是从中断位置继续计算,创建文件
    lzoutput->createFile(output_file.c_str(), sizeof(DataHead));
    bool input_isopen = lzinput->openFile(input_file.c_str());
    bool output_isopen = lzoutput->openFile(output_file.c_str());
        
    if (!input_isopen || !output_isopen)
    {
        // 关闭文件
        lzinput->closeFile();
        lzoutput->closeFile();
        delete lzinput;
        delete lzoutput;
        return -5;
    }

    std::vector<BLOCK_KEY> inputkeys = lzinput->readKeys();

    BLOCK_KEY startfr = inputkeys.at(0);
    BLOCK_KEY endfr = inputkeys.at(inputkeys.size() - 1);
    
    DataHead datahead;

    lzinput->readHead(&datahead);

    qDebug() << QString::fromLocal8Bit(datahead.line_name);
    qDebug() << QString::fromLocal8Bit(datahead.tunnel_name);
    bool isdouble = datahead.is_double_line;
    if (isdouble)
        qDebug() << QObject::tr("双线");
    else
        qDebug() << QObject::tr("单线");
        
    bool isnormal = datahead.is_normal;
    if (isnormal && isdouble)
        qDebug() << QObject::tr("正常拍摄");
    else if (!isnormal && isdouble)
        qDebug() << QObject::tr("非正常拍摄");
    else if (!isnormal && !isdouble)
        qDebug() << QObject::tr("逆序");
    else
        qDebug() << QObject::tr("正序");
        
    bool direct = datahead.carriage_direction;
    if (direct)
        qDebug() << QObject::tr("正向");
    else
        qDebug() << QObject::tr("反向");

    qDebug() << QString("%1").arg(datahead.interval_pulse_num);
    qDebug() << QString("%1").arg(datahead.seqno);
    qDebug() << QString::fromLocal8Bit(datahead.datetime);
        
    qDebug() << QString("%1").arg(endfr - startfr + 1);
    qDebug() << QString("%1").arg(startfr);
    qDebug() << QString("%1").arg(endfr);

    //lzoutput->writeHead(&datahead);
    lzoutput->setAcquiHead( &datahead );

    bool tmphasblock = false;

    if (newstartfc > startfr)
        startfr = newstartfc;
    if (newendfc < endfr)
        endfr = newendfc;

    tmphasblock = lzinput->retrieveBlock(startfr);
    if ( !tmphasblock )
    {
        // 关闭文件
        lzinput->closeFile();
        lzoutput->closeFile();
        delete lzinput;
        delete lzoutput;

        return -9;
    }

    do
	{
        char* matbuffer = decompressbuf;

        size_t size;
        BlockInfo blockinfo;

        if(!lzinput->readBlock(matbuffer))
        {
            startfr++;
            continue;
        }

        // @author 范翔加方法，未经测试
        blockinfo = lzinput->getCurrentBlockInfo();
        size = blockinfo.size;

        //lzoutput->writeBlock(matbuffer, size);
        lzoutput->addFrame((char*)matbuffer, blockinfo.size, &blockinfo);

        startfr++;
	}
	while( startfr != endfr );
        
    // 关闭文件
    lzinput->closeFile();
    lzoutput->closeFile();
    delete lzinput;
    delete lzoutput;

    return 0;
    
}