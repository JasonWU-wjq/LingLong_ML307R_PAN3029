/*
 * save.c
 *
 *  Created on: 2021å¹´6æœˆ4æ—¥
 *      Author: WJQ
 */

#include  "../usr_lib/at24.h"
#include  "../usr_lib/datapro.h"

void Delay_Ms(unsigned int tt)
{
    unsigned int i,n;
    //unsigned char re,cmd;

    //re = 0;
    //cmd = 0;

    for(i=0;i<tt;i++)
    {
        for(n=0;n<1230;n++);

//        if(ReceiveFlag)     //æœ‰æ— çº¿æ§åˆ¶å‘½ä»¤
//        {
//            break;
//        }
    }
}

void Delay10us(void)
{
  uint8_t  i;

  for(i=0;i<3;i++);
}

unsigned char AT24_writebyte(unsigned int Addr, unsigned char *buff, unsigned int num)
{
  unsigned int i;
  unsigned char addr1,addr2;

  addr1 = (unsigned char)(Addr>>8);
  addr2 = (unsigned char)(Addr&0xFF);

  AT24_start();                       //å‘èµ·å§‹ä½
  AT24_wbyte(0xa0);                  //å‘å†™å‘½ä»¤
  AT24_wbyte(addr1);                  //å‘åœ°å€é«˜å­—èŠ‚
  AT24_wbyte(addr2);                 //å‘åœ°å€ä½å­—èŠ‚

  for(i=0;i<num;i++)
  {
     ////////è§£å†³è¿å†™é—®é¢˜////
    if(!((Addr+i)%128))      //  ??(Addr+i)%127
    {
      AT24_stop();
      delayms(10);               //åˆ†é¡µæ“ä½œï¼Œå¿…é¡»ä¿è¯å»¶æ—¶
      addr1 = (unsigned char)((Addr+i)>>8);
      addr2 = (unsigned char)((Addr+i)&0xFF);
      AT24_start();                      //é‡æ–°å‘å¯å§‹ä½
      AT24_wbyte(0xa0);                  //é‡æ–°å‘ä¼ªå†™å‘½ä»¤
      AT24_wbyte(addr1);                 //é‡æ–°å‘åœ°å€é«˜å­—èŠ‚
      AT24_wbyte(addr2);                 //é‡æ–°å‘åœ°å€ä½å­—èŠ‚
    }

    if(AT24_wbyte(buff[i]))                    //å‘éœ€å†™çš„æ•°æ®
      return 1;
  }

  AT24_stop();                          //å‘åœæ­¢ä½
  delayms(1);                           //å†™å®Œæˆåç¨å¾®å»¶æ—¶ï¼Œä¿è¯å†™å…¥æ­£ç¡®
  return 0;
}

unsigned char AT24_readbyte(unsigned int Addr, unsigned char* buff, unsigned int num)
{
  unsigned int i;
  unsigned char addr1,addr2;

  addr1 = (unsigned char)(Addr/256);
  addr2 = (unsigned char)(Addr%256);
  AT24_start();                      //å‘å¯å§‹ä½
  AT24_wbyte(0xa0);                  //å‘ä¼ªå†™å‘½ä»¤
  AT24_wbyte(addr1);                 //å‘åœ°å€é«˜å­—èŠ‚
  AT24_wbyte(addr2);                 //å‘åœ°å€ä½å­—èŠ‚
  AT24_start();                      //å‘å¯å§‹ä½
  AT24_wbyte(0xa1);                  //å‘è¯»å‘½ä»¤

  for(i=0;i<num;i++)
  {
    buff[i] = AT24_rbyte();          //ä»åœ°å€ä¸­è¯»ä¸€ä¸ªå­—èŠ‚åˆ°å˜é‡bytetempä¸­
    if(i == (num-1))
       AT24_bitout(1);
    else
       AT24_bitout(0);

    SDA_SET_IN;                       //SDA_DIR &= ~SDA_BIT;
  }

  AT24_stop();                       //å‘åœæ­¢ä½

  return 0;
}

//unsigned char AT24_writebyte(unsigned int Addr, unsigned char *buff, unsigned int num)
//{
//  unsigned int i;
//  unsigned char addr1,addr2;
//
//  addr1 = (unsigned char)(Addr>>8);
//  addr2 = (unsigned char)(Addr&0xFF);
//
//  AT24_start();                       //å‘èµ·å§‹ä½
//  AT24_wbyte(0xa0);                  //å‘å†™å‘½ä»¤
//  AT24_wbyte(addr1);                  //å‘åœ°å€é«˜å­—èŠ‚
//  AT24_wbyte(addr2);                 //å‘åœ°å€ä½å­—èŠ‚
//
//  for(i=0;i<num;i++)
//  {
//     ////////è§£å†³è¿å†™é—®é¢˜////
//    if(!((Addr+i)%128))      //  ??(Addr+i)%127
//    {
//      AT24_stop();
//      delayms(10);
//      addr1 = (unsigned char)((Addr+i)>>8);
//      addr2 = (unsigned char)((Addr+i)&0xFF);
//      AT24_start();                      //é‡æ–°å‘å¯å§‹ä½
//      AT24_wbyte(0xa0);                  //é‡æ–°å‘ä¼ªå†™å‘½ä»¤
//      AT24_wbyte(addr1);                 //é‡æ–°å‘åœ°å€é«˜å­—èŠ‚
//      AT24_wbyte(addr2);                 //é‡æ–°å‘åœ°å€ä½å­—èŠ‚
//    }
//
//    if(AT24_wbyte(buff[i]))                    //å‘éœ€å†™çš„æ•°æ®
//      return 1;
//  }
//
//  AT24_stop();                          //å‘åœæ­¢ä½
//  delayms(1);                           //å†™å®Œæˆåç¨å¾®å»¶æ—¶ï¼Œä¿è¯å†™å…¥æ­£ç¡®
//  return 0;
//}

unsigned char AT24_rbyte(void)
{
  unsigned char bittemp,bytetemp,i;
  bytetemp = 0;

  for (i=0; i<8; i++)
  {
    bytetemp <<= 1;

    bittemp = AT24_bitin();
    if (bittemp)
      bytetemp |= 0x01;
  }

  return bytetemp;
}


unsigned char AT24_wbyte(unsigned char ch)
{
  int i;
  unsigned char mask;

  mask = 0x80;

  for (i=0; i<8; i++)
  {
    if (ch & mask)
      AT24_bitout(1);
    else
      AT24_bitout(0);

    mask >>= 1;
  }

  if(AT24_bitin())
    return 1;
  return 0;
}

void AT24_start(void)
{
  SDA_SET_OUT;                //SDA_DIR |= SDA_BIT;

  SCL_SET_OUT;                //SCL_DIR |= SCL_BIT;
  Delay10us();
  SCL_HIGH;                   //SCL_OUT |= SCL_BIT;
  Delay10us();
  SDA_HIGH;                   //SDA_OUT |= SDA_BIT;
  Delay10us();
  SDA_LOW;                    //SDA_OUT &= ~SDA_BIT;
  Delay10us();
  SCL_LOW;                    //SCL_OUT &= ~SCL_BIT;
  Delay10us();
}

void AT24_stop(void)
{
  SDA_SET_OUT;                //SDA_DIR |= SDA_BIT;

  SCL_SET_OUT;                //SCL_DIR |= SCL_BIT;
  Delay10us();
  SDA_LOW;                    //SDA_OUT &= ~SDA_BIT;
  Delay10us();
  SCL_HIGH;                   //SCL_OUT |= SCL_BIT;
  Delay10us();
  SDA_HIGH;                   //SDA_OUT |= SDA_BIT;
  Delay10us();
}

void AT24_bitout(unsigned char ch)
{
  SDA_SET_OUT;                 //SDA_DIR |= SDA_BIT;

  SCL_SET_OUT;                 //SCL_OUT &= ~SCL_BIT;

  Delay10us();
  if (ch)
    SDA_HIGH;                  //SDA_OUT |= SDA_BIT;
  else
    SDA_LOW;                   //SDA_OUT &= ~SDA_BIT;

  Delay10us();
  SCL_HIGH;                    //SCL_OUT |= SCL_BIT;
  Delay10us();
  SCL_LOW;                     //SCL_OUT &= ~SCL_BIT;
  Delay10us();
}

unsigned char AT24_bitin(void)
{
  //unsigned char ch;
  bsp_io_level_t  level;

  SDA_SET_IN;                   //SDA_DIR &= ~SDA_BIT;
  Delay10us();
  SCL_LOW;                      //SCL_OUT &= ~SCL_BIT;
  Delay10us();
  SCL_HIGH;                     //SCL_OUT |= SCL_BIT;
  Delay10us();

  //ch = SDA_IN;

  R_IOPORT_PinRead(&g_ioport_ctrl, BSP_IO_PORT_00_PIN_13, &level);

  SCL_LOW;                      //SCL_OUT &= ~SCL_BIT;
  Delay10us();

  //if(ch)//if (ch & SDA_BIT)
  if(level)
    return 1;
  else
    return 0;
}

//void saveaddr(void)
//{
//  AT24_POWERON;
//  delayms(5);
//
//  AT24_POWEROFF;
//}

//void AT24_FunctionEnable(void)
//{
//  AT24_POWERON;
//  SDA_SET_OUT;
//  SDA_HIGH;
//  SCL_SET_OUT;
//  SCL_HIGH;
//}
//
//void AT24_FunctionDisable(void)
//{
//  AT24_POWEROFF;
//  SDA_SET_IN;
//  SCL_SET_IN;
//}

/*é—´éš”
 7  6   5   4   3   2  | 1   0   BIT
æ—¶é—´æ•°å€¼    æ—¶é—´å•ä½    |
èŒƒå›´ï¼š1----31          | 0ï¼šmin  1ï¼šHour    2ï¼šDay

 */

void InitSchedule(void)
{
  MainThermostatSchedule[0].hour = 6;    //Ê±¶Î1
  MainThermostatSchedule[0].min = 0;
  MainThermostatSchedule[0].temper = 22;
  
  MainThermostatSchedule[1].hour = 8;      //Ê±¶Î2
  MainThermostatSchedule[1].min = 0;
  MainThermostatSchedule[1].temper = 18;
  
  MainThermostatSchedule[2].hour = 11;      //Ê±¶Î3
  MainThermostatSchedule[2].min = 30;
  MainThermostatSchedule[2].temper = 20;
  
  MainThermostatSchedule[3].hour = 14;       //Ê±¶Î4
  MainThermostatSchedule[3].min = 0;
  MainThermostatSchedule[3].temper = 16;
  
  MainThermostatSchedule[4].hour = 18;      //Ê±¶Î5
  MainThermostatSchedule[4].min = 0;
  MainThermostatSchedule[4].temper = 22;
  
  MainThermostatSchedule[5].hour = 22;       //Ê±¶Î6
  MainThermostatSchedule[5].min = 0;
  MainThermostatSchedule[5].temper = 16;
}

void InitData(void)
{
  unsigned char i,j;
  //memset(DeviceId,0x88,7);
  UpLoadMode    = 0x01;
  UpLoadCycle   = 0x01;     //1h
  UpLoadCycleUint = 0x02;
  UpLoadYear    = 21;
  UpLoadMonth   = 6;
  UpLoadDay     = 8;
  UpLoadHour    = 9;
  UpLoadMin     = 0;
  UpLoadSec     = 0;
  LinkDelayTime = 20;
  DataSaveCycle = 0x01;     //1h
  DataSaveCycleUint = 0x02;

  GroupNo  = 0x01;
  PeakTime = 0x3C;    //60

  UploadStartTime.year = 21;    //ä¸ŠæŠ¥èµ·å§‹æ—¶é—´ 2021å¹´6æœˆ1æ—¥8:00:00
  UploadStartTime.month = 6;
  UploadStartTime.day = 1;
  UploadStartTime.hour = 8;
  UploadStartTime.min = 0;
  UploadStartTime.sec = 0;

  ThermostatNum = 0;   //Ä¬ÈÏ²»´øÎÂ¿ØÆ÷
  
  MainThermostatMode = MANVAL_RUN;
  ManualTemper = 18;     
  AntifreezTemper = 10;
  
  MainThermostatScheduleNum = 6;
  InitSchedule();           //³õÊ¼»¯Ö÷ÎÂ¿ØÆ÷Ê±¶ÎĞÅÏ¢
  
  //===================================ÁãÀäË®²ÎÊı
  RevId[0] = 0x00;      //½ÓÊÕÆ÷³õÊ¼»¯ID
  RevId[1] = 0x11;
  RevId[2] = 0x22;
  RevId[3] = 0x33;
  RevId[4] = 0x44;
  RevId[5] = 0x55;
  RevId[6] = 0x66;
  NonColdWateStatus = 0;
  LLS_Pump.run_mode = 0;           //ÁãÀäË®¹Ø±Õ£¬1£ºµã¶¯  2£ºÑ²º½  3£º Ô¤Ô¼
  LLS_Pump.control_mode = 0;       //0£ºÊ±¼ä¿ØÖÆ    1£ºÎÂ¶È¿ØÖÆ
  LLS_Pump.pump_time = 180;     //3min£¬Ë®±ÃÖ´ĞĞÊ±³¤
  LLS_Pump.cuiser_interval = 60;  //60min  Ñ²º½¼ä¸ô
  LLS_Pump.control_temp  = 42;     //¿ØÖÆË®ÎÂ42¡æ
  LLS_Pump.schedule_num = 24;
  
  for(i=0,j=0;i<LLS_Pump.schedule_num*2;i+=2)            //Ä¬ÈÏÈÕ³ÌÊÇ24Ğ¡Ê±
  {
    LLS_Pump.schedule[i] = j++;
    LLS_Pump.schedule[i+1] = 0;
  }
   
  LLS_Pump.ec_num = 0x03;     //3¸ö½ÚÄÜÊ±¼ä¶Î
  LLS_Pump.ec_time[0] = 9;    //9:00---11:30
  LLS_Pump.ec_time[1] = 0;
  LLS_Pump.ec_time[2] = 11;
  LLS_Pump.ec_time[3] = 30;
  
  LLS_Pump.ec_time[4] = 14;   //14:00---17:30
  LLS_Pump.ec_time[5] = 0;
  LLS_Pump.ec_time[6] = 17;
  LLS_Pump.ec_time[7] = 30;
  
  LLS_Pump.ec_time[8] = 22;   //22:00---´ÎÈÕ6:00
  LLS_Pump.ec_time[9] = 30;
  LLS_Pump.ec_time[10] = 6;     
  LLS_Pump.ec_time[11] = 0;
   //============================ÍøÂç²ÎÊı
//    WifiPort = 5683;
//    memset(WifiDns,0,30);
//    //sprintf((char *)WifiDns,"117.60.157.137");   //OCå¹³å°
//    sprintf((char *)WifiDns,"221.229.214.202");  //AEPå¹³å°
  WifiPort = 30110;
  memset(WifiDns,0,30);
  //sprintf((char *)WifiDns,"117.60.157.137");   //OCå¹³å°
  sprintf((char *)WifiDns,"rmt-meta.com");  //CAT1·şÎñÆ÷
  
  XH_PumpFlag = 0;
  XH_PumpId[0] = 0x00;      //³õÊ¼»¯Ñ­»·±ÃID
  XH_PumpId[1] = 0x01;
  XH_PumpId[2] = 0x02;
  XH_PumpId[3] = 0x03;
  XH_PumpId[4] = 0x04;
  XH_PumpId[5] = 0x05;
  XH_PumpId[6] = 0x06;
}

void SaveData(void)
{
  unsigned int i;
  unsigned char check;

  check = 0;

  for(i=0;i<(SAVEMAXSIZE-1);i++)
  {
    check ^= FlashBuf[i];
  }

  FlashBuf[SAVEMAXSIZE-1] = check;

  AT24_POWERON;
  delayms(5);
  AT24_writebyte(UserParaAddr,FlashBuf,SAVEMAXSIZE);
  delayms(5);
  AT24_POWEROFF;
}

unsigned char ReadData(void)
{
  unsigned int i;
  unsigned char check;

  check = 0;

  AT24_POWERON;
  delayms(5);
  AT24_readbyte(UserParaAddr,FlashBuf,SAVEMAXSIZE);
  delayms(5);
  AT24_POWEROFF;

  for(i= 0;i<(SAVEMAXSIZE-1);i++)
  {
    check ^= FlashBuf[i];
  }
  
  //check = 0x00;
  if(check != FlashBuf[SAVEMAXSIZE-1] || 0x30 != FlashBuf[SAVEMAXSIZE-2])
  {
    InitData();
    return 0;
  }
  return 1;
}

void LoadSysPara(void)
{
  unsigned char re;

  re = 0;

  re = ReadData();
  //re = 0;
  if(re)
  {
    Buf_To_Para();
  }
  else
  {
    Buf_To_Para();    //¼ÓÔØµØÖ·£¬È»ºóÖØĞÂ³õÊ¼»¯ÆäËû²ÎÊı
//  DeviceId[0] = 0x11;
//  DeviceId[1] = 0x30;
//  DeviceId[2] = 0x00;
//  DeviceId[3] = 0x77;
//  DeviceId[4] = 0x40;
//  DeviceId[5] = 0x22;
//  DeviceId[6] = 0x15;
    InitData();
    SavePara();
  }
}

void SavePara(void)
{
  Para_To_Buf();
  SaveData();
}

void Buf_To_Para(void)
{
  unsigned char i,j,len = 0;
  
  UpLoadMode    = FlashBuf[0];
  UpLoadCycle   = FlashBuf[1];
  UpLoadCycleUint = FlashBuf[2];
  UpLoadYear    = FlashBuf[3];
  UpLoadMonth   = FlashBuf[4];
  UpLoadDay     = FlashBuf[5];
  UpLoadHour    = FlashBuf[6];
  UpLoadMin     = FlashBuf[7];
  UpLoadSec     = FlashBuf[8];
  LinkDelayTime = FlashBuf[9];
  DataSaveCycle = FlashBuf[10];
  DataSaveCycleUint = FlashBuf[11];

  GroupNo  = FlashBuf[12];
  PeakTime = FlashBuf[13];

  UploadStartTime.year  = FlashBuf[14];    //ä¸ŠæŠ¥èµ·å§‹æ—¶é—´
  UploadStartTime.month = FlashBuf[15];
  UploadStartTime.day   = FlashBuf[16];
  UploadStartTime.hour  = FlashBuf[17];
  UploadStartTime.min   = FlashBuf[18];
  UploadStartTime.sec   = FlashBuf[19];

  memcpy(DeviceId,FlashBuf+20,7);

  WifiPort  = (unsigned int)FlashBuf[27] * 10000 + (unsigned int)FlashBuf[28] * 100 + FlashBuf[29];
  memset((char*)WifiDns,0x00,30);
  memcpy((char*)WifiDns,FlashBuf+30,30);
  //memcpy(ThermostatId,FlashBuf+60,7);
  ThermostatNum = FlashBuf[60];           //ÎÂ¿ØÆ÷ÊıÁ¿
  
  for(i=0;i<ThermostatNum;i++)           //Ö÷´ÓÎÂ¿ØÆ÷±àºÅ£¬ÊıÁ¿Ğ¡ÓÚ5£¬
  {
    memcpy(RoomThermostat[i].id,FlashBuf+61+len,7);
    len += 7;
  }
  //=========================================ÎÂ¿ØÆ÷ĞÅÏ¢
  len += 61;   
  MainThermostatMode = FlashBuf[len];
  ManualTemper = FlashBuf[len+1];     
  AntifreezTemper = FlashBuf[len+2];
  MainThermostatScheduleNum = FlashBuf[len+3];
  
  for(i=0,j=0;i<MainThermostatScheduleNum*3;i+=3)
  {
    MainThermostatSchedule[j].hour = FlashBuf[len+4+i];
    MainThermostatSchedule[j].min  = FlashBuf[len+5+i];
    MainThermostatSchedule[j].temper = FlashBuf[len+6+i];
    j ++;
  }
  //InitSchedule();           //³õÊ¼»¯Ö÷ÎÂ¿ØÆ÷Ê±¶ÎĞÅÏ¢
  len += i + 4;
  //==========ÁãÀäË®²ÎÊı
  memcpy(RevId,FlashBuf+len,7);
  NonColdWateStatus = FlashBuf[len+7];
  LLS_Pump.pump_time = ((unsigned int)FlashBuf[len+8] << 8) + FlashBuf[len+9];
  if(LLS_Pump.pump_time < PUMP_WORKLIMT)
  {
    LLS_Pump.pump_time = 30;
  }
  
  LLS_Pump.run_mode  = FlashBuf[len+10];
  LLS_Pump.control_mode = FlashBuf[len+11];
  LLS_Pump.cuiser_interval = FlashBuf[len+12];
  LLS_Pump.control_temp = FlashBuf[len+13];
  LLS_Pump.schedule_num = FlashBuf[len+14];
  memcpy(LLS_Pump.schedule,FlashBuf+len+15,LLS_Pump.schedule_num*2);
  len += LLS_Pump.schedule_num*2;
  LLS_Pump.ec_num = FlashBuf[len+15];     //½ÚÄÜÊ±¼ä¶ÎÊıÁ¿
  
  if(LLS_Pump.ec_num > 3)
  {
    LLS_Pump.ec_num = 0;
  }
    
  for(i=0;i<LLS_Pump.ec_num*4;i++)
  {
    LLS_Pump.ec_time[i] = FlashBuf[len+16+i];
  }
  
//  ThermostatId[0] = FlashBuf[SAVEMAXSIZE-10];
//  ThermostatId[1] = FlashBuf[SAVEMAXSIZE-9];
//  ThermostatId[2] = FlashBuf[SAVEMAXSIZE-8];
//  ThermostatId[3] = FlashBuf[SAVEMAXSIZE-7];
//  ThermostatId[4] = FlashBuf[SAVEMAXSIZE-6];
//  ThermostatId[5] = FlashBuf[SAVEMAXSIZE-5];
//  ThermostatId[6] = FlashBuf[SAVEMAXSIZE-4];
//  
//  ThermEnableFlag = FlashBuf[SAVEMAXSIZE-3];
  ThermostatId[0] = FlashBuf[SAVEMAXSIZE-18];
  ThermostatId[1] = FlashBuf[SAVEMAXSIZE-17];
  ThermostatId[2] = FlashBuf[SAVEMAXSIZE-16];
  ThermostatId[3] = FlashBuf[SAVEMAXSIZE-15];
  ThermostatId[4] = FlashBuf[SAVEMAXSIZE-14];
  ThermostatId[5] = FlashBuf[SAVEMAXSIZE-13];
  ThermostatId[6] = FlashBuf[SAVEMAXSIZE-12];
  ThermEnableFlag = FlashBuf[SAVEMAXSIZE-11];
  
  XH_PumpFlag  = FlashBuf[SAVEMAXSIZE-10];
  XH_PumpId[0] = FlashBuf[SAVEMAXSIZE-9]; 
  XH_PumpId[1] = FlashBuf[SAVEMAXSIZE-8];
  XH_PumpId[2] = FlashBuf[SAVEMAXSIZE-7];
  XH_PumpId[3] = FlashBuf[SAVEMAXSIZE-6];
  XH_PumpId[4] = FlashBuf[SAVEMAXSIZE-5];
  XH_PumpId[5] = FlashBuf[SAVEMAXSIZE-4];
  XH_PumpId[6] = FlashBuf[SAVEMAXSIZE-3];
}

void Para_To_Buf(void)
{
  unsigned char i,j,len = 0;
  memset(FlashBuf,0,SAVEMAXSIZE);

  FlashBuf[0] = UpLoadMode;
  FlashBuf[1] = UpLoadCycle;
  FlashBuf[2] = UpLoadCycleUint;
  FlashBuf[3] = UpLoadYear;
  FlashBuf[4] = UpLoadMonth;
  FlashBuf[5] = UpLoadDay;
  FlashBuf[6] = UpLoadHour;
  FlashBuf[7] = UpLoadMin;
  FlashBuf[8] = UpLoadSec ;
  FlashBuf[9] = LinkDelayTime;
  FlashBuf[10] = DataSaveCycle;
  FlashBuf[11] = DataSaveCycleUint;

  FlashBuf[12] = GroupNo;
  FlashBuf[13] = PeakTime;

  FlashBuf[14] = UploadStartTime.year;    //ä¸ŠæŠ¥èµ·å§‹æ—¶é—´
  FlashBuf[15] = UploadStartTime.month;
  FlashBuf[16] = UploadStartTime.day;
  FlashBuf[17] = UploadStartTime.hour;
  FlashBuf[18] = UploadStartTime.min;
  FlashBuf[19] = UploadStartTime.sec;

  memcpy(FlashBuf+20,DeviceId,7);

  FlashBuf[27] = (unsigned char)(WifiPort / 10000);
  FlashBuf[28] = (unsigned char)(WifiPort % 10000 / 100);
  FlashBuf[29] = (unsigned char)(WifiPort % 100);

  memcpy(FlashBuf+30,WifiDns,30);

  //memcpy(FlashBuf+60,ThermostatId,7);
  FlashBuf[60] = ThermostatNum;       //ÎÂ¿ØÆ÷ÊıÁ¿
  
  for(i=0;i<ThermostatNum;i++)    //ÊıÁ¿Ğ¡ÓÚ5
  {
    memcpy(FlashBuf+61+len,RoomThermostat[i].id,7);
    len += 7;
  }
  
  //=========================================ÎÂ¿ØÆ÷ĞÅÏ¢
  len += 61;   
  FlashBuf[len]   = MainThermostatMode;
  FlashBuf[len+1] = ManualTemper;     
  FlashBuf[len+2] = AntifreezTemper;
  FlashBuf[len+3] = MainThermostatScheduleNum;
 
  for(i=0,j=0;i<MainThermostatScheduleNum*3;i+=3)
  {
    FlashBuf[len+4+i] = MainThermostatSchedule[j].hour;
    FlashBuf[len+5+i] = MainThermostatSchedule[j].min;
    FlashBuf[len+6+i] = MainThermostatSchedule[j].temper;
    j ++;
  }
  //==========ÁãÀäË®²ÎÊı
  len += i + 4;
  memcpy(FlashBuf+len,RevId,7);
  FlashBuf[len+7] = NonColdWateStatus;
  //LLS_Pump.pump_time = ((unsigned int)FlashBuf[len+8] << 8) + FlashBuf[len+9];
  FlashBuf[len+8]  = LLS_Pump.pump_time >> 8;
  FlashBuf[len+9]  = LLS_Pump.pump_time & 0xff;
  FlashBuf[len+10] = LLS_Pump.run_mode;
  FlashBuf[len+11] = LLS_Pump.control_mode;
  FlashBuf[len+12] = LLS_Pump.cuiser_interval;
  FlashBuf[len+13] = LLS_Pump.control_temp;
  FlashBuf[len+14] = LLS_Pump.schedule_num;
  memcpy(FlashBuf+len+15,LLS_Pump.schedule,LLS_Pump.schedule_num*2);
  len += LLS_Pump.schedule_num*2;
  FlashBuf[len+15] = LLS_Pump.ec_num;     //½ÚÄÜÊ±¼ä¶ÎÊıÁ¿
  
  for(i=0;i<LLS_Pump.ec_num*4;i++)
  {
    FlashBuf[len+16+i] = LLS_Pump.ec_time[i];
  }
   
//  FlashBuf[SAVEMAXSIZE-10] =  ThermostatId[0];
//  FlashBuf[SAVEMAXSIZE-9]  =  ThermostatId[1]; 
//  FlashBuf[SAVEMAXSIZE-8]  =  ThermostatId[2]; 
//  FlashBuf[SAVEMAXSIZE-7]  =  ThermostatId[3]; 
//  FlashBuf[SAVEMAXSIZE-6]  =  ThermostatId[4]; 
//  FlashBuf[SAVEMAXSIZE-5]  =  ThermostatId[5]; 
//  FlashBuf[SAVEMAXSIZE-4]  =  ThermostatId[6]; 
//  
//  FlashBuf[SAVEMAXSIZE-3] = ThermEnableFlag;
  
  FlashBuf[SAVEMAXSIZE-18] =  ThermostatId[0];
  FlashBuf[SAVEMAXSIZE-17]  =  ThermostatId[1]; 
  FlashBuf[SAVEMAXSIZE-16]  =  ThermostatId[2]; 
  FlashBuf[SAVEMAXSIZE-15]  =  ThermostatId[3]; 
  FlashBuf[SAVEMAXSIZE-14]  =  ThermostatId[4]; 
  FlashBuf[SAVEMAXSIZE-13]  =  ThermostatId[5]; 
  FlashBuf[SAVEMAXSIZE-12]  =  ThermostatId[6]; 
  FlashBuf[SAVEMAXSIZE-11]  = ThermEnableFlag;
  
  if(XH_PumpFlag > 1)
    XH_PumpFlag = 0;
  FlashBuf[SAVEMAXSIZE-10] = XH_PumpFlag ;
  FlashBuf[SAVEMAXSIZE-9] = XH_PumpId[0]; 
  FlashBuf[SAVEMAXSIZE-8] = XH_PumpId[1];
  FlashBuf[SAVEMAXSIZE-7] = XH_PumpId[2];
  FlashBuf[SAVEMAXSIZE-6] = XH_PumpId[3];
  FlashBuf[SAVEMAXSIZE-5] = XH_PumpId[4];
  FlashBuf[SAVEMAXSIZE-4] = XH_PumpId[5];
  FlashBuf[SAVEMAXSIZE-3] = XH_PumpId[6];
  
  FlashBuf[SAVEMAXSIZE-2] = 0x30;
}

//unsigned char HexToBcd(unsigned char data)
//{
//   return ((data >> 4) * 10 + (data & 0x0f));
//}
//
//unsigned char BcdToHex(unsigned char data)
//{
//   return (((data / 10) << 4) + (data % 10));
//}

//void ReadSaveInfo(void)
//{
//    unsigned char tempbuf[4];
//
//    AT24_readbyte(DataSaveAddr,tempbuf,3);        //è¯»å‡ºå‰4å­—èŠ‚ï¼Œå‰1å­—èŠ‚ï¼Œå­˜å‚¨æ€»æ•°ï¼Œåä¸¤å­—èŠ‚ï¼Œèµ·å§‹åœ°å€ï¼Œé«˜å­—èŠ‚åœ¨å‰
//    delayms(3);
//
//    DataSaveNum = tempbuf[0];
//
//    if(DataSaveNum >= 255)
//    {
//        DataSaveNum = 0x00;
//    }
//
//    CurrSaveAddr = (tempbuf[1] << 8) | tempbuf[2];
//}

void SaveHisData(void)   //å†å²æ•°æ®å­˜å‚¨
{
  unsigned char tempbuf[5];
  unsigned char sdata[45];

  AT24_readbyte(SaveInfoAddr,tempbuf,4);        //è¯»å‡ºå‰4å­—èŠ‚ï¼Œå‰2å­—èŠ‚ï¼Œå­˜å‚¨æ€»æ•°ï¼Œå2å­—èŠ‚ï¼Œèµ·å§‹åœ°å€ï¼Œé«˜å­—èŠ‚åœ¨å‰
  delayms(3);

  DataSaveNum = ((unsigned int)tempbuf[0] << 8) | tempbuf[1];

  if(DataSaveNum >= 255)    //ä¸èƒ½è¶Šç•Œ
  {
     DataSaveNum = 0x00;
     CurrSaveAddr = DataSaveAddr;
  }
  else
  {
    CurrSaveAddr = ((unsigned int)tempbuf[2] << 8) | tempbuf[3];
  }

  sdata[0] = HexToBCD(SysYear);     //é‡‡é›†æ—¶é—´
  sdata[1] = HexToBCD(SysMonth);
  sdata[2] = HexToBCD(SysDay);
  sdata[3] = HexToBCD(SysHour);
  sdata[4] = HexToBCD(SysMinute);
  sdata[5] = HexToBCD(SysSecond);

  sdata[6] = (unsigned char)(WaterTotalVol & 0xFF);
  sdata[7] = (unsigned char)(WaterTotalVol >> 8);
  sdata[8] = (unsigned char)(WaterTotalVol >> 16);
  sdata[9] = (unsigned char)(WaterTotalVol >> 24);

  sdata[10] = (unsigned char)(GasTotalVol & 0xFF);
  sdata[11] = (unsigned char)(GasTotalVol >> 8);
  sdata[12] = (unsigned char)(GasTotalVol >> 16);
  sdata[13] = (unsigned char)(GasTotalVol >> 24);

//    sdata[14] = (unsigned char)(ElecTotalPower & 0x000F);
//    sdata[15] = (unsigned char)(ElecTotalPower >> 8);
//    sdata[16] = (unsigned char)(ElecTotalPower >> 16);
//    sdata[17] = (unsigned char)(ElecTotalPower >> 24);
  sdata[14] = (unsigned char)(PowerUsingTime & 0xFF);
  sdata[15] = (unsigned char)(PowerUsingTime >> 8);
  sdata[16] = (unsigned char)(PowerUsingTime >> 16);
  sdata[17] = (unsigned char)(PowerUsingTime >> 24);

  sdata[18] = Dev_Status[0];
  sdata[19] = Dev_Status[1];
  sdata[20] = ErrorFlag;
  sdata[21] = BathWaterTemp;
  sdata[22] = BathWaterTargetTemp;
  sdata[23] = HeatBathWaterTemp;
  sdata[24] = HeatBathWaterTargetTemp;
  sdata[25] = FlucTemp;
  sdata[26] = CurrFlowRate;

  sdata[27] = DevFacPara[0];
  sdata[28] = DevFacPara[1];
  sdata[29] = DevFacPara[2];
  sdata[30] = DevFacPara[3];
  sdata[31] = MinPower;    //ç”¨ä½œç«åŠ›æ¡£æ•°å€¼
  sdata[32] = RemoteCtrFlag;//MaxPower;
  sdata[33] = LossCommTime;//FirePower;
  sdata[34] = ProValveOut;//ç”¨ä½œå£æŒ‚ç‚‰åŸå§‹ç¯å¢ƒæ¸©åº¦

  sdata[35] = Temp_Compensation_Switch;//UserId;
  sdata[36] = SoftWareVer;//DevSoftVer[0];  é‡‡ç”¨æ¨¡å—çš„è½¯ç¡¬ä»¶ç‰ˆæœ¬å·
  sdata[37] = HardWareVer;//DevSoftVer[1];
  sdata[38] = SensorPara;
  sdata[39] = DevType;

  AT24_writebyte(CurrSaveAddr,sdata,OneFrameLen);
  delayms(3);

  DataSaveNum ++;
  tempbuf[0] = (unsigned char)(DataSaveNum >> 8);    //h byte
  tempbuf[1] = (unsigned char)(DataSaveNum & 0xff);  //l byte

  CurrSaveAddr += OneFrameLen;
  tempbuf[2] = (unsigned char)(CurrSaveAddr >> 8);
  tempbuf[3] = (unsigned char)(CurrSaveAddr & 0xff);

  AT24_writebyte(SaveInfoAddr,tempbuf,4);
  delayms(3);
}



