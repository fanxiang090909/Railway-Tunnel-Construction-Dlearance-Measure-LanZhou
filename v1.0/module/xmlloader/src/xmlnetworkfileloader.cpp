#include "xmlnetworkfileloader.h"

#include "slavemodel.h"
#include "network_config_list.h"
#include <QDebug>
#include <string>
using namespace std;

/**
 * Network网络配置的XML文件解析实现
 * 继承自XMLFileLoader
 * @see XMLFileLoader
 * @author fanxiang
 * @date 20131107
 * @version 1.0.0
 */
XMLNetworkFileLoader::XMLNetworkFileLoader(QString filename) : XMLFileLoader(filename)
{

}

XMLNetworkFileLoader::~XMLNetworkFileLoader()
{

}

// 子类实现父类的函数
bool XMLNetworkFileLoader::loadFile()
{
    mutex.lock();
    NetworkConfigList::getNetworkConfigListInstance()->clear();

    TiXmlDocument * doc = new TiXmlDocument();
    // 【注意！】QString转cchar不能用QString().toStdString().c_str()，否则可能会出问题
    QByteArray tempstr = getFilename().toLocal8Bit();
    bool hasfile = doc->LoadFile(tempstr.constData());    
    if (!hasfile)
    {
        mutex.unlock();
        return false;
    }
    TiXmlElement *my_root1 = doc->RootElement();
    TiXmlAttribute *my_root2Attr1 =my_root1->FirstAttribute();
    const char* temp1;
    temp1 = my_root2Attr1->Value();
    string calibration_file(temp1);
    //qDebug()<<my_root1->Value();
    NetworkConfigList::getNetworkConfigListInstance()->setCalibraitionFile(calibration_file);
    // master_ip
    TiXmlAttribute * my_root2Attr2 = my_root2Attr1->Next();
    temp1 = my_root2Attr2->Value();
    string masteripstr(temp1);
    NetworkConfigList::getNetworkConfigListInstance()->setMasterIP(masteripstr.c_str());
    // nas_ip
    my_root2Attr1 = my_root2Attr2->Next();
    temp1 = my_root2Attr1->Value();
    string nasipstr(temp1);
    NetworkConfigList::getNetworkConfigListInstance()->setMasterBackupNasIP(nasipstr.c_str());
    // db_server_ip
    my_root2Attr2 = my_root2Attr1->Next();
    temp1 = my_root2Attr2->Value();
    string dbserveripstr(temp1);
    NetworkConfigList::getNetworkConfigListInstance()->setDBServerIP(dbserveripstr.c_str());

    TiXmlElement *my_root2 = my_root1->FirstChildElement();

    while(my_root2)
    {
        // slave slaveid
        TiXmlAttribute *my_root2Attr1 =my_root2->FirstAttribute();
        const char* temp;
        temp = my_root2Attr1->Value();
        int slaveid = atoi(temp);//把字符串转换成整形数
        //qDebug()<<my_root2Attr1->Name()<<my_root2Attr1->Value() << slaveid;
        // slave ip
        TiXmlAttribute *my_root2Attr2 =my_root2Attr1->Next();
        temp = my_root2Attr2->Value();
        string ip(temp);//将temp的值赋值给ip变量
        //qDebug()<<my_root2Attr2->Name()<<my_root2Attr2->Value() << ip.c_str();
        // slave port
        TiXmlAttribute *my_root2Attr3 =my_root2Attr2->Next();
        temp = my_root2Attr3->Value();
        int port = atoi(temp);
        //qDebug()<<my_root2Attr3->Name()<<my_root2Attr3->Value() << port;
        // slave is rt slave
        TiXmlAttribute *my_root2Attr4 = my_root2Attr3->Next();
        temp = my_root2Attr4->Value();
        int is_rttmp = atoi(temp);
        bool is_rt;
        if (is_rttmp == 0)
            is_rt = false;
        else
            is_rt = true;

        TiXmlAttribute *my_root2Attr5 = my_root2Attr4->Next();
        temp = my_root2Attr5->Value();
        string backupipstr(temp);

        //qDebug()<<my_root2Attr4->Name()<<my_root2Attr4->Value() << is_rt;

        SlaveModel newModel = SlaveModel(slaveid, ip, port, is_rt, backupipstr);

        TiXmlElement *my_root3 = my_root2->FirstChildElement();
        int i = 0;
        while(my_root3)
        {
            // camera_pair index
            TiXmlAttribute *my_root3Attr1 =my_root3->FirstAttribute();
            temp = my_root3Attr1->Value();
            //qDebug()<<"temp[0]"<<temp[0];
            char boxid = temp[0];
            //qDebug()<<my_root3Attr1->Name()<<my_root3Attr1->Value() << boxid;
            // camera_pair rtfile
            TiXmlAttribute *my_root3Attr2 =my_root3Attr1->Next();
            temp = my_root3Attr2->Value();
            string cameracalibfile(temp);

            TiXmlAttribute *my_root3Attr3 =my_root3Attr2->Next();
            temp = my_root3Attr3->Value();
            string fenzhongcalibfile(temp);

            //qDebug()<<my_root3Attr2->Name()<<my_root3Attr2->Value() << rtfile.c_str();

            TiXmlElement *my_root4 = my_root3->FirstChildElement();

            // camera index
            TiXmlAttribute *my_root4Attr1 =my_root4->FirstAttribute();
            temp = my_root4Attr1->Value();
            string cameraindex(temp);
            // qDebug()<<my_root4Attr1->Name()<<my_root4Attr1->Value() << cameraindex.c_str();
            // camera snid
            TiXmlAttribute *my_root4Attr2 = my_root4Attr1->Next();
            temp = my_root4Attr2->Value();
            string camera_sn(temp);
            //qDebug()<<my_root4Attr2->Name()<<my_root4Attr2->Value() << camera_sn.c_str();;
            // camera paramfile
            TiXmlAttribute *my_root4Attr3 =my_root4Attr2->Next();
            temp = my_root4Attr3->Value();
            string camera_param_file(temp);
            // qDebug()<<my_root4Attr3->Name()<<my_root4Attr3->Value() << camera_param_file.c_str();

            my_root4 = my_root4->NextSiblingElement();
                            // camera index
            my_root4Attr1 = my_root4->FirstAttribute();
            temp = my_root4Attr1->Value();
            string cameraindex2(temp);
            //qDebug()<<my_root4Attr1->Name()<<my_root4Attr1->Value() << cameraindex2.c_str();;
            // camera snid
            my_root4Attr2 = my_root4Attr1->Next();
            temp = my_root4Attr2->Value();
            string camera_sn2(temp);
            // qDebug()<<my_root4Attr2->Name()<<my_root4Attr2->Value() << camera_sn2.c_str();;
            // camera paramfile
            my_root4Attr3 = my_root4Attr2->Next();
            temp = my_root4Attr3->Value();
            string camera_param_file2(temp);
            // qDebug()<<my_root4Attr3->Name()<<my_root4Attr3->Value() << camera_param_file2.c_str();;

            CameraPair pair1;
            pair1.boxindex = boxid;
            pair1.box_camera_calib_file = cameracalibfile;
            pair1.box_fenzhong_calib_file = fenzhongcalibfile;

            pair1.camera_ref = cameraindex;
            pair1.camera_ref_sn = camera_sn;
            pair1.camera_ref_file = camera_param_file;
            pair1.camera = cameraindex2;
            pair1.camera_sn = camera_sn2;
            pair1.camera_file = camera_param_file2;
            if (i == 0)
                newModel.setBox1(pair1);
            else
                newModel.setBox2(pair1);
            i++;
            my_root3 = my_root3->NextSiblingElement();
        }
        NetworkConfigList::getNetworkConfigListInstance()->pushBackToSlaveListsnid(newModel);
        //Setting::getSettingInstance()->pushBackToSlaveList(newModel);
        my_root2 = my_root2->NextSiblingElement();
    }
    mutex.unlock();

    return true;
}

// 子类实现父类的函数
bool XMLNetworkFileLoader::saveFile()
{
    mutex.lock();

    //生成xml文件
    TiXmlDocument *doc = new TiXmlDocument();
    TiXmlDeclaration dec("1.0","gb2312","yes");
    doc->InsertEndChild(dec);
    TiXmlElement *my_root1 = new TiXmlElement("network_config");
    doc->LinkEndChild(my_root1);
    my_root1->SetAttribute("calibration_file", NetworkConfigList::getNetworkConfigListInstance()->getCalibrationFile().c_str());
    my_root1->SetAttribute("master_ip", NetworkConfigList::getNetworkConfigListInstance()->getMasterIP().toLocal8Bit().constData());
    my_root1->SetAttribute("master_backup_nasip", NetworkConfigList::getNetworkConfigListInstance()->getMasterBackupNasIP().toLocal8Bit().constData());
    my_root1->SetAttribute("db_server_ip", NetworkConfigList::getNetworkConfigListInstance()->getDBServerIP().toLocal8Bit().constData());

    // string slaveip[9]={"10.13.29.210","10.13.29.210","10.13.29.210","10.13.29.210","10.13.29.210","10.13.29.210","10.13.29.210","10.13.29.210","10.13.29.210"};
    for (int i = 0; i <= NetworkConfigList::getNetworkConfigListInstance()->listsnid()->size(); i++)
    {
        SlaveModel slavemodel = NetworkConfigList::getNetworkConfigListInstance()->listsnid()->at(0);
        TiXmlElement *my_root2 = new TiXmlElement("slave");
        my_root2->SetAttribute("slaveid", slavemodel.getIndex());
        //my_root2->SetAttribute("ip", "10.13.29.217");
        my_root2->SetAttribute("ip", slavemodel.getHostAddress().c_str());
        my_root2->SetAttribute("port",slavemodel.getPort());
        my_root2->SetAttribute("is_rt_slave", slavemodel.getIsRT());
        my_root2->SetAttribute("backup_nasip", slavemodel.getBackupNasIP().c_str());

        for (int j = 0; j <= 1; j++)
        {
            CameraPair x;
            switch (j)
            {
                case 0: x = slavemodel.box1;
                    break;
                case 1: x = slavemodel.box2;
                    break;
                default: break;
            }
            TiXmlElement *my_root3 =new TiXmlElement("camera_pair");
            my_root3->SetAttribute("index", x.boxindex);
            my_root3->SetAttribute("camera_calib_file", x.box_camera_calib_file.c_str());
            my_root3->SetAttribute("index", x.box_fenzhong_calib_file.c_str());

            {
                TiXmlElement *my_root4 =new TiXmlElement("camera_ref");
                my_root4->SetAttribute("cameraid", x.camera_ref.c_str());
                my_root4->SetAttribute("sn", x.camera_ref_sn.c_str());
                my_root4->SetAttribute("paramfile", x.camera_ref_file.c_str());
                my_root3->LinkEndChild(my_root4);
                TiXmlElement *my_root5 =new TiXmlElement("camera");
                my_root5->SetAttribute("cameraid", x.camera.c_str());
                my_root5->SetAttribute("sn", x.camera_sn.c_str());
                my_root5->SetAttribute("calibration_file", x.camera_file.c_str());
                my_root3->LinkEndChild(my_root5);
            }
            my_root2->LinkEndChild(my_root3);
        }
        my_root1->LinkEndChild(my_root2);
    }
    // 【注意！】QString转cchar不能用QString().toStdString().c_str()，否则可能会出问题
    QByteArray tempstr = getFilename().toAscii();
    bool ret = doc->SaveFile(tempstr.constData());
    mutex.unlock();
    return ret;
}

