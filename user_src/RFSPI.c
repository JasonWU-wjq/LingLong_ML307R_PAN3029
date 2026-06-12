/*
 * RFSPI.c
 *
 *  Created on: 2021年6月3日
 *      Author: WJQ
 *  Updated  on: 2026-05-14  PAN3029 寻址格式：(addr<<1)|0x01 写 / (addr<<1)&0xFE 读
 *
 * GPIO bit-bang SPI，沿用原 SX1278 引脚（CLK=P208, MOSI=P408,
 * MISO=P407, NSS=P409，复位=P207）。PAN3029 不使用 MISO 作 busy
 * 标志，所以读写路径不再调用 WaitReady。
 */

#include "../usr_lib/sys.h"

void RFSPIWait(unsigned int cycles)
{
  while(cycles>15)                          // 15 cycles consumed by overhead
    cycles = cycles - 6;                    // 6 cycles consumed each iteration
}

void RFSPISetup(void)
{
   RFSPI_NSS_SETOUT_HIGH;
   RFSPI_NSS_SETDIR_OUT;
   RFSPI_CLK_SETOUT_LOW;
   RFSPI_CLK_SETDIR_OUT;
   RFSPI_MISO_SETDIR_IN;
   RFSPI_RFRESET_SETOUT_HIGH;
   RFSPI_RFRESET_SETDIR_OUT;
   RFSPI_MOSI_SETOUT_HIGH;
   RFSPI_MOSI_SETDIR_OUT;
}

void RFSPI_bitbang_out(unsigned char value)
{
  unsigned char box;

  for(box=8;box>0;box--)
  {
    RFSPI_CLK_SETOUT_LOW;

    if(value & 0x80)
       RFSPI_MOSI_SETOUT_HIGH;
    else
       RFSPI_MOSI_SETOUT_LOW;
    value = (unsigned char)(value << 1);

    RFSPI_CLK_SETOUT_HIGH;
  }
  RFSPI_CLK_SETOUT_LOW;
}

unsigned char RFSPI_bitbang_in(void)
{
  unsigned char bbx=0;
  bsp_io_level_t level;
  unsigned int bby;

  for(bby=8;bby>0;bby--)
  {
    RFSPI_CLK_SETOUT_LOW;
    RFSPI_CLK_SETOUT_HIGH;

    R_IOPORT_PinRead(&g_ioport_ctrl, BSP_IO_PORT_04_PIN_07, &level);

    bbx = (unsigned char)(bbx << 1);
    bbx = (unsigned char)(bbx | level);
  }
  RFSPI_CLK_SETOUT_LOW;
  return bbx;
}

void RFSPIWriteReg(unsigned char addr, unsigned char value)
{
    RFSPI_NSS_SETOUT_LOW;
    RFSPI_bitbang_out((unsigned char)((addr << 1) | 0x01));
    RFSPI_bitbang_out(value);
    RFSPI_NSS_SETOUT_HIGH;
}

void RFSPIWriteBurstReg(unsigned char addr, unsigned char *buffer, unsigned char count)
{
   unsigned char bri;

    RFSPI_NSS_SETOUT_LOW;
    RFSPI_bitbang_out((unsigned char)((addr << 1) | 0x01));
    for (bri = 0; bri < count; bri++)
      RFSPI_bitbang_out(buffer[bri]);
    RFSPI_NSS_SETOUT_HIGH;
}

unsigned char RFSPIReadReg(unsigned char addr)
{
  unsigned char rrx;

  RFSPI_NSS_SETOUT_LOW;
  RFSPI_bitbang_out((unsigned char)((addr << 1) & 0xFE));

  RFSPI_MOSI_SETOUT_HIGH;                 // Dummy 0xff while clocking data out
  rrx = RFSPI_bitbang_in();
  RFSPI_NSS_SETOUT_HIGH;

  return rrx;
}

void RFSPIReadBurstReg(unsigned char addr, unsigned char *buffer, unsigned char count)
{
  unsigned char rbri;

  RFSPI_NSS_SETOUT_LOW;
  RFSPI_bitbang_out((unsigned char)((addr << 1) & 0xFE));
  RFSPI_MOSI_SETOUT_HIGH;
  for (rbri = 0; rbri < count; rbri++)
    buffer[rbri] = RFSPI_bitbang_in();
  RFSPI_NSS_SETOUT_HIGH;
}

//Wait ready - kept for binary compatibility, no-op for PAN3029
void WaitReady(void)
{
  ;
}
