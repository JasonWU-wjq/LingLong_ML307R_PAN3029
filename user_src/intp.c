/*
 * intp.c
 *
 *  Created on: 2021年6月3日
 *      Author: WJQ
 */

#include "../usr_lib/sys.h"

//unsigned char ReceiveFlag;

void usr_ex_irq6_callback(external_irq_callback_args_t *p_args)   //DIO0  int
{
  bsp_io_level_t  level;

  FSP_PARAMETER_NOT_USED(p_args);

  R_IOPORT_PinRead(&g_ioport_ctrl, BSP_IO_PORT_03_PIN_01, &level);

  if(level)
  {
    ReceiveFlag = 0x01;
    //ProcessRF();       //无线数据处理
    GetDataTime = 0x00;
  }
}

void usr_ex_irq3_callback(external_irq_callback_args_t *p_args)  //key int
{
  bsp_io_level_t  level;

  FSP_PARAMETER_NOT_USED(p_args);

  R_IOPORT_PinRead(&g_ioport_ctrl, BSP_IO_PORT_02_PIN_12, &level);

  if(!level)
  {
    //RedLed_ON;
    //RedLed_OFF;
    KeyPressFlag = 0x01;
    KeyPressHoldTime = 0x00;
  }
}
