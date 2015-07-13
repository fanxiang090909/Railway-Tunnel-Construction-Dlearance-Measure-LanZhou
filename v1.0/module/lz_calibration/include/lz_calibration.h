#ifndef LZ_CALIBRATION_H
#define LZ_CALIBRATION_H

#include <QtGui/QMainWindow>
#include "ui_lz_calibration.h"

#include "MatToQImage.h"
#include "ReadFilenames.h"

#include <vector>

using namespace std;

#define REC_IMG_SIZE_WID 1200
#define REC_IMG_SIZE_HEI 1500

/**
 * 兰州标定程序界面
 * 包括双目标定，和分中系统标定
 * @author 丁志宇 范翔
 * @date 20150401
 * @version 1.0.0
 */
class LZ_Calibration : public QWidget
{
	Q_OBJECT

public:
	LZ_Calibration(QWidget *parent = 0, Qt::WFlags flags = 0);
	~LZ_Calibration();

	bool ge_addimages(string leftname, string rightname);
	double calcReprojecterr(vector<Point2f> imagepoints, vector<Point3f> objectpoints, Mat rvecs, Mat tvecs, Mat cameramat, Mat dist );
	
	double calcEpipolarErr( vector<Point2f> lpts, vector<Point2f> rpts,
	Mat lcammat, Mat rcammat, Mat ldistcoeff, Mat rdistcoeff, Mat F );
	// fit template to model yielding R,t (M = R*T + t)
	double calcRt(vector<Point3f> sp_points, Mat templat, Mat &R, Mat &T);
	//double calc3dptsRMS( vector<Point3f> model, vector<Point3f> templat, Mat &R, Mat &t );

private:
	QFileDialog ge_fd, sp_fd, save_fd;

	//初始设置
	Size st_pattern_size;
	Size st_image_size;
	double st_pt_distance;
	Size st_blob_size;
	int  st_blob_thresh;
	//设置状态
	bool st_ok;

	// 特殊标定图像域
	QGraphicsScene* sp_lft_scene, *sp_rgt_scene;
	Mat sp_left,sp_right;
	vector<QGraphicsItem*> sp_lft_pts_item, sp_rgt_pts_item;
	vector<Point2f> sp_lft_pts, sp_rgt_pts;

	//选择状态，未选状态等(特殊图像域)
	bool sp_addnew;
	int current_op_index;
	Point2f lft_pt_temp, rgt_pt_temp;
	vector<Point2f> sp_final_lft_pts, sp_final_rgt_pts;
	vector<Point3f> sp_points;
	double sp_lft_err, sp_rgt_err;
	bool sp_ok;
    vector<Point3f> sp_calibboard_data_candidate;

	// 棋盘/点阵标定图像域
	vector< vector<Point2f> > ge_lft_pts, ge_rgt_pts;
	vector< Mat >	ge_lft_imgs, ge_rgt_imgs;
	vector< string > ge_lft_img_names, ge_rgt_img_names;
	vector< double > ge_lft_err, ge_rgt_err;
	vector<QListWidgetItem*> ge_listitems; 
	int ge_count;
	int ge_current;
	bool ge_ok;

	// 标定结果中间变量
	vector < vector<Point2f> > cali_ge_left, cali_ge_right;
	vector< Point2f > cali_sp_left, cali_sp_right;
	vector< Point3f > cali_sp_pts;

private:
	//重投影误差
	//@todo 
	// 添加更多误差
	double lreprojecterr, rreprojecterr, stereoerr;
	double epierr, recerr, recyerr;
	vector<double> epi_errs, rec_errs, rec_y_errs;

	//左右相机矩阵
	Mat lcameramat, rcameramat;
	//对应与每一幅标定图像的旋转和平移矩阵(基准相机相对于分中坐标系平移在最后)
	vector< Mat > lrvecs, ltvecs, rrvecs, rtvecs;
	//基准相机相对分中坐标系 旋转 平移矩阵
	Mat std_rvecs, std_tvecs;
	//左右相机的畸变参数
	Mat ldistcoeffs, rdistcoeffs;
	//双目标定结果矩阵
	Mat fundamentalmat, essentialmat;
	Mat rvecs, tvecs;
	//标定状态
	bool cali_ok;

	//最终结果
	Mat R1,R2,P1,P2;
	Mat lmap1, lmap2, rmap1, rmap2;
	Mat R,T;
	Rect lroi, rroi;
	Size recsize;
	double rterr;

private slots:
    // Tab 切换
    void tabCurrentChanged(int index);

	//初始设置
	void st_confirm();
	void st_clearall();

	//轨平面相关
	void sp_openright();
	void sp_openleft();

	//加载图片，处理图片，提取中心点，设置中心点选取等
	void sp_loadimages();
	
	//这五个函数比较乱
	void sp_clearall();
	void sp_inputcomplete();
	void sp_confirm();
	void sp_select();
	void sp_delete();
	void sp_choose();

	void sp_calculate();

	void sp_openload();
	void sp_opensave();
	
	//加载双目标定结果, 存储轨分中RT
	void sp_loadstereo();
	void sp_saveresult();

    //定西外标定板三维点数据
    void sp_load_caliboard_data();
    void sp_sellect_caliboard_pnt();
    void sp_sellect_caliboard_pnt_one(int index);

	//标定板图像相关
	void ge_openleft();
	void ge_openright();
	void ge_loadimages();
	void ge_delete();
	void ge_clearall();
	void ge_inputcomplete();
	void ge_selected();

	void runCalibration();
	void openXml();
	void saveAsXml();

private:
	Ui::LZ_CalibrationClass ui;
};

#endif // LZ_CALIBRATION_H
