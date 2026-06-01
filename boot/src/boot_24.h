/*
 * at24.h
 *
 *  Created on: 2021年6月4日
 *      Author: WJQ
 */

#ifndef AT24_H_
#define AT24_H_

#include  "sys.h"
//#include  "ExternPara.h"

//==================================参数存储地址列表
#define   DeviceStatus_Offset   2      // 传感器
#define   ComAddr_Offset        2      // 地址
#define   ManTemp_Offset        2      // 手动设定温度
#define   ExTempH_Offset        1 //2      // 外部传感器温度上限
#define   ExTempL_Offset        1 //2      // 外部传感器温度下限
#define   SetTempH_Offset       1 //2      //温度上限
#define   SetTempL_Offset       1 //2      //温度下限
#define   FreezeTemp_Offset     1          //防冻温度
#define   RunMode_Offset        1          //运行模式
#define   AreaNO_Offset         1          //区域号
#define   RoomNO_Offset         1          //房间号
#define   CalTemp_Offset        1          //校准温度
#define   FlucTemp_Offset       1            //限温回差
#define   SaveInt_Offset        2          //存储间隔
#define   HeatTime_Offset       4          //加热时长
#define   Code_Offset           2          // 密码
#define   CodeEn_Offset         1          // 密码使能
#define   SwTemp_Offset         1          // 波动温度
#define   LockFlag_Offset       1          // 锁标志

#define   DeviceAddr_Offset     2          //对码值
#define   DeviceId_Offset       7          //设备地址

//
//#define   UpLoadMode_Offset     1          //上传模式,按小时
//#define   UpLoadCycle_Offset    1          //上传模式,按小时
//#define   UpLoadHour_Offset     1
//#define   UpLoadMin_Offset      1
//#define   UpLoadSec_Offset      1
//#define   LinkDelayTime_Offset  1

#define   UpLoadTime_Offset       6          //上传模式,按小时

#define   WifiPort_Offset       3
#define   WifiDns_Offset        30


#define   Prog_Offset           360 //每天10时段，编程的最大长度   10 * 5 *7 = 350

#define   EEpromSaveFlag_Offset 2

//#define   ComAddr_Addr          0x0000//(DeviceStatus_Addr + DeviceStatus_Offset)


#define   EEpromSaveFlag_Addr   0x0000
#define   Prog_Addr             0x0004

#define   USER_PARA_Addr        (Prog_Addr + Prog_Offset)      //参数首地址

#define   DeviceStatus_Addr      USER_PARA_Addr
#define   ComAddr_Addr          (DeviceStatus_Addr + DeviceStatus_Offset)
#define   ManTemp_Addr          (ComAddr_Addr + ComAddr_Offset)
#define   ExTempH_Addr          (ManTemp_Addr + ManTemp_Offset)
#define   ExTempL_Addr          (ExTempH_Addr + ExTempH_Offset)
#define   SetTempH_Addr         (ExTempL_Addr + ExTempL_Offset)
#define   SetTempL_Addr         (SetTempH_Addr + SetTempH_Offset)
#define   FreezeTemp_Addr       (SetTempL_Addr + SetTempL_Offset)
#define   RunMode_Addr          (FreezeTemp_Addr + FreezeTemp_Offset)
#define   AreaNO_Addr           (RunMode_Addr + RunMode_Offset)
#define   RoomNO_Addr           (AreaNO_Addr + AreaNO_Offset)
#define   CalTemp_Addr          (RoomNO_Addr + RoomNO_Offset)
#define   FlucTemp_Addr         (CalTemp_Addr + CalTemp_Offset)
#define   SaveInt_Addr          (FlucTemp_Addr + FlucTemp_Offset)
#define   HeatTime_Addr         (SaveInt_Addr + SaveInt_Offset)
#define   Code_Addr             (HeatTime_Addr + HeatTime_Offset)
#define   CodeEn_Addr           (Code_Addr + Code_Offset)
#define   SwTemp_Addr           (CodeEn_Addr + CodeEn_Offset)
#define   LockFlag_Addr         (SwTemp_Addr + SwTemp_Offset)

#define   DeviceAddr_Addr       (LockFlag_Addr + LockFlag_Offset)
#define   DeviceId_Addr         (DeviceAddr_Addr + DeviceAddr_Offset)

//#define   UpLoadMode_Addr         (DeviceId_Offsetr_Addr + DeviceId_Offset)
//#define   UpLoadCycle_Addr        (UpLoadMode_Addr + UpLoadMode_Offset)
//#define   UpLoadHour_Addr         (UpLoadCycle_Addr + UpLoadCycle_Offset)
//#define   UpLoadMin_Addr          (UpLoadHour_Addr + UpLoadHour_Offset)
//#define   UpLoadSec_Addr          (UpLoadMin_Addr + UpLoadMin_Offset)
//#define   LinkDelayTime_Addr      (UpLoadSec_Addr + UpLoadSec_Offset)

#define   UpLoadTime_Addr        (DeviceId_Addr + DeviceId_Offset)

#define   WifiPort_Addr          (UpLoadTime_Addr + UpLoadTime_Offset)
#define   WifiDns_Addr           (WifiPort_Addr + WifiPort_Offset)

//=====================================================

//#define   SDA_IN    R_IOPORT_PinRead(&g_ioport_ctrl, BSP_IO_PORT_00_PIN_13, &level);//P6_bit.no1

//#define   SDA_BIT   0x01//BIT0
//#define   SCL_BIT   0x02//BIT1

#define   AT24_POWERON    ;//P12_bit.no5 = 1
#define   AT24_POWEROFF   ;//P12_bit.no5 = 0

#define   SDA_SET_OUT     R_IOPORT_PortDirectionSet(&g_ioport_ctrl, BSP_IO_PORT_00, OUTPUT, BIT13)//PM6_bit.no1 = 0
#define   SDA_SET_IN      R_IOPORT_PortDirectionSet(&g_ioport_ctrl, BSP_IO_PORT_00, INPUT, BIT13)//PM6_bit.no1 = 1
#define   SDA_HIGH        R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_00_PIN_13, BSP_IO_LEVEL_HIGH)//P6_bit.no1 = 1
#define   SDA_LOW         R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_00_PIN_13, BSP_IO_LEVEL_LOW)//P6_bit.no1 = 0

#define   SCL_SET_OUT     R_IOPORT_PortDirectionSet(&g_ioport_ctrl, BSP_IO_PORT_00, OUTPUT, BIT12)//PM6_bit.no0 = 0
#define   SCL_SET_IN      R_IOPORT_PortDirectionSet(&g_ioport_ctrl, BSP_IO_PORT_00, INPUT, BIT12)//PM6_bit.no0 = 1
#define   SCL_HIGH        R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_00_PIN_12, BSP_IO_LEVEL_HIGH)//P6_bit.no0 = 1
#define   SCL_LOW         R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_00_PIN_12, BSP_IO_LEVEL_LOW)//P6_bit.no0 = 0

//#define   SDA_SET_OUT     R_IOPORT_PortDirectionSet(&g_ioport_ctrl, BSP_IO_PORT_00, BIT13, BIT13)//PM6_bit.no1 = 0
//#define   SDA_SET_IN      R_IOPORT_PortDirectionSet(&g_ioport_ctrl, BSP_IO_PORT_00, (~BIT13), BIT13)//PM6_bit.no1 = 1
//#define   SDA_HIGH        R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_00_PIN_13, BSP_IO_LEVEL_HIGH)//P6_bit.no1 = 1
//#define   SDA_LOW         R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_00_PIN_13, BSP_IO_LEVEL_LOW)//P6_bit.no1 = 0
//
//#define   SCL_SET_OUT     R_IOPORT_PortDirectionSet(&g_ioport_ctrl, BSP_IO_PORT_00, BIT12, BIT12)//PM6_bit.no0 = 0
//#define   SCL_SET_IN      R_IOPORT_PortDirectionSet(&g_ioport_ctrl, BSP_IO_PORT_00, (~BIT12), BIT12)//PM6_bit.no0 = 1
//#define   SCL_HIGH        R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_00_PIN_12, BSP_IO_LEVEL_HIGH)//P6_bit.no0 = 1
//#define   SCL_LOW         R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_00_PIN_12, BSP_IO_LEVEL_LOW)//P6_bit.no0 = 0


unsigned char boot_AT24_readbyte(unsigned int Addr,unsigned char* buf,unsigned int num);
unsigned char boot_AT24_writebyte(unsigned int Addr,unsigned char *buf,unsigned int num);
void boot_AT24_start(void);
void boot_AT24_stop(void);
unsigned char boot_AT24_bitin(void);
void boot_AT24_bitout(unsigned char ch);
unsigned char boot_AT24_rbyte(void);
unsigned char boot_AT24_wbyte(unsigned char ch);
//void AT24_FunctionEnable(void);
//void AT24_FunctionDisable(void);
void boot_Delay_Ms(unsigned int tt);
void boot_Delay10us(void);

//unsigned char ReadData(void);
//void SaveData(void);
////void SavePara(void);
////void SaveSysPara(void);
////void ReadSysPara(void);
//void InitData(void);
//void Buf_To_Para(void);
//void Para_To_Buf(void);
//void LoadSysPara(void);
//void SavePara(void);
//void SaveHisData(void);
#endif /* AT24_H_ */

