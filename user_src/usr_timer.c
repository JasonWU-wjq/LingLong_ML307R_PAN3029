/*
 * usr_timer.c
 *
 *  Created on: 2021å¹´6æœˆ3æ—¥
 *      Author: WJQ
 */

#include "../usr_lib/sys.h"
#include "../usr_lib/ExternPara.h"

extern unsigned char ComTime;
extern unsigned char ComFlag;
bool g_timer_ms_generated;
extern bool g_rtc_period_generated;
extern unsigned char StrSearchTime;
unsigned char delay500ms;
void RunSysTime(void);

void RunSysTime(void)
{
//    rtc_time_t systime;
//
//    R_RTC_CalendarTimeGet (&g_rtc0_ctrl, &systime);
//
//    SysSecond = (unsigned char)systime.tm_sec;
//    SysMinute = (unsigned char)systime.tm_min;
//    SysHour   = (unsigned char)systime.tm_hour;

  SysSecond ++;

  if(SysSecond > 59)
  {
    SysSecond = 0x00;
    SysMinute ++;
    
    SetTimeFlag = 0x01;
    ScheduleCheckFlag = 1;    //Ô¤Ô¼Ä£Ê½¼ì²âÊ±¼ä
    if(ThermostatNum)   //ÓÐÎÂ¿ØÆ÷
    {
      SetTempFlag = 0x01;
    }
    
    if(SysMinute > 59)
    {
      SysMinute = 0x00;
      SysHour ++;

      if(SysHour > 23)
      {
        SysHour = 0x00;    //è¶…è¿‡24hï¼Œæ—¥æœŸåŠ 1
        SysDay ++;
        DateUpdateFlag = 0x01;
      }
    }
  }
}

void rtc_callback(rtc_callback_args_t * p_args)
{
  FSP_PARAMETER_NOT_USED(p_args);

  g_rtc_period_generated = true;
}

void agt_callback(timer_callback_args_t * p_args)
{
  FSP_PARAMETER_NOT_USED(p_args);
  //g_timer_1ms_generated = true;

  delay500ms ++;
  CommReadTime ++;
  CmdRunTime ++;

  if(delay500ms < 2)     //1s  delay
    return ;

  delay500ms = 0x00;
  
  NetDelayTime ++;
  
  if(ConnectFlag == NO_CONNECT) //if(TimingSaveFlag)  //Éè±¸²»ÔÚÏß£¬¿ªÊ¼¼ÆÊ±¶¨Ê±´æ´¢
  {
    SavingTime ++;
  }

  if(SavingTime >= SaveWaitingTime)
  {
    SavingTime = 0x00;
    SaveFlag = HISDATA_SAVE;
  }

//    if(SysYear >= UploadStartTime.year && SysMonth >= UploadStartTime.month && SysDay >= UploadStartTime.day &&
//       SysHour >= UploadStartTime.hour && SysMinute >= UploadStartTime.min)    //å½“å‰æ—¶é—´è¶…è¿‡ä¸ŠæŠ¥èµ·å§‹æ—¶é—´ï¼Œå¼€å§‹ç´¯åŠ æ—¶é—´ä¸ŠæŠ¥
#if 0
    if((SysYear > UploadStartTime.year) || (SysYear == UploadStartTime.year && SysMonth > UploadStartTime.month) ||
       (SysYear == UploadStartTime.year && SysMonth == UploadStartTime.month && SysDay > UploadStartTime.day) ||
       (SysYear == UploadStartTime.year && SysMonth == UploadStartTime.month && SysDay == UploadStartTime.day && SysHour > UploadStartTime.hour) ||
       (SysYear == UploadStartTime.year && SysMonth == UploadStartTime.month && SysDay == UploadStartTime.day && SysHour == UploadStartTime.hour && SysMinute >= UploadStartTime.min))
    {
        UpLoadTime ++;
    }
#endif
  
  UpLoadTime ++;
    
  if(ConnectFlag)        //ÍøÂçÁ¬½Ó³É¹¦ºó£¬¿ªÊ¼¼ÆÊ±¶
  {
    ConnectHoldTime ++;
    
#if 1   
    g_timer_ms_generated ^= 1;
    if(g_timer_ms_generated)
    {
      GreenLed_ON;
      //RedLed_OFF;
    }
    else
    {
      //RedLed_ON;
      GreenLed_OFF;
    }
#endif
  }

  RF_HandShakeTime ++;

  RunSysTime();

#if 0     //æ•…éšœå‘ç”Ÿæ—¶ï¼Œç³»ç»Ÿåªä¸ŠæŠ¥ä¸€æ¬¡ï¼Œç„¶åŽæ­£å¸¸ä¸ŠæŠ¥æ•°æ®ï¼Œä¸å†é—´éš”æŠ¥è­¦
    if(ErrFirstHappenFlag)   //é¦–æ¬¡æ•…éšœï¼Œå¼€å§‹è®¡æ—¶ï¼Œé—´éš”5minä¸ŠæŠ¥ä¸€ä¸‹
    {
      ErrHoldTime ++;
      if(ErrHoldTime >= TIME_5M)
      {
        ErrHoldTime = 0x00;
        ErrFirstHappenFlag = 0x00;
      }
    }
#endif

  if(KeyPressFlag)  //°´¼ü
  {
    KeyPressHoldTime ++;
  }

  ComTime ++;

  if(ComTime >= 3)    //´®¿ÚÍ¨ÐÅÊ±¼ä
  {
    ComTime = 0x00;
    ComFlag = 0x00;
    Uart0_Rx_index = 0x00;
    Uart9_Flag = 0x00;
  }

  if(UpgradeFlag)
  {
    UpgradeOverTime ++;
  }

  GetDataTime ++;  
  
  if(ReadBeginFlag)   //¶ÁÈ¡ÎÂ¿ØÆ÷Êý¾Ý¿ªÊ¼
  {
    GetThermInfoOverTime ++;
  }
  
  if(GetThermInfoOverTime >= 15)  //15s³¬Ê±ºò¼ÌÐøÏÂÒ»Ì¨Éè±¸¶ÁÈ¡
  {
    GetThermInfoOverTime = 0;
    ReadBeginFlag = 0;
    CurrThermNo += 1;           //´Ë´¦¿ÉÒÔ¿¼ÂÇ³¬Ê±ÒÔºóÊÇ·ñÐèÒªÖØÐÂ·¢ËÍ£¬ÖØ·¢¼´ÊÇ²»ÓÃÀÛ¼ÓÉè±¸Êý£¬¿ÉÒÔÔö¼Ó´ÎÊý£¬Í¨¹ý´ÎÊýÅÐ¶Ï
    GetThermInfoFlag = 1;
  }
  
  LLS_CuiserTime ++;    //Ñ²º½ÔËÐÐÊ±¼ä
    
  if(LLS_StartFlag == 1)  // ¿ªÆôË®±Ãºó¿ªÊ¼¼ÆÊ±ÔËÐÐÊ±¼ä
  {
    LLS_HoldTime ++;
  }
  
  if(LLS_HoldTime == LLS_Pump.pump_time && LLS_Pump.pump_time >= PUMP_WORKLIMT)
  {
    LLS_HoldTime = 0;
    LLS_StartFlag = 2;   //¹Ø±ÕË®±ÃÊ±¼äµ½
  }
  //ÅÐ¶ÏÓÃ»§ÊÇ·ñ¿ªÆôË®ÁúÍ·´¥·¢£¬²¢ÀÛ¼ÆÊ±¼ä2---10s£¬ÈÏÎªÊÇµã¶¯Ö´ÐÐÒ»´Î
  if(BathWaterUseFlag)    //¿ªÊ¼ÀÛ¼ÆÎÀÔ¡Ë®Ê¹ÓÃÊ±¼ä£¬Í¬Ê±ÒªÅÐ¶Ï¹Ø±Õ
  {
    BathWaterUseTime ++;
  }
  else if(!BathWaterUseFlag)
  {
    if(BathWaterUseTime >= 2 && BathWaterUseTime <= 10)   //¼ì²âµ½¿ªÆôË®ÁúÍ·±êÖ¾£¬¿ªÆôÁãÀäË®
    {
      BathWaterUseTime = 0;
      //======================Ö´ÐÐµã¶¯
      RunOnceFlag = 1;
    }
    else
    {
      BathWaterUseTime= 0;
    }
  }
  
  if(LLS_ReSendFlag)   //ÁãÀäË®ÃüÁî·¢³öºó¿ªÊ¼¼ÆÊ±£¬×¼±¸ÔÙ´Î·¢ÃüÁî
  {
    LLS_ReSendTime ++;
  }
  
  ///=======================20240803
  NetRstTime ++;    //ÍøÂçÍ¨ÐÅÊ±¼ä¸ú×Ù
  if(NetRstTime >= TIME_15M)   //ÍøÂçÍ¨ÐÅ³¬Ê±³¬¹ý15min£¬ÖØÐÂÁ¬½ÓÍøÂç
  {
    NetRstTime = 0;
    ConnectFlag = NO_CONNECT;
    UpLoadTime = UpLoadWaitingTime - 1;
  }
  
  StrSearchTime ++;
  ////====================================================ÔÚ²â250114
#if 0     //²âÊÔºó·¢ÏÖÎÞÐ§
  if(SysHour == 1 && SysMinute == 0 && SysSecond == 0)   //Ã¿Íí1µã¸´Î»
  {
    SysSecond ++;
    RESET_MCU;
  }
#endif
  ///=====================================================
  if(ActiveTime)    //rf¼¤»î±£³ÖÊ±¼ä
  {
    ActiveTime --;
  }
  
  if(ReissuedFlag)   //ÖØ·¢¼ÆÊ±¿ªÊ¼
  {
    ReissuedTime ++;
  }
  ///=====================================================

  if(DevSosRunFlag)
  {
    DevSosRunTime ++;
  }
}



