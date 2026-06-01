/*
 * Nb_Pro.c
 *
 *  Created on: 2021å¹´6æœˆ5æ—¥
 *      Author: WJQ
 */

#include  "../usr_lib/nbpro.h"
unsigned char StrSearchTime;

unsigned char CaculateWeekDay(int y,int m,int d)
{
  unsigned char week;

  if(m==1||m==2)
  {
    m += 12;
    y --;
  }

  week = (d+2*m + 3*(m+1)/5 + y + y/4- y/100 + y/400) % 7;

  return week;
}

void UpdateCurrTime(void)
{
  int tempyear;
  unsigned int week;

  if(DateUpdateFlag)
  {
    DateUpdateFlag = 0x00;

    //tempyear = SysYear + 2000;

    switch(SysMonth)
    {
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:
      if(SysDay > 31)
      {
        SysDay = 1;
        SysMonth ++;
      }
      break;
    case 2:
      if((((SysYear + 2000) % 4 == 0) && ((SysYear + 2000) % 100 != 0)) || ((SysYear + 2000) % 400 == 0))     //é—°å¹´
      {
        if(SysDay > 29)
        {
          SysDay = 1;
          SysMonth ++;
        }
      }
      else
      {
        if(SysDay > 28)
        {
          SysDay = 1;
          SysMonth ++;
        }
      }
      break;
    case 4:
    case 6:
    case 9:
    case 11:
      if(SysDay > 30)
      {
        SysDay = 1;
        SysMonth ++;
      }
      break;
    }

    if(SysMonth > 12)
    {
      SysMonth = 1;
      SysYear ++;
    }
    tempyear = SysYear + 2000;
    SysWeek = CaculateWeekDay(tempyear,SysMonth,SysDay);
  }
#ifndef VER_07
  if(SetTimeFlag)  //1minæ›´æ–°ä¸€æ¬¡å£æŒ‚ç‚‰æ—¶é—´
  {
    SetTimeFlag = 0x00;
    week = (unsigned int)SysWeek << 13;
    QK_SetSignleRegister(StoveTime__offset,week + ((unsigned int)SysHour << 8) + SysMinute);
  }
#endif
}

void GetNetTime(unsigned char *year,unsigned char *month,unsigned char *day,unsigned char *hour,unsigned char *min,unsigned char *sec)
{
  int i,j,len;

  unsigned char buf[25];

  delayms(100);//delayms(50);   //ç•¥å»¶æ—¶ï¼Œç­‰å¾…æ•°æ®æ¥æ”¶å®Œæˆï¼Œ2022-02-11
  len = strlen((char *)USCIA0RXBUF);

  for(i=0;i<len;i++)   //ÕÒµ½µÚÒ»¸ö¡°µÄÎ»ÖÃ
  {
    if(USCIA0RXBUF[i] == '"')//if(USCIA0RXBUF[i] == 'C' && USCIA0RXBUF[i+1] == 'L' && USCIA0RXBUF[i+2] == 'K' && USCIA0RXBUF[i+3] == ':')
    {
      //memcpy(buf,USCIA0RXBUF+i+4,17);
      i += 1;     //"ºó±ß½ô¸úÊı¾İ
      break;
    }
  }

  j = i;
  
  while(USCIA0RXBUF[i] != '"')   //ÅĞ¶ÏµÚ¶ş¸ö¡±£¬Ê±¼äÔÚÁ½¸ö¡°Ö®¼ä
  {
    buf[i-j] = USCIA0RXBUF[i];
    
    if('0'<= buf[i-j] && buf[i-j] <= '9')
    {
      buf[i-j] -= '0';
    }
    
    i++;
  }
  
//  for(i=0;i<17;i++)
//  {
//    if('0'<= buf[i] && buf[i] <= '9')
//    {
//      buf[i] -= '0';
//    }
//  }
  //DateUpdateFlag = 0x01;    
  //buf[0]ºÍbuf[1] ÊÇÄêµÄ¸ßÎ»--20
  *year  = buf[0] * 10  + buf[1];
  *month = buf[3] * 10  + buf[4];
  *day   = buf[6] * 10  + buf[7];

  *hour  = buf[9] * 10  + buf[10] + 8;   //¶«°ËÇøÊ±¼äº

  if(*hour >= 24)
  {
    *hour -= 24;
    *day += 1;
  }

  *min = buf[12] * 10 + buf[13];
  *sec = buf[15] * 10 + buf[16];

  DateUpdateFlag = 0x01;
  SetTimeFlag = 0x01;
  UpdateCurrTime();
}

///////////////////////////////////////////////////////////////////
/***************************************************************************************************************************
å‡½æ•°åç§°ï¼šUINT8 FrequencyClearing(void)

è¾“å‡ºå‚æ•°ï¼šæ— 

è¿”å›å‚æ•°ï¼š

20: CFUN å…³é—­å¤±è´¥;

21: æ¸…é™¤é¢‘ç‚¹å¤±è´¥;

0 : è¡¨ç¤ºæ¸…é™¤é¢‘ç‚¹æˆåŠŸ;

1 : è¡¨ç¤ºæ‰ç”µ ;

è¯´æ˜: æ¸…é™¤NBæ¨¡å—é¢‘ç‚¹å‡½æ•°ï¼Œ

****************************************************************************************************************************/

//UINT8 FrequencyClearing(void)
//{
//  INT8 sc_re;
//
//  sc_re = 0;
//
//  USCIA0RXNum = 0;
//
//  WifiParaSend("AT+CFUN=0", 9);
//  sc_re = ReCommand();
//  if(sc_re != 1)
//    return 20;
//  else
//    return 1;
//
//  //WifiParaSend("AT+NBAND=5", 10);
//
//  sc_re = 0;
//
//  USCIA0RXNum = 0;
//
//  WifiParaSend("AT+NCSEARFCN", 12);
//  sc_re = ReCommand();
//  if(sc_re != 1)
//    return 21;
//  else
//    return 1;
//}

/***************************************************************************************************************************
å‡½æ•°åç§°ï¼šINT8 * MyStrstr(const INT8 *sc_str1, const INT8 *sc_str2 ,unsigned int ui_n)

*sc_str1 :è¢«æŸ¥æ‰¾å­—ç¬¦ä¸²

*sc_str2 :éœ€è¦æŸ¥æ‰¾çš„å­—ç¬¦ä¸²

ui_n : æŸ¥æ‰¾å­—ç¬¦ä¸²çš„æ€»é•¿åº¦

è¿”å›å‚æ•°ï¼šéNULLè¡¨ç¤ºæˆåŠŸæŸ¥æ‰¾åˆ°å­—ç¬¦ä¸²   è¿”å›å€¼ä¸ºæŸ¥æ‰¾åˆ°å­—ç¬¦ä¸²çš„é¦–å­—æ¯åœ°å€

è¯´æ˜: strstrçš„ä¿®æ”¹å‡½æ•°ï¼ŒæŸ¥æ‰¾å­—ç¬¦ä¸²æŒ‰ç…§æ€»é•¿åº¦æŸ¥è¯¢  ä¸å—æ¢è¡Œå­—ç¬¦ä¸²çš„é™åˆ¶

****************************************************************************************************************************/
/*
INT8 * MyStrstr(const INT8 *sc_str1, const INT8 *sc_str2 ,UINT32 ui_n)
{
  INT8 *sc_cp = (INT8 *)sc_str1;
  INT8 *sc_cp1, *sc_cp2;

  if (!*sc_str2)
    return((INT8 *)sc_str1);

  while(ui_n--)
  {
    sc_cp1 = sc_cp;
    sc_cp2 = (INT8 *)sc_str2;

    while(*sc_cp2 && !(*sc_cp1 - *sc_cp2))
      sc_cp1++, sc_cp2++;

    if (!*sc_cp2)
      return(sc_cp);

    sc_cp++;
  }

  return (NULL);
}
*/
/***************************************************************************************************************************
å‡½æ•°åç§°ï¼šUINT8 GetChar(INT8 *su_src,INT8 *su_identifier,INT8 *sc_msg,UINT8 uc_msg_numb)

è¾“å…¥å‚æ•°:

*su_src:  æºå­—ç¬¦ä¸²

*su_identifier: è·å–æ•°æ®çš„æ ‡è¯†ç¬¦  è·å–å­—ç¬¦ä¸²ä»æ ‡è¯†ç¬¦åå¼€å§‹è·å–

*sc_msg: è·å–çš„æ•°æ®ç¼“å­˜å˜é‡

uc_msg_numb: è·å–çš„æ•°æ®ç¼“å­˜å˜é‡é•¿åº¦

è¿”å›å‚æ•°ï¼š  0 è¡¨ç¤ºæˆåŠŸ   å…¶å®ƒè¡¨ç¤ºå¤±è´¥

è¯´æ˜: åœ¨å­—ç¬¦ä¸²ä¸­è·å–å­—ç¬¦ä¸²

****************************************************************************************************************************/
/*
UINT8 GetChar(INT8 *su_src,INT8 *su_identifier,INT8 *sc_msg,UINT8 uc_msg_numb)
{
  INT8 *sc_p;
  UINT8 uc_loopj;

  sc_p = MyStrstr(su_src,su_identifier,200);

  if(NULL != sc_p)
  {
    sc_p += strlen(su_identifier);

    for(uc_loopj = 0; uc_loopj < uc_msg_numb; uc_loopj++)
    {
      *sc_msg++ = *sc_p++;

    }
    return 0;
  }
  return 1;
}
*/
/***************************************************************************************************************************
å‡½æ•°åç§°ï¼šint MyAtoi(const INT8 *sc_nptr)

è¾“å…¥å‚æ•°:

const INT8 *sc_nptr:  éœ€è¦è½¬æ¢çš„å­—ç¬¦ä¸²

è¿”å›å‚æ•°ï¼š  0 è¡¨ç¤ºæˆåŠŸ   å…¶å®ƒè¡¨ç¤ºå¤±è´¥

è¯´æ˜: æ­¤å‡½æ•°æ˜¯åœ¨æ ‡å‡†Cå‡½æ•°ï¼šint atoi(const INT8 *nptr); (è¡¨ç¤º ascii to integer) çš„åŸºç¡€ä¸Šä¿®æ”¹è€Œæ¥

****************************************************************************************************************************/
/*
INT32 MyAtoi(const INT8 *sc_nptr)
{
  INT32 si_cp;
  INT32 si_total;
  INT32 si_sign;

  si_cp = *sc_nptr++;
  si_sign = si_cp;
  if(si_cp == '-' || si_cp == '+')
    si_cp = *sc_nptr++;

  si_total = 0;

  while (si_cp != 0x00 && si_cp != ' ' && si_cp != '\r') //å¦‚æœé‡åˆ°0x00 ã€â€˜ â€™ã€â€˜\râ€™ æ•°æ®æ—¶åˆ¤æ–­ä¸ºæ•°æ®ç»“æŸï¼Œè·³å‡ºå‡½æ•°
  {
    si_total = 10 * si_total + (si_cp - '0');
    si_cp = *sc_nptr++;
    if(si_cp < '0' || si_cp > '9')  //  é˜²é”™  ç¡®ä¿æ•°æ®æ—¶æ•°å­—å­—ç¬¦ä¸²
      break;
  }

  if (si_sign == '-')
    return -si_total;
  else
    return si_total;
}
*/
/***************************************************************************************************************************
å‡½æ•°åç§°ï¼šUINT8 GetNetPara(UINT8 *netbuf,UINT8 mode)

è¾“è¾“å…¥å‚æ•°ï¼š

UINT8 *uc_net_para

UINT8 uc_mode

è¿”å›å‚æ•°ï¼šè¿”å›0è¡¨ç¤ºè·å–å‚æ•°æˆåŠŸ

è¯´æ˜: è·å–ç½‘ç»œå‚æ•°ï¼Œä»¥ä¸‹ç½‘ç»œå‚æ•°

Signal powerã€Total powerã€Cell IDã€ECLã€SNRã€EARFCNã€PCI

****************************************************************************************************************************/
/*
UINT8 GetNetPara(UINT8 *uc_net_para,UINT8 uc_mode)
{
  INT32 si_temp_data;
  INT16 ui_temp_num;
  INT8 sc_buf[20];

//  è·å– Signal power
  memset(sc_buf,0x00,20);
//  if(1 == ui_NBIoTModeFlag)  //Signal power:
//  {
//    GetChar((INT8 *)uc_net_para,"Signal power: ",sc_buf,4);
//  }
//  else
//  {
//    GetChar((INT8 *)uc_net_para,"Signal power:",sc_buf,4);
//  }
  GetChar((INT8 *)uc_net_para,"Signal power:",sc_buf,4);
  si_temp_data = MyAtoi(sc_buf);
  RSRP[0] =  (UINT8)(si_temp_data & 0xFF);
  RSRP[1] =  (UINT8)((si_temp_data>>8)&0xFF);

//  è·å– Total power
  memset(sc_buf,0x00,10);

//  if(1 == ui_NBIoTModeFlag)   //Total power:
//  {
//      GetChar((INT8 *)uc_net_para,"Total power: ",sc_buf,4);
//  }
//  else
//  {
//      GetChar((INT8 *)uc_net_para,"Total power:",sc_buf,4);
//  }

  GetChar((INT8 *)uc_net_para,"Total power:",sc_buf,4);

  ui_temp_num = MyAtoi(sc_buf);
  RSSI[0] =  (UINT8)(ui_temp_num & 0xFF);
  RSSI[1] =  (UINT8)((ui_temp_num>>8)&0xFF);

//  è·å– Cell ID
  memset(sc_buf,0x00,10);
//  if(1 == ui_NBIoTModeFlag)
//  {
//      GetChar((INT8 *)uc_net_para,"Cell ID:\", ",sc_buf,10);//æœªéªŒè¯
//  }
//  else
//  {
//    GetChar((INT8 *)uc_net_para,"Cell ID:",sc_buf,10);
//  }

  GetChar((INT8 *)uc_net_para,"Cell ID:",sc_buf,10);

  si_temp_data = MyAtoi(sc_buf);
  CELL_ID[3] =  (UINT8)((si_temp_data>>24) & 0Xff);
  CELL_ID[2] =  (UINT8)((si_temp_data>>16) & 0Xff);
  CELL_ID[1] =  (UINT8)((si_temp_data>>8) & 0Xff);
  CELL_ID[0] =  (UINT8)(si_temp_data & 0Xff);

//  è·å– ECL
    memset(sc_buf,0x00,10);

//  if(1 == ui_NBIoTModeFlag)
//  {
//      GetChar((INT8 *)uc_net_para,"ECL: ",sc_buf,10);
//  }
//  else
//  {
//      GetChar((INT8 *)uc_net_para,"ECL:",sc_buf,4);
//  }

  GetChar((INT8 *)uc_net_para,"ECL:",sc_buf,4);

  si_temp_data = MyAtoi(sc_buf);
  ECL =  (UINT8)(si_temp_data & 0xFF);

//  è·å– SNR
  memset(sc_buf,0x00,10);

//  if(1 == ui_NBIoTModeFlag)   //SNR:
//  {
//    GetChar((INT8 *)uc_net_para,"SNR:",sc_buf,4);
//  }
//  else
//  {
//      GetChar((INT8 *)uc_net_para,"SNR:",sc_buf,4);
//  }

  GetChar((INT8 *)uc_net_para,"SNR:",sc_buf,4);

  si_temp_data = MyAtoi(sc_buf);
//  if(si_temp_data < 0)
//  {
//   ui_temp_num = 0 - (int)si_temp_data;
//  }
//  else
//  {
//   ui_temp_num = (int)si_temp_data;
//  }

  SNR[0] =  (UINT8)(ui_temp_num & 0xFF);
  SNR[1] =  (UINT8)((ui_temp_num>>8)&0xFF);

//  è·å– EARFCN
  memset(sc_buf,0x00,10);
//  if(1 == ui_NBIoTModeFlag)
//  {
//      GetChar((INT8 *)uc_net_para,"EARFCN: ",sc_buf,6);
//  }
//  else
//  {
//      GetChar((INT8 *)uc_net_para,"EARFCN:",sc_buf,6);
//  }
  GetChar((INT8 *)uc_net_para,"EARFCN:",sc_buf,6);
  si_temp_data = MyAtoi(sc_buf);
  EARFCN[0] =  (UINT8)(si_temp_data & 0xFF);
  EARFCN[1] =  (UINT8)((si_temp_data>>8)&0xFF);

//  è·å– PCI
  memset(sc_buf,0x00,10);
//  if(1 == ui_NBIoTModeFlag)
//  {
//      GetChar((INT8 *)uc_net_para,"PCI: ",sc_buf,6);
//  }
//  else
//  {
//      GetChar((INT8 *)uc_net_para,"PCI:",sc_buf,6);
//  }
  GetChar((INT8 *)uc_net_para,"PCI:",sc_buf,6);
  si_temp_data = MyAtoi(sc_buf);
  PCI[0] =  (UINT8)(si_temp_data & 0xFF);
  PCI[1] =  (UINT8)((si_temp_data>>8)&0xFF);

  return 0;
}
*/
////////////////////////////////////////////////////////////////////

void WifiParaSend(unsigned char *buf,int len)
{
  unsigned char sbuf[3];

  sbuf[0] = '\r';
  sbuf[1] = '\n';

  USCIA0RXNum = 0;
  memset((char *)USCIA0RXBUF,0,UARTPOINTNUM);
  CmdRunTime = 0x00;
  UART_RxNum = USCIA0RXNum;
  g_uart9_send_complete = false;
  R_SCI_UART_Write(&g_uart9_ctrl, buf, len);
  while(!g_uart9_send_complete)
  {
    if(CmdRunTime > 2)
      return;
  }
  R_SCI_UART_Write(&g_uart9_ctrl, sbuf, 1);//R_SCI_UART_Write(&g_uart9_ctrl, sbuf, 2);
}

void HexToString(unsigned char *DataBuffer,unsigned int DataLenl)
{
  unsigned char TempHexbuf[UARTPOINTNUM],chh1,chh2;
  unsigned int DataLoopi;

  if(DataLenl <= UARTPOINTNUM)
  {
    memcpy(TempHexbuf,DataBuffer,DataLenl);

    for(DataLoopi=0;DataLoopi<DataLenl;DataLoopi++)
    {
      chh1 = (TempHexbuf[DataLoopi] & 0xF0) >> 4;
      chh2 = TempHexbuf[DataLoopi] & 0x0F;

      if(chh1 > 9)
      {
        DataBuffer[DataLoopi*2] = chh1 - 0x0A + 'A';
      }
      else
      {
        DataBuffer[DataLoopi*2] = chh1 + '0';
      }

      if(chh2 > 9)
      {
        DataBuffer[DataLoopi*2+1] = chh2 - 0x0A + 'A';
      }
      else
      {
        DataBuffer[DataLoopi*2+1] = chh2 + '0';
      }
    }
  }
}

void StringToHex(unsigned char *DataBuffer,unsigned int DataLenl)
{
  unsigned char TempHexbuf[UARTPOINTNUM],chh1,chh2;
  unsigned int  DataLoopi;

  if(DataLenl <= UARTPOINTNUM)
  {
    for(DataLoopi=0;DataLoopi<DataLenl;DataLoopi++)
    {
      chh1 = DataBuffer[DataLoopi*2];
      chh2 = DataBuffer[DataLoopi*2+1];

      if((chh1 >= '0')&&(chh1 <= '9'))
      {
       chh1 -= '0';
      }
      else if((chh1 >= 'A')&&(chh1 <= 'F'))
      {
       chh1 = (chh1 - 'A')+10;
      }
      else if((chh1 >= 'a')&&(chh1 <= 'f'))
      {
       chh1 = (chh1 - 'a')+10;
      }

      if((chh2 >= '0')&&(chh2 <= '9'))
      {
       chh2 -= '0';
      }
      else if((chh2 >= 'A')&&(chh2 <= 'F'))
      {
       chh2 = (chh2 - 'A')+10;
      }
      else if((chh2 >= 'a')&&(chh2 <= 'f'))
      {
       chh2 = (chh2 - 'a')+10;
      }

      TempHexbuf[DataLoopi] = (chh1<<4) + chh2;
    }

    memcpy(DataBuffer,TempHexbuf,DataLoopi);
  }
}

void Nb_Reset(void)  //Æô¶¯UMA603 Module
{
  NB_MODULE_ENABLE;  //µçÔ´Ê¹ÄÜ
  NB_RST_HIGH;
  delayms(10);
  NB_RST_LOW;       //ONOFF
  delayms(800);     //µÍµçÆ½³¬¹ı500ms¿ª»ú
  NB_RST_HIGH;
  delayms(3000);   //µÈ´ı2sÒÔÉÏ
}

void Nb_ModuleClose(void)
{
//  WifiParaSend("AT+ECPOWD=1",11);    // è‡ªåŠ¨æ‰§è¡Œcfun=0ï¼Œ ç„¶åå…³æœº
//  delayms(2000);
//
//  NB_MODULE_DISABLE;     //ç”µæºæ§åˆ¶
//  delayms(15);
//  NB_RST_HIGH;           //ON/OFF
//  WifiParaSend("AT+IPSWTMD=1",12);   //ÇĞ»»µ½Ö±´«Ä£Ê½
  
//  WifiParaSend("AT+IPCLOSE=1",12);
//  delayms(500);
  NB_RST_LOW;
  delayms(1200);
  NB_RST_HIGH;
  delayms(100);
  NB_MODULE_DISABLE;
  delayms(15);
}

unsigned int ReadRevData(unsigned char* buffer, int limit)
{
  unsigned int num;

  num = 0;

  CmdRunTime = 0;

  while (1)
  {
#if 0
    if(ReceiveFlag)    //æœ‰æ— çº¿å‘½ä»¤ï¼Œé€€å‡ºæ‰§è¡Œ
    {
	    ProcessRF();  //break;
    }
#endif
    if(UART_RxNum != USCIA0RXNum)
    {
      buffer[num++] = USCIA0RXBUF[UART_RxNum++];
    }

    if(UART_RxNum >= UARTPOINTNUM)
      UART_RxNum = 0;

    if(num >= limit)
    {
      break;
    }

    if(CmdRunTime >= UartDelayTime)
    {
      break;
    }

    if(buffer[num-1] == 0x0a && num > 1)
    {
      num --;
      break;
    }
  }

  buffer[num] = '\0';

  return num;
}

unsigned char ReCommand(void)
{
  char re,*re1;

  re1 = NULL;

  delayms(100);
  StrSearchTime = 0;
  
  while (1)
  {
    //re = ReadRevData(TxBuffer, 100);//200
    re = ReadRevData(TxBuffer, 100);
    if(re == 0)
      return 0;

    re1 = strstr((char *)TxBuffer,"SUCCESS");    //send ok
    if (re1 != NULL)
      return 7;

    re1 = strstr((char *)TxBuffer,"CEREG: 0,5");    //æ¼«æ¸¸æ³¨å†ŒæˆåŠŸ
    if (re1 != NULL)
      return 6;
    re1 = strstr((char *)TxBuffer,"CEREG: 0,1");    //æœ¬åœ°æ³¨å†ŒæˆåŠŸ
    if (re1 != NULL)
      return 6;
        
    re1 = strstr((char *)TxBuffer,"CGATT: 1");    //active
    if (re1 != NULL)
      return 5;
//
//    re1 = strstr((char *)inbuffer,"NSONMI");     //data send ok
//    if (re1 != NULL)
//      return 4;

    re1 = strstr((char *)TxBuffer,"CONNECT");//strstr((char *)TxBuffer,"CONNECTED");
    if(re1 != NULL)
      return 3;

    re1 = strstr((char *)TxBuffer,"ERROR");
    if(re1 != NULL)
      return 2;

    re1 = strstr((char *)TxBuffer,"OK");
    if(re1 != NULL)
      return 1;
    
    if(StrSearchTime >= 20)
    {
      StrSearchTime = 0;
      return 0;
    }
  }
//  return 0;
}

unsigned char CheckModule(void)
{
  int i,len,re;

  while (1)
  {
    re = 0;
    WifiParaSend("AT", 2);
    //R_SCI_UART_Write(&g_uart9_ctrl, "AT\r", 3);
    re = ReadRevData(TxBuffer, 200);

    if(re == 0)
      break;

    len = strlen((char *)TxBuffer);

    for(i=0;i<len;i++)
    {
      if(TxBuffer[i] == 'O' && TxBuffer[i+1] == 'K')
      {
        return 1;
      }
    }

    TxBuffer[0] = '\0';
  }

  return 0;
}

unsigned char GetImeiForMac(void)
{
  unsigned int i,j,re;

  re = 0;

  USCIA0RXNum = 0;

  WifiParaSend("AT+CGSN=1", 9);    //Request Product Serial Number   IMEI
  re = ReCommand();
  if(re != 1)
    return 0;

  j = 0;
  
  for(i=0;i<UARTPOINTNUM;i++)      //:"866774061072214"
  {
    if(TxBuffer[i] >= '0' && TxBuffer[i] <= '9' )//if(TxBuffer[i] == '"')
    {
      j = i;
      break;
    }
  }

  //i = j;

  memset(Imei,0,20);   //æ¸…é™¤imeiç¼“å†²åŒºï¼Œæœ€å¤?0

  while(TxBuffer[j] != 0x0d && j < i+15)//UARTPOINTNUM)   //imeiå·é•¿åº¦ä¸º15ï¼Œæ‹·è´æ•°æ®åˆ°imei
  {
    Imei[j-i] = TxBuffer[j] - '0';    //- 0x30
    j ++;
  }

  j = 0;

  for(i=0;i<16;i+=2)    //é‡ç»„æ•°æ®ï¼Œå¾—åˆ°æœ€ç»ˆimei
  {
    Imei[j++] = (Imei[i] << 4) + Imei[i+1];
  }

  return 1;
}

unsigned char GetIccid(void)
{
  unsigned int i,j,re;

  re = 0;

  USCIA0RXNum = 0;

  WifiParaSend("AT+ECICCID", 10);    //Request Product Serial Number
  re = ReCommand();
  if(re != 1)
    return 0;

  j = 0;

  for(i=0;i<UARTPOINTNUM;i++)
  {
    if(TxBuffer[i] >= '0' && TxBuffer[i] <= '9')
    {
      j = i;
      break;
    }
  }

  //i = j;

  memset(Iccid,0,30);

  while(TxBuffer[j] != 0x0d && j < i+30)//UARTPOINTNUM)
  {
    Iccid[j-i] = TxBuffer[j];
    j ++;
  }

  return 1;
}

unsigned char Nb_Connect(void)  //re=1:connect
{
  unsigned int i,len;//j
  unsigned char re;
  //unsigned char sbuf[50];
  
  //memset(sbuf,0,50);

  //NB_MODULE_DISABLE;
  //delayms(200);

  //Nb_ModuleClose();     //Æô¶¯ÍøÂçÇ°ÏÈ¹Ø±ÕÄ£×é¼°socket
  UartDelayTime = 30;

  re = 0;

  Nb_Reset();               //reset module first
  
#if 0
  if(ReceiveFlag)
  {
    ProcessRF();
    //return 0;
  }
#endif
  
  USCIA0RXNum = 0;
  re = CheckModule();   //ÅĞ¶ÏATÃüÁîÊÇ·ñÕı³£
  //re = CheckModule();
  if(re != 1)
  {
    re = CheckModule();
  }

  if(!re)
  {
    Nb_ModuleClose();
    return 2; //return 0;
  }
  
  re = 0;
  USCIA0RXNum = 0;   //¹Ø±Õ»ØÏÔ
  WifiParaSend((unsigned char *)"ATE0", 4);  
  re = ReCommand();
  if(re != 1)
  {
    return 0;
  }

//  re = 0;
//  USCIA0RXNum = 0;   //Ê¹ÄÜLEDÖ¸Ê¾
//  WifiParaSend((unsigned char *)"AT+MLED=0,1", 11);  
//  re = ReCommand();
//  if(re != 1)
//  {
//    return 0;
//  }
  
  re = 0;
  USCIA0RXNum = 0;
  WifiParaSend((unsigned char *)"AT+CGMI", 7);   //³§¼Ò±êÊ¶--UNIONMAN
  re = ReCommand();
  if(re != 1)
  {
    return 3;//return 0;
  }
      
  if(!GetImeiForMac())     //»ñÈ¡imei
  {
    Nb_ModuleClose();
    return 4;//return 0;
  }

  re = 0;
  USCIA0RXNum = 0;
  memset(TxBuffer,0,UARTPOINTNUM);
  sprintf((char *)TxBuffer,"AT+CGDCONT=1,\"IP\",\"cmnet\"");         
  //sprintf(TxBuffer,"AT+CGDCONT=1,\"IP\",\"psm0.eDRXC.ctnb\"");
  WifiParaSend(TxBuffer, strlen((char *)TxBuffer));
  re = ReCommand();
  if(re != 1)
    return 9;//return 0;

  re = 0;
  USCIA0RXNum = 0;
  //UartDelayTime = 20;
  WifiParaSend((unsigned char *)"AT+CFUN=1", 9);    //Full functionality
  re = ReCommand();
  if(re != 1)
  {
    Nb_ModuleClose();
    return 6;//return 0;
  }
  delayms(500);
  
#if 0
  if(ReceiveFlag)
  {
    ProcessRF();
      //return 0;
  }
#endif
  ///////////////////////////////////////////////
  re = 0;
  USCIA0RXNum = 0;

  WifiParaSend((unsigned char *)"AT+CIMI", 7);    //Request International Mobile Subscriber Identity,
  re = ReCommand();
  if(re != 1)
    return 7;//return 0;

  re = 0;
  USCIA0RXNum = 0;

  if(!GetIccid())     //Iccid
  {
    Nb_ModuleClose();
    return 8;//return 0;
  }
    
  for(i=0;i<50;i++)   //ÅĞ¶ÏÍøÂç×¢²á×´Ì¬
  {
    re = 0;
    USCIA0RXNum = 0;
    WifiParaSend("AT+CEREG?",9);
    re = ReCommand();
    if(re == 6)
    {
      break;
    }
    delayms(500);
  }
  if(re != 6)
  {
    return 0;
  }
  for(i=0;i<50;i++)   //¼¤»îÍøÂç
  {
    re = 0;
    USCIA0RXNum = 0;
    WifiParaSend("AT+CGATT?",9);
    re = ReCommand();
    if(re == 5)
    {
      break;
    }
    delayms(500);
  }
  if(re != 5)
  {
    return 0;
  }

  //delayms(500);

  re = 0;
  USCIA0RXNum = 0;

  WifiParaSend((unsigned char *)"AT+CSQ", 6);   //»ñÈ¡ĞÅºÅÇ¿¶È
  re = ReCommand();
  if(re != 1)
    return 14;//return 0;

  for(i=0;i<50;i++)
  {
    if(TxBuffer[i] == ':' && TxBuffer[i+1] == ' ')
    {
      TxBuffer[i+2] -= 0x30;
      TxBuffer[i+3] -= 0x30;

      Csq = TxBuffer[i+2] * 10 + TxBuffer[i+3];
      break;
    }
  }

//  re = 0;
//  USCIA0RXNum = 0;
//
//  WifiParaSend((unsigned char *)"AT+CCLK?", 8);
//  re = ReCommand();
//  if(re != 1)
//    return 0;
//
//  GetNetTime(&SysYear,&SysMonth,&SysDay,&SysHour,&SysMinute,&SysSecond);
 
#if 1
  //Ê¹ÓÃ AT+IPOPEN ÃüÁîÇ°Ê¹ÓÃ¸ÃÃüÁîÅäÖÃ¼´¿É
  re = 0;
  USCIA0RXNum = 0;
  memset(TxBuffer,0,UARTPOINTNUM);
  sprintf((char*)TxBuffer,"AT+IPCFG=\"tcp/keepalive\",1,180,25,3");  //³¤Á´½Ó
  len = strlen((char*)TxBuffer);
  WifiParaSend(TxBuffer, len);
  re = ReCommand(); 
  if(re != 1)
    return 0;
#endif

  re = 0;
  USCIA0RXNum = 0;
  memset(TxBuffer,0,UARTPOINTNUM);
  //sprintf((char*)TxBuffer,"AT+IPOPEN=1,\"TCP\",\"xnyc.3322.org\",7878,0,0");
  sprintf((char*)TxBuffer,"AT+IPOPEN=1,\"TCP\",\"%s\",%d,0,0",WifiDns,WifiPort);      //open  socket
  //sprintf((char*)TxBuffer,"AT+IPOPEN=1,\"TCP\",\"106.12.254.55\",30110,0,0");
  len = strlen((char*)TxBuffer);
  WifiParaSend(TxBuffer, len);
  re = ReCommand(); 
  if(re != 1)
    return 15;
//  delayms(1000);
/*   ML307R Í¸´«Á½ÖÖ·½Ê½
Ö±½Ó½¨Á¢Í¸´«Ä£Ê½Á¬½Ó£º
AT+MIPOPEN=0,"TCP","www.6gforce.com",2012,,1 //ÒÔÍ¸´«Ä£Ê½½¨Á¢Ò»Â·TCPÁ¬½Ó
OK
CONNECT //½øÈëÍ¸´«Ä£Ê½
+++ //ÍË³öÍ¸´«Ä£Ê½
OK
ĞŞ¸ÄÁ¬½ÓÄ£Ê½£º
AT+MIPOPEN=0,"TCP","www.6gforce.com",2012,,0 //ÒÔÆÕÍ¨Ä£Ê½½¨Á¢Ò»Â·TCPÁ¬½Ó
OK
+MIPOPEN: 0,0 //½¨Á¢Á¬½Ó³É¹¦
AT+MIPMODE=0,1 //connect_idÎª0µÄÁ¬½ÓÄ£Ê½ĞŞ¸ÄÎªÍ¸´«Ä£Ê½
OK
CONNECT //½øÈëÍ¸´«Ä£Ê½
+++ //ÍË³öÍ¸´«Ä£Ê½
OK
*/
  re = 0;
  USCIA0RXNum = 0;
#ifdef TRANS_MODE
  WifiParaSend((unsigned char *)"AT+IPSWTMD=1,2", 14);   //ÇĞ»»µ½Í¸´«Ä£Ê½
#else
  WifiParaSend((unsigned char *)"AT+IPSWTMD=1,1", 14);    //Ä¬ÈÏÖ±´«Ä£Ê½
#endif
  re = ReCommand();
  if(re != 3)    
    return 0;
      
  return 1;
}

unsigned char SendNbData(unsigned char *buf,unsigned int len)
{
#ifndef TRANS_MODE
  int slen;
  unsigned char re = 0;
  char sbuf[UARTPOINTNUM];
#endif
//  EI;
//  memset(sbuf,0,UARTPOINTNUM);
//  //g_uart9_recv_complete = false;
//  HexToString(buf,len);
//
//  sprintf(sbuf,"AT+NMGS=%d,",len);
//
//  memcpy(sbuf+strlen(sbuf),buf,len*2);
//  slen = strlen(sbuf);
//  WifiParaSend((unsigned char*)sbuf,slen);
//  //delayms(200);
//  re = ReCommand();
//
//  if(re != 1)
//    return 0;
#ifdef TRANS_MODE    //Í¸´«Ä£Ê½Ö±½Ó·¢ËÍ
  USCIA0RXNum = 0;
  R_SCI_UART_Write(&g_uart9_ctrl, buf, len);
  delayms(50);   //´®¿Ú·¢ËÍÊı¾İÑÓ³ÙÒ»ÏÂ¡£È·±£³¤Êı¾İ°ü¿ÉÒÔÍêÈ«·¢ËÍ
#else
  memset(sbuf,0,UARTPOINTNUM);
  HexToString(buf,len);
  sprintf(sbuf,"AT+IPSENDEX=1,\"%s\"",buf);
  //memcpy(sbuf+14,buf,len);
  slen = strlen(sbuf);
  WifiParaSend((unsigned char*)sbuf,slen);
  re = ReCommand();
  if(re != 7)
    return 0;
#endif

  return 1;//len;
}

#if 0
unsigned char ReadNbData(unsigned char *buf,unsigned int len)
{
  unsigned int i,re,index;

  unsigned char sbuf[30];

  re = 0;

  USCIA0RXNum = 0;
  memset(USCIA0RXBUF,0,UARTPOINTNUM);
  memset((char*)sbuf,30,0);
  sprintf((char *)sbuf,(char*)"AT+NSORF=0,%d",len);
  WifiParaSend(sbuf,strlen((char *)sbuf));
  re = ReCommand();
  if(re != 1)
    return 0;

  for(i=0;i<UARTPOINTNUM;i++)
  {
    if(USCIA0RXBUF[i] == 'O' && USCIA0RXBUF[i+1] == 'K')
    {
      index = i - 6;
      break;
    }
  }

  memcpy(buf,USCIA0RXBUF+(index-len*2),len*2);

  StringToHex(buf,len);

  return 1;
}
#endif

unsigned char GprsConnect(void)
{
  unsigned char i,re;
       
  for(i = 0;i < 3; i++)
  {
    ConnectFlag = NO_CONNECT;
    GreenLed_OFF;
    
    re = Nb_Connect();
    
    if(re == 1)       //ÍøÂçÁ¬½Ó³É¹¦
    {
      ConnectFlag = CONNECK_OK;            //ÁªÍø³É¹¦¿ªÊ¼·¢ËÍ×¢²áÃüÁî
      HeartBeatSendNum = 0x00;
      HeartBeatSendFlag = 0x00;
      if(SendNbRegFrame())
      {
        UpLoadTime = 0x00;
        RedLed_OFF;
        //RF_HandShakeTime = TIME_10M - 5;
        //AutoConnectFlag = 0x01;
        break;
      }
      //break;
    }
  }
  
  return re;
}
