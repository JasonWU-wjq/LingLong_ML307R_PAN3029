/*
 * main.h
 *
 *  Created on: 2021年6月3日
 *      Author: WJQ
 */

#ifndef MAIN_H_
#define MAIN_H_

#include  "sys.h"

void system_init(void);
void user_main(void);

unsigned char FireFlag;

unsigned char SysYear,SysMonth,SysDay,SysHour,SysMinute,SysSecond,SysWeek;

unsigned char DateUpdateFlag;

unsigned char Uart0_Tx_buf[15] = "123456789";
unsigned char Uart0_Rx_buf[64] = {0};
//unsigned char Uart0_Rx_buf1[35];
unsigned char Uart0_Rx_index = 0;

//unsigned char Uart9_Tx_buf[15] = "\r\n";
//unsigned char Uart9_Rx_buf[15] = {0};
//unsigned char Uart9_Rx_index = 0;

bool g_uart0_recv_complete = false;
bool g_uart0_send_complete = false;
bool g_uart9_recv_complete = false;
bool g_uart9_send_complete = false;

bool g_timer_1ms_generated = false;
bool g_rtc_period_generated = false;

unsigned char Uart0_Flag;
//unsigned char Uart0_Valid;
unsigned char Uart0_Check;

unsigned int USCIA0RXNum,UART_RxNum;

unsigned char USCIA0RXBUF[UARTPOINTNUM];
unsigned char TxBuffer[UARTPOINTNUM];

unsigned char CmdRunTime,UartDelayTime,CommReadTime;

unsigned char Imei[20],Iccid[30];

unsigned char  WifiDns[30];
unsigned int WifiPort;
unsigned char  Csq;
unsigned char  RevChar;

unsigned char  DeviceId[7];
unsigned char  FlashBuf[SAVEMAXSIZE];

unsigned char  GroupNo;
unsigned char  PeakTime;

unsigned char  UpLoadMode;
unsigned char  UpLoadCycle;
unsigned char  UpLoadCycleUint;
unsigned char  UpLoadYear;
unsigned char  UpLoadMonth;
unsigned char  UpLoadDay;
unsigned char  UpLoadHour;
unsigned char  UpLoadMin;
unsigned char  UpLoadSec;
unsigned char  LinkDelayTime;

unsigned char  ConnectFlag,ConnectHoldTime;

unsigned char  DataSaveFlag;
unsigned char  DataSaveCycle;
unsigned char  DataSaveCycleUint;
unsigned int   DataSaveNum;
unsigned int   CurrSaveAddr;
unsigned char  ECL,RSRP[2],RSSI[2],SNR[2],CELL_ID[4],PCI[2],EARFCN[2];

//========================壁挂炉数据=============================
unsigned char  DevOnOff;
unsigned char  RunMode;
unsigned char  RoomTemp;
unsigned char  FirePower;

int SysRunStatus;
int PreRunStatus;

unsigned char  Dev_Status[2];    //2状态字节
unsigned char  ErrorFlag;       //故障代码
unsigned char  BathWaterTemp;   //卫浴水温度
unsigned char  BathWaterTargetTemp,PreBathWaterTargetTemp;   //卫浴水目标温度
unsigned char  HeatBathWaterTemp;   //供暖水温度
unsigned char  HeatBathWaterTargetTemp,PreHeatBathWaterTargetTemp;   //供暖水目标温度
unsigned char  FlucTemp;   //回差温度
unsigned char  CurrFlowRate;   //瞬时流量
unsigned char  DevFacPara[4];         //出厂参数
unsigned char  MinPower,MaxPower,FirePower;
unsigned char  ProValveOut;          //比例阀输出
unsigned char  UserId;
unsigned char  DevSoftVer[2];
unsigned char  SensorPara;
unsigned char  DevType;
//===============================================================
unsigned char  SaveFlag;
unsigned long SavingTime;
unsigned long UpLoadTime;
unsigned int GetDataTime;

unsigned long SaveWaitingTime;
unsigned long UpLoadWaitingTime;

unsigned char  FrameSer;

unsigned long WaterUsingTotalTime;
unsigned long GasUsingTotalTime;

//unsigned int WaterPulseNum;
unsigned int TempWaterVol;
unsigned long WaterTotalVol;

//unsigned int TempGasVol;
unsigned long GasTotalVol;

unsigned int TempElecPower;
unsigned long ElecTotalPower;
unsigned long PowerUsingTime;

unsigned char  ReceiveFlag;
unsigned char  RF_RxBuf[BufferLenth];
unsigned char  ThermostatId[7];
unsigned char  FrameSer;
unsigned char  FrameDir;

unsigned int  RF_HandShakeTime;

unsigned char KeyPressHoldTime;
unsigned char KeyPressFlag;
unsigned char ErrFirstHappenFlag;
unsigned int  ErrHoldTime;

unsigned char SetUpFlag,ComValid;
//=========================================upgrade
unsigned int  Curraddr;
unsigned int  SoftVer,TempVer;
unsigned int  TotalNum;
unsigned int  CurrFrameSer;
unsigned char StatusBuf[15];   /*buf[0]:是否需要升级Y/N；buf[1]:升级结束55/AA；buf[2--3]:程序版本号；buf[4--5]:总帧数；buf[6--7]:当前帧序号；*/

unsigned char UpgradeOverTime;
unsigned char UpgradeFlag;
unsigned char UpgradeNum;

unsigned char AutoConnectFlag;
unsigned char TimingSaveFlag;

unsigned char Thermo_Buf[50];

unsigned char Temp_Compensation_Switch;
int EnvironmentTemp;
//int RoomTemp;
int SetTemp;
unsigned char Stove_Hour,Stove_Min;
unsigned char Therm_RoomTemp,Therm_SetTemp;

unsigned char NetDelayTime;
unsigned char ThermostatControlFlag,ThermostatControlSer;

unsigned char SetTimeFlag;

unsigned char RemoteCtrFlag;  //远程锁定
unsigned char LossCommTime;   //串口失联时间

unsigned char MainBoardTyp;   //主板型号
unsigned char MainBoardVer;   //主板版本号

unsigned char DisplayBoardTyp;  //显示板型号
unsigned char DisplayBoardVer;  //显示板版本号

unsigned char ComBoardVer;     //485通信板版本号
struct Stime UploadStartTime;

unsigned int PackLen;    //�������ݰ�����

unsigned char HeartBeatSendFlag;
unsigned char HeartBeatSendNum;

unsigned char Uart9_Flag;
unsigned char ProtocolFlag;
unsigned char  Uart9_Check;
unsigned int  Uart9_DataNum;
unsigned char NetStatusFlag;

unsigned char ThermostatNum;
struct  THERMOSTAT  RoomThermostat[5];

unsigned char LDCMode;   //ǰ������ģʽ��1��ʾ����ǰ����0������
//unsigned char MS_Flag;   //���ӱ�ʶ
unsigned char GetThermInfoFlag;
unsigned char ReadBeginFlag;
unsigned char GetThermInfoOverTime;
unsigned char CurrThermNo;

unsigned char SetTempFlag;
unsigned char ManualTemper;       //�ֶ��¶�
unsigned char AntifreezTemper;   //�����¶�
//unsigned char CurrTimeNo,PreTimeNo;
unsigned char PreSetTemper,SetTemper;          //�趨�¶�
unsigned char MainThermostatMode;    //���¿�����ģʽ
unsigned char MainThermostatScheduleNum;   //ʱ����
unsigned char CurrTimeFrame,PreTimeFrame;
struct TIME_FRAME MainThermostatSchedule[TIME_FRAME_NUM];   //���¿ر�̣���ʽ��ʱ���֡��¶ȣ�ÿ3�ֽ�һ��ʱ��
unsigned char TempSchedule[3];    //��ʱʱ��

unsigned char NonColdWateStatus;
unsigned char RevId[7]; 
struct LLS_CONTROL  LLS_Pump;
unsigned char RunOnceFlag;
unsigned char LLS_StartFlag;     
unsigned char LLS_HoldTime;    
unsigned char LLS_CuiserInterval;
unsigned char LLS_CuiserTime ;
unsigned char ScheduleCheckTime;
unsigned char ScheduleCheckFlag;
unsigned char PumpRunFlag;
unsigned char EnergySavingFlag;
unsigned char BathWaterUseFlag,BathWaterUseTime;
unsigned char WaterTempCheckFlag;
unsigned char LLS_ReSendFlag,LLS_ReSendTime;

unsigned int  NetRstTime;

unsigned char ThermEnableFlag;

unsigned char DeviceSta;
unsigned char RF_PairFlag,RF_PairTime;
unsigned char alarm_sta;
unsigned char ReissuedFlag,ReissuedTime;

unsigned char ActiveTime;

bool PowerOnFlag;

bool DevSosRunFlag;
unsigned char DevSosRunTime;

unsigned char XH_PumpFlag;
unsigned char XH_PumpId[8];
unsigned char ReSendBuf[80];
#endif /* MAIN_H_ */


