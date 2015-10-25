/*
 *	LzSerialStorageRT.h
 *	兰州隧道限界检测项目  计算车身晃动流式存储类
 *
 *	作者: Ding(leeleedzy@gmail.com)
 *  版本: alpha v1.0.0
 *	日期：2014.1.14
 */

#ifndef LZ_SERIALSTORAGERT_H
#define LZ_SERIALSTORAGERT_H

#include <opencv2\core\core.hpp>
#include "LzSerialStorageMat.h"

#include <datastructure.h>

class LzSerialStorageRT : public LzSerialStorageMat
{
public:

    void writeMatV2(double a, double b, cv::Mat m) {
        size_t szcount = 0;
        // 给mapbuffer和szcount赋值
        memcpy(blockinfo.reservebit, &a, sizeof(double));
        memcpy(blockinfo.reservebit+sizeof(double), &b, sizeof(double));

        writeMat(m);
    }

    void writeMatV2(double a, double b, cv::Mat &m, BlockInfo &blockinfo, bool specifykey = true ) {
        memcpy(blockinfo.reservebit, &a, sizeof(double));
        memcpy(blockinfo.reservebit+sizeof(double), &b, sizeof(double));

        LzSerialStorageMat::writeMat(m, blockinfo, specifykey);
    }


    bool readMatV2(double& a, double& b, cv::Mat & m) 
    {
        if ( !LzSerialStorageMat::readMat(m) )
            return false;

        // fan: memcpy 某种意义上可理解为赋值号“=”，尤其适用大数据块的赋值
        // fan: 与strcpy()不同的是,memcpy()会完整的复制n个字节,不会因为遇到字符串结束'\0'而结束
        memcpy(&a, blockinfo.reservebit, sizeof(double));
        memcpy(&b, blockinfo.reservebit+sizeof(double), sizeof(double));
        return true;
    }
};

#endif