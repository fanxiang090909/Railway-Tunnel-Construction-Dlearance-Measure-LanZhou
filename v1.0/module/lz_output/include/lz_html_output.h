#ifndef LZ_HTML_OUTPUT_H
#define LZ_HTML_OUTPUT_H

#include "lz_output.h"

// UTF 编码转换需要
#include <Windows.h>

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

/**
 * 兰州项目限界数据图表输出
 * HTML格式输出限界图表
 * @see LzOutput 父类
 * @author 范翔
 * @date 2014-06-12
 * @version 1.0.0
 */
class LzHtmlOutput : public LzOutput
{
public:
    LzHtmlOutput ();
	
    virtual ~LzHtmlOutput ();

    int outputSingleSection(TunnelDataModel * inputbasicdata, QString collectdate, SectionData & inputdata, QString templatepath, QString outputfile, QString insertimgpath);

    int outputSingleTunnel(TunnelDataModel * inputbasicdata, ClearanceSingleTunnel * inputdata, QString templatepath, QString outputfile, QString insertimgpath);

    int outputMultiTunnels(ClearanceMultiTunnels * inputdata, QString templatepath, QString outputfile, QString insertimgpath);

private:

    string& replace_all(string& str, const string& old_value, const string& new_value);
    
    std::string string_To_UTF8(const std::string& str);

    ifstream infile;

    ofstream outfile;
};

#endif // LZ_HTML_OUTPUT_H