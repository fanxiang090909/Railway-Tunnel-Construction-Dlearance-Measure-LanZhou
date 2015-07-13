#include <iostream>

#include "LzSerialStorageSynthesis.h"
#include "clearance_item.h"
#include "LzSynth.h"
#include "headdefine.h"
#include "connectdb.h"
#include "xmlsynthesisheightsfileloader.h"
#include <QDebug>

using namespace std;

int main(int argc, char *argv[])
{
    std::cout << "隧道综合";

	XMLSynthesisHeightsFileLoader * ff3 = new XMLSynthesisHeightsFileLoader("output_heights.xml");
    ff3->loadFile();
    delete ff3;

    string filename = "范翔_20140122.syn";

	// 【注意！】必须要先init再用
	SectionData data;
    bool ret = data.initMaps(); // 初始化高度，不可缺少
	if (ret == true)
	{
		float valss = 20;
		data.resetMaps();
        data.updateToMapVals(150,3000,true);//true表示左侧距离，false表示右侧距离
        data.updateToMapVals(200,2589,true);
        data.updateToMapVals(300,2600+valss,true);
        data.updateToMapVals(350,2732+valss,true);
        data.updateToMapVals(500,2733+valss,true);
        data.updateToMapVals(600,2732,true);
        data.updateToMapVals(800,2744+valss,true);
        data.updateToMapVals(1000,2722,true);
        data.updateToMapVals(1100,2733,true);
        data.updateToMapVals(1210,2744,true);
        data.updateToMapVals(1250,2730,true);
        data.updateToMapVals(1600,2738,true);
        data.updateToMapVals(2000,2744,true);
        data.updateToMapVals(2400,2744,true);
        data.updateToMapVals(2800,2695,true);
        data.updateToMapVals(3000,2880,true);
        data.updateToMapVals(3200,2911,true);
        data.updateToMapVals(3600,2933,true);
        data.updateToMapVals(3800,2900,true);
        data.updateToMapVals(4000,2872,true);
        data.updateToMapVals(4200,2823,true);
        data.updateToMapVals(4400,2774,true);
        data.updateToMapVals(4500,2720,true);
        data.updateToMapVals(4600,2695,true);
        data.updateToMapVals(4800,2628,true);
        data.updateToMapVals(5000,2549,true);
        data.updateToMapVals(5200,2464,true);
        data.updateToMapVals(5400,2373,true);
        data.updateToMapVals(5500,2330,true);
        data.updateToMapVals(5600,2269,true);
        data.updateToMapVals(5800,2160,true);
        data.updateToMapVals(6000,2038+valss,true);
        data.updateToMapVals(6200,2000,true);
        data.updateToMapVals(6400,1940,true);
        data.updateToMapVals(6600,1700,true);
        data.updateToMapVals(7000,1200,true);
        data.updateToMapVals(7500,600,true);
        data.updateToMapVals(8000,0,true);

        data.updateToMapVals(150,3000,false);//true表示左侧距离，false表示右侧距离
        data.updateToMapVals(200,2589,false);
        data.updateToMapVals(300,2600,false);
        data.updateToMapVals(350,2732,false);
        data.updateToMapVals(500,2733,false);
        data.updateToMapVals(600,2732,false);
        data.updateToMapVals(800,2744,false);
        data.updateToMapVals(1000,2722,false);
        data.updateToMapVals(1100,2733,false);
        data.updateToMapVals(1210,2744,false);
        data.updateToMapVals(1250,2730,false);
        data.updateToMapVals(1600,2738,false);
        data.updateToMapVals(2000,2744,false);
        data.updateToMapVals(2400,2744,false);
        data.updateToMapVals(2800,2695,false);
        data.updateToMapVals(3000,2880,false);
        data.updateToMapVals(3200,2911,false);
        data.updateToMapVals(3600,2933+valss,false);
        data.updateToMapVals(3800,2900,false);
        data.updateToMapVals(4000,2872,false);
        data.updateToMapVals(4200,2823,false);
        data.updateToMapVals(4400,2774,false);
        data.updateToMapVals(4500,2720,false);
        data.updateToMapVals(4600,2695,false);
        data.updateToMapVals(4800,2628,false);
        data.updateToMapVals(5000,2549,false);
        data.updateToMapVals(5200,2464,false);
        data.updateToMapVals(5400,2373,false);
        data.updateToMapVals(5500,2330,false);
        data.updateToMapVals(5600,2269,false);
        data.updateToMapVals(5800,2160,false);
        data.updateToMapVals(6000,2038,false);
        data.updateToMapVals(6200,2000,false);
        data.updateToMapVals(6400,1940,false);
        data.updateToMapVals(6600,1700,false);
        data.updateToMapVals(7000,1200,false);
        data.updateToMapVals(7500,600,false);
        data.updateToMapVals(8000,0,false);
		//data.showMaps();
		//data.resetMaps();
		data.showMaps();
	
		LzSerialStorageSynthesis * syn = new LzSerialStorageSynthesis();
		// 【注意！】设置缓存大小1M，默认1G太大
		syn->setting(100, 1024*1024, true);

		bool ret;

		/**********流式文件********/
		/**********写文件**********/
		float milew = 355.5;
		long framecounterw = 202;
		valss = 0.5;

		ret = syn->createFile(filename.c_str());
		if (ret == false)
			/*TODO*/;
        ret = syn->openFile(filename.c_str());
		if (ret == false)
			/*TODO*/;

		// 写文件头
		DataHead head;
		head.carriage_direction = false;	// 正向 【用】
		head.is_normal = true;				// 正常拍摄（正序）【用】
		head.start_mileage = 33333;			// 隧道起始里程 【用于check与数据库中的信息是否发生改变】
		head.end_mileage = 44445;			// 隧道终止里程 【用于check与数据库中的信息是否发生改变】
		head.is_double_line = false;		// 是否双线 【用于check与数据库中的信息是否发生改变】
		head.tunnel_id = 6200;				// 隧道ID 【用】
		strcpy(head.tunnel_name, (string("范翔计划")).c_str());
		strcpy(head.camera_index, "HH");
		strcpy(head.datetime, "20140122 19:00");

		syn->writeHead(&head);

		//char block[500] = {'\0'};
		BlockInfo tmpblockinfo;
	
		for (int i = 0; i < 1000; i++)
		{
			//【方案二】指定key，传入小头引用
			tmpblockinfo.isvalid = true;
			tmpblockinfo.key = framecounterw;
		
			//syn->writeBlock(block, 500);
			//block[0] += 1;
			//【方案一】未指定key
			//syn->writeMap(milew, framecounterw, data.getMaps());
			//【方案二】指定key
			syn->writeMap(framecounterw, milew, data.getMaps(), tmpblockinfo, true);

			qDebug() << "write " << i << "times.";
			milew += 0.49;
			framecounterw++;
			valss += 50;
			data.updateToMapVals(150,3000,true);//true表示左侧距离，false表示右侧距离
			data.updateToMapVals(200,2589,true);
			data.updateToMapVals(300,2600+valss,true);
			data.updateToMapVals(350,2732+valss,true);
			data.updateToMapVals(500,2733+valss,true);
			data.updateToMapVals(600,2732,true);
			data.updateToMapVals(800,2744+valss,true);
			data.updateToMapVals(1000,2722,true);
			data.updateToMapVals(1100,2733,true);
			data.updateToMapVals(1210,2744,true);
			data.updateToMapVals(1250,2730,true);
			data.updateToMapVals(1600,2738,true);
			data.updateToMapVals(2000,2744,true);
			data.updateToMapVals(2400,2744,true);
			data.updateToMapVals(2800,2695,true);
			data.updateToMapVals(3000,2880,true);
			data.updateToMapVals(3200,2911,true);
			data.updateToMapVals(3600,2933,true);
			data.updateToMapVals(3800,2900,true);
			data.updateToMapVals(4000,2872,true);
			data.updateToMapVals(4200,2823,true);
			data.updateToMapVals(4400,2774,true);
			data.updateToMapVals(4500,2720,true);
			data.updateToMapVals(4600,2695,true);
			data.updateToMapVals(4800,2628,true);
			data.updateToMapVals(5000,2549,true);
			data.updateToMapVals(5200,2464,true);
			data.updateToMapVals(5400,2373,true);
			data.updateToMapVals(5500,2330,true);
			data.updateToMapVals(5600,2269,true);
			data.updateToMapVals(5800,2160,true);
			data.updateToMapVals(6000,2038+valss,true);
			data.updateToMapVals(6200,2000,true);
			data.updateToMapVals(6400,1940,true);
			data.updateToMapVals(6600,1700,true);
			data.updateToMapVals(7000,1200,true);
			data.updateToMapVals(7500,600,true);
			data.updateToMapVals(8000,0,true);

			data.updateToMapVals(150,3000,false);//true表示左侧距离，false表示右侧距离
			data.updateToMapVals(200,2589,false);
			data.updateToMapVals(300,2600,false);
			data.updateToMapVals(350,2732,false);
			data.updateToMapVals(500,2733,false);
			data.updateToMapVals(600,2732,false);
			data.updateToMapVals(800,2744,false);
			data.updateToMapVals(1000,2722,false);
			data.updateToMapVals(1100,2733,false);
			data.updateToMapVals(1210,2744,false);
			data.updateToMapVals(1250,2730,false);
			data.updateToMapVals(1600,2738,false);
			data.updateToMapVals(2000,2744,false);
			data.updateToMapVals(2400,2744,false);
			data.updateToMapVals(2800,2695,false);
			data.updateToMapVals(3000,2880,false);
			data.updateToMapVals(3200,2911,false);
			data.updateToMapVals(3600,2933+valss,false);
			data.updateToMapVals(3800,2900,false);
			data.updateToMapVals(4000,2872,false);
			data.updateToMapVals(4200,2823,false);
			data.updateToMapVals(4400,2774,false);
			data.updateToMapVals(4500,2720,false);
			data.updateToMapVals(4600,2695,false);
			data.updateToMapVals(4800,2628,false);
			data.updateToMapVals(5000,2549,false);
			data.updateToMapVals(5200,2464,false);
			data.updateToMapVals(5400,2373,false);
			data.updateToMapVals(5500,2330,false);
			data.updateToMapVals(5600,2269,false);
			data.updateToMapVals(5800,2160,false);
			data.updateToMapVals(6000,2038,false);
			data.updateToMapVals(6200,2000,false);
			data.updateToMapVals(6400,1940,false);
			data.updateToMapVals(6600,1700,false);
			data.updateToMapVals(7000,1200,false);
			data.updateToMapVals(7500,600,false);
			data.updateToMapVals(8000,0,false);
			//data.showMaps();
		}

		data.updateToMapVals(2000, 200, false);

		// 重写某帧指定高度数据到文件中
		syn->rewriteMap(205, framecounterw, data.getMaps());

		syn->closeFile(); 

		/**********读文件**********/
		float mile;
		_int64 framecounter;
		if (!syn->openFile(filename.c_str()))
		{
			qDebug() << "can not open file:" << filename.c_str();
			delete syn;
			return 0;
		}
		qDebug() << "open file: " << filename.c_str();

		DataHead dataread;
		// 读文件头
		syn->readHead(&dataread);
		qDebug() << dataread.carriage_direction  << dataread.is_normal << dataread.tunnel_id << QString::fromLocal8Bit(dataread.tunnel_name) << dataread.datetime;

		std::vector<BLOCK_KEY> keys = syn->readKeys();
		qDebug() << "size:" << keys.size();
		for (int i = 0; i < keys.size(); i++)
			qDebug() << keys.at(i);
		//【方案一】未指定key，key从0开始++
		//ret = syn->retrieveMap(0);
		//【方案二】指定key
		ret = syn->retrieveMap(202);
		qDebug() << "retrive result:" << ret;
		for (int i = 0; i < keys.size(); i++)
		{
			// 在readMap中调用的blocktomap中已调用data.resetMaps();
			ret = syn->readMap(framecounter, mile, data);
			if (ret == false)
				continue;
			qDebug() << "read " << i << "times. return:" << ret;
			qDebug() << "mile:" << mile << ", framecounter:" << framecounter;
			data.showMaps();
		}
		syn->closeFile();

		delete syn;
	
		syn = NULL;
	}

	//XMLSynthesisHeightsFileLoader * ff3 = new XMLSynthesisHeightsFileLoader("output_heights.xml");
    //ff3->loadFile();
    //delete ff3;

    QString ip = "10.13.29.217";

    if (!CREATE_MYSQL_CONNECTION(ip))
		return 1;

    // 测试隧道综合
    TunnelDataModel * m = new TunnelDataModel(656);
    m->loadTunnelData();
    qDebug() << "id" << m->getId() << "name" << QString::fromLocal8Bit(m->getName().c_str()) << "idstd" << m->getIdStd().c_str()
             << "isdouble" << m->getIsDoubleLine() << "isdownlink" << m->getIsDownlink() << "isnew" << m->getIsNew()
             << "linetype" << m->getLineType() << "endpoint" << m->getEndPoint() << "startpoint" << m->getStartPoint()
             << "lineid" << m->getLineid() << "linename" << QString::fromLocal8Bit(m->getLinename().c_str());
    qDebug() << "list size:" << m->getCurveList().size() << m->getNumberOfLeftCurves() << m->getNumberOfRightCurves() << m->getNumberOfStrights();

    CheckedTunnelTaskModel tmp = CheckedTunnelTaskModel();
    PlanTask plan;
    plan.datetime = "20140121";
    plan.linename = "西工大";
    plan.linenum = 2222;
    plan.tunnelnum = 2345;
    plan.tunnelname = "范翔计划";
    plan.doubleline = true;
    CalcuFileItem calcuitem;
    calcuitem.cal_filename_prefix = "asdf";
    calcuitem.cal_framecounter_begin = 200;
    calcuitem.cal_framecounter_end = 1234;
    calcuitem.cal_valid_frames_num = calcuitem.cal_framecounter_end - calcuitem.cal_framecounter_begin + 1;
    for (int j = 0; j < 2; j++)
    {
        CheckedItem *ch = new CheckedItem();
        ch->filename_prefix = "aaaaa";
        ch->seqno = j+1;
        ch->start_frame_master = j;
        ch->end_frame_master = j+10000;
        ch->tunnel_name = "范翔";
        ch->tunnel_id = 123;
        tmp.pushback(*ch);
    }
    tmp.setPlanTask(plan);
    tmp.setCalcuItem(calcuitem);

    // 打开文件名
    string tunnelheightssynname = filename;
    ClearanceData straightdata;
    ClearanceData leftdata;
    ClearanceData rightdata;

    LzSynthesis lzsyn;
    lzsyn.initSynthesis(tunnelheightssynname, m, &tmp);
    bool hasstraight = false, hasleft = false,  hasright = false;
    lzsyn.synthesis(straightdata, leftdata, rightdata, hasstraight, hasleft, hasright);

    return 0;
}
