/******************************************************************************
 * Copyright (C) 2017-2023 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

/* This file includes code originally published under the following license. */

/** @file
  Platform Hook Library instance.

  Copyright (c) 2010, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/PlatformHookLib.h>
#include "WA_ClearEspiIoForLegacyIo.h"

#define FCH_SMNSMBUSPCIx000000FC_UARTCONTROLREG        0xFED800FCul
#define   FCH_SMNSMBUSPCIx000000FC_uart_1p843mclk_en0  28
#define   FCH_SMNSMBUSPCIx000000FC_clock18432          0x1
#define   FCH_SMNSMBUSPCIx000000FC_clock48MHz          0x0
#define   FCH_UART_FREQUENCY_1_8432MHz                 1843200ul
#define   FCH_UART_FREQUENCY_48MHz                     48000000ul
#define FCH_IOMUXx88_UART0_RXD_AGPIO136           0xFED80D88ul
#define FCH_IOMUXx8A_UART0_TXD_AGPIO138           0xFED80D8Aul

#define FCH_IOMUXx8D_UART1_RXD_AGPIO141           0xFED80D8Dul
#define FCH_IOMUXx8E_UART1_TXD_AGPIO142           0xFED80D8Eul

#define FCH_AL2AHBx20_LEGACY_UART_IO_ENABLE       0xFEDC0020ul

#define UART0_SERIAL_PORT                         0xFEDC9000ul
#define UART1_SERIAL_PORT                         0xFEDCA000ul
#define UART2_SERIAL_PORT                         0xFEDCE000ul
#define UART3_SERIAL_PORT                         0xFEDCF000ul

#define FCH_AOACx40_D3_CONTROL                    0xFED81E40ul
#define FCH_AOACx56_UART0                         FCH_AOACx40_D3_CONTROL + 0x16          // UART0
#define FCH_AOACx58_UART1                         FCH_AOACx40_D3_CONTROL + 0x18          // UART1
#define FCH_AOACx62_AMBA                          FCH_AOACx40_D3_CONTROL + 0x22          // AMBA
#define AOAC_PWR_ON_DEV                           BIT3           // PwrOnDev
#define AOAC_TARGET_DEVICE_STATE                  (BIT0 + BIT1)  // TargetedDeviceState
#define DEVICE_STATE_D3_COLD                      0x03

#define R_UART_RXBUF         0    // LCR_DLAB = 0
#define R_UART_TXBUF         0    // LCR_DLAB = 0
#define R_UART_BAUD_LOW      0    // LCR_DLAB = 1
#define R_UART_BAUD_HIGH     1    // LCR_DLAB = 1
#define R_UART_IER           1    // LCR_DLAB = 0
#define R_UART_FCR           2
#define   B_UART_FCR_FIFOE   BIT0
#define   B_UART_FCR_FIFO64  BIT5
#define R_UART_LCR           3
#define   B_UART_LCR_DLAB    BIT7
#define R_UART_MCR           4
#define   B_UART_MCR_DTRC    BIT0
#define   B_UART_MCR_RTS     BIT1
#define R_UART_LSR           5
#define   B_UART_LSR_RXRDY   BIT0
#define   B_UART_LSR_TXRDY   BIT5
#define   B_UART_LSR_TEMT    BIT6
#define R_UART_MSR           6
#define   B_UART_MSR_CTS     BIT4
#define   B_UART_MSR_DSR     BIT5
#define   B_UART_MSR_RI      BIT6
#define   B_UART_MSR_DCD     BIT7

typedef struct {
  UINT32  BaseAddr;
  UINT32  Range;
} IO_PORT_RANGE;

IO_PORT_RANGE IoPortRangeList[] = {
    {0x2E8, 0},
    {0x2F8, 1},
    {0x3E8, 2},
    {0x3F8, 3}
};

#define MAX_IO_PORTS  (sizeof(IoPortRangeList) / sizeof(IO_PORT_RANGE))


/**
  Performs platform specific initialization required for the CPU to access
  the hardware associated with a SerialPortLib instance.  This function does
  not intialize the serial port hardware itself.  Instead, it initializes 
  hardware devices that are required for the CPU to access the serial port 
  hardware.  This function may be called more than once.

  @retval RETURN_SUCCESS       The platform specific initialization succeeded.
  @retval RETURN_DEVICE_ERROR  The platform specific initialization could not be completed.
 
**/
RETURN_STATUS
EFIAPI
PlatformHookSerialPortInitialize (
  VOID
  )
{
  UINTN                     timeout;
  UINT32                    AL2AHB_Legacy_UART_IO_Enable;
  UINT32                    Which_UART_Range;
  UINT32                    UartIndex;
  UINT32                    IoRange;
  UINT32                    IoIndex;
  UINT32                    UartClockSelection;
  UINT32                    Divisor;
  UINT32                    UartBaseReg;

  // REVISIT
  // eSPI workaround to claim legacy IO ranges
  ClearEspiIOForlegacyIO ();

  //
  // Initialize for FCH UART
  //
  UartIndex = FixedPcdGet8 (PcdFchUartPort);

  // AOAC 'PwrOnDev' bit of AMBA
  timeout = 0x1000;
  while ((MmioRead8 (FCH_AOACx62_AMBA) & AOAC_TARGET_DEVICE_STATE) != DEVICE_STATE_D3_COLD) {
    MmioWrite8 (FCH_AOACx62_AMBA, (MmioRead8 (FCH_AOACx62_AMBA) | AOAC_PWR_ON_DEV));
    if (--timeout == 0) {
      break;
    }
  }
  // AOAC 'PwrOnDev' bit of UART
  if (UartIndex == 0) {
    timeout = 0x1000;
    while ((MmioRead8 (FCH_AOACx56_UART0) & AOAC_TARGET_DEVICE_STATE) != DEVICE_STATE_D3_COLD) {
      MmioWrite8 (FCH_AOACx56_UART0, (MmioRead8 (FCH_AOACx56_UART0) | AOAC_PWR_ON_DEV));
      if (--timeout == 0) {
        break;
      }
    }
    // Switch to UART0_RTS_L & UART0_TXD
    MmioWrite8 (FCH_IOMUXx88_UART0_RXD_AGPIO136, 0);
    MmioWrite8 (FCH_IOMUXx8A_UART0_TXD_AGPIO138, 0);
  } else if (UartIndex == 1) {
    timeout = 0x1000;
    while ((MmioRead8 (FCH_AOACx58_UART1) & AOAC_TARGET_DEVICE_STATE) != DEVICE_STATE_D3_COLD) {
      MmioWrite8 (FCH_AOACx58_UART1, (MmioRead8 (FCH_AOACx58_UART1) | AOAC_PWR_ON_DEV));
      if (--timeout == 0) {
        break;
      }
    }
    // Switch to UART1_RTS_L & UART1_TXD
    MmioWrite8 (FCH_IOMUXx8D_UART1_RXD_AGPIO141, 0);
    MmioWrite8 (FCH_IOMUXx8E_UART1_TXD_AGPIO142, 0);
  } else {
    return RETURN_DEVICE_ERROR;
  }

  if (PcdGet32 (PcdSerialClockRate) == FCH_UART_FREQUENCY_48MHz) {
    UartClockSelection = FCH_SMNSMBUSPCIx000000FC_clock48MHz;
  } else if (PcdGet32 (PcdSerialClockRate) == FCH_UART_FREQUENCY_1_8432MHz) {
    UartClockSelection = FCH_SMNSMBUSPCIx000000FC_clock18432;
  } else {
    return RETURN_DEVICE_ERROR;
  }

  MmioBitFieldWrite32 (FCH_SMNSMBUSPCIx000000FC_UARTCONTROLREG,
      FCH_SMNSMBUSPCIx000000FC_uart_1p843mclk_en0 + UartIndex,
      FCH_SMNSMBUSPCIx000000FC_uart_1p843mclk_en0 + UartIndex,
      UartClockSelection);

  // Enable legacy mode (IO register access) if selected. Use MMIO register access otherwise.
  if (PcdGetBool (PcdSerialUseMmio) == 0) {
    IoRange = 0;
    for (IoIndex = 0; IoIndex < MAX_IO_PORTS && IoRange == 0; ++IoIndex) {
      if (IoPortRangeList[IoIndex].BaseAddr == (UINT32)FixedPcdGet64 (PcdSerialRegisterBase)) {
        IoRange = IoPortRangeList[IoIndex].Range;
      }
    }

    Which_UART_Range = UartIndex << (8 + (IoRange << 1));
    AL2AHB_Legacy_UART_IO_Enable = (1 << IoRange) | Which_UART_Range;
    MmioWrite32 (FCH_AL2AHBx20_LEGACY_UART_IO_ENABLE, AL2AHB_Legacy_UART_IO_Enable);
  }

  switch (UartIndex) {
    case 0:
      UartBaseReg = UART0_SERIAL_PORT;
      break;
    case 1:
      UartBaseReg = UART1_SERIAL_PORT;
      break;
    case 2:
      UartBaseReg = UART2_SERIAL_PORT;
      break;
    case 3:
      UartBaseReg = UART3_SERIAL_PORT;
      break;
    default:
      return RETURN_SUCCESS;
  }

  Divisor = PcdGet32 (PcdSerialClockRate) / (16 * (PcdGet32 (PcdSerialBaudRate)));
  MmioWrite8 (UartBaseReg + R_UART_LCR * 4, B_UART_LCR_DLAB);
  MmioWrite8 (UartBaseReg + R_UART_BAUD_HIGH * 4, (UINT8) (Divisor >> 8));
  MmioWrite8 (UartBaseReg + R_UART_BAUD_LOW * 4, (UINT8)(Divisor & 0xff));
  MmioWrite8 (UartBaseReg + R_UART_LCR * 4, (UINT8)(R_UART_LCR & 0x3F) );
  MmioWrite8 (UartBaseReg + R_UART_FCR * 4, (UINT8)(0x7 & 0x27));
  MmioWrite8 (UartBaseReg + R_UART_MCR * 4, 0x00);

  return RETURN_SUCCESS;
}




