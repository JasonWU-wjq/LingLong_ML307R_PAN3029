/*
 * SX1278.h
 *
 *  Created on: 2021年6月3日
 *      Author: WJQ
 */

#ifndef SX1278_H_
#define SX1278_H_


#include "../usr_lib/RFSPI.h"

#define RegFifo                 0x00
#define RegOpMode           0x01
#define RegRes02            0x02
#define RegRes03            0x03
#define RegFdevMsb              0x04
#define RegRes05                0x05
#define RegFrfMsb           0x06
#define RegFrfMid           0x07
#define RegFrfLsb           0x08
#define RegPaConfig         0x09
#define RegPaRamp           0x0A
#define RegOcp                  0x0B
#define RegLna                  0x0C

#define RegFifoAddrPtr          0x0D
#define RegFifoTxBaseAddr   0x0E
#define RegFifoRxBaseAddr       0x0F
#define RegFifoRxCurrentAddr    0x10
#define RegIrqFlagsMask         0x11
#define RegIrqFlags         0x12
#define RegRxNbBytes            0x13

#define RegRxHeaderCntValueMsb  0x14
#define RegRxHeaderCntValueLsb  0x15
#define RegRxPacketCntValueMsb  0x16
#define RegRxPacketCntValueLsb  0x17

#define RegModemStat        0x18
#define RegPktSnrValue      0x19
#define RegPktRssiValue         0x1A
#define RegRssiValue            0x1B
#define RegHopChannel           0x1C
#define RegModemConfig1         0x1D
#define RegModemConfig2     0x1E
#define RegSymbTimeout          0x1F

#define RegPreambleLengthL      0x20
#define RegPreambleLengthH      0x21
#define RegPayloadLength        0x22
#define RegMaxPayloadLength 0x23
#define RegHoppingPeriod    0x24
#define RegFifoRxByteAddr   0x25
#define RegModemConfig3         0x26

#define RegDioMapping1      0x40
#define RegDioMapping2      0x41
#define RegVersion          0x42

#define RegPllHop           0x44
#define RegTcxo             0x4B
#define RegPaDac            0x4D

#define RegFormerTemp       0x5B
#define RegBitrateFrac      0x5D

#define RegAgcRef           0x61
#define RegAgcThresh1       0x62
#define RegAgcThresh2       0x63
#define RegAgcThresh3       0x64

#define RegPll              0x70

void InitRF(void);

void SX1278Settings(void);
void SX1278_RX(void);
void SX1278_Sleep(void);
void SX1278_CADInit(void);
unsigned char SX1278ReceivePacket(unsigned char* RxBuffer);
void SX1278SendPacket(unsigned char *TxBuf, unsigned char Len);
void SX1278_RST(void);



#endif /* SX1278_H_ */
