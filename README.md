# 灵龙壁挂炉 IoT 控制器

**LingLong Gas Boiler IoT Controller — ML307R (CAT1) + PNA3029 (LoRa)**

---

## 项目概述

本项目是灵龙壁挂炉配套的物联网通信控制器固件，运行于 Renesas RA2L1 MCU。  
通过 ML307R CAT1 模块实现云端数据上报，通过 PNA3029 LoRa 模块实现与无线温控器的本地无线通信，同时支持循环水泵（LLS Pump）的定时/巡回自动控制。

---

## 硬件平台

| 组件 | 型号 / 规格 |
|------|------------|
| MCU | Renesas RA2L1 (R7FA2L1AB2DFL)，48 PIN |
| CAT1 模块 | ML307R，AT 指令驱动（UART9，9600 bps） |
| LoRa 模块 | PNA3029，477 MHz / SF9 / BW125k / CR4/5，SPI 接口 |
| EEPROM | AT24 系列，存储历史数据与系统参数 |
| 串口通信 | UART0（9600 bps），与壁挂炉主板通信 |
| IDE | IAR EWARM + Renesas FSP |

---

## 主要功能

### 云端通信（CAT1）
- 定时 / 事件触发数据上报（运行状态、故障码、温度、流量、累计用量等）
- 心跳保活，自动断线重连
- 支持远程参数下发（洗浴温度、采暖温度、循环泵配置等）
- 固件升级（OTA 分帧传输）

### 无线温控器通信（LoRa / PNA3029）
- LDC（Low Duty Cycle）唤醒机制：长前导码（~4.3 s）唤醒休眠温控器
- ActiveTime 自动切换：15 s 活跃窗口内使用短前导码，节省发送时间
- 支持配对（C3 01）、握手（C3 02）、心跳上报（C0 19）、参数设置（C0 20）等协议帧
- 温控器信息同步上云（F0 1C 帧）

### 循环水泵控制（LLS Pump）
- 两种运行模式：**巡回模式**（按间隔自动启动）/ **预约模式**（按时间表启动）
- 节能模式判断：非冷水状态 & 无故障时才允许水泵运行
- 支持 SOS 一键启动（RunOnceFlag）
- 重发保障：5 s 内自动重发指令，确保接收器收到

### 安全 / 可靠性
- WDT 看门狗，每次读取壁挂炉数据时喂狗
- 接收帧长度上界检查（≤ 40 字节），防止缓冲区溢出
- 重发缓冲区（ReSendBuf）与 NB 发送缓冲区（TxBuffer）完全隔离
- EC 时间槽数量截断（ec_num ≤ 3），防止 ec_time[12] 越界

---

## 目录结构

```
├── user_src/               用户业务逻辑
│   ├── user_main.c         主循环、初始化、调度
│   ├── frame.c             LoRa 帧解析与处理、无线响应
│   ├── NetDataPro.c        云端协议帧解析与上报
│   ├── Nb_Pro.c / _603.c   CAT1 AT 指令驱动
│   ├── PNA3029.c           LoRa 模块驱动（SPI）
│   ├── user_uart.c         串口收发
│   ├── usr_timer.c         定时器中断 / 软件计时
│   ├── save.c              EEPROM 历史数据存取
│   └── intp.c              外部中断处理
├── usr_lib/                头文件 / 全局变量声明
│   ├── main.h              全局变量定义（仅 user_main.c 包含）
│   ├── ExternPara.h        所有模块共享的 extern 声明
│   ├── rf.h                RF / 帧处理公共头
│   ├── sys.h               系统级宏与类型定义
│   └── ...
├── ra_gen/                 Renesas FSP 自动生成代码
├── boot/                   Bootloader 子工程
├── script/                 IAR 链接脚本
└── LingLong_ML307R_WDT.ewp  IAR 主工程文件
```

---

## 开发环境

- **IDE**：IAR Embedded Workbench for ARM（EWARM）8.x 及以上
- **FSP**：Renesas Flexible Software Package（通过 e² studio 配置，ra_gen/ 目录由工具生成）
- **调试器**：J-Link / E2 Lite（支持 SEGGER RTT 日志输出）
- **编码**：源文件为 GBK 编码（中文注释），git 仓库存储原始字节

### 编译步骤

1. 用 IAR EWARM 打开 `LingLong_ML307R_WDT.eww`
2. 选择配置（Debug / Release）
3. 直接 Build 即可，无需额外依赖

> **注意**：项目目录路径不可包含 `&` 字符，否则 IAR Post-build 命令解析会报错。

---

## 版本说明

| 标签 | 说明 |
|------|------|
| `_3029` | 使用 PNA3029 LoRa 模块（替代早期 SX1278） |
| `_WDT` | 启用硬件看门狗（R_WDT）|
| `_addPump` | 新增循环水泵（LLS Pump）控制逻辑 |
| `808A` | 壁挂炉主板型号适配（808A 协议，供暖水温度字段） |

---

## 许可

本项目为商业私有项目，代码仅供内部参考使用。
