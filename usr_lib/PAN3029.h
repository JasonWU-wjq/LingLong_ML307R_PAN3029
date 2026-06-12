/*
 * PAN3029.h
 *
 *  Created on: 2026-05-14
 *      Author: ported from reference/user
 *
 * PAN3029 LoRa 收发芯片接口（替代 SX1278）。文件名/函数名沿用参考工程。
 * 硬件接线沿用原 SX1278 引脚（见 RFSPI.h），SPI 传输的地址字节格式为
 * (addr<<1)|0x01 写、(addr<<1)&0xFE 读，与 SX1278 不同，因此 RFSPI 的
 * 读写函数已同步调整，原 SX1278.c 不再被调用。
 */

#ifndef PAN3029_H_
#define PAN3029_H_

#include "../usr_lib/RFSPI.h"
/* 不在此处 include ExternPara.h —— ExternPara.h 反向 include sys.h，
 * 而 sys.h 又 include 本头文件，会因 include-guard 提前返回导致全局变量
 * 声明丢失。本头只暴露函数原型与自身常量，由调用方在 .c 中包含 sys.h 即可。*/

#ifndef CHANNEL_10
#define CHANNEL_10                     10
#endif

#define PAN3029_RF_BASE_FREQ_HZ        477000000UL
#define PAN3029_RF_CHANNEL_STEP_HZ     250000UL
#define PAN3029_RF_CHANNEL             CHANNEL_10
#define PAN3029_RF_FREQ_HZ             (PAN3029_RF_BASE_FREQ_HZ + ((unsigned long)PAN3029_RF_CHANNEL * PAN3029_RF_CHANNEL_STEP_HZ))

#define PAN3029_RF_SYNC_WORD           0x12
#define PAN3029_RF_BW_125K             7
#define PAN3029_RF_SF9                 0x09
#define PAN3029_RF_CR_4_5              0x01
#define PAN3029_RF_CRC_OFF             0x00
#define PAN3029_RF_LDR_OFF             0x00
#define PAN3029_RF_LDR_ON              0x01
#define PAN3029_RF_LDR_MODE            PAN3029_RF_LDR_ON
#define PAN3029_RF_IQ_NORMAL           0x02
#define PAN3029_RF_MODE1               0x04
#define PAN3029_RF_MODEM_OPT_OFF       0x00
#define PAN3029_RF_CONTINUOUS_RX_ENABLE 1
#define PAN3029_RF_TX_POWER_LEVEL       22
#define PAN3029_RF_TX_POWER_RAMP        0x15
#define PAN3029_RF_TX_POWER_LDO         0x50
#define PAN3029_RF_TX_POWER_PABIAS      0x70
#define PAN3029_RF_TX_POWER_PABIAS_FALLBACK 8

#define PAN3029_RF_PREAMBLE_SHORT_L    0x08
#define PAN3029_RF_PREAMBLE_SHORT_H    0x00
#define PAN3029_RF_PREAMBLE_LONG_L     0x1C
#define PAN3029_RF_PREAMBLE_LONG_H     0x04
#define PAN3029_RF_PREAMBLE_RX_L       0xF0
#define PAN3029_RF_PREAMBLE_RX_H       0x03

#define PAN3029_TX_SYMBOL_US           4096UL
#define PAN3029_TX_WAIT_GUARD_MS       900UL
#define PAN3029_TX_WAIT_LEN_STEP_MS    12UL
#define PAN3029_TX_WAIT_MIN_MS         300UL
#define PAN3029_TX_WAIT_MAX_MS         6500UL

#define PAN3029_WOR_CAD_THRESHOLD          0x10
#define PAN3029_WOR_CAD_THRESHOLD_DEFAULT  0x0A
#define PAN3029_WOR_CAD_SYMBOLS            0x02
#define PAN3029_WOR_CAD_DONE_ACTIVE        0xFE
#define PAN3029_WOR_CAD_DONE_DEFAULT       0xF4

void PAN3029_Init(void);
void PAN3029_RX(void);
void PAN3029_Sleep(void);
void PAN3029_CADInit(void);
unsigned char PAN3029_ReceivePacket(unsigned char* RxBuffer);
void PAN3029_SendPacket(unsigned char *TxBuffer, unsigned char Len);
void PAN3029_Reset(void);

extern unsigned char PAN3029SpiOk;
extern unsigned char PAN3029SpiCheckReg;
extern unsigned char PAN3029SpiCheckError;
extern unsigned char PAN3029LastIrq;
extern unsigned char PAN3029TxDone;
extern unsigned char PAN3029TxUseLongPreamble;
extern unsigned char PAN3029TxLen;
extern unsigned int  PAN3029TxWaitMs;
extern unsigned int  PAN3029TxElapsedMs;
extern unsigned int  PAN3029TxTimeoutCount;
extern unsigned char PAN3029CalMarker;
extern unsigned char PAN3029CalPaBias;
extern unsigned char PAN3029PaBiasRaw;
extern unsigned char PAN3029PaBiasApplied;
extern unsigned char PAN3029RxLen;
extern unsigned char PAN3029RxError;
extern unsigned char PAN3029State;
extern unsigned char PAN3029CurrentChannel;

#endif /* PAN3029_H_ */
