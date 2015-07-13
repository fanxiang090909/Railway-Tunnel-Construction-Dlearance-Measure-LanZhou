#ifndef LZ_CORRECT_QUEUE_H
#define LZ_CORRECT_QUEUE_H

#include <QList>
#include <QDebug>

#include "lz_slavemsg_queue.h"

#include "LzSerialStorageAcqui.h"

/**
 * 从控机查看原图队列类设计
 * 顺序执行接受的消息任务，防止冲突
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2014-7-31
 */
class LzCorrectQueue : public LzSlaveMsgQueue
{
    Q_OBJECT

public:
    /**
     * 构造函数
     */
    LzCorrectQueue(QObject * parent = 0);

    virtual ~LzCorrectQueue();

protected:
    /**
     * 队列完成后执行
     */
    virtual void finish();

private:

    /**
     * 任务命令解析
     * 实现父类的纯虚函数
     */
    virtual bool parseMsg(QString newmsg);

//TODO TOCHANGE TO private
public:

    /**
     * 发送图像文件压缩包
     * @param 压缩文件查找时需要帧的位置及压缩帧数
     * @author 熊雪
     */
    void send_RARFile(QString realfilename, int seqno, QString cameraindex, _int64 startFrameCounter, int frameNum, int lineid, QString datetime, int tunnelid);

    /**
     * 创建临时计算流式文件
     */
    int createNewDir(QString sourceDir, QString toDir, long long startfc, long long endfc);

private:
    
    char decompressbuf[DECOMPRESS_BUF_SIZE];
};

#endif // LZ_CORRECT_QUEUE_H
