/*
 * user_uart.c
 *
 *  Created on: 2021å¹´6æœˆ3æ—¥
 *      Author: WJQ
 */

#include "../usr_lib/sys.h"

extern bool g_uart0_recv_complete;
extern bool g_uart0_send_complete;
extern bool g_uart9_recv_complete;
extern bool g_uart9_send_complete;

//unsigned char Uart0_Rxbuf[35];
//unsigned char Uart0_Rx_index;
unsigned char ch,oldch;
unsigned char uart0_ch,Prech;
unsigned char ComBuf[30];
unsigned char ComFlag;
unsigned char ComPtr;
unsigned char ComTime;
unsigned char ComNum;
unsigned char ComCheck;

void user_uart0_callback(uart_callback_args_t * p_args)
{
  unsigned int  crc16;

    /* Handle the UART event */
  switch (p_args->event)
  {
         /* Received a character */
    case UART_EVENT_RX_CHAR:
    {
      if(UART_DATA_BITS_8 == g_uart0_cfg.data_bits)
      {
        uart0_ch = (unsigned char) p_args->data;

        if(SetUpFlag)
        {
          if(ComPtr >= 30)
          {
            ComPtr = 0x00;
          }

          if(!ComFlag)
          {
            if(Prech == 0x68 && uart0_ch == 0x50)
            {
              ComFlag = 1;
              ComTime = 0;

              ComBuf[0] = Prech;
              ComBuf[1] = uart0_ch;

              ComNum = 14;

              ComCheck = uart0_ch + Prech;
              ComPtr = 2;
            }
          }
          else
          {
            ComBuf[ComPtr++]= uart0_ch;

            ComTime = 0;

            ComNum --;

            if(ComNum == 0)
            {
              ComFlag = 0;

              if(uart0_ch == 0x16)
              {
                 ComValid = 0x01;
              }
            }
            else
            {
              if(ComNum == 0x02)
              {
                 ComNum += uart0_ch;
              }

              if(ComPtr == 15)
              {
                if(uart0_ch != ComCheck)
                {
                  ComFlag = 0;
                  ComPtr = 0;
                  return;
                }
              }

              ComCheck += uart0_ch;
            }
          }
          Prech = uart0_ch;
        }
        else
        {
          if(Uart0_Rx_index >= 64)
          {
            Uart0_Rx_index = 0;
          }

          ComTime = 0x00;

          Uart0_Rx_buf[Uart0_Rx_index++] = uart0_ch;

		      if(Uart0_Rx_index > 4)
		      {
			      if(Uart0_Rx_buf[0] == LocalAddr)
			      {
			        if(Uart0_Rx_buf[1] == 0x03)
			        {
				        if((Uart0_Rx_index > Uart0_Rx_buf[2]+4))
			          {
				          crc16 = chkcrc(Uart0_Rx_buf,Uart0_Rx_buf[2]+3);

				          if((Uart0_Rx_buf[Uart0_Rx_buf[2]+3] == (unsigned char)(crc16 & 0xff)) && (Uart0_Rx_buf[Uart0_Rx_buf[2]+4] == (unsigned char)(crc16 >> 8)))
				          {
                    Uart0_Rx_index = 0x00;

                    HeatBathWaterTemp = Uart0_Rx_buf[4];     //²ÉÅ¯Ë®µ±Ç°ÎÂ¶È£¬16Î»µÍ8Î»
                    ProValveOut = Uart0_Rx_buf[5]; //RoomTemp = Uart0_Rx_buf[5];   //»·¾³ÎÂ¶È
                    BathWaterTemp = Uart0_Rx_buf[6];       //ÎÀÔ¡Ë®µ±Ç°ÎÂ¶È

					// Dev_Status
#if 1  //
                    if(Uart0_Rx_buf[8] & BIT0)     //È¡Å¯Ë®Ñ¹¿ª¹Ø
                    {
                      Dev_Status[0] |= BIT4;
                      SysRunStatus |= BIT0;
                    }
                    else
                    {
                      Dev_Status[0] &= ~BIT4;
                      SysRunStatus &= ~BIT0;
                    }
#endif
                   if(Uart0_Rx_buf[8] & BIT1)   //ÊÒÄÚÎÂ¿ØÆ÷¿ª¹Ø
                   {
                     Dev_Status[0] |= BIT3;
                     SysRunStatus |= BIT1;
                   }
                   else
                   {
                     Dev_Status[0] &= ~BIT3;
                     SysRunStatus &= ~BIT1;
                   }
#if 1
                   if(Uart0_Rx_buf[8] & BIT2)   //ÓÐÎÀÔ¡Ë®
                   {
                     Dev_Status[0] |= BIT5;
                     SysRunStatus |= BIT2;
                     BathWaterUseFlag = 1;     
                   }
                   else
                   {
                     Dev_Status[0] &= ~BIT5;
                     SysRunStatus &= ~BIT2;
                     BathWaterUseFlag = 0;
                   }

                   if(Uart0_Rx_buf[8] & BIT3)   //ÓÐÈ¡Å¯Ë®
                   {
                     Dev_Status[0] |= BIT7;
                     SysRunStatus |= BIT3;
                   }
                   else
                   {
                     Dev_Status[0] &= ~BIT7;
                     SysRunStatus &= ~BIT3;
                   }
#endif
                   if(Uart0_Rx_buf[8] & BIT4)   //·ç»úÔËÐÐ
                   {
                     DevFacPara[2] |= BIT5;
                     SysRunStatus |= BIT4;
                   }
                   else
                   {
                     DevFacPara[2] &= ~BIT5;
                     SysRunStatus &= ~BIT4;
                   }

                   if(Uart0_Rx_buf[8] & BIT5)   //ÓÐ»ð
                   {
                     Dev_Status[0] |= BIT1;
                     SysRunStatus |= BIT5;
                   }
                   else
                   {
                     Dev_Status[0] &= ~BIT1;
                     SysRunStatus &= ~BIT5;
                   }

                   if(Uart0_Rx_buf[8] & BIT6)   //Ë®±ÃÔËÐÐ
                   {
                     DevFacPara[2] |= BIT6;
                     SysRunStatus |= BIT6;
                   }
                   else
                   {
                     DevFacPara[2] &= ~BIT6;
                     SysRunStatus &= ~BIT6;
                   }
#if 0
                   if(Uart0_Rx_buf[8] & BIT7)   //·À¶³ÔËÐÐ¡Œ
                   {
                     Dev_Status[1] |= BIT5;
                     SysRunStatus |= BIT7;
                   }
                   else
                   {
                     Dev_Status[1] &= ~BIT5;
                     SysRunStatus &= ~BIT7;
                   }
#endif
					 //===============================================================
                   if(!Uart0_Rx_buf[10])
                   {
                     ErrorFlag = 0x00;
                   }

                   if(Uart0_Rx_buf[10] & BIT0)   //ç»“å†° 12
                   {
                     ErrorFlag = 12;  //ç»“å†°æ•…éšœ  E0
                   }
                   if(Uart0_Rx_buf[10] & BIT1)
                   {
                     ErrorFlag = 1;  //ç‚¹ç«æ•…éšœ  E1
                   }
                   if(Uart0_Rx_buf[10] & BIT2)
                   {
                     ErrorFlag = 6;   //ç«ç„°æ£€æµ‹æ•…éšœ  E8
                   }
                   if(Uart0_Rx_buf[10] & BIT3)
                   {
                     ErrorFlag = 2;  //é£Žæœº/é£ŽåŽ‹æ•…éšœ  E2
                   }
                   if(Uart0_Rx_buf[10] & BIT4)
                   {
                     ErrorFlag = 3;    //æœºæ¢°æ¸©æŽ§æ•…éšœ  E3
                   }
                   if(Uart0_Rx_buf[10] & BIT5)
                   {
                     ErrorFlag = 7;   //æ°´åŽ‹æ•…éšœ   E9
                   }
                   if(Uart0_Rx_buf[10] & BIT6)
                   {
                     ErrorFlag = 8;    //é‡‡æš–æŽ¢å¤´è¶…æ¸©æ•…éšœ
                   }
                   if(Uart0_Rx_buf[10] & BIT7)
                   {
                     ErrorFlag = 4;    //å«æµ´æŽ¢å¤´è¶…æ¸©æ•…éšœ
                   }
                   if(Uart0_Rx_buf[9] & BIT0)
                   {
                     ErrorFlag = 5;    //é‡‡æš–æŽ¢å¤´æ•…éšœ
                   }
                   if(Uart0_Rx_buf[9] & BIT1)
                   {
                     ErrorFlag = 10;   //å«æµ´æŽ¢å¤´æ•…éšœ
                   }
                   if(Uart0_Rx_buf[9] & BIT2)
                   {
                     ErrorFlag = 11;  //çŽ¯å¢ƒæŽ¢å¤´æ•…éšœ
                   }
                   if(Uart0_Rx_buf[9] & BIT3)
                   {
                     ErrorFlag = 9;   //æ¸©å‡æ•…éšœ
                   }

                   HeatBathWaterTargetTemp = Uart0_Rx_buf[12];    //ä¾›æš–æ°´ç›®æ ‡æ¸©åº¦
                   BathWaterTargetTemp = Uart0_Rx_buf[14];        //å«æµ´æ°´ç›®æ ‡æ¸©åº¦

                   MinPower = Uart0_Rx_buf[16];//FirePower = Uart0_Rx_buf[16];  //ç«åŠ›æ¡£ä½
#if 1
                   //ÁãÀäË®Æô¶¯£¬ÅÐ¶ÏË®ÎÂ¼ì²â±êÖ¾ºÍÊµ¼ÊË®ÎÂ
                   if((BathWaterTemp >= BathWaterTargetTemp) && WaterTempCheckFlag)    //µ±Ç°ÎÀÔ¡Ë®ÎÂ¶ÈÒÑ¾­³¬¹ýÉè¶¨ÎÂ¶È
                   {
                      WaterTempCheckFlag = 0;
                      if(PumpRunFlag)  //ÅÐ¶ÏÑ­»·±ÃÊÇ·ñ»¹ÔÚ¹¤×÷
                      {
                       if(LLS_HoldTime + OVERTEMPTIME < LLS_Pump.pump_time)   //ÑÓ³Ù5s¹Ø±ÕË®±Ã£¬·ÀÖ¹Ë®ÎÂ¹ý¸ß
                       {
                         LLS_HoldTime = LLS_Pump.pump_time - OVERTEMPTIME;  //ÑÓ³Ù5s
                         LLS_StartFlag = 1;  //·ÀÖ¹Ã»ÓÐ¼ÆÊ±
                       }
//                       else     //¾àÀë¹Ø±ÕË®±Ã²»×ã5s£¬°´ÕÕµ±Ç°Ê±¼ä¼ÌÐøÖ´ÐÐ
//                       {
//                         LLS_HoldTime = LLS_HoldTime
//                       }
                     }
                   }
#endif
                   if(Uart0_Rx_buf[18])    //1ï¼šå¼€æœº  0--å…³æœº
                   {
                    Dev_Status[1] |= BIT0;
                    SysRunStatus |= BIT8;
                   }
                   else
                   {
                    Dev_Status[1] &= ~BIT0;
                    SysRunStatus &= ~BIT8;
                   }

					 //RunMode  = Uart0_Rx_buf[20];
                   if(Uart0_Rx_buf[20])    //1ï¼šå†¬å­£æ¨¡å¼ï¼Œ0--å¤å­£æ¨¡å¼
                   {
                    Dev_Status[1] |= BIT6;
                    SysRunStatus |= BIT9;
                   }
                   else
                   {
                    Dev_Status[1] &= ~BIT6;
                    SysRunStatus &= ~BIT9;
                   }

                   GasTotalVol = ((unsigned long)Uart0_Rx_buf[21] << 24) + ((unsigned long)Uart0_Rx_buf[22] << 16) + ((unsigned int)Uart0_Rx_buf[23] << 8) + Uart0_Rx_buf[24];
                   ///¼ä¸ôÒ»¸ö¼Ä´æÆ÷  25--26
                   ///===´Ë´¦Ôö¼Ówifi\ÎÞÏß¶ÔÂëºÍ±¨¾¯ buf[25]==1£º½øÈëÎÞÏßÅäÖÃ 0£ºÍË³öÎÞÏßÅäÖÃ   2£º½øÈëWIFIÅäÖÃ 0£ºÍË³öWIFIÅäÖÃ
                  ///===buf[26]==1£º½øÈë±¨¾¯ 0£ºÍË³ö±¨¾¯
                   if(Uart0_Rx_buf[25] == 1 && !RF_PairFlag)   //µ±Ç°²»ÊÇÅä¶Ô×´Ì¬£¬½øÈëÅä¶Ô×´Ì¬
                   {
                     DeviceSta = MatchCode;    //½øÈëÅä¶Ô×´Ì¬
                     RF_PairFlag = 1;
                     RF_PairTime = 0;
                   }
//                   else if(Uart0_Rx_buf[25] == 2 && !NetPairFlag && ConfigNetFlag == OFF)   //WiFiÅäÍø
//                   {
//                     NetPairFlag = 1;
//                     NetPairTime = 0;
//                   }
                   
                   alarm_sta = Uart0_Rx_buf[26];                            //é—´éš”ä¸€ä¸ªå¯„å­˜å™¨  25--26
                   if(alarm_sta & ALARM_ICON)
                   {
                     DevSosRunFlag = true;          //sosÏìÓ¦Ö´ÐÐ¼ÆÊ±£¬30sÇå³ý
                   }
                   
                   PowerUsingTime = ((unsigned long)Uart0_Rx_buf[27] << 24) + ((unsigned long)Uart0_Rx_buf[28] << 16) + ((unsigned int)Uart0_Rx_buf[29] << 8) + Uart0_Rx_buf[30];

                   FlucTemp = Uart0_Rx_buf[32];  //å›žå·®æ¸©åº¦

                   if(Uart0_Rx_buf[34])   //æ¢çƒ­æ–¹å¼
                   {
                     DevFacPara[0] &= ~BIT2;    //æ•£çƒ­ç‰‡æ¨¡å¼
                   }
                   else
                   {
                     DevFacPara[0] |= BIT2;   //åœ°æš–
                   }

                   CurrFlowRate = Uart0_Rx_buf[36];  //å½“å‰æ°´æµé‡

                   WaterTotalVol = ((unsigned long)Uart0_Rx_buf[37] << 24) + ((unsigned long)Uart0_Rx_buf[38] << 16) + ((unsigned int)Uart0_Rx_buf[39] << 8) + Uart0_Rx_buf[40];
#ifdef PreSetting_Version
					 //====================================================æ–°å¢žæ•°æ®
                   Temp_Compensation_Switch = Uart0_Rx_buf[42];    //41---42
                   EnvironmentTemp = Uart0_Rx_buf[44];    //43---44
                   RoomTemp = Uart0_Rx_buf[46];    //45---46;
                   SetTemp  = Uart0_Rx_buf[48];    //47---48
                   Stove_Hour = Uart0_Rx_buf[49];
                   Stove_Min  = Uart0_Rx_buf[50];
					 //====================================================
#endif

#ifdef RemoteCtr
                   RemoteCtrFlag = Uart0_Rx_buf[52];
                   LossCommTime  = Uart0_Rx_buf[54];
#endif

#ifdef VerInfoCtr
                   MainBoardTyp = Uart0_Rx_buf[11] >> 5;
                   MainBoardVer = Uart0_Rx_buf[11] & 0x1F;
                   DisplayBoardTyp = Uart0_Rx_buf[13];
                   DisplayBoardVer = Uart0_Rx_buf[15];
                   ComBoardVer = Uart0_Rx_buf[17];
#endif
                  }
			          }
			       }
//			       else if (Uart0_Rx_buf[1] == 0x06)   //å†™å‘½ä»¤åº”ç­”
//			       {
//				       Uart0_Rx_index = 0x00;
//			       }
           }
         }
       }
     }
     break;
   }
         /* Receive complete */
   case UART_EVENT_RX_COMPLETE:
   {
       //APP_PRINT("\r\nUART0 receive complete");
       //g_uart0_recv_complete = true;
       break;
   }
         /* Transmit complete */
   case UART_EVENT_TX_COMPLETE:
   {
       //APP_PRINT("\r\nUART0 send complete");
       g_uart0_send_complete = true;
       break;
   }
   default:
   {
       break;
   }
  }
}

void user_uart9_callback(uart_callback_args_t * p_args)    //
{
  unsigned int crc;
  
  if(USCIA0RXNum >= UARTPOINTNUM)
  {
     USCIA0RXNum = 0;
  }
  /* Handle the UART event */
  switch (p_args->event)
  {
      /* Received a character */
    case UART_EVENT_RX_CHAR:
    {
      if(UART_DATA_BITS_8 == g_uart9_cfg.data_bits)
      {
        ch = (unsigned char) p_args->data;
        
        if(USCIA0RXNum >= UARTPOINTNUM)   //·ÀÖ¹½ÓÊÕÔ½½ç
        {
          USCIA0RXNum = 0;
        }
        
        if(ConnectFlag)   //ÊµÊ±¼ì²â£¬socket¶Ï¿ªÊÕµ½+IPCLOSE
        {
          if(oldch == '+' && ch == 'I')
          {
            NetStatusFlag = 0x01;
          }
          else if (oldch == 'P' && ch == 'C' && NetStatusFlag == 1)
          {
            NetStatusFlag = 0x02;
          }
          else if(oldch == 'L' && ch == 'O' && NetStatusFlag == 2)
          {
            NetStatusFlag = 0x03;
          }
          else if(oldch == 'S' && ch == 'E' && NetStatusFlag == 3)
          {
            NetStatusFlag = 0x04;
          }
        }
        
#ifdef TRANS_MODE
        USCIA0RXBUF[USCIA0RXNum++] = ch;
        //Ä£×éÍ¸´«Ä£Ê½ÏÂµÄ½ÓÊÕ
        if(!Uart9_Flag)   
        {
          if(oldch == 0x68 && (ch == 0x86 || ch == 0x50))   //±Ú¹ÒÂ¯Ð­ÒéºÍWiFiÎÂ¿ØÐ­Òé
          {
            Uart9_Flag = 0x01;
            
            USCIA0RXBUF[0] = oldch;
            USCIA0RXBUF[1] = ch;
            USCIA0RXNum = 0x02;
            ComTime = 0x00;
            if(ch == 0x86)        //±Ú¹ÒÂ¯Ð­Òé
            {
              ProtocolFlag = 0x00;
              Uart9_DataNum = 18;   //×îÐ¡°ü20×Ö½Ú£¬ÒÑ¾­ÊÕµ½ÁË2×Ö½Ú
            }
            else                  //ÎÂ¿ØÆ÷Ð­Òé
            {
              ProtocolFlag = 0x01;
              Uart9_Check = oldch + ch;   //Ð£ÑéºÍÈ¡µÍ×Ö½Ú
              Uart9_DataNum = 14;   //×îÐ¡°ü16×Ö½Ú£¬ÒÑ¾­ÊÕµ½ÁË2×Ö½Ú
            }
          }
        }
        else
        {
          //USCIA0RXBUF[USCIA0RXNum++] = ch;
          Uart9_DataNum --;
          ComTime = 0;
          ConnectHoldTime = 0;
          NetRstTime = 0;
          
          if(Uart9_DataNum == 0)   //½ÓÊÕÍê³É
          {
            if(ch == 0x16)
            {
//              if(!ProtocolFlag)
//              {
//                ComValid = 1;
//              }
//              else
//              {
//                ComValid = 2;
//              }
              HeartBeatSendNum = 0;
              HeartBeatSendFlag = 0;
              Uart9_Flag = 0x00;
              ComValid = 0x01;
            }
          }
          else
          {
            if(!ProtocolFlag)    //±Ú¹ÒÂ¯Ð­Òé³¤¶È×Ö½ÚºÍcrcÐ£Ñé´¦Àí
            {
              if(USCIA0RXNum == 12)     //ÊÕµ½³¤¶È×Ö½Ú
              {
                Uart9_DataNum = ((unsigned int)ch << 8) + oldch + 3;  //Êý¾ÝÓò³¤¶ÈÔÙ¼ÓÉÏCRCºÍ16
                 //delayus(1);
                if(Uart9_DataNum >= UARTPOINTNUM)   //³¤¶È´íÎó
                {
                  Uart9_Flag = 0;
                  USCIA0RXNum = 0;
                }
              }
              else
              {
                if(Uart9_DataNum == 1)
                {
                  crc = chkcrc(USCIA0RXBUF,USCIA0RXNum-2);
                  if(USCIA0RXBUF[USCIA0RXNum-2] != (crc & 0xff) || USCIA0RXBUF[USCIA0RXNum-1] != (crc >> 8))   //¼ìÑé³ö´í
                  {
                    Uart9_Flag = 0;
                    USCIA0RXNum = 0;
                  }
                }
              }
            }
            else   //WiFiÎÂ¿ØÆ÷Ð­Òé³¤¶È×Ö½ÚºÍÐ£ÑéºÍ´¦Àí
            {
               if(USCIA0RXNum == 11)  //ÊÕµ½³¤¶È×Ö½Ú
               {
                  Uart9_DataNum = ch + 2;
               }
               else
               {
                 if(Uart9_DataNum == 1)
                 {
                   if(ch != Uart9_Check) 
                   {
                     Uart9_Flag = 0;
                     USCIA0RXNum = 0;
                   }
                 }
               }
               Uart9_Check += ch;
            }
          }
        }
#else         //¸Ã·½·¨ÊÇ°´ÕÕÄ£×éÄ¬ÈÏÖ±´«Ä£Ê½£¬°´Ä£×éÊä³öµÄ³¤¶È½øÐÐ½ÓÊÕ
        USCIA0RXBUF[USCIA0RXNum++] = ch;
        
        if(oldch == 'r' && ch == 'e')// && RevChar == 1)            //ÊÕµ½recv£¬ºó±ß½ô¸úsocket ID£¬Êý¾Ý³¤¶ÈºÍÊý¾Ý
        {
          RevChar = 1;
        }
        else if(oldch == 'c' && ch == 'v' && RevChar == 1)
        {
          RevChar = 2;
          USCIA0RXNum = 0;       //ÊÕµ½±êÊ¶·ûºó¿ªÊ¼½ÓÊÕºó±ßµÄÊý¾Ý
        }
        else if(RevChar == 2 && oldch == 0x0D && ch == 0x0A)   //»Ø³µ»»ÐÐÇ°±ßÊÇÊý¾Ý³¤¶È£¬ºó±ßÊÇÓÐÐ§Êý¾Ý£¬¿ªÊ¼½ÓÊÕÊý¾Ý
        {
          if(USCIA0RXNum == 7)   //¸ù¾ÝÖ¸ÕëÅÐ¶ÏÊÇ¼¸Î»³¤¶È
          {
            PackLen = USCIA0RXBUF[USCIA0RXNum-3] - 0x30;
          }
          else if(USCIA0RXNum == 8)
          {
            PackLen = (USCIA0RXBUF[USCIA0RXNum-4] - 0x30) * 10 + (USCIA0RXBUF[USCIA0RXNum-3] - 0x30);
          }
          else if(USCIA0RXNum == 9)
          {
            PackLen = (USCIA0RXBUF[USCIA0RXNum-5] - 0x30) * 100 + (USCIA0RXBUF[USCIA0RXNum-3] - 0x30) * 10 + (USCIA0RXBUF[USCIA0RXNum-2] - 0x30);
          }
          
          if(PackLen == 0 || PackLen >= UARTPOINTNUM)
          {
            PackLen = 0;
            USCIA0RXNum = 0;
            return ;
          }
          else
          {
            RevChar = 0x03;      //ÒÑ¾­»ñÈ¡µ½³¤¶È£¬¿ªÊ¼½ÓÊÕÊµ¼ÊÊý¾Ý
            USCIA0RXNum = 0;
          }
        }
        else if(RevChar == 0x03)
        {
          if(USCIA0RXNum == PackLen)  //Êý¾Ý°´³¤¶È½ÓÊÕÍê³É
          {
            RevChar = 0x04;     
          }
        }
#endif
        oldch = ch;
      }
      break;
    }
        /* Receive complete */
    case UART_EVENT_RX_COMPLETE:
    {
        //APP_PRINT("\r\nUART9 receive complete");
        g_uart9_recv_complete = true;
        break;
    }
    /* Transmit complete */
    case UART_EVENT_TX_COMPLETE:
    {
        //APP_PRINT("\r\nUART9 send complete");
        g_uart9_send_complete = true;
        break;
    }
    default:
    {
        break;
    }
  }
}


