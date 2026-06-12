/*
 * PAN3029.c
 *
 *  Created on: 2026-05-14
 *      Author: ported from reference/user
 *
 * PAN3029 LoRa 收发驱动（替换 SX1278）。函数名与参考一致：
 *   PAN3029_Init / PAN3029_RX / PAN3029_Sleep / PAN3029_CADInit /
 *   PAN3029_ReceivePacket / PAN3029_SendPacket / PAN3029_Reset
 *
 * 通信参数（必须与现场温控器/水泵接收器一致）：
 *   频点  = 477MHz + LoRaChannel × 250kHz  （默认 LoRaChannel=10 → 479.5MHz）
 *   带宽  = 125kHz   扩频因子 = SF9   编码率 = 4/5
 *   同步字 = 0x12   CRC 关  IQ 不反转
 * 主循环判定 LDCMode || !ActiveTime 时切换 4.3s 长前导，用以唤醒
 * 处于低占空比休眠的对端节点（沿用原 SX1278 LDC 唤醒策略）。
 */

#include "../usr_lib/sys.h"
#include "../usr_lib/PAN3029.h"

/* PAN3029 内部状态机寄存器值 */
#define PNA_RF_STATE_DEEPSLEEP  0x00
#define PNA_RF_STATE_SLEEP      0x01
#define PNA_RF_STATE_STB1       0x02
#define PNA_RF_STATE_STB2       0x03
#define PNA_RF_STATE_STB3       0x04
#define PNA_RF_STATE_TX         0x05
#define PNA_RF_STATE_RX         0x06

#define PNA_RF_IRQ_TX_DONE      0x01
#define PNA_RF_IRQ_RX_TIMEOUT   0x02
#define PNA_RF_IRQ_CRC_ERR      0x04
#define PNA_RF_IRQ_RX_DONE      0x08

#define PNA_RF_TX_SINGLE        0x00
#define PNA_RF_RX_CONTINUOUS    0x02

#define PNA_SPI_CHECK_OK           0x00
#define PNA_SPI_CHECK_SYNC_ERR     0x01
#define PNA_SPI_CHECK_PREAMBLE_ERR 0x02

#define PNA_GPIO_TX             0
#define PNA_GPIO_RX             10
#define PNA_GPIO_CAD_IRQ        11

#define PNA_PAGE_SYS            0
#define PNA_PAGE_PKT            1
#define PNA_PAGE_AGC            2
#define PNA_PAGE_LORA           3

#define PNA_REG_PAGE            0x00
#define PNA_REG_FIFO            0x01
#define PNA_REG_RF_STATE        0x02
#define PNA_REG_ANALOG_CTRL     0x04
#define PNA_REG_SPI_MODE        0x1A
#define PNA_REG_LO_PARAM        0x3D

#define PNA_SYS_REG_PA_CTRL             0x4F
#define PNA_SYS_REG_DIO_MODE            0x58
#define PNA_SYS_REG_CAD_CTRL            0x5E
#define PNA_SYS_REG_GPIO_IN_EN_L        0x63
#define PNA_SYS_REG_GPIO_IN_EN_H        0x64
#define PNA_SYS_REG_GPIO_OUT_EN_L       0x65
#define PNA_SYS_REG_GPIO_OUT_EN_H       0x66
#define PNA_SYS_REG_GPIO_LEVEL_L        0x67
#define PNA_SYS_REG_GPIO_LEVEL_H        0x68
#define PNA_SYS_REG_IRQ_FLAGS           0x6C
#define PNA_SYS_REG_VCO_START           0x40
#define PNA_SYS_REG_TX_POWER_RAMP       0x1E
#define PNA_SYS_REG_PA_CFG              0x45
#define PNA_SYS_REG_PA_MODE             0x46
#define PNA_SYS_REG_PA_TRIM             0x4B

#define PNA_PKT_REG_PAYLOAD_LEN         0x0C
#define PNA_PKT_REG_IQ_CFG              0x0E
#define PNA_PKT_REG_CAD_THRESHOLD       0x0F
#define PNA_PKT_REG_MODE_CFG            0x25
#define PNA_PKT_REG_CAD_DONE_CFG        0x35
#define PNA_PKT_REG_RX_LEN              0x7D

#define PNA_AGC_REG_ENABLE              0x06
#define PNA_AGC_REG_TABLE_START         0x0A
#define PNA_AGC_REG_FINAL               0x34
#define PNA_AGC_REG_LORA_GAIN           0x3F
#define PNA_INFO_REG_DATA               0x3B
#define PNA_INFO_REG_CTRL               0x3E
#define PNA_INFO_REG_DCDC               0x3D
#define PNA_INFO_UNLOCK_BIT             0x08

#define PNA_LORA_REG_MODE               0x06
#define PNA_LORA_REG_FREQ_START         0x09
#define PNA_LORA_REG_BW_CR              0x0D
#define PNA_LORA_REG_SF_CRC             0x0E
#define PNA_LORA_REG_SYNC_WORD          0x0F
#define PNA_LORA_REG_MODEM_OPT          0x12
#define PNA_LORA_REG_PREAMBLE_L         0x13
#define PNA_LORA_REG_PREAMBLE_H         0x14
#define PNA_LORA_REG_FA_FB_START        0x15
#define PNA_LORA_REG_PA_RAMP            0x22
#define PNA_LORA_REG_REGULATOR          0x24
#define PNA_LORA_REG_SLEEP_CTRL         0x26

#define PNA_REG_BIT_PA_ENABLE           0x08
#define PNA_REG_BIT_SLEEP_DIGITAL       0x20
#define PNA_REG_BIT_CAD_DISABLE         0x40
#define PNA_REG_BIT_AGC_DISABLE         0x01
#define PNA_REG_BIT_DIO_MODE            0x04
#define PNA_REG_BIT_PA_MODE             0x04
#define PNA_REG_BIT_LORA_TX_SINGLE      0x04
#define PNA_REG_MASK_LORA_RX_MODE       0x03
#define PNA_REG_MASK_IQ                 0x60
#define PNA_REG_MASK_BW                 0xF0
#define PNA_REG_MASK_CR                 0x0E
#define PNA_REG_MASK_SF                 0xF0
#define PNA_REG_MASK_CRC                0x08
#define PNA_REG_MASK_LDR                0x08
#define PNA_REG_MASK_MODE0              0xF0
#define PNA_REG_MASK_CRC_IRQ            0x08
#define PNA_REG_MASK_MODEM_OPT_2        0x04
#define PNA_REG_MASK_MODEM_OPT_4        0x10

#define PNA_RX_PEEK_LEN                 8

/* 调试可观测变量 */
unsigned char PAN3029SpiOk;
unsigned char PAN3029SpiCheckReg;
unsigned char PAN3029SpiCheckError;
unsigned char PAN3029LastIrq;
unsigned char PAN3029TxDone;
unsigned char PAN3029TxUseLongPreamble;
unsigned char PAN3029TxLen;
unsigned int  PAN3029TxWaitMs;
unsigned int  PAN3029TxElapsedMs;
unsigned int  PAN3029TxTimeoutCount;
unsigned char PAN3029CalMarker;
unsigned char PAN3029CalPaBias;
unsigned char PAN3029PaBiasRaw;
unsigned char PAN3029PaBiasApplied;
unsigned char PAN3029RxLen;
unsigned char PAN3029RxError;
unsigned char PAN3029State;
unsigned char PAN3029CurrentChannel = 0xff;

/* 缓存当前页与射频状态，避免重复写页寄存器、方便调试观察 */
static unsigned char gPanPage = 0xff;
static unsigned char gPanState = PNA_RF_STATE_STB3;
static unsigned char gPanRegulatorMode = 0;

/* 低频段 AGC 表，按寄存器组顺序写入，顺序不可变（PAN3029 厂家参考配置） */
static const unsigned char gLowFreqAgcCfg[40] = {
    0x06, 0x00, 0xf8, 0x06, 0x06, 0x00, 0xf8, 0x06,
    0x06, 0x00, 0xf8, 0x06, 0x06, 0x00, 0xf8, 0x06,
    0x14, 0xc0, 0xf9, 0x14, 0x22, 0xd4, 0xf9, 0x22,
    0x30, 0xd8, 0xf9, 0x30, 0x3e, 0xde, 0xf9, 0x3e,
    0x0e, 0xff, 0x80, 0x4f, 0x12, 0x80, 0x38, 0x01
};

typedef struct {
    unsigned char page;
    unsigned char addr;
    unsigned char value;
} PNA_RegCfg_t;

/* 芯片默认工作参数表 */
static const PNA_RegCfg_t gPanDefaultConfig[] = {
    {0,0x03,0x1B},{0,0x04,0x76},{0,0x06,0x01},{0,0x0B,0x04},
    {0,0x13,0x04},{0,0x11,0x20},{0,0x12,0x10},{0,0x1F,0x07},
    {0,0x20,0x07},{0,0x24,0x03},{0,0x46,0x03},{0,0x25,0x00},
    {0,0x21,0x07},{0,0x22,0x07},{0,0x15,0x21},{0,0x31,0xD0},
    {0,0x36,0x66},{0,0x37,0x6B},{0,0x38,0xCC},{0,0x39,0x09},
    {0,0x3C,0xB4},{0,0x3E,0x42},{0,0x40,0x6A},{0,0x41,0x06},
    {0,0x42,0xAA},{0,0x48,0x77},{0,0x49,0x77},{0,0x4A,0x77},
    {0,0x4B,0x05},{0,0x4F,0x04},{0,0x50,0xD2},{0,0x5E,0x80},
    {1,0x03,0x1B},{1,0x04,0x76},{1,0x0B,0x08},{1,0x0F,0x0A},
    {1,0x19,0x00},{1,0x2F,0xD0},{1,0x43,0xDA},
    {2,0x03,0x1B},{2,0x04,0x76},{2,0x2C,0xC0},{2,0x2D,0x27},
    {2,0x2E,0x09},{2,0x2F,0x00},{2,0x30,0x10},
    {3,0x03,0x1B},{3,0x04,0x76},{3,0x0A,0x0E},{3,0x0B,0xCF},
    {3,0x0C,0x19},{3,0x0D,0x98},{3,0x12,0x16},{3,0x13,0x14},
    {3,0x16,0xF4},{3,0x17,0x01},{3,0x1F,0xD9}
};

static unsigned char pna_ctz(unsigned char value)
{
    unsigned char i;
    for(i = 0; i < 8; i++) {
        if((value >> i) & 0x01) {
            return i;
        }
    }
    return 0;
}

static void PNA_DelayUs(unsigned int us)
{
    R_BSP_SoftwareDelay(us, BSP_DELAY_UNITS_MICROSECONDS);
}

static void PNA_SetPage(unsigned char page)
{
    if(gPanPage != page) {
        RFSPIWriteReg(PNA_REG_PAGE, page);
        gPanPage = page;
    }
}

static void PNA_WritePageReg(unsigned char page, unsigned char addr, unsigned char value)
{
    PNA_SetPage(page);
    RFSPIWriteReg(addr, value);
}

static unsigned char PNA_ReadPageReg(unsigned char page, unsigned char addr)
{
    PNA_SetPage(page);
    return RFSPIReadReg(addr);
}

static void PNA_WritePageRegs(unsigned char page, unsigned char addr, const unsigned char *buf, unsigned char len)
{
    PNA_SetPage(page);
    RFSPIWriteBurstReg(addr, (unsigned char *)buf, len);
}

static void PNA_WritePageRegBits(unsigned char page, unsigned char addr, unsigned char value, unsigned char mask)
{
    unsigned char temp;
    unsigned char shift;
    shift = pna_ctz(mask);
    value = (unsigned char)((value << shift) & mask);
    PNA_SetPage(page);
    temp = RFSPIReadReg(addr);
    RFSPIWriteReg(addr, (unsigned char)((temp & (~mask)) | value));
}

static void PNA_SetPageRegBits(unsigned char page, unsigned char addr, unsigned char mask)
{
    unsigned char temp;
    PNA_SetPage(page);
    temp = RFSPIReadReg(addr);
    RFSPIWriteReg(addr, (unsigned char)(temp | mask));
}

static void PNA_ResetPageRegBits(unsigned char page, unsigned char addr, unsigned char mask)
{
    unsigned char temp;
    PNA_SetPage(page);
    temp = RFSPIReadReg(addr);
    RFSPIWriteReg(addr, (unsigned char)(temp & (~mask)));
}

static void PNA_ConfigGpio(unsigned char gpio, unsigned char output)
{
    if(output) {
        if(gpio < 8) {
            PNA_SetPageRegBits(PNA_PAGE_SYS, PNA_SYS_REG_GPIO_OUT_EN_L, (unsigned char)(1 << gpio));
        }
        else {
            PNA_SetPageRegBits(PNA_PAGE_SYS, PNA_SYS_REG_GPIO_OUT_EN_H, (unsigned char)(1 << (gpio - 8)));
        }
    }
    else {
        if(gpio < 8) {
            PNA_SetPageRegBits(PNA_PAGE_SYS, PNA_SYS_REG_GPIO_IN_EN_L, (unsigned char)(1 << gpio));
        }
        else {
            PNA_SetPageRegBits(PNA_PAGE_SYS, PNA_SYS_REG_GPIO_IN_EN_H, (unsigned char)(1 << (gpio - 8)));
        }
    }
}

static void PNA_WriteGpioLevel(unsigned char gpio, unsigned char level)
{
    if(gpio < 8) {
        PNA_WritePageRegBits(PNA_PAGE_SYS, PNA_SYS_REG_GPIO_LEVEL_L, level, (unsigned char)(1 << gpio));
    }
    else {
        PNA_WritePageRegBits(PNA_PAGE_SYS, PNA_SYS_REG_GPIO_LEVEL_H, level, (unsigned char)(1 << (gpio - 8)));
    }
}

static void PNA_TurnoffPA(void)
{
    PNA_ResetPageRegBits(PNA_PAGE_SYS, PNA_SYS_REG_PA_CTRL, PNA_REG_BIT_PA_ENABLE);
    PNA_WriteGpioLevel(PNA_GPIO_RX, 0);
    PNA_WriteGpioLevel(PNA_GPIO_TX, 0);
    if(gPanRegulatorMode) {
        PNA_WritePageReg(PNA_PAGE_LORA, PNA_LORA_REG_REGULATOR, 0x08);
    }
}

static void PNA_TurnonRxAnt(void)
{
    PNA_WriteGpioLevel(PNA_GPIO_TX, 0);
    PNA_WriteGpioLevel(PNA_GPIO_RX, 1);
}

static void PNA_TurnonPA(void)
{
    if(gPanRegulatorMode) {
        PNA_WritePageReg(PNA_PAGE_LORA, PNA_LORA_REG_REGULATOR, 0x00);
    }
    PNA_SetPageRegBits(PNA_PAGE_SYS, PNA_SYS_REG_PA_CTRL, PNA_REG_BIT_PA_ENABLE);
    PNA_WriteGpioLevel(PNA_GPIO_RX, 0);
    PNA_WriteGpioLevel(PNA_GPIO_TX, 1);
}

static void PNA_SetRfState(unsigned char state)
{
    RFSPIWriteReg(PNA_REG_RF_STATE, state);
    gPanState = state;
    PAN3029State = state;
}

static void PNA_ExitSleepState(void)
{
    if(gPanState != PNA_RF_STATE_SLEEP) {
        return;
    }

    PNA_SetPageRegBits(PNA_PAGE_LORA, PNA_LORA_REG_MODE, PNA_REG_BIT_SLEEP_DIGITAL);
    PNA_DelayUs(10);
    PNA_SetRfState(PNA_RF_STATE_STB1);
    PNA_DelayUs(10);
    PNA_WritePageReg(PNA_PAGE_LORA, PNA_LORA_REG_SLEEP_CTRL, PNA_REG_BIT_SLEEP_DIGITAL);
    PNA_DelayUs(100);
    RFSPIWriteReg(PNA_REG_ANALOG_CTRL, 0x36);
    PNA_DelayUs(100);
    PNA_SetRfState(PNA_RF_STATE_STB2);
    delayms(1);
    PNA_SetRfState(PNA_RF_STATE_STB3);
    PNA_DelayUs(100);
}

static void PNA_ConfigDefaultParams(void)
{
    unsigned char i;
    for(i = 0; i < (sizeof(gPanDefaultConfig) / sizeof(gPanDefaultConfig[0])); i++) {
        PNA_WritePageReg(gPanDefaultConfig[i].page, gPanDefaultConfig[i].addr, gPanDefaultConfig[i].value);
    }
}

static unsigned char PNA_ReadInfoByte(unsigned char addr, unsigned int pattern, unsigned char infoAddr)
{
    unsigned char value;
    unsigned char buffer[3];
    unsigned int timeout = 10000u;

    buffer[0] = (unsigned char)(pattern >> 8);
    buffer[1] = (unsigned char)pattern;
    buffer[2] = (unsigned char)(infoAddr << 1);

    PNA_WritePageRegs(PNA_PAGE_AGC, addr, buffer, sizeof(buffer));
    do {
        if(PNA_ReadPageReg(PNA_PAGE_SYS, PNA_SYS_REG_IRQ_FLAGS) & 0x80) {
            break;
        }
    } while(timeout--);

    value = PNA_ReadPageReg(PNA_PAGE_AGC, addr);
    return value;
}

static void PNA_SetTxPower(void)
{
    unsigned char paBias;
    unsigned char temp;

    PNA_WritePageReg(PNA_PAGE_LORA, PNA_LORA_REG_PA_RAMP, (unsigned char)(PAN3029_RF_TX_POWER_LDO & 0x01));
    PNA_WritePageReg(PNA_PAGE_SYS, PNA_SYS_REG_TX_POWER_RAMP, PAN3029_RF_TX_POWER_RAMP);
    PNA_WritePageReg(PNA_PAGE_SYS, PNA_SYS_REG_PA_TRIM, (unsigned char)(PAN3029_RF_TX_POWER_LDO >> 4));

    if(PAN3029_RF_TX_POWER_PABIAS != 0x70) {
        PNA_SetPageRegBits(PNA_PAGE_SYS, PNA_SYS_REG_PA_MODE, PNA_REG_BIT_PA_MODE);
    }
    else {
        PNA_ResetPageRegBits(PNA_PAGE_SYS, PNA_SYS_REG_PA_MODE, PNA_REG_BIT_PA_MODE);
    }

    paBias = PAN3029PaBiasRaw;
    if(paBias == 0) {
        paBias = PAN3029_RF_TX_POWER_PABIAS_FALLBACK;
    }

    temp = (unsigned char)(paBias - (PAN3029_RF_TX_POWER_PABIAS & 0x0F));
    PAN3029PaBiasApplied = (unsigned char)((PAN3029_RF_TX_POWER_PABIAS & 0xF0) | (temp & 0x0F));
    PNA_WritePageReg(PNA_PAGE_SYS, PNA_SYS_REG_PA_CFG, PAN3029PaBiasApplied);
}

static void PNA_Calibrate(void)
{
    unsigned char i;
    unsigned char temp[3];

    PNA_ResetPageRegBits(PNA_PAGE_AGC, PNA_INFO_REG_CTRL, PNA_INFO_UNLOCK_BIT);
    for(i = 0; i < sizeof(temp); i++) {
        temp[i] = PNA_ReadInfoByte(PNA_INFO_REG_DATA, 0x5AA5u, (unsigned char)(0x1E + i));
    }

    PAN3029CalMarker = PNA_ReadInfoByte(PNA_INFO_REG_DATA, 0x5AA5u, 0x1C);
    if(PAN3029CalMarker == 0x5A) {
        PNA_WritePageReg(PNA_PAGE_AGC, PNA_INFO_REG_DCDC, 0xFD);
        if(temp[2] != 0) {
            PNA_WritePageReg(PNA_PAGE_SYS, PNA_SYS_REG_PA_CFG, temp[2]);
        }
        PNA_WritePageReg(PNA_PAGE_LORA, 0x1C, (unsigned char)(0xC0 | (temp[0] & 0x1F)));
        PNA_WritePageReg(PNA_PAGE_LORA, 0x1D, temp[1]);
    }
    PAN3029CalPaBias = temp[2];
    PAN3029PaBiasRaw = temp[2];
    PNA_SetPageRegBits(PNA_PAGE_AGC, PNA_INFO_REG_CTRL, PNA_INFO_UNLOCK_BIT);
}

static void PNA_ConfigAgc(void)
{
    PNA_ResetPageRegBits(PNA_PAGE_AGC, PNA_AGC_REG_ENABLE, PNA_REG_BIT_AGC_DISABLE);
    PNA_WritePageRegs(PNA_PAGE_AGC, PNA_AGC_REG_TABLE_START, gLowFreqAgcCfg, sizeof(gLowFreqAgcCfg));
    PNA_WritePageReg(PNA_PAGE_AGC, PNA_AGC_REG_FINAL, 0xEF);
}

static void PNA_SetFreq(unsigned long freq)
{
    unsigned char freqReg[4];
    unsigned char fab[3];
    unsigned long temp;
    unsigned long integerPart;
    unsigned long fa;
    unsigned long fb;
    unsigned char vco[2];
    unsigned char loParam;

    if(freq <= 415000000UL) {
        vco[0] = 0x1A; vco[1] = 0x06; loParam = 0x90;
    }
    else if(freq <= 430000000UL) {
        vco[0] = 0x2A; vco[1] = 0x06; loParam = 0x90;
    }
    else if(freq <= 445000000UL) {
        vco[0] = 0x3A; vco[1] = 0x06; loParam = 0x90;
    }
    else if(freq <= 465000000UL) {
        vco[0] = 0x4A; vco[1] = 0x06; loParam = 0x90;
    }
    else if(freq <= 485000000UL) {
        vco[0] = 0x5A; vco[1] = 0x06; loParam = 0x90;
    }
    else if(freq <= 505000000UL) {
        vco[0] = 0x6A; vco[1] = 0x06; loParam = 0x90;
    }
    else {
        vco[0] = 0x7A; vco[1] = 0x06; loParam = 0x90;
    }

    temp = freq * 4UL;
    PNA_WritePageRegs(PNA_PAGE_SYS, PNA_SYS_REG_VCO_START, vco, 2);
    RFSPIWriteReg(PNA_REG_LO_PARAM, loParam);

    integerPart = temp / 32000000UL;
    fa = integerPart - 20UL;
    fb = (temp % 32000000UL) / 40000UL;

    freqReg[0] = (unsigned char)freq;
    freqReg[1] = (unsigned char)(freq >> 8);
    freqReg[2] = (unsigned char)(freq >> 16);
    freqReg[3] = (unsigned char)(freq >> 24);
    PNA_WritePageRegs(PNA_PAGE_LORA, PNA_LORA_REG_FREQ_START, freqReg, 4);

    fab[0] = (unsigned char)fa;
    fab[1] = (unsigned char)fb;
    fab[2] = (unsigned char)((fb >> 8) & 0x0F);
    PNA_WritePageRegs(PNA_PAGE_LORA, PNA_LORA_REG_FA_FB_START, fab, 3);
}

static unsigned char PNA_GetActiveChannel(void)
{
    return (unsigned char)LoRaChannel;
}

static void PNA_SelectChannel(void)
{
    unsigned char channel;

    channel = PNA_GetActiveChannel();
    if(PAN3029CurrentChannel == channel) {
        return;
    }

    PNA_SetFreq(PAN3029_RF_BASE_FREQ_HZ + ((unsigned long)channel * PAN3029_RF_CHANNEL_STEP_HZ));
    PAN3029CurrentChannel = channel;
}

static void PNA_ConfigUserParams(void)
{
    PAN3029CurrentChannel = 0xff;
    PNA_SelectChannel();
    PNA_WritePageRegBits(PNA_PAGE_LORA, PNA_LORA_REG_BW_CR, PAN3029_RF_BW_125K, PNA_REG_MASK_BW);
    PNA_SetPageRegBits(PNA_PAGE_AGC, PNA_AGC_REG_LORA_GAIN, 0x02);
    PNA_WritePageRegBits(PNA_PAGE_LORA, PNA_LORA_REG_SF_CRC, PAN3029_RF_SF9, PNA_REG_MASK_SF);
    PNA_WritePageRegBits(PNA_PAGE_LORA, PNA_LORA_REG_BW_CR, PAN3029_RF_CR_4_5, PNA_REG_MASK_CR);
    PNA_WritePageRegBits(PNA_PAGE_LORA, PNA_LORA_REG_SF_CRC, PAN3029_RF_CRC_OFF, PNA_REG_MASK_CRC);
    PNA_WritePageRegBits(PNA_PAGE_LORA, PNA_LORA_REG_MODEM_OPT, PAN3029_RF_LDR_MODE, PNA_REG_MASK_LDR);
    PNA_WritePageReg(PNA_PAGE_LORA, PNA_LORA_REG_PREAMBLE_L, PAN3029_RF_PREAMBLE_SHORT_L);
    PNA_WritePageReg(PNA_PAGE_LORA, PNA_LORA_REG_PREAMBLE_H, PAN3029_RF_PREAMBLE_SHORT_H);
    PNA_WritePageRegBits(PNA_PAGE_PKT, PNA_PKT_REG_IQ_CFG, PAN3029_RF_IQ_NORMAL, PNA_REG_MASK_IQ);
    PNA_WritePageReg(PNA_PAGE_LORA, PNA_LORA_REG_SYNC_WORD, PAN3029_RF_SYNC_WORD);
    PNA_WritePageReg(PNA_PAGE_LORA, PNA_LORA_REG_REGULATOR, 0x00);
    gPanRegulatorMode = 0;

    PNA_WritePageRegBits(PNA_PAGE_PKT, PNA_PKT_REG_MODE_CFG, PAN3029_RF_MODE1, PNA_REG_MASK_MODE0);
    PNA_WritePageRegBits(PNA_PAGE_PKT, PNA_PKT_REG_MODE_CFG, 1, PNA_REG_MASK_CRC_IRQ);
    PNA_WritePageRegBits(PNA_PAGE_LORA, PNA_LORA_REG_MODEM_OPT, PAN3029_RF_MODEM_OPT_OFF, PNA_REG_MASK_MODEM_OPT_2);
    PNA_WritePageRegBits(PNA_PAGE_LORA, PNA_LORA_REG_MODEM_OPT, PAN3029_RF_MODEM_OPT_OFF, PNA_REG_MASK_MODEM_OPT_4);
    PNA_WritePageRegBits(PNA_PAGE_SYS, PNA_SYS_REG_DIO_MODE, 0, PNA_REG_BIT_DIO_MODE);

    PNA_SetTxPower();
}

static void PNA_SetPreamble(unsigned char low, unsigned char high)
{
    PNA_WritePageReg(PNA_PAGE_LORA, PNA_LORA_REG_PREAMBLE_L, low);
    PNA_WritePageReg(PNA_PAGE_LORA, PNA_LORA_REG_PREAMBLE_H, high);
}

static unsigned int PNA_GetTxWaitMs(unsigned char len, bool useWakeupPreamble)
{
    unsigned long preambleSymbols;
    unsigned long waitMs;

    if(useWakeupPreamble) {
        preambleSymbols = (((unsigned long)PAN3029_RF_PREAMBLE_LONG_H) << 8) | PAN3029_RF_PREAMBLE_LONG_L;
    }
    else {
        preambleSymbols = (((unsigned long)PAN3029_RF_PREAMBLE_SHORT_H) << 8) | PAN3029_RF_PREAMBLE_SHORT_L;
    }

    waitMs = ((preambleSymbols * PAN3029_TX_SYMBOL_US) + 999UL) / 1000UL;
    waitMs += PAN3029_TX_WAIT_GUARD_MS + ((unsigned long)len * PAN3029_TX_WAIT_LEN_STEP_MS);

    if(waitMs < PAN3029_TX_WAIT_MIN_MS) {
        waitMs = PAN3029_TX_WAIT_MIN_MS;
    }
    if(waitMs > PAN3029_TX_WAIT_MAX_MS) {
        waitMs = PAN3029_TX_WAIT_MAX_MS;
    }

    return (unsigned int)waitMs;
}

static bool PNA_NeedWakeupPreamble(void)
{
    /* 沿用原 SX1278SendPacket 的判定：LDCMode 上层显式要求长前导，
     * 或 ActiveTime 为 0 表示对端可能已退出激活窗，需要长前导唤醒。 */
    return (LDCMode || (0 == ActiveTime)) ? true : false;
}

static unsigned char PNA_GetIRQFlag(void)
{
    return (unsigned char)(PNA_ReadPageReg(PNA_PAGE_SYS, PNA_SYS_REG_IRQ_FLAGS) & 0x7F);
}

static void PNA_ClrIRQFlag(unsigned char flags)
{
    PNA_WritePageReg(PNA_PAGE_SYS, PNA_SYS_REG_IRQ_FLAGS, flags);
}

static unsigned char PNA_CheckSpiAfterInit(void)
{
    /* 初始化结束后对同步字和前导寄存器做读写校验，验证 SPI 与页选切换正常 */
    unsigned char oldValue;
    unsigned char readValue;
    unsigned char testValue;

    oldValue = PNA_ReadPageReg(PNA_PAGE_LORA, PNA_LORA_REG_SYNC_WORD);
    testValue = (unsigned char)(oldValue ^ 0x5A);
    PNA_WritePageReg(PNA_PAGE_LORA, PNA_LORA_REG_SYNC_WORD, testValue);
    readValue = PNA_ReadPageReg(PNA_PAGE_LORA, PNA_LORA_REG_SYNC_WORD);
    PAN3029SpiCheckReg = readValue;
    PNA_WritePageReg(PNA_PAGE_LORA, PNA_LORA_REG_SYNC_WORD, oldValue);
    if(readValue != testValue) {
        return PNA_SPI_CHECK_SYNC_ERR;
    }
    readValue = PNA_ReadPageReg(PNA_PAGE_LORA, PNA_LORA_REG_SYNC_WORD);
    PAN3029SpiCheckReg = readValue;
    if(readValue != oldValue) {
        return PNA_SPI_CHECK_SYNC_ERR;
    }

    oldValue = PNA_ReadPageReg(PNA_PAGE_LORA, PNA_LORA_REG_PREAMBLE_L);
    testValue = (unsigned char)(oldValue ^ 0xA5);
    PNA_WritePageReg(PNA_PAGE_LORA, PNA_LORA_REG_PREAMBLE_L, testValue);
    readValue = PNA_ReadPageReg(PNA_PAGE_LORA, PNA_LORA_REG_PREAMBLE_L);
    PAN3029SpiCheckReg = readValue;
    PNA_WritePageReg(PNA_PAGE_LORA, PNA_LORA_REG_PREAMBLE_L, oldValue);
    if(readValue != testValue) {
        return PNA_SPI_CHECK_PREAMBLE_ERR;
    }
    readValue = PNA_ReadPageReg(PNA_PAGE_LORA, PNA_LORA_REG_PREAMBLE_L);
    PAN3029SpiCheckReg = readValue;
    if(readValue != oldValue) {
        return PNA_SPI_CHECK_PREAMBLE_ERR;
    }

    return PNA_SPI_CHECK_OK;
}

void PAN3029_Reset(void)
{
    RFSPI_RFRESET_SETOUT_LOW;
    delayms(10);
    RFSPI_RFRESET_SETOUT_HIGH;
    delayms(20);
}

unsigned char test ;
void PAN3029_Init(void)
{
    unsigned char irq;

    PAN3029_Reset();
    RFSPISetup();
    RFSPI_CLK_SETOUT_LOW;
    RFSPI_NSS_SETOUT_HIGH;
///=========================  spi test
   // RFSPIWriteReg(0x04, 0x55);
   // test = RFSPIReadReg(0x04);
///==========================
    RFSPIWriteReg(PNA_REG_ANALOG_CTRL, 0x06);
    PNA_DelayUs(100);
    RFSPIWriteReg(PNA_REG_PAGE, PNA_PAGE_LORA);
    RFSPIWriteReg(PNA_REG_SPI_MODE, 0x03); /* 4-line SPI */
    gPanPage = 0xff;

    PNA_SetPage(PNA_PAGE_SYS);
    PNA_SetRfState(PNA_RF_STATE_DEEPSLEEP);
    PNA_DelayUs(10);
    PNA_SetRfState(PNA_RF_STATE_SLEEP);
    PNA_DelayUs(10);
    PNA_WritePageReg(PNA_PAGE_LORA, PNA_LORA_REG_MODE, PNA_REG_BIT_SLEEP_DIGITAL);
    PNA_DelayUs(10);
    PNA_SetRfState(PNA_RF_STATE_STB1);
    PNA_DelayUs(10);
    PNA_WritePageReg(PNA_PAGE_LORA, PNA_LORA_REG_SLEEP_CTRL, PNA_REG_BIT_SLEEP_DIGITAL);
    PNA_DelayUs(100);
    RFSPIWriteReg(PNA_REG_ANALOG_CTRL, 0x36);
    delayms(1);
    PNA_SetRfState(PNA_RF_STATE_STB2);
    delayms(1);
    PNA_SetRfState(PNA_RF_STATE_STB3);
    PNA_DelayUs(100);

    PNA_ConfigDefaultParams();
    PNA_Calibrate();
    PNA_ConfigAgc();
    PNA_ConfigGpio(PNA_GPIO_RX, 1);
    PNA_ConfigGpio(PNA_GPIO_TX, 1);
    PNA_WriteGpioLevel(PNA_GPIO_RX, 0);
    PNA_WriteGpioLevel(PNA_GPIO_TX, 0);
    PNA_ConfigUserParams();

    irq = PNA_GetIRQFlag();
    if(irq) {
        PNA_ClrIRQFlag(irq);
    }
    PAN3029SpiCheckError = PNA_CheckSpiAfterInit();
    PAN3029SpiOk = (PAN3029SpiCheckError == PNA_SPI_CHECK_OK);

    /* 强电供电、无功耗约束：初始化结束立即进入 RX 持续监听，
     * 避免停在 STB3 漏收对端首包。 */
    if(PAN3029SpiOk) {
        PAN3029_RX();
    }
}

void PAN3029_SendPacket(unsigned char *TxBuffer, unsigned char Len)
{
    unsigned long i;
    unsigned long waitMs;
    unsigned char irq;
    bool useWakeupPreamble;

    if(!PAN3029SpiOk) {
        return;
    }
    PAN3029TxDone = 0;
    PAN3029TxLen = Len;
    PAN3029TxElapsedMs = 0;
    useWakeupPreamble = PNA_NeedWakeupPreamble();
    waitMs = PNA_GetTxWaitMs(Len, useWakeupPreamble);
    PAN3029TxUseLongPreamble = useWakeupPreamble ? 1 : 0;
    PAN3029TxWaitMs = (unsigned int)waitMs;

    RFTX_ON;
    PNA_ExitSleepState();
    PNA_SetRfState(PNA_RF_STATE_STB3);
    PNA_SelectChannel();
    if(useWakeupPreamble) {
        PNA_SetPreamble(PAN3029_RF_PREAMBLE_LONG_L, PAN3029_RF_PREAMBLE_LONG_H);
    }
    else {
        PNA_SetPreamble(PAN3029_RF_PREAMBLE_SHORT_L, PAN3029_RF_PREAMBLE_SHORT_H);
    }
    PNA_ClrIRQFlag(0x7F);
    PNA_WritePageRegBits(PNA_PAGE_LORA, PNA_LORA_REG_MODE, PNA_RF_TX_SINGLE, PNA_REG_BIT_LORA_TX_SINGLE);
    PNA_WritePageReg(PNA_PAGE_PKT, PNA_PKT_REG_PAYLOAD_LEN, Len);
    PNA_TurnonPA();
    PNA_SetRfState(PNA_RF_STATE_TX);
    RFSPIWriteBurstReg(PNA_REG_FIFO, TxBuffer, Len);

    for(i = 0; i < waitMs; i++) {
        irq = PNA_GetIRQFlag();
        PAN3029LastIrq = irq;
        if(irq & PNA_RF_IRQ_TX_DONE) {
            PNA_ClrIRQFlag(PNA_RF_IRQ_TX_DONE);
            PAN3029TxDone = 1;
            break;
        }
        delayms(1);
        WDT_FEED;   /* 长前导发送可达 4~5s，长循环内必须喂狗 */
    }
    PAN3029TxElapsedMs = (unsigned int)i;
    if(!PAN3029TxDone) {
        PAN3029TxTimeoutCount++;
    }
    PNA_TurnoffPA();
    PNA_SetRfState(PNA_RF_STATE_STB3);
    PAN3029_RX();
}

void PAN3029_RX(void)
{
    if(!PAN3029SpiOk) {
        return;
    }

    RFRX_ON;
    PNA_ExitSleepState();
    PNA_SetRfState(PNA_RF_STATE_STB3);
    PNA_SelectChannel();
    PNA_SetPreamble(PAN3029_RF_PREAMBLE_RX_L, PAN3029_RF_PREAMBLE_RX_H);
    PNA_TurnonRxAnt();
    PNA_ResetPageRegBits(PNA_PAGE_SYS, PNA_SYS_REG_PA_CTRL, PNA_REG_BIT_PA_ENABLE);
    PNA_WritePageRegBits(PNA_PAGE_LORA, PNA_LORA_REG_MODE, PNA_RF_RX_CONTINUOUS, PNA_REG_MASK_LORA_RX_MODE);
    PNA_SetRfState(PNA_RF_STATE_RX);
}

unsigned char PAN3029_ReceivePacket(unsigned char* RxBuffer)
{
    /* 返回值约定：1=成功 2=RX 超时 0=无包/CRC 错/长度异常，
     * 与原 SX1278ReceivePacket 的 0/1 语义兼容，上层 if(re) 判断不需改动。 */
    unsigned char irq;
    unsigned char len;
    unsigned char peek[PNA_RX_PEEK_LEN];

    if(!PAN3029SpiOk) {
        return 0;
    }

    irq = PNA_GetIRQFlag();
    PAN3029LastIrq = irq;
    if(irq & PNA_RF_IRQ_CRC_ERR) {
        PNA_ClrIRQFlag(PNA_RF_IRQ_CRC_ERR);
        PAN3029RxError = PNA_RF_IRQ_CRC_ERR;
        return 0;
    }
    if(irq & PNA_RF_IRQ_RX_TIMEOUT) {
        PNA_ClrIRQFlag(PNA_RF_IRQ_RX_TIMEOUT);
        PAN3029RxError = PNA_RF_IRQ_RX_TIMEOUT;
        return 2;
    }
    if(!(irq & PNA_RF_IRQ_RX_DONE)) {
        return 0;
    }

    len = PNA_ReadPageReg(PNA_PAGE_PKT, PNA_PKT_REG_RX_LEN);
    PAN3029RxLen = len;
    if(len >= BufferLenth - 1) {
        RFSPIReadBurstReg(PNA_REG_FIFO, peek, PNA_RX_PEEK_LEN);
        PNA_ClrIRQFlag(PNA_RF_IRQ_RX_DONE);
        PAN3029RxError = 0x80;
        return 0;
    }

    RFSPIReadBurstReg(PNA_REG_FIFO, RxBuffer, len);
    PNA_ClrIRQFlag(PNA_RF_IRQ_RX_DONE);
    PAN3029RxError = 0;
    return 1;
}

void PAN3029_CADInit(void)
{
    if(!PAN3029SpiOk) {
        return;
    }

    PNA_ExitSleepState();
    PNA_SetRfState(PNA_RF_STATE_STB3);
    PNA_ClrIRQFlag(0x7F);
    PNA_ConfigGpio(PNA_GPIO_CAD_IRQ, 1);
    PNA_ResetPageRegBits(PNA_PAGE_SYS, PNA_SYS_REG_CAD_CTRL, PNA_REG_BIT_CAD_DISABLE);
    PNA_WritePageReg(PNA_PAGE_PKT, PNA_PKT_REG_CAD_THRESHOLD, PAN3029_WOR_CAD_THRESHOLD);
    PNA_WritePageRegBits(PNA_PAGE_PKT, PNA_PKT_REG_MODE_CFG, PAN3029_WOR_CAD_SYMBOLS - 1, PNA_REG_MASK_LORA_RX_MODE);
    PNA_WritePageReg(PNA_PAGE_PKT, PNA_PKT_REG_CAD_DONE_CFG, PAN3029_WOR_CAD_DONE_ACTIVE);
    PAN3029_RX();
}

void PAN3029_Sleep(void)
{
    if(!PAN3029SpiOk) {
        return;
    }

    PNA_TurnoffPA();
    PNA_SetRfState(PNA_RF_STATE_STB3);
    PNA_DelayUs(150);
    PNA_SetRfState(PNA_RF_STATE_STB2);
    PNA_DelayUs(10);
    PNA_SetRfState(PNA_RF_STATE_STB1);
    PNA_DelayUs(10);
    RFSPIWriteReg(PNA_REG_ANALOG_CTRL, 0x16);
    PNA_DelayUs(10);
    PNA_SetRfState(PNA_RF_STATE_SLEEP);
    PNA_DelayUs(10);
    PNA_ResetPageRegBits(PNA_PAGE_LORA, PNA_LORA_REG_MODE, PNA_REG_BIT_SLEEP_DIGITAL);
    PNA_DelayUs(10);
    PNA_WritePageReg(PNA_PAGE_LORA, PNA_LORA_REG_SLEEP_CTRL, 0x00);
    RFSPI_NSS_SETOUT_HIGH;
    RFSPI_CLK_SETOUT_LOW;
    RFSPI_MOSI_SETOUT_LOW;
}
