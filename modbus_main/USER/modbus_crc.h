#ifndef MODBUS_CRC_H
#define MODBUS_CRC_H

#include "stm32f4xx.h"

u16  Modbus_CRC16( u8 *puchMsg, u16 usDataLen );

#endif
