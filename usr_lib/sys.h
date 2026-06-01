/*
 * sys.h
 *
 *  Created on: 2021年6月4日
 *      Author: WJQ
 */

#ifndef SYS_H_
#define SYS_H_

#include  "hal_data.h"
#include  "r_icu.h"
#include  "r_icu_cfg.h"
#include  "bsp_api.h"

#include  "stdio.h"
#include  "string.h"
#include  "stdlib.h"
#include  "stdbool.h"

#include  "../user_src/common_utils.h"
#include  "../usr_lib/RFSPI.h"
#include  "../usr_lib/PNA3029.h"
#include  "../usr_lib/at24.h"
#include  "../usr_lib/nbpro.h"
#include  "../usr_lib/datapro.h"
#include  "../usr_lib/rf.h"

#define  LocalAddr    0x00    //modbus  锜科地址

#define  DI          __disable_irq()
#define  EI          __enable_irq()

//1101完善升级请求帧数据长度 ，恢复1101为新版本开始
//1102增加壁挂炉预设命令，WIFI温控器操作命令
//1103预设命令，增加模块读取信息寄存器长度0x13---0x18,设置温度时一次设置3个温度 环境温度/室温/设定温度
//1104修改modbus协议数据unsigned---》int，可以支持高低字节写入,收到温控器应答后直接更新壁挂炉相关温度
//1105修改串口接收长度，增加联网成功后更新壁挂炉时间，增加1min更新壁挂炉时间
//1106打开WeekSet宏定义，设置星期
//1108增加联网后设置星期  ，增加远程锁定功能  == 09
//1110增加串口失联时间锁定
//1111修改锁定设定顺序
//1112壁挂炉主板、显示板、485通信板版本号
//31xx---锜科主板   21xx---昊森主板
//3101和东壁挂炉新版本iar环境=e2 studio  1114版本,硬件采用swd接口下载
//3120开始aep平台
//3182---CAT1ģʽ����������ˮ������ģʽ��ˮ��ͷ���ƣ�����¿�ģʽ���¶��Լ����
//3183---��ʱ�������ԡ��Թ�
//3184---�������ñڹ�¯���������Ч������״̬�ı��ϱ���Ϣ
//3185---����ˮ�����Ż������ӳ��±������ط�
//3280---cat1ģ�飬�޸�ΪUMA603,����XIAO_RUI_NO_RF��ȡ������ˮ�������ƺл���
//3281---����WiFi������ʾ
//3282---�������������ϱ�����
//3283---�Ż�����ˮ���ơ����ӿ����͹ر�AI����ģʽ����F01D
//3284---����ǰ��������ƣ���ӵ�ذ汾�ƺ�
//3285----����ˮ�����Ż�
//3286---����СԲ�ƺУ��Ż���ʱʱ��Ϊ8s�����Ӻ��й��ܣ���չˮ�������Ϣ
//3786---ML307R�汾������СԲ�ƺУ��Ż���ʱʱ��Ϊ8s�����Ӻ��й��ܣ���չˮ�������Ϣ
//3787---ML307Rģ�飬����WDT����.WDT������33s
//3788--ѭ����
//#define VER_07     //�ɰ汾����

//#define   NET_DELAY_TEST        //���������ӳ�ʹ�ã���ʽ�汾���ô˶���

#ifdef  VER_07
  #define SoftWareVer    0x07
#else
  #define  SoftWareVer   0x01

  #define  RemoteCtr
  #define  PreSetting_Version      //预设命令调试

  #if (SoftWareVer >= 0x06)
  #define  WeekSet
  #endif

  #if(SoftWareVer >= 0x50)
  #define VerInfoCtr
  #endif

//  #if(SoftWareVer >= 0x20)    //aep平台
//    #define  AEP_BOARD     
//  #endif
#endif

#define  HardWareVer     0x97           //锜科  48pin_256K flash，不同硬件单独再做说明
 
#define  WDT_FEED       R_WDT_Refresh(&g_wdt0_ctrl)

//#define    XIAO_RUI_NO_RF      //С����Я�����߹���

//#define  RemoteCtr
//#define  PreSetting_Version      //预设命令调试
//
//#if (SoftWareVer >= 0x06)
//#define  WeekSet
//#endif

#define  RESET_MCU          NVIC_SystemReset();

#define  INPUT           0x0000
#define  OUTPUT          0xFFFF

#define  BufferLenth     64
#define  SAVEMAXSIZE     180//120//80     �洢��������С
#define  UARTPOINTNUM    255

#define  HeartBeatLength  32     //wifi温控器心跳包总长度

#define  FacCode         0xFD
#define  ElcHeater       0x00
#define  GasHeater       0x11//0x01   

#define  Uint_Sec        0x00      //时间单位
#define  Uint_Min        0x01
#define  Uint_Hour       0x02
#define  Uint_Day        0x03
#define  Uint_Week       0x04
#define  Uint_Month      0x05
//////////////////////////////////用户参数存储
#define  UserParaAddr     0      //0---160
#define  SaveInfoAddr     200    //�洢��ַ��������Ϣ���ڵ�ַ
#define  DataSaveAddr     210    //�洢��Ϣ��ʼ��ַ
#define  OneFrameLen      40
#define  MaxSaveNum       100//255   //1440   //���洢����

//===============================================升级参数
#define  USER_PARA_OFFSET   0x3000    //用户参数存储地址
#define  USER_DATA_OFFSET   0x3080    //程序存储起始地址
//================================================

#define  UINT32    unsigned long
#define  INT32     signed long
#define  UINT16    unsigned int
#define  INT16     signed int
#define  UINT8     unsigned char
#define  INT8      char
#define  SUINT8    signed char

/////////////////////////////////////////////////////////////////////////
#define  delayms(x)      Delay_Ms(x)//R_BSP_SoftwareDelay(x,BSP_DELAY_UNITS_MILLISECONDS)
#define  delayus(t)      R_BSP_SoftwareDelay(t,BSP_DELAY_UNITS_MICROSECONDS)

#define   BIT0           0x0001
#define   BIT1           0x0002
#define   BIT2           0x0004
#define   BIT3           0x0008
#define   BIT4           0x0010
#define   BIT5           0x0020
#define   BIT6           0x0040
#define   BIT7           0x0080
#define   BIT8           0x0100
#define   BIT9           0x0200
#define   BIT10          0x0400
#define   BIT11          0x0800
#define   BIT12          0x1000
#define   BIT13          0x2000
#define   BIT14          0x4000
#define   BIT15          0x8000

#define   RedLed_OFF    R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_00_PIN_00, BSP_IO_LEVEL_LOW)
#define   RedLed_ON     R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_00_PIN_00, BSP_IO_LEVEL_HIGH)

#define   GreenLed_OFF     R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_00_PIN_01, BSP_IO_LEVEL_LOW)
#define   GreenLed_ON      R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_00_PIN_01, BSP_IO_LEVEL_HIGH)

#define   RELAY_ON       R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_04_PIN_00, BSP_IO_LEVEL_HIGH)
#define   RELAY_OFF      R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_04_PIN_00, BSP_IO_LEVEL_LOW)

#define   NB_RST_HIGH    R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_01_PIN_12, BSP_IO_LEVEL_HIGH)
#define   NB_RST_LOW     R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_01_PIN_12, BSP_IO_LEVEL_LOW)

//#define   NB_MODULE_ENABLE    R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_00_PIN_14, BSP_IO_LEVEL_HIGH)
//#define   NB_MODULE_DISABLE   R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_00_PIN_14, BSP_IO_LEVEL_LOW)
#define   NB_MODULE_ENABLE     R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_01_PIN_02, BSP_IO_LEVEL_HIGH)
#define   NB_MODULE_DISABLE   R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_01_PIN_02, BSP_IO_LEVEL_LOW)
//#define   MachinePower_18kW
//#define   MachinePower_20kW
//#define   MachinePower_24kW
#define   MachinePower_26kW
//#define   MachinePower_28kW
//#define   MachinePower_30kW
//#define   MachinePower_32kW
//#define   MachinePower_36kW
//#define   MachinePower_38kW
//#define   MachinePower_40kW

#define   WorkingPower     30     //150w * 200ms = 30w.s
#define   NoWorkingPower   8      //40w * 200ms  = 8w.s

struct Stime
{
   uint8_t  year;
   uint8_t  month;
   uint8_t  day;
   uint8_t  hour;
   uint8_t  min;
   uint8_t  sec;
};// UploadStartTime;

#define  NULL_SAVE       0x00
#define  HISDATA_SAVE    0x01
#define  PARA_SAVE       0x02
//enum DataStatus
//{
//  NO_DATA,
//  
//}

#define  TIMER_1S       1
#define  TIMER_2S       2
#define  TIMER_3S       3
#define  TIMER_5S       5
#define  TIMER_6S       6
#define  TIMER_8S       8
#define  TIMER_10S      10
#define  TIMER_15S      15
#define  TIMER_30S      30
#define  TIMER_1M       60

#define  SetONOFF       0x11
#define  SettingSucc    0x55
#define  ResetSucc      0x66

#define  TIME_3S        3
#define  TIME_5S        5
#define  TIME_10S       10
#define  TIME_3M        180
#define  TIME_5M        300
#define  TIME_10M       600
#define  TIME_15M       900
#define  TIME_20M       1200
#define  TIME_30M       1800
#define  TIME_1H        3600

#define  GetInfoTime    1//5
//==================================================================QK modbus addr
#define  HeatingWater      0x0004
#define  BathWater         0x0005
#define  FireSize          0x0006
#define  ON_OFF            0x0007
#define  Win_Summer        0x0008
#define  CumulativeGasL    0x0009
#define  CumulativeGasH    0x000A        //清零操作此寄存器
 
#define  D_Status          0x000B//11==status
  
#define  CumulativePwrL      12
#define  CumulativePwrH      13        //清零操作此寄存器

#define  FlucTempReg         14

#define  HeaterMode_offset   15

#define  CurrFlowReg         16       //清除水量操作此寄存器

#define  CumulativeWaterL    17
#define  CumulativeWaterH    18

#define  TempCompenation_offset        19
#define  EnvironmentTemp_offset        20
#define  RoomTemp__offset              21
#define  SetTemp__offset               22
#define  StoveTime__offset             23

#define  RemoteCtr_offset              24

#define  LossComm_offset               25

#define  NO_CONNECT          0
#define  CONNECK_OK          1
#define  TRANS_MODE                 //͸��ģʽ

struct THERMOSTAT
{
  unsigned char id[7];      //���
  int           room_temp; //����
  int           set_temp;  //�趨�¶�
  unsigned char rh;        //ʪ��
  unsigned char fire;      //�Ƿ����
  unsigned char m_s;       //�Ƿ�������
}__attribute__((aligned(1)));

struct TIME_FRAME
{
  unsigned char hour;
  unsigned char min;
  unsigned char temper;
};

#define  TIME_FRAME_NUM   6

#define  AUTO_RUN                      0  //0x66
#define  MANVAL_RUN                    1  //0xaa
#define  ANTIFREEZE_RUN                2  //0x99

struct LLS_CONTROL
{
  unsigned int  pump_time;
  unsigned char run_mode;
  unsigned char control_mode;
  unsigned char cuiser_interval;
  unsigned char control_temp;
  unsigned char schedule_num;
  unsigned char schedule[48];
  unsigned char ec_num;
  unsigned char ec_time[12];       //ÿ4��һ��
};

#define  LLS_ON         1
#define  LLS_OFF       0
#define  RUN_ONCE_MODE  1
#define  CUISER_MODE    2
#define  SCHEDULE_MODE  3

#define  NO_PUMP        0
#define  PUMP_CLOSE     1
#define  PUMP_OPEN      2

#define  OVERTEMPTIME   8   

#define  SOCKET_ID      0
#define  LLS_PUMP       0
#define  XH_PUMP        1

#define    NormalState             1
#define    MatchCode               2
#define    ConfigNet               3 
#define    S_PRESSKEY              4

#define    RF_PAIR              'a'
#define    RF_SHAKE             'b'
#define    THERM_REPORT         'c'
#define    THERM_ACK            'd'
#define    SET_BOILER_PARA      'e'
#define    SET_SOS              'f'

#define  ONLINE    1
#define  OFFLINE   0  

#define   ALARM_ICON           BIT0
#define   NET_ICON             BIT1
#define   NON_COLDWATER_ICON   BIT2
#define   AIMODE_ICON          BIT3  

#define  PUMP_WORKLIMT        20

#define  LoRaChannel          10

#define  ActiveHoldTime       15

#endif /* SYS_H_ */






