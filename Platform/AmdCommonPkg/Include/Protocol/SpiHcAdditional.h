/******************************************************************************
 * Copyright (C) 2018-2023 Advanced Micro Devices, Inc. All rights reserved.
 *
 *
 ***************************************************************************/

/**
  These are definitions that should be part of the UEFI PI specificaiton, but
  are not yet.
**/

#ifndef AMD_SPI_HC_ADDITIONAL_H_
#define AMD_SPI_HC_ADDITIONAL_H_

#include <Uefi/UefiBaseType.h>

#define HC_TRANSFER_SIZE_INCLUDES_ADDRESS       BIT0
#define HC_TRANSFER_SIZE_INCLUDES_OPCODE        BIT1
#define HC_SUPPORTS_4_B1T_DATA_BUS_WIDTH        BIT2
#define HC_SUPPORTS_2_BIT_DATA_BUS_WIDTH        BIT3
#define HC_RX_FRAME_IN_MOST_SIGNIFICANT_BITS    BIT4
#define HC_TX_FRAME_IN_MOST_SIGNIFICANT_BITS    BIT5
#define HC_SUPPORTS_WRITE_THEN_READ_OPERATIONS  BIT6
#define HC_SUPPORTS_READ_ONLY_OPERATIONS        BIT7
#define HC_SUPPORTS_WRITE_ONLY_OPERATIONS       BIT8

#endif // AMD_SPI_HC_ADDITIONAL_H_
