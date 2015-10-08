#ifndef SYNTHESIS_CORRECT_WIDGET_H
#define SYNTHESIS_CORRECT_WIDGET_H

#include <QWidget>
#include "synthesis_tunnel.h"
#include "correct_clearance.h"
#include <string>


using namespace std;

namespace Ui {
    class SynthesisCorrectWidget;
}

/**
 * 隧道综合和图像修正共有界面类声明
 *
 * @author 范翔
 * @author 熊雪
 * @version 1.0.0
 * @date 20140416
 */
class SynthesisCorrectWidget: public QWidget
{
    Q_OBJECT

public:
    explicit SynthesisCorrectWidget(QWidget *parent = 0);
    ~SynthesisCorrectWidget();
	

signals:
    // 界面切换外部信号，为了换上层界面的按钮状态
	void toTabCorrect();
    void toTabSynthesis();
    void finish();

    // 发送当前选择帧号
    void sendCurrentFrame(_int64);
    
    // 发送给隧道综合、图形修正两个界面
    void signalSelectedTunnelToEdit(int, QString, bool, bool, long long, long long);

private:
    Ui::SynthesisCorrectWidget *ui;

    string filename;

    // 内部界面
    SynthesisTunnelWidget * widget_syn;
    CorrectClearanceWidget * widget_cor;

public slots:
    // 界面切换槽函数
    void slotSelectedTunnelToSynthesis(int tunnelid, QString, bool, bool, long long, long long);

private slots:
    // 界面切换槽函数
    void finishcorrect();
    void finishsynthesis();
    void tocorrectTab();
    
    /**
     * 更新listWidget的数值
     */
	void updateFramesViewInSearchArea(__int64 newfc, float newmile, int newdirection);

    /**
     * 情况筛选帧号listWidget的内容
     */
	void clearListWidget();

    /**
     * 给所包含的界面发送帧号消息
     */
	void sendFrameToChildWidget();
};

#endif // SYNTHESIS_CORRECT_WIDGET_H
