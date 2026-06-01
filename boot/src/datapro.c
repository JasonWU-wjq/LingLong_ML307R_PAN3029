/*
 * datapro.c
 *
 *  Created on: 2021年8月27日
 *      Author: RMT
 */

#include  "boot_24.h"
#include  "string.h"
#include  "r_flash_lp.h"

#define UPGRADE_DATA_OFFSET     0x3080
#define BLOCK_SIZE              2048    //2K
#define BLOCK_ADDRESS(x)        ((0) + BLOCK_SIZE * (uint32_t)(x))

//#define   RedLed_OFF    R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_00_PIN_10, BSP_IO_LEVEL_LOW)
//#define   RedLed_ON     R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_00_PIN_10, BSP_IO_LEVEL_HIGH)
//
//#define   GreenLed_OFF     R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_00_PIN_11, BSP_IO_LEVEL_LOW)
//#define   GreenLed_ON      R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_00_PIN_11, BSP_IO_LEVEL_HIGH)

extern uint32_t TotalNum,dataaddr;
//extern void Delay_Ms(unsigned int tt);
unsigned char FlashDataPro(void);
uint16_t  flashaddr;

unsigned char FlashDataPro(void)
{
  unsigned char check;//,blk,num;
  unsigned int i,j,len;
  unsigned char databuf[255];
  fsp_err_t  err;

  //blk = 0x08;
  //num = 0x00;

  RedLed_ON;

  for(i=0;i<TotalNum;i++)
  {
    if(i % 2)
    {
      GreenLed_OFF;
    }
    else
    {
      GreenLed_ON; 
    }
      //boot_AT24_readbyte(0x3080, databuf, 2);  //读出升级参数,判断是否需要升级
      //uint16_t t = *(uint16_t *)0x20004b0a;
        
      //boot_AT24_readbyte(dataaddr,databuf,2);
    memset(databuf,0xff,150);
    //boot_AT24_readbyte(unsigned int Addr, unsigned char* buff, unsigned int num)
    boot_AT24_readbyte(dataaddr,databuf,2);   //dbuf[0] ==80     dbuf[1]=len
    boot_Delay_Ms(3);

    if(databuf[0] != 0x80)   //数据域帧头错误
    {
      GreenLed_OFF;
      RedLed_ON;
      dataaddr = UPGRADE_DATA_OFFSET;
      return 0;
    }
    boot_AT24_readbyte(dataaddr+2,databuf+2,databuf[1]+1);
    boot_Delay_Ms(3);

    check = 0x00;

    for(j=0;j<databuf[1]+2;j++)
    {
      check ^= databuf[j];
    }

    if(check != databuf[databuf[1]+2])   //xor  check err
    {
      RedLed_OFF;
      GreenLed_ON;
      dataaddr = UPGRADE_DATA_OFFSET;
      return 0;
    }
        //flashaddr = ((uint16_t)databuf[5] << 8) | databuf[4];
    len = databuf[6];    //需要判断数据长度是否满足4的倍数，不够4，补ff

    if(databuf[6] % 4 == 0x01)  //填充3字节
    {
      databuf[len+7] = 0xff;
      databuf[len+8] = 0xff;
      databuf[len+9] = 0xff;

      len += 3;
    }
    else if(databuf[6] % 4 == 0x02)  //填充2字节
    {
      databuf[len+7] = 0xff;
      databuf[len+8] = 0xff;
      len += 2;
    }
    else if(databuf[6] % 4 == 0x03)  //填充1字节
    {
      databuf[len+7] = 0xff;
      len += 1;
    }

        //GreenLed_OFF;
        //Boot_Write_data(flashaddr,databuf[6],databuf+7);
        //R_FLASH_LP_Write(&g_flash0_ctrl, (uint32_t) g_src, BLOCK_ADDRESS(16)+256*0, 256);
//        if(i>= 340)
//        {
//            boot_Delay_Ms(3);
//        }
//        if(len < 128)
//        {
//            boot_Delay_Ms(1);
//        }

        //err = R_FLASH_LP_Write(&g_flash0_ctrl, databuf+7, BLOCK_ADDRESS(blk)+len*num, len);  //默认len=128
    err = R_FLASH_LP_Write(&g_flash0_ctrl, (uint32_t)databuf+7, ((unsigned int)databuf[5]<<8)+databuf[4], len);  //默认len=128
    while(FSP_SUCCESS != err)
    {
      RedLed_OFF;
      GreenLed_OFF;
      return 0;//boot_Delay_Ms(1);
    }
//        num ++;
//
//        if(num >= 16)    //128 * 16 = 2K
//        {
//            num = 0x00;
//            blk ++;
//        }
    dataaddr += databuf[1]+3;

    boot_Delay_Ms(3);
  }

  RedLed_ON;
  GreenLed_ON;
  boot_Delay_Ms(300);//boot_Delay_Ms(1000);
  return 1;
}
