#include "lz_html_output.h"

#include <QDebug>
#include <QObject>
#include <sstream>

/**
 * 兰州项目限界数据图表输出
 * HTML格式输出限界图表
 * @see LzOutput 父类
 * @author 范翔
 * @date 2014-06-12
 * @version 1.0.0
 */
LzHtmlOutput::LzHtmlOutput () {}
	
LzHtmlOutput::~LzHtmlOutput () {}
	
int LzHtmlOutput::outputSingleSection(TunnelDataModel * inputbasicdata, QString collectdate, SectionData & inputdata, QString templatepath, QString outputfile, QString insertimgpath) 
{
    QString templatename = "SingleSectionTemplate.html";
    int ret = copy(templatepath, templatename, outputfile);
    if (ret != 0)
        return ret;

    // 插入图片路径不存在
    if (!QFile(insertimgpath).exists())
        return 3;

    // 加载模板文件
    QByteArray tmpba = (templatepath + "/" + templatename).toLocal8Bit();
    infile = ifstream(tmpba.constData());
    tmpba = outputfile.toLocal8Bit();
    outfile = ofstream(tmpba.constData());
	
	outfile.clear();
    if (!infile)
    {
        return 1;
    }
    if (!outfile)
    {
        return 2;
    }

    using std::string;

    // 输出限界表到html
    string word;
    string url_linenamestr = "";
    string url_startstation = "";
    string url_endstationstr = "";
    string url_tunnelnamestr = "";
    string collectdate1 = "";
    __int64 startdistance = 0;
    string startdistance1 = "0";
    __int64 tunnellength = 0;
    string tunnellength1 = "0";
    float centerlength = 0;
    string centerlength1 = "0";
    if (inputbasicdata != NULL)
    {
        url_linenamestr = string_To_UTF8(inputbasicdata->getLinename());//将string类型的变量转换为utf-8编码，这样Qt中QTextEdit控件才能显示
        url_startstation = string_To_UTF8(inputbasicdata->getLineStartStation());
        url_endstationstr = string_To_UTF8(inputbasicdata->getLineEndStation());
        url_tunnelnamestr = string_To_UTF8(inputbasicdata->getName());
        tmpba = collectdate.toLocal8Bit();
        collectdate1 = tmpba.constData();

        // 起始里程
        startdistance = inputbasicdata->getStartPoint();
        char sss[10];
        sprintf(sss, "%ld", startdistance);
        startdistance1 = sss;
        // 计算隧道长度
        tunnellength = inputbasicdata->getEndPoint() - inputbasicdata->getStartPoint();
        if (tunnellength < 0)
            tunnellength = 0 - tunnellength;
        sprintf(sss,"%ld",tunnellength);
        tunnellength1 = sss;
        // 计算中心里程 隧道中心点的里程
        centerlength = 1.0 * (inputbasicdata->getEndPoint() + inputbasicdata->getStartPoint()) / 2000;  
        sprintf(sss,"%f",centerlength);
        centerlength1 = sss;
    }

    // 限界图路径
    tmpba = insertimgpath.toLocal8Bit();
    string url_imgaddr = string_To_UTF8(tmpba.constData());
    tmpba = (templatepath + "/red.jpg").toLocal8Bit();
    string url_imgred = string_To_UTF8(tmpba.constData());
    tmpba = (templatepath + "/blue.jpg").toLocal8Bit();
    string url_imgblue = string_To_UTF8(tmpba.constData());

    while(getline(infile, word))
    {
        // 找到插入位置
        if (word.compare("&!herehere!") == 0)
        {
            CurveType type = inputdata.getType();
            long long pos = inputdata.getMile();
            int radius = inputdata.getRadius();

            //inputdata.showMaps();

            // map反向遍历，正向为高度从小到大，插入图表是需要高度从大到小
            std::map<int, item>::reverse_iterator its = inputdata.getMaps().rbegin();

            while (its != inputdata.getMaps().rend())
            {
                outfile << "<tr>\n";

                std::pair<int, item> pairs = (*its);
                string leftval = "";
                string rightval = "";
                if (pairs.second.left >= 0)
                {
                    stringstream ss;
                    ss << pairs.second.left;
                    ss >> leftval;
                }
                if (pairs.second.right >= 0)
                {
                    stringstream ss;
                    ss << pairs.second.right;
                    ss >> rightval;
                }

                outfile << "<td>" << pairs.first << "</td>\n";

                if (type == CurveType::Curve_Straight)
                    outfile << "<td>" << string_To_UTF8(leftval) << "</td>\n<td>" << string_To_UTF8(rightval) << "</td>\n";
                else
                    outfile << "<td></td>\n<td></td>\n";

                if (type == CurveType::Curve_Left) // 折减后尺寸 TODO
                    outfile << "<td>" << string_To_UTF8(leftval) << "</td>\n<td>" << string_To_UTF8(rightval) << "</td>\n<td>" << "" << "</td>\n<td>" << "" << "</td>\n";
                else
                    outfile << "<td></td>\n<td></td>\n<td></td>\n<td></td>\n";

                if (type == CurveType::Curve_Right) // 折减后尺寸 TODO
                    outfile << "<td>" << string_To_UTF8(leftval) << "</td>\n<td>" << string_To_UTF8(rightval) << "</td>\n<td>" << "" << "</td>\n<td>" << "" << "</td>\n";
                else
                    outfile << "<td></td>\n<td></td>\n<td></td>\n<td></td>\n";

                // 此处写要插入的内容
                outfile << "</tr>\n";

                its++;
            }
            // 删除插入位置标记
            replace_all(word,"&!herehere!", "");
        }

        // <150高度手动输入？ ///TODO
        
        // 写入隧道名
        replace_all(word,"&TunnelName", url_tunnelnamestr.c_str());
        // 线路名称
        replace_all(word,"&LineName", url_linenamestr.c_str());
        // 起讫站名
        replace_all(word,"&StartStationName", url_endstationstr.c_str());
        // 面向站名
        replace_all(word,"&EndStationName", url_endstationstr.c_str());
        // 起讫里程
        replace_all(word,"&StartLength", startdistance1);
        // 采集日期
        replace_all(word,"&Time", collectdate1);
        // 隧道长度
		replace_all(word,"&TunnelLength", tunnellength1);
        // 中心里程
        replace_all(word,"&CenterLength", centerlength1);

        // 最小曲线半径///TODO
        replace_all(word, "&MinCurveRadius", tunnellength1);
        // 最大外轨超高///TODO
        replace_all(word, "&MaxWaiGui", tunnellength1);
        // 最低接触网高度///TODO
        replace_all(word, "&MinJieChu", tunnellength1);
        // 线路中心线上方最低净高mm///TODO
        replace_all(word, "&MinJingGao", tunnellength1);
    
        // 限界图片路径
        replace_all(word, "&ImagePath", url_imgaddr);
        replace_all(word, "&ImageRedPath", url_imgred);
        replace_all(word, "&ImageBluePath", url_imgblue);

        outfile << word << endl;	
    }
    infile.close();
    outfile.close();

    return 0;
}

int LzHtmlOutput::outputSingleTunnel(TunnelDataModel * inputbasicdata, ClearanceSingleTunnel* inputdata, QString templatepath, QString outputfile, QString insertimgpath) 
{
    QString templatename = "SingleTunnelTemplate.html";
    int ret = copy(templatepath, templatename, outputfile);
    if (ret != 0)
        return ret;

    // 插入图片路径不存在
    if (!QFile(insertimgpath).exists())
        return 3;

    // 加载模板文件
    QByteArray tmpba = (templatepath + "/" + templatename).toLocal8Bit();
    ifstream infile(tmpba.constData());
    tmpba = outputfile.toLocal8Bit();
    ofstream outfile(tmpba.constData());
	
    // 采集日期
    QString tunnelinfo = inputdata->getTaskTunnelInfo();
    QStringList strlist = tunnelinfo.split("_");
    if (strlist.length() < 2)
        return -1;
    QString collectdate = strlist.at(1);

	outfile.clear();
    if (!infile)
    {
        return 1;
    }
    if (!outfile)
    {
        return 2;
    }

    // 输出限界表到html
    string word;
    string url_linenamestr = "";
    string url_startstation = "";
    string url_endstationstr = "";
    string url_tunnelnamestr = "";
    string collectdate1 = "";
    __int64 startdistance = 0;
    string startdistance1 = "0";
    __int64 tunnellength = 0;
    string tunnellength1 = "0";
    float centerlength = 0;
    string centerlength1 = "0";
    if (inputbasicdata != NULL)
    {
        url_linenamestr = string_To_UTF8(inputbasicdata->getLinename());//将string类型的变量转换为utf-8编码，这样Qt中QTextEdit控件才能显示
        url_startstation = string_To_UTF8(inputbasicdata->getLineStartStation());
        url_endstationstr = string_To_UTF8(inputbasicdata->getLineEndStation());
        url_tunnelnamestr = string_To_UTF8(inputbasicdata->getName());
        tmpba = collectdate.toLocal8Bit();
        collectdate1 = tmpba.constData();

        // 起始里程
        startdistance = inputbasicdata->getStartPoint();
        char sss[10];
        sprintf(sss, "%ld", startdistance);
        startdistance1 = sss;
        // 计算隧道长度
        tunnellength = inputbasicdata->getEndPoint() - inputbasicdata->getStartPoint();
        if (tunnellength < 0)
            tunnellength = 0 - tunnellength;
        sprintf(sss,"%ld",tunnellength);
        tunnellength1 = sss;
        // 计算中心里程 隧道中心点的里程
        centerlength = 1.0 * (inputbasicdata->getEndPoint() + inputbasicdata->getStartPoint()) / 2000;  
        sprintf(sss,"%f",centerlength);
        centerlength1 = sss;
    }

    // 起讫站名
    tmpba = inputdata->getQiQiZhanMing().toLocal8Bit();
    string url_qiqizhanming = string_To_UTF8(tmpba.constData());

    // 最大外轨超高
    tmpba = inputdata->getWaiGuiChaoGao().toLocal8Bit();
    string url_waiguichaogao = string_To_UTF8(tmpba.constData());
    
    // 最低接触网高度
    tmpba = inputdata->getJieChuWangGaoDu().toLocal8Bit();
    string url_jiechuwanggaodu = string_To_UTF8(tmpba.constData());


    // 限界图路径
    tmpba = insertimgpath.toLocal8Bit();
    string url_imgaddr = string_To_UTF8(tmpba.constData());
    tmpba = (templatepath + "/red.jpg").toLocal8Bit();
    string url_imgred = string_To_UTF8(tmpba.constData());
    tmpba = (templatepath + "/blue.jpg").toLocal8Bit();
    string url_imgblue = string_To_UTF8(tmpba.constData());

    while(getline(infile, word))
    {
        // 找到插入位置
        if (word.compare("&!herehere!") == 0)
        {
            ClearanceData & datas = inputdata->getClearanceStraightData();
            ClearanceData & datal = inputdata->getClearanceLeftData();
            ClearanceData & datar = inputdata->getClearanceRightData();

            //datas.showMaps();
            //datal.showMaps();
            //datar.showMaps();

            // 临时变量
            bool hass = inputdata->getHasStraight();
            bool hasl = inputdata->getHasLeft();
            bool hasr = inputdata->getHasRight();

            // map反向遍历，正向为高度从小到大，插入图表是需要高度从大到小
            std::map<int, ClearanceItem>::reverse_iterator its = datas.getMaps().rbegin();
            std::map<int, ClearanceItem>::reverse_iterator itl = datal.getMaps().rbegin();
            std::map<int, ClearanceItem>::reverse_iterator itr = datar.getMaps().rbegin();

            while (its != datas.getMaps().rend())
            {
                outfile << "<tr>\n";

                std::pair<int, ClearanceItem> pairs = (*its);
                std::pair<int, ClearanceItem> pairl = (*itl);
                std::pair<int, ClearanceItem> pairr = (*itr);
                
                string s_leftval = "";
                string s_rightval = "";
                string s_leftpos = "";
                string s_rightpos = "";
                if (pairs.second.leftval >= 0)
                {
                    stringstream ss;
                    ss << pairs.second.leftval;
                    ss >> s_leftval;
                    ss << pairs.second.leftpos;
                    ss >> s_leftpos;
                }
                if (pairs.second.rightval >= 0)
                {
                    stringstream ss;
                    ss << pairs.second.rightval;
                    ss >> s_rightval;
                    ss << pairs.second.rightpos;
                    ss >> s_rightpos;
                }

                string l_leftval = "";
                string l_rightval = "";
                string l_leftpos = "";
                string l_rightpos = "";
                string l_leftradius = "";
                string l_rightradius = "";
                if (pairl.second.leftval >= 0)
                {
                    stringstream ss;
                    ss << pairl.second.leftval;
                    ss >> l_leftval;
                    ss << pairl.second.leftpos;
                    ss >> l_leftpos;
                    ss << pairl.second.leftradius;
                    ss >> l_leftradius;
                }
                if (pairl.second.rightval >= 0)
                {
                    stringstream ss;
                    ss << pairl.second.rightval;
                    ss >> l_rightval;
                    ss << pairl.second.rightpos;
                    ss >> l_rightpos;
                    ss << pairl.second.rightradius;
                    ss >> l_rightradius;
                }

                string r_leftval = "";
                string r_rightval = "";
                string r_leftpos = "";
                string r_rightpos = "";
                string r_leftradius = "";
                string r_rightradius = "";
                if (pairr.second.leftval >= 0)
                {
                    stringstream ss;
                    ss << pairr.second.leftval;
                    ss >> r_leftval;
                    ss << pairr.second.leftpos;
                    ss >> r_leftpos;
                    ss << pairr.second.leftradius;
                    ss >> r_leftradius;
                }
                if (pairr.second.rightval >= 0)
                {
                    stringstream ss;
                    ss << pairr.second.rightval;
                    ss >> r_rightval;
                    ss << pairr.second.rightpos;
                    ss >> r_rightpos;
                    ss << pairr.second.rightradius;
                    ss >> r_rightradius;
                }

                outfile << "<td>" << pairs.first << "</td>\n";

                if (hass)
                    outfile << "<td>" << string_To_UTF8(s_leftval) << "</td>\n<td>" << string_To_UTF8(s_rightval) << "</td>\n<td>" << string_To_UTF8(s_leftpos) << "</td>\n<td>" << string_To_UTF8(s_rightpos) << "</td>\n";
                else
                    outfile << "<td></td>\n<td></td>\n<td></td>\n<td></td>\n";

                if (hasl) // 折减后尺寸
                    outfile << "<td>" << string_To_UTF8(l_leftval) << "</td>\n<td>" << string_To_UTF8(l_rightval) << "</td>\n<td>" << string_To_UTF8(l_leftradius) << "</td>\n<td>" <<  string_To_UTF8(l_rightradius) 
                                                                                                        << "</td>\n<td>" << string_To_UTF8(l_leftpos) << "</td>\n<td>" << string_To_UTF8(r_rightpos) << "</td>\n";
                else
                    outfile << "<td></td>\n<td></td>\n<td></td>\n<td></td>\n<td></td>\n<td></td>\n";

                if (hasr) // 折减后尺寸
                    outfile << "<td>" << string_To_UTF8(r_leftval) << "</td>\n<td>" << string_To_UTF8(r_rightval) << "</td>\n<td>" << string_To_UTF8(r_leftradius) << "</td>\n<td>" << string_To_UTF8(r_rightradius) 
                                                                                                        << "</td>\n<td>" << string_To_UTF8(r_leftpos) << "</td>\n<td>" << string_To_UTF8(r_rightpos) << "</td>\n";
                else
                    outfile << "<td></td>\n<td></td>\n<td></td>\n<td></td>\n<td></td>\n<td></td>\n";

                // 此处写要插入的内容
                outfile << "</tr>\n";

                its++;
                itl++;
                itr++;
            }
            // 删除插入位置标记
            replace_all(word,"&!herehere!", "");
        }

        // <150高度手动输入？ ///TODO
        
        // 写入隧道名
        replace_all(word,"&TunnelName", url_tunnelnamestr.c_str());
        // 线路名称
        replace_all(word,"&LineName", url_linenamestr.c_str());
        // 起讫站名
        replace_all(word,"&StartStationName", url_qiqizhanming.c_str());
        // 面向站名
        replace_all(word,"&EndStationName", url_endstationstr.c_str());
        // 起讫里程
        replace_all(word,"&StartLength", startdistance1);
        // 采集日期
        replace_all(word,"&Time", collectdate1);
        // 隧道长度
		replace_all(word,"&TunnelLength", tunnellength1);
        // 中心里程
        replace_all(word,"&CenterLength", centerlength1);

        // 最小曲线半径///TODO
        replace_all(word, "&MinCurveRadius", tunnellength1);
        // 最大外轨超高
        replace_all(word, "&MaxWaiGui", url_waiguichaogao);
        // 最低接触网高度
        replace_all(word, "&MinJieChu", url_jiechuwanggaodu);
        // 线路中心线上方最低净高mm///TODO
        replace_all(word, "&MinJingGao", tunnellength1);
    
        // 限界图片路径
        replace_all(word, "&ImagePath", url_imgaddr);
        replace_all(word, "&ImageRedPath", url_imgred);
        replace_all(word, "&ImageBluePath", url_imgblue);

        outfile << word << endl;	
    }
    infile.close();
    outfile.close();

    return 0;
}

int LzHtmlOutput::outputMultiTunnels(ClearanceMultiTunnels * inputdata, QString templatepath, QString outputfile, QString insertimgpath) 
{
    QString templatename = "MultiTunnelsTemplate.html";
    int ret = copy(templatepath, templatename, outputfile);
    if (ret != 0)
        return ret;

    // 插入图片路径不存在
    if (!QFile(insertimgpath).exists())
        return 3;

    // 加载模板文件
    QByteArray tmpba = (templatepath + "/" + templatename).toLocal8Bit();
    ifstream infile(tmpba.constData());
    tmpba = outputfile.toLocal8Bit();
    ofstream outfile(tmpba.constData());
	
    // 采集日期
    QStringListModel * tunnelsmodel = inputdata->getTunnelsNames();
    QModelIndex index = tunnelsmodel->index(0,0);
    QString tunnelinfo = index.data().toString();//这两行代码获取QStringListModel中第一行的值
    QStringList strlist = tunnelinfo.split("_");
    if (strlist.length() < 2)
        return -1;
    QString collectdate = strlist.at(1);

	outfile.clear();
    if (!infile)
    {
        return 1;
    }
    if (!outfile)
    {
        return 2;
    }
    tmpba = tunnelinfo.toLocal8Bit();//TODO
    // 输出限界表到html
    string word;
    string url_linenamestr = string_To_UTF8(tmpba.constData());//将string类型的变量转换为utf-8编码，这样Qt中QTextEdit控件才能显示
    string url_startstation = string_To_UTF8(tmpba.constData());//TODO
    string url_endstationstr = string_To_UTF8(tmpba.constData());//TODO
    string url_tunnelnamestr = string_To_UTF8(tmpba.constData());//TODO
    tmpba = collectdate.toLocal8Bit();
    string collectdate1(tmpba.constData());

    char sss[10];
    // 计算隧道长度
    _int64 tunnellength = 0;//TODO
    if (tunnellength < 0)
        tunnellength = 0 - tunnellength;
    sprintf(sss,"%ld",tunnellength);
    string tunnellength1(sss);
    // 计算中心里程 隧道中心点的里程数？////TODO
    float centerlength = 0;  //TODO
    sprintf(sss,"%f",centerlength);
    string centerlength1(sss);

    // 最大外轨超高
    tmpba = inputdata->getWaiGuiChaoGao().toLocal8Bit();
    string url_waiguichaogao = string_To_UTF8(tmpba.constData());
    
    // 最低接触网高度
    tmpba = inputdata->getJieChuWangGaoDu().toLocal8Bit();
    string url_jiechuwanggaodu = string_To_UTF8(tmpba.constData());

    // 限界图路径
    tmpba = insertimgpath.toLocal8Bit();
    string url_imgaddr = string_To_UTF8(tmpba.constData());
    tmpba = (templatepath + "/red.jpg").toLocal8Bit();
    string url_imgred = string_To_UTF8(tmpba.constData());
    tmpba = (templatepath + "/blue.jpg").toLocal8Bit();
    string url_imgblue = string_To_UTF8(tmpba.constData());

    while(getline(infile, word))
    {
        // 找到插入位置
        if (word.compare("&!herehere!") == 0)
        {
            ClearanceData & datas = inputdata->getClearanceStraightData();
            ClearanceData & datal = inputdata->getClearanceLeftData();
            ClearanceData & datar = inputdata->getClearanceRightData();

            //datas.showMaps();
            //datal.showMaps();
            //datar.showMaps();

            // 临时变量
            int hass = inputdata->getNumOfStraight();
            int hasl = inputdata->getNumOfLeft();
            int hasr = inputdata->getNumOfRight();

            // map反向遍历，正向为高度从小到大，插入图表是需要高度从大到小
            std::map<int, ClearanceItem>::reverse_iterator its = datas.getMaps().rbegin();
            std::map<int, ClearanceItem>::reverse_iterator itl = datal.getMaps().rbegin();
            std::map<int, ClearanceItem>::reverse_iterator itr = datar.getMaps().rbegin();

            while (its != datas.getMaps().rend())
            {
                outfile << "<tr>\n";

                std::pair<int, ClearanceItem> pairs = (*its);
                std::pair<int, ClearanceItem> pairl = (*itl);
                std::pair<int, ClearanceItem> pairr = (*itr);
                
                string s_leftval = "";
                string s_rightval = "";
                string s_leftpos = "";
                string s_rightpos = "";
                if (pairs.second.leftval >= 0)
                {
                    stringstream ss;
                    ss << pairs.second.leftval;
                    ss >> s_leftval;
                    ss << pairs.second.leftpos;
                    ss >> s_leftpos;
                }
                if (pairs.second.rightval >= 0)
                {
                    stringstream ss;
                    ss << pairs.second.rightval;
                    ss >> s_rightval;
                    ss << pairs.second.rightpos;
                    ss >> s_rightpos;
                }

                string l_leftval = "";
                string l_rightval = "";
                string l_leftpos = "";
                string l_rightpos = "";
                string l_leftradius = "";
                string l_rightradius = "";
                if (pairl.second.leftval >= 0)
                {
                    stringstream ss;
                    ss << pairl.second.leftval;
                    ss >> l_leftval;
                    ss << pairl.second.leftpos;
                    ss >> l_leftpos;
                    ss << pairl.second.leftradius;
                    ss >> l_leftradius;
                }
                if (pairl.second.rightval >= 0)
                {
                    stringstream ss;
                    ss << pairl.second.rightval;
                    ss >> l_rightval;
                    ss << pairl.second.rightpos;
                    ss >> l_rightpos;
                    ss << pairl.second.rightradius;
                    ss >> l_rightradius;
                }

                string r_leftval = "";
                string r_rightval = "";
                string r_leftpos = "";
                string r_rightpos = "";
                string r_leftradius = "";
                string r_rightradius = "";
                if (pairr.second.leftval >= 0)
                {
                    stringstream ss;
                    ss << pairr.second.leftval;
                    ss >> r_leftval;
                    ss << pairr.second.leftpos;
                    ss >> r_leftpos;
                    ss << pairr.second.leftradius;
                    ss >> r_leftradius;
                }
                if (pairr.second.rightval >= 0)
                {
                    stringstream ss;
                    ss << pairr.second.rightval;
                    ss >> r_rightval;
                    ss << pairr.second.rightpos;
                    ss >> r_rightpos;
                    ss << pairr.second.rightradius;
                    ss >> r_rightradius;
                }
                
                outfile << "<td>" << pairs.first << "</td>\n";

                if (hass)
                {
                    outfile << "<td>" << s_leftval << "</td>\n<td>" << s_rightval 
                            << "</td>\n<td>" << toTunnelPosString(pairs.second.leftpos, pairs.second.lefttunnelid, s_leftval) << "</td>\n<td>" << toTunnelPosString(pairs.second.rightpos, pairs.second.righttunnelid, s_rightval) << "</td>\n";

                }
                else
                    outfile << "<td></td>\n<td></td>\n<td></td>\n<td></td>\n";

                if (hasl) // 折减后尺寸
                {
                    outfile << "<td>" << l_leftval << "</td>\n<td>" << l_rightval << "</td>\n<td>" << l_leftradius << "</td>\n<td>" << l_rightradius 
                            << "</td>\n<td>" << toTunnelPosString(pairl.second.leftpos, pairl.second.lefttunnelid, l_leftval) << "</td>\n<td>" << toTunnelPosString(pairl.second.rightpos, pairl.second.righttunnelid, l_rightval) << "</td>\n";
                }
                else
                    outfile << "<td></td>\n<td></td>\n<td></td>\n<td></td>\n<td></td>\n<td></td>\n";

                if (hasr) // 折减后尺寸
                {
                    outfile << "<td>" << r_leftval << "</td>\n<td>" << r_rightval << "</td>\n<td>" << r_leftradius << "</td>\n<td>" << r_rightradius 
                            << "</td>\n<td>" << toTunnelPosString(pairr.second.leftpos, pairr.second.lefttunnelid, r_leftval) << "</td>\n<td>" << toTunnelPosString(pairr.second.rightpos, pairr.second.righttunnelid, r_rightval) << "</td>\n";
                }
                else
                    outfile << "<td></td>\n<td></td>\n<td></td>\n<td></td>\n<td></td>\n<td></td>\n";

                // 此处写要插入的内容
                outfile << "</tr>\n";

                its++;
                itl++;
                itr++;
            }
            // 删除插入位置标记
            replace_all(word,"&!herehere!", "");
        }

        // <150高度手动输入？ ///TODO
        
        // 桥隧总数
        replace_all(word,"&NumTotal", centerlength1);///TODO
        // 桥梁数
        replace_all(word,"&NumOfBridges", centerlength1);///TODO
        // 隧道数
        replace_all(word,"&NumOfTunnels", centerlength1);///TODO

        // 写入隧道名
        replace_all(word,"&TunnelName", url_tunnelnamestr.c_str());
        // 线路名称
        replace_all(word,"&LineName", url_linenamestr.c_str());
        // 起讫站名
        replace_all(word,"&StartStationName", url_endstationstr.c_str());
        // 面向站名
        replace_all(word,"&EndStationName", url_endstationstr.c_str());
        // 起讫里程
        replace_all(word,"&StartLength", centerlength1);///TODO
        // 采集日期
        replace_all(word,"&Time", collectdate1);
        // 隧道长度
		replace_all(word,"&TunnelLength", tunnellength1);
        // 中心里程
        replace_all(word,"&CenterLength", centerlength1);

        // 最小曲线半径///TODO
        replace_all(word, "&MinCurveRadius", tunnellength1);
        // 最大外轨超高
        replace_all(word, "&MaxWaiGui", url_waiguichaogao);
        // 最低接触网高度
        replace_all(word, "&MinJieChu", url_jiechuwanggaodu);
        // 线路中心线上方最低净高mm///TODO
        replace_all(word, "&MinJingGao", tunnellength1);
    
        // 限界图片路径
        replace_all(word, "&ImagePath", url_imgaddr);
        replace_all(word, "&ImageRedPath", url_imgred);
        replace_all(word, "&ImageBluePath", url_imgblue);

        outfile << word << endl;	
    }
    infile.close();
    outfile.close();

    return 0;
}

string& LzHtmlOutput::replace_all(string& str, const string& old_value, const string& new_value)
{
    while(true)
    {
        string::size_type pos(0);
        if( (pos=str.find(old_value))!=string::npos )//pos如果等于string::npos表示查找没有匹配 ，string 类将 npos 定义为保证大于任何有效下标的值
                str.replace(pos,old_value.length(),new_value);
        else break;
    }
    return str;
}

std::string LzHtmlOutput::string_To_UTF8(const std::string &str)
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