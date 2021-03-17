/** @file
  Header file for Cpu Platform Lib implementation.

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _CPU_PLATFORM_LIBRARY_IMPLEMENTATION_H_
#define _CPU_PLATFORM_LIBRARY_IMPLEMENTATION_H_

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/CpuLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PciLib.h>
#include <Library/TimerLib.h>
#include <Library/SynchronizationLib.h>

#include <Register/Cpuid.h>
#include <Register/Msr.h>
#include <CpuAccess.h>
#include <Library/CpuPlatformLib.h>

#endif
