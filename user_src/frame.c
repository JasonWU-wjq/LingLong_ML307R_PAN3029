/*
 * frame.c
 *
 *  Created on: 2021年6月22日
 *      Author: WJQ
 */

#include  "../usr_lib/rf.h"

extern unsigned char calSum(unsigned char *buf,unsigned char len);

//void ProtcolChange(unsigned char *dbuf,unsigned char *sbuf)    
//{
//  ;
//}

unsigned char ProcessFrame(void)
{
  unsigned char i,check;
  unsigned int len;
  //unsigned char fire_sta;
  if(RF_RxBuf[1] == 0x68 && RF_RxBuf[11] == 0xaa)   //壁挂炉控制
  {
    for(i=3;i<10;i++)
    {
      if(RF_RxBuf[i] != DeviceId[i-3])
      {
        return 5;
      }
    }

    if(RF_RxBuf[2] == 0xa6)
    {
      RELAY_ON;   //FireFlag = 0x01;
    }
    else if(RF_RxBuf[2] == 0xa7)
    {
      RELAY_OFF;  //FireFlag = 0x00;
    }

	  return SetONOFF;
  }

  if(RF_RxBuf[0] != 0x68 || RF_RxBuf[1] != 0x50)   //֡ͷ����
  {
    return 1;
  }

//  if((RF_RxBuf[9] & 0x80) && (RF_RxBuf[11] != 0xc0) && (RF_RxBuf[12] != 0x2d))   //�ж��Ƿ���Ӧ��֡
//  {
//    return 2;    //c02d�Ƕ�ȡ�¿�����ϢӦ��
//  }

  len = RF_RxBuf[10];

//===============================获取温控器心跳信息c019
  /*
  if(RF_RxBuf[11] == 0xC0 && RF_RxBuf[12] == 0x19)
  {
    for(i=0;i<7;i++)
    {
      if(DeviceId[i] != RF_RxBuf[len+13+i])    //16后增加7字节编号+cs+26
      {
        return 0;
      }
    }

    if(RF_RxBuf[len+13+8] != 0x26)
    {
      return 0;
    }
    check = 0;
    for(i=0;i<(len+13+7);i++)
    {
      check += RF_RxBuf[i];
    }
    if(check != RF_RxBuf[len+13+7])
    {
      return 0;
    }
    //===========================================
//========================================== 
    if(RF_RxBuf[18] == 1)  
    {
      RELAY_ON;
    }
    else
    {
      RELAY_OFF;
    }
//====================================
    //============================================
    memset(Thermo_Buf,0,strlen((char*)Thermo_Buf));
    Thermo_Buf[0] = len + 13;
    memcpy(Thermo_Buf+1,RF_RxBuf,len+13);
    //===========================
    if(ThermostatControlFlag)
    {
	    ThermostatControlFlag = 0x00;
	    SendThermostatData();
    }
      
#ifndef VER_07
    Therm_RoomTemp = RF_RxBuf[14];   //获取室温和设定温度后直接设定
    Therm_SetTemp = RF_RxBuf[16];

    if(Therm_RoomTemp < 5 || Therm_RoomTemp > 35)
    {
      Therm_RoomTemp = 18;
    }
    QK_SetSignleRegister(RoomTemp__offset,Therm_RoomTemp);
    delayms(200);
    if(Therm_SetTemp < 5 || Therm_SetTemp > 35)
    {
      Therm_SetTemp = 18;
    }
    QK_SetSignleRegister(SetTemp__offset,Therm_SetTemp);
#endif
    return 0;
  }
  */

//============================================
//  if((RF_RxBuf[11] != 0xc0) && (RF_RxBuf[12] != 0x2d))
//  {
//    if(DeviceSta != MatchCode)
//    {
//      for(i=14;i<21;i++)
//      {
//        if(DeviceId[i-14] != RF_RxBuf[i])    //�ڹ�¯����жϡ�֮ǰЭ��
//        {
//          return 3;
//        }
//      }
//    }
//  }
//  else
//  {
//    for(i=2;i<9;i++)
//    {
//      if(ThermostatId[i-2] != RF_RxBuf[i])    //����¿�Э��
//      {
//        return 3;
//      }
//    }
//  }
  
  //DeviceSta = MatchCode;///����  
  if(DeviceSta != MatchCode)    //����״̬����������ж�
  {
    for(i=2;i<9;i ++)//for(loopi=3;loopi<9;loopi ++)   //macֻ�ж����ֽ�
    {
      if(ThermostatId[i-2] != RF_RxBuf[i])
      {
        return 3;
      }
    }
  }
  
  check = 0x00;

  for(i=0;i<len+11;i++)
  {
    check += RF_RxBuf[i];
  }

  if(check != RF_RxBuf[len+11])   //check  err
  {
    return 4;
  }

  if(RF_RxBuf[len+12] != 0x16)    //�жϽ�����
  {
    return 5;
  }

  FrameDir = RF_RxBuf[9];
  FrameSer = RF_RxBuf[13];                  //����ԭ�ȵ�֡���
  /*
  if((RF_RxBuf[11] != 0xc0) || (RF_RxBuf[12] != 0x2d))   //c02d����
  {
    for(i=0;i<7;i++)
    {
      if(ThermostatId[i] != RF_RxBuf[i+2])    //����б仯�Ż�洢
      {
        memcpy(ThermostatId,RF_RxBuf+2,7);       //====================�����ƺб��
        SaveFlag = PARA_SAVE;
        break;
      }
    }
  }
  
  if(RF_RxBuf[11] == 0xC0 && RF_RxBuf[12] == 0x20)    //�յ��ƺ�������ñڹ�¯��Ϣ
  {
    SetParaToGasHeater(RF_RxBuf);   //串口设置
    //GetDataTime = 0x02;
    return SettingSucc;
  }
  else if(RF_RxBuf[11] == 0xC0 && RF_RxBuf[12] == 0x22) //reset
  {
    SetGasHeaterRst();
    return ResetSucc;
  }
  else if(RF_RxBuf[11] == 0xC0 && RF_RxBuf[12] == 0x2d)    //�յ��¿������ݣ������ϱ�
  {
    RoomThermostat[CurrThermNo-1].room_temp = RF_RxBuf[21] * 10 + RF_RxBuf[22];
    RoomThermostat[CurrThermNo-1].set_temp  = RF_RxBuf[23] * 10 + RF_RxBuf[24];
    RoomThermostat[CurrThermNo-1].rh        = RF_RxBuf[25];
    RoomThermostat[CurrThermNo-1].fire      = RF_RxBuf[26];
    if(CurrThermNo == 1)   //ֻ��1��������
    {
      RoomThermostat[CurrThermNo-1].m_s = 1;
    }
    else
    {
      RoomThermostat[CurrThermNo-1].m_s = 0;
    }
    
    ReportThermostatInfo(CurrThermNo);   //ÿ�յ�һ�����ϱ��¿�����Ϣ
    CurrThermNo += 1;                   //��ʼ��һ�¿�����ȡ
    GetThermInfoFlag = 1; 
    GetThermInfoOverTime = 0;
    ReadBeginFlag = 0;
  }
  */
  if(RF_RxBuf[11] == 0xC3 && RF_RxBuf[12] == 0x01)   //����
  {
    memcpy(ThermostatId,RF_RxBuf+2,7);
    SaveFlag = PARA_SAVE;
    return RF_PAIR;
  }
  else if(RF_RxBuf[11] == 0xC3 && RF_RxBuf[12] == 0x02)  //��������
  {
    return RF_SHAKE;
  }
  else if(RF_RxBuf[11] == 0xC0 && RF_RxBuf[12] == 0x19)
  {
    Thermo_Buf[0] = RF_RxBuf[10] - 3;
    memcpy(Thermo_Buf+1,RF_RxBuf+14,Thermo_Buf[0]);
    return THERM_REPORT;
  }
  else if(RF_RxBuf[11] == 0xC0 && RF_RxBuf[12] == 0x24) 
  {
    return THERM_ACK;
  }
  else if(RF_RxBuf[11] == 0xC0 && RF_RxBuf[12] == 0x20)
  {
    return SET_BOILER_PARA;
  }
  else if(RF_RxBuf[11] == 0xAA && RF_RxBuf[12] == 0x10)
  {
    return SET_SOS;
  }
  
  return 0;
}

//=================================================
//RF��������
//=================================================
void ProcessRF(void)
{
  unsigned char re,cmd;
  unsigned int len;
  unsigned char txbuf[40];
  unsigned char rf_ansflag = 0;
  unsigned char is_netcmd = 1;   //Ĭ���ϱ���������
  unsigned int crccheck;

  if(ReceiveFlag)
  {
    ReceiveFlag = 0;
    memset(RF_RxBuf,0x00,BufferLenth);
    GetDataTime = GetInfoTime;//0x05;
    cmd = 0;
    re = PAN3029_ReceivePacket(RF_RxBuf);
    delayms(300);

    if(re && RF_RxBuf[10] <= (BufferLenth - 24))
    {
      cmd = ProcessFrame();
    
      if(cmd == RF_PAIR)           //C301
      {
        if(DeviceSta != MatchCode)//�յ�����������ǲ��Ǳ���û�д��ڶ���״̬ ���˳�
        {
          PAN3029_RX();
          return;
        }
        rf_ansflag = 1;
        is_netcmd = 0;
        ActiveTime = ActiveHoldTime;
        ///=========================����ڹ�¯�Ĵ���״̬
        RF_PairFlag = 0;
        RF_PairTime = 0;
        QK_SetSignleRegister(D_Status,(0<<8) | alarm_sta);   //��Գɹ�
        GetDataTime = 0; DeviceSta = 0;
      }
      else if(cmd == RF_SHAKE)    //C302
      {
        if(RF_RxBuf[14])                         //ʹ�������еĻ���״̬���п��ػ�
        {
          RELAY_ON;	
          FireFlag = 1;
          //GreenLed_ON;
        }
        else
        {
          RELAY_OFF;		
          //RedLed_ON;
          FireFlag = 0; 
        }
        rf_ansflag = 2;
        is_netcmd = 0;
        ActiveTime = ActiveHoldTime;
#ifdef NET_DELAY_TEST
        SendNbData("Receive:C302\n",13); 
#endif
//        if(XH_PumpFlag)
//        {
//          SendRecData(FireFlag,XH_PUMP);
//        }
      }
      else if(cmd == THERM_REPORT)  //C019
      {
        if(RF_RxBuf[32] <= TIMER_2S)
        {
          //ActiveTime = 0;
          ActiveTime = TIMER_2S;
        }
        else
        {
          ActiveTime = RF_RxBuf[32] - TIMER_2S;
        }
        ///==================================
        RF_RxBuf[11+RF_RxBuf[10]] -= RF_RxBuf[32];  //��������0�ϱ���У��λ��ȥ���ֽ�
        Thermo_Buf[19] = RF_RxBuf[32] = 0;          //��Ӧ������λ��0�����ݸ�����Э�飬���������¶����ݽ�������
        ///=================================
        if(RF_RxBuf[18])//ʹ�������еĻ�����п��ػ�
        {
          RELAY_ON;	
          FireFlag = 1;
          //GreenLed_ON;
        }
        else
        {
          RELAY_OFF;		
          //RedLed_ON;
          FireFlag = 0; 
        }
//        if(XH_PumpFlag)
//        {
//          SendRecData(FireFlag,XH_PUMP);
//        }
        ReissuedFlag = 0;
        ReissuedTime = 0;
  //      memcpy(HeratData,RF_RxBuf+1,RF_RxBuf[0]);//��ʱ������������
        rf_ansflag = 2;      //C019Ӧ����ʱ��У׼
#ifdef NET_DELAY_TEST
        SendNbData("Receive:C019\n",13); 
#endif
      }
      else if(cmd == THERM_ACK)    //C024
      {
        if(RF_RxBuf[9] & 0x80)    //Ӧ������
        {
          delayms(1);
        }
        else
        {
          len = RF_RxBuf[10];
          if(len < 4 || len > 23) { is_netcmd = 0; rf_ansflag = 0; }
          else {
          txbuf[0] = 0x68;
          txbuf[1] = 0x86;
          memcpy(txbuf+2,DeviceId,7);
          txbuf[9] = 0x01;
          txbuf[10] = (len+ 5) & 0xff;
          txbuf[11] = (len+ 5) >> 8;
          txbuf[12] = 0xF0;
          txbuf[13] = 0x0C;
          txbuf[14] = FrameSer;
          txbuf[15] = FacCode;
          txbuf[16] = GasHeater;
          memcpy(txbuf+17,RF_RxBuf+14,len-3);   //�������ݱ�ʾ��֡���3�ֽڲ���Ҫ
          crccheck = chkcrc(txbuf,len+14);

          txbuf[len+14] = (unsigned char)(crccheck & 0xFF);
          txbuf[len+15] = (unsigned char)(crccheck >> 8);

          txbuf[len+16] = 0x16;

          SendNbData(txbuf,len + 17);
          //is_netcmd = 0;
          rf_ansflag = 0;
#ifdef NET_DELAY_TEST
          SendNbData("Receive:C024\n",13);
#endif
          } /* end else len-valid */
        }
        is_netcmd = 0;
      }
      else if(cmd == SET_BOILER_PARA)//(RF_RxBuf[11] == 0xc0 && RF_RxBuf[12] == 0x20)
      {
        if(RF_RxBuf[21] & BIT4)    //����ʹ��λ��Ӧ��ԡˮ
          QK_SetSignleRegister(BathWater,RF_RxBuf[24]);
        else if(RF_RxBuf[21] & BIT5)  //��Ӧ��ůˮ
          QK_SetSignleRegister(HeatingWater,RF_RxBuf[24]);
        rf_ansflag = 1; 
#ifdef NET_DELAY_TEST
          SendNbData("Receive:water_temp\n",19); 
#endif
      }
      else if(cmd == SET_SOS)
      {
        alarm_sta |= ALARM_ICON;
        QK_SetSignleRegister(D_Status,alarm_sta);
        rf_ansflag = 1; 
        ActiveTime = ActiveHoldTime;    
        //DevSosRunFlag = true;           //sos��Ӧִ�м�ʱ��30s���
        DevSosRunTime = 0;
        UpLoadTime = UpLoadWaitingTime - TIMER_2S;  //2s���ϱ�
#ifdef NET_DELAY_TEST
          SendNbData("Receive:SOS\n",12); 
#endif
      }
      else
      {
        cmd = 0xFF;
      }
      
      if(rf_ansflag)  //��Ҫ��Ӧ���������ض�Ӧ��Ӧ��
      {
        RF_RxBuf[9] |= 0x80;
        
        memcpy(RF_RxBuf+2,ThermostatId,7);//memcpy(RF_RxBuf+3,WiFiMac,6);
        
        if(rf_ansflag == 1)
        {     
          memcpy(RF_RxBuf+14,DeviceId,7);
        }
        else if(rf_ansflag == 2)
        {
          RF_RxBuf[10] = 11;  //����ʱ�����ݣ����ĳ��ȣ���ԭ�ȵ�����֡���ȶ�9�ֽ�
          //RF_RxBuf[0]  = 23;      //�ܳ�
          RF_RxBuf[14] = SysYear;
          RF_RxBuf[15] = SysMonth;
          RF_RxBuf[16] = SysDay;
          RF_RxBuf[17] = SysHour;
          RF_RxBuf[18] = SysMinute;
          RF_RxBuf[19] = SysSecond;
          RF_RxBuf[20] = SysWeek ;
          
          if(ConnectFlag == CONNECK_OK)     //��ʾWiFi����
          {
            RF_RxBuf[21] = ONLINE;
          }
          else
          {
            RF_RxBuf[21] = OFFLINE;
          }
          RF_RxBuf[22] = BathWaterTargetTemp ;
          RF_RxBuf[23] = HeatBathWaterTargetTemp ;   //808AӦ������������ȡůˮ�趨�¶�
          RF_RxBuf[10] += 1;
        }
        ///=======================���������������WiFi״̬�ֽ�  
        //RF_RxBuf[0] += 2;   //Ӧ��֡����    Ӧ�����WiFi״̬ + ��ԡ�趨�¶�
        RF_RxBuf[10] += 1;   //�����򳤶�

      ///========================================
         //����У��
        RF_RxBuf[RF_RxBuf[10]+11] = 0;
        for(re=0;re<11+RF_RxBuf[10];re++)
        {
          RF_RxBuf[RF_RxBuf[10]+11] += RF_RxBuf[re];
        }
        RF_RxBuf[RF_RxBuf[10]+12] = 0x16;

        PAN3029_SendPacket(RF_RxBuf, RF_RxBuf[10]+13);
#ifdef NET_DELAY_TEST
        SendNbData("RF_Ack_send\n",12); 
#endif
        ///=================================
        delayms(100);
        if(XH_PumpFlag)
        {
          SendRecData(FireFlag,XH_PUMP);
        }
        ///=================================
      }
      
      if(is_netcmd && (ConnectFlag == CONNECK_OK) && (cmd != 0xFF))   //��������£� ������Ҫȫ���ϱ�������
      {
        //if(RF_RxBuf[11] == 0xC0 && RF_RxBuf[12] == 0x19)
        //{
        len = Thermo_Buf[0];    //�����ݳ���
        RF_RxBuf[0]  = 0x68;      //�޸ıڹ�¯Э��
        RF_RxBuf[1]  = 0x86;      //�޸ıڹ�¯Э��
        ///=================================
        memcpy(RF_RxBuf+2,DeviceId,7);
        ///================================
        RF_RxBuf[9]  = 0x01;
        
        RF_RxBuf[10] = (len + 12) & 0xff;   //�����ݳ���
        RF_RxBuf[11] = (len + 12) >> 8;
        
        RF_RxBuf[12] = 0xF0;      //��������������ݱ�ʾ�ϴ�
        RF_RxBuf[13] = 0x1C;
        RF_RxBuf[14] = FrameSer;
        RF_RxBuf[15] = FacCode;
        RF_RxBuf[16] = GasHeater;
        
        memcpy(RF_RxBuf+17,ThermostatId,7);
        memcpy(RF_RxBuf+24,Thermo_Buf+1,len);
        //}
        
        len = ((unsigned int)RF_RxBuf[11] << 8) + RF_RxBuf[10];
        memcpy(RF_RxBuf+2,DeviceId,7);
        
        crccheck = chkcrc(RF_RxBuf,len+12);

        RF_RxBuf[len+12] = (unsigned char)(crccheck & 0xFF);
        RF_RxBuf[len+13] = (unsigned char)(crccheck >> 8);
        RF_RxBuf[len+14] = 0x16;
        
        SendNbData(RF_RxBuf,len+15); 
#ifdef NET_DELAY_TEST
        SendNbData("Net_data_upload\n",16); 
#endif
      }
    }
    PAN3029_RX();   /* 处理完一帧后切回持续 RX，避免重新 Init 造成长时间不接收 */
  }
  ///============================================20250523��չ
  if(ReissuedTime >= TIMER_8S)   //�ط�һ�μ���,����ǰ�����ʽ���ͣ�֮ǰ6sʱ����Щ���ţ����ܻ���ֳ�ͻ
  {
    ReissuedFlag = 0;
    ReissuedTime = 0;
#ifdef NET_DELAY_TEST
    SendNbData("RF_send_again",13); 
#endif
    LDCMode = 1;
    PAN3029_SendPacket(ReSendBuf+1,ReSendBuf[0]);
    LDCMode = 0;
    PAN3029_RX();
  }
  ///============================================
}

unsigned char LoRaSendSatusToThermostat(void)
{
  unsigned char i;

  TxBuffer[0] = 0x68;
  TxBuffer[1] = 0x50;

  memcpy(TxBuffer+2,ThermostatId,7);

  TxBuffer[9] = 0x03;

  TxBuffer[10] = 0x37;      //len

  TxBuffer[11] = 0xC0;      //
  TxBuffer[12] = 0x23;
  TxBuffer[13] = FrameSer;      //ser

  TxBuffer[14] = FacCode;
  TxBuffer[15] = GasHeater; //type

  memcpy(TxBuffer+16,DeviceId,7);

  TxBuffer[23] = Csq;
  /////////////////////////////////////////////////////从24中读取数据循环发送
  TxBuffer[24] = 0x88;    //上报结束
  TxBuffer[25] = 0x01;    //包含一帧数据

  TxBuffer[26] = HexToBCD(SysYear);
  TxBuffer[27] = HexToBCD(SysMonth);
  TxBuffer[28] = HexToBCD(SysDay);
  TxBuffer[29] = HexToBCD(SysHour);
  TxBuffer[30] = HexToBCD(SysMinute);
  TxBuffer[31] = HexToBCD(SysSecond);

  TxBuffer[32] = (unsigned char)(WaterTotalVol & 0xFF);  //累计水量
  TxBuffer[33] = (unsigned char)(WaterTotalVol >> 8);
  TxBuffer[34] = (unsigned char)(WaterTotalVol >> 16);
  TxBuffer[35] = (unsigned char)(WaterTotalVol >> 24);

  TxBuffer[36] = (unsigned char)(GasTotalVol & 0xFF);    //累计气量
  TxBuffer[37] = (unsigned char)(GasTotalVol >> 8);
  TxBuffer[38] = (unsigned char)(GasTotalVol >> 16);
  TxBuffer[39] = (unsigned char)(GasTotalVol >> 24);

//    TxBuffer[40] = (unsigned char)(ElecTotalPower & 0x000F);  //累计电量
//    TxBuffer[41] = (unsigned char)(ElecTotalPower >> 8);
//    TxBuffer[42] = (unsigned char)(ElecTotalPower >> 16);
//    TxBuffer[43] = (unsigned char)(ElecTotalPower >> 24);
    //PowerUsingTime
  TxBuffer[40] = (unsigned char)(PowerUsingTime & 0xFF);  //累计电量
  TxBuffer[41] = (unsigned char)(PowerUsingTime >> 8);
  TxBuffer[42] = (unsigned char)(PowerUsingTime >> 16);
  TxBuffer[43] = (unsigned char)(PowerUsingTime >> 24);

  TxBuffer[44] = Dev_Status[0];
  TxBuffer[45] = Dev_Status[1];
  TxBuffer[46] = ErrorFlag;
  TxBuffer[47] = BathWaterTemp;
  TxBuffer[48] = BathWaterTargetTemp;
  TxBuffer[49] = HeatBathWaterTemp;
  TxBuffer[50] = HeatBathWaterTargetTemp;
  TxBuffer[51] = FlucTemp;
  TxBuffer[52] = CurrFlowRate;

  TxBuffer[53] = DevFacPara[0];
  TxBuffer[54] = DevFacPara[1];
  TxBuffer[55] = DevFacPara[2];
  TxBuffer[56] = DevFacPara[3];
  TxBuffer[57] = MinPower;
  TxBuffer[58] = RemoteCtrFlag;//MaxPower;
  TxBuffer[59] = LossCommTime;//FirePower;
  TxBuffer[60] = ProValveOut;

  TxBuffer[61] = Temp_Compensation_Switch;//UserId;
  TxBuffer[62] = SoftWareVer;//DevSoftVer[0];
  TxBuffer[63] = HardWareVer;//DevSoftVer[1];
  TxBuffer[64] = SensorPara;
  TxBuffer[65] = DevType;

  TxBuffer[66] = 0x00;

  for(i=0;i<66;i++)
  {
    TxBuffer[66] += TxBuffer[i];
  }

  TxBuffer[67] = 0x16;

  //SX1278SendPacket(TxBuffer,68);
  //SX1278SendPacket(TxBuffer,68);
  
  SendNbData(TxBuffer,68);
    
  return 1;
}

unsigned char ReadThermostatInfo(unsigned char n)
{
  unsigned char i;
  unsigned char txbuf[25];
  
  txbuf[0] = 0x68; 
  txbuf[1] = 0x50;  //֡ͷ
  for(i=0;i<7;i++)  //�����¿������
  {
    txbuf[i+2] = RoomThermostat[n-1].id[i];
  }
  txbuf[9] = 0x04;  
  txbuf[10] = 10;
  txbuf[11] = 0xC0; 
  txbuf[12] = 0x2d;  
  txbuf[13] = 0x01; //ser
  for(i=0;i<7;i++)  //�������¿������
  {
    txbuf[i+14] = DeviceId[i];
  }
  txbuf[21] = calSum(txbuf,21);
  txbuf[22] = 0x16;
  LDCMode = 0x01;    //ǰ������
  PAN3029_SendPacket(txbuf,23);
  //PAN3029_SendPacket(txbuf,23);
  LDCMode = 0x00;
  PAN3029_RX();
  ReadBeginFlag = 1;
  GetThermInfoOverTime = 0;
  return 1;
}

void LoRaSendSettingTemp(unsigned char *id,unsigned char temp)
{
  unsigned char i;
  unsigned char txbuf[20];
  
  txbuf[0] = 0x68;
  txbuf[1] = 0x50;
  
  for(i=0;i<7;i++)
  {
    txbuf[i+2] = id[i];
  }
  
  txbuf[9]  = 0x03;
  txbuf[10] = 0x06;
  txbuf[11] = 0xC0;
  txbuf[12] = 0x12;
  txbuf[13] = 0x01;   //ser
  txbuf[14] = BIT7;   //�ֶ��¶�BIT7���ֶ��¶ȣ� BIT6����ʱ�¶ȣ�BIT5��У׼�¶ȣ�
                     //BIT4�������¶ȣ�BIT3��������ˮ�¶ȣ�BIT2��ȡůˮ�¶ȣ�BIT1�������¶ȣ�BIT0����������¶ȣ�
  txbuf[15] = temp;// / 10;
  txbuf[16] = 0;//temp % 10;
  
  txbuf[17] = calSum(txbuf,17);
  txbuf[18] = 0x16;
  
  LDCMode = 1;
  PAN3029_SendPacket(txbuf,19);
  //PAN3029_SendPacket(txbuf,19);
  LDCMode = 0;
  PAN3029_RX();
}

void SendRecData(unsigned char flag,unsigned char type) //发送接收器命令
{
  unsigned char i;
  unsigned char txbuf[30];
  
  txbuf[0] = 0x68;
  txbuf[1] = 0x50;
      
  //memcpy(txbuf+2,RevId,7);
  if(type == XH_PUMP)
  {
    memcpy(txbuf+2,XH_PumpId,7);
  }
  else
  {
    memcpy(txbuf+2,RevId,7);
  }
    
  txbuf[9] = 0x04;

  txbuf[11] = 0xC0;      //
  txbuf[12] = 0x28;
    
  txbuf[13] = 0;   //ser
  
  memcpy(txbuf+14,DeviceId,7);
    
  if(flag)
    txbuf[21] = 0x55;   
  else
    txbuf[21] = 0x99;  
    
  txbuf[22] = 0;

  for(i=0;i<22;i++)
  {
    txbuf[22] += txbuf[i];
  }

  txbuf[23] = 0x16;
    
  LDCMode = 0;
  ActiveTime = TIMER_3S;
  
  PAN3029_SendPacket(txbuf,24);
  PAN3029_SendPacket(txbuf,24);
  PAN3029_RX();
}

