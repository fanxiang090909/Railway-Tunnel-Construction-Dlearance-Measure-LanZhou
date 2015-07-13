#ifndef LZ_OUTPUT_ACCESS_H
#define LZ_OUTPUT_ACCESS_H

#include <iostream>
#include <fstream>
#include <string>

#include "lz_output.h"
#include "lz_html_output.h"
#include "lz_excel_output.h"

using namespace std;

/**
 * 兰州项目限界数据图表输出访问对象
 * 策略设计模式
 * @author 范翔
 * @date 2014-06-12
 * @version 1.0.0
 */
class LzOutputAccess
{
public:
    enum OutputType {
        OutputInHTML = 0,
        OutputInExcel = 1,
    };

    LzOutputAccess();
	
    ~LzOutputAccess();
	
    /**
     * 初始化输出类
     * @param initType 输出类别，excel还是html
     */
    void initOutput(OutputType initType);
    
    /**
     * 设置模板路径
     */
    void setTemplatePath(QString newpath);

    /**
     * 输出单幅断面。单帧限界图表
     * @param inputbasicdata 该断面所在隧道的基础信息，用于表头填写
     * @param collectdate 采集日期
     * @param inputdata 断面限界数据
     * @param outputfile 文件输出路径（excel或是html）
     * @param insertimgpath 插入的限界图片文件路径
     */
    int outputSingleSection(TunnelDataModel * inputbasicdata, QString collectdate, SectionData & inputdata, QString outputfile, QString insertimgpath);
    
    /**
     * 输出单个隧道限界图表
     * @param inputbasicdata 该断面所在隧道的基础信息，用于表头填写
     * @param inputdata 单个隧道综合结果限界数据
     * @param outputfile 文件输出路径（excel或是html）
     * @param insertimgpath 插入的限界图片文件路径
     */
    int outputSingleTunnel(TunnelDataModel * inputbasicdata, ClearanceSingleTunnel* inputdata, QString outputfile, QString insertimgpath);

    /**
     * 输出多个隧道综合结果限界图表
     * @param inputdata 多个隧道综合结果限界数据
     * @param outputfile 文件输出路径（excel或是html）
     * @param insertimgpath 插入的限界图片文件路径
     */
    int outputMultiTunnels(ClearanceMultiTunnels * inputdata, QString outputfile, QString insertimgpath);

private:
    // 
    LzOutput * output;
	
    // 输入模板路径
    QString templatepath;

    bool isinit;
	
};

#endif // LZ_OUTPUT_ACCESS_H