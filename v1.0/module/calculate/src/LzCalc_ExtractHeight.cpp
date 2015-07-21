#include "LzCalc_ExtractHeight.h"

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
        hasinit_syn_rectify_input_file = true;

    this->syn_output_file = initsyn_output_file;

    if (fusefile_no_RT.compare("") != 0 && syn_output_file.compare("") != 0)
        hasinit_can_calc = true;
    else
        hasinit_can_calc = false;
    return hasinit_can_calc;
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

int LzCalculate_ExtractHeight::extract_height(list<int> Item,LzSerialStorageSynthesis* extr_high, __int64 frame_num, double mile_count)
{
	int blank_step = 75;

	BlockInfo tmpblockinfo;
    tmpblockinfo.isvalid = true;
    tmpblockinfo.key = frame_num;

	SectionData Data;
	bool ret = Data.initMaps();
    if (!ret)
        return 1;

    float center_height = -1, center_height_HI = -1, minx = 10000;
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
    extr_high->writeMap(frame_num, mile_count, center_height, Data.getMaps(), tmpblockinfo, true);    //参数需要加！！！！
    return 0;
}

int LzCalculate_ExtractHeight::run()
{
    if (!hasinit_can_calc)
    {
        return -1;
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

    BlockInfo Info;
    Info.key = start_num;

    // @author 范翔改
    // 记录18组文件是否有数据
    bool hasdata = false;
    DataHead towritedatahead;
    // @author 范翔添加
    carriagedirect = true;
    
    // 如果首次创建文件（非暂停后继续），则写入文件头
    if (!isinterruptfile && hasdata) // TODO +RT
    {
        strcpy(towritedatahead.camera_index, "SYN\0");
        towritedatahead.seqno = 0;
        extra_high->writeHead(&towritedatahead);
    }
    else
        return 6; // 没有任何数据

    

    lzMatFuse->retrieveBlock(start_num);

    // TODO TODELETE
    double mile_count = 0;
	for(int frame_num = start_num; frame_num < start_num+frame_cunt; frame_num++, mile_count+=1)  // mile_count++ TODO TODELETE
    {
                                                  //将17组从机vector放入一个vector中
        if(hasinit_has_RTfile)
        {
		    rectify_RT();                                                  //对vector进行rt矫正
        }

		ret = extract_height(Item,extra_high,frame_num, mile_count);
        fus_vector.clear();
        ret = 0;
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
        fus_vector.clear();
	}

    lzMatFuse->closeFile();
    extra_high->closeFile();
	//system("pause");
    return ret;
}