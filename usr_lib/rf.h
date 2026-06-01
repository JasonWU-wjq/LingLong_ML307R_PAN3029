/*
 * rf.h
 *
 *  Created on: 2021年6月22日
 *      Author: WJQ
 */

#ifndef RF_H_
#define RF_H_

//#include   "sys.h"
#include   "PNA3029.h"
#include   "ExternPara.h"

unsigned char ProcessFrame(void);
void ProcessRF(void);
//void sendframe(unsigned char ch);
unsigned char LoRaSendSatusToThermostat(void);

unsigned char ReadThermostatInfo(unsigned char n);

void LoRaSendSettingTemp(unsigned char *id,unsigned char temp);

void SendRecData(unsigned char flag,unsigned char type) ;
#endif /* RF_H_ */
