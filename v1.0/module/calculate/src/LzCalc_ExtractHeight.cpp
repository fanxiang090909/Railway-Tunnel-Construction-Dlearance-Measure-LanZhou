#include "LzCalc_ExtractHeight.h"
#include <QFile>

LzCalculate_ExtractHeight::LzCalculate_ExtractHeight()
{
    hasinit_can_calc = false;
    hasinit_has_RTfile = false;
    hasinit_syn_rectify_input_file = false;

    // 文件操作类
    // 隧道_日期.fdat
    lzMatFuse = NULL;
    
    // 隧道_日期_Q.mdat
    lzMat_Q = NULL;
    // 隧道_日期_R.mdat
    lzMat_R = NULL;
    
    // 隧道_日期.syn
    extra_high = NULL;

    Item = list<int>();

    // @author 范翔，是否终止计算
    ifsuspend = false;
    // @author 范翔，火车行驶时（计划配置时）车厢正反
    carriagedirect = true;
    isinterruptfile = false;
}

LzCalculate_ExtractHeight::~LzCalculate_ExtractHeight()
{
    if (lzMatFuse)
    {
        delete lzMatFuse;
        lzMatFuse = NULL;
    }
    if (lzMat_Q)
    {
        delete lzMat_Q;
        lzMat_Q = NULL;
    }
    if (lzMat_R)
    {
        delete lzMat_R;
        lzMat_R = NULL;
    }
    if (extra_high)
    {
        delete extra_high;
        extra_high = NULL;
    }

    Item.clear();
}

bool LzCalculate_ExtractHeight::init(list<int> initHeights, string initfusefile_no_RT, string initQmdat_file, string initRmdat_file, string initsyn_rectify_input_file, string initsyn_output_file)
{
    if (lzMatFuse)
    {
        delete lzMatFuse;
        lzMatFuse = NULL;
    }
    if (lzMat_Q)
    {
        delete lzMat_Q;
        lzMat_Q = NULL;
    }
    if (lzMat_R)
    {
        delete lzMat_R;
        lzMat_R = NULL;
    }
    if (extra_high)
    {
        delete extra_high;
        extra_high = NULL;
    }

    Item.clear();
    Item = initHeights;

    this->fusefile_no_RT = initfusefile_no_RT;
    this->Qmdat_file = initQmdat_file;
    this->Rmdat_file = initRmdat_file;
    if (Qmdat_file.compare("") == 0 || Rmdat_file.compare("") == 0)
        hasinit_has_RTfile = false;
    else
        hasinit_has_RTfile = true;

    this->syn_rectify_input_file = initsyn_rectify_input_file;
    if (syn_rectify_input_file.compare("") == 0)
        hasinit_syn_rectify_input_file = false;
    else
    {
        vals.clear();
        std::list<int>::iterator it = Item.begin();
        while (it != Item.end())
        {
            RectifyFactor i;
            i.l_a = 0;
            i.l_b = 0;
            i.l_c = 0;
            i.l_d = 0;
            i.r_a = 0;
            i.r_b = 0;
            i.r_c = 0;
            i.r_d = 0;
            vals.insert(std::pair<int,RectifyFactor>((*it),i));
            it++;
        }

        loadRectifyData(syn_rectify_input_file);
        hasinit_syn_rectify_input_file = true;
    }

    this->syn_output_file = initsyn_output_file;

    if (fusefile_no_RT.compare("") != 0 && syn_output_file.compare("") != 0)
        hasinit_can_calc = true;
    else
        hasinit_can_calc = false;
    return hasinit_can_calc;
}

bool LzCalculate_ExtractHeight::loadRectifyData(string filename)
{
    QFile file;
    QString in;

    file.setFileName(QString::fromLocal8Bit(filename.c_str()));

    // Currently here for debugging purposes
    qDebug() << "Reading file: " << QString::fromLocal8Bit(filename.c_str()) << endl;


    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream inStream(&file);
        
        // readfile
        in = inStream.readLine();
        in = inStream.readLine();

        int height;
        float l_a;
        float l_b;
        float l_c;
        float l_d;
        float r_a;
        float r_b;
        float r_c;
        float r_d;

        while(!(in.isNull()))
        {
            height = in.section("\t",0,0).toInt();
            l_a = in.section("\t",1,1).toFloat();
            l_b = in.section("\t",2,2).toFloat();
            l_c = in.section("\t",3,3).toFloat();
            l_d = in.section("\t",4,4).toFloat();

            r_a = in.section("\t",5,5).toFloat();
            r_b = in.section("\t",6,6).toFloat();
            r_c = in.section("\t",7,7).toFloat();
            r_d = in.section("\t",8,8).toFloat();

            vals.at(height).l_a = l_a;
            vals.at(height).l_b = l_b;
            vals.at(height).l_c = l_c;
            vals.at(height).l_d = l_d;
            vals.at(height).r_a = r_a;
            vals.at(height).r_b = r_b;
            vals.at(height).r_c = r_c;
            vals.at(height).r_d = r_d;


            in = inStream.readLine();
        }

        file.close();

        qDebug() << "read_end" << endl;
    }

    // Debug
    std::map<int,RectifyFactor>::iterator it = vals.begin();
    while (it != vals.end())
    {
        std::pair<int,RectifyFactor> pair = (*it);

        qDebug() << pair.first << pair.second.l_a << pair.second.l_b << pair.second.l_c << pair.second.l_d << 
                                  pair.second.r_a << pair.second.r_b << pair.second.r_c << pair.second.r_d;
        it++;
    }
    return true;
}

/**
 * 加校正系数校正提高度结果
 */
void LzCalculate_ExtractHeight::rectifyHeight(SectionData & data, bool safetyfactor)
{
    std::map<int,item>::iterator it = data.getMaps().begin();
    int tempkey;
    float leftVal;
    float rightVal;
    RectifyFactor rectifyFactor;
    while (it != data.getMaps().end())
    {
        std::pair<int,item> pair = (*it);
        tempkey = (*it).first;
        leftVal = (*it).second.left;
        rightVal = (*it).second.right;
        rectifyFactor = vals.at(tempkey);

        // 公式计算
        // 如果不加安全缩减因子
        if (leftVal > 0)
            leftVal += rectifyFactor.l_a * leftVal + rectifyFactor.l_b;
        if (rightVal > 0)
            rightVal += rectifyFactor.r_a * rightVal + rectifyFactor.r_b;

        // 如果加安全缩减因子
        if (safetyfactor)
        {
            if (leftVal > 0)
                leftVal += rectifyFactor.l_c * leftVal + rectifyFactor.l_d;
            if (rightVal > 0)
                rightVal += rectifyFactor.r_c * rightVal + rectifyFactor.r_d;
        }
        data.updateToMapVals(tempkey, leftVal, rightVal);
        
        it++;
    }
}

void LzCalculate_ExtractHeight::rectify_RT()                                                 //对三维点向量进行RT矫正
{
	 /*out_pnts = Mat(3,fus_vector.size(),CV_64FC4);
	 int i = 0;
	 for(Vector<Point3d>::iterator it= fus_vector.begin(); it!=fus_vector.end(); it++)
	 {
	    qDebug() << "原点： " << "  x: " << (*it).x <<  "  y: " << (*it).y << endl;
		 *out_pnts.ptr<float>(0,i) = (*it).x*cos(*RT_point.ptr<double>(0,2)/180*3.14159265)-(*it).y*sin(*RT_point.ptr<double>(0,2)/180*3.14159265)+(*RT_point.ptr<double>(0,1));
		 *out_pnts.ptr<float>(1,i) = (*it).y*cos(*RT_point.ptr<double>(0,2)/180*3.14159265)+(*it).x*sin(*RT_point.ptr<double>(0,2)/180*3.14159265)+(*RT_point.ptr<double>(0,0));
		 *out_pnts.ptr<float>(2,i) = (*it).z;
		 (*it).x = *out_pnts.ptr<float>(0,i);
		 (*it).y = *out_pnts.ptr<float>(1,i);
		 (*it).z = *out_pnts.ptr<float>(2,i);
		 std::cout<<"矫正点： " <<"x: "<<(*it).x<< "  y: " << (*it).y <<endl;
		 std::cout<<endl;
		 i++;
	 }*/
}

/**
 * Mat(3,fus_vector.size(),CV_64FC4);
 */
void LzCalculate_ExtractHeight::Vector2Mat(Vector<Point3d> & input, Mat & output)
{
    int i = 0;
	for (Vector<Point3d>::iterator it= input.begin(); it != input.end(); it++)
    {
	    *output.ptr<float>(0,i) = (*it).x;
        *output.ptr<float>(1,i) = (*it).y;
        *output.ptr<float>(2,i) = (*it).z;
	    i++;
    }
}

void LzCalculate_ExtractHeight::get_vector(Mat& mat_point, Vector<Point3d> & fus_vector, int cameragroupid, bool carriagedirection)
{
    Point3d point;
    // @author 范翔 车厢正反对x值的影响，使得结果浏览时符合火车真实前行时人的直观左右认知
    int a = 1;
    if (!carriagedirection)
        a = -1;
    if(4 != mat_point.cols)
        for(int i=0; i<mat_point.cols; i++)                          
        {
            point.x = (*mat_point.ptr<double>(0,i));
            //////////TODO TO DELETE////////
            // (-1) 是因为外标定的坐标系与车厢正厢正向坐标系相反，建议下次外标定点测量时采用所规定的车厢正向坐标系
            point.y = a*(-1)*(*mat_point.ptr<double>(1,i));
            point.z = cameragroupid;//*mat_point.ptr<float>(2,i);
            fus_vector.push_back(point);
        }
}

void LzCalculate_ExtractHeight::Mat2Vector(Mat & input, Vector<Point3d> & output)
{
    Point3d point;
    for (int i = 0; i < input.cols; i++)
    {
        point.x = (*input.ptr<float>(0,i));
        point.y = (*input.ptr<float>(1,i));
        point.z = (*input.ptr<float>(2,i));
        output.push_back(point);
    }
}

int LzCalculate_ExtractHeight::extract_height(Vector<Point3d> & fus_vector, SectionData & Data, float & center_height)
{
	int blank_step = 75;

    center_height = -1;
    float center_height_HI = -1, minx = 10000;
	for(list<int>::iterator it = Item.begin(); it != Item.end() ;it++)                      //对List进行遍历获取点
	{
		bool flag = false;
		vector<Point2d> around_H;                                                           //在指定点没有高度的情况下用附近数据拟合高度
	    vector<Point2d> result;
	    Point2d Pts;
	    Point2d mean_1,mean_2;                                                              //当无指定高度的情况下计算平均高度
	    bool postive = false;                                                               //用于指明有无指定高度数据
	    bool mean_2_init = false;
	    bool mean_1_init = false;
		for(Vector<Point3d>::iterator point = fus_vector.begin(); point != fus_vector.end(); point++)
	    {
            // 中心顶部净高
            if ((*point).x > center_height)
                center_height = (*point).x;
            /*if ((*point).x < minx)
                minx = (*point).x;*/
            if ((*point).z >= 7.5 && (*point).z <= 9.5)
            {
                if ((*point).x > center_height_HI)
                    center_height_HI = (*point).x;
            }

		    if((*point).x==*it)
		    {
			    Pts.x = (*point).x;
			    Pts.y = (*point).y;
			    result.push_back(Pts);
			    if(result.size()==2)
			       break;
		    }
			else if (abs((*point).x-*it)<blank_step)
		    {
			   Pts.x = (*point).x;
			   Pts.y = (*point).y;
			   around_H.push_back(Pts);
		    }
	   }
	   if(around_H.size()!=0&&result.size()!=2) 
	   {
	        mean_1.x = 0;
	        mean_1.y = 0;
		    mean_2.x = 0;
		    mean_2.y = 0;
		    if(0<=around_H.at(0).y)
			     postive = true;
		    for(vector<Point2d>::iterator point = around_H.begin(); point != around_H.end(); point++)
	        {
			    if(postive&&(*point).y>0||!postive&&(*point).y>0) 
		        {
                    mean_1.y += (*point).y;
                    mean_1.x++;
				    mean_1_init = true;
		        }
		        else
		        {
                    mean_2.y += (*point).y;
                    mean_2.x++;
			        mean_2_init = true;
 		        }

	      }
		  if(result.size()==0)
		  {
			  if(mean_1_init==true)
			  {
                  Pts.x = *it;
	              Pts.y = mean_1.y/mean_1.x;
		          result.push_back(Pts);
			  }
		      if(mean_2_init==true)
		      {
		          Pts.x = *it;
				  Pts.y = mean_2.y/mean_2.x;
		          result.push_back(Pts);
		      }
		  }
		  else if(result.size()==1)
		  {
			   if(mean_1_init==true&&blank_step<abs(result.at(0).x-(mean_1.y/mean_1.x)))
			   {
	              Pts.x = *it;
	              Pts.y = mean_1.y/mean_1.x;
		          result.push_back(Pts);
			   }
			   if(mean_2_init==true&&blank_step<abs(result.at(0).x-(mean_2.y/mean_2.x)))
			   {
	              Pts.x = *it;
	              Pts.y = mean_2.y/mean_2.x;
		          result.push_back(Pts);
			   }
			
            }

	    }
	    if(result.size()==0)
	    {
		    Data.updateToMapVals(*it,float(0),float(0));
        }
	    else
	    {	
		    for(int i=0;i<result.size();i++)
	        {
                if(result.at(i).y<0)                                 //小于0为左侧距离
	                Data.updateToMapVals(*it,(-1)*result.at(i).y,true);
                else
	                Data.updateToMapVals(*it,result.at(i).y,false);  //大于0为右侧高度
	        }
	    }
    }
    // 大于最高和最低的赋零，暂时弃用，画图时做判断
    /*std::map<int,item>::iterator it = Data.getMaps().begin();
    int tempkey;
    while (it != Data.getMaps().end())
    {
        std::pair<int,item> pair = (*it);
        tempkey = pair.first;
        if (tempkey > center_height || tempkey < minx)
        {
	         Data.updateToMapVals(tempkey,0,true);
   	         Data.updateToMapVals(tempkey,0,false);
        }
        it++;
    }*/
    return 0;
}

int LzCalculate_ExtractHeight::run()
{
    if (!hasinit_can_calc)
    {
        return 6; // 没有任何数据
    }

    lzMatFuse = new LzSerialStorageMat();
    lzMatFuse->setting(250, 1024*1024*500, true);
        
    extra_high = new LzSerialStorageSynthesis(); 
    extra_high->setting(250, 1024*1024*50, true);

    ifsuspend = false;

    int ret;
    if (!isinterruptfile)
    {
        bool flag1 = extra_high->createFile(syn_output_file.c_str());
    }
    bool is_open = lzMatFuse->openFile(fusefile_no_RT.c_str());
    bool flag2 = extra_high->openFile(syn_output_file.c_str());
    if (!is_open || !flag2)
    {
        return 5;
    }

    std::vector<BLOCK_KEY> inputkeys = lzMatFuse->readKeys();

    BLOCK_KEY startfr = inputkeys.at(0);
    BLOCK_KEY endfr = inputkeys.at(inputkeys.size() - 1);

    BlockInfo Info;
    Info.key = startfr;

    // @author 范翔改
    // 记录18组文件是否有数据
    DataHead towritedatahead;
    // @author 范翔添加
    carriagedirect = true;
    
    // 如果首次创建文件（非暂停后继续），则写入文件头
    if (!isinterruptfile) // TODO +RT
    {
        strcpy(towritedatahead.camera_index, "SYN\0");
        towritedatahead.seqno = 0;
        extra_high->writeHead(&towritedatahead);
    }

    lzMatFuse->retrieveBlock(startfr);
    bool ifread = false;

    // TODO TODELETE
    double mile_count = 0;
	for(int frame_num = startfr; frame_num < endfr; frame_num++, mile_count+=1)  // mile_count++ TODO TODELETE
    {
        // 【Step1】 Mat转Vector
        Mat fuseMat;
        ifread = lzMatFuse->readMat(fuseMat);
        //Mat2Vector(fuseMat, fuse_vector);
        get_vector(fuseMat, fuse_vector, 0, false);

        // 【Step2】对vector进行rt矫正
        // 将17组从机vector放入一个vector中
        if (hasinit_has_RTfile)
        {
		    rectify_RT();                                                  
        }

      	BlockInfo tmpblockinfo;
        tmpblockinfo.isvalid = true;
        tmpblockinfo.key = frame_num;

	    SectionData Data;
	    bool retb = Data.initMaps();
        if (!retb)
        {
            // 终止计算
            lzMatFuse->closeFile();
            extra_high->closeFile();
            return -8;
        }
        float center_height = -1;

        // 【Step3】提取高度
		ret = extract_height(fuse_vector, Data, center_height);
        
        // 【Step4】系数校正
        if (hasinit_syn_rectify_input_file)
        {
            rectifyHeight(Data, false);
        }

        extra_high->writeMap(frame_num, mile_count, center_height, Data.getMaps(), tmpblockinfo, true);    //参数需要加！！！！


        fuse_vector.clear();
        if (ret != 0)
        {
            // 终止计算
            lzMatFuse->closeFile();
            extra_high->closeFile();
            return ret;
        }
        qDebug() << "fuse " << Info.key;
        // @author 范翔改 继承自QObject 可以通过消息槽反馈计算进度
        emit statusShow(Info.key, false);

        // 是否终止计算，传递当前计算帧号
        if (ifsuspend)
        {
            emit statusShow(Info.key, true);
            // 关闭文件
            lzMatFuse->closeFile();
            extra_high->closeFile();
            return -1; // 暂停计算返回
        }

        Info.key++;
        fuse_vector.clear();
	}

    lzMatFuse->closeFile();
    extra_high->closeFile();
	//system("pause");
    return ret;
}