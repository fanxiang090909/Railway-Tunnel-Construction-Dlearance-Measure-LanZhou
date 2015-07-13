#ifndef REALTASK_H
#define REALTASK_H
#include <string>

using namespace std;

/**
 * 实际采集“隧道”任务信息元素
 * @see RealTaskList
 * 该实际采集“隧道”也许是半个隧道，也许是多条隧道连着采集成一条
 * 需要后期做校正把它分开 @see 
 */
class RealTask
{
public:
    RealTask() {
        this->tunnelid = -1;
        this->tunnelname = "";
        this->seqno = -1;
        this->valid_frames = -1;
        this->datetime = "";
        this->isvalid = false;

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
        
        this->has_backup_A1 = false;
        this->has_backup_A2 = false;
        this->has_backup_B1 = false;
        this->has_backup_B2 = false;
        this->has_backup_C1 = false;
        this->has_backup_C2 = false;
        this->has_backup_D1 = false;
        this->has_backup_D2 = false;
        this->has_backup_E1 = false;
        this->has_backup_E2 = false;
        this->has_backup_F1 = false;
        this->has_backup_F2 = false;
        this->has_backup_G1 = false;
        this->has_backup_G2 = false;
        this->has_backup_H1 = false;
        this->has_backup_H2 = false;
        this->has_backup_I1 = false;
        this->has_backup_I2 = false;
        this->has_backup_J1 = false;
        this->has_backup_J2 = false;
        this->has_backup_K1 = false;
        this->has_backup_K2 = false;
        this->has_backup_L1 = false;
        this->has_backup_L2 = false;
        this->has_backup_M1 = false;
        this->has_backup_M2 = false;
        this->has_backup_N1 = false;
        this->has_backup_N2 = false;
        this->has_backup_O1 = false;
        this->has_backup_O2 = false;
        this->has_backup_P1 = false;
        this->has_backup_P2 = false;
        this->has_backup_Q1 = false;
        this->has_backup_Q2 = false;
        this->has_backup_R1 = false;
        this->has_backup_R2 = false;
    }
    // 复制构造函数
    RealTask(const RealTask & real)
    {
        this->tunnelid = real.tunnelid;
        this->tunnelname = real.tunnelname;
        this->seqno = real.seqno;
        this->valid_frames = real.valid_frames;
        this->datetime = real.datetime;
        this->isvalid = real.isvalid;

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

        this->has_backup_A1 = real.has_backup_A1;
        this->has_backup_A2 = real.has_backup_A2;
        this->has_backup_B1 = real.has_backup_B1;
        this->has_backup_B2 = real.has_backup_B2;
        this->has_backup_C1 = real.has_backup_C1;
        this->has_backup_C2 = real.has_backup_C2;
        this->has_backup_D1 = real.has_backup_D1;
        this->has_backup_D2 = real.has_backup_D2;
        this->has_backup_E1 = real.has_backup_E1;
        this->has_backup_E2 = real.has_backup_E2;
        this->has_backup_F1 = real.has_backup_F1;
        this->has_backup_F2 = real.has_backup_F2;
        this->has_backup_G1 = real.has_backup_G1;
        this->has_backup_G2 = real.has_backup_G2;
        this->has_backup_H1 = real.has_backup_H1;
        this->has_backup_H2 = real.has_backup_H2;
        this->has_backup_I1 = real.has_backup_I1;
        this->has_backup_I2 = real.has_backup_I2;
        this->has_backup_J1 = real.has_backup_J1;
        this->has_backup_J2 = real.has_backup_J2;
        this->has_backup_K1 = real.has_backup_K1;
        this->has_backup_K2 = real.has_backup_K2;
        this->has_backup_L1 = real.has_backup_L1;
        this->has_backup_L2 = real.has_backup_L2;
        this->has_backup_M1 = real.has_backup_M1;
        this->has_backup_M2 = real.has_backup_M2;
        this->has_backup_N1 = real.has_backup_N1;
        this->has_backup_N2 = real.has_backup_N2;
        this->has_backup_O1 = real.has_backup_O1;
        this->has_backup_O2 = real.has_backup_O2;
        this->has_backup_P1 = real.has_backup_P1;
        this->has_backup_P2 = real.has_backup_P2;
        this->has_backup_Q1 = real.has_backup_Q1;
        this->has_backup_Q2 = real.has_backup_Q2;
        this->has_backup_R1 = real.has_backup_R1;
        this->has_backup_R2 = real.has_backup_R2;
    }
    // 重载赋值号
    RealTask & operator=(const RealTask &real)
    {
        if (this == &real)
            return *this;
        this->tunnelid = real.tunnelid;
        this->tunnelname = real.tunnelname;
        this->seqno = real.seqno;
        this->valid_frames = real.valid_frames;
        this->datetime = real.datetime;
        this->isvalid = real.isvalid;

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

        this->has_backup_A1 = real.has_backup_A1;
        this->has_backup_A2 = real.has_backup_A2;
        this->has_backup_B1 = real.has_backup_B1;
        this->has_backup_B2 = real.has_backup_B2;
        this->has_backup_C1 = real.has_backup_C1;
        this->has_backup_C2 = real.has_backup_C2;
        this->has_backup_D1 = real.has_backup_D1;
        this->has_backup_D2 = real.has_backup_D2;
        this->has_backup_E1 = real.has_backup_E1;
        this->has_backup_E2 = real.has_backup_E2;
        this->has_backup_F1 = real.has_backup_F1;
        this->has_backup_F2 = real.has_backup_F2;
        this->has_backup_G1 = real.has_backup_G1;
        this->has_backup_G2 = real.has_backup_G2;
        this->has_backup_H1 = real.has_backup_H1;
        this->has_backup_H2 = real.has_backup_H2;
        this->has_backup_I1 = real.has_backup_I1;
        this->has_backup_I2 = real.has_backup_I2;
        this->has_backup_J1 = real.has_backup_J1;
        this->has_backup_J2 = real.has_backup_J2;
        this->has_backup_K1 = real.has_backup_K1;
        this->has_backup_K2 = real.has_backup_K2;
        this->has_backup_L1 = real.has_backup_L1;
        this->has_backup_L2 = real.has_backup_L2;
        this->has_backup_M1 = real.has_backup_M1;
        this->has_backup_M2 = real.has_backup_M2;
        this->has_backup_N1 = real.has_backup_N1;
        this->has_backup_N2 = real.has_backup_N2;
        this->has_backup_O1 = real.has_backup_O1;
        this->has_backup_O2 = real.has_backup_O2;
        this->has_backup_P1 = real.has_backup_P1;
        this->has_backup_P2 = real.has_backup_P2;
        this->has_backup_Q1 = real.has_backup_Q1;
        this->has_backup_Q2 = real.has_backup_Q2;
        this->has_backup_R1 = real.has_backup_R1;
        this->has_backup_R2 = real.has_backup_R2;
        return *this;
    }
    // 重载==号
    friend bool operator==(const RealTask &real1, const RealTask &real2)
    {
        if (real1.tunnelid == real2.tunnelid && real1.seqno == real2.seqno)
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
        else if (tmpcameraindex.compare("master") == 0)
        {
            start_frame_master = tmpstartfc;
            end_frame_master = tmpendfc;
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
        else if (tmpcameraindex.compare("master") == 0)
        {
            tmpstartfc = start_frame_master;
            tmpendfc = end_frame_master;
        }
        else
            ret = false;
        return ret;
    }

    bool saveToHasBackup(string tmpcameraindex, bool tmphasbackup)
    {
        bool ret = true;
        if (tmpcameraindex.compare("A1") == 0)
        {
            has_backup_A1 = tmphasbackup;
        }
        else if (tmpcameraindex.compare("A2") == 0)
        {
            has_backup_A2 = tmphasbackup;
        }
        else if (tmpcameraindex.compare("B1") == 0)
        {
            has_backup_B1 = tmphasbackup;
        }
        else if (tmpcameraindex.compare("B2") == 0)
        {
            has_backup_B2 = tmphasbackup;
        }
        else if (tmpcameraindex.compare("C1") == 0)
        {
            has_backup_C1 = tmphasbackup;
        }
        else if (tmpcameraindex.compare("C2") == 0)
        {
            has_backup_C2 = tmphasbackup;
        }
        else if (tmpcameraindex.compare("D1") == 0)
        {
            has_backup_D1 = tmphasbackup;
        }
        else if (tmpcameraindex.compare("D2") == 0)
        {
            has_backup_D2 = tmphasbackup;
        }
        else if (tmpcameraindex.compare("E1") == 0)
        {
            has_backup_E1 = tmphasbackup;
        }
        else if (tmpcameraindex.compare("E2") == 0)
        {
            has_backup_E2 = tmphasbackup;
        }
        else if (tmpcameraindex.compare("F1") == 0)
        {
            has_backup_F1 = tmphasbackup;
        }
        else if (tmpcameraindex.compare("F2") == 0)
        {
            has_backup_F2 = tmphasbackup;
        }
        else if (tmpcameraindex.compare("G1") == 0)
        {
            has_backup_G1 = tmphasbackup;
        }
        else if (tmpcameraindex.compare("G2") == 0)
        {
            has_backup_G2 = tmphasbackup;
        }
        else if (tmpcameraindex.compare("H1") == 0)
        {
            has_backup_H1 = tmphasbackup;
        }
        else if (tmpcameraindex.compare("H2") == 0)
        {
            has_backup_H2 = tmphasbackup;
        }
        else if (tmpcameraindex.compare("I1") == 0)
        {
            has_backup_I1 = tmphasbackup;
        }
        else if (tmpcameraindex.compare("I2") == 0)
        {
            has_backup_I2 = tmphasbackup;
        }
        else if (tmpcameraindex.compare("J1") == 0)
        {
            has_backup_J1 = tmphasbackup;
        }
        else if (tmpcameraindex.compare("J2") == 0)
        {
            has_backup_J2 = tmphasbackup;
        }
        else if (tmpcameraindex.compare("K1") == 0)
        {
            has_backup_K1 = tmphasbackup;
        }
        else if (tmpcameraindex.compare("K2") == 0)
        {
            has_backup_K2 = tmphasbackup;
        }
        else if (tmpcameraindex.compare("L1") == 0)
        {
            has_backup_L1 = tmphasbackup;
        }
        else if (tmpcameraindex.compare("L2") == 0)
        {
            has_backup_L2 = tmphasbackup;
        }
        else if (tmpcameraindex.compare("M1") == 0)
        {
            has_backup_M1 = tmphasbackup;
        }
        else if (tmpcameraindex.compare("M2") == 0)
        {
            has_backup_M2 = tmphasbackup;
        }
        else if (tmpcameraindex.compare("N1") == 0)
        {
            has_backup_N1 = tmphasbackup;
        }
        else if (tmpcameraindex.compare("N2") == 0)
        {
            has_backup_N2 = tmphasbackup;
        }
        else if (tmpcameraindex.compare("O1") == 0)
        {
            has_backup_O1 = tmphasbackup;
        }
        else if (tmpcameraindex.compare("O2") == 0)
        {
            has_backup_O2 = tmphasbackup;
        }
        else if (tmpcameraindex.compare("P1") == 0)
        {
            has_backup_P1 = tmphasbackup;
        }
        else if (tmpcameraindex.compare("P2") == 0)
        {
            has_backup_P2 = tmphasbackup;
        }
        else if (tmpcameraindex.compare("Q1") == 0)
        {
            has_backup_Q1 = tmphasbackup;
        }
        else if (tmpcameraindex.compare("Q2") == 0)
        {
            has_backup_Q2 = tmphasbackup;
        }
        else if (tmpcameraindex.compare("R1") == 0)
        {
            has_backup_R1 = tmphasbackup;
        }
        else if (tmpcameraindex.compare("R2") == 0)
        {
            has_backup_R2 = tmphasbackup;
        }
        else
            ret = false;
        return ret;
    }

    bool getHasBackup(string tmpcameraindex, bool & tmphasbackup)
    {
        bool ret = true;
        if (tmpcameraindex.compare("A1") == 0)
        {
            tmphasbackup = has_backup_A1;
        }
        else if (tmpcameraindex.compare("A2") == 0)
        {
            tmphasbackup = has_backup_A2;
        }
        else if (tmpcameraindex.compare("B1") == 0)
        {
            tmphasbackup = has_backup_B1;
        }
        else if (tmpcameraindex.compare("B2") == 0)
        {
            tmphasbackup = has_backup_B2;
        }
        else if (tmpcameraindex.compare("C1") == 0)
        {
            tmphasbackup = has_backup_C1;
        }
        else if (tmpcameraindex.compare("C2") == 0)
        {
            tmphasbackup = has_backup_C2;
        }
        else if (tmpcameraindex.compare("D1") == 0)
        {
            tmphasbackup = has_backup_D1;
        }
        else if (tmpcameraindex.compare("D2") == 0)
        {
            tmphasbackup = has_backup_D2;
        }
        else if (tmpcameraindex.compare("E1") == 0)
        {
            tmphasbackup = has_backup_E1;
        }
        else if (tmpcameraindex.compare("E2") == 0)
        {
            tmphasbackup = has_backup_E2;
        }
        else if (tmpcameraindex.compare("F1") == 0)
        {
            tmphasbackup = has_backup_F1;
        }
        else if (tmpcameraindex.compare("F2") == 0)
        {
            tmphasbackup = has_backup_F2;
        }
        else if (tmpcameraindex.compare("G1") == 0)
        {
            tmphasbackup = has_backup_G1;
        }
        else if (tmpcameraindex.compare("G2") == 0)
        {
            tmphasbackup = has_backup_G2;
        }
        else if (tmpcameraindex.compare("H1") == 0)
        {
            tmphasbackup = has_backup_H1;
        }
        else if (tmpcameraindex.compare("H2") == 0)
        {
            tmphasbackup = has_backup_H2;
        }
        else if (tmpcameraindex.compare("I1") == 0)
        {
            tmphasbackup = has_backup_I1;
        }
        else if (tmpcameraindex.compare("I2") == 0)
        {
            tmphasbackup = has_backup_I2;
        }
        else if (tmpcameraindex.compare("J1") == 0)
        {
            tmphasbackup = has_backup_J1;
        }
        else if (tmpcameraindex.compare("J2") == 0)
        {
            tmphasbackup = has_backup_J2;
        }
        else if (tmpcameraindex.compare("K1") == 0)
        {
            tmphasbackup = has_backup_K1;
        }
        else if (tmpcameraindex.compare("K2") == 0)
        {
            tmphasbackup = has_backup_K2;
        }
        else if (tmpcameraindex.compare("L1") == 0)
        {
            tmphasbackup = has_backup_L1;
        }
        else if (tmpcameraindex.compare("L2") == 0)
        {
            tmphasbackup = has_backup_L2;
        }
        else if (tmpcameraindex.compare("M1") == 0)
        {
            tmphasbackup = has_backup_M1;
        }
        else if (tmpcameraindex.compare("M2") == 0)
        {
            tmphasbackup = has_backup_M2;
        }
        else if (tmpcameraindex.compare("N1") == 0)
        {
            tmphasbackup = has_backup_N1;
        }
        else if (tmpcameraindex.compare("N2") == 0)
        {
            tmphasbackup = has_backup_N2;
        }
        else if (tmpcameraindex.compare("O1") == 0)
        {
            tmphasbackup = has_backup_O1;
        }
        else if (tmpcameraindex.compare("O2") == 0)
        {
            tmphasbackup = has_backup_O2;
        }
        else if (tmpcameraindex.compare("P1") == 0)
        {
            tmphasbackup = has_backup_P1;
        }
        else if (tmpcameraindex.compare("P2") == 0)
        {
            tmphasbackup = has_backup_P2;
        }
        else if (tmpcameraindex.compare("Q1") == 0)
        {
            tmphasbackup = has_backup_Q1;
        }
        else if (tmpcameraindex.compare("Q2") == 0)
        {
            tmphasbackup = has_backup_Q2;
        }
        else if (tmpcameraindex.compare("R1") == 0)
        {
            tmphasbackup = has_backup_R1;
        }
        else if (tmpcameraindex.compare("R2") == 0)
        {
            tmphasbackup = has_backup_R2;
        }
        else
            ret = false;
        return ret;
    }

    int tunnelid;
    string tunnelname;
    int seqno;
    int valid_frames;
    string datetime; // 精确到年月日，时分秒， 如"2014-04-05 16:51:02"
    bool isvalid;

    float start_mile;
    float end_mile;

    _int64 start_frame_master;
    _int64 end_frame_master;

    _int64 start_frame_A1;
    _int64 end_frame_A1;
    _int64 start_frame_A2;
    _int64 end_frame_A2;
    _int64 start_frame_B1;
    _int64 end_frame_B1;
    _int64 start_frame_B2;
    _int64 end_frame_B2;
    _int64 start_frame_C1;
    _int64 end_frame_C1;
    _int64 start_frame_C2;
    _int64 end_frame_C2;
    _int64 start_frame_D1;
    _int64 end_frame_D1;
    _int64 start_frame_D2;
    _int64 end_frame_D2;
    _int64 start_frame_E1;
    _int64 end_frame_E1;
    _int64 start_frame_E2;
    _int64 end_frame_E2;
    _int64 start_frame_F1;
    _int64 end_frame_F1;
    _int64 start_frame_F2;
    _int64 end_frame_F2;
    _int64 start_frame_G1;
    _int64 end_frame_G1;
    _int64 start_frame_G2;
    _int64 end_frame_G2;
    _int64 start_frame_H1;
    _int64 end_frame_H1;
    _int64 start_frame_H2;
    _int64 end_frame_H2;
    _int64 start_frame_I1;
    _int64 end_frame_I1;
    _int64 start_frame_I2;
    _int64 end_frame_I2;
    _int64 start_frame_J1;
    _int64 end_frame_J1;
    _int64 start_frame_J2;
    _int64 end_frame_J2;
    _int64 start_frame_K1;
    _int64 end_frame_K1;
    _int64 start_frame_K2;
    _int64 end_frame_K2;
    _int64 start_frame_L1;
    _int64 end_frame_L1;
    _int64 start_frame_L2;
    _int64 end_frame_L2;
    _int64 start_frame_M1;
    _int64 end_frame_M1;
    _int64 start_frame_M2;
    _int64 end_frame_M2;
    _int64 start_frame_N1;
    _int64 end_frame_N1;
    _int64 start_frame_N2;
    _int64 end_frame_N2;
    _int64 start_frame_O1;
    _int64 end_frame_O1;
    _int64 start_frame_O2;
    _int64 end_frame_O2;
    _int64 start_frame_P1;
    _int64 end_frame_P1;
    _int64 start_frame_P2;
    _int64 end_frame_P2;
    _int64 start_frame_Q1;
    _int64 end_frame_Q1;
    _int64 start_frame_Q2;
    _int64 end_frame_Q2;
    _int64 start_frame_R1;
    _int64 end_frame_R1;
    _int64 start_frame_R2;
    _int64 end_frame_R2;

    bool has_backup_A1;
    bool has_backup_A2;
    bool has_backup_B1;
    bool has_backup_B2;
    bool has_backup_C1;
    bool has_backup_C2;
    bool has_backup_D1;
    bool has_backup_D2;
    bool has_backup_E1;
    bool has_backup_E2;
    bool has_backup_F1;
    bool has_backup_F2;
    bool has_backup_G1;
    bool has_backup_G2;
    bool has_backup_H1;
    bool has_backup_H2;
    bool has_backup_I1;
    bool has_backup_I2;
    bool has_backup_J1;
    bool has_backup_J2;
    bool has_backup_K1;
    bool has_backup_K2;
    bool has_backup_L1;
    bool has_backup_L2;
    bool has_backup_M1;
    bool has_backup_M2;
    bool has_backup_N1;
    bool has_backup_N2;
    bool has_backup_O1;
    bool has_backup_O2;
    bool has_backup_P1;
    bool has_backup_P2;
    bool has_backup_Q1;
    bool has_backup_Q2;
    bool has_backup_R1;
    bool has_backup_R2;

};

#endif // REALTASK_H
