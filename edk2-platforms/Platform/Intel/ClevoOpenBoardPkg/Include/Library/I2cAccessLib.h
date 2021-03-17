/** @file
  Support for IO expander TCA6424.

Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _I2C_ACCESS_LIB_H_
#define _I2C_ACCESS_LIB_H_

#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/TimerLib.h>
#include <Library/IoLib.h>
#include <Library/UefiLib.h>
#include <PchAccess.h>
#include <Library/PchSerialIoLib.h>

#define WAIT_1_SECOND            1600000000 //1.6 * 10^9

EFI_STATUS
I2cWriteRead (
  IN UINTN  MmioBase,
  IN UINT8  SlaveAddress,
  IN UINT8  WriteLength,
  IN UINT8  *WriteBuffer,
  IN UINT8  ReadLength,
  IN UINT8  *ReadBuffer,
  IN UINT64  TimeBudget
  );

#endif