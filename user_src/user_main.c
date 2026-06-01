/*
 * user_main.c
 *
 *  Created on: 2021年5月24日
 *      Author: Halley
 */

#include "../usr_lib/main.h"

//#define QK_TEST    //锜科主板测试，正式程序屏蔽

void software_reset(void);

void LLS_ScheduleCheck(void) ;
void InitSysVar(void);
unsigned char GetSetTemp(void);
extern unsigned char ComBuf[30];

void InitSysVar(void)
{
  SysYear   = UploadStartTime.year;
  SysMonth  = UploadStartTime.month;
  SysDay    = UploadStartTime.day;
  SysHour   = UploadStartTime.hour;
  SysMinute = UploadStartTime.min;
  SysSecond = UploadStartTime.sec;

  ErrFirstHappenFlag = 0x00;
  GetDataTime = GetInfoTime;
  TimingSaveFlag = 0;  //定时存储允许标志，联网更新时间后，才可以正常定时保存

  AutoConnectFlag = 0x00;
  SysRunStatus = 0x00;
  PreRunStatus = -1;
  SetUpFlag = 0;
  ThermostatControlFlag = 0x00;
  ThermostatControlSer = 0x00;
  
  PreTimeFrame = CurrTimeFrame = 0;   //ʱ�κų�ʼ��
  BathWaterUseFlag = 0;
  BathWaterUseTime = 0;
  SetTempFlag = 1;
}
//unsigned int lednum;
//波特率：9600
//NB  -----  uart9     ,  dev----uart0
//unsigned char ReadTimes;
void PowerOn_HanShake(void)
{
  unsigned char buf[10];
  buf[0] = 6; 
  buf[1] = 0x68;
  buf[2] = 0xE1;
		//buf[3] = DeviceId[0];
		//buf[4] = DeviceId[1];
		//buf[5] = FireFlag;//RF_RxBuf[5];
  buf[3] = ThermostatId[2];
  buf[4] = ThermostatId[3];
  buf[5] = ThermostatId[4];
  buf[6] = 0xAA;
  
  LDCMode = 1;
  PNA3029_SendPacket(buf,7);
  LDCMode = 0;
  PNA3029_Init();
}

void user_main(void)
{
  bsp_io_level_t  level;
  
  unsigned char i,re = 0;
  unsigned char tempbuf[35];
  //unsigned char status;

  system_init();

  LoadSysPara();

//  if((ThermostatNum > 0 && ThermostatNum <= 5) || ThermEnableFlag)  //���¿��������ñպ�
//  {
//    RELAY_OFF;   
//  }
//  else
//  {
//    RELAY_ON;    //CAT1¯���ȱ�֤����ʹ��
//  }

  InitSysVar();
  //=================================
  R_IOPORT_PinRead(&g_ioport_ctrl, BSP_IO_PORT_02_PIN_12, &level);  //��ȡ�˿�

  if(!level)    //��ȡ�˿ڵ�ƽ���ж��Ƿ��а���
  {
    SetUpFlag = 0x01;
    RedLed_ON;
  }

  while(SetUpFlag)
  {
    WDT_FEED;
    
    if(ComValid)
    {
      ComValid = 0x00;
      GreenLed_ON;
      memcpy(tempbuf,ComBuf,20);

      tempbuf[9] |= 0x80;

      if(tempbuf[10] == 0xBA && tempbuf[11] == 0x01)
      {
        memcpy(tempbuf+2,DeviceId,7);
      }
      else if(tempbuf[10] == 0xBA && tempbuf[11] == 0x02)
      {
        memcpy(DeviceId,tempbuf+2,7);
        SavePara();
      }

      tempbuf[14] = 0x00;

      for(i=0;i<14;i++)
      {
        tempbuf[14] += tempbuf[i];
      }

      tempbuf[15] = 0x16;

      R_SCI_UART_Write(&g_uart0_ctrl, tempbuf, 16);
      GreenLed_OFF;
//======================================================����Э��Ӧ���ִ�й�װ����
      tempbuf[0] = 0x09;
      tempbuf[1] = 0xAA;
      memcpy(tempbuf+2,DeviceId,7);
      tempbuf[9] = 0x55;
      ///===============================���ü���
      LDCMode = 0;
      ActiveTime = ActiveHoldTime;
      ///===============================
      PNA3029_SendPacket(tempbuf,tempbuf[0]+1);   //无线发送
      PNA3029_RX();
      GreenLed_ON;
      RedLed_OFF;
              
      RELAY_ON;
      delayms(1000);
      RELAY_OFF;
//========================================================
    }
    
    if(ReceiveFlag)
    {
      ReceiveFlag = 0x00;
      PNA3029_ReceivePacket(RF_RxBuf);
      delayms(100);

      if(RF_RxBuf[1] == 0xBB && RF_RxBuf[0] < BufferLenth && RF_RxBuf[RF_RxBuf[0]] == 0x66)
      {
        for(i=0;i<7;i++)
        {
          if(DeviceId[i] != RF_RxBuf[i+2])
          {
            GreenLed_OFF;
            RedLed_OFF;
            break;
          }
        }

        if(i == 7)    //�ж�Ӧ����ȷ
        {
          GreenLed_ON;
          RedLed_ON;

          RELAY_ON;
          delayms(1000);
          RELAY_OFF;
        }
      }
      PNA3029_Init();
    }
  }
//====================================================================
  switch(UpLoadCycleUint)    //��ȡ�ϱ�ʱ��
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
  switch(DataSaveCycleUint)  //��ȡ�洢ʱ����
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
//=======================================================
  UpLoadTime = UpLoadWaitingTime;    //�ϵ�����
//=====================================�洢��ʽ==
  //tempbuf：  读出前4字节，前2字节，存储总数，后2字节，起始地址，高字节在前
  AT24_readbyte(SaveInfoAddr,tempbuf,4); //读取存储信息
  delayms(3);

  DataSaveNum = ((unsigned int)tempbuf[0] << 8) | tempbuf[1];      //数据总数
  CurrSaveAddr = ((unsigned int)tempbuf[2] << 8) | tempbuf[3];     //存储地址

  if(DataSaveNum == 0x00 || DataSaveNum >= MaxSaveNum)    //
  {
    DataSaveNum  = 0x00;
    CurrSaveAddr = DataSaveAddr;    //0x80

    re = 0x77;
  }
  if((CurrSaveAddr > DataSaveNum * OneFrameLen + DataSaveAddr) || (CurrSaveAddr <= DataSaveAddr))   //�洢�����쳣
  {
    DataSaveNum = 0x00;
    CurrSaveAddr = DataSaveAddr;
    re = 0x77;
  }

  if(re == 0x77)    //�������ݣ����³�ʼ��
  {
    tempbuf[0] = 0x00;
    tempbuf[1] = 0x00;
    tempbuf[2] = DataSaveAddr >> 8; //0x00;
    tempbuf[3] = DataSaveAddr & 0xff;//0x80;

    AT24_writebyte(SaveInfoAddr,tempbuf,4);
    delayms(3);
  }
  
  RELAY_ON;
  PowerOnFlag = true;
///==========������������ͼ��ָʾ  
//  QK_SetSignleRegister(11,0x01);
//  QK_SetSignleRegister(11,0x02);
//  QK_SetSignleRegister(11,0x04);
//  QK_SetSignleRegister(11,0x08);
//  //QK_SetSignleRegister(11,0x0f);
//  QK_SetSignleRegister(11,0);
//  QK_SetSignleRegister(11,~0x01);
//  QK_SetSignleRegister(11,~0x02);
//  QK_SetSignleRegister(11,~0x04);
//  QK_SetSignleRegister(11,~0x08);
///=====================================
//  while(1)     //����wor
//  {
//    LDCMode = 1;
//    SX1278SendPacket("AABBCC112233445566",18);
//    SX1278_RX();
//    delayms(300);
//  }
//#if 0
//  DeviceId[0] = 0x11;
//  DeviceId[1] = 0x30;
//  DeviceId[2] = 0x00;
//  DeviceId[3] = 0x03;
//  DeviceId[4] = 0x30;
//  DeviceId[5] = 0x32;
//  DeviceId[6] = 0x15;
//#endif  
  while(1)
  {
    UpdateCurrTime();    //���µ�ǰʱ��

    if(GetDataTime >= GetInfoTime)   //���5s��ȡһ��������Ϣ
    {
      GetDataTime = 0x00;
      QK_ReadAllRegisters();

      if((PreRunStatus != SysRunStatus) || (BathWaterTargetTemp != PreBathWaterTargetTemp) || (HeatBathWaterTargetTemp != PreHeatBathWaterTargetTemp))  //�ȶ�ʵ�����ݣ��б仯���ϱ�
      {
        PreRunStatus = SysRunStatus;
        //RF_HandShakeTime = TIME_10M;      //�����ϱ�ʱ�䵽
        UpLoadTime = UpLoadWaitingTime;
        
//        if(SysRunStatus & BIT2)   //������ԡˮ
//        {
//          BathWaterUseFlag = 1;
//        }
        PreBathWaterTargetTemp = BathWaterTargetTemp;
        PreHeatBathWaterTargetTemp = HeatBathWaterTargetTemp;
      }
      ///==============================================
      WDT_FEED;   //����������ι��������ÿ���ȡһ����Ϣ
      ///==============================================
	  }

    ProcessRevData();   //������������

    ProcessRF();       //������������

#if 1
    if(ErrorFlag)   /////////////////////////////////////////�й��ϣ���ʱ�ϱ�
    {
      RedLed_ON;    //�����

      if(!ErrFirstHappenFlag)
      {
        ErrFirstHappenFlag = 0x01;
        //RF_HandShakeTime = TIME_15M;//TIME_10M;
        UpLoadTime = UpLoadWaitingTime - 5;//10;   //WiFi�ϱ�
      }
    }
    else
    {
      RedLed_OFF;
      ErrHoldTime = 0x00;
      ErrFirstHappenFlag = 0x00;
    }
#endif
    
    if(KeyPressFlag)     //�ж����ް���
    {
      R_IOPORT_PinRead(&g_ioport_ctrl, BSP_IO_PORT_02_PIN_12, &level);

      if(KeyPressHoldTime >= TIME_3S)  //��������3s�������ϱ�
      {
        ErrHoldTime = 0x00;
        KeyPressFlag = 0x00;
        KeyPressHoldTime = 0x00;
        UpLoadTime = UpLoadWaitingTime;  //�����ϱ�
        ConnectHoldTime = 0x00;
        
        GreenLed_ON;
        delayms(200);
        GreenLed_OFF;
      }
      else
      {
        if(level)     //�ߵ�ƽ
        {
          ErrHoldTime = 0x00;
          KeyPressFlag = 0x00;
          KeyPressHoldTime = 0x00;
        }
      }
    }
/////////////////////////////////////////////////////////////
#if 1 
    if(UpLoadTime >= UpLoadWaitingTime)   //�ϱ�ʱ�䵽��������磬��ʼ�ϱ�����
    {
      UpLoadTime = 0x00;  
   
      if(!ConnectFlag)     //û��������������
      {
        re = GprsConnect();
        
        if(PowerOnFlag)   //�״��ϵ�
        {
          PowerOnFlag = false;
          PowerOn_HanShake();    //�ϵ���ȴ�����������
        }
      }
      else
      {
        SendUpLoadFrame();    //����������ֱ���ϱ�
      }
    }
#endif
    if(SaveFlag)    //���ݴ洢
    {
      if(SaveFlag == HISDATA_SAVE)  //==1
      {
        SaveHisData();  //��ʷ��¼
      }
      else  //==2
      {
        SavePara();     //ϵͳ����
      }
      SaveFlag = NULL_SAVE;
    }
    
    if(ConnectHoldTime >= TIMER_1M)  //������һ����һ��
    {
      ConnectHoldTime = 0;      
      SendHeartBeatFrame();
      HeartBeatSendFlag = 0x01;
      HeartBeatSendNum ++;
    }
    
    if(HeartBeatSendFlag)   //������������ʼ��ʱ
    {
      if(ConnectHoldTime >= TIMER_6S)  //��������6s֮����Ҫ�յ�Ӧ�𣬷����ط�����
      {
        ConnectHoldTime = TIMER_1M;
        HeartBeatSendFlag = 0;
      }
    }
    
    if(HeartBeatSendNum >= 3 || (NetStatusFlag == 4))   //3������ʧ�ܣ���Ϊ����Ͽ���NetStatusFlag = 4��socket�Ͽ�
    {
      HeartBeatSendNum = 0x00;
      ConnectHoldTime = 0x00;  
      ConnectFlag = NO_CONNECT;
      NetStatusFlag = 0;
      GreenLed_OFF;
      QK_SetSignleRegister(D_Status,(alarm_sta & ~NET_ICON));
      Nb_ModuleClose();  
      UpLoadTime = UpLoadWaitingTime;  //�ر������������������
    }
    
    if(RF_HandShakeTime >= TIME_15M)   //����15min�ϱ�һ�Σ�ͬʱ�𵽼�����ӵ����ã�ͬ����ȡ�ӻ�����
    {
      RF_HandShakeTime = 0x00;
      //LoRaSendSatusToThermostat();
      //SX1278_RX();
      UpLoadTime = UpLoadWaitingTime;   //
    }

    if(UpgradeOverTime >= TIMER_15S)    //�������ݰ��ȴ�15sû���յ�Ӧ����Ϊ���γ�ʱ
    {
      UpgradeOverTime = 0x00;
      UpgradeFlag = 0x00;
      ConnectHoldTime = 0x00;
      UpgradeNum ++;
      SendRequestFrame(0);
    }

    if(UpgradeNum >= 3)       //�������ݷ��Ͳ�����3��
    {
      UpgradeNum = 0x00;
      UpgradeOverTime = 0x00;
      UpgradeFlag = 0x00;
      CurrFrameSer = 0x01;
      //Nb_ModuleClose();
    }
    
    
    if(DevSosRunTime >= TIMER_30S + 2)   //��2s
    {
      DevSosRunFlag = false;
      DevSosRunTime = 0;
      
      alarm_sta &= ~ALARM_ICON;
      QK_SetSignleRegister(D_Status,alarm_sta);
    }
    
#if 0
    if(GetThermInfoFlag)   //��ʼ��ȡ�¿�������
    {
      GetThermInfoFlag = 0;
      //=====================//��ȡ�¿�����Ϣ
      //MS_Flag = RoomThermostat[i].m_s;
      if(CurrThermNo <= ThermostatNum)   //û�дﵽ�¿����������ޣ�������ȡ��һ̨
      {
        ReadThermostatInfo(CurrThermNo);
      }
      //====================================
    }
    
    if(SetTempFlag)    //���¿�������ʱ����趨�¶�
    {
      if(SetTempFlag != 2)   //�趨�¶ȹ��ܳ��⣬������Ҫ��ȡ�趨�¶�
      {
        SetTemper = GetSetTemp();
      }
      
      if(SetTemper != PreSetTemper)   //�¶��б仯���·�������£���ʡ����
      {
        LoRaSendSettingTemp(RoomThermostat[0].id,SetTemper);
        PreSetTemper = SetTemper;
        //delayms(300);
        if(ConnectFlag)
        {
          GetThermInfoFlag = 1;  //�����Ժ��ϱ������Ϣ
          CurrThermNo = 0;
        }
      }
      SetTempFlag = 0;
    }
#endif     
    //�ڹ�¯û�й��ϣ�����������ˮ�豸���Ż�һֱ�������ˮ״̬
    if(NonColdWateStatus && !ErrorFlag) 
    { 
      EnergySavingFlag = CheckEnergySaving();  //����ʱ����ж�
      LLS_ScheduleCheck();   //ʵʱ���ģʽ��ʱ��
      if(LLS_StartFlag == 2)  //�ر�ˮ��ʱ�䵽
      {
        PumpRunFlag = 0;
        LLS_StartFlag = 0;
        SendRecData(LLS_OFF,LLS_PUMP);
        LLS_ReSendFlag = 1;       //�ط���ʱ��ʼ
      }
    }
  }
}

void LLS_ScheduleCheck(void)  //���ģʽ���ж��Ƿ�������ˮ
{
  unsigned char i;
  unsigned char flag = 0;

  if(LLS_Pump.run_mode == CUISER_MODE) //Ѳ��ģʽ
  {
    if(LLS_CuiserTime >= LLS_Pump.cuiser_interval * 60)   //Ѳ��ʱ�䣬�Է���Ϊ��λ
    {
      LLS_CuiserTime = 0;
      flag = 1;
    }
  }
  else if(LLS_Pump.run_mode == SCHEDULE_MODE)  //ԤԼģʽ
  {
    if(ScheduleCheckFlag)   //��ʱ����ÿ���Ӽ��һ��
    {
      ScheduleCheckFlag = 0;
      for(i=0;i<LLS_Pump.schedule_num*2;i+=2)
      {
        if((SysHour == LLS_Pump.schedule[i]) && (SysMinute == LLS_Pump.schedule[i+1]))  //ʱ����ж�
        {
          if(!PumpRunFlag)  //ˮ��û������
          {
            flag = 1;
          }
          break;
        }
      }
    }
  }
    
  if(!EnergySavingFlag || RunOnceFlag)  //�ǽ���ʱ��ο��Կ���ˮ�ã����ߵ㶯ִ��
  {
    if(flag || RunOnceFlag)
    {
      RunOnceFlag = 0;
      PumpRunFlag = 1;
      SendRecData(LLS_ON,LLS_PUMP);
      LLS_StartFlag = 1;
      LLS_HoldTime = 0;
      WaterTempCheckFlag = 1;
      LLS_ReSendFlag = 1;       //�ط���ʱ��ʼ
    }
  }
  
  if(LLS_ReSendTime >= TIME_5S)   //5s�������ط�һ�������֤�ɹ���
  {
    LLS_ReSendFlag = 0;
    LLS_ReSendTime = 0;
    SendRecData(PumpRunFlag,LLS_PUMP);//old mode ---(LLS_ReSendFlag - 1);   //LLS_ReSendFlag - 1 = LLS_ON  ��   LLS_OFF
  }
}

void system_init(void)
{
  fsp_err_t err = FSP_SUCCESS;
  //rtc_time_t systime;
//    APP_PRINT("\r\n*******************************************");
//    APP_PRINT("\r\n                System Init                ");
//    APP_PRINT("\r\n*******************************************");
//  R_DEBUG->DBGSTOPCR_b.DBGSTOP_WDT = 0;      //�˹��ܱ�֤�ڷ���ʱ��wdt������������ʽ������
#if 1
  err =  R_WDT_Open(&g_wdt0_ctrl, &g_wdt0_cfg);    //wdt init
  if(FSP_SUCCESS != err)
  {
      //APP_ERR_PRINT("\r\nIO Open API failed");
  }
  WDT_FEED;
#endif
//  while(1);
//    err = R_CGC_Open(&g_cgc0_ctrl, &g_cgc0_cfg);
//    if(FSP_SUCCESS != err)
//    {
//        APP_ERR_PRINT("\r\nCGC Open API failed");
//    }
//    err = R_CGC_ClockStart(&g_cgc0_ctrl, CGC_CLOCK_LOCO, NULL);
//    if(FSP_SUCCESS != err)
//    {
//        APP_ERR_PRINT("\r\nLOCO Start failed");
//    }
 //fsp_err_t R_IOPORT_Open (ioport_ctrl_t * const p_ctrl, const ioport_cfg_t * p_cfg)
  err =  R_IOPORT_Open(&g_ioport_ctrl, &g_bsp_pin_cfg);    //启用端口配置
  if(FSP_SUCCESS != err)
  {
      //APP_ERR_PRINT("\r\nIO Open API failed");
  }
//================================================
//    err = R_RTC_Open (&g_rtc0_ctrl, &g_rtc0_cfg);
//    if(FSP_SUCCESS != err)
//    {
//        APP_ERR_PRINT("\r\nRTC Open API failed");
//    }
//    err = R_RTC_PeriodicIrqRateSet(&g_rtc0_ctrl, RTC_PERIODIC_IRQ_SELECT_1_SECOND);
//    if(FSP_SUCCESS != err)
//    {
//        APP_ERR_PRINT("\r\nRTC Set Period failed");
//    }
//================================================

  err = R_AGT_Open(&g_timer0_ctrl, &g_timer0_cfg);     //timer--500ms
  if(FSP_SUCCESS != err)
  {
     // APP_ERR_PRINT("\r\nAGT Open API failed");
  }
  err = R_AGT_Start(&g_timer0_ctrl);                   //start
  if(FSP_SUCCESS != err)
  {
     // APP_ERR_PRINT("\r\nAGT Start failed");
  }

  err = R_SCI_UART_Open(&g_uart0_ctrl, &g_uart0_cfg);  //uart0 9600  串口读写+主板通信
  if(FSP_SUCCESS != err)
  {
      //APP_ERR_PRINT("\r\nUART0 Open API failed");
  }

  err = R_SCI_UART_Open(&g_uart9_ctrl, &g_uart9_cfg);  //uart9 9600   //NB 
  if(FSP_SUCCESS != err)
  {
     // APP_ERR_PRINT("\r\nUART9 Open API failed");
  }
  
//#ifndef XIAO_RUI_NO_RF
  PNA3029_Init();   //LoRa,RX（PNA3029 内部完成 Settings 后已自动进入 RX）
//#endif
  //intp6
  err = R_ICU_ExternalIrqOpen (&g_external_irq6_ctrl,&g_external_irq6_cfg);
  if(FSP_SUCCESS != err)
  {
       // APP_ERR_PRINT("\r\niCU Open API failed");
  }
  R_ICU_ExternalIrqEnable(&g_external_irq6_ctrl);

  //intp3
  err = R_ICU_ExternalIrqOpen (&g_external_irq3_ctrl,&g_external_irq3_cfg);
  if(FSP_SUCCESS != err)
  {
     // APP_ERR_PRINT("\r\niCU Open API failed");
  }
  R_ICU_ExternalIrqEnable(&g_external_irq3_ctrl);

  EI;      //ʹ���ж�
}

unsigned char GetSetTemp(void)
{
  unsigned char i,mode;
  unsigned char st,timeflag;
  
  timeflag = 0;
  mode = MainThermostatMode;
  
  if(mode > ANTIFREEZE_RUN)     //mode������Ĭ��Ϊ�ֶ�
  {
    mode = MANVAL_RUN;
  }
  
  if(mode == MANVAL_RUN)   //�ֶ�
  {
    st = ManualTemper;
  }
  else if (mode == ANTIFREEZE_RUN)    //����
  {
    st = AntifreezTemper;
  }
  else       //�Զ�
  {
    for(i=0;i<MainThermostatScheduleNum-1;i++)   //���ж�ǰ����ʱ�Σ����һ��ʱ�����⴦��
    {
      timeflag = 0;   //ÿ�ж�һ������־��0
      //�ж�ʱ�εķ��������ж��Ƿ����ǰһʱ�Σ����ж��Ƿ�С�ں�һʱ��
      if((SysHour > MainThermostatSchedule[i].hour) || ((SysHour == MainThermostatSchedule[i].hour) && (SysMinute >= MainThermostatSchedule[i].min)))
      {
        timeflag = 1;
      }
      if(timeflag && ((SysHour < MainThermostatSchedule[i+1].hour) || ((SysHour == MainThermostatSchedule[i+1].hour) && (SysMinute < MainThermostatSchedule[i+1].min))))
      {
        timeflag = 2;
        break;
      }
    }
    
//    if(timeflag != 2)   //�ж���ɣ�û�к���ʱ�Σ���Ϊ�����һ��ʱ��
//    {
//      st = MainThermostatSchedule[MainThermostatScheduleNum-1].temper;
//    }
//    else
//    {
//      CurrTimeFrame = i + 1;               //��ȡ��ǰʱ��
//      if(CurrTimeFrame != PreTimeFrame)   //ʱ�β�ͬ��������趨�¶�
//      {
//        PreTimeFrame = CurrTimeFrame;
//        st = MainThermostatSchedule[i].temper;
//      }
//      else    
//      {
//        st = SetTemper;    //ʱ��û�б仯���趨�¶Ȳ���
//      }
//    }
    
    CurrTimeFrame = i + 1;               //��ȡ��ǰʱ��
    if(CurrTimeFrame != PreTimeFrame)   //ʱ�β�ͬ��������趨�¶�
    {
      PreTimeFrame = CurrTimeFrame;
      
      if(timeflag != 2)   //�ж���ɣ�û�к���ʱ�Σ���Ϊ�����һ��ʱ��
      {
        st = MainThermostatSchedule[MainThermostatScheduleNum-1].temper;
      }
      else  //������������ȡ��ǰʱ���¶�
      {
        st = MainThermostatSchedule[i].temper;
      }
    }
    else
    {
      st = SetTemper;
    }
  }
  return st;
}

//void software_reset(void)
//{
//  NVIC_SystemReset();
//}


