/*
 * save.c
 *
 *  Created on: 2021年6月4日
 *      Author: WJQ
 */

#include  "boot_24.h"
//#include  "hal_data.h"
unsigned char FlashBuf[SAVEMAXSIZE];

void boot_Delay_Ms(unsigned int tt)
{
  unsigned int i,n;

  for(i=0;i<tt;i++)
  {
      for(n=0;n<1230;n++);
  }
}

void boot_Delay10us(void)
{
  unsigned char i;

  for(i=0;i<5;i++);
}

unsigned char boot_AT24_writebyte(unsigned int Addr, unsigned char *buff, unsigned int num)
{
  unsigned int i;
  unsigned char addr1,addr2;

  addr1 = (unsigned char)(Addr>>8);
  addr2 = (unsigned char)(Addr&0xFF);

  boot_AT24_start();                       //发起始位
  boot_AT24_wbyte(0xa0);                  //发写命令
  boot_AT24_wbyte(addr1);                  //发地址高字节
  boot_AT24_wbyte(addr2);                 //发地址低字节

  for(i=0;i<num;i++)
  {
     ////////解决连写问题////
    if(!((Addr+i)%128))      //  ??(Addr+i)%127
    {
      boot_AT24_stop();
      boot_Delay_Ms(15);
      addr1 = (unsigned char)((Addr+i)>>8);
      addr2 = (unsigned char)((Addr+i)&0xFF);
      boot_AT24_start();                      //重新发启始位
      boot_AT24_wbyte(0xa0);                  //重新发伪写命令
      boot_AT24_wbyte(addr1);                 //重新发地址高字节
      boot_AT24_wbyte(addr2);                 //重新发地址低字节
    }

    if(boot_AT24_wbyte(buff[i]))                    //发需写的数据
      return 1;
  }

  boot_AT24_stop();                          //发停止位
  boot_Delay_Ms(1);                           //写完成后稍微延时，保证写入正确
  return 0;
}

unsigned char boot_AT24_readbyte(unsigned int Addr, unsigned char* buff, unsigned int num)
{
  unsigned int i;
  unsigned char addr1,addr2;

  addr1 = (unsigned char)(Addr/256);
  addr2 = (unsigned char)(Addr%256);
  boot_AT24_start();                      //发启始位
  boot_AT24_wbyte(0xa0);                  //发伪写命令
  boot_AT24_wbyte(addr1);                 //发地址高字节
  boot_AT24_wbyte(addr2);                 //发地址低字节
  boot_AT24_start();                      //发启始位
  boot_AT24_wbyte(0xa1);                  //发读命令

  for(i=0;i<num;i++)
  {
    buff[i] = boot_AT24_rbyte();          //从地址中读一个字节到变量bytetemp中
    if(i == (num-1))
       boot_AT24_bitout(1);
    else
       boot_AT24_bitout(0);

    SDA_SET_IN;                       //SDA_DIR &= ~SDA_BIT;
  }

  boot_AT24_stop();                       //发停止位

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
//  AT24_start();                       //发起始位
//  AT24_wbyte(0xa0);                  //发写命令
//  AT24_wbyte(addr1);                  //发地址高字节
//  AT24_wbyte(addr2);                 //发地址低字节
//
//  for(i=0;i<num;i++)
//  {
//     ////////解决连写问题////
//    if(!((Addr+i)%128))      //  ??(Addr+i)%127
//    {
//      AT24_stop();
//      delayms(10);
//      addr1 = (unsigned char)((Addr+i)>>8);
//      addr2 = (unsigned char)((Addr+i)&0xFF);
//      AT24_start();                      //重新发启始位
//      AT24_wbyte(0xa0);                  //重新发伪写命令
//      AT24_wbyte(addr1);                 //重新发地址高字节
//      AT24_wbyte(addr2);                 //重新发地址低字节
//    }
//
//    if(AT24_wbyte(buff[i]))                    //发需写的数据
//      return 1;
//  }
//
//  AT24_stop();                          //发停止位
//  delayms(1);                           //写完成后稍微延时，保证写入正确
//  return 0;
//}

unsigned char boot_AT24_rbyte(void)
{
  unsigned char bittemp,bytetemp,i;
  bytetemp = 0;

  for (i=0; i<8; i++)
  {
    bytetemp <<= 1;

    bittemp = boot_AT24_bitin();
    if (bittemp)
      bytetemp |= 0x01;
  }

  return bytetemp;
}

unsigned char boot_AT24_wbyte(unsigned char ch)
{
  int i;
  unsigned char mask;

  mask = 0x80;

  for (i=0; i<8; i++)
  {
    if (ch & mask)
      boot_AT24_bitout(1);
    else
      boot_AT24_bitout(0);

    mask >>= 1;
  }

  if(boot_AT24_bitin())
    return 1;
  return 0;
}

void boot_AT24_start(void)
{
  SDA_SET_OUT;                //SDA_DIR |= SDA_BIT;

  SCL_SET_OUT;                //SCL_DIR |= SCL_BIT;
  boot_Delay10us();
  SCL_HIGH;                   //SCL_OUT |= SCL_BIT;
  boot_Delay10us();
  SDA_HIGH;                   //SDA_OUT |= SDA_BIT;
  boot_Delay10us();
  SDA_LOW;                    //SDA_OUT &= ~SDA_BIT;
  boot_Delay10us();
  SCL_LOW;                    //SCL_OUT &= ~SCL_BIT;
  boot_Delay10us();
}

void boot_AT24_stop(void)
{
  SDA_SET_OUT;                //SDA_DIR |= SDA_BIT;

  SCL_SET_OUT;                //SCL_DIR |= SCL_BIT;
  boot_Delay10us();
  SDA_LOW;                    //SDA_OUT &= ~SDA_BIT;
  boot_Delay10us();
  SCL_HIGH;                   //SCL_OUT |= SCL_BIT;
  boot_Delay10us();
  SDA_HIGH;                   //SDA_OUT |= SDA_BIT;
  boot_Delay10us();
}

void boot_AT24_bitout(unsigned char ch)
{
  SDA_SET_OUT;                 //SDA_DIR |= SDA_BIT;

  SCL_SET_OUT;                 //SCL_OUT &= ~SCL_BIT;

  boot_Delay10us();
  if (ch)
    SDA_HIGH;                  //SDA_OUT |= SDA_BIT;
  else
    SDA_LOW;                   //SDA_OUT &= ~SDA_BIT;

  boot_Delay10us();
  SCL_HIGH;                    //SCL_OUT |= SCL_BIT;
  boot_Delay10us();
  SCL_LOW;                     //SCL_OUT &= ~SCL_BIT;
  boot_Delay10us();
}

unsigned char boot_AT24_bitin(void)
{
  //unsigned char ch;
  bsp_io_level_t  level;

  SDA_SET_IN;                   //SDA_DIR &= ~SDA_BIT;
  boot_Delay10us();
  SCL_LOW;                      //SCL_OUT &= ~SCL_BIT;
  boot_Delay10us();
  SCL_HIGH;                     //SCL_OUT |= SCL_BIT;
  boot_Delay10us();

  //ch = SDA_IN;

  R_IOPORT_PinRead(&g_ioport_ctrl, BSP_IO_PORT_00_PIN_13, &level);

  SCL_LOW;                      //SCL_OUT &= ~SCL_BIT;
  boot_Delay10us();

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

/*间隔
 7  6   5   4   3   2  | 1   0   BIT
时间数值    时间单位    |
范围：1----31          | 0：min  1：Hour    2：Day

 */
//void InitData(void)
//{
//    memset(DeviceId,0x88,7);
//
//    UpLoadMode    = 0x01;
//    UpLoadCycle   = 0x01;     //1h
//    UpLoadCycleUint = 0x02;
//    UpLoadYear    = 21;
//    UpLoadMonth   = 6;
//    UpLoadDay     = 8;
//    UpLoadHour    = 9;
//    UpLoadMin     = 0;
//    UpLoadSec     = 0;
//    LinkDelayTime = 20;
//    DataSaveCycle = 0x01;     //1h
//    DataSaveCycleUint = 0x02;
//
//    GroupNo  = 0x01;
//    PeakTime = 0x3C;    //60
//
//    UploadStartTime.year = 21;    //上报起始时间 2021年6月1日8:00:00
//    UploadStartTime.month = 6;
//    UploadStartTime.day = 1;
//    UploadStartTime.hour = 8;
//    UploadStartTime.min = 0;
//    UploadStartTime.sec = 0;
//
//    WifiPort = 5683;
//    memset(WifiDns,0,30);
//    sprintf((char *)WifiDns,"117.60.157.137");
//}
/*
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
 // check = 0x00;
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
    InitData();
    SavePara();
  }
}

void SavePara(void)
{
    Para_To_Buf();
    SaveData();
}
*/
//void Buf_To_Para(void)
//{
//  UpLoadMode    = FlashBuf[0];
//  UpLoadCycle   = FlashBuf[1];
//  UpLoadCycleUint = FlashBuf[2];
//  UpLoadYear    = FlashBuf[3];
//  UpLoadMonth   = FlashBuf[4];
//  UpLoadDay     = FlashBuf[5];
//  UpLoadHour    = FlashBuf[6];
//  UpLoadMin     = FlashBuf[7];
//  UpLoadSec     = FlashBuf[8];
//  LinkDelayTime = FlashBuf[9];
//  DataSaveCycle = FlashBuf[10];
//  DataSaveCycleUint = FlashBuf[11];
//
//  GroupNo  = FlashBuf[12];
//  PeakTime = FlashBuf[13];
//
//  UploadStartTime.year  = FlashBuf[14];    //上报起始时间
//  UploadStartTime.month = FlashBuf[15];
//  UploadStartTime.day   = FlashBuf[16];
//  UploadStartTime.hour  = FlashBuf[17];
//  UploadStartTime.min   = FlashBuf[18];
//  UploadStartTime.sec   = FlashBuf[19];
//
//  memcpy(DeviceId,FlashBuf+20,7);
//
//  WifiPort  = (unsigned int)FlashBuf[27] * 10000 + (unsigned int)FlashBuf[28] * 100 + FlashBuf[29];
//  memset((char*)WifiDns,0x00,30);
//  memcpy((char*)WifiDns,FlashBuf+30,30);
//}
//
//void Para_To_Buf(void)
//{
//  memset(FlashBuf,0,SAVEMAXSIZE);
//
//  FlashBuf[0] = UpLoadMode;
//  FlashBuf[1] = UpLoadCycle;
//  FlashBuf[2] = UpLoadCycleUint;
//  FlashBuf[3] = UpLoadYear;
//  FlashBuf[4] = UpLoadMonth;
//  FlashBuf[5] = UpLoadDay;
//  FlashBuf[6] = UpLoadHour;
//  FlashBuf[7] = UpLoadMin;
//  FlashBuf[8] = UpLoadSec ;
//  FlashBuf[9] = LinkDelayTime;
//  FlashBuf[10] = DataSaveCycle;
//  FlashBuf[11] = DataSaveCycleUint;
//
//  FlashBuf[12] = GroupNo;
//  FlashBuf[13] = PeakTime;
//
//  FlashBuf[14] = UploadStartTime.year;    //上报起始时间
//  FlashBuf[15] = UploadStartTime.month;
//  FlashBuf[16] = UploadStartTime.day;
//  FlashBuf[17] = UploadStartTime.hour;
//  FlashBuf[18] = UploadStartTime.min;
//  FlashBuf[19] = UploadStartTime.sec;
//
//  memcpy(FlashBuf+20,DeviceId,7);
//
//  FlashBuf[27] = (unsigned char)(WifiPort / 10000);
//  FlashBuf[28] = (unsigned char)(WifiPort % 10000 / 100);
//  FlashBuf[29] = (unsigned char)(WifiPort % 100);
//
//  memcpy(FlashBuf+30,WifiDns,30);
//
//  FlashBuf[SAVEMAXSIZE-2] = 0x30;
//}

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
//    AT24_readbyte(DataSaveAddr,tempbuf,3);        //读出前4字节，前1字节，存储总数，后两字节，起始地址，高字节在前
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

//void SaveHisData(void)   //历史数据存储
//{
//    unsigned char tempbuf[5];
//    unsigned char sdata[45];
//
//    AT24_readbyte(DataSaveAddr,tempbuf,3);        //读出前4字节，前1字节，存储总数，后两字节，起始地址，高字节在前
//    delayms(3);
//
//    DataSaveNum = (tempbuf[0] << 8) | tempbuf[1];
//
//    if(DataSaveNum >= 255)
//    {
//       DataSaveNum = 0x00;
//    }
//
//    CurrSaveAddr = (tempbuf[2] << 8) | tempbuf[3];
//
//    sdata[0] = HexToBCD(SysYear);     //采集时间
//    sdata[1] = HexToBCD(SysMonth);
//    sdata[2] = HexToBCD(SysDay);
//    sdata[3] = HexToBCD(SysHour);
//    sdata[4] = HexToBCD(SysMinute);
//    sdata[5] = HexToBCD(SysSecond);
//
//    sdata[6] = (unsigned char)(WaterTotalVol & 0x000F);
//    sdata[7] = (unsigned char)(WaterTotalVol >> 8);
//    sdata[8] = (unsigned char)(WaterTotalVol >> 16);
//    sdata[9] = (unsigned char)(WaterTotalVol >> 24);
//
//    sdata[10] = (unsigned char)(GasTotalVol & 0x000F);
//    sdata[11] = (unsigned char)(GasTotalVol >> 8);
//    sdata[12] = (unsigned char)(GasTotalVol >> 16);
//    sdata[13] = (unsigned char)(GasTotalVol >> 24);
//
//    sdata[14] = (unsigned char)(ElecTotalPower & 0x000F);
//    sdata[15] = (unsigned char)(ElecTotalPower >> 8);
//    sdata[16] = (unsigned char)(ElecTotalPower >> 16);
//    sdata[17] = (unsigned char)(ElecTotalPower >> 24);
//
//    sdata[18] = Dev_Satus[0];
//    sdata[19] = Dev_Satus[1];
//    sdata[20] = ErrorFlag;
//    sdata[21] = BathWaterTemp;
//    sdata[22] = BathWaterTargetTemp;
//    sdata[23] = HeatBathWaterTemp;
//    sdata[24] = HeatBathWaterTargetTemp;
//    sdata[25] = FlucTemp;
//    sdata[26] = CurrFlowRate;
//
//    sdata[27] = DevFacPara[0];
//    sdata[28] = DevFacPara[1];
//    sdata[29] = DevFacPara[2];
//    sdata[30] = DevFacPara[3];
//    sdata[31] = MinPower;
//    sdata[32] = MaxPower;
//    sdata[33] = FirePower;
//    sdata[34] = ProValveOut;
//
//    sdata[35] = UserId;
//    sdata[36] = DevSoftVer[0];
//    sdata[37] = DevSoftVer[1];
//    sdata[38] = SensorPara;
//    sdata[39] = DevType;
//
//    AT24_writebyte(CurrSaveAddr,sdata,OneFrameLen);
//    delayms(3);
//
//    DataSaveNum ++;
//    tempbuf[0] = DataSaveNum >> 8;
//    tempbuf[1] = DataSaveNum & 0xff;
//
//    CurrSaveAddr += OneFrameLen;
//    tempbuf[2] = CurrSaveAddr >> 8;
//    tempbuf[3] = CurrSaveAddr & 0xff;
//
//    AT24_writebyte(SaveInfoAddr,tempbuf,4);
//    delayms(3);
//}



