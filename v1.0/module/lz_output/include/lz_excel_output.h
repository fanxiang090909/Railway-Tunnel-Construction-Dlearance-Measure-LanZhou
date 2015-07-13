#ifndef LZ_Excel_OUTPUT_H
#define LZ_Excel_OUTPUT_H

#include <iostream>
#include <fstream>
#include <string>

#include "lz_output.h"
#include "excelengine.h"

using namespace std;

/**
 * 兰州项目限界数据图表输出
 * EXCEL格式输出限界图表
 * @see LzOutput 父类
 * @author 范翔
 * @date 2014-06-12
 * @version 1.0.0
 */
class LzExcelOutput : public LzOutput
{
public:
    LzExcelOutput ();
	
    virtual ~LzExcelOutput ();
	
    int outputSingleSection(TunnelDataModel * inputbasicdata, QString collectdate, SectionData & inputdata, QString templatepath, QString outputfile, QString insertimgpath);

    int outputSingleTunnel(TunnelDataModel * inputbasicdata, ClearanceSingleTunnel * inputdata, QString templatepath, QString outputfile, QString insertimgpath);

    int outputMultiTunnels(ClearanceMultiTunnels * inputdata, QString templatepath, QString outputfile, QString insertimgpath);

private:
    /**
     * 输出图片用
     */
    QAxObject * excel1;
    bool hasinit;
    /**
     * 初始化EXCEL生成接口
     */
    void initExcelEngine();

};

#endif // LZ_Excel_OUTPUT_H