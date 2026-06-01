/*
 * sys.h
 *
 *  Created on: 2021年6月4日
 *      Author: WJQ
 */

#ifndef SYS_H_
#define SYS_H_

#include  "hal_data.h"
//#include  "r_icu.h"
//#include  "r_icu_cfg.h"
//#include  "bsp_api.h"

#include  "stdio.h"
#include  "string.h"
#include  "stdlib.h"

//#include  "../user_src/common_utils.h"
//#include  "../usr_lib/RFSPI.h"
//#include  "../usr_lib/SX1278.h"
//#include  "../usr_lib/at24.h"
//#include  "../usr_lib/nbpro.h"
//#include  "../usr_lib/datapro.h"
//#include  "../usr_lib/rf.h"

//#define  SoftWareVer     0x01
//#define  HardWareVer     0x11

#define  INPUT           0x0000
#define  OUTPUT          0xFFFF

//#define  BufferLenth     64
#define  SAVEMAXSIZE     80
//#define  UARTPOINTNUM    300
#if 0
#define  FacCode         0xFD
#define  ElcHeater       0x00
#define  GasHeater       0x01

#define  Uint_Sec        0x00      //时间单位
#define  Uint_Min        0x01
#define  Uint_Hour       0x02
#define  Uint_Day        0x03
#define  Uint_Week       0x04
#define  Uint_Month      0x05

//////////////////////////////////用户参数存储
#define  UserParaAddr     0      //0---119
#define  SaveInfoAddr     120
#define  DataSaveAddr     128
#define  OneFrameLen      40
#define  MaxSaveNum       1440
#endif
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

#define   RedLed_OFF     R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_00_PIN_00, BSP_IO_LEVEL_LOW)
#define   RedLed_ON      R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_00_PIN_00, BSP_IO_LEVEL_HIGH)

#define   GreenLed_OFF   R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_00_PIN_01, BSP_IO_LEVEL_LOW)
#define   GreenLed_ON    R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_00_PIN_01, BSP_IO_LEVEL_HIGH)

#define   RELAY_ON       R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_04_PIN_00, BSP_IO_LEVEL_HIGH)
#define   RELAY_OFF      R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_04_PIN_00, BSP_IO_LEVEL_LOW)

#if 0
#define   NB_RST_HIGH    R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_01_PIN_12, BSP_IO_LEVEL_HIGH)
#define   NB_RST_LOW     R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_01_PIN_12, BSP_IO_LEVEL_LOW)

//#define   NB_MODULE_ENABLE    R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_00_PIN_14, BSP_IO_LEVEL_HIGH)
//#define   NB_MODULE_DISABLE   R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_00_PIN_14, BSP_IO_LEVEL_LOW)
#define   NB_MODULE_ENABLE    R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_01_PIN_02, BSP_IO_LEVEL_HIGH)
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
} UploadStartTime;

#define  SettingSucc    0x55
#define  ResetSucc      0x66

#define  TIME_3S        3
#define  TIME_5S        5
#define  TIME_10S       10
#define  TIME_3M        180
#define  TIME_5M        300
#define  TIME_10M       600
#define  TIME_1H        3600
#endif

//#define UPGRADE_PARA_OFFSET     0x3000
//#define UPGRADE_DATA_OFFSET     0x3080
//
//#define BLOCK_SIZE              2048    //2K
//#define BLOCK_ADDRESS(x)        ((0) + BLOCK_SIZE * (uint32_t)(x))
//
//#define  USER_ADDR     0x4000


#endif /* SYS_H_ */





