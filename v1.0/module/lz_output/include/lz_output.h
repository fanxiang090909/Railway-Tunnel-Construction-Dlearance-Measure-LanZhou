#ifndef LZ_OUTPUT_H
#define LZ_OUTPUT_H

// UTF 编码转换需要
#include <Windows.h>

#include <iostream>
#include <fstream>
#include <string>

#include "tunneldatamodel.h"
#include "clearance_tunnel.h"
#include "clearance_tunnels.h"
#include "daotunnel.h"

#include <QFileInfo>
#include <QDir>
#include <QDebug>

using namespace std;

/**
 * 限界表的列头枚举类型
 */
enum LzOutputTableColumn {
    Height = 0,
    Straight_Left_Val = 1,
    Straight_Right_Val = 2,
    Straight_Left_Pos = 3,
    Straight_Right_Pos = 4,
    LeftCurve_Left_Val = 5,
    LeftCurve_Right_Val = 6,
    LeftCurve_Left_Radius = 7,
    LeftCurve_Right_Radius = 8,        
    LeftCurve_Left_Pos = 9,
    LeftCurve_Right_Pos = 10,
    RightCurve_Left_Val = 11,
    RightCurve_Right_Val = 12,
    RightCurve_Left_Radius = 13,
    RightCurve_Right_Radius = 14,        
    RightCurve_Left_Pos = 15,
    RightCurve_Right_Pos = 16,
};

/**
 * 兰州项目限界数据图表输出
 * @author 范翔
 * @date 2014-06-12
 * @version 1.0.0
 */
class LzOutput
{

public:
    LzOutput() { };
	
    virtual ~LzOutput() { };

    /**
     * 模板拷贝到新文件处并更换文件名
     * @return 0 拷贝模板成功到目标文件
     *         1 模板文件不存在
     *         2 模板文件存在，但目标文件不能打开
     */
    int copy(QString templatepath, QString templatename, QString outputfile)
    {
        // 如果模板文件不存在
        if (!QFile::exists(templatepath + "/" + templatename))
            return 1;
        QDir dir;
        bool ret = dir.exists(outputfile);
        // 如果存在输出文件，删除原文件
        if (ret) 
        {
            ret = dir.remove(outputfile);
        }
        // 如果为false，原因可能是正在使用原有文件，无法删除
        ret = QFile::copy(templatepath + "/" + templatename, outputfile);
        if (ret)
            return 0;
        else
            return 2;
    };

    std::string string_To_UTF8(const std::string &str)
    {

        int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);

        wchar_t * pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然会出现尾巴
        ZeroMemory(pwBuf, nwLen * 2 + 2);

        ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);

        int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

        char * pBuf = new char[nLen + 1];
        ZeroMemory(pBuf, nLen + 1);

        ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

        std::string retStr(pBuf);

        delete []pwBuf;
        delete []pBuf;

        pwBuf = NULL;
        pBuf = NULL;

        return retStr;
    } 

    /**
     * 多隧道区段综合转换为输出控制点位置字符串
     */
    string toTunnelPosString(int pos, int tunnelid, string valstr)
    {
         // 如果值为-1，说明无数据，控制点字段不输出任何内容
         if (valstr.compare("") == 0)
             return "";

         QString tmpstr = TunnelDAO::getTunnelDAOInstance()->getOneTunnelName(tunnelid);
         if (tmpstr.compare("") == 0)
             tmpstr = QString("%1,%2").arg(tunnelid).arg(pos);
         else
             tmpstr = tmpstr + QString(",%1").arg(pos);
         QByteArray tmpba = tmpstr.toLocal8Bit();
         return string_To_UTF8(tmpba.constData());
    };

    /**
     * 将正斜杠路径转换为反斜杠路径 主要针对AxObject操作EXCEL输出，Excel的SaveAs和AddPicture函数
     * 例如 "D:/data.xls" -> "D:\\data.xls"
     * @param inputstr 输入string
     * @return 输出string
     */
    QString toBackSlashStr(QString inputstr)
    {
        QStringList slist = inputstr.split("/");
        QString retstr;
        
        if (slist.length() >= 1)
            retstr = slist.at(0);
        else
            return "";

        for (int i = 1; i < slist.length(); i++)
        {
            retstr += "\\" + slist.at(i);
        }
        //qDebug() << "input string:" << inputstr << ", output string:" << retstr;
        return retstr;
    }

    /**
     * 生成输出文件
     * @return 0 生成成功
     *         1 模板文件不存在
     *         2 模板文件存在，但目标文件不能打开
     *         3 嵌入图片文件不存在
     *         -1 未知错误
     */
    virtual int outputSingleSection(TunnelDataModel * inputbasicdata, QString collectdate, SectionData & inputdata, QString templatepath, QString outputfile, QString insertimgpath) = 0;

    /**
     * 生成输出文件
     * @return 0 生成成功
     *         1 模板文件不存在
     *         2 模板文件存在，但目标文件不能打开
     *         3 嵌入图片文件不存在
     *         -1 未知错误
     */
    virtual int outputSingleTunnel(TunnelDataModel * inputbasicdata, ClearanceSingleTunnel* inputdata, QString templatepath, QString outputfile, QString insertimgpath) = 0;

    /**
     * 生成输出文件
     * @return 0 生成成功
     *         1 模板文件不存在
     *         2 模板文件存在，但目标文件不能打开
     *         3 嵌入图片文件不存在
     *         -1 未知错误
     */
    virtual int outputMultiTunnels(ClearanceMultiTunnels * inputdata, QString templatepath, QString outputfile, QString insertimgpath) = 0;

};

#endif // LZ_OUTPUT_H