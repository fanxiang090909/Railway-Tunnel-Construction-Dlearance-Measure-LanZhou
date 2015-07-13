#include "lz_calibration.h"
#include "icpPointToPoint.h"

/**
 * 兰州标定程序界面
 * 包括双目标定，和分中系统标定
 * @author 丁志宇 范翔
 * @date 20150401
 * @version 1.0.0
 */
LZ_Calibration::LZ_Calibration(QWidget *parent, Qt::WFlags flags)
	: QWidget(parent, flags)
{
	ui.setupUi(this);

    setWindowIcon(QIcon(":/cali_icon.png"));

	ui.ge_list->setIconSize(QSize(80, 64));        //设置QListWidget中的单元项的图片大小
	ui.ge_list->setResizeMode(QListView::Adjust);
	ui.ge_list->setViewMode(QListView::IconMode);   //设置QListWidget的显示模式
	ui.ge_list->setMovement(QListView::Static);    //设置QListWidget中的单元项不可被拖动
	ui.ge_list->setSpacing(7);                   //设置QListWidget中的单元项的间距

	ge_fd.setFileMode(QFileDialog::Directory);
	ge_fd.setOption(QFileDialog::ShowDirsOnly);

	ui.tabWidget->setTabText(0, tr("双目标定（标定板图像）") );
	ui.tabWidget->setTabText(1, tr("系统标定（轨平面坐标系下图像）") );
    tabCurrentChanged(ui.tabWidget->currentIndex());

	sp_lft_scene = new QGraphicsScene;
	ui.sp_lft_view->setScene(sp_lft_scene);
	ui.sp_lft_view->centerOn(0,0);
	sp_rgt_scene = new QGraphicsScene;
	ui.sp_rgt_view->setScene(sp_rgt_scene);
	ui.sp_rgt_view->centerOn(0,0);

	///////////////////init///////////////////	
	st_clearall();

	///////////////////bounding signals with slots///////////////
	// tab change
    connect(ui.tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabCurrentChanged(int)));
    
    //st
	connect(ui.st_confirm, SIGNAL(clicked()), this, SLOT(st_confirm()));
	connect(ui.st_clearall, SIGNAL(clicked()), this, SLOT(st_clearall()));

	//ge_cali
	connect(ui.cali_start, SIGNAL(clicked()), this, SLOT(runCalibration()));
	connect(ui.cali_save_open, SIGNAL(clicked()), this, SLOT(openXml()));
	connect(ui.cali_save, SIGNAL(clicked()), this, SLOT(saveAsXml()));

	//ge
	connect(ui.ge_leftimageopen, SIGNAL(clicked()), this, SLOT(ge_openleft()));
	connect(ui.ge_rightimageopen, SIGNAL(clicked()), this, SLOT(ge_openright()));
	connect(ui.ge_load_img, SIGNAL(clicked()), this, SLOT(ge_loadimages()));
	connect(ui.ge_list, SIGNAL(itemSelectionChanged()), this, SLOT(ge_selected()));
	connect(ui.ge_delete, SIGNAL(clicked()), this, SLOT(ge_delete()));
	connect(ui.ge_inputcomplete, SIGNAL(clicked()), this, SLOT(ge_inputcomplete()));
	connect(ui.ge_clearall, SIGNAL(clicked()), this, SLOT(ge_clearall()));

	//sp
	ui.sp_spinBox->setRange(0,0);
	connect(ui.sp_rightimageopen, SIGNAL(clicked()), this, SLOT(sp_openright()));
	connect(ui.sp_leftimageopen, SIGNAL(clicked()), this, SLOT(sp_openleft()));
	connect(ui.sp_loadimages, SIGNAL(clicked()), this, SLOT(sp_loadimages()));
	connect(sp_lft_scene,SIGNAL(selectionChanged()), this, SLOT(sp_select()));
	connect(sp_rgt_scene,SIGNAL(selectionChanged()), this, SLOT(sp_select()));
	connect(ui.sp_choose, SIGNAL(clicked()), this, SLOT(sp_choose()));
	connect(ui.sp_delete, SIGNAL(clicked()), this, SLOT(sp_delete()));
	connect(ui.sp_confirm, SIGNAL(clicked()), this, SLOT(sp_confirm()));
	connect(ui.sp_inputcomplete, SIGNAL(clicked()), this, SLOT(sp_inputcomplete()));
	connect(ui.sp_clearall, SIGNAL(clicked()), this, SLOT(sp_clearall()));
	connect(ui.sp_resultopen, SIGNAL(clicked()), this, SLOT(sp_opensave()));
	connect(ui.sp_stereoopen, SIGNAL(clicked()), this, SLOT(sp_openload()));
	connect(ui.sp_loadstereo, SIGNAL(clicked()), this, SLOT(sp_loadstereo()));
	connect(ui.sp_saveresult, SIGNAL(clicked()), this, SLOT(sp_saveresult()));
	connect(ui.sp_calc, SIGNAL(clicked()), this, SLOT(sp_calculate()));

    // 外标定板三维点数据选择
    connect(ui.sp_loadCaliBoardDataButton, SIGNAL(clicked(bool)), this, SLOT(sp_load_caliboard_data()));
    connect(ui.radio01, SIGNAL(clicked(bool)), this, SLOT(sp_sellect_caliboard_pnt()));
    connect(ui.radio02, SIGNAL(clicked(bool)), this, SLOT(sp_sellect_caliboard_pnt()));
    connect(ui.radio03, SIGNAL(clicked(bool)), this, SLOT(sp_sellect_caliboard_pnt()));
    connect(ui.radio04, SIGNAL(clicked(bool)), this, SLOT(sp_sellect_caliboard_pnt()));
    connect(ui.radio05, SIGNAL(clicked(bool)), this, SLOT(sp_sellect_caliboard_pnt()));
    connect(ui.radio06, SIGNAL(clicked(bool)), this, SLOT(sp_sellect_caliboard_pnt()));
    connect(ui.radio07, SIGNAL(clicked(bool)), this, SLOT(sp_sellect_caliboard_pnt()));
    connect(ui.radio08, SIGNAL(clicked(bool)), this, SLOT(sp_sellect_caliboard_pnt()));
    connect(ui.radio09, SIGNAL(clicked(bool)), this, SLOT(sp_sellect_caliboard_pnt()));
    connect(ui.radio0a, SIGNAL(clicked(bool)), this, SLOT(sp_sellect_caliboard_pnt()));
    connect(ui.radio11, SIGNAL(clicked(bool)), this, SLOT(sp_sellect_caliboard_pnt()));
    connect(ui.radio12, SIGNAL(clicked(bool)), this, SLOT(sp_sellect_caliboard_pnt()));
    connect(ui.radio13, SIGNAL(clicked(bool)), this, SLOT(sp_sellect_caliboard_pnt()));
    connect(ui.radio14, SIGNAL(clicked(bool)), this, SLOT(sp_sellect_caliboard_pnt()));
    connect(ui.radio15, SIGNAL(clicked(bool)), this, SLOT(sp_sellect_caliboard_pnt()));
    connect(ui.radio16, SIGNAL(clicked(bool)), this, SLOT(sp_sellect_caliboard_pnt()));
    connect(ui.radio17, SIGNAL(clicked(bool)), this, SLOT(sp_sellect_caliboard_pnt()));
    connect(ui.radio18, SIGNAL(clicked(bool)), this, SLOT(sp_sellect_caliboard_pnt()));
    connect(ui.radio19, SIGNAL(clicked(bool)), this, SLOT(sp_sellect_caliboard_pnt()));
    connect(ui.radio1a, SIGNAL(clicked(bool)), this, SLOT(sp_sellect_caliboard_pnt()));
}

LZ_Calibration::~LZ_Calibration()
{

}

void LZ_Calibration::sp_openright()
{
	sp_fd.show();
	if (sp_fd.exec() == QDialog::Accepted)
	{
		QString path = sp_fd.selectedFiles()[0];
		ui.sp_rightimage->setText(path);
	}
}

void LZ_Calibration::sp_openleft()
{
	sp_fd.show();
	if (sp_fd.exec() == QDialog::Accepted)
	{
		QString path = sp_fd.selectedFiles()[0];
		ui.sp_leftimage->setText(path);
	}
}

void LZ_Calibration::sp_loadimages()
{
	QString sleft = ui.sp_leftimage->text();
	QString sright = ui.sp_rightimage->text();

	sp_left = imread(sleft.toStdString());
	sp_right = imread(sright.toStdString());

	// fill full resolution
	Mat full_sp_left = Mat::zeros(1280, 1024, sp_left.type());
	Mat full_sp_right = Mat::zeros(1280, 1024, sp_right.type());
	sp_left.copyTo(full_sp_left.colRange(512,1024));
	sp_right.copyTo(full_sp_right.colRange(0,512));
	sp_left = full_sp_left.clone();
	sp_right = full_sp_right.clone();


	if ( sp_left.empty() || sp_right.empty() )
	{
		QMessageBox::information(this,tr("提示"),tr("加载图片失败"));
		return;
	}

	remap(sp_left, sp_left, lmap1, lmap2, INTER_LINEAR);
	remap(sp_right, sp_right, rmap1, rmap2, INTER_LINEAR);

	//@todo
	imwrite("C:\\Users\\dingzhiyu\\Desktop\\Left.jpg", sp_left);
	imwrite("C:\\Users\\dingzhiyu\\Desktop\\Right.jpg", sp_right);

	//********corner detection*************
	SimpleBlobDetector::Params params;
	params.blobColor = 255;

	params.minThreshold = st_blob_thresh;
	params.minArea = st_blob_size.width;
	params.maxArea = st_blob_size.height;

	params.filterByCircularity = false;
	params.filterByConvexity = false;
	params.filterByInertia = false;

	cv::Ptr<cv::FeatureDetector> blob_detector = new cv::SimpleBlobDetector(params);
	blob_detector->create("SimpleBlob");

	vector<KeyPoint> kp_left, kp_right;
	blob_detector->detect(sp_left, kp_left);
	blob_detector->detect(sp_right, kp_right);

	sp_lft_pts.clear();
	sp_rgt_pts.clear();
	for (int i=0; i<kp_left.size(); i++)
		sp_lft_pts.push_back(kp_left[i].pt);
	for (int i=0; i<kp_right.size(); i++)
		sp_rgt_pts.push_back(kp_right[i].pt);
	//********corner detection*************
	
	//sp_lft_scene->addPixmap(QPixmap::fromImage(MatToQImage(sp_left).scaled(400, 500)));
	//sp_rgt_scene->addPixmap(QPixmap::fromImage(MatToQImage(sp_right).scaled(400, 500)));
	sp_lft_scene->addPixmap(QPixmap::fromImage(MatToQImage(sp_left).scaled(1200, 1500)));
	sp_rgt_scene->addPixmap(QPixmap::fromImage(MatToQImage(sp_right).scaled(1200, 1500)));

	QPen qpen;
	qpen.setWidth(2);
	qpen.setColor(QColor(255,0,0));
	QGraphicsRectItem* itemp;

	sp_lft_pts_item.clear();
	sp_rgt_pts_item.clear();

	/*for(int i=0; i<sp_lft_pts.size(); i++)
	{
		itemp =  sp_lft_scene->addRect(sp_lft_pts[i].x/3-4, sp_lft_pts[i].y/3-4, 8, 8, qpen);
		sp_lft_pts_item.push_back( itemp );
		itemp->setFlags( QGraphicsItem::ItemIsSelectable );
	}
	for(int i=0; i<sp_rgt_pts.size(); i++)
	{
		itemp = sp_rgt_scene->addRect(sp_rgt_pts[i].x/3-4, sp_rgt_pts[i].y/3-4, 8, 8, qpen);
		sp_rgt_pts_item.push_back( itemp );
		itemp->setFlags( QGraphicsItem::ItemIsSelectable );
	}*/
	for(int i=0; i<sp_lft_pts.size(); i++)
	{
		itemp =  sp_lft_scene->addRect(sp_lft_pts[i].x-16, sp_lft_pts[i].y-16, 32, 32, qpen);
		sp_lft_pts_item.push_back( itemp );
		itemp->setFlags( QGraphicsItem::ItemIsSelectable );
	}
	for(int i=0; i<sp_rgt_pts.size(); i++)
	{
		itemp = sp_rgt_scene->addRect(sp_rgt_pts[i].x-16, sp_rgt_pts[i].y-16, 32, 32, qpen);
		sp_rgt_pts_item.push_back( itemp );
		itemp->setFlags( QGraphicsItem::ItemIsSelectable );
	}
	//*;

	ui.sp_lft_view->show();
	ui.sp_rgt_view->show();
	//*/
}

void LZ_Calibration::sp_clearall()
{
	sp_lft_scene->clear();
	sp_rgt_scene->clear();

	sp_lft_pts_item.clear();
	sp_rgt_pts_item.clear();
	sp_lft_pts.clear();
	sp_rgt_pts.clear();

	sp_addnew = false;
	current_op_index = -1;
	sp_final_lft_pts.clear();
	sp_final_rgt_pts.clear();
	sp_points.clear();

	ui.sp_spinBox->setRange(0, 0);

	sp_ok = false;
}

void LZ_Calibration::sp_inputcomplete()
{
	cali_sp_left.assign( sp_final_lft_pts.begin(), sp_final_lft_pts.end() );
	cali_sp_right.assign( sp_final_rgt_pts.begin(), sp_final_rgt_pts.end() );
	cali_sp_pts.assign( sp_points.begin(), sp_points.end() );

	ui.cali_sp_count->setText(QString::number(cali_sp_left.size()));

	sp_ok = true;
}

void LZ_Calibration::sp_confirm()
{
	if ( sp_addnew )
	{
		Point3f tempp3;
		tempp3.x = ui.sp_coorX->text().toDouble();
		tempp3.y = ui.sp_coorY->text().toDouble();
		tempp3.z = ui.sp_coorZ->text().toDouble();

		sp_final_lft_pts.push_back( lft_pt_temp );
		sp_final_rgt_pts.push_back( rgt_pt_temp );
		sp_points.push_back( tempp3 );

		ui.sp_spinBox->setMaximum( sp_points.size() );
		ui.sp_spinBox->setValue( sp_points.size() );
		current_op_index = sp_points.size() - 1;

		sp_lft_scene->clearSelection();
		sp_rgt_scene->clearSelection();

		sp_addnew = false;
	}
	else if ( current_op_index != -1 )
	{
		sp_points[current_op_index].x = ui.sp_coorX->text().toDouble();
		sp_points[current_op_index].y = ui.sp_coorY->text().toDouble();
		sp_points[current_op_index].z = ui.sp_coorZ->text().toDouble();
	}
}

void LZ_Calibration::sp_select()
{
	QList<QGraphicsItem*> leftlist, rightlist;
	leftlist = sp_lft_scene->selectedItems();
	rightlist = sp_rgt_scene->selectedItems();

	if (leftlist.empty() || rightlist.empty())
		return;

	//当选择一组点之后，执行以下代码

	//找出该组点对应的坐标存储至 lft_pt_temp, rgt_pt_temp
	//
	//	sp_lft_pts_item, sp_lft_pts
	//  sp_rgt_pts_item , sp_rgt_pts
	//  两组容器的内容一一对应
	vector<QGraphicsItem*>::iterator itemiter;
	itemiter = std::find(sp_lft_pts_item.begin(), sp_lft_pts_item.end(), leftlist[0]);
	int leftindex = itemiter - sp_lft_pts_item.begin();
	itemiter = std::find(sp_rgt_pts_item.begin(), sp_rgt_pts_item.end(), rightlist[0]);
	int rightindex = itemiter - sp_rgt_pts_item.begin();

	lft_pt_temp = sp_lft_pts[leftindex];
	rgt_pt_temp = sp_rgt_pts[rightindex];

	//检测lft_pt_temp, rgt_pt_temp是否组成有效选择对
	//
	//	sp_final_lft_pts, sp_final_rgt_pts, sp_points
	//  三个容器的内容是一一对应的
	//  故通过检测lft_pt_temp, rgt_pt_temp在容器中的
	//  index的对应关系可判断状态
	vector<Point2f>::iterator lpointiter, rpointiter;
	lpointiter = std::find(sp_final_lft_pts.begin(), sp_final_lft_pts.end(), lft_pt_temp);
	rpointiter = std::find(sp_final_rgt_pts.begin(), sp_final_rgt_pts.end(), rgt_pt_temp);

	//状态重置
	sp_addnew = false;
	current_op_index = -1;
	//ui.sp_spinBox->setValue( 0 );

	//新匹配点被选择
	if ( lpointiter==sp_final_lft_pts.end() && rpointiter==sp_final_rgt_pts.end() )
	{
		ui.sp_coorX->setText( QString::number(0.0, 'f') );
		ui.sp_coorY->setText( QString::number(0.0, 'f') );
		ui.sp_coorZ->setText( QString::number(0.0, 'f') );
		sp_addnew = true;
	}
	//旧匹配点被选择
	else if ( lpointiter-sp_final_lft_pts.begin() == rpointiter-sp_final_rgt_pts.begin() )
	{
		current_op_index = lpointiter - sp_final_lft_pts.begin();
		ui.sp_spinBox->setValue(current_op_index + 1);
		Point3f tempp3 = sp_points[current_op_index];

		ui.sp_coorX->setText( QString::number(tempp3.x, 'f') );
		ui.sp_coorY->setText( QString::number(tempp3.y, 'f') );
		ui.sp_coorZ->setText( QString::number(tempp3.z, 'f') );
	}
	//无效匹配点（两点中，至少有一点已经被选择匹配，并且两点不属于一组匹配点）
	else
	{
		QMessageBox::information(this, tr("提示"), tr("所选两点已经进行过匹配，请重新选择或者浏览已匹配队列进行删除") );
	}

}

void LZ_Calibration::sp_delete()
{
	if ( current_op_index == -1 )
	{
		return;
	}
	else
	{
		sp_final_lft_pts.erase( sp_final_lft_pts.begin() + current_op_index );
		sp_final_rgt_pts.erase( sp_final_rgt_pts.begin() + current_op_index );
		sp_points.erase( sp_points.begin() + current_op_index );

		sp_lft_scene->clearSelection();
		sp_rgt_scene->clearSelection();
		current_op_index = -1;
		ui.sp_spinBox->setValue(0);
		ui.sp_spinBox->setMaximum( sp_points.size() );
	}
}

void LZ_Calibration::sp_choose()
{
	int index =ui.sp_spinBox->value();

	if (index == 0)
		return;
	else
	{
		sp_addnew = false;
		current_op_index = index - 1;
		//反向查找item
		lft_pt_temp = sp_final_lft_pts[current_op_index];
		rgt_pt_temp = sp_final_rgt_pts[current_op_index];

		vector<Point2f>::iterator ltemp,rtemp;
		ltemp = std::find(sp_lft_pts.begin(), sp_lft_pts.end(), lft_pt_temp);
		rtemp = std::find(sp_rgt_pts.begin(), sp_rgt_pts.end(), rgt_pt_temp);

		int lindex = ltemp - sp_lft_pts.begin();
		int rindex = rtemp - sp_rgt_pts.begin();

		sp_lft_scene->clearSelection();
		sp_rgt_scene->clearSelection();

		sp_lft_pts_item[lindex]->setSelected( true );
		sp_rgt_pts_item[rindex]->setSelected( true );
	}
}

void LZ_Calibration::ge_openleft()
{
	ge_fd.show();
	if (ge_fd.exec() == QDialog::Accepted)
	{
		QString path = ge_fd.selectedFiles()[0];
		ui.ge_lft_imgpath->setText(path);
	}
}

void LZ_Calibration::ge_openright()
{
	ge_fd.show();
	if (ge_fd.exec() == QDialog::Accepted)
	{
		QString path = ge_fd.selectedFiles()[0];
		ui.ge_rgt_imgpath->setText(path);
	}
}

void LZ_Calibration::ge_loadimages()
{
	ge_clearall();

	vector<string> lft_img_names, rgt_img_names;
	readFilenames( ui.ge_lft_imgpath->text().toStdString(), lft_img_names);
	readFilenames( ui.ge_rgt_imgpath->text().toStdString(), rgt_img_names);

	//读取文件名数量不一样，返回
	if ( lft_img_names.size()!=rgt_img_names.size() || lft_img_names.empty() || rgt_img_names.empty() )
	{
		QMessageBox::information( this, tr("错误"), tr("加载图片失败") );
		//undone
	}
	for ( int i=0; i<lft_img_names.size(); i++ )
	{
		ge_addimages(lft_img_names[i], rgt_img_names[i]);
	}
}

bool LZ_Calibration::ge_addimages(string leftname, string rightname)
{
	Mat left = imread(leftname);
	Mat right = imread(rightname);

	//threshold(left, left, 10, 255, THRESH_BINARY);

	if (left.empty() || right.empty())
		return false;

	vector<Point2f> leftcorners;
	vector<Point2f> rightcorners;

	//********corner detection*************
	SimpleBlobDetector::Params params;
	params.blobColor = 255;

	params.minThreshold = st_blob_thresh;
	params.minArea = st_blob_size.width;
	params.maxArea = st_blob_size.height;

	params.filterByCircularity = false;
	params.filterByConvexity = false;
	params.filterByInertia = false;

	cv::Ptr<cv::FeatureDetector> blob_detector = new cv::SimpleBlobDetector(params);
	blob_detector->create("SimpleBlob");

	bool leftfound,rightfound;
	leftfound = findCirclesGrid(left, st_pattern_size, leftcorners, CALIB_CB_CLUSTERING|CALIB_CB_SYMMETRIC_GRID, blob_detector);
	rightfound = findCirclesGrid(right, st_pattern_size, rightcorners, CALIB_CB_CLUSTERING|CALIB_CB_SYMMETRIC_GRID, blob_detector);
	//********corner detection*************

	drawChessboardCorners(left,st_pattern_size,leftcorners,leftfound);
	drawChessboardCorners(right,st_pattern_size,rightcorners,rightfound);

	//update
	ge_count++;
	ge_lft_imgs.push_back(left);
	ge_rgt_imgs.push_back(right);
	ge_lft_pts.push_back(leftcorners);
	ge_rgt_pts.push_back(rightcorners);
	ge_lft_img_names.push_back(leftname);
	ge_rgt_img_names.push_back(rightname);
	ge_lft_err.push_back(0.0);
	ge_rgt_err.push_back(0.0);
	epi_errs.push_back(0.0);
	rec_errs.push_back(0.0);
	rec_y_errs.push_back(0.0);

	QListWidgetItem* temp = new QListWidgetItem(QIcon(QPixmap::fromImage(MatToQImage(left)).scaled(QSize(80,64))), QString::number(ge_count));
	ge_listitems.push_back(temp);
	temp->setSizeHint(QSize(100,80));            //设置单元项的宽度和高度

	ui.ge_list->addItem(temp);
	return true;
}

void LZ_Calibration::ge_delete()
{
	if ( ge_current == -1 )
		return;
	else
	{
		ge_ok = false;

		ge_listitems.erase( ge_listitems.begin()+ge_current );
		ge_lft_img_names.erase( ge_lft_img_names.begin()+ge_current );
		ge_rgt_img_names.erase( ge_rgt_img_names.begin()+ge_current );
		ge_lft_imgs.erase( ge_lft_imgs.begin()+ge_current );
		ge_rgt_imgs.erase( ge_rgt_imgs.begin()+ge_current );
		ge_lft_pts.erase( ge_lft_pts.begin()+ge_current );
		ge_rgt_pts.erase( ge_rgt_pts.begin()+ge_current );
		ge_lft_err.erase( ge_lft_err.begin()+ge_current );
		ge_rgt_err.erase( ge_rgt_err.begin()+ge_current );

		epi_errs.erase( epi_errs.begin()+ge_current );
		rec_errs.erase( rec_errs.begin()+ge_current );
		rec_y_errs.erase( rec_y_errs.begin()+ge_current );

		ui.ge_list->takeItem( ge_current );
		//tabeItem会在listWidget真正将item删除前，将currentRow往前置位
		//并且emit itemSelectionChanged信号
		//这样会导致删除item操作后，ge_current值错误，所以在进行takeItem
		//之后，再次调用选择槽函数
		ge_selected();

	}
}

void LZ_Calibration::ge_clearall()
{
	ge_ok = false;

	while( ge_listitems.size() > 0 )
	{
		delete ge_listitems[0];
		ge_listitems.erase( ge_listitems.begin() );
	}

	ge_count = 0;
	ge_lft_imgs.clear();
	ge_rgt_imgs.clear();
	ge_lft_pts.clear();
	ge_rgt_pts.clear();
	ge_lft_img_names.clear();
	ge_rgt_img_names.clear();
	ge_lft_err.clear();
	ge_rgt_err.clear();
	epi_errs.clear();
	rec_errs.clear();
	rec_y_errs.clear();

	ui.cali_ge_count->setText(QString::number( cali_ge_left.size() ));
}

void LZ_Calibration::ge_inputcomplete()
{
	ge_ok = true;

	cali_ge_left.clear();
	cali_ge_right.clear();

	for (int i=0; i<ge_lft_pts.size(); i++)
	{
		cali_ge_left.push_back(ge_lft_pts[i]);
		cali_ge_right.push_back( ge_rgt_pts[i] );
	}

	ui.cali_ge_count->setText(QString::number( cali_ge_left.size() ));
}

//changed by 11.10
void LZ_Calibration::ge_selected()
{
	ge_current = ui.ge_list->currentRow();

	//以下两个if语句是用来判断ge_current是否超出边界
	//QListWidget在进行删除等操作后，会发出 itemSelectionChanged() 信号
	//触发此槽函数，但是此时ui.ge_list->currentRow()返回的是，在item未被
	//删除之前的，下一个item的Row，这样会导致越界访问
	if (ge_current == ge_listitems.size())
		ge_current--;
	if (ge_current < 0)
		return;

	//显示重投影误差（在标定之后）
	//@todo
	//  添加标定之后显示的信息
	if ( cali_ok )
	{
		ui.cali_current_lft_err->setText( QString::number( ge_lft_err[ge_current] ,'f') );
		ui.cali_current_rgt_err->setText( QString::number( ge_rgt_err[ge_current] ,'f') );
		ui.cali_current_epi_err->setText( QString::number( epi_errs[ge_current] ,'f') );
		ui.cali_current_3d_err->setText( QString::number( rec_errs[ge_current] ,'f') );
		ui.cali_current_rec_y_err->setText( QString::number( rec_y_errs[ge_current] ,'f') );
	}

	ui.ge_lft_img->setPixmap( QPixmap::fromImage(MatToQImage(ge_lft_imgs[ge_current])).scaled(QSize(256,320)) );
	ui.ge_rgt_img->setPixmap( QPixmap::fromImage(MatToQImage(ge_rgt_imgs[ge_current])).scaled(QSize(256,320)) );

	ui.ge_lftimgname->setText( tr(ge_lft_img_names[ge_current].c_str() ));
	ui.ge_rgtimgname->setText( tr(ge_rgt_img_names[ge_current].c_str() ));

	ui.ge_lft_pts_count->setText( QString::number(ge_lft_pts[ge_current].size()) );
	ui.ge_rgt_pts_count->setText( QString::number(ge_rgt_pts[ge_current].size()) );

}

void LZ_Calibration::runCalibration()
{
	if ( !ge_ok )
	{
		QMessageBox::information( this, tr("错误"), tr("标定数据未输入完毕") );
		return;
	}

	//assemble parameters
	Size patternsize = st_pattern_size;
	double distance = st_pt_distance;
	Size imagesize = st_image_size;

	//image corners
	vector< vector<Point2f> > left_img_pts, right_img_pts;
	for ( int i=0; i<cali_ge_left.size(); i++)
	{
		left_img_pts.push_back(cali_ge_left[i]);
		right_img_pts.push_back(cali_ge_right[i]);
	}

	//object points 
	vector< Point3f > optemp;
	vector< vector<Point3f> > objectpoints;
	for( int i = 0; i < patternsize.height; ++i ) 
		for( int j = 0; j < patternsize.width; ++j ) 
			optemp.push_back(Point3f(float( j*distance ), float( i*distance ), 0));
	//cali_ge_left.size()代表标定版图像的数量
	for ( int i=0; i<cali_ge_left.size(); i++ )
		objectpoints.push_back(optemp);

	//run monocam calibration
	Mat Q;
	lreprojecterr = 
		calibrateCamera(objectpoints, left_img_pts,
		imagesize, lcameramat, ldistcoeffs,
		lrvecs, ltvecs, 0,
		TermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS, 100, 1e-6));
	rreprojecterr = 
		calibrateCamera(objectpoints, right_img_pts,
		imagesize, rcameramat, rdistcoeffs,
		rrvecs, rtvecs, 0,
		TermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS, 100, 1e-6));

	//run monocam calibration error
	// vector<double> left,right
	// double avr_err
	vector<double> lft_err, rgt_err;
	lft_err.resize( left_img_pts.size() );
	rgt_err.resize( left_img_pts.size() );
	for ( int i=0; i<left_img_pts.size(); i++)
	{
		lft_err[i] = calcReprojecterr( left_img_pts[i], objectpoints[i], lrvecs[i], ltvecs[i], lcameramat, ldistcoeffs) ;
		rgt_err[i] = calcReprojecterr( right_img_pts[i], objectpoints[i], rrvecs[i], rtvecs[i], rcameramat, rdistcoeffs) ;
	}
	ge_lft_err.assign( lft_err.begin(), lft_err.end() );
	ge_rgt_err.assign( rgt_err.begin(), rgt_err.end() );
	///////////////////////////////////////////////////////////////////////

	// run stereo calibration
	stereoerr = stereoCalibrate(objectpoints, left_img_pts, right_img_pts,
		lcameramat, ldistcoeffs,
		rcameramat, rdistcoeffs,
		imagesize, rvecs, tvecs,
		essentialmat, fundamentalmat,
		TermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS, 100, 1e-6),
		CV_CALIB_FIX_INTRINSIC);

	//run stereo calibration error
	vector<double> epi_err;
	double epi_err_avg = 0;
	epi_err.resize( left_img_pts.size() );
	for ( int i=0; i<left_img_pts.size(); i++)
	{
		epi_err[i] = calcEpipolarErr(left_img_pts[i], right_img_pts[i],
			lcameramat, rcameramat,
			ldistcoeffs, rdistcoeffs,
			fundamentalmat);

		epi_err_avg += epi_err[i];
	}
	int nimgs = (int)left_img_pts.size();
	epierr = epi_err_avg/nimgs;
	epi_errs.assign( epi_err.begin(), epi_err.end() );

	//run stereo rectify
	Size rectifysize(REC_IMG_SIZE_WID, REC_IMG_SIZE_HEI);
	stereoRectify(lcameramat, ldistcoeffs,
		rcameramat, rdistcoeffs,
		imagesize, rvecs, tvecs,
		R1, R2, P1, P2, Q,
		0, -1,
		rectifysize, &lroi, &rroi);

	//run stereo rectify error
	// undistortpoint
	// triangulate
	// calc transform
	// give err
	vector<double> rec_err;
	rec_err.resize(left_img_pts.size());
	double rec_err_avg = 0;
	Mat tmpR, tmpT;
	vector<double> rec_y_err;
	rec_y_err.resize(left_img_pts.size());
	double rev_y_err_avg=0;
	for (int i=0; i<left_img_pts.size(); i++)
	{
		undistortPoints(left_img_pts[i], left_img_pts[i],
			lcameramat, ldistcoeffs, R1, P1);
		undistortPoints(right_img_pts[i], right_img_pts[i],
			rcameramat, rdistcoeffs, R2, P2);

		rev_y_err_avg = 0;
		for (int j=0; j<left_img_pts[i].size(); j++)
		{
			rev_y_err_avg += fabs(left_img_pts[i][j].y - right_img_pts[i][j].y);
		}
		rev_y_err_avg /= (int)left_img_pts[i].size();
		rec_y_err[i] = rev_y_err_avg;
		recyerr += rec_y_err[i];
	}
	rec_y_errs.assign(rec_y_err.begin(), rec_y_err.end());
	recyerr /= (int)left_img_pts.size();

	vector<Mat> pnt3ds;
	Mat pnt3dtmp(1,left_img_pts[0].size(),CV_64FC4);
	pnt3ds.resize( left_img_pts.size(), pnt3dtmp);

	for (int i=0; i<left_img_pts.size(); i++)
	{
		//triangul;
		triangulatePoints(P1,P2, left_img_pts[i], right_img_pts[i],
			pnt3ds[i]);
		rec_err[i] = calcRt(objectpoints[i], pnt3ds[i], tmpR, tmpT);
		rec_err_avg += rec_err[i];
	}
	recerr = rec_err_avg / nimgs;
	rec_errs.assign(rec_err.begin(), rec_err.end());

	//@todo

	//
	//Rect tmp = boundingRect(pts);
	//undistortPoints(left_img_pts[i], left_img_pts[i],
	//		lcameramat, ldistcoeffs, R1, P1);
	//rroi &= tmp;
	

			//show result
			ui.stereo_err->setText( QString::number( stereoerr, 'f' ) );
			ui.cali_avr_lft_error->setText( QString::number(lreprojecterr, 'f' ) );
			ui.cali_avr_rgt_error->setText( QString::number(rreprojecterr, 'f' ) );
			ui.epipolar_err->setText( QString::number(epierr, 'f' ) );
			ui.reproject_3d_err->setText( QString::number(recerr, 'f' ) );
			ui.rec_y_err->setText( QString::number(recyerr, 'f' ) );
			//ldistcoeffs
			ui.cali_lft_k1->setText( QString::number(ldistcoeffs.at<double>(0), 'f' ) );
			ui.cali_lft_k2->setText( QString::number(ldistcoeffs.at<double>(1), 'f' ) );
			ui.cali_lft_p1->setText( QString::number(ldistcoeffs.at<double>(2), 'f' ) );
			ui.cali_lft_p2->setText( QString::number(ldistcoeffs.at<double>(3), 'f' ) );
			ui.cali_lft_k3->setText( QString::number(ldistcoeffs.at<double>(4), 'f' ) );
			
			ui.cali_rgt_k1->setText( QString::number(rdistcoeffs.at<double>(0), 'f' ) );
			ui.cali_rgt_k2->setText( QString::number(rdistcoeffs.at<double>(1), 'f' ) );
			ui.cali_rgt_p1->setText( QString::number(rdistcoeffs.at<double>(2), 'f' ) );
			ui.cali_rgt_p2->setText( QString::number(rdistcoeffs.at<double>(3), 'f' ) );
			ui.cali_rgt_k3->setText( QString::number(rdistcoeffs.at<double>(4), 'f' ) );

			cali_ok = true;
}

double LZ_Calibration::calcReprojecterr(vector<Point2f> imagepoints, vector<Point3f> objectpoints, Mat rvecs, Mat tvecs, Mat cameramat, Mat dist )
{
	vector<Point2f> imagepoints_calc;
	projectPoints( Mat(objectpoints), rvecs, tvecs, cameramat, 
		dist, imagepoints_calc); 

	double err;
	err = norm(Mat(imagepoints), Mat(imagepoints_calc), CV_L2);
	int n = (int)objectpoints.size();
	err = std::sqrt(err*err/n);

	return err;
}

double LZ_Calibration::calcEpipolarErr( vector<Point2f> lpts, vector<Point2f> rpts,
	Mat lcammat, Mat rcammat, Mat ldistcoeff, Mat rdistcoeff, Mat F)
{
	double err = 0;
	int npts;
	npts = (int)lpts.size( );
	vector<Vec3f> lines[2];
	Mat imgpt[2];

	imgpt[0] = Mat(lpts);
	imgpt[1] = Mat(rpts);

	undistortPoints(imgpt[0], imgpt[0], lcammat, ldistcoeff, Mat(), lcammat);
	undistortPoints(imgpt[1], imgpt[1], rcammat, rdistcoeff, Mat(), rcammat);

	computeCorrespondEpilines(imgpt[0], 1, F, lines[0]);
	computeCorrespondEpilines(imgpt[1], 2, F, lines[1]);

	for(int i=0; i<npts; i++)
	{
		double erri = fabs( lpts[i].x*lines[1][i][0] + lpts[i].y*lines[1][i][1]+ lines[1][i][2])					+ fabs( rpts[i].x*lines[0][i][0] + rpts[i].y*lines[0][i][1]+ lines[0][i][2] );
		err += erri;
	}
	err /= npts;

	return err;
}

void LZ_Calibration::openXml()
{
	save_fd.show();
	if (save_fd.exec() == QDialog::Accepted)
	{
		QString path = save_fd.selectedFiles()[0];
		ui.cali_save_path->setText(path);
	}
}

void LZ_Calibration::saveAsXml()
{
	if ( !cali_ok)
	{
		QMessageBox::information( this, tr("错误"), tr("标定未完成，标定结果无效") );
		return;
	}

	QString filename = ui.cali_save_path->text();
	//使用Opencv的xml接口
	FileStorage fs( filename.toStdString(), FileStorage::WRITE );

	if ( !fs.isOpened() )
	{
		QMessageBox::information( this, tr("错误"), tr("打开存储文件失败") );
		return;
	}

	cvWriteComment( *fs, "LZ_Calibration_Result", 0 );

	time_t tt;
	time( &tt );
	struct tm *t2 = localtime( &tt );
	char buf[1024];
	strftime( buf, sizeof(buf)-1, "%c", t2 );
	fs << "calibration_time" << buf;

	fs << "Average_reproject_error_Standard_camera" << lreprojecterr;
	fs << "Average_reproject_error_Second_camera" << rreprojecterr;
	fs << "Stereo_reproject_error" << stereoerr;
	fs << "Epipolar_error" << epierr;
	fs << "Rectified_Epipolar_error" << recyerr;
	fs << "Rectified_3D_error" << recerr;
	fs << "Rectify_new_image_size" << Size(REC_IMG_SIZE_WID, REC_IMG_SIZE_HEI);

	// @todo 
	fs << "P1" << P1;
	fs << "P2" << P2;
	fs << "R1" << R1;
	fs << "R2" << R2;
	fs << "LEFT_ROI" << lroi;
	fs << "RIGHT_ROI" << rroi;
	fs << "lcameramat" << lcameramat;
	fs << "rcameramat" << rcameramat;
	fs << "ldistcoeffs" << ldistcoeffs;
	fs << "rdistcoeffs" << rdistcoeffs;

	fs.release();
}

void LZ_Calibration::tabCurrentChanged(int index)
{
    if (index == 0)
        ui.ge_current_err_widget->setVisible(true);
    else
        ui.ge_current_err_widget->setVisible(false);
}

void LZ_Calibration::st_confirm()
{
	st_pattern_size.width = ui.st_row->text().toInt();
	st_pattern_size.height = ui.st_col->text().toInt();
	st_image_size.width = ui.st_img_row->text().toInt();
	st_image_size.height = ui.st_img_col->text().toInt();
	st_pt_distance = ui.st_pt_distance->text().toDouble();
	st_blob_size.width = ui.st_blobsize_min->text().toInt();
	st_blob_size.height = ui.st_blobsize_max->text().toInt();
	st_blob_thresh = ui.st_blob_thresh->text().toInt();


	st_ok = true;
	ui.tabWidget->setDisabled(false);

	ui.st_groupBox->setDisabled(true);
}

void LZ_Calibration::st_clearall()
{
	st_ok = false;
	cali_ok = false;

	sp_clearall();
	ge_clearall();

	ui.tabWidget->setDisabled(true);
	ui.st_groupBox->setDisabled(false);
}

void LZ_Calibration::sp_openload()
{
	sp_fd.show();
	if (sp_fd.exec() == QDialog::Accepted)
	{
		QString path = sp_fd.selectedFiles()[0];
		ui.sp_stereoresultpath->setText(path);
	}
}

void LZ_Calibration::sp_opensave()
{
	sp_fd.show();
	if (sp_fd.exec() == QDialog::Accepted)
	{
		QString path = sp_fd.selectedFiles()[0];
		ui.sp_resultpath->setText(path);
	}
}

void LZ_Calibration::sp_loadstereo()
{

	QString filename = ui.sp_stereoresultpath->text();
	//使用Opencv的xml接口
	FileStorage fs( filename.toStdString(), FileStorage::READ );

	if ( !fs.isOpened() )
	{
		QMessageBox::information( this, tr("错误"), tr("打开双目标定文件失败") );
		return;
	}

	fs["P1"] >> P1;
	fs["P2"] >> P2;
	fs["R1"] >> R1;
	fs["R2"] >> R2;
	//fs["LEFT_ROI"] >> lroi;
	//fs["RIGHT_ROI"] >> rroi;
	fs["lcameramat"] >> lcameramat;
	fs["rcameramat"] >> rcameramat;
	fs["ldistcoeffs"] >> ldistcoeffs;
	fs["rdistcoeffs"] >> rdistcoeffs;
	recsize = Size(REC_IMG_SIZE_WID, REC_IMG_SIZE_HEI);

	fs.release();

	initUndistortRectifyMap(lcameramat, ldistcoeffs, R1, P1,
		recsize, CV_32FC1, lmap1, lmap2);
	initUndistortRectifyMap(rcameramat, rdistcoeffs, R2, P2,
		recsize, CV_32FC1, rmap1, rmap2);
}

//todo
void LZ_Calibration::sp_saveresult()
{
	QString filename = ui.sp_resultpath->text();
	//使用Opencv的xml接口
	FileStorage fs( filename.toStdString(), FileStorage::APPEND );

	if ( !fs.isOpened() )
	{
		QMessageBox::information( this, tr("错误"), tr("打开存储文件失败") );
		return;
	}

	fs << "R" << R;
	fs << "T" << T;
	fs << "recsizewid" << recsize.width;
	fs << "recsizehei" << recsize.height;

	fs.release();

}

void LZ_Calibration::sp_load_caliboard_data()
{
    sp_calibboard_data_candidate.clear();
    
    QString filename = QFileDialog::getOpenFileName(this, QObject::tr("载入系统标定板三维点数据"), "", "Document files (*.calipnt)");
	{
        QFile file;
        QString in;

        file.setFileName(filename);

        // Currently here for debugging purposes
        ui.sp_loadCalibBoardDataEdit->appendPlainText(QString(filename));

        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream inStream(&file);
            in = inStream.readLine();
            if(!(in.isNull()))
                in = inStream.readLine();
            else
            {
                ui.sp_loadCalibBoardDataEdit->appendPlainText(QObject::tr("无数据，加载失败\n"));
                file.close();
                return;
            }
            Point3f tmp;
            while(!(in.isNull()))
            {
                // Reads in fifth column: signal-to-noise ratio
                tmp.x = in.section("\t",0,0).toDouble();
                // Reads in fifth column: signal-to-noise ratio
                tmp.y = in.section("\t",1,1).toDouble();
                // Reads in fifth column: signal-to-noise ratio
                tmp.z = in.section("\t",2,2).toDouble();

                sp_calibboard_data_candidate.push_back(tmp);

                // Reads in fifth column: signal-to-noise ratio
                tmp.x = in.section("\t",4,4).toDouble();
                // Reads in fifth column: signal-to-noise ratio
                tmp.y = in.section("\t",5,5).toDouble();
                // Reads in fifth column: signal-to-noise ratio
                tmp.z = in.section("\t",6,6).toDouble();

                sp_calibboard_data_candidate.push_back(tmp);                //qDebug() << "x=" << tmp.x << ", y=" << tmp.y << ", z=" << tmp.z;
                
                in = inStream.readLine();
            }
            file.close();
            ui.sp_loadCalibBoardDataEdit->appendPlainText(QObject::tr("加载成功\n"));

            // 显示
            bool ifshow = true;
            for (int i = 0; i < sp_calibboard_data_candidate.size(); i++)
            {    
                ifshow = true;
                tmp = sp_calibboard_data_candidate[i];
                if (tmp.x == 0 && tmp.y == 0 && tmp.z == 0)
                {
                    ifshow = false;
                }
                switch (i)
                {
                    case 0: ui.radio01->setEnabled(ifshow); break;
                    case 1: ui.radio11->setEnabled(ifshow); break;
                    case 2: ui.radio02->setEnabled(ifshow); break;
                    case 3: ui.radio12->setEnabled(ifshow); break;
                    case 4: ui.radio03->setEnabled(ifshow); break;
                    case 5: ui.radio13->setEnabled(ifshow); break;
                    case 6: ui.radio04->setEnabled(ifshow); break;
                    case 7: ui.radio14->setEnabled(ifshow); break;
                    case 8: ui.radio05->setEnabled(ifshow); break;
                    case 9: ui.radio15->setEnabled(ifshow); break;
                    case 10:ui.radio06->setEnabled(ifshow); break;
                    case 11:ui.radio16->setEnabled(ifshow); break;
                    case 12:ui.radio07->setEnabled(ifshow); break;
                    case 13:ui.radio17->setEnabled(ifshow); break;
                    case 14:ui.radio08->setEnabled(ifshow); break;
                    case 15:ui.radio18->setEnabled(ifshow); break;
                    case 16:ui.radio09->setEnabled(ifshow); break;
                    case 17:ui.radio19->setEnabled(ifshow); break;
                    case 18:ui.radio0a->setEnabled(ifshow); break;
                    case 19:ui.radio1a->setEnabled(ifshow); break;
                    default: break;
                }
            }
            
            qDebug() << "read_end" << endl;
        }
        // Do some proper debugging here at some point...
        else 
        {
            ui.sp_loadCalibBoardDataEdit->appendPlainText(QObject::tr("无数据，加载失败\n"));
            ui.sp_coorX->setText("");
	        ui.sp_coorY->setText("");
	        ui.sp_coorZ->setText("");
        }
	}
}

void LZ_Calibration::sp_sellect_caliboard_pnt()
{
    if (sender() == ui.radio01) {  sp_sellect_caliboard_pnt_one(0);
    } else if (sender() == ui.radio02) {  sp_sellect_caliboard_pnt_one(2);
    } else if (sender() == ui.radio03) {  sp_sellect_caliboard_pnt_one(4);
    } else if (sender() == ui.radio04) {  sp_sellect_caliboard_pnt_one(6);
    } else if (sender() == ui.radio05) {  sp_sellect_caliboard_pnt_one(8);
    } else if (sender() == ui.radio06) {  sp_sellect_caliboard_pnt_one(10);
    } else if (sender() == ui.radio07) {  sp_sellect_caliboard_pnt_one(12);
    } else if (sender() == ui.radio08) {  sp_sellect_caliboard_pnt_one(14);
    } else if (sender() == ui.radio09) {  sp_sellect_caliboard_pnt_one(16);
    } else if (sender() == ui.radio0a) {  sp_sellect_caliboard_pnt_one(18);
    } else if (sender() == ui.radio11) {  sp_sellect_caliboard_pnt_one(1);
    } else if (sender() == ui.radio12) {  sp_sellect_caliboard_pnt_one(3);
    } else if (sender() == ui.radio13) {  sp_sellect_caliboard_pnt_one(5);
    } else if (sender() == ui.radio14) {  sp_sellect_caliboard_pnt_one(7);
    } else if (sender() == ui.radio15) {  sp_sellect_caliboard_pnt_one(9);
    } else if (sender() == ui.radio16) {  sp_sellect_caliboard_pnt_one(11);
    } else if (sender() == ui.radio17) {  sp_sellect_caliboard_pnt_one(13);
    } else if (sender() == ui.radio18) {  sp_sellect_caliboard_pnt_one(15);
    } else if (sender() == ui.radio19) {  sp_sellect_caliboard_pnt_one(17);
    } else if (sender() == ui.radio1a) {  sp_sellect_caliboard_pnt_one(19);
    }
}

void LZ_Calibration::sp_sellect_caliboard_pnt_one(int index)
{
    if (index >= sp_calibboard_data_candidate.size())
    {
        ui.sp_loadCalibBoardDataEdit->appendPlainText(QObject::tr("还未加载外标定三维点真实数据\n"));
        return;
    }
    ui.sp_coorX->setText(QString("%1").arg(sp_calibboard_data_candidate[index].x));
	ui.sp_coorY->setText(QString("%1").arg(sp_calibboard_data_candidate[index].y));
	ui.sp_coorZ->setText(QString("%1").arg(sp_calibboard_data_candidate[index].z));
}

//icp
void LZ_Calibration::sp_calculate()
{
	// 2d points
	sp_final_lft_pts;
	sp_final_rgt_pts;
	// triangle
	for(vector<Point2f>::iterator it = sp_final_lft_pts.begin();it != sp_final_lft_pts.end();++it)
	{
		qDebug()<<(*it).x;
		qDebug()<<(*it).y;
		qDebug();
	}
	cv::Mat pnts3D(1,sp_points.size(),CV_64FC4);
	triangulatePoints(P1,P2, sp_final_lft_pts, sp_final_rgt_pts,
		pnts3D);

	rterr = calcRt(sp_points, pnts3D, R, T);
	ui.cali_sp_error->setText( QString::number(rterr, 'f') );
}

double LZ_Calibration::calcRt(vector<Point3f> sp_points, Mat templat, Mat &R, Mat &T)
{
	//cv::Mat pnts3D(1,sp_points.size(),CV_32FC4);


	templat.row(0) /= templat.row(3);
	templat.row(1) /= templat.row(3);
	templat.row(2) /= templat.row(3);
	templat.row(3) /= templat.row(3);

	// 3d points	
	double* pd = (double*)calloc(3*sp_points.size(),sizeof(double));
	double * target = (double*)calloc(3*sp_points.size(),sizeof(double));
	for (int i = 0; i < sp_points.size(); i++ )
	{
		pd[i*3+0] = templat.at<float>(0, i);
		pd[i*3+1] = templat.at<float>(1, i);
		pd[i*3+2] = templat.at<float>(2, i);
	}
	for (int i=0; i<sp_points.size(); i++)
	{
		target[i*3+0] = sp_points[i].x;
		target[i*3+1] = sp_points[i].y;
		target[i*3+2] = sp_points[i].z;
	}

	// icp
	Matrix r = Matrix::eye(3);
	Matrix t(3,1);
	IcpPointToPoint icp(target,sp_points.size(),3);
	icp.setMaxIterations(1);
	icp.setSubsamplingStep(1);
	icp.fit(pd,sp_points.size(),r,t,-1);

	R.create(3,3,CV_32FC1);
	T.create(3,1,CV_32FC1);
	//icp Matrix m.val[col][row]
	for(int i=0;i<3;i++)
	{
		for(int j=0;j<3;j++)
		{
			R.at<float>(i,j) = r.val[i][j];
		}
	}
	for(int i=0;i<3;i++)
		T.at<float>(i,0) = t.val[i][0]; 

	free(target);
	free(pd);

	//vector<Point3f> sp_points
	// Mat templat 4*N

	Mat Rotated = R * ( templat.rowRange(0,3) );
	for (int i=0; i<Rotated.cols ;i++)
	{
		Rotated.at<float>(0,i) += T.at<float>(0,0);
		Rotated.at<float>(1,i) += T.at<float>(1,0);
		Rotated.at<float>(2,i) += T.at<float>(2,0);
	}

	vector<Point3f> tmp;
	for (int i=0; i<Rotated.cols; i++)
	{
		tmp.push_back(
			Point3f( Rotated.at<float>(0,i),
			Rotated.at<float>(1,i),
			Rotated.at<float>(2,i) )
			);
	}

	double err = norm(Mat(sp_points), Mat(tmp), CV_L2);
	int siz = sp_points.size();
	err = sqrt(err*err/siz);

	return err;
}