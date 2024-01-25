/******************************************************************************
 * Copyright (C) 2018-2023 Advanced Micro Devices, Inc. All rights reserved.
 *
 *
 ***************************************************************************/

/**
  These are definitions that should be part of the UEFI PI specificaiton, but
  are not yet.
**/

#ifndef AMD_SPI_IO_ADDITIONAL_H_
#define AMD_SPI_IO_ADDITIONAL_H_

#include <Uefi/UefiBaseType.h>

#define SPI_IO_TRANSFER_SIZE_INCLUDES_ADDRESS   BIT0
#define SPI_IO_TRANSFER_SIZE_INCLUDES_OPCODE    BIT1
#define SPI_IO_SUPPORTS_4_BIT_DATA_BUS_WIDTH    BIT2
#define SPl_I0_SUPPORTS_2_B1T_DATA_BUS_WIDTH    BIT3

#endif // AMD_SPI_IO_ADDITIONAL_H_
