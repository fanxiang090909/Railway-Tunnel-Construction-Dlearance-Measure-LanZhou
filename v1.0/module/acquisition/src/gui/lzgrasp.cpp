#include "lzgrasp.h"
#include "xmlnetworkfileloader.h"
#include "network_config_list.h"

/**
 * 兰州双目标定采集程序界面
 * @author 丁志宇
 * @date 20141201
 * @version 1.0.0
 */
LzGrasp::LzGrasp(QWidget *parent, Qt::WFlags flags)
	: QWidget(parent, flags), 
	lpath("left\\"),
	rpath("right\\"),
	suffix(".bmp"),
	save_count(0),
	right_index(-1),
	left_index(-1),
    slaveModel(1.0)
{
	ui.setupUi(this);


    this->setWindowIcon(QIcon(":/cali_icon.png"));

    // 加载本地文件存储目录
    ui.camgroup_sellect->setEnabled(false);
    QFile pathfile("slave_datastore_path.txt");
    if (pathfile.open(QIODevice::ReadOnly))
    {
        QString parentpath = pathfile.readLine().trimmed();
   		QString myslaveip = pathfile.readLine().trimmed();
        pathfile.close();
        QString network_file = parentpath + "/system/network_config.xml";
        QFile file0(network_file);
        if (file0.exists())
        {
            if(file0.exists())
            {
                file0.close();
                qDebug() << QObject::tr("解析network_config文件%1，从中找%2的SlaveModel").arg(parentpath).arg(myslaveip);//在控制台上显示
                // @author fanxiang注文件解析

                XMLNetworkFileLoader* ff2 = new XMLNetworkFileLoader(network_file);
                networkfileok = ff2->loadFile();
                delete ff2;
                if (networkfileok)
                {
                    //NetworkConfigList::getNetworkConfigListInstance()->showList();

                    networkfileok = NetworkConfigList::getNetworkConfigListInstance()->findSlave(myslaveip, slaveModel);

                    if (networkfileok)
                    {
                        ui.camgroup_sellect->setEnabled(true);
                        ui.camgroup_sellect->insertItem(0, QString("%1").arg(slaveModel.box1.boxindex));
                        ui.camgroup_sellect->insertItem(1, QString("%1").arg(slaveModel.box2.boxindex));
                        connect(ui.camgroup_sellect, SIGNAL(currentIndexChanged(int)), this, SLOT(sellectCamGroup(int)));
                        sellectCamGroup(0);
                    }
			        else
                        ui.camgroup_sellect->setEnabled(false);
                }
            }
            file0.close();
        }
    }


	connect(ui.confirmbutton, SIGNAL(clicked()), this, SLOT(comfirmSetup()));
	connect(ui.savebutton, SIGNAL(clicked()), this, SLOT(saveframe()));
	connect(ui.graspbutton, SIGNAL(clicked()), this, SLOT(graspframe()));

    // 添加键盘快捷键，采集和存储
    QAction* actionCollect = new QAction(this);
    actionCollect->setShortcut(QKeySequence(Qt::Key_Space));
    QAction* actionSave = new QAction(this);
    actionSave->setShortcut(QKeySequence(Qt::Key_Enter));
    this->addAction(actionCollect);
    this->addAction(actionSave);
    // 添加键盘快捷键信号槽
    connect(actionCollect, SIGNAL(triggered()), this, SLOT(graspframe()));
    connect(actionSave, SIGNAL(triggered()), this, SLOT(saveframe()));
}

LzGrasp::~LzGrasp()
{
	cameras.StopGrabbing();
	cameras.Close();
}

void LzGrasp::comfirmSetup()
{
	blobsize.width = ui.blobsize_min->text().toInt();
	blobsize.height = ui.blobsize_max->text().toInt();
	graythreshold = ui.graythresh->text().toInt();

	left_camseq = ui.camseq_left->text().toStdString();
	right_camseq = ui.camseq_right->text().toStdString();
	exposuretime = ui.exposuretime->text().toInt();

	patternsize.width = ui.patternsize_col->text().toInt();
	patternsize.height = ui.patternsize_row->text().toInt();

	//create blobdetector
	SimpleBlobDetector::Params params;
	params.blobColor = 255;

	params.minThreshold = graythreshold;
	params.minArea = blobsize.width;
	params.maxArea = blobsize.height;

	params.filterByCircularity = false;
	params.filterByConvexity = false;
	params.filterByInertia = false;
	blob_detector = new cv::SimpleBlobDetector(params);
	blob_detector->create("SimpleBlob");
	///////////

	//init camera
	try
	{
		CTlFactory& tlFactory = CTlFactory::GetInstance();
		DeviceInfoList_t devices;
		tlFactory.EnumerateDevices( devices );

		if ( devices.empty() || devices.size() < 2 )
		{
			cout << "Not enough camera present." << endl;
			throw RUNTIME_EXCEPTION( "Not enough camera present.");
		}

		cameras.Initialize( devices.size() );
		cameras.DestroyDevice();

		// Attach devices
		for ( size_t i = 0; i < devices.size(); i++)
			cameras[i].Attach( tlFactory.CreateDevice( devices[i] ) );

		// check camera serial id
		String_t lid( left_camseq.c_str() );
		String_t rid( right_camseq.c_str() );

		for ( int i = 0; i < cameras.GetSize(); i++ )
		{
			if ( cameras[i].GetDeviceInfo().GetSerialNumber() == lid )
				left_index = i;
			if ( cameras[i].GetDeviceInfo().GetSerialNumber() == rid )
				right_index = i;
		}
		if (left_index==-1 || right_index==-1)
		{
			cout << "haven't found correct device, please check camera serial ids and restart!" << endl;
			throw RUNTIME_EXCEPTION( "Not enough camera present.");
		}
	}
	catch(std::exception& ex)
	{

		throw(ex);
	}

	cameras[left_index].RegisterConfiguration( new CameraRegularConfiguration(true, exposuretime),  RegistrationMode_ReplaceAll, Cleanup_Delete);
	cameras[right_index].RegisterConfiguration( new CameraRegularConfiguration(false, exposuretime),  RegistrationMode_ReplaceAll, Cleanup_Delete);

	cameras.Open();
	cameras.StartGrabbing(GrabStrategy_OneByOne);
	///////////

	//create path and file
	_mkdir("left");
	_mkdir("right");
	///////////

}

void LzGrasp::graspframe()
{
	// send trigger
	if ( cameras[left_index].WaitForFrameTriggerReady(1000, TimeoutHandling_ThrowException) )
	{
		if ( cameras[right_index].WaitForFrameTriggerReady(1000, TimeoutHandling_ThrowException) )
		{
			cameras[left_index].TriggerSoftware.Execute();
			cameras[right_index].TriggerSoftware.Execute();
		}
	}
	// retrieve result

	cameras[left_index].RetrieveResult(5000, lptrGrabResult,  TimeoutHandling_ThrowException);
	cameras[right_index].RetrieveResult(5000, rptrGrabResult,  TimeoutHandling_ThrowException);

	// convert mat
	cvt.convert((char*)(lptrGrabResult->GetBuffer()), leftimg);
	cvt.convert((char*)(rptrGrabResult->GetBuffer()), rightimg);

	// copy to left and right
	Mat lefttmp, righttmp;
	lefttmp = leftimg.clone();
	righttmp = rightimg.clone();

	// detect blob and draw
	bool leftfound,rightfound;
	vector<Point2f> leftcorners;
	vector<Point2f> rightcorners;

	leftfound = findCirclesGrid(lefttmp, patternsize, leftcorners, CALIB_CB_CLUSTERING|CALIB_CB_SYMMETRIC_GRID, blob_detector);
	rightfound = findCirclesGrid(righttmp, patternsize, rightcorners, CALIB_CB_CLUSTERING|CALIB_CB_SYMMETRIC_GRID, blob_detector);

	cvtColor(lefttmp, lefttmp, CV_GRAY2BGR);
	cvtColor(righttmp, righttmp, CV_GRAY2BGR);

	drawChessboardCorners(lefttmp, patternsize, leftcorners, leftfound);
	drawChessboardCorners(righttmp, patternsize, rightcorners, rightfound);


	// show in ui
	ui.img_left->setPixmap( QPixmap::fromImage(MatToQImage(lefttmp)).scaled(QSize(256,320)) );
	ui.img_right->setPixmap( QPixmap::fromImage(MatToQImage(righttmp)).scaled(QSize(256,320)) );
}

void LzGrasp::saveframe()
{
	if (leftimg.empty() || rightimg.empty())
		return;

	ss.str("");
	ss << save_count++;
	count = ss.str();
	imwrite(lpath+count+suffix, leftimg);
	imwrite(rpath+count+suffix, rightimg);

}

void LzGrasp::sellectCamGroup(int index)
{
    if (networkfileok)
    {
        QString current = ui.camgroup_sellect->currentText();
        QString boxid = slaveModel.box1.boxindex;
        if (boxid.compare(current) == 0)
        {
            ui.camseq_left->setText(slaveModel.box1.camera_ref_sn.c_str());
            ui.camseq_right->setText(slaveModel.box1.camera_sn.c_str());
            return;
        }
        boxid = slaveModel.box2.boxindex;
        if (boxid.compare(current) == 0)
        {
            ui.camseq_left->setText(slaveModel.box2.camera_ref_sn.c_str());
            ui.camseq_right->setText(slaveModel.box2.camera_sn.c_str());
            return;
        }
        ui.camseq_left->setText("");
        ui.camseq_right->setText("");
    }
}