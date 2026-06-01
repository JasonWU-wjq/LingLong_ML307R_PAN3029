/*
 * ExternPara.h
 *
 *  Created on: 2021年6月3日
 *      Author: WJQ
 */

#ifndef EXTERNPARA_H_
#define EXTERNPARA_H_

#include "../usr_lib/sys.h"

//#define  BufferLenth     64
//#define  SAVEMAXSIZE     80
//#define  UARTPOINTNUM    300
extern unsigned char FireFlag;
extern unsigned char USCIA0RXBUF[];//[255];
extern unsigned char TxBuffer[];//[255];

extern unsigned char SysYear,SysMonth,SysDay,SysHour,SysMinute,SysSecond,SysWeek;
extern unsigned char DateUpdateFlag;

extern unsigned char Uart9_Tx_buf[15];
extern unsigned char Uart0_Rx_buf[64];
//extern unsigned char Uart0_Rx_buf1[35];
extern unsigned char Uart0_Rx_index ;
//extern unsigned char Uart0_Tx_buf[15];
//extern unsigned char Uart9_Rx_buf[15];
//extern unsigned char Uart9_Rx_index ;

extern bool g_uart0_recv_complete ;
extern bool g_uart0_send_complete ;
extern bool g_uart9_recv_complete;
extern bool g_uart9_send_complete ;

extern unsigned char Uart0_Flag;
//extern unsigned char Uart0_Valid;
extern unsigned char Uart0_Check;

extern bool g_timer_1ms_generated ;
extern bool g_rtc_period_generatede;

extern unsigned int USCIA0RXNum,UART_RxNum;

extern unsigned char CmdRunTime,UartDelayTime,CommReadTime;

extern unsigned char  Imei[20],Iccid[30];
extern unsigned char  WifiDns[30];
extern unsigned int   WifiPort;
extern unsigned char  Csq;
extern unsigned char  RevChar;

extern unsigned char  DeviceId[7];
extern unsigned char  FlashBuf[];//[120];

extern unsigned char  GroupNo;
extern unsigned char  PeakTime;

extern unsigned char  UpLoadMode;
extern unsigned char  UpLoadCycle;
extern unsigned char  UpLoadCycleUint;
extern unsigned char  UpLoadYear;
extern unsigned char  UpLoadMonth;
extern unsigned char  UpLoadDay;
extern unsigned char  UpLoadHour;
extern unsigned char  UpLoadMin;
extern unsigned char  UpLoadSec;
extern unsigned char  LinkDelayTime;

extern unsigned char  ConnectFlag,ConnectHoldTime;

extern unsigned char  ECL,RSRP[2],RSSI[2],SNR[2],CELL_ID[4],PCI[2],EARFCN[2];

extern unsigned char DataSaveFlag;
extern unsigned char DataSaveCycle;
extern unsigned char DataSaveCycleUint;
extern unsigned int  CurrSaveAddr;
extern unsigned int  DataSaveNum;

extern unsigned char  DevOnOff;
extern unsigned char  RunMode;
extern unsigned char  RoomTemp;
extern unsigned char  FirePower;

extern int SysRunStatus;
extern int PreRunStatus;

extern unsigned char  Dev_Status[2];    //2状态字节
extern unsigned char  ErrorFlag;       //故障代码
extern unsigned char  BathWaterTemp;   //卫浴水温度
extern unsigned char  BathWaterTargetTemp;   //卫浴水目标温度
extern unsigned char  HeatBathWaterTemp;   //供暖水温度
extern unsigned char  HeatBathWaterTargetTemp;   //供暖水目标温度
extern unsigned char  FlucTemp;   //回差温度
extern unsigned char  CurrFlowRate;   //瞬时流量
extern unsigned char  DevFacPara[4];         //出厂参数
extern unsigned char  MinPower,MaxPower,FirePower;
extern unsigned char  ProValveOut;          //比例阀输出
extern unsigned char  UserId;
extern unsigned char  DevSoftVer[2];
extern unsigned char  SensorPara;
extern unsigned char  DevType;

extern unsigned char  SaveFlag;
extern unsigned long SavingTime;
extern unsigned long UpLoadTime;
extern unsigned int GetDataTime;

extern unsigned long SaveWaitingTime;
extern unsigned long UpLoadWaitingTime;

extern unsigned char FrameSer;

extern unsigned long WaterUsingTotalTime;
extern unsigned long GasUsingTotalTime;

extern unsigned int TempWaterVol;
//extern unsigned int WaterPulseNum;
//extern unsigned int TempGasVol;
extern unsigned int TempElecPower;

extern unsigned long WaterTotalVol;
extern unsigned long GasTotalVol;
extern unsigned long ElecTotalPower;
extern unsigned long PowerUsingTime;

extern unsigned char  ReceiveFlag;
extern unsigned char  RF_RxBuf[];//[64];
extern unsigned char  ThermostatId[7];
extern unsigned char  FrameSer;
extern unsigned char  FrameDir;

extern unsigned int RF_HandShakeTime;

extern unsigned char  KeyPressHoldTime;
extern unsigned char  KeyPressFlag;
extern unsigned char  ErrFirstHappenFlag;
extern unsigned int ErrHoldTime;

extern unsigned char  SetUpFlag,ComValid;

extern unsigned int  Curraddr;
extern unsigned int  SoftVer,TempVer;
extern unsigned int  TotalNum;
extern unsigned int  CurrFrameSer;
extern unsigned char StatusBuf[15];

extern unsigned char UpgradeOverTime;
extern unsigned char UpgradeFlag;
extern unsigned char UpgradeNum;

extern unsigned char AutoConnectFlag;
extern unsigned char TimingSaveFlag;

extern unsigned char Thermo_Buf[];//[50];
extern unsigned char Temp_Compensation_Switch;
extern int EnvironmentTemp;
//extern int RoomTemp;
extern int SetTemp;
extern unsigned char Stove_Hour,Stove_Min;
extern unsigned char Therm_RoomTemp,Therm_SetTemp;

extern unsigned char NetDelayTime;
extern unsigned char ThermostatControlFlag,ThermostatControlSer;

extern unsigned char SetTimeFlag;  //1min更新一次时间
extern unsigned char RemoteCtrFlag;
extern unsigned char LossCommTime;

extern unsigned char MainBoardTyp;   //主板型号
extern unsigned char MainBoardVer;   //主板版本号

extern unsigned char DisplayBoardTyp;  //显示板型号
extern unsigned char DisplayBoardVer;  //显示板版本号

extern unsigned char ComBoardVer;     //485通信板版本号
extern struct Stime UploadStartTime;

extern unsigned int PackLen;  

extern unsigned char HeartBeatSendFlag;
extern unsigned char HeartBeatSendNum;

extern unsigned char Uart9_Flag;
extern unsigned char ProtocolFlag;
extern unsigned char  Uart9_Check;
extern unsigned int  Uart9_DataNum;
extern unsigned char NetStatusFlag;

extern unsigned char ThermostatNum;
extern struct THERMOSTAT RoomThermostat[5];

extern unsigned char LDCMode;
//extern unsigned char MS_Flag;   //���ӱ�ʶ
extern unsigned char GetThermInfoFlag;
extern unsigned char ReadBeginFlag;
extern unsigned char GetThermInfoOverTime;
extern unsigned char CurrThermNo;

extern unsigned char SetTempFlag;
extern unsigned char  ManualTemper;       //�ֶ��¶�
extern unsigned char AntifreezTemper; 
//extern unsigned char CurrTimeNo,PreTimeNo;
extern unsigned char PreSetTemper,SetTemper;          //�趨�¶�
extern unsigned char MainThermostatMode;    //���¿�����ģʽ
extern unsigned char MainThermostatScheduleNum;
extern unsigned char CurrTimeFrame,PreTimeFrame;
extern struct TIME_FRAME MainThermostatSchedule[];   //���¿ر�̣���ʽ��ʱ���֡��¶ȣ�ÿ3�ֽ�һ��ʱ��
extern unsigned char TempSchedule[3];    //��ʱʱ��

extern unsigned char NonColdWateStatus;
extern unsigned char RevId[7]; 
extern struct LLS_CONTROL  LLS_Pump;
extern unsigned char RunOnceFlag;
extern unsigned char LLS_StartFlag;     
extern unsigned char LLS_HoldTime;    
extern unsigned char LLS_CuiserInterval;
extern unsigned char LLS_CuiserTime ;
extern unsigned char ScheduleCheckTime;
extern unsigned char ScheduleCheckFlag;
extern unsigned char PumpRunFlag;
extern unsigned char EnergySavingFlag;
extern unsigned char BathWaterUseFlag,BathWaterUseTime;
extern unsigned char WaterTempCheckFlag;
extern unsigned char LLS_ReSendFlag,LLS_ReSendTime;

extern unsigned int  NetRstTime;
extern unsigned char ThermEnableFlag;

extern unsigned char DeviceSta;
extern unsigned char RF_PairFlag,RF_PairTime;
extern unsigned char alarm_sta;
extern unsigned char ReissuedFlag,ReissuedTime;
extern unsigned char ReSendBuf[80];

extern unsigned char ActiveTime;
extern bool PowerOnFlag;

extern bool DevSosRunFlag;
extern unsigned char DevSosRunTime;

extern unsigned char XH_PumpFlag;
extern unsigned char XH_PumpId[8];

#endif /* EXTERNPARA_H_ */



