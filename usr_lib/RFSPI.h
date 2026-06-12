/*
 * RFSPI.h
 *
 *  Created on: 2021年6月3日
 *      Author: WJQ
 */

#ifndef RFSPI_H_
#define RFSPI_H_


#include "bsp_api.h"
#include "r_ioport.h"
#include "r_ioport_api.h"
#include "common_data.h"

//DIO0
//#define DIO0_JUDGEIN                R_IOPORT_PinRead (&ioport_ctrl_t, BSP_IO_PORT_00_PIN_15, BSP_IO_LEVEL_HIGH) // P015
//DIO4
//#define DIO4_JUDGEIN

//////////////////////LORA PAN3029（沿用原 SX1278 引脚）////////////////////
#define RFSPI_CLK_SETDIR_OUT        R_IOPORT_PortDirectionSet(&g_ioport_ctrl, BSP_IO_PORT_02, OUTPUT, BIT8)
#define RFSPI_CLK_SETOUT_LOW        R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_02_PIN_08, BSP_IO_LEVEL_LOW)
#define RFSPI_CLK_SETOUT_HIGH       R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_02_PIN_08, BSP_IO_LEVEL_HIGH)

#define RFSPI_MOSI_SETDIR_OUT       R_IOPORT_PortDirectionSet(&g_ioport_ctrl, BSP_IO_PORT_04, OUTPUT, BIT8)
#define RFSPI_MOSI_SETOUT_LOW       R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_04_PIN_08, BSP_IO_LEVEL_LOW)
#define RFSPI_MOSI_SETOUT_HIGH      R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_04_PIN_08, BSP_IO_LEVEL_HIGH)

//MISO
#define RFSPI_MISO_SETDIR_IN        R_IOPORT_PortDirectionSet(&g_ioport_ctrl, BSP_IO_PORT_04, INPUT, BIT7)

//CSN
#define RFSPI_NSS_SETDIR_OUT        R_IOPORT_PortDirectionSet(&g_ioport_ctrl, BSP_IO_PORT_04, OUTPUT, BIT9)
#define RFSPI_NSS_SETOUT_LOW        R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_04_PIN_09, BSP_IO_LEVEL_LOW)
#define RFSPI_NSS_SETOUT_HIGH       R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_04_PIN_09, BSP_IO_LEVEL_HIGH)

//RF reset（与原 SX1278 RST 同一引脚 P207）
#define RFSPI_RFRESET_SETDIR_OUT    R_IOPORT_PortDirectionSet(&g_ioport_ctrl, BSP_IO_PORT_02, OUTPUT, BIT7)
#define RFSPI_RFRESET_SETOUT_LOW    R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_02_PIN_07, BSP_IO_LEVEL_LOW)
#define RFSPI_RFRESET_SETOUT_HIGH   R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_02_PIN_07, BSP_IO_LEVEL_HIGH)

/* 兼容旧名（不再使用，仅保留以防外部引用） */
#define RFSPI_SXRESET_SETDIR_OUT    RFSPI_RFRESET_SETDIR_OUT
#define RFSPI_SXRESET_SETOUT_LOW    RFSPI_RFRESET_SETOUT_LOW
#define RFSPI_SXRESET_SETOUT_HIGH   RFSPI_RFRESET_SETOUT_HIGH

//20151022 AS179  RXD   SDN
#define RFTX_ON    R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_01_PIN_03, BSP_IO_LEVEL_LOW)//P12_bit.no5  = 1
#define RFRX_ON    R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_01_PIN_03, BSP_IO_LEVEL_HIGH)//P12_bit.no5  = 1

#define RF_SLEEP   //P12_bit.no5  = 0
//#define RFTX_ON    P0.7 = 1;P0.6 = 0
//#define RFRX_ON    P0.7 = 0;P0.6 = 1

//1 for write access and 0 for read access
#define SX1278_WRITE_BURST  0x80
#define SX1278_READ_SINGLE  0x7F//0x80
#define SX1278_READ_BURST   0x00

void RFSPISetup(void);
void WaitReady(void);
void TI_CC_PowerupResetCCxxxx(void);

void RFSPIWriteReg(unsigned char,unsigned char);
void RFSPIWriteBurstReg(unsigned char,unsigned char*,unsigned char);

void RFSPIReadBurstReg(unsigned char,unsigned char *,unsigned char);

unsigned char RFSPIReadReg(unsigned char);
unsigned char RFSPIReadStatus(unsigned char);

void TI_CC_SPIStrobe(char);

void RFSPIWait(unsigned int);
unsigned char RFSPI_bitbang_in(void);
void RFSPI_bitbang_out(unsigned char value);


#endif /* RFSPI_H_ */
