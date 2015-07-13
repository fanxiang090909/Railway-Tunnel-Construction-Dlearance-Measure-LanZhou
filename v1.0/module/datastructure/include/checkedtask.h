#ifndef CHECKED_TUNNEL_TASK_MODEL_H
#define CHECKED_TUNNEL_TASK_MODEL_H

#include "plantask.h"

#include <string>
#include <list>
using namespace std;

/**
 * 实际采集“隧道文件”校正项
 * 一条计划任务实际对应多条采集“隧道文件”
 * @author 熊雪
 * @date 2013.11.12
 * @author 范翔
 * @date 2014.01.18
 */
class CheckedItem
{
public:
    CheckedItem() {
        this->filename_prefix = "";
        this->tunnel_id = -1;
        this->tunnel_name = "";
        this->seqno = -1;
        this->collecttime = "";
        this->isvalid = false;
        this->valid_frames_num = -1;
        this->status = 0;

        this->start_mile = -1;
        this->end_mile = -1;
        this->start_frame_master = -1;
        this->end_frame_master = -1;
        this->start_frame_A1 = -1;
        this->end_frame_A1 = -1;
        this->start_frame_A2 = -1;
        this->end_frame_A2 = -1;
        this->start_frame_B1 = -1;
        this->end_frame_B1 = -1;
        this->start_frame_B2 = -1;
        this->end_frame_B2 = -1;
        this->start_frame_C1 = -1;
        this->end_frame_C1 = -1;
        this->start_frame_C2 = -1;
        this->end_frame_C2 = -1;
        this->start_frame_D1 = -1;
        this->end_frame_D1 = -1;
        this->start_frame_D2 = -1;
        this->end_frame_D2 = -1;
        this->start_frame_E1 = -1;
        this->end_frame_E1 = -1;
        this->start_frame_E2 = -1;
        this->end_frame_E2 = -1;
        this->start_frame_F1 = -1;
        this->end_frame_F1 = -1;
        this->start_frame_F2 = -1;
        this->end_frame_F2 = -1;
        this->start_frame_G1 = -1;
        this->end_frame_G1 = -1;
        this->start_frame_G2 = -1;
        this->end_frame_G2 = -1;
        this->start_frame_H1 = -1;
        this->end_frame_H1 = -1;
        this->start_frame_H2 = -1;
        this->end_frame_H2 = -1;
        this->start_frame_I1 = -1;
        this->end_frame_I1 = -1;
        this->start_frame_I2 = -1;
        this->end_frame_I2 = -1;
        this->start_frame_J1 = -1;
        this->end_frame_J1 = -1;
        this->start_frame_J2 = -1;
        this->end_frame_J2 = -1;
        this->start_frame_K1 = -1;
        this->end_frame_K1 = -1;
        this->start_frame_K2 = -1;
        this->end_frame_K2 = -1;
        this->start_frame_L1 = -1;
        this->end_frame_L1 = -1;
        this->start_frame_L2 = -1;
        this->end_frame_L2 = -1;
        this->start_frame_M1 = -1;
        this->end_frame_M1 = -1;
        this->start_frame_M2 = -1;
        this->end_frame_M2 = -1;
        this->start_frame_N1 = -1;
        this->end_frame_N1 = -1;
        this->start_frame_N2 = -1;
        this->end_frame_N2 = -1;
        this->start_frame_O1 = -1;
        this->end_frame_O1 = -1;
        this->start_frame_O2 = -1;
        this->end_frame_O2 = -1;
        this->start_frame_P1 = -1;
        this->end_frame_P1 = -1;
        this->start_frame_P2 = -1;
        this->end_frame_P2 = -1;
        this->start_frame_Q1 = -1;
        this->end_frame_Q1 = -1;
        this->start_frame_Q2 = -1;
        this->end_frame_Q2 = -1;
        this->start_frame_R1 = -1;
        this->end_frame_R1 = -1;
        this->start_frame_R2 = -1;
        this->end_frame_R2 = -1;

        this->has_backup_A1 = 0;
        this->has_backup_A2 = 0;
        this->has_backup_B1 = 0;
        this->has_backup_B2 = 0;
        this->has_backup_C1 = 0;
        this->has_backup_C2 = 0;
        this->has_backup_D1 = 0;
        this->has_backup_D2 = 0;
        this->has_backup_E1 = 0;
        this->has_backup_E2 = 0;
        this->has_backup_F1 = 0;
        this->has_backup_F2 = 0;
        this->has_backup_G1 = 0;
        this->has_backup_G2 = 0;
        this->has_backup_H1 = 0;
        this->has_backup_H2 = 0;
        this->has_backup_I1 = 0;
        this->has_backup_I2 = 0;
        this->has_backup_J1 = 0;
        this->has_backup_J2 = 0;
        this->has_backup_K1 = 0;
        this->has_backup_K2 = 0;
        this->has_backup_L1 = 0;
        this->has_backup_L2 = 0;
        this->has_backup_M1 = 0;
        this->has_backup_M2 = 0;
        this->has_backup_N1 = 0;
        this->has_backup_N2 = 0;
        this->has_backup_O1 = 0;
        this->has_backup_O2 = 0;
        this->has_backup_P1 = 0;
        this->has_backup_P2 = 0;
        this->has_backup_Q1 = 0;
        this->has_backup_Q2 = 0;
        this->has_backup_R1 = 0;
        this->has_backup_R2 = 0;

        this->backup_pos_A1 = 0;
        this->backup_pos_A2 = 0;
        this->backup_pos_B1 = 0;
        this->backup_pos_B2 = 0;
        this->backup_pos_C1 = 0;
        this->backup_pos_C2 = 0;
        this->backup_pos_D1 = 0;
        this->backup_pos_D2 = 0;
        this->backup_pos_E1 = 0;
        this->backup_pos_E2 = 0;
        this->backup_pos_F1 = 0;
        this->backup_pos_F2 = 0;
        this->backup_pos_G1 = 0;
        this->backup_pos_G2 = 0;
        this->backup_pos_H1 = 0;
        this->backup_pos_H2 = 0;
        this->backup_pos_I1 = 0;
        this->backup_pos_I2 = 0;
        this->backup_pos_J1 = 0;
        this->backup_pos_J2 = 0;
        this->backup_pos_K1 = 0;
        this->backup_pos_K2 = 0;
        this->backup_pos_L1 = 0;
        this->backup_pos_L2 = 0;
        this->backup_pos_M1 = 0;
        this->backup_pos_M2 = 0;
        this->backup_pos_N1 = 0;
        this->backup_pos_N2 = 0;
        this->backup_pos_O1 = 0;
        this->backup_pos_O2 = 0;
        this->backup_pos_P1 = 0;
        this->backup_pos_P2 = 0;
        this->backup_pos_Q1 = 0;
        this->backup_pos_Q2 = 0;
        this->backup_pos_R1 = 0;
        this->backup_pos_R2 = 0;
    }
    // 默认复制构造函数、重载赋值号
    /*// 复制构造函数
    CheckedItem(const CheckedItem & real)
    {
        this->filename_prefix = real.filename_prefix;
        this->tunnel_id = real.tunnel_id;
        this->tunnel_name = real.tunnel_name;
        this->seqno = real.seqno;
        this->collecttime = real.collecttime;
        this->isvalid = real.isvalid;
        this->valid_frames_num = real.valid_frames_num;
        this->status = real.status;

        this->start_mile = real.start_mile;
        this->end_mile = real.end_mile;
        this->start_frame_master = real.start_frame_master;
        this->end_frame_master = real.end_frame_master;
        this->start_frame_A1 = real.start_frame_A1;
        this->end_frame_A1 = real.end_frame_A1;
        this->start_frame_A2 = real.start_frame_A2;
        this->end_frame_A2 = real.end_frame_A2;
        this->start_frame_B1 = real.start_frame_B1;
        this->end_frame_B1 = real.end_frame_B1;
        this->start_frame_B2 = real.start_frame_B2;
        this->end_frame_B2 = real.end_frame_B2;
        this->start_frame_C1 = real.start_frame_C1;
        this->end_frame_C1 = real.end_frame_C1;
        this->start_frame_C2 = real.start_frame_C2;
        this->end_frame_C2 = real.end_frame_C2;
        this->start_frame_D1 = real.start_frame_D1;
        this->end_frame_D1 = real.end_frame_D1;
        this->start_frame_D2 = real.start_frame_D2;
        this->end_frame_D2 = real.end_frame_D2;
        this->start_frame_E1 = real.start_frame_E1;
        this->end_frame_E1 = real.end_frame_E1;
        this->start_frame_E2 = real.start_frame_E2;
        this->end_frame_E2 = real.end_frame_E2;
        this->start_frame_F1 = real.start_frame_F1;
        this->end_frame_F1 = real.end_frame_F1;
        this->start_frame_F2 = real.start_frame_F2;
        this->end_frame_F2 = real.end_frame_F2;
        this->start_frame_G1 = real.start_frame_G1;
        this->end_frame_G1 = real.end_frame_G1;
        this->start_frame_G2 = real.start_frame_G2;
        this->end_frame_G2 = real.end_frame_G2;
        this->start_frame_H1 = real.start_frame_H1;
        this->end_frame_H1 = real.end_frame_H1;
        this->start_frame_H2 = real.start_frame_H2;
        this->end_frame_H2 = real.end_frame_H2;
        this->start_frame_I1 = real.start_frame_I1;
        this->end_frame_I1 = real.end_frame_I1;
        this->start_frame_I2 = real.start_frame_I2;
        this->end_frame_I2 = real.end_frame_I2;
        this->start_frame_J1 = real.start_frame_J1;
        this->end_frame_J1 = real.end_frame_J1;
        this->start_frame_J2 = real.start_frame_J2;
        this->end_frame_J2 = real.end_frame_J2;
        this->start_frame_K1 = real.start_frame_K1;
        this->end_frame_K1 = real.end_frame_K1;
        this->start_frame_K2 = real.start_frame_K2;
        this->end_frame_K2 = real.end_frame_K2;
        this->start_frame_L1 = real.start_frame_L1;
        this->end_frame_L1 = real.end_frame_L1;
        this->start_frame_L2 = real.start_frame_L2;
        this->end_frame_L2 = real.end_frame_L2;
        this->start_frame_M1 = real.start_frame_M1;
        this->end_frame_M1 = real.end_frame_M1;
        this->start_frame_M2 = real.start_frame_M2;
        this->end_frame_M2 = real.end_frame_M2;
        this->start_frame_N1 = real.start_frame_N1;
        this->end_frame_N1 = real.end_frame_N1;
        this->start_frame_N2 = real.start_frame_N2;
        this->end_frame_N2 = real.end_frame_N2;
        this->start_frame_O1 = real.start_frame_O1;
        this->end_frame_O1 = real.end_frame_O1;
        this->start_frame_O2 = real.start_frame_O2;
        this->end_frame_O2 = real.end_frame_O2;
        this->start_frame_P1 = real.start_frame_P1;
        this->end_frame_P1 = real.end_frame_P1;
        this->start_frame_P2 = real.start_frame_P2;
        this->end_frame_P2 = real.end_frame_P2;
        this->start_frame_Q1 = real.start_frame_Q1;
        this->end_frame_Q1 = real.end_frame_Q1;
        this->start_frame_Q2 = real.start_frame_Q2;
        this->end_frame_Q2 = real.end_frame_Q2;
        this->start_frame_R1 = real.start_frame_R1;
        this->end_frame_R1 = real.end_frame_R1;
        this->start_frame_R2 = real.start_frame_R2;
        this->end_frame_R2 = real.end_frame_R2;
    }
    // 重载赋值号
    CheckedItem & operator=(const CheckedItem &real)
    {
        if (this == &real)
            return *this;
        this->filename_prefix = real.filename_prefix;
        this->tunnel_id = real.tunnel_id;
        this->tunnel_name = real.tunnel_name;
        this->seqno = real.seqno;
        this->collecttime = real.collecttime;
        this->isvalid = real.isvalid;
        this->valid_frames_num = real.valid_frames_num;
        this->status = real.status;

        this->start_mile = real.start_mile;
        this->end_mile = real.end_mile;
        this->start_frame_master = real.start_frame_master;
        this->end_frame_master = real.end_frame_master;
        this->start_frame_A1 = real.start_frame_A1;
        this->end_frame_A1 = real.end_frame_A1;
        this->start_frame_A2 = real.start_frame_A2;
        this->end_frame_A2 = real.end_frame_A2;
        this->start_frame_B1 = real.start_frame_B1;
        this->end_frame_B1 = real.end_frame_B1;
        this->start_frame_B2 = real.start_frame_B2;
        this->end_frame_B2 = real.end_frame_B2;
        this->start_frame_C1 = real.start_frame_C1;
        this->end_frame_C1 = real.end_frame_C1;
        this->start_frame_C2 = real.start_frame_C2;
        this->end_frame_C2 = real.end_frame_C2;
        this->start_frame_D1 = real.start_frame_D1;
        this->end_frame_D1 = real.end_frame_D1;
        this->start_frame_D2 = real.start_frame_D2;
        this->end_frame_D2 = real.end_frame_D2;
        this->start_frame_E1 = real.start_frame_E1;
        this->end_frame_E1 = real.end_frame_E1;
        this->start_frame_E2 = real.start_frame_E2;
        this->end_frame_E2 = real.end_frame_E2;
        this->start_frame_F1 = real.start_frame_F1;
        this->end_frame_F1 = real.end_frame_F1;
        this->start_frame_F2 = real.start_frame_F2;
        this->end_frame_F2 = real.end_frame_F2;
        this->start_frame_G1 = real.start_frame_G1;
        this->end_frame_G1 = real.end_frame_G1;
        this->start_frame_G2 = real.start_frame_G2;
        this->end_frame_G2 = real.end_frame_G2;
        this->start_frame_H1 = real.start_frame_H1;
        this->end_frame_H1 = real.end_frame_H1;
        this->start_frame_H2 = real.start_frame_H2;
        this->end_frame_H2 = real.end_frame_H2;
        this->start_frame_I1 = real.start_frame_I1;
        this->end_frame_I1 = real.end_frame_I1;
        this->start_frame_I2 = real.start_frame_I2;
        this->end_frame_I2 = real.end_frame_I2;
        this->start_frame_J1 = real.start_frame_J1;
        this->end_frame_J1 = real.end_frame_J1;
        this->start_frame_J2 = real.start_frame_J2;
        this->end_frame_J2 = real.end_frame_J2;
        this->start_frame_K1 = real.start_frame_K1;
        this->end_frame_K1 = real.end_frame_K1;
        this->start_frame_K2 = real.start_frame_K2;
        this->end_frame_K2 = real.end_frame_K2;
        this->start_frame_L1 = real.start_frame_L1;
        this->end_frame_L1 = real.end_frame_L1;
        this->start_frame_L2 = real.start_frame_L2;
        this->end_frame_L2 = real.end_frame_L2;
        this->start_frame_M1 = real.start_frame_M1;
        this->end_frame_M1 = real.end_frame_M1;
        this->start_frame_M2 = real.start_frame_M2;
        this->end_frame_M2 = real.end_frame_M2;
        this->start_frame_N1 = real.start_frame_N1;
        this->end_frame_N1 = real.end_frame_N1;
        this->start_frame_N2 = real.start_frame_N2;
        this->end_frame_N2 = real.end_frame_N2;
        this->start_frame_O1 = real.start_frame_O1;
        this->end_frame_O1 = real.end_frame_O1;
        this->start_frame_O2 = real.start_frame_O2;
        this->end_frame_O2 = real.end_frame_O2;
        this->start_frame_P1 = real.start_frame_P1;
        this->end_frame_P1 = real.end_frame_P1;
        this->start_frame_P2 = real.start_frame_P2;
        this->end_frame_P2 = real.end_frame_P2;
        this->start_frame_Q1 = real.start_frame_Q1;
        this->end_frame_Q1 = real.end_frame_Q1;
        this->start_frame_Q2 = real.start_frame_Q2;
        this->end_frame_Q2 = real.end_frame_Q2;
        this->start_frame_R1 = real.start_frame_R1;
        this->end_frame_R1 = real.end_frame_R1;
        this->start_frame_R2 = real.start_frame_R2;
        this->end_frame_R2 = real.end_frame_R2;
        return *this;
    }*/
    // 重载==号
    friend bool operator==(const CheckedItem &real1, const CheckedItem &real2)
    {
        if (real1.tunnel_id == real2.tunnel_id && real1.seqno == real2.seqno)
            return true;
        else
            return false;
    }

    bool saveToStartEndVal(string tmpcameraindex, __int64 tmpstartfc, __int64 tmpendfc)
    {
        bool ret = true;
        if (tmpcameraindex.compare("A1") == 0)
        {
            start_frame_A1 = tmpstartfc;
            end_frame_A1 = tmpendfc;
        }
        else if (tmpcameraindex.compare("A2") == 0)
        {
            start_frame_A2 = tmpstartfc;
            end_frame_A2 = tmpendfc;
        }
        else if (tmpcameraindex.compare("B1") == 0)
        {
            start_frame_B1 = tmpstartfc;
            end_frame_B1 = tmpendfc;
        }
        else if (tmpcameraindex.compare("B2") == 0)
        {
            start_frame_B2 = tmpstartfc;
            end_frame_B2 = tmpendfc;
        }
        else if (tmpcameraindex.compare("C1") == 0)
        {
            start_frame_C1 = tmpstartfc;
            end_frame_C1 = tmpendfc;
        }
        else if (tmpcameraindex.compare("C2") == 0)
        {
            start_frame_C2 = tmpstartfc;
            end_frame_C2 = tmpendfc;
        }
        else if (tmpcameraindex.compare("D1") == 0)
        {
            start_frame_D1 = tmpstartfc;
            end_frame_D1 = tmpendfc;
        }
        else if (tmpcameraindex.compare("D2") == 0)
        {
            start_frame_D2 = tmpstartfc;
            end_frame_D2 = tmpendfc;
        }
        else if (tmpcameraindex.compare("E1") == 0)
        {
            start_frame_E1 = tmpstartfc;
            end_frame_E1 = tmpendfc;
        }
        else if (tmpcameraindex.compare("E2") == 0)
        {
            start_frame_E2 = tmpstartfc;
            end_frame_E2 = tmpendfc;
        }
        else if (tmpcameraindex.compare("F1") == 0)
        {
            start_frame_F1 = tmpstartfc;
            end_frame_F1 = tmpendfc;
        }
        else if (tmpcameraindex.compare("F2") == 0)
        {
            start_frame_F2 = tmpstartfc;
            end_frame_F2 = tmpendfc;
        }
        else if (tmpcameraindex.compare("G1") == 0)
        {
            start_frame_G1 = tmpstartfc;
            end_frame_G1 = tmpendfc;
        }
        else if (tmpcameraindex.compare("G2") == 0)
        {
            start_frame_G2 = tmpstartfc;
            end_frame_G2 = tmpendfc;
        }
        else if (tmpcameraindex.compare("H1") == 0)
        {
            start_frame_H1 = tmpstartfc;
            end_frame_H1 = tmpendfc;
        }
        else if (tmpcameraindex.compare("H2") == 0)
        {
            start_frame_H2 = tmpstartfc;
            end_frame_H2 = tmpendfc;
        }
        else if (tmpcameraindex.compare("I1") == 0)
        {
            start_frame_I1 = tmpstartfc;
            end_frame_I1 = tmpendfc;
        }
        else if (tmpcameraindex.compare("I2") == 0)
        {
            start_frame_I2 = tmpstartfc;
            end_frame_I2 = tmpendfc;
        }
        else if (tmpcameraindex.compare("J1") == 0)
        {
            start_frame_J1 = tmpstartfc;
            end_frame_J1 = tmpendfc;
        }
        else if (tmpcameraindex.compare("J2") == 0)
        {
            start_frame_J2 = tmpstartfc;
            end_frame_J2 = tmpendfc;
        }
        else if (tmpcameraindex.compare("K1") == 0)
        {
            start_frame_K1 = tmpstartfc;
            end_frame_K1 = tmpendfc;
        }
        else if (tmpcameraindex.compare("K2") == 0)
        {
            start_frame_K2 = tmpstartfc;
            end_frame_K2 = tmpendfc;
        }
        else if (tmpcameraindex.compare("L1") == 0)
        {
            start_frame_L1 = tmpstartfc;
            end_frame_L1 = tmpendfc;
        }
        else if (tmpcameraindex.compare("L2") == 0)
        {
            start_frame_L2 = tmpstartfc;
            end_frame_L2 = tmpendfc;
        }
        else if (tmpcameraindex.compare("M1") == 0)
        {
            start_frame_M1 = tmpstartfc;
            end_frame_M1 = tmpendfc;
        }
        else if (tmpcameraindex.compare("M2") == 0)
        {
            start_frame_M2 = tmpstartfc;
            end_frame_M2 = tmpendfc;
        }
        else if (tmpcameraindex.compare("N1") == 0)
        {
            start_frame_N1 = tmpstartfc;
            end_frame_N1 = tmpendfc;
        }
        else if (tmpcameraindex.compare("N2") == 0)
        {
            start_frame_N2 = tmpstartfc;
            end_frame_N2 = tmpendfc;
        }
        else if (tmpcameraindex.compare("O1") == 0)
        {
            start_frame_O1 = tmpstartfc;
            end_frame_O1 = tmpendfc;
        }
        else if (tmpcameraindex.compare("O2") == 0)
        {
            start_frame_O2 = tmpstartfc;
            end_frame_O2 = tmpendfc;
        }
        else if (tmpcameraindex.compare("P1") == 0)
        {
            start_frame_P1 = tmpstartfc;
            end_frame_P1 = tmpendfc;
        }
        else if (tmpcameraindex.compare("P2") == 0)
        {
            start_frame_P2 = tmpstartfc;
            end_frame_P2 = tmpendfc;
        }
        else if (tmpcameraindex.compare("Q1") == 0)
        {
            start_frame_Q1 = tmpstartfc;
            end_frame_Q1 = tmpendfc;
        }
        else if (tmpcameraindex.compare("Q2") == 0)
        {
            start_frame_Q2 = tmpstartfc;
            end_frame_Q2 = tmpendfc;
        }
        else if (tmpcameraindex.compare("R1") == 0)
        {
            start_frame_R1 = tmpstartfc;
            end_frame_R1 = tmpendfc;
        }
        else if (tmpcameraindex.compare("R2") == 0)
        {
            start_frame_R2 = tmpstartfc;
            end_frame_R2 = tmpendfc;
        }
        else
            ret = false;
        return ret;
    }

    bool getStartEndVal(string tmpcameraindex, __int64 & tmpstartfc, __int64 & tmpendfc)
    {
        bool ret = true;
        if (tmpcameraindex.compare("A1") == 0)
        {
            tmpstartfc = start_frame_A1;
            tmpendfc = end_frame_A1;
        }
        else if (tmpcameraindex.compare("A2") == 0)
        {
            tmpstartfc = start_frame_A2;
            tmpendfc = end_frame_A2;
        }
        else if (tmpcameraindex.compare("B1") == 0)
        {
            tmpstartfc = start_frame_B1;
            tmpendfc = end_frame_B1;
        }
        else if (tmpcameraindex.compare("B2") == 0)
        {
            tmpstartfc = start_frame_B2;
            tmpendfc = end_frame_B2;
        }
        else if (tmpcameraindex.compare("C1") == 0)
        {
            tmpstartfc = start_frame_C1;
            tmpendfc = end_frame_C1;
        }
        else if (tmpcameraindex.compare("C2") == 0)
        {
            tmpstartfc = start_frame_C2;
            tmpendfc = end_frame_C2;
        }
        else if (tmpcameraindex.compare("D1") == 0)
        {
            tmpstartfc = start_frame_D1;
            tmpendfc = end_frame_D1;
        }
        else if (tmpcameraindex.compare("D2") == 0)
        {
            tmpstartfc = start_frame_D2;
            tmpendfc = end_frame_D2;
        }
        else if (tmpcameraindex.compare("E1") == 0)
        {
            tmpstartfc = start_frame_E1;
            tmpendfc = end_frame_E1;
        }
        else if (tmpcameraindex.compare("E2") == 0)
        {
            tmpstartfc = start_frame_E2;
            tmpendfc = end_frame_E2;
        }
        else if (tmpcameraindex.compare("F1") == 0)
        {
            tmpstartfc = start_frame_F1;
            tmpendfc = end_frame_F1;
        }
        else if (tmpcameraindex.compare("F2") == 0)
        {
            tmpstartfc = start_frame_F2;
            tmpendfc = end_frame_F2;
        }
        else if (tmpcameraindex.compare("G1") == 0)
        {
            tmpstartfc = start_frame_G1;
            tmpendfc = end_frame_G1;
        }
        else if (tmpcameraindex.compare("G2") == 0)
        {
            tmpstartfc = start_frame_G2;
            tmpendfc = end_frame_G2;
        }
        else if (tmpcameraindex.compare("H1") == 0)
        {
            tmpstartfc = start_frame_H1;
            tmpendfc = end_frame_H1;
        }
        else if (tmpcameraindex.compare("H2") == 0)
        {
            tmpstartfc = start_frame_H2;
            tmpendfc = end_frame_H2;
        }
        else if (tmpcameraindex.compare("I1") == 0)
        {
            tmpstartfc = start_frame_I1;
            tmpendfc = end_frame_I1;
        }
        else if (tmpcameraindex.compare("I2") == 0)
        {
            tmpstartfc = start_frame_I2;
            tmpendfc = end_frame_I2;
        }
        else if (tmpcameraindex.compare("J1") == 0)
        {
            tmpstartfc = start_frame_J1;
            tmpendfc = end_frame_J1;
        }
        else if (tmpcameraindex.compare("J2") == 0)
        {
            tmpstartfc = start_frame_J2;
            tmpendfc = end_frame_J2;
        }
        else if (tmpcameraindex.compare("K1") == 0)
        {
            tmpstartfc = start_frame_K1;
            tmpendfc = end_frame_K1;
        }
        else if (tmpcameraindex.compare("K2") == 0)
        {
            tmpstartfc = start_frame_K2;
            tmpendfc = end_frame_K2;
        }
        else if (tmpcameraindex.compare("L1") == 0)
        {
            tmpstartfc = start_frame_L1;
            tmpendfc = end_frame_L1;
        }
        else if (tmpcameraindex.compare("L2") == 0)
        {
            tmpstartfc = start_frame_L2;
            tmpendfc = end_frame_L2;
        }
        else if (tmpcameraindex.compare("M1") == 0)
        {
            tmpstartfc = start_frame_M1;
            tmpendfc = end_frame_M1;
        }
        else if (tmpcameraindex.compare("M2") == 0)
        {
            tmpstartfc = start_frame_M2;
            tmpendfc = end_frame_M2;
        }
        else if (tmpcameraindex.compare("N1") == 0)
        {
            tmpstartfc = start_frame_N1;
            tmpendfc = end_frame_N1;
        }
        else if (tmpcameraindex.compare("N2") == 0)
        {
            tmpstartfc = start_frame_N2;
            tmpendfc = end_frame_N2;
        }
        else if (tmpcameraindex.compare("O1") == 0)
        {
            tmpstartfc = start_frame_O1;
            tmpendfc = end_frame_O1;
        }
        else if (tmpcameraindex.compare("O2") == 0)
        {
            tmpstartfc = start_frame_O2;
            tmpendfc = end_frame_O2;
        }
        else if (tmpcameraindex.compare("P1") == 0)
        {
            tmpstartfc = start_frame_P1;
            tmpendfc = end_frame_P1;
        }
        else if (tmpcameraindex.compare("P2") == 0)
        {
            tmpstartfc = start_frame_P2;
            tmpendfc = end_frame_P2;
        }
        else if (tmpcameraindex.compare("Q1") == 0)
        {
            tmpstartfc = start_frame_Q1;
            tmpendfc = end_frame_Q1;
        }
        else if (tmpcameraindex.compare("Q2") == 0)
        {
            tmpstartfc = start_frame_Q2;
            tmpendfc = end_frame_Q2;
        }
        else if (tmpcameraindex.compare("R1") == 0)
        {
            tmpstartfc = start_frame_R1;
            tmpendfc = end_frame_R1;
        }
        else if (tmpcameraindex.compare("R2") == 0)
        {
            tmpstartfc = start_frame_R2;
            tmpendfc = end_frame_R2;
        }
        else
            ret = false;
        return ret;
    }

    /**
     * 设置采集文件的备份状态
     * @param tmpcameraindex 相机编号A1-Q2
     * @param tmphasbackup 备份进度 
     *                      0表示都没有做，1表示备份中（暂停备份），2表示已经备份完（计算过后）
     * @param tmphasbackuppos 当 tmphasbackup == 1 时（备份中暂停） 暂停时的备份位置
     */
    bool saveToHasBacpup(string tmpcameraindex, int tmphasbackup, __int64 tmphasbackuppos)
    {
        bool ret = true;
        if (tmpcameraindex.compare("A1") == 0)
        {
            has_backup_A1 = tmphasbackup;
            backup_pos_A1 = tmphasbackuppos;
        }
        else if (tmpcameraindex.compare("A2") == 0)
        {
            has_backup_A2 = tmphasbackup;
            backup_pos_A2 = tmphasbackuppos;
        }
        else if (tmpcameraindex.compare("B1") == 0)
        {
            has_backup_B1 = tmphasbackup;
            backup_pos_B1 = tmphasbackuppos;
        }
        else if (tmpcameraindex.compare("B2") == 0)
        {
            has_backup_B2 = tmphasbackup;
            backup_pos_B2 = tmphasbackuppos;
        }
        else if (tmpcameraindex.compare("C1") == 0)
        {
            has_backup_C1 = tmphasbackup;
            backup_pos_C1 = tmphasbackuppos;
        }
        else if (tmpcameraindex.compare("C2") == 0)
        {
            has_backup_C2 = tmphasbackup;
            backup_pos_C2 = tmphasbackuppos;
        }
        else if (tmpcameraindex.compare("D1") == 0)
        {
            has_backup_D1 = tmphasbackup;
            backup_pos_D1 = tmphasbackuppos;
        }
        else if (tmpcameraindex.compare("D2") == 0)
        {
            has_backup_D2 = tmphasbackup;
            backup_pos_D2 = tmphasbackuppos;
        }
        else if (tmpcameraindex.compare("E1") == 0)
        {
            has_backup_E1 = tmphasbackup;
            backup_pos_E1 = tmphasbackuppos;
        }
        else if (tmpcameraindex.compare("E2") == 0)
        {
            has_backup_E2 = tmphasbackup;
            backup_pos_E2 = tmphasbackuppos;
        }
        else if (tmpcameraindex.compare("F1") == 0)
        {
            has_backup_F1 = tmphasbackup;
            backup_pos_F1 = tmphasbackuppos;
        }
        else if (tmpcameraindex.compare("F2") == 0)
        {
            has_backup_F2 = tmphasbackup;
            backup_pos_F2 = tmphasbackuppos;
        }
        else if (tmpcameraindex.compare("G1") == 0)
        {
            has_backup_G1 = tmphasbackup;
            backup_pos_G1 = tmphasbackuppos;
        }
        else if (tmpcameraindex.compare("G2") == 0)
        {
            has_backup_G2 = tmphasbackup;
            backup_pos_G2 = tmphasbackuppos;
        }
        else if (tmpcameraindex.compare("H1") == 0)
        {
            has_backup_H1 = tmphasbackup;
            backup_pos_H1 = tmphasbackuppos;
        }
        else if (tmpcameraindex.compare("H2") == 0)
        {
            has_backup_H2 = tmphasbackup;
            backup_pos_H2 = tmphasbackuppos;
        }
        else if (tmpcameraindex.compare("I1") == 0)
        {
            has_backup_I1 = tmphasbackup;
            backup_pos_I1 = tmphasbackuppos;
        }
        else if (tmpcameraindex.compare("I2") == 0)
        {
            has_backup_I2 = tmphasbackup;
            backup_pos_I2 = tmphasbackuppos;
        }
        else if (tmpcameraindex.compare("J1") == 0)
        {
            has_backup_J1 = tmphasbackup;
            backup_pos_J1 = tmphasbackuppos;
        }
        else if (tmpcameraindex.compare("J2") == 0)
        {
            has_backup_J2 = tmphasbackup;
            backup_pos_J2 = tmphasbackuppos;
        }
        else if (tmpcameraindex.compare("K1") == 0)
        {
            has_backup_K1 = tmphasbackup;
            backup_pos_K1 = tmphasbackuppos;
        }
        else if (tmpcameraindex.compare("K2") == 0)
        {
            has_backup_K2 = tmphasbackup;
            backup_pos_K2 = tmphasbackuppos;
        }
        else if (tmpcameraindex.compare("L1") == 0)
        {
            has_backup_L1 = tmphasbackup;
            backup_pos_L1 = tmphasbackuppos;
        }
        else if (tmpcameraindex.compare("L2") == 0)
        {
            has_backup_L2 = tmphasbackup;
            backup_pos_L2 = tmphasbackuppos;
        }
        else if (tmpcameraindex.compare("M1") == 0)
        {
            has_backup_M1 = tmphasbackup;
            backup_pos_M1 = tmphasbackuppos;
        }
        else if (tmpcameraindex.compare("M2") == 0)
        {
            has_backup_M2 = tmphasbackup;
            backup_pos_M2 = tmphasbackuppos;
        }
        else if (tmpcameraindex.compare("N1") == 0)
        {
            has_backup_N1 = tmphasbackup;
            backup_pos_N1 = tmphasbackuppos;
        }
        else if (tmpcameraindex.compare("N2") == 0)
        {
            has_backup_N2 = tmphasbackup;
            backup_pos_N2 = tmphasbackuppos;
        }
        else if (tmpcameraindex.compare("O1") == 0)
        {
            has_backup_O1 = tmphasbackup;
            backup_pos_O1 = tmphasbackuppos;
        }
        else if (tmpcameraindex.compare("O2") == 0)
        {
            has_backup_O2 = tmphasbackup;
            backup_pos_O2 = tmphasbackuppos;
        }
        else if (tmpcameraindex.compare("P1") == 0)
        {
            has_backup_P1 = tmphasbackup;
            backup_pos_P1 = tmphasbackuppos;
        }
        else if (tmpcameraindex.compare("P2") == 0)
        {
            has_backup_P2 = tmphasbackup;
            backup_pos_P2 = tmphasbackuppos;
        }
        else if (tmpcameraindex.compare("Q1") == 0)
        {
            has_backup_Q1 = tmphasbackup;
            backup_pos_Q1 = tmphasbackuppos;
        }
        else if (tmpcameraindex.compare("Q2") == 0)
        {
            has_backup_Q2 = tmphasbackup;
            backup_pos_Q2 = tmphasbackuppos;
        }
        else if (tmpcameraindex.compare("R1") == 0)
        {
            has_backup_R1 = tmphasbackup;
            backup_pos_R1 = tmphasbackuppos;
        }
        else if (tmpcameraindex.compare("R2") == 0)
        {
            has_backup_R2 = tmphasbackup;
            backup_pos_R2 = tmphasbackuppos;
        }
        else
            ret = false;
        return ret;
    }

    /**
     * 获得采集文件的备份状态
     * @param tmpcameraindex 相机编号A1-Q2（输入）
     * @param tmphasbackup 备份进度 （输出）
     *                      0表示都没有做，1表示备份中（暂停备份），2表示已经备份完（计算过后）
     * @param tmphasbackuppos （输出）当 tmphasbackup == 1 时（备份中暂停） 暂停时的备份位置
     */
    bool getHasBackup(string tmpcameraindex, int & tmphasbackup, __int64 & tmphasbackuppos)
    {
        bool ret = true;
        if (tmpcameraindex.compare("A1") == 0)
        {
            tmphasbackup = has_backup_A1;
            tmphasbackuppos = backup_pos_A1;
        }
        else if (tmpcameraindex.compare("A2") == 0)
        {
            tmphasbackup = has_backup_A2;
            tmphasbackuppos = backup_pos_A2;
        }
        else if (tmpcameraindex.compare("B1") == 0)
        {
            tmphasbackup = has_backup_B1;
            tmphasbackuppos = backup_pos_B1;
        }
        else if (tmpcameraindex.compare("B2") == 0)
        {
            tmphasbackup = has_backup_B2;
            tmphasbackuppos = backup_pos_B2;
        }
        else if (tmpcameraindex.compare("C1") == 0)
        {
            tmphasbackup = has_backup_C1;
            tmphasbackuppos = backup_pos_C1;
        }
        else if (tmpcameraindex.compare("C2") == 0)
        {
            tmphasbackup = has_backup_C2;
            tmphasbackuppos = backup_pos_C2;
        }
        else if (tmpcameraindex.compare("D1") == 0)
        {
            tmphasbackup = has_backup_D1;
            tmphasbackuppos = backup_pos_D1;
        }
        else if (tmpcameraindex.compare("D2") == 0)
        {
            tmphasbackup = has_backup_D2;
            tmphasbackuppos = backup_pos_D2;
        }
        else if (tmpcameraindex.compare("E1") == 0)
        {
            tmphasbackup = has_backup_E1;
            tmphasbackuppos = backup_pos_E1;
        }
        else if (tmpcameraindex.compare("E2") == 0)
        {
            tmphasbackup = has_backup_E2;
            tmphasbackuppos = backup_pos_E2;
        }
        else if (tmpcameraindex.compare("F1") == 0)
        {
            tmphasbackup = has_backup_F1;
            tmphasbackuppos = backup_pos_F1;
        }
        else if (tmpcameraindex.compare("F2") == 0)
        {
            tmphasbackup = has_backup_F2;
            tmphasbackuppos = backup_pos_F2;
        }
        else if (tmpcameraindex.compare("G1") == 0)
        {
            tmphasbackup = has_backup_G1;
            tmphasbackuppos = backup_pos_G1;
        }
        else if (tmpcameraindex.compare("G2") == 0)
        {
            tmphasbackup = has_backup_G2;
            tmphasbackuppos = backup_pos_G2;
        }
        else if (tmpcameraindex.compare("H1") == 0)
        {
            tmphasbackup = has_backup_H1;
            tmphasbackuppos = backup_pos_H1;
        }
        else if (tmpcameraindex.compare("H2") == 0)
        {
            tmphasbackup = has_backup_H2;
            tmphasbackuppos = backup_pos_H2;
        }
        else if (tmpcameraindex.compare("I1") == 0)
        {
            tmphasbackup = has_backup_I1;
            tmphasbackuppos = backup_pos_I1;
        }
        else if (tmpcameraindex.compare("I2") == 0)
        {
            tmphasbackup = has_backup_I2;
            tmphasbackuppos = backup_pos_I2;
        }
        else if (tmpcameraindex.compare("J1") == 0)
        {
            tmphasbackup = has_backup_J1;
            tmphasbackuppos = backup_pos_J1;
        }
        else if (tmpcameraindex.compare("J2") == 0)
        {
            tmphasbackup = has_backup_J2;
            tmphasbackuppos = backup_pos_J2;
        }
        else if (tmpcameraindex.compare("K1") == 0)
        {
            tmphasbackup = has_backup_K1;
            tmphasbackuppos = backup_pos_K1;
        }
        else if (tmpcameraindex.compare("K2") == 0)
        {
            tmphasbackup = has_backup_K2;
            tmphasbackuppos = backup_pos_K2;
        }
        else if (tmpcameraindex.compare("L1") == 0)
        {
            tmphasbackup = has_backup_L1;
            tmphasbackuppos = backup_pos_L1;
        }
        else if (tmpcameraindex.compare("L2") == 0)
        {
            tmphasbackup = has_backup_L2;
            tmphasbackuppos = backup_pos_L2;
        }
        else if (tmpcameraindex.compare("M1") == 0)
        {
            tmphasbackup = has_backup_M1;
            tmphasbackuppos = backup_pos_M1;
        }
        else if (tmpcameraindex.compare("M2") == 0)
        {
            tmphasbackup = has_backup_M2;
            tmphasbackuppos = backup_pos_M2;
        }
        else if (tmpcameraindex.compare("N1") == 0)
        {
            tmphasbackup = has_backup_N1;
            tmphasbackuppos = backup_pos_N1;
        }
        else if (tmpcameraindex.compare("N2") == 0)
        {
            tmphasbackup = has_backup_N2;
            tmphasbackuppos = backup_pos_N2;
        }
        else if (tmpcameraindex.compare("O1") == 0)
        {
            tmphasbackup = has_backup_O1;
            tmphasbackuppos = backup_pos_O1;
        }
        else if (tmpcameraindex.compare("O2") == 0)
        {
            tmphasbackup = has_backup_O2;
            tmphasbackuppos = backup_pos_O2;
        }
        else if (tmpcameraindex.compare("P1") == 0)
        {
            tmphasbackup = has_backup_P1;
            tmphasbackuppos = backup_pos_P1;
        }
        else if (tmpcameraindex.compare("P2") == 0)
        {
            tmphasbackup = has_backup_P2;
            tmphasbackuppos = backup_pos_P2;
        }
        else if (tmpcameraindex.compare("Q1") == 0)
        {
            tmphasbackup = has_backup_Q1;
            tmphasbackuppos = backup_pos_Q1;
        }
        else if (tmpcameraindex.compare("Q2") == 0)
        {
            tmphasbackup = has_backup_Q2;
            tmphasbackuppos = backup_pos_Q2;
        }
        else if (tmpcameraindex.compare("R1") == 0)
        {
            tmphasbackup = has_backup_R1;
            tmphasbackuppos = backup_pos_R1;
        }
        else if (tmpcameraindex.compare("R2") == 0)
        {
            tmphasbackup = has_backup_R2;
            tmphasbackuppos = backup_pos_R2;
        }
        else
            ret = false;
        return ret;
    }

    int seqno;              // 文件序列号
    string filename_prefix; // 文件名前缀
    int tunnel_id;          // 隧道编号
    string tunnel_name;     // 隧道名称
    int valid_frames_num;   // 文件有效帧长度
    bool isvalid;
    int status;             // 该文件计算备份状态
    string collecttime;     // 该隧道的实际采集时间（realList中也有，如"2014-04-05 16:58:23"）

    float start_mile;       // 起始里程
    float end_mile;         // 终止里程

    __int64 start_frame_master;// 起始帧数
    __int64 end_frame_master;  // 终止帧数

    __int64 start_frame_A1;    // 起始帧数
    __int64 end_frame_A1;      // 终止帧数
    __int64 start_frame_A2;
    __int64 end_frame_A2;
    __int64 start_frame_B1;
    __int64 end_frame_B1;
    __int64 start_frame_B2;
    __int64 end_frame_B2;
    __int64 start_frame_C1;
    __int64 end_frame_C1;
    __int64 start_frame_C2;
    __int64 end_frame_C2;
    __int64 start_frame_D1;
    __int64 end_frame_D1;
    __int64 start_frame_D2;
    __int64 end_frame_D2;
    __int64 start_frame_E1;
    __int64 end_frame_E1;
    __int64 start_frame_E2;
    __int64 end_frame_E2;
    __int64 start_frame_F1;
    __int64 end_frame_F1;
    __int64 start_frame_F2;
    __int64 end_frame_F2;
    __int64 start_frame_G1;
    __int64 end_frame_G1;
    __int64 start_frame_G2;
    __int64 end_frame_G2;
    __int64 start_frame_H1;
    __int64 end_frame_H1;
    __int64 start_frame_H2;
    __int64 end_frame_H2;
    __int64 start_frame_I1;
    __int64 end_frame_I1;
    __int64 start_frame_I2;
    __int64 end_frame_I2;
    __int64 start_frame_J1;
    __int64 end_frame_J1;
    __int64 start_frame_J2;
    __int64 end_frame_J2;
    __int64 start_frame_K1;
    __int64 end_frame_K1;
    __int64 start_frame_K2;
    __int64 end_frame_K2;
    __int64 start_frame_L1;
    __int64 end_frame_L1;
    __int64 start_frame_L2;
    __int64 end_frame_L2;
    __int64 start_frame_M1;
    __int64 end_frame_M1;
    __int64 start_frame_M2;
    __int64 end_frame_M2;
    __int64 start_frame_N1;
    __int64 end_frame_N1;
    __int64 start_frame_N2;
    __int64 end_frame_N2;
    __int64 start_frame_O1;
    __int64 end_frame_O1;
    __int64 start_frame_O2;
    __int64 end_frame_O2;
    __int64 start_frame_P1;
    __int64 end_frame_P1;
    __int64 start_frame_P2;
    __int64 end_frame_P2;
    __int64 start_frame_Q1;
    __int64 end_frame_Q1;
    __int64 start_frame_Q2;
    __int64 end_frame_Q2;
    __int64 start_frame_R1;
    __int64 end_frame_R1;
    __int64 start_frame_R2;
    __int64 end_frame_R2;

    int has_backup_A1;
    int has_backup_A2;
    int has_backup_B1;
    int has_backup_B2;
    int has_backup_C1;
    int has_backup_C2;
    int has_backup_D1;
    int has_backup_D2;
    int has_backup_E1;
    int has_backup_E2;
    int has_backup_F1;
    int has_backup_F2;
    int has_backup_G1;
    int has_backup_G2;
    int has_backup_H1;
    int has_backup_H2;
    int has_backup_I1;
    int has_backup_I2;
    int has_backup_J1;
    int has_backup_J2;
    int has_backup_K1;
    int has_backup_K2;
    int has_backup_L1;
    int has_backup_L2;
    int has_backup_M1;
    int has_backup_M2;
    int has_backup_N1;
    int has_backup_N2;
    int has_backup_O1;
    int has_backup_O2;
    int has_backup_P1;
    int has_backup_P2;
    int has_backup_Q1;
    int has_backup_Q2;
    int has_backup_R1;
    int has_backup_R2;

    __int64 backup_pos_A1;
    __int64 backup_pos_A2;
    __int64 backup_pos_B1;
    __int64 backup_pos_B2;
    __int64 backup_pos_C1;
    __int64 backup_pos_C2;
    __int64 backup_pos_D1;
    __int64 backup_pos_D2;
    __int64 backup_pos_E1;
    __int64 backup_pos_E2;
    __int64 backup_pos_F1;
    __int64 backup_pos_F2;
    __int64 backup_pos_G1;
    __int64 backup_pos_G2;
    __int64 backup_pos_H1;
    __int64 backup_pos_H2;
    __int64 backup_pos_I1;
    __int64 backup_pos_I2;
    __int64 backup_pos_J1;
    __int64 backup_pos_J2;
    __int64 backup_pos_K1;
    __int64 backup_pos_K2;
    __int64 backup_pos_L1;
    __int64 backup_pos_L2;
    __int64 backup_pos_M1;
    __int64 backup_pos_M2;
    __int64 backup_pos_N1;
    __int64 backup_pos_N2;
    __int64 backup_pos_O1;
    __int64 backup_pos_O2;
    __int64 backup_pos_P1;
    __int64 backup_pos_P2;
    __int64 backup_pos_Q1;
    __int64 backup_pos_Q2;
    __int64 backup_pos_R1;
    __int64 backup_pos_R2;
};

/**
 * 校正后的一条隧道对应一个计算及备份文件信息结构体
 */
class CalcuFileItem
{
public:
    CalcuFileItem()
    {
        cal_filename_prefix = "";
        cal_framecounter_begin = -1;
        cal_framecounter_end = -1;
        cal_valid_frames_num = -1;
        fuse_calculte_time = "";

        has_backup_calc_A = 0;
        has_backup_calc_B = 0;
        has_backup_calc_C = 0;
        has_backup_calc_D = 0;
        has_backup_calc_E = 0;
        has_backup_calc_F = 0;
        has_backup_calc_G = 0;
        has_backup_calc_H = 0;
        has_backup_calc_I = 0;
        has_backup_calc_J = 0;
        has_backup_calc_K = 0;
        has_backup_calc_L = 0;
        has_backup_calc_M = 0;
        has_backup_calc_N = 0;
        has_backup_calc_O = 0;
        has_backup_calc_P = 0;
        has_backup_calc_Q = 0;
        has_backup_calc_R = 0;
        has_backup_calc_RT = 0;
        has_backup_fusecalc = 0;

        backup_calc_pos_A = 0;
        backup_calc_pos_B = 0;
        backup_calc_pos_C = 0;
        backup_calc_pos_D = 0;
        backup_calc_pos_E = 0;
        backup_calc_pos_F = 0;
        backup_calc_pos_G = 0;
        backup_calc_pos_H = 0;
        backup_calc_pos_I = 0;
        backup_calc_pos_J = 0;
        backup_calc_pos_K = 0;
        backup_calc_pos_L = 0;
        backup_calc_pos_M = 0;
        backup_calc_pos_N = 0;
        backup_calc_pos_O = 0;
        backup_calc_pos_P = 0;
        backup_calc_pos_Q = 0;
        backup_calc_pos_R = 0;
        backup_calc_pos_RT = 0;
        backup_calc_pos_fuse = 0;
    }
    // 默认复制构造函数，重载赋值号
    /*// 复制构造函数
    CalcuFileItem(const CalcuFileItem & real)
    {
    }
    // 重载赋值号
    CalcuFileItem & operator=(const CalcuFileItem &real)
    {
        if (this == &real)
            return *this;
    }*/

    /**
     * 设置计算备份状态
     * @param tmpboxid 相机组号
     * @param tmphasbackup 计算备份进度 
     *                      0表示都没有做，1表示已经计算中（暂停状态），2表示计算完，5表示备份中（暂停备份），6表示已经备份完（计算过后）
     */
    bool saveHasBackupCalc(string tmpboxid, int tmphasbackup, __int64 tmphasbackuppos)
    {
        bool ret = true;
        if (tmpboxid.compare("A") == 0)
        {
            this->has_backup_calc_A = tmphasbackup;
            this->backup_calc_pos_A = tmphasbackuppos;
        }
        else if (tmpboxid.compare("B") == 0)
        {
            this->has_backup_calc_B = tmphasbackup;
            this->backup_calc_pos_B = tmphasbackuppos;
        }
        else if (tmpboxid.compare("C") == 0)
        {
            this->has_backup_calc_C = tmphasbackup;
            this->backup_calc_pos_C = tmphasbackuppos;
        }
        else if (tmpboxid.compare("D") == 0)
        {
            this->has_backup_calc_D = tmphasbackup;
            this->backup_calc_pos_D = tmphasbackuppos;
        }
        else if (tmpboxid.compare("E") == 0)
        {
            this->has_backup_calc_E = tmphasbackup;
            this->backup_calc_pos_E = tmphasbackuppos;
        }
        else if (tmpboxid.compare("F") == 0)
        {
            this->has_backup_calc_F = tmphasbackup;
            this->backup_calc_pos_F = tmphasbackuppos;
        }
        else if (tmpboxid.compare("G") == 0)
        {
            this->has_backup_calc_G = tmphasbackup;
            this->backup_calc_pos_G = tmphasbackuppos;
        }
        else if (tmpboxid.compare("H") == 0)
        {
            this->has_backup_calc_H = tmphasbackup;
            this->backup_calc_pos_H = tmphasbackuppos;
        }
        else if (tmpboxid.compare("I") == 0)
        {
            this->has_backup_calc_I = tmphasbackup;
            this->backup_calc_pos_I = tmphasbackuppos;
        }
        else if (tmpboxid.compare("J") == 0)
        {
            this->has_backup_calc_J = tmphasbackup;
            this->backup_calc_pos_J = tmphasbackuppos;
        }
        else if (tmpboxid.compare("K") == 0)
        {
            this->has_backup_calc_K = tmphasbackup;
            this->backup_calc_pos_K = tmphasbackuppos;
        }
        else if (tmpboxid.compare("L") == 0)
        {
            this->has_backup_calc_L = tmphasbackup;
            this->backup_calc_pos_L = tmphasbackuppos;
        }
        else if (tmpboxid.compare("M") == 0)
        {
            this->has_backup_calc_M = tmphasbackup;
            this->backup_calc_pos_M = tmphasbackuppos;
        }
        else if (tmpboxid.compare("N") == 0)
        {
            this->has_backup_calc_N = tmphasbackup;
            this->backup_calc_pos_N = tmphasbackuppos;
        }
        else if (tmpboxid.compare("O") == 0)
        {
            this->has_backup_calc_O = tmphasbackup;
            this->backup_calc_pos_O = tmphasbackuppos;
        }
        else if (tmpboxid.compare("P") == 0)
        {
            this->has_backup_calc_P = tmphasbackup;
            this->backup_calc_pos_P = tmphasbackuppos;
        }
        else if (tmpboxid.compare("Q") == 0)
        {
            this->has_backup_calc_Q = tmphasbackup;
            this->backup_calc_pos_Q = tmphasbackuppos;
        }
        else if (tmpboxid.compare("R") == 0)
        {
            this->has_backup_calc_R = tmphasbackup;
            this->backup_calc_pos_R = tmphasbackuppos;
        }
        else if (tmpboxid.compare("RT") == 0)
        {
            this->has_backup_calc_RT = tmphasbackup;
            this->backup_calc_pos_RT = tmphasbackuppos;
        }
        else if (tmpboxid.compare("fuse") == 0)
        {
            this->has_backup_fusecalc = tmphasbackup;
            this->backup_calc_pos_fuse = tmphasbackuppos;
        }
        else
            ret = false;
        return ret;

    }

    /**
     * 得到计算备份状态
     * @param tmpboxid 相机组号（输入）
     * @param tmphasbackup 计算备份进度（输出） 
     *                      0表示都没有做，1表示已经计算中（暂停状态），2表示计算完，5表示备份中（暂停备份），6表示已经备份完（计算过后）
     */
    bool getHasBackupCalc(string tmpboxid, int & tmphasbackup, __int64 & tmphasbackuppos)
    {
        bool ret = true;
        if (tmpboxid.compare("A") == 0)
        {
            tmphasbackup = this->has_backup_calc_A;
            tmphasbackuppos = this->backup_calc_pos_A;
        }
        else if (tmpboxid.compare("B") == 0)
        {
            tmphasbackup = this->has_backup_calc_B;
            tmphasbackuppos = this->backup_calc_pos_B;
        }
        else if (tmpboxid.compare("C") == 0)
        {
            tmphasbackup = this->has_backup_calc_C;
            tmphasbackuppos = this->backup_calc_pos_C;
        }
        else if (tmpboxid.compare("D") == 0)
        {
            tmphasbackup = this->has_backup_calc_D;
            tmphasbackuppos = this->backup_calc_pos_D;
        }
        else if (tmpboxid.compare("E") == 0)
        {
            tmphasbackup = this->has_backup_calc_E;
            tmphasbackuppos = this->backup_calc_pos_E;
        }
        else if (tmpboxid.compare("F") == 0)
        {
            tmphasbackup = this->has_backup_calc_F;
            tmphasbackuppos = this->backup_calc_pos_F;
        }
        else if (tmpboxid.compare("G") == 0)
        {
            tmphasbackup = this->has_backup_calc_G;
            tmphasbackuppos = this->backup_calc_pos_G;
        }
        else if (tmpboxid.compare("H") == 0)
        {
            tmphasbackup = this->has_backup_calc_H;
            tmphasbackuppos = this->backup_calc_pos_H;
        }
        else if (tmpboxid.compare("I") == 0)
        {
            tmphasbackup = this->has_backup_calc_I;
            tmphasbackuppos = this->backup_calc_pos_I;
        }
        else if (tmpboxid.compare("J") == 0)
        {
            tmphasbackup = this->has_backup_calc_J;
            tmphasbackuppos = this->backup_calc_pos_J;
        }
        else if (tmpboxid.compare("K") == 0)
        {
            tmphasbackup = this->has_backup_calc_K;
            tmphasbackuppos = this->backup_calc_pos_K;
        }
        else if (tmpboxid.compare("L") == 0)
        {
            tmphasbackup = this->has_backup_calc_L;
            tmphasbackuppos = this->backup_calc_pos_L;
        }
        else if (tmpboxid.compare("M") == 0)
        {
            tmphasbackup = this->has_backup_calc_M;
            tmphasbackuppos = this->backup_calc_pos_M;
        }
        else if (tmpboxid.compare("N") == 0)
        {
            tmphasbackup = this->has_backup_calc_N;
            tmphasbackuppos = this->backup_calc_pos_N;
        }
        else if (tmpboxid.compare("O") == 0)
        {
            tmphasbackup = this->has_backup_calc_O;
            tmphasbackuppos = this->backup_calc_pos_O;
        }
        else if (tmpboxid.compare("P") == 0)
        {
            tmphasbackup = this->has_backup_calc_P;
            tmphasbackuppos = this->backup_calc_pos_P;
        }
        else if (tmpboxid.compare("Q") == 0)
        {
            tmphasbackup = this->has_backup_calc_Q;
            tmphasbackuppos = this->backup_calc_pos_Q;
        }
        else if (tmpboxid.compare("R") == 0)
        {
            tmphasbackup = this->has_backup_calc_R;
            tmphasbackuppos = this->backup_calc_pos_R;
        }
        else if (tmpboxid.compare("RT") == 0)
        {
            tmphasbackup = this->has_backup_calc_RT;
            tmphasbackuppos = this->backup_calc_pos_RT;
        }
        else if (tmpboxid.compare("fuse") == 0)
        {
            tmphasbackup = this->has_backup_fusecalc;
            tmphasbackuppos = this->backup_calc_pos_fuse;
        }
        else
            ret = false;
        return ret;
    }

    string cal_filename_prefix;	    // 文件名前缀
    __int64 cal_framecounter_begin; // 文件起始帧位置
    __int64 cal_framecounter_end;	// 文件终止帧位置
    __int64 cal_valid_frames_num;	// 有效帧长度
    string fuse_calculte_time;      // 该隧道的融合计算时间（realList中也有，如"2014-04-05 16:58:23"）

    // 计算备份状态，0表示都没有做，1表示已经计算中（暂停状态），2表示计算完，5表示备份中（暂停备份），6表示已经备份完（计算过后）
    int has_backup_calc_A;
    int has_backup_calc_B;
    int has_backup_calc_C;
    int has_backup_calc_D;
    int has_backup_calc_E;
    int has_backup_calc_F;
    int has_backup_calc_G;
    int has_backup_calc_H;
    int has_backup_calc_I;
    int has_backup_calc_J;
    int has_backup_calc_K;
    int has_backup_calc_L;
    int has_backup_calc_M;
    int has_backup_calc_N;
    int has_backup_calc_O;
    int has_backup_calc_P;
    int has_backup_calc_Q; // QR分开算
    int has_backup_calc_R; // QR分开算
    int has_backup_calc_RT;// QR一起算RT
    int has_backup_fusecalc;

    // 如果暂停计算，备份，此处存储暂停计算、备份的位置（为下一次开始准备）
    __int64 backup_calc_pos_A;
    __int64 backup_calc_pos_B;
    __int64 backup_calc_pos_C;
    __int64 backup_calc_pos_D;
    __int64 backup_calc_pos_E;
    __int64 backup_calc_pos_F;
    __int64 backup_calc_pos_G;
    __int64 backup_calc_pos_H;
    __int64 backup_calc_pos_I;
    __int64 backup_calc_pos_J;
    __int64 backup_calc_pos_K;
    __int64 backup_calc_pos_L;
    __int64 backup_calc_pos_M;
    __int64 backup_calc_pos_N;
    __int64 backup_calc_pos_O;
    __int64 backup_calc_pos_P;
    __int64 backup_calc_pos_Q; // QR分开算
    __int64 backup_calc_pos_R; // QR分开算
    __int64 backup_calc_pos_RT;// QR一起算RT
    __int64 backup_calc_pos_fuse;
};

/**
 * 校正后的“实际采集隧道”元素，是CheckedTaskList的元素
 * @see CheckedTaskList 
 * @author 熊雪
 * @date 2013.11.12
 * @author 范翔
 * @date 2014.01.18
 */
class CheckedTunnelTaskModel
{
public:
    /**
     * 校正后的一条隧道对应一个计划隧道
     */
    PlanTask planTask;
    /**
     * 校正后的一条隧道对应一个计算及备份文件信息
     */
    CalcuFileItem calcuItem;
    /**
     * 校正后的一条隧道对应一个实际采集“隧道文件”队列
     */
    std::list<CheckedItem> * realList;

public:
    /**
     * 校正后的“实际采集隧道”元素
     * 构造函数
     */
    CheckedTunnelTaskModel();
    ~CheckedTunnelTaskModel();
    // 复制构造函数
    CheckedTunnelTaskModel(const CheckedTunnelTaskModel & real);
    // 重载赋值号
    CheckedTunnelTaskModel & operator=(const CheckedTunnelTaskModel &real);
    // 重载==号
    friend bool operator==(const CheckedTunnelTaskModel &real1, const CheckedTunnelTaskModel &real2);
    /**
     * 设置校正隧道所对应的计划隧道（一个）
     */
    void setPlanTask(PlanTask ptask);
    /**
     * 设置校正隧道所对应的计算信息（一个）
     */
    void setCalcuItem(CalcuFileItem citem);

    /**
     * 得到对应一个实际采集“隧道文件”队列
     */
    std::list<CheckedItem>* getRealList();

    /**
     * 末尾插入一个实际采集“隧道文件”元素到实际采集“隧道文件”队列
     * @param real 要插入的实际采集“隧道文件”元素
     */
    void pushback(CheckedItem& real);

    /**
     * 中间插入一个实际采集“隧道文件”元素到实际采集“隧道文件”队列
     * @param i 插入位置
     * @param real 要插入的实际采集“隧道文件”元素
     */
    void insertAt(int i, CheckedItem& real);

    /**
     * 从实际采集“隧道文件”队列中删除某元素
     * @param real 要删除的实际采集“隧道文件”元素
     */
    void remove(CheckedItem& real);

    /**
     * 从实际采集“隧道文件”队列中删除某元素
     * @param i 要删除的实际采集“隧道文件”元素所在索引，0为第一个元素
     */
    void removeAt(int i);

    /**
     * qDebug查看对象
     */
    void showCheckedTaskModel();

    std::list<CheckedItem>::iterator begin();
    std::list<CheckedItem>::iterator end();
    std::list<CheckedItem>::reverse_iterator rbegin();
    std::list<CheckedItem>::reverse_iterator rend();

};

#endif // CHECKED_TUNNEL_TASK_MODEL_H
