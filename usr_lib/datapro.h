/*
 * datapro.h
 *
 *  Created on: 2021年6月10日
 *      Author: WJQ
 */

#ifndef DATAPRO_H_
#define DATAPRO_H_

#include  "sys.h"
#include  "ExternPara.h"

#define   ONOFF                BIT7
#define   WIN_SUM              BIT6
#define   WarmTarget           BIT5
#define   BathTarget           BIT4
#define   FlucTarget           BIT3
#define   MinPSeting           BIT2
#define   MaxPSeting           BIT1
#define   FirePSeting          BIT0

#define   ValveType            BIT7
#define   WaterSwitch          BIT6
#define   HeatingMode          BIT5
#define   MachineType          BIT4
#define   HeatExchanger        BIT3
#define   FlowSwitchType       BIT2
#define   BlockValve           BIT1
#define   PumpMode             BIT0

#define   BurningMode          BIT7
#define   PressSeneor          BIT6
#define   MachinePwrType       BIT5

#define   TempComps            BIT4
#define   EnvTemp              BIT3
#define   StoveTime            BIT2
#define   HeaterMode           BIT1
#define   RemoteLock           BIT0

unsigned char calXOR(unsigned char *buf,unsigned char len);
unsigned char HexToBCD(unsigned char data);
unsigned char BCDToHex(unsigned char data);
unsigned int calccrc(unsigned char crcbuf,unsigned int crc);
unsigned int chkcrc(unsigned char *buf,unsigned int len);

unsigned char SendHeartBeatFrame(void) ;
unsigned char SendNbRegFrame(void) ;
unsigned char SendUpLoadFrame(void);
unsigned char SendParaSettingAck(void);
unsigned int ProNbData(void);
void ProcessRevData(void);
void SetParaToGasHeater(unsigned char *probuf);
unsigned char SendRequestFrame(uint8_t flag);
unsigned char SendThermostatData(void);
void QK_ReadAllRegisters(void);
void QK_SetSignleRegister(unsigned int reg,int data);

unsigned char ReportThermostatInfo(unsigned char n);
void UpLoadNonColdWaterStatus(void);   
unsigned char CheckEnergySaving(void);
#endif /* DATAPRO_H_ */

