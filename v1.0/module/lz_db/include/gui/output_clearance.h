#ifndef OUTPUT_CLEARANCE_H
#define OUTPUT_CLEARANCE_H

#include <QWidget>
#include <QSqlTableModel>
#include "setting_client.h"

#include <QDateTime>
#include <QScrollArea>
#include <string>
#include <fstream>

#include "fancybrowser.h"
#include "textedit.h"
#include "lz_output_access.h"

#include "drawimage.h"
#include "clearance_item.h"
#include <iostream>

using namespace std;

namespace Ui {
    class OutputClearanceWidget;
}

/**
 * 隧道限界图表输出界面类声明
 *
 * @author 熊雪
 * @author 范翔
 * @version 1.0.0
 * @date 20140609
 */
class OutputClearanceWidget : public QWidget
{
    Q_OBJECT

public:
    explicit OutputClearanceWidget(QWidget *parent = 0);
    ~OutputClearanceWidget();

private:

    std::string OutputClearanceWidget::string_To_UTF8(const std::string &str)
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
     * 初始化PDF预览接口
     */
    void initTextEdit();

	/**
	 * 初始化浏览器界面
	 */
	void initFancyBrowser();

    /**
     * makedir，在用textEdit生成html时不选择生成目录，因此可能目录不存在
     */
    void makedir(QString filename);

    /**
     * 界面上的单隧道限界表、图预览
     */
    void clearanceTableHeaderLoad();
    void clearanceTablePreview();
    void clearanceImagePreview();

    /**
     * statuslabel显示状态
     * @param outfilename 输出文件名
     * @param retval 输出状态结果
     * @param isappend 是否追加写入状态提示框
     */
    void statusShow(QString outfilename, int retval, bool isappend);

private:
    Ui::OutputClearanceWidget *ui;

    /**
     * 格外手动输入到表格项-起讫站名
     */
    QString qiqizhanming;

    /**
     * 格外手动输入到表格项-最大外轨超高
     */
    QString waiguichaogao;
    
    /**
     * 格外手动输入到表格项-最低接触网高度
     */
    QString jiechuwanggaodu;

    /**
     * PDF预览接口界面
     */
    TextEdit *edit;

	/**
	 * 浏览器界面
	 */
	FancyBrowser * webview;

    /**
     * 画图界面
     */
    ClearanceImage *syntunnelcorrect;

    QScrollArea *scrollArea;

    /**
     * 限界图表html和excel模板路径
     */
    QString templatepath;

    bool iscorrect;

    /**
     * 输出限界图形时的底板限界
     */
    OutputClearanceImageType outImageType;
    /**
     * 输出限界图形时的数据类型
     */
    ShowDataType datatype;
    
    /**
     * 当前综合方式：单隧道综合结果还是任意多隧道综合结果
     * @see ClientSetting中定义的SingleOrMultiSelectionMode，与其同步更新
     */
    SingleOrMultiSelectionMode singleMultiMode;

    /**
     * 单隧道或是多隧道综合时，选择的查看结果（直线段、左转曲线段、右转曲线段）
     */
    CurveType selectClearanceType;

    /**
     * 单隧道限界结果模型
     */
    ClearanceSingleTunnel & singleTunnelModel;
    /**
     * 单隧道基本信息
     */
    TunnelDataModel * singleTunnelBasicMode;

    /**
     * 多隧道区段综合限界结果模型
     */
    ClearanceMultiTunnels & multiTunnelsModel;

    /**
     * 兰州隧道限界图表文件输出类
     */
    LzOutputAccess * outputAccess;

signals:
    /**
     * 转换界面信号
     */
    void backToSelectionSignal();

    /**
     * 发送信号告知ImageSection画限界图的底板类型
     */
    void sendOutputType(OutputClearanceImageType);
    /**
     * 发送信号告知ImageSection画限界图的数据类型(直线/左曲线/右曲线))
     */
    void sendOutputDataType(ShowDataType);

public slots:
    /**
     * 界面切换槽，接收由隧道表格输出选择界面传递过来的信号
     * @param SingleOrMultiSelectionMode 是单隧道综合结果还是多隧道综合结果
     * @param CurveType 要显示的限界类型
     */
    void updateClearanceTableModel(SingleOrMultiSelectionMode, CurveType);

private slots:
    /**
     * 返回到限界选择槽，触发转换界面信号
     */
    void backToSelectionTunnel();
    
    /**
     * 限界输出预览槽函数，PDF方式
     */
    void pdfPreview();
    /**
     * 限界输出导出并预览槽函数，EXCEL方式
     */
    void excelPreview();

    /**
     * 选择输出限界图形时的底板限界
     */
    void selectOutputClearanceImageType(int index);
    void selectOutputClearanceDataType(int index);

    /**
     * 当多隧道综合时，批量输出各个隧道的限界图表到EXCEL文件
     */
    void exportAllToExcels();
};

#endif // OUTPUT_CLEARANCE_H
