#ifndef FILE_OPERATION_LZ_SERIAL_STORAGE_H
#define FILE_OPERATION_LZ_SERIAL_STORAGE_H

#include <QDebug>

#include "fileoperation.h"

#include "LzSerialStorageAcqui.h"

/**
 * 备份线程类
 * @author 范翔
 * @version 1.0.0
 * @date 2014-08-05
 */
class LzSerialStorageBackupThread : public LzBackupThread 
{
    Q_OBJECT

public:

    LzSerialStorageBackupThread(QObject * parent = 0) : LzBackupThread(parent) { }

    virtual ~LzSerialStorageBackupThread() { }

protected:

    virtual void run()
    {
        // 初始时不中断设置
        ifsuspend = false;

        if (!hasinit)
        {
            emit finish(-6, filename, 0);
            return;
        }
        toDir.replace("\\", "/");
        if (sourceDir == toDir)
        {
            emit finish(-2, filename, 0);
            return;
        }
        if (!QFile::exists(sourceDir))
        {
            emit finish(-3, filename, 0);
            return;
        }
        QDir createfile;
        bool exist = createfile.exists(toDir);
        if (exist)
        {
            if(coverFileIfExist){
                createfile.remove(toDir);
            }
        }
        else
        {
            if (!createfile.mkpath(QFileInfo(toDir).path()))
            {
                emit finish(-4, filename, 0);
                return;
            }
        }

        // 文件拷贝
        LzSerialStorageAcqui * lzinput = new LzSerialStorageAcqui();                               //左相机流式存储类
        LzSerialStorageAcqui * lzoutput = new LzSerialStorageAcqui(); 
        string input_file = sourceDir.toLocal8Bit().constData();
        string output_file = toDir.toLocal8Bit().constData();
        
        // @author范翔加,如果不是从中断位置继续计算,创建文件
        if (!isfrominterruptfile)
            lzoutput->createFile(output_file.c_str(), sizeof(DataHead));
        bool input_isopen = lzinput->openFile(input_file.c_str());
        bool output_isopen = lzoutput->openFile(output_file.c_str());
        
        if (!input_isopen || !output_isopen)
        {
            if (!input_isopen)
            {
                emit finish(-7, filename, 0);
            }
            if (!output_isopen)
            {
                emit finish(-5, filename, 0);
            }

            // 关闭文件
            lzinput->closeFile();
            lzoutput->closeFile();
            delete lzinput;
            delete lzoutput;
            return;
        }

        std::vector<BLOCK_KEY> inputkeys = lzinput->readKeys();

        BLOCK_KEY startfr = inputkeys.at(0);
        BLOCK_KEY endfr = inputkeys.at(inputkeys.size() - 1);

        lzinput->readHead(&datahead);

        if (!isfrominterruptfile)
        {
            lzinput->readHead(&datahead);

            qDebug() << QString::fromLocal8Bit(datahead.line_name);
            qDebug() << QString::fromLocal8Bit(datahead.tunnel_name);
            bool isdouble = datahead.is_double_line;
            if (isdouble)
                qDebug() << tr("双线");
            else
                qDebug() << tr("单线");
        
            bool isnormal = datahead.is_normal;
            if (isnormal && isdouble)
                qDebug() << tr("正常拍摄");
            else if (!isnormal && isdouble)
                qDebug() << tr("非正常拍摄");
            else if (!isnormal && !isdouble)
                qDebug() << tr("逆序");
            else
                qDebug() << tr("正序");
        
            bool direct = datahead.carriage_direction;
            if (direct)
                qDebug() << tr("正向");
            else
                qDebug() << tr("反向");

            qDebug() << QString("%1").arg(datahead.interval_pulse_num);
            qDebug() << QString("%1").arg(datahead.seqno);
            qDebug() << QString::fromLocal8Bit(datahead.datetime);
        
            qDebug() << QString("%1").arg(endfr - startfr + 1);
            qDebug() << QString("%1").arg(startfr);
            qDebug() << QString("%1").arg(endfr);

            //lzoutput->writeHead(&datahead);
            lzoutput->setAcquiHead( &datahead );
        }

        bool tmphasblock = false;

        if (isfrominterruptfile)
            startfr = interruptedfilepos + 1;
       
        tmphasblock = lzinput->retrieveBlock(startfr);
        if ( !tmphasblock )
        {
            // 关闭文件
            lzinput->closeFile();
            lzoutput->closeFile();
            delete lzinput;
            delete lzoutput;

            emit finish(-9, filename, 0);

            return;
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

            emit statusShow(blockinfo.key, filename);

            if (ifsuspend)
            {
                // 关闭文件
                lzinput->closeFile();
                lzoutput->writeHead(&datahead);
                lzoutput->closeFile();
                delete lzinput;
                delete lzoutput;

                emit finish(-1, filename, blockinfo.key);

                return;
            }

            startfr++;
		}
		while( startfr != endfr );
        
        // 关闭文件
        lzinput->closeFile();
        lzoutput->closeFile();
        delete lzinput;
        delete lzoutput;

        emit finish(0, filename, 0);
        return;
    }

private:

    DataHead datahead;

   	char decompressbuf[DECOMPRESS_BUF_SIZE];
};

#endif // FILE_OPERATION_LZ_SERIAL_STORAGE_H