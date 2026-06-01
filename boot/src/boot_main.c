/*
 * boot_main.c
 *
 *  Created on: 2021年8月19日
 *      Author: Halley
 */

#include "hal_data.h"
//#include "sys.h"

#define UPGRADE_PARA_OFFSET     0x3000
#define UPGRADE_DATA_OFFSET     0x3080

#define BLOCK_SIZE              2048    //2K
#define BLOCK_ADDRESS(x)        ((0) + BLOCK_SIZE * (uint32_t)(x))
//
#define  USER_ADDR     0x4000
unsigned char boot_AT24_readbyte(unsigned int Addr, unsigned char* buff, unsigned int num);
extern unsigned char FlashDataPro(void);
extern void boot_Delay_Ms(unsigned int tt);
uint32_t TotalNum,dataaddr;
uint8_t  StatusBuf[15];
extern unsigned char boot_AT24_writebyte(unsigned int Addr, unsigned char *buff, unsigned int num);
struct arm_vector_table
{
    uint32_t msp;
    uint32_t reset;
};

typedef void (*func_ptr_t)(void);
uint32_t JumpAddress;
func_ptr_t JumpToApplication;

void boot_main(void);

static void IAP_JumpToApp(uintptr_t addr)
{
    static struct arm_vector_table *vt;
    uintptr_t flash_base;

    __disable_irq();
    flash_base = addr;

    vt = (struct arm_vector_table *)(flash_base);

    vt->msp = *(uint32_t *)flash_base;
    vt->reset = *(uint32_t *)(flash_base+4);

    __set_MSPLIM(0);

    SCB->VTOR = ((int)(&(vt->msp)) & 0x1FFFFF80);
    __DSB();

    R_MPU_SPMON->SP[0].CTL = 0;
    while(R_MPU_SPMON->SP[0].CTL != 0);

    __set_MSP(vt->msp);
    ((void (*)()) vt->reset)();
}
//unsigned char buf1[130];
//unsigned int addr = UPGRADE_DATA_OFFSET;
void boot_main(void)
{
  unsigned char blk,check;
  unsigned long i = 5;
  fsp_err_t err = FSP_SUCCESS;
  flash_result_t blank_check_result;
  unsigned char re;
/* Disable MSP monitoring  */
    //R_MPU_SPMON->SP[0].CTL = 0;
    //while(R_MPU_SPMON->SP[0].CTL != 0);
      
       // boot_AT24_readbyte(dataaddr,databuf,2);
       //   boot_AT24_readbyte(UPGRADE_PARA_OFFSET, StatusBuf, 11);
    
  re = 0x00;
  __disable_irq();    //boot 不用中断方式
//=========================================================port init
  err =  R_IOPORT_Open(&g_ioport_ctrl, &g_bsp_pin_cfg);    //启用端口配置
  while(FSP_SUCCESS != err);

  boot_Delay_Ms(500);
//    IAP_JumpToApp(USER_ADDR);
//    for(i=0;i<357;i++)
//    {
//        boot_AT24_readbyte(addr, buf1, 136);
//        boot_Delay_Ms(3);
//        if(buf1[0] != 0x80 || buf1[6] != 0x80)
//        {
//            boot_Delay_Ms(1);
//        }
//        addr += 136;
//    }
////==========================================================
////    err = R_AGT_Open(&g_timer0_ctrl, &g_timer0_cfg);     //timer--500ms
////    while(FSP_SUCCESS != err);
////
////    err = R_AGT_Start(&g_timer0_ctrl);                   //start
////    while(FSP_SUCCESS != err);
////=========================================================read 24，get info
  boot_AT24_readbyte(UPGRADE_PARA_OFFSET, StatusBuf, 11);  //读出升级参数,判断是否需要升级
  boot_Delay_Ms(10);
#if 1
  if(StatusBuf[0] != 'Y' || StatusBuf[1] != 0xAA)     //没有升级数据
  {
    IAP_JumpToApp(USER_ADDR);
  }

  check = 0;

  for(i=0;i<10;i++)          //判断信息区校验
  {
    check += StatusBuf[i];
  }

  if(check != StatusBuf[10])   //参数校验错误，退出至主程序执行
  {
    IAP_JumpToApp(USER_ADDR);
  }
#endif
  TotalNum = (unsigned int)StatusBuf[4] * 256 + StatusBuf[5];   //升级数据帧的总帧数
  dataaddr = UPGRADE_DATA_OFFSET;
//=========================================================flash  init
  err = R_FLASH_LP_Open(&g_flash0_ctrl, &g_flash0_cfg);
  while(FSP_SUCCESS != err);

  for(blk=8;blk<64;blk++)    //保留16K，擦除剩余block
  {
    err = R_FLASH_LP_Erase(&g_flash0_ctrl, BLOCK_ADDRESS(blk), 1);
    while(FSP_SUCCESS != err);

    err = R_FLASH_LP_BlankCheck(&g_flash0_ctrl, BLOCK_ADDRESS(blk), BLOCK_SIZE, &blank_check_result);
    while(FSP_SUCCESS != err);
  }

  while(1)
  {
    re = FlashDataPro();
    //re = 1;               //不再判断写入是否升级成功，避免失败后不做标志，程序还是无法下载
    if(1)//(re)
    {
      StatusBuf[0] = 'N';       //升级完成，更新状态
      StatusBuf[1] = 0x55;
      boot_AT24_writebyte(UPGRADE_PARA_OFFSET, StatusBuf, 11);
      boot_Delay_Ms(100);
      break;
    }
  }

  R_FLASH_LP_Close(&g_flash0_ctrl);
  //err = R_FLASH_LP_Close(&g_flash0_ctrl);
  //while(FSP_SUCCESS != err);    //不做判断，防止死循环
  __enable_irq();

  if(re != 1)
  {
    R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_02_PIN_13, BSP_IO_LEVEL_HIGH);   //加入升级失败，继电器闭合，保证壁挂炉工作
      //RELAY_ON;
  }
  else
  {   //两种方式均可采用
      //IAP_JumpToApp(USER_ADDR); //跳转
    NVIC_SystemReset();       //升级完成后直接复位
  }
}
