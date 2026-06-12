/*
 * SX1278.c
 *
 *  Created on: 2021年6月3日
 *      Author: WJQ
 *
 * 已停用：硬件改为 PAN3029，所有调用已迁移至 PAN3029.c。
 * 本文件保留并清空函数体，避免触动 IAR .ewp 工程文件。
 * 如果上层不再 include SX1278.h，则下面这些符号也不会被引用，
 * 链接器会自动丢弃。
 */

#include "../usr_lib/sys.h"

void InitRF(void)                                    { }
void SX1278Settings(void)                            { }
void SX1278SendPacket(unsigned char *TxBuf, unsigned char Len)
{
    (void)TxBuf;
    (void)Len;
}
void SX1278_RX(void)                                 { }
unsigned char SX1278ReceivePacket(unsigned char* RxBuffer)
{
    (void)RxBuffer;
    return 0;
}
void SX1278_CADInit(void)                            { }
void SX1278_Sleep(void)                              { }
void SX1278_RST(void)                                { }
