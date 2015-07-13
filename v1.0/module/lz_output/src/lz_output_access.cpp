#include "lz_output_access.h"

/**
 * 兰州项目限界数据图表输出访问对象
 * @author 范翔
 * @date 2014-06-12
 * @version 1.0.0
 */
LzOutputAccess::LzOutputAccess() 
{
    // 默认当前目录
    templatepath = "./";
    isinit = false;
    output = NULL;
}
	
LzOutputAccess::~LzOutputAccess() 
{ 
    if (output != NULL) 
        delete output;
}

void LzOutputAccess::initOutput(OutputType initType) 
{
    if (output != NULL) 
    {
        delete output;
        isinit = false;
        output = NULL;
    }
    switch (initType)
    {
	    case OutputInHTML :
        {
		    output = new LzHtmlOutput(); 
            isinit = true;
            break;
        }
        case OutputInExcel :
	    {	output = new LzExcelOutput(); 
            isinit = true;
            break;
        }
        default: break;
    }
    isinit = false;
}

void LzOutputAccess::setTemplatePath(QString newpath) { templatepath = newpath; }

int LzOutputAccess::outputSingleSection(TunnelDataModel * inputbasicdata, QString collectdate, SectionData & inputdata, QString outputfile, QString insertimgpath)
{
    if (output != NULL)
        return output->outputSingleSection(inputbasicdata, collectdate, inputdata, templatepath, outputfile, insertimgpath);
    return -1;
}

int LzOutputAccess::outputSingleTunnel(TunnelDataModel * inputbasicdata, ClearanceSingleTunnel * inputdata, QString outputfile, QString insertimgpath)
{
    if (output != NULL)
        return output->outputSingleTunnel(inputbasicdata, inputdata, templatepath, outputfile, insertimgpath);
    return -1;
}

int LzOutputAccess::outputMultiTunnels(ClearanceMultiTunnels * inputdata, QString outputfile, QString insertimgpath)
{
    if (output != NULL)
        return output->outputMultiTunnels(inputdata, templatepath, outputfile, insertimgpath);
	return -1;
}