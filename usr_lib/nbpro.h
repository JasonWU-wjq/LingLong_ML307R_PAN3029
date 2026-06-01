/*
 * nbpro.h
 *
 *  Created on: 2021年6月5日
 *      Author: WJQ
 */

#ifndef NBPRO_H_
#define NBPRO_H_

#include  "../usr_lib/ExternPara.h"

//extern unsigned int chkcrc(unsigned char *buf,unsigned int len);

unsigned char HexToBCD(unsigned char data);
unsigned char BCDToHex(unsigned char data);

unsigned char CaculateWeekDay(int y,int m,int d);
void UpdateCurrTime(void);

void GetNetTime(unsigned char *year,unsigned char *month,unsigned char *day,unsigned char *hour,unsigned char *min,unsigned char *sec);
void WifiParaSend(unsigned char *buf,int len);
void HexToString(unsigned char *DataBuffer,unsigned int DataLenl);
void StringToHex(unsigned char *DataBuffer,unsigned int DataLenl);
void Nb_Reset(void);
void Nb_ModuleClose(void);
unsigned int ReadRevData(unsigned char* buffer, int limit);
unsigned char ReCommand(void);
unsigned char CheckModule(void);
unsigned char GetImeiForMac(void);
unsigned char GetIccid(void);
unsigned char Nb_Connect(void) ;
unsigned char GprsConnect(void);
unsigned char SendNbData(unsigned char *buf,unsigned int len) ;
unsigned char ReadNbData(unsigned char *buf,unsigned int len);
unsigned char R_uart0_send(unsigned char *sbuf,unsigned char len,unsigned char num);
void SetGasHeaterRst(void);
#endif /* NBPRO_H_ */
