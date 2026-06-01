/*
 * NetDataPro.c
 *
 *  Created on: 2021年6月10日
 *      Author: WJQ
 */

#include  "../usr_lib/datapro.h"
#include  "stdbool.h"
extern void PowerOn_HanShake(void);
//unsigned int ShortCount;
unsigned char calSum(unsigned char *buf,unsigned char len)
{
  unsigned char i,check;

  check = 0x00;

  for(i=0;i<len;i++)
  {
    check += buf[i];
  }

  return check;
}

unsigned char calXOR(unsigned char *buf,unsigned char len)
{
  unsigned char i,check;

  check = 0x00;

  for(i=0;i<len;i++)
  {
      check ^= buf[i];
  }

  return check;
}

unsigned char HexToBCD(unsigned char data)
{
  unsigned char re;

  re = ((data / 10) << 4) | (data % 10);

  return re;
}

unsigned char BCDToHex(unsigned char data)
{
  unsigned char re;

  re = ((data >> 4) * 10) + (data & 0x0f);

  return re;
}


//=================================================================CRC校验
unsigned int calccrc(unsigned char crcbuf,unsigned int crc)
{
  unsigned char i,chk;

  chk = 0;
  crc = crc ^ crcbuf;

  for(i=0;i<8;i++)
  {
    chk = crc & 1;
    crc = crc >> 1;
    crc = crc & 0x7fff;
    if(chk == 1)
      crc = crc ^ 0xa001;
    crc = crc & 0xffff;
  }

  return crc;
}

unsigned int chkcrc(unsigned char *buf,unsigned int len)
{
  unsigned char hi,lo;
  unsigned int i;
  unsigned int crc;

  crc = 0xFFFF;

  for(i=0;i<len;i++)
  {
    crc = calccrc(*buf,crc);
    buf ++;
  }

  hi = (unsigned char)(crc / 256);
  lo = (unsigned char)(crc % 256);

  crc = (((unsigned int)hi + 0x0000) << 8) | lo;

  return crc;
}
//=================================================================

unsigned char SendNbRegFrame(void)      //power on reg 上电注册
{
  unsigned char re = 0x00;
  unsigned int crccheck;

  TxBuffer[0] = 0x68;
  TxBuffer[1] = 0x86;

  memcpy(TxBuffer+2,DeviceId,7);

  TxBuffer[9] = 0x01;

  TxBuffer[10] = 0x24;      //len=45
  TxBuffer[11] = 0x00;

  TxBuffer[12] = 0xF0;      //
  TxBuffer[13] = 0x0D;

  TxBuffer[14] = 0x00;      //ser

  TxBuffer[15] = FacCode;

  TxBuffer[16] = GasHeater; //type

  memcpy(TxBuffer+17,Iccid,20);

  memcpy(TxBuffer+37,Imei,8);

  TxBuffer[45] = SoftWareVer;
  TxBuffer[46] = HardWareVer;
  /*
  TxBuffer[46] = ECL;

  TxBuffer[47] = RSRP[0];
  TxBuffer[48] = RSRP[1];

  TxBuffer[49] = RSSI[0];
  TxBuffer[50] = RSSI[1];

  TxBuffer[51] = SNR[0];
  TxBuffer[52] = SNR[1];

  TxBuffer[53] = CELL_ID[0];
  TxBuffer[54] = CELL_ID[1];
  TxBuffer[55] = CELL_ID[2];
  TxBuffer[56] = CELL_ID[3];

  TxBuffer[57] = PCI[0];
  TxBuffer[58] = PCI[1];

  TxBuffer[59] = EARFCN[0];
  TxBuffer[60] = EARFCN[1];
  crccheck = chkcrc(TxBuffer,61);

  TxBuffer[61] = (unsigned char)(crccheck & 0xFF);
  TxBuffer[62] = (unsigned char)(crccheck >> 8);

  TxBuffer[63] = 0x16;

  re = SendNbData(TxBuffer,64);

*/
  TxBuffer[47] = Csq;
  crccheck = chkcrc(TxBuffer,48);

  TxBuffer[48] = (unsigned char)(crccheck & 0xFF);
  TxBuffer[49] = (unsigned char)(crccheck >> 8);

  TxBuffer[50] = 0x16;

  re = SendNbData(TxBuffer,51);

  return re;
}

unsigned char SendHeartBeatFrame(void)   //��������
{
  unsigned char re = 0x00;
  unsigned int crccheck;
  
  TxBuffer[0] = 0x68;
  TxBuffer[1] = 0x86;

  memcpy(TxBuffer+2,DeviceId,7);

  TxBuffer[9] = 0x01;

  TxBuffer[10] = 0x05;      //len=49
  TxBuffer[11] = 0x00;

  TxBuffer[12] = 0xF0;      //
  TxBuffer[13] = 0x0E;

  TxBuffer[14] = 0x00;      //ser

  TxBuffer[15] = FacCode;

  TxBuffer[16] = GasHeater; //type
  
  crccheck = chkcrc(TxBuffer,17);

  TxBuffer[17] = (unsigned char)(crccheck & 0xFF);
  TxBuffer[18] = (unsigned char)(crccheck >> 8);

  TxBuffer[19] = 0x16;

  re = SendNbData(TxBuffer,20);

  return re;
}

unsigned char SendUpLoadFrame(void)
{
  unsigned char i;
  unsigned char re = 0x00,num;
  unsigned int crccheck,tempaddr;
  unsigned char len;
  unsigned char tempbuf[5];

    /////////////////////////////////////////////////////从24中读取数据循环发送
  if(DataSaveNum > 0)
  {
    if(DataSaveNum % 3)    //保存条数不是3的倍数
    {
      num = DataSaveNum / 3 + 1;
    }
    else                  //是3的倍数
    {
      num = DataSaveNum / 3;
    }

    tempaddr = DataSaveAddr;

    for(i=0;i<num;i++)
    {
      TxBuffer[0] = 0x68;
      TxBuffer[1] = 0x86;

      memcpy(TxBuffer+2,DeviceId,7);

      TxBuffer[9]  = 0x01;

      TxBuffer[12] = 0xF0;      //
      TxBuffer[13] = 0x08;
      TxBuffer[14] = FrameSer ++;//0x00;      //ser

      TxBuffer[15] = FacCode;

      TxBuffer[16] = GasHeater; //type

      TxBuffer[17] = Csq;

      if(DataSaveNum <= 0x03)     //一帧数据最多包含3包
      {
        TxBuffer[18] = 0x88;    //上报结束
        TxBuffer[19] = (unsigned char)DataSaveNum;    //包含一帧数据
        DataSaveNum = 0x00;
      }
      else                       //超过3帧数据就一次发3帧
      {
        TxBuffer[18] = 0x99;
        TxBuffer[19] = 0x03;
        DataSaveNum -= 0x03;
      }

      len = TxBuffer[19] * OneFrameLen;

      TxBuffer[10] = (len + 6) & 0xFF;
      TxBuffer[11] = (len + 6) >> 8;

      AT24_readbyte(tempaddr, TxBuffer+20, len);  //从开始读取

      tempaddr += len;

      crccheck = chkcrc(TxBuffer,20 + len);

      TxBuffer[20 + len] = (unsigned char)(crccheck & 0xFF);
      TxBuffer[21 + len] = (unsigned char)(crccheck >> 8);

      TxBuffer[22 + len] = 0x16;

      re = SendNbData(TxBuffer,23 + len);
      delayms(50);
    }

    tempbuf[0] = 0x00;
    tempbuf[1] = 0x00;

    tempbuf[2] = DataSaveAddr >> 8;
    tempbuf[3] = DataSaveAddr & 0xff;

    AT24_writebyte(SaveInfoAddr,tempbuf,4);
    delayms(10);
    //AT24_readbyte(SaveInfoAddr,tempbuf,4);
    //delayms(3);
  }
  else
  {
    TxBuffer[0] = 0x68;
    TxBuffer[1] = 0x86;

    memcpy(TxBuffer+2,DeviceId,7);

    TxBuffer[9]  = 0x01;

    TxBuffer[10] = 0x30;      //len
    TxBuffer[11] = 0x00;

    TxBuffer[12] = 0xF0;      //
    TxBuffer[13] = 0x08;
    TxBuffer[14] = 0x00;      //ser

    TxBuffer[15] = FacCode;

    TxBuffer[16] = GasHeater; //type

    TxBuffer[17] = Csq;

    TxBuffer[18] = 0x88;      //上报结束
    TxBuffer[19] = 0x01;      //

    TxBuffer[20] = HexToBCD(SysYear);
    TxBuffer[21] = HexToBCD(SysMonth);
    TxBuffer[22] = HexToBCD(SysDay);
    TxBuffer[23] = HexToBCD(SysHour);
    TxBuffer[24] = HexToBCD(SysMinute);
    TxBuffer[25] = HexToBCD(SysSecond);
//    TxBuffer[20] = SysYear;
//    TxBuffer[21] = SysMonth;
//    TxBuffer[22] = SysDay;
//    TxBuffer[23] = SysHour;
//    TxBuffer[24] = SysMinute;
//    TxBuffer[25] = SysSecond;

    TxBuffer[26] = (unsigned char)(WaterTotalVol & 0xFF);
    TxBuffer[27] = (unsigned char)(WaterTotalVol >> 8);
    TxBuffer[28] = (unsigned char)(WaterTotalVol >> 16);
    TxBuffer[29] = (unsigned char)(WaterTotalVol >> 24);

    TxBuffer[30] = (unsigned char)(GasTotalVol & 0xFF);
    TxBuffer[31] = (unsigned char)(GasTotalVol >> 8);
    TxBuffer[32] = (unsigned char)(GasTotalVol >> 16);
    TxBuffer[33] = (unsigned char)(GasTotalVol >> 24);

//        TxBuffer[34] = (unsigned char)(ElecTotalPower & 0x000F);
//        TxBuffer[35] = (unsigned char)(ElecTotalPower >> 8);
//        TxBuffer[36] = (unsigned char)(ElecTotalPower >> 16);
//        TxBuffer[37] = (unsigned char)(ElecTotalPower >> 24);
    TxBuffer[34] = (unsigned char)(PowerUsingTime & 0xFF);
    TxBuffer[35] = (unsigned char)(PowerUsingTime >> 8);
    TxBuffer[36] = (unsigned char)(PowerUsingTime >> 16);
    TxBuffer[37] = (unsigned char)(PowerUsingTime >> 24);

    TxBuffer[38] = Dev_Status[0];
    TxBuffer[39] = Dev_Status[1];
    
    TxBuffer[40] = ErrorFlag;
    
    TxBuffer[41] = BathWaterTemp;
    TxBuffer[42] = BathWaterTargetTemp;
    TxBuffer[43] = HeatBathWaterTemp;
    TxBuffer[44] = HeatBathWaterTargetTemp;
    TxBuffer[45] = FlucTemp;
    TxBuffer[46] = CurrFlowRate;

    TxBuffer[47] = DevFacPara[0];      //BIT2--0:暖气片  1：地板模式
    TxBuffer[48] = DevFacPara[1];
    TxBuffer[49] = DevFacPara[2];
      
    TxBuffer[50] = DevFacPara[3];
      
    if(alarm_sta & ALARM_ICON)      //有报警数据
    {
      TxBuffer[50] = 0xAA;
    }
  
    TxBuffer[51] = MinPower;  //火力档
    TxBuffer[52] = RemoteCtrFlag;//MaxPower;远程锁定
    TxBuffer[53] = LossCommTime;//FirePower;
    TxBuffer[54] = ProValveOut; //已被占用。串口数据

    TxBuffer[55] = Temp_Compensation_Switch;//UserId;
    TxBuffer[56] = SoftWareVer;//DevSoftVer[0];  使用模块的软硬件版本号
    TxBuffer[57] = HardWareVer;//DevSoftVer[1];
    TxBuffer[58] = SensorPara;
    TxBuffer[59] = DevType;

    crccheck = chkcrc(TxBuffer,60);

    TxBuffer[60] = (unsigned char)(crccheck & 0xFF);
    TxBuffer[61] = (unsigned char)(crccheck >> 8);

    TxBuffer[62] = 0x16;

    re = SendNbData(TxBuffer,63);
  }
  SavingTime = 0;
  //��ʼ�ϱ��¿�����Ϣ
  GetThermInfoFlag = 1;
  CurrThermNo = 0;
  return re;
}

unsigned char SendThermostatData(void)
{
  unsigned int re,crccheck,len;

  TxBuffer[0] = 0x68;
  TxBuffer[1] = 0x86;

  for(re=0;re<7;re++)
  {
    TxBuffer[re+2] = DeviceId[re];
  }

  TxBuffer[9] = 0x84;

  len = Thermo_Buf[0];
  if(len < 12 || len >= 60)
  {
    return 0;
  }
  TxBuffer[10] = (len+ 5) & 0xff;
  TxBuffer[11] = (len+ 5) >> 8;

  TxBuffer[12] = 0xF0;
  TxBuffer[13] = 0x0C;
  TxBuffer[14] = ThermostatControlSer;
  TxBuffer[15] = FacCode;
  TxBuffer[16] = GasHeater;

  memcpy(TxBuffer+17,Thermo_Buf+1,len);

  crccheck = chkcrc(TxBuffer,len+17);

  TxBuffer[len+17] = (unsigned char)(crccheck & 0xFF);
  TxBuffer[len+18] = (unsigned char)(crccheck >> 8);

  TxBuffer[len+19] = 0x16;

  re = SendNbData(TxBuffer,len + 20);
  return re;
}

unsigned char SendParaSettingAck(void)     //设置命令的ack
{
  unsigned char re = 0x00;
  unsigned int crccheck;
  unsigned char xbuf[128];

  xbuf[0] = 0x68;
  xbuf[1] = 0x86;

  memcpy(xbuf+2,DeviceId,7);

  xbuf[9] = 0x84;

  xbuf[10] = 0x05;      //len
  xbuf[11] = 0x00;

  xbuf[12] = 0xF0;      //
  xbuf[13] = 0x09;

  xbuf[14] = FrameSer;      //ser

  xbuf[15] = FacCode;

  xbuf[16] = GasHeater; //type

//  memcpy(xbuf+17,Thermo_Buf,HeartBeatLength);
//
//  crccheck = chkcrc(xbuf,17+HeartBeatLength);
//
//  xbuf[17+HeartBeatLength] = (unsigned char)(crccheck & 0xFF);
//  xbuf[18+HeartBeatLength] = (unsigned char)(crccheck >> 8);
//
//  xbuf[19+HeartBeatLength] = 0x16;
//  re = SendNbData(xbuf,20+HeartBeatLength);

  crccheck = chkcrc(xbuf,17);

  xbuf[17] = (unsigned char)(crccheck & 0xFF);
  xbuf[18] = (unsigned char)(crccheck >> 8);

  xbuf[19] = 0x16;
  re = SendNbData(xbuf,20);
  return re;
}

unsigned int ProNbData(void)    //接收到的数据处理
{
  unsigned int cid = 0;
  unsigned int i;//len;
  //unsigned int index;
  unsigned int crc;

  //re = 0x00;
    //ģ��ֱ��ģʽ���ݴ���
  if(RevChar == 4)         //有上位机数据
  {
    RevChar = 0;

//    NetDelayTime = 0;
//    while(!g_uart9_recv_complete)  //判断数据接收完成
//    {
//	if(NetDelayTime >= 5)    //5s超时
//	{
//	    return 0;
//	}
//    }
//    g_uart9_recv_complete = false;
/*
    delayms(100);         //延时等待数据接收完成
    index = 0;

    for(i=0;i<50;i++)
    {
      if(USCIA0RXBUF[i] == 0x0D && USCIA0RXBUF[i+1] == 0x0A)   //先检测到‘：’，后边紧跟着数据长度字节，到第一个','截止
      {
        index -= 0x04;   //ȥ����������ǰ�ߵ��ֽڸ�����ʣ�µľͱ�ʾ��λ��
        break;
      }

      index ++;
//      if(USCIA0RXBUF[i] == ',')
//      {
//        index = i - 1 - index;
//        break;
//      }
    }

    if(!index || index > 3)
    {
      Nb_ModuleClose();
      return 0;
    }

    if(index == 1)
    {
      len = USCIA0RXBUF[i-1] - 0x30;
    }
    else if(index == 2)
    {
      len = (USCIA0RXBUF[i-2] - 0x30) * 10 + (USCIA0RXBUF[i-1] - 0x30);
    }
    else if(index == 3)
    {
      len = (USCIA0RXBUF[i-3] - 0x30) * 100 + (USCIA0RXBUF[i-2] - 0x30) * 10 + (USCIA0RXBUF[i-1] - 0x30);
    }

    if(len > UARTPOINTNUM)    //限制数据长度
    {
      Nb_ModuleClose();
      return 0;
    }
    
    index = i + 2;   //ʵ�����ݵ���ʼλ��
    memset(TxBuffer,0,UARTPOINTNUM);
    memcpy(TxBuffer,USCIA0RXBUF+index,len);
*/
    memcpy(TxBuffer,USCIA0RXBUF,PackLen);
    //memcpy(TxBuffer,USCIA0RXBUF+i+2,len*2);
    //StringToHex(TxBuffer,len);
//数据格式判断68 86 88 88 88 88 88 88 88 04 09 00 F0 0A 23 FD 01 00 50 03 12 8D 78 16
    if(TxBuffer[0] != 0x68 || TxBuffer[1] != 0x86)
    {
      return 0;
    }

    for(i=0;i<7;i++)
    {
      if(TxBuffer[i+2] != DeviceId[i])
      {
        return 0;
      }
    }

    //memset(USCIA0RXBUF,0,UARTPOINTNUM);

    crc = chkcrc(TxBuffer,PackLen-3);

    if((TxBuffer[PackLen-3] != (unsigned char)crc) || (TxBuffer[PackLen-2] != (crc >> 8)))
    {
      return 0;
    }

    if(TxBuffer[PackLen-1] != 0x16)
    {
      return 0;
    }

    ConnectHoldTime = 0x00;

    cid = ((unsigned int)TxBuffer[12] << 8) + TxBuffer[13];

    return cid;
  }
  
  return 0;
}

unsigned char SendRequestFrame(unsigned char flag)
{
  unsigned char i,re;
  unsigned int crc16;
  unsigned char num = 0x00;
  unsigned char txbuf[64];

  re = 0x00;

  UpgradeFlag = 0x01;
  UpgradeOverTime = 0x00;

  txbuf[0] = 0x68;
  txbuf[1] = 0x86;

  for(i=0;i<7;i++)
  {
      txbuf[2+i] = DeviceId[i];
  }

  txbuf[9] = 0x04;
//===============================  数据长度，固定10字节
  txbuf[10] = 0x0A;
  txbuf[11] = 0x00;
//================================
  txbuf[12] = 0xf0;
  txbuf[13] = 0x0b;

  txbuf[14] = ++FrameSer;
  txbuf[15] = FacCode;
  txbuf[16] = DevType;

  txbuf[17] = HexToBCD((unsigned char)(TotalNum / 100));
  txbuf[18] = HexToBCD((unsigned char)(TotalNum % 100));

  txbuf[19] = HexToBCD((unsigned char)(CurrFrameSer / 100));
  txbuf[20] = HexToBCD((unsigned char)(CurrFrameSer % 100));
//=================================================
//    if(CurrFrameSer == 1)
//      {
//	delayms(1);
//      }
//=================================================
  if(flag)     //错误升级
  {
    txbuf[21] = 0x77;
    re = 0x77;
    UpgradeFlag = 0x00;
    UpgradeNum = 0x00;
    StatusBuf[11] = 'O';     //程序升级报错，认为升级成功，下次从头开始
    StatusBuf[12] = 'K';
    AT24_writebyte(USER_PARA_OFFSET, StatusBuf, 13);
    delayms(3);
  }
  else
  {
	  if(CurrFrameSer > TotalNum)    //此时升级帧序号已超过总包数
    {
      txbuf[21] = 0x88;
      re = 0x88;
        //return 0x88;        //结束
    }
    else
    {
      txbuf[21] = 0x99;
      re = 0x99;
    }
  }

  crc16 = chkcrc(txbuf,22);

  txbuf[22] = (unsigned char)(crc16 >> 8);
  txbuf[23] = (unsigned char)(crc16 & 0xff);

  txbuf[24] = 0x16;

  for(num=0;num<2;num++)      //若失败，延时3s再发送1次
  {
    if(SendNbData(txbuf,25))
    {
        break;
    }
    delayms(3000);
  }
  GreenLed_ON;
  return re;
}

void ProcessRevData(void)
{
  unsigned int i,j,re = 0x00;
  unsigned int cid;
  unsigned char rxbuf[150],txbuf[255];
  unsigned char xorcheck;
  unsigned int tempser;
  unsigned char t1,t2;
  unsigned int tempaddr,len,crc;
  unsigned char ansflag = 0;
#ifdef TRANS_MODE
  if(!ComValid)
    return;
  
  ComValid = 0x00;
  memcpy(TxBuffer,USCIA0RXBUF,USCIA0RXNum);
  
//  if(TxBuffer[9] & 0x80)   //Ӧ��֡��������
//  {
//    USCIA0RXNum = 0;
//    return;
//  }
  
  if(!ProtocolFlag)    //�ڹ�¯Э��
  {
    cid = ((unsigned int)TxBuffer[12] << 8) | TxBuffer[13];
  }
  else               //�¿���Э��
  {
    cid = ((unsigned int)TxBuffer[11] << 8) | TxBuffer[12];
  }
  
  ProtocolFlag = 0x00;
   
#ifdef NET_DELAY_TEST
  SendNbData(USCIA0RXBUF,USCIA0RXNum); 
  SendNbData("\nNet_Rev:",9); 
  SendNbData(TxBuffer+11,2);   
#endif
    
#else
  cid = ProNbData();
#endif
 
  switch(cid)
  {
  case 0xF00D://0xF001:     //收到注册数据的应答
    SysYear   = BCDToHex(TxBuffer[17]);    //系统时间
    SysMonth  = BCDToHex(TxBuffer[18]);
    SysDay    = BCDToHex(TxBuffer[19]);
    SysHour   = BCDToHex(TxBuffer[20]);
    SysMinute = BCDToHex(TxBuffer[21]);
    SysSecond = BCDToHex(TxBuffer[22]);
   // QK_SetSignleRegister(StoveTime__offset,((int)SysHour << 8) + SysMinute);  //更新壁挂炉时间
    DateUpdateFlag = 0x01;
    SetTimeFlag = 0x01;

    GroupNo  = TxBuffer[23];               //组号
    PeakTime = TxBuffer[24];
    TimingSaveFlag = 0x01;
    re = SendUpLoadFrame();
    //delayms(50);
    //SendThermostatData();
    //ReportThermostatInfo(1);
    QK_SetSignleRegister(D_Status,alarm_sta | NET_ICON);
    break;
  case 0xF00E:
    SysYear   = BCDToHex(TxBuffer[17]);    //系统时间
    SysMonth  = BCDToHex(TxBuffer[18]);
    SysDay    = BCDToHex(TxBuffer[19]);
    SysHour   = BCDToHex(TxBuffer[20]);
    SysMinute = BCDToHex(TxBuffer[21]);
    SysSecond = BCDToHex(TxBuffer[22]);
    DateUpdateFlag = 0x01;
    SetTimeFlag = 0x01;
    HeartBeatSendFlag = 0x00;
    HeartBeatSendNum = 0x00;
    break;
  case 0xF008:
    UploadStartTime.year  = BCDToHex(TxBuffer[17]);    //上报起始时间
    UploadStartTime.month = BCDToHex(TxBuffer[18]);
    UploadStartTime.day   = BCDToHex(TxBuffer[19]);
    UploadStartTime.hour  = BCDToHex(TxBuffer[20]);
    UploadStartTime.min   = BCDToHex(TxBuffer[21]);
    UploadStartTime.sec   = BCDToHex(TxBuffer[22]);

    DataSaveCycle = TxBuffer[23];           //���ݴ洢���
    DataSaveCycleUint = TxBuffer[24];       //�洢ʱ�䵥λ
    UpLoadCycle = TxBuffer[25];             //�ϱ�����
    UpLoadCycleUint = TxBuffer[26];         //�ϱ�ʱ�䵥λ
    SaveFlag = PARA_SAVE;                       //�����洢��־
    switch(UpLoadCycleUint)    //���¼����ϱ���ʱ
    {
    case Uint_Sec:
      UpLoadWaitingTime = UpLoadCycle;
      break;
    case Uint_Min:
      UpLoadWaitingTime = (uint32_t)UpLoadCycle * 60;
      break;
    case Uint_Hour:
      UpLoadWaitingTime = (uint32_t)UpLoadCycle * 3600;
      break;
    case Uint_Day:
      UpLoadWaitingTime = (uint32_t)UpLoadCycle * 3600 * 24;
      break;
    case Uint_Week:
      UpLoadWaitingTime = (uint32_t)UpLoadCycle * 3600 * 24 * 7;
      break;
    case Uint_Month:
      UpLoadWaitingTime = (uint32_t)UpLoadCycle * 3600 * 24 * 30;
      break;
    default:
      UpLoadWaitingTime = (uint32_t)UpLoadCycle * 3600;
      break;
    }
      //======================================================================
    switch(DataSaveCycleUint)  //����洢��ʱ
    {
    case Uint_Sec:
      SaveWaitingTime = DataSaveCycle;
      break;
    case Uint_Min:
      SaveWaitingTime = (uint32_t)DataSaveCycle * 60;
      break;
    case Uint_Hour:
      SaveWaitingTime = (uint32_t)DataSaveCycle * 3600;
      break;
    case Uint_Day:
      SaveWaitingTime = (uint32_t)DataSaveCycle * 3600 * 24;
      break;
    case Uint_Week:
      SaveWaitingTime = (uint32_t)DataSaveCycle * 3600 * 24 * 7;
      break;
    case Uint_Month:
      SaveWaitingTime = (uint32_t)DataSaveCycle * 3600 * 24 * 30;
      break;
    default:
      SaveWaitingTime = (uint32_t)DataSaveCycle * 3600;
      break;
    }
    //GetThermInfoFlag = 1;    //�ٲ�ѯ�¿�����Ϣ����ʼ�ϱ�
    //CurrThermNo = 0;
    break;
  case 0xF009:
    if(TxBuffer[9] & 0x80)
      return;
    //CAT 1�ڹ�¯����Ӧ���
    FrameSer = TxBuffer[14];
    memcpy(rxbuf,TxBuffer,50);
    re = SendParaSettingAck();    //ԭ��Ӧ����Ϣ�����¿�����Ϣ�����ǵ���
    SetParaToGasHeater(rxbuf);          //���ʹ�������
    GetDataTime = 0;//GetInfoTime - 3;
    //UpLoadTime = UpLoadWaitingTime - 2;
    //RF_HandShakeTime = TIME_15M - 2;//4;//
    break;
  case 0xF00A:                            //��ʼ������־�
    AT24_readbyte(USER_PARA_OFFSET, StatusBuf, 13);
    delayms(3);

    UpLoadTime = 0x00;
    ErrHoldTime = 0x00;

    //SoftVer = 0x00;
    SoftVer = ((unsigned int)StatusBuf[2] << 8) + StatusBuf[3];   //旧版本号
    TempVer = ((unsigned int)TxBuffer[17] << 8) + TxBuffer[18];   //新版本号

    if(TempVer == SoftVer) //�汾���жϯ
    {
        //�������������쳣��
      SendRequestFrame(1);
    }
    else
    {
      StatusBuf[0] = 'N';   //��ʼ����
      StatusBuf[1] = 0x55;

      //StatusBuf[2] = TxBuffer[17];   //旧版本号
      //StatusBuf[3] = TxBuffer[18];

      //StatusBuf[8] = 0x00;     //预留
      //StatusBuf[9] = 0x00;

      t1 = BCDToHex(TxBuffer[19]);
      t2 = BCDToHex(TxBuffer[20]);
      //SoftVer  = (TxBuffer[17] << 8) + TxBuffer[18];
      TotalNum = (unsigned int)t1 * 100 + t2;
//================================================
//                StatusBuf[11] = 0x00;   //test
//================================================
      if(StatusBuf[11] != 'K' || StatusBuf[12] != 'S')  //�ж��Ƿ��쳣�ж�
      {
        Curraddr = USER_DATA_OFFSET;
        CurrFrameSer = 0x01;
      }
      else
      {
        tempaddr = ((unsigned int)StatusBuf[8] << 8) + StatusBuf[9];  //断点地址
        tempser = ((unsigned int)StatusBuf[6] << 8) + StatusBuf[7];    //断点帧序号
        CurrFrameSer = tempser;
        Curraddr = tempaddr;
      }
        //SavingTime = 0x00;
      SendRequestFrame(0);
    }
    break;
  case 0xF00B:            //������������
    re = 0x00;
    GreenLed_OFF;
    t1 = HexToBCD((unsigned char)(CurrFrameSer / 100));//BCDToHex(TxBuffer[19]);
    t2 = HexToBCD((unsigned char)(CurrFrameSer % 100));//BCDToHex(TxBuffer[20]);

    if(t1 == TxBuffer[19] && t2 == TxBuffer[20])//if(tempser <= TotalNum && tempser == CurrFrameSer)
    {
        //memcpy(rxbuf,TxBuffer+22,TxBuffer[23]+3);
      len = (unsigned int)TxBuffer[23]+3;
      for(i=0;i<len;i++)
      {
        rxbuf[i] = TxBuffer[i+22];
      }

      if(rxbuf[0] != 0x80)   //֡ͷ�жϴ
      {
        SendRequestFrame(1);
        return;
      }

      xorcheck = 0x00;

      for(i=0;i<rxbuf[1]+2;i++)  //���¼���У������
      {
        xorcheck ^= rxbuf[i];
      }

      if(xorcheck != rxbuf[rxbuf[1]+2])   //check err
      {
        SendRequestFrame(1);
        return;
      }
                //================================================��24��д�벢�������ȶ�����
      DI;
      len = rxbuf[1]+3;
      AT24_writebyte(Curraddr, rxbuf, len);     //24����
      delayms(5);

      AT24_readbyte(Curraddr, txbuf, len);     //����
      delayms(3);
      for(i=0;i<len;i++)
      {
        if(rxbuf[i] != txbuf[i])
        {
          SendRequestFrame(1);
          EI;
          return;
        }
      }
                //ʵʱ�洢������̵�����
      StatusBuf[4] = (unsigned char)(TotalNum >> 8);
      StatusBuf[5] = (unsigned char)(TotalNum & 0xff);

      StatusBuf[6] = (unsigned char)(CurrFrameSer >> 8);
      StatusBuf[7] = (unsigned char)(CurrFrameSer & 0xff);

      StatusBuf[8] = (unsigned char)(Curraddr >> 8);
      StatusBuf[9] = (unsigned char)(Curraddr & 0xff);

      StatusBuf[10] = 0x00;

      for(i=0;i<10;i++)
      {
        StatusBuf[10] += StatusBuf[i];
      }

      StatusBuf[11] = 'K';
      StatusBuf[12] = 'S';

      AT24_writebyte(USER_PARA_OFFSET, StatusBuf, 13);
      delayms(3);
      EI;
      //=====================================================
      Curraddr += len;    //������ɣ��洢��ַ�ۼ�
      //===================================
      CurrFrameSer ++;
      re = SendRequestFrame(0);

      memset(USCIA0RXBUF,0,UARTPOINTNUM);   //���������
    }
    RF_HandShakeTime = 0x00;    //程序升级不允许RF上报
    UpLoadTime = 0x00;          //不允许自动上报
    ErrHoldTime = 0x00;         //不允许故障定时上报
    UpgradeNum = 0x00;
    //SavingTime = 0x00;          //自动存储停止
//================================
    if(re == 0x88)// && (CurrFrameSer > TotalNum))   //升级完成
    {
  //DI;
      StatusBuf[0] = 'Y';   //有新的升级文件
      StatusBuf[1] = 0xAA;
      StatusBuf[2] = (unsigned char)(TempVer >> 8);    //升级成功，保存版本号
      StatusBuf[3] = (unsigned char)(TempVer & 0xff);

      StatusBuf[10] = 0x00;

      for(i=0;i<10;i++)
      {
        StatusBuf[10] += StatusBuf[i];
      }

      StatusBuf[11] = 'O';
      StatusBuf[12] = 'K';

      AT24_writebyte(USER_PARA_OFFSET, StatusBuf, 13);
      delayms(2000);//delayms(5);
      QK_SetSignleRegister(D_Status,0);
      Nb_ModuleClose();    //关闭网络，系统复位更新
      NVIC_SystemReset();
    }
    break;
//======================================================
  case 0xF00C:      //通过网络设置温控器
  case 0xF01E:      //�����¿���
    len = ((unsigned int)TxBuffer[11] << 8) + TxBuffer[10];
    if(len > 66) break;

    txbuf[0] = 0x68;
    txbuf[1] = 0x50;

    for(i=0;i<7;i++)
    {
      txbuf[i+2] = ThermostatId[i];
    }

    txbuf[9] = 0x03;
    txbuf[10] = len;
    txbuf[11] = 0xC0;
    if(cid == 0xF00C)
    {
      txbuf[12] = 0x24;
    }
    else 
    {
      txbuf[12] = 0x10;    //C010дʱ�伤���λ��ȥ��FD 1���ֽ�,11λ���޸�Ϊ00��ռ��comλ�ã����Ի��Ǵ�buf[15]��ʼ����
      LDCMode = 1;        //ǰ������ģʽ
    }
    txbuf[13] = TxBuffer[14];
    memcpy(txbuf+14,TxBuffer+15,txbuf[10]);
    txbuf[txbuf[10]+11] = 0x00;
    for(i=0;i<txbuf[10]+11;i++)
    {
      txbuf[txbuf[10]+11] += txbuf[i];
    }
    txbuf[txbuf[10]+12] = 0x16;
    ///======================================后期需要完善前导激活方式
#ifdef NET_DELAY_TEST
  SendNbData("RF_send_begin",13); 
#endif
    PNA3029_SendPacket(txbuf,txbuf[10]+13);
    //PNA3029_SendPacket(txbuf,txbuf[10]+13);
    PNA3029_RX();
    LDCMode = 0;
    //ThermostatControlFlag = 0x01;
    //ThermostatControlSer = TxBuffer[14];
    ///============================================20250523��չ
    ReSendBuf[0] = txbuf[10]+13;
    memcpy(ReSendBuf+1,txbuf,ReSendBuf[0]);
    ReissuedFlag = 1;
    ReissuedTime = 0;
#ifdef NET_DELAY_TEST
  SendNbData("RF_send_end",11); 
#endif
    ///=============================================
    break;
  case 0xF00F:     //�����¿���
  case 0xF010:     //�����¿���ģʽ
  case 0xF011:     //�����¶�
  case 0xF012:     //�����¿ر��
  case 0xF014:     //��������ˮ�豸
  case 0xF015:     //����ˮģʽ����
  case 0xF017:     //���ý���ʱ���
  case 0xF018:     //�����˿�����
  case 0xF01A:     //������ر��¿�������
  case 0xF01B:
  case 0xF01C:
  case 0xF01D:
  case 0xF01F:
  case 0xF020:
    //memcpy(txbuf,TxBuffer+17,8);  //����������
    USCIA0RXBUF[9] |= 0x80;
    USCIA0RXBUF[10] = 0x05;     //�޸ĳ���
    USCIA0RXBUF[11] = 0x00;
    crc = chkcrc(USCIA0RXBUF,17);
    USCIA0RXBUF[17] = crc & 0xff;
    USCIA0RXBUF[18] = crc >> 8;
    USCIA0RXBUF[19] = 0x16;
    SendNbData(USCIA0RXBUF,20);
    delayms(50);
    //===========================���Ӵ���,��ȡ�趨�¶�֮���·����¿���
//    if(cid == 0xF00C)
//    {
//      len = ((unsigned int)TxBuffer[11] << 8) + TxBuffer[10];
//           
//      txbuf[0] = 0x68;
//      txbuf[1] = 0x50;
//
//      for(i=0;i<7;i++)
//      {
//        txbuf[i+2] = ThermostatId[i];
//      }
//
//      txbuf[9] = 0x03;
//      txbuf[10] = len;
//      txbuf[11] = 0xC0;
//      txbuf[12] = 0x24;
//      txbuf[13] = TxBuffer[14];
//      memcpy(txbuf+14,TxBuffer+15,txbuf[10]);
//      txbuf[txbuf[10]+11] = 0x00;
//      for(i=0;i<txbuf[10]+11;i++)
//      {
//        txbuf[txbuf[10]+11] += txbuf[i];
//      }
//      txbuf[txbuf[10]+12] = 0x16;
//      ///======================================后期需要完善前导激活方式
//      SX1278SendPacket(txbuf,txbuf[10]+13);
//      SX1278SendPacket(txbuf,txbuf[10]+13);
//      SX1278_RX();
//    }
//    else 
    if(cid == 0xF00F)
    {
      ThermostatNum = TxBuffer[17];
      if(ThermostatNum == 0 || ThermostatNum > 5)
      {
        RELAY_ON;
      }
      
      for(i=0,j=0;i<ThermostatNum*7;i++)   //��ȡ�����¿���id����һ�����������
      {
        if(i && (i % 7 == 0))  //ÿ7����һ�����
        {
          j += 1;
        }
        
        RoomThermostat[j].id[i-j*7] = TxBuffer[18+i];
      }
      //SaveFlag = 0x02;
      //============================������������
      if(ThermostatNum > 0 && ThermostatNum <= 5)
      {
        txbuf[0] = 0x68; 
        txbuf[1] = 0x50;  //֡ͷ
        for(i=0;i<7;i++)  //�������¿������
        {
          txbuf[i+2] = RoomThermostat[0].id[i];
        }
        txbuf[9] = 0x04;  
        txbuf[10] = 10;
        txbuf[11] = 0xC0; 
        txbuf[12] = 0x2C;  
        txbuf[13] = 0x01; //ser
        for(i=0;i<7;i++)  //���ñڹ�¯���
        {
          txbuf[i+14] = DeviceId[i];
        }
        txbuf[21] = calSum(txbuf,21);
        txbuf[22] = 0x16;
        LDCMode = 0x01;    //ǰ������
        PNA3029_SendPacket(txbuf,23);
        //PNA3029_SendPacket(txbuf,23);
        LDCMode = 0x00;
        PNA3029_RX();
        RELAY_OFF;      //����ס�¿����󣬱ڹ�¯����ǿ�Ʋ�ů
      //===============================
      }
    }
    else if(cid == 0xF010)    //������ģʽ���·��¶�
    {
      //MainThermostatMode = TxBuffer[24];  //��ȡ��ǰģʽ;0xAA:�ֶ�ģʽ;0x99:����ģʽ0x66:�Զ�ģʽ;
      if(TxBuffer[24] == 0x66)
      {
        MainThermostatMode = AUTO_RUN;
      }
      else if(TxBuffer[24] == 0x99)
      {
        MainThermostatMode = ANTIFREEZE_RUN;
      }
      else if(TxBuffer[24] == 0xAA)
      {
        MainThermostatMode = MANVAL_RUN;
      }
      SetTempFlag = 0x01;
    }
    else if(cid == 0xF011)    //��Ҫ�·�
    {
      SetTemper = TxBuffer[24];
      if(MainThermostatMode == MANVAL_RUN)
      {
        ManualTemper = SetTemper;
      }
      else if(MainThermostatMode == ANTIFREEZE_RUN)
      {
        AntifreezTemper = SetTemper;
      }
      SetTempFlag  = 0x02;
    }
    else if(cid == 0xF012)  //���
    {
      MainThermostatScheduleNum = TxBuffer[24];
      len = MainThermostatScheduleNum * 3;
      
      for(i=0,j=0;i<len;i+=3)
      {
        MainThermostatSchedule[j].hour = TxBuffer[i+25];
        MainThermostatSchedule[j].min = TxBuffer[i+26];
        MainThermostatSchedule[j].temper = TxBuffer[i+27];
        j ++;
      }
      SetTempFlag = 0x01;
    }
    else if(cid == 0xF014)   //��������ˮ
    {
      memcpy(RevId,&TxBuffer[17],7);
      NonColdWateStatus = 1;     //������ˮ������ӻ����
      QK_SetSignleRegister(D_Status,(alarm_sta | NON_COLDWATER_ICON));
    }
    else if(cid == 0xF015)   //����ˮģʽ����
    {
      LLS_Pump.pump_time = ((unsigned int)TxBuffer[19] << 8) + TxBuffer[18];
      
      LLS_Pump.control_mode = (TxBuffer[17] >> 5) & 0x01;  //0��ʱ����ƣ�1���¶ȿ���
           
      if(LLS_Pump.control_mode)   
      {
        LLS_Pump.control_temp = TxBuffer[21];  
      }
//      else   ///==0:��ʾɾ������ˮ
//      {
//        NonColdWateStatus = 0;
//        SaveFlag = PARA_SAVE;
//        QK_SetSignleRegister(D_Status,(alarm_sta & ~NON_COLDWATER_ICON));
//      }
        
      if((TxBuffer[17] >> 6) == RUN_ONCE_MODE)   //�㶯
      {
//        SendRecData(LLS_ON);    
//        LLS_StartFlag = 1;     
//        LLS_HoldTime = 0;     
        RunOnceFlag = 1;
        QK_SetSignleRegister(D_Status,(alarm_sta | NON_COLDWATER_ICON));
      }
      else   
      {
        QK_SetSignleRegister(D_Status,(alarm_sta | NON_COLDWATER_ICON));
        LLS_Pump.run_mode = TxBuffer[17] >> 6;
        if(LLS_Pump.run_mode == LLS_OFF)        //�ر�
        {
          SendRecData(LLS_OFF,LLS_PUMP);
          QK_SetSignleRegister(D_Status,(alarm_sta & ~NON_COLDWATER_ICON));
        }
        else if(LLS_Pump.run_mode == CUISER_MODE)    //Ѳ��ģʽ
        {
          SendRecData(LLS_ON,LLS_PUMP);
           
          if(LLS_Pump.control_mode == 0) 
          {
            LLS_CuiserInterval = TxBuffer[20];
            LLS_CuiserTime = 0;
            LLS_Pump.cuiser_interval = LLS_CuiserInterval;
            LLS_StartFlag = 1;      
            LLS_HoldTime = 0;      
          }
        }
        else         //ԤԼ
        {
          i = 0;
          LLS_Pump.schedule_num = TxBuffer[17] & 0x1f;  
          if(LLS_Pump.control_mode)
          {
            i = 1;  
          }
          ScheduleCheckTime = 0;
          memcpy(LLS_Pump.schedule,TxBuffer+20+i,LLS_Pump.schedule_num*2);  
        }
      }
      ansflag = 1;  
    }
    else if(cid == 0xF017)   //��������ˮ����ʱ���
    {
      LLS_Pump.ec_num = TxBuffer[17];
      if(LLS_Pump.ec_num > 3) LLS_Pump.ec_num = 3;
      memcpy(LLS_Pump.ec_time,&TxBuffer[18],LLS_Pump.ec_num*4);
      //CheckEnergySaving();
      ansflag = 1;
    }
    else if(cid == 0xF018)   //���������˿�
    {
      len = TxBuffer[17];
      memset(WifiDns,0,30);
      memcpy(WifiDns,&TxBuffer[18],len);
      WifiPort = ((unsigned int)TxBuffer[18+len] << 8) + TxBuffer[19+len];
      Nb_ModuleClose();               //�ػ��������µ������Ͷ˿�������������
      ConnectFlag = NO_CONNECT;
      UpLoadTime = UpLoadWaitingTime;
    }
    else if(cid == 0xF01A)   //������ر��¿�������
    {
      ThermEnableFlag = TxBuffer[17];
      if(ThermEnableFlag)   //�����¿������Ͽ��պ��ź�
      {
        RELAY_OFF;
      }
      else
      {
        RELAY_ON;
      }
    }
    else if(cid == 0xF01B)   //����mini�����¿���
    {
      memcpy(ThermostatId,TxBuffer+17,7);
      //SaveFlag = PARA_SAVE;
    }
    else if(cid == 0xF01D)    //�������رս���ģʽ
    {
      if(TxBuffer[17] == 0x55)
      {
        QK_SetSignleRegister(D_Status,(alarm_sta | AIMODE_ICON));
      }
      else if(TxBuffer[17] == 0x99)
      {
        QK_SetSignleRegister(D_Status,(alarm_sta & ~AIMODE_ICON));
      }
    }
    else if(cid == 0xF020)     //����ѭ����
    {
#if 1
      if(TxBuffer[17] == 0xFF)     //ɾ��ѭ����
      {
        XH_PumpFlag = 0; 
      }
      else
      { 
        memcpy(XH_PumpId,TxBuffer+18,7);
        XH_PumpFlag = 1;                   //��ѭ���ã�����ӻ����
      }
      SaveFlag = 0x02;
#endif
    }
    
    if(ansflag)  //�ϱ�����ˮ��Ϣ
    {
      ansflag = 0;
      UpLoadNonColdWaterStatus();
      delayms(100);     //���ݽϳ���������ɺ���΢��ʱ��
    }
    
    SaveFlag = PARA_SAVE;    //�ñ�־���洢��ز���
    break; 
  default:
    break;
  }
}

void SetParaToGasHeater(unsigned char *probuf)
{
  unsigned char index = 0x00;
  int tempdata,tdata;
  unsigned int week;

  if(probuf[21] & ONOFF)    //
  {
    tempdata = probuf[24+index];
    index ++;
    QK_SetSignleRegister(ON_OFF,tempdata);
  }

  if(probuf[21] & WIN_SUM)   //冬夏模式
  {
    tempdata = probuf[24+index];
    index ++;
    QK_SetSignleRegister(Win_Summer,tempdata);
  }

  if(probuf[21] & WarmTarget)   //供暖水目标温度
  {
    tempdata = probuf[24+index];
    index ++;
    QK_SetSignleRegister(HeatingWater,tempdata);
  }

  if(probuf[21] & BathTarget)   //卫浴水目标温度
  {
    tempdata = probuf[24+index];
    index ++;
    QK_SetSignleRegister(BathWater,tempdata);
  }

  if(probuf[21] & FlucTarget)   //回差温度
  {
    tempdata = probuf[24+index];
    index ++;
    QK_SetSignleRegister(FlucTempReg,tempdata);
  }
/*
#if 0
    if(probuf[21] & MinPSeting)   //
    {
      tbuf[3] = 0x08;
      tbuf[4] = probuf[24+index];
      index ++;
      tbuf[31] = calXOR(tbuf+1,30);
      R_uart0_send(tbuf,32,2);
    }

    if(probuf[21] & MaxPSeting)   
    {
      tbuf[3] = 0x09;
      tbuf[4] = probuf[24+index];
      index ++;
      tbuf[31] = calXOR(tbuf+1,30);
      R_uart0_send(tbuf,32,2);
    }

    if(probuf[21] & FirePSeting)   
    {
      tbuf[3] = 0x0a;
      tbuf[4] = probuf[24+index];
      index ++;
      tbuf[31] = calXOR(tbuf+1,30);
      R_uart0_send(tbuf,32,2);
    }

///=============================================================================
    if(probuf[22] & ValveType)    
    {
      tbuf[3] = 0x1e;
      tbuf[4] = probuf[24+index];
      index ++;
      tbuf[31] = calXOR(tbuf+1,30);
      R_uart0_send(tbuf,32,2);
    }

    if(probuf[22] & WaterSwitch)  
    {
      tbuf[3] = 0x1f;
      tbuf[4] = probuf[24+index];
      index ++;
      tbuf[31] = calXOR(tbuf+1,30);
      R_uart0_send(tbuf,32,2);
    }

    if(probuf[22] & HeatingMode)   
    {
      tbuf[3] = 0x20;
      tbuf[4] = probuf[24+index];
      index ++;
      tbuf[31] = calXOR(tbuf+1,30);
      R_uart0_send(tbuf,32,2);
    }

    if(probuf[22] & MachineType)  
    {
      tbuf[3] = 0x21;
      tbuf[4] = probuf[24+index];
      index ++;
      tbuf[31] = calXOR(tbuf+1,30);
      R_uart0_send(tbuf,32,2);
    }

    if(probuf[22] & HeatExchanger)   
    {
      tbuf[3] = 0x22;
      tbuf[4] = probuf[24+index];
      index ++;
      tbuf[31] = calXOR(tbuf+1,30);
      R_uart0_send(tbuf,32,2);
    }

    if(probuf[22] & FlowSwitchType)  
    {
      tbuf[3] = 0x23;
      tbuf[4] = probuf[24+index];
      index ++;
      tbuf[31] = calXOR(tbuf+1,30);
      R_uart0_send(tbuf,32,2);
    }

    if(probuf[22] & BlockValve)   
    {
      tbuf[3] = 0x2c;
      tbuf[4] = probuf[24+index];
      index ++;
      tbuf[31] = calXOR(tbuf+1,30);
      R_uart0_send(tbuf,32,2);
    }

    if(probuf[22] & PumpMode) 
    {
      tbuf[3] = 0x2d;
      tbuf[4] = probuf[24+index];
      index ++;
      tbuf[31] = calXOR(tbuf+1,30);
      R_uart0_send(tbuf,32,2);
    }

 //========================================================================
    if(probuf[23] & BurningMode)   
    {
      tbuf[3] = 0x2e;
      tbuf[4] = probuf[24+index];
      index ++;
      tbuf[31] = calXOR(tbuf+1,30);
      R_uart0_send(tbuf,32,2);
    }

    if(probuf[23] & PressSeneor)  
    {
      tbuf[3] = 0x2f;
      tbuf[4] = probuf[24+index];
      index ++;
      tbuf[31] = calXOR(tbuf+1,30);
      R_uart0_send(tbuf,32,2);
    }

    if(probuf[23] & MachinePwrType)  
    {
      tbuf[3] = 0x45;
      tbuf[4] = probuf[24+index];
      index ++;
      tbuf[31] = calXOR(tbuf+1,30);
      R_uart0_send(tbuf,32,2);
    }
#endif
*/
    //====================================================
#ifdef PreSetting_Version
    if(probuf[23] & TempComps)   //气候补偿开关
    {
      tempdata = probuf[24+index];
      index ++;
      QK_SetSignleRegister(TempCompenation_offset,tempdata);
    }
    if(probuf[23] & EnvTemp)   //环境温度
    {
      tempdata = probuf[24+index];  //取整数字节
      index += 2;
      QK_SetSignleRegister(EnvironmentTemp_offset,tempdata);
	//===================================增加设定室温和设定温度，考虑是否需要在获取到信息后第一时间写入，可以节省两字节变量
//	delayms(200);
//	if(Therm_RoomTemp < 5 || Therm_RoomTemp > 35)
//	{
//	    Therm_RoomTemp = 18;
//	}
//	QK_SetSignleRegister(RoomTemp__offset,Therm_RoomTemp);
//	delayms(200);
//	if(Therm_SetTemp < 5 || Therm_SetTemp > 35)
//	{
//	    Therm_SetTemp = 18;
//	}
//	QK_SetSignleRegister(SetTemp__offset,Therm_SetTemp);
    }
    if(probuf[23] & StoveTime)   //时间  按照16进制格式设置，直接采用系统时间更新
    {
#ifdef WeekSet   //时间高3位表示星期，中间5位表示小时，低8位是分钟
        //tempdata = ((int)probuf[24+index] << 8) + probuf[25+index] ;    //设置时和分
	    week = (unsigned int)SysWeek << 13;
	//tempdata = week + ((unsigned int)SysHour << 8) + SysMinute;
	    QK_SetSignleRegister(StoveTime__offset,week + ((unsigned int)SysHour << 8) + SysMinute);
#else
	    tempdata = ((int)SysHour << 8) + SysMinute;
	    QK_SetSignleRegister(StoveTime__offset,tempdata);
#endif
      index += 2;
    }
    if(probuf[23] & HeaterMode)      //设置散热片或地暖模式
    {
	    tempdata = probuf[24+index] ^ 0x01;   //逻辑与设置相反
	    index ++;
      QK_SetSignleRegister(HeaterMode_offset,tempdata);
    }
#ifdef  RemoteCtr
    if(probuf[23] & RemoteLock) //远程锁定
    {
      tdata = probuf[24+index];   //第一字节表示开启数据
      index ++;
      //QK_SetSignleRegister(RemoteCtr_offset,tempdata);

      tempdata = probuf[24+index];    //第二字节0表示不启用锁定功能，非0表示启用锁定，同时表示失联时间限制
      QK_SetSignleRegister(LossComm_offset,tempdata);   //先发标志
      delayms(50);
      QK_SetSignleRegister(RemoteCtr_offset,tdata);     //再发数据
    }
#endif
#endif
}

//unsigned char R_uart0_send(unsigned char *sbuf,unsigned char len,unsigned char num)
//{
//  unsigned char i;
//
//  CmdRunTime = 0x00;
//  //g_uart0_recv_complete = false;  //  等待接收空闲
//  //while(!g_uart0_recv_complete || CmdRunTime < 2);
//
//  for(i=0;i<num;i++)
//  {
//    g_uart0_send_complete = false;
//    CmdRunTime = 0;
//    R_SCI_UART_Write(&g_uart0_ctrl, sbuf, len);
//#if 1
//    while(!g_uart0_send_complete)//while(!g_uart0_send_complete)
//    {
//      if(CmdRunTime > 2)//if(CmdRunTime > 2)
//      {
//      //delayms(5);
//        return 0;
//      }
//    }
// //       delayms(5);
//#endif
//  }
//  return 1;
//}

unsigned char R_uart0_send(unsigned char *sbuf,unsigned char len,unsigned char num)
{
  unsigned char i;

  CommReadTime = 0x00;
  //g_uart0_recv_complete = false;  //  等待接收空闲
  //while(!g_uart0_recv_complete || CmdRunTime < 2);

  for(i=0;i<num;i++)
  {
    g_uart0_send_complete = false;
    CommReadTime = 0;
    R_SCI_UART_Write(&g_uart0_ctrl, sbuf, len);
#if 1
    while(!g_uart0_send_complete)//while(!g_uart0_send_complete)
    {
      if(CommReadTime > 2)//if(CmdRunTime > 2)
      {
      //delayms(5);
        return 0;
      }
    }
 //       delayms(5);
#endif
  }
  return 1;
}

void SetGasHeaterRst(void)
{
  unsigned char tbuf[35];

  tbuf[0]  = 0xFC;
  tbuf[1]  = 0x03;
  tbuf[2]  = 0x00;
  tbuf[3]  = 0x03;
  tbuf[4]  = 0x00;
  tbuf[5]  = 0xFC;
  tbuf[6]  = 0xFC;
  tbuf[7]  = 0xFC;
  tbuf[8]  = 0x01;
  tbuf[9]  = 0x01;
  tbuf[10] = 0x01;
  tbuf[11] = 0x01;
  tbuf[12] = 0x01;
  tbuf[13] = 0x00;
  tbuf[14] = 0x00;
  tbuf[15] = 0x00;
  tbuf[16] = 0x00;
  tbuf[17] = 0x00;
  tbuf[18] = 0x00;
  tbuf[19] = 0x00;
  tbuf[20] = 0x00;
  tbuf[21] = 0x00;
  tbuf[22] = 0x00;
  tbuf[23] = 0x00;
  tbuf[24] = 0x00;
  tbuf[25] = 0x00;
  tbuf[26] = 0x00;
  tbuf[27] = 0x00;
  tbuf[28] = 0x00;
  tbuf[29] = 0x00;
  tbuf[30] = 0x00;
  tbuf[31] = calXOR(tbuf+1,30);
  R_uart0_send(tbuf,32,2);
}

void QK_ReadAllRegisters(void)
{
  unsigned char sbuf[10];
  unsigned int crc16;
  Uart0_Rx_index = 0;
  sbuf[0] = LocalAddr;   //addr
  sbuf[1] = 0x03;        //read function
  sbuf[2] = 0x00;        //start address
  sbuf[3] = 0x00;
  sbuf[4] = 0x00;        //counts = 19
#ifndef VER_07   //#ifdef RemoteCtr
  sbuf[5] = 0x1A;  //0---25
#else
  sbuf[5] = 0x13;      //
#endif
  crc16 =  chkcrc(sbuf,6);
  sbuf[6] = (unsigned char)(crc16 & 0xff);
  sbuf[7] = (unsigned char)(crc16 >> 8);

  R_uart0_send(sbuf,8,1);
  delayms(100);//delayms(300);
}

void QK_SetSignleRegister(unsigned int reg,int data)
{
  unsigned char sbuf[10];
  unsigned int crc16;

  Uart0_Rx_index = 0;
  sbuf[0] = LocalAddr;   //addr
  sbuf[1] = 0x06;        //Single set function

  sbuf[2] = (unsigned char)(reg >> 8);
  sbuf[3] = reg & 0xff;

  sbuf[4] = (unsigned char)(data >> 8);
  sbuf[5] = data & 0xff;

  crc16 =  chkcrc(sbuf,6);
  sbuf[6] = (unsigned char)(crc16 & 0xff);
  sbuf[7] = (unsigned char)(crc16 >> 8);

  //R_SCI_UART_Write(&g_uart0_ctrl, sbuf, 8);
  R_uart0_send(sbuf,8,1);
  delayms(100);//delayms(200);
}

unsigned char ReportThermostatInfo(unsigned char n)  //���վ������ϱ�����1---5
{
  unsigned char i;
  unsigned int crc;
  unsigned char txbuf[40];
  
  txbuf[0] = 0x68;
  txbuf[1] = 0x86;
  
  for(i=0;i<7;i++)
  {
    txbuf[i+2] = DeviceId[i];
  }
  
  txbuf[9]  = 0x01;
  txbuf[10] = 20;     //len
  txbuf[11] = 0;
  
  txbuf[12] = 0xF0;
  txbuf[13] = 0x13;
  
  txbuf[14] = 0x01;      //ser

  txbuf[15] = FacCode;
  txbuf[16] = GasHeater; //type
  
  txbuf[17] = 0x01;   //һ���¿���
  
  for(i=0;i<7;i++)
  {
    txbuf[i+18] = RoomThermostat[n-1].id[i];
  }
  
  txbuf[25] = RoomThermostat[n-1].room_temp / 10;
  txbuf[26] = RoomThermostat[n-1].room_temp % 10;
    
  txbuf[27] = RoomThermostat[n-1].set_temp / 10;
  txbuf[28] = RoomThermostat[n-1].set_temp % 10;
  
  txbuf[29] = RoomThermostat[n-1].rh;
  txbuf[30] = RoomThermostat[n-1].fire;
  txbuf[31] = RoomThermostat[n-1].m_s;
  
  crc = chkcrc(txbuf,32);
  txbuf[32] = crc & 0xff;
  txbuf[33] = crc >> 8;
  
  txbuf[34] = 0x16;
  
  SendNbData(txbuf,35);
  delayms(50);
  return 1;
}

void UpLoadNonColdWaterStatus(void)    //����ˮӦ���ϱ�
{
  unsigned char num;
  unsigned int crc;
  unsigned char txbuf[100];

  txbuf[0] = 0x68;
  txbuf[1] = 0x86;
      
  memcpy(txbuf+2,DeviceId,7);
    
  txbuf[9] = 0x01;

  txbuf[12] = 0xF0;
  txbuf[13] = 0x16;     
  
  txbuf[14] = 0x01;   //ser

  txbuf[15] = FacCode;
  txbuf[16] = GasHeater; //type

  memcpy(txbuf+17,RevId,7);
  
  txbuf[24] = (LLS_Pump.run_mode << 6) | (LLS_Pump.control_mode << 5) | LLS_Pump.schedule_num;  //运行模式
  num = txbuf[24] & 0x1f;        //
  txbuf[25] = LLS_Pump.pump_time & 0xff;
  txbuf[26] = LLS_Pump.pump_time >> 8;
  txbuf[27] = LLS_Pump.cuiser_interval;
  txbuf[28] = LLS_Pump.control_temp;
  
  memcpy(txbuf+29,LLS_Pump.schedule,num*2);
  
  txbuf[10] = (17+num*2) & 0xff;
  txbuf[11] = (17+num*2) >> 8;      //���ֽ���ǰ
  
  crc = chkcrc(txbuf,29+num*2);
  txbuf[29+num*2] = crc & 0xff;
  txbuf[30+num*2] = crc >> 8;

  txbuf[31+num*2] = 0x16;
    
  SendNbData(txbuf,32+num*2);
}

unsigned char CheckEnergySaving(void)  //������ʱ�����Ϣ���ж��Ƿ���Ҫ��������ˮ
{
  unsigned char i;//,j;
  unsigned char hour,min;
  unsigned char num,flag = 0;
  unsigned char workflag = 0;    //Ĭ��������ʱ�Σ�ˮ����������
  unsigned char info[20];
  
  if(LLS_Pump.ec_num == 0 || LLS_Pump.ec_num > 3)
  {
    return NO_PUMP;
  }
  
  hour = SysHour;
  min  = SysMinute;
  
  num = LLS_Pump.ec_num;
  memcpy(info,LLS_Pump.ec_time,num*4);   //��������ʱ���������������
  for(i=0;i<num*4;i+=4)                 //��������ʱ����Ϣ
  {
    if((LLS_Pump.ec_time[i+2] < LLS_Pump.ec_time[i]) || ((LLS_Pump.ec_time[i+2] == LLS_Pump.ec_time[i]) && (LLS_Pump.ec_time[i+3] < LLS_Pump.ec_time[i+1])))   //��һʱ���С��ǰһʱ��Σ���Ϊ�ǿ���
    { //ֻ��������һ��ʱ����Ϣ�д������
//      for(j=num*4+2;j>i+1;j--)   //
//      {
//        info[j] = info[j-1];
//      }
      memcpy(info+i+6,info+i+2,num*4-2);  //����ߵ�ʱ��κ��ƣ�����4����λ
      info[i+2] = 23;    //��λ����Ϊ����ʱ����Ϣ����0��Ϊ��
      info[i+3] = 59;
      info[i+4] = 0;
      info[i+5] = 0;
      //info[i+6] = LLS_Pump.ec_time[i+2];
      //info[i+7] = LLS_Pump.ec_time[i+3];
      num ++;          //ʱ������1       
      break;
    }
  }               
  //����ϵͳʱ���ж��Ƿ���Ҫ�ر�ˮ��
  //PumpRunFlag = 1;��ʾ��ǰ���ڹ�����0��ʾ�ر�
  for(i=0;i<num*4;i+=4)
  {
    //������ʼʱ��㣬����Ϊ�ǹر�ˮ��
    flag = 0;
    if((hour > info[i]) || ((hour == info[i]) && (min > info[i+1])))   
    {
      flag = 1;    //������ʼʱ�䣬
      //workflag = 0;
    }
    //���ж�ʱ���Ƿ��������ʱ���
    if(flag && ((hour < info[i+2]) || ((hour == info[i+2]) && (min < info[i+3]))))
    {
      workflag = 1;    //ʱ�䷶Χ�����㣬����Ϊ�������ʱ��Σ���Ҫ�ر�ˮ��
      break;
    }
  }
  
//  if(!PumpRunFlag && workflag)   //ˮ��û�����У���Ҫ����ˮ��
//  {
//    return workflag;
//  }
//  if(PumpRunFlag && !workflag)  //ˮ����ִ�У���Ҫ�ر�ˮ��
//  {
//    SendRecData(LLS_OFF);
//    PumpRunFlag = 0;
//  }
  
  return workflag;
}



