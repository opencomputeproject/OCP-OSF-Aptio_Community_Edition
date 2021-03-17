/** @file
  Header file for the PeiCpuPolicyLib library.

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#ifndef _PEI_CPU_POLICY_LIBRARY_H_
#define _PEI_CPU_POLICY_LIBRARY_H_

#include <PiPei.h>
#include <CpuAccess.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Ppi/MasterBootMode.h>
#include <Ppi/SiPolicy.h>
#include <Library/CpuPolicyLib.h>
#include <IndustryStandard/SmBios.h>
#include <Library/SiConfigBlockLib.h>
#include <Library/CpuPlatformLib.h>
#include <Register/Cpuid.h>
#include <Library/PcdLib.h>

#define MAX_MICROCODE_PATCH_SIZE 0x20000

#endif // _PEI_CPU_POLICY_LIBRARY_H_
