#ifndef SELECT_HISTORICAL_TUNNEL_H
#define SELECT_HISTORICAL_TUNNEL_H

#include <QWidget>
#include <QTableView>
#include <QtSql/QSqlRelationalTableModel>
#include <QDataWidgetMapper>
#include <QMessageBox>
#include <QModelIndex>

#include "lz_working_enum.h"
#include "drawimage.h"
#include "daoline.h"
#include "manage_line.h"
#include "clearance_item.h"
#include "setting_client.h"

namespace Ui {
    class SelectHistoricalTunnelDataWidget;
}

/**
 * 按哪种方式选择隧道数据
 */
enum WatchHistoryMode
{
    By_Date = 0,    // 按最邻近采集方式选择
    By_Line = 1     // 按隧道方式浏览
};

/**
 * 历史隧道选择输出选择（隧道限界图表输出之前）界面类声明
 * 在ClientSetting中定义综合方式：单隧道综合结果还是任意多隧道综合结果
 * @author 熊雪 范翔
 * @version 1.0.0
 * @date 20140322
 */
class SelectHistoricalTunnelDataWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SelectHistoricalTunnelDataWidget(QWidget *parent = 0);

    ~SelectHistoricalTunnelDataWidget();

private:
    Ui::SelectHistoricalTunnelDataWidget *ui;

    /**
     * 当watchMode == By_Line时按经典的选择线路模式加载
     */
    void loadLinesData();
    /**
     * 当watchMode == By_Line时界面左侧使用的线路模型
     */
    QStringListModel * linesModel;

    /**
     * 当watchMode == By_Date时按经典的选择线路模式加载
     */
    void loadRecentTaskData();
    /**
     * 当watchMode == By_Date时界面左侧使用的采集工程模型
     */
    QSqlQueryModel * taskModel;

    /**
     * 当watchMode == By_Line时界面中间使用的线路对应隧道模型
     * 当watchMode == By_Date时界面中间使用的采集工程对应采集隧道模型
     */
    QSqlQueryModel * taskTunnelsModel;

    /**
     * 当watchMode == By_Line或By_Date时
     * 界面中间的隧道视图更新
     */
    void loadTunnelData();

    /**
     * 某一隧道的历史隧道信息
     */
    QSqlQueryModel * oneTunnelHistoricalModel;

    /**
     * tunnelView的当前选择行
     */
    int currentRow;
    /**
     * historyTunnelTaskView的当前选择行
     */
    int currentHistorySelectRow;
    /**
     * 历史隧道浏览方式
     */
    WatchHistoryMode watchMode;

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
     * 是否可以切换到预览界面
     * 若尚未做出选择综合隧道则不能
     */
    bool canGoNextPage;

    /**
     * 单隧道限界结果模型
     */
    ClearanceSingleTunnel & singleTunnelModel;
    /**
     * 多隧道区段综合限界结果模型
     */
    ClearanceMultiTunnels & multiTunnelsModel;

    /**
     * 输出限界图形时的底板限界
     */
    OutputClearanceImageType outImageType;
    /**
     * 统计超限桥隧数量 限界模板路径
     */
    QString templatepath;

signals:
    /**
     * 界面切换信号，在界面切换到输出预览之前（即在本界面）已处理好输出信息（单隧道或多隧道综合结果的直线、左转曲线、右转曲线限界）
     * 且详细的相关信息已存入ClientSetting之中。不用作为参数传递
     * @param singleOrMultiTunnel 单隧输出还是多隧综合输出
     * @param type 直、左曲或右曲
     */
    void updateOutput(SingleOrMultiSelectionMode singleOrMultiTunnel, CurveType type);

private slots:

    /**
     * 变换隧道输出选择方式
     * 点击输出方式GroupBox后的槽函数
     */
    void changeSingleMultiMode(bool checked);

    /**
     * 变换隧道查看模式
     */
    void changeSelectionMode(bool mode);

    /**
     * 隧道列表视图更新显示，选择不同线路视图时的槽函数
     */
    void updateTunnelsView(const QModelIndex &index);

    ///////////  单隧道综合选择使用  //////////
    /**
     * 当singleMultiMode == Single_Mode时，点击某一条隧道后的槽函数
     * 查看单隧道具体信息，将该隧道历史采集任务显示到界面中，以及隧道基本信息，含几条曲线等
     */
    void showTunnelDetail(const QModelIndex &);
    /**
     * 当singleMultiMode == Single_Mode时，
     * 用于mapper的四个按钮，调用mapper的toFirst、toNext、toLast、toPrevious方法
     */
    void toFirst();
    void toNext();
    void toLast();
    void toPrevious();
    /**
     * 当singleMultiMode == Single_Mode时，
     * 设置隧道详细框当前匹配的内容显示出来，调用mapper的setCurrentIndex
     */
    void setCurrentMapper(int currentRow);
    /**
     * 更新历史选择隧道行号
     */
    void updateSelectHistoryRow(const QModelIndex &);
    
    ///////////  区段综合使用  //////////
    /**
     * 初始化多隧道综合表格表头信息
     */
    void initMultiTunnelsTableHeader();
    /**
     * 当singleMultiMode == Multi_Mode，即区段选择综合时，添加多条隧道
     */
    void addMultiTaskTunnels();
    /**
     * 当singleMultiMode == Multi_Mode，即区段选择综合时，删除多条隧道
     */
    void removeMultiTaskTunnels();

    /**
     * 加载单隧或多隧道综合结果数据槽函数
     * 界面上的预览直线段、左转曲线段、右转曲线段按钮及数字的信号槽
     */
    void loadClearanceDataToOutput();
    void clearClearanceDataInfo();

    /**
     * 区段综合，限界选择
     */
    void selectOutputClearanceImageType(int);

    /**
     * 点击跳转页面后跳转到OutputWidget页面，同时传参数
     */
    void toOutputWidget();

    /**
     * 查找框输入
     */
    void checkFindEdit();
    void showAllLines();
    void findTunnel();
    void findLine();
};

#endif // SELECT_HISTORICAL_TUNNEL_H
