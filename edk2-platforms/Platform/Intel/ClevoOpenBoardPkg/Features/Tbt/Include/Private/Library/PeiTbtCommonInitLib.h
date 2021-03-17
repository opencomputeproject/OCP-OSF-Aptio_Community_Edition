/**@file
  PEI TBT Common Init Dispatch library Header file

Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#ifndef __PEI_TBT_COMMON_INIT_LIB_H__
#define __PEI_TBT_COMMON_INIT_LIB_H__

#include <Library/PeiServicesLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/GpioLib.h>
#include <Library/TimerLib.h>
#include <Library/IoLib.h>
#include <Library/PciSegmentLib.h>
#include <Library/PcdLib.h>
#include <Library/TbtCommonLib.h>
#include <IndustryStandard/Pci22.h>
#include <Library/PchPmcLib.h>

VOID
TbtSetSxMode(
IN    BOOLEAN                 Type,
IN    UINT8                   Bus,
IN    UINT8                   Device,
IN    UINT8                   Function,
IN    UINT8                   TbtBootOn
);

VOID
TbtClearVgaRegisters(
IN    UINTN                   Segment,
IN    UINTN                   Bus,
IN    UINTN                   Device,
IN    UINTN                   Function
);

#endif