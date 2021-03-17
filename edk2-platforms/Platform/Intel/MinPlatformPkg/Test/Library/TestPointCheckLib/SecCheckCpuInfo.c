/** @file

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Uefi.h>
#include <PiPei.h>
#include <Library/TestPointCheckLib.h>
#include <Library/DebugLib.h>
#include <Register/Cpuid.h>
#include <Register/Msr.h>

VOID
TestPointDumpCpuInfo (
  VOID
  )
{
  UINT32  RegEax;
  
  DEBUG ((DEBUG_INFO, "==== TestPointDumpCpuInfo - Enter\n"));

  DEBUG((DEBUG_INFO, "CPU info\n"));
  AsmCpuid (CPUID_VERSION_INFO, &RegEax, NULL, NULL, NULL);
  DEBUG((DEBUG_INFO, "  CPUID = 0x%08x\n", RegEax));

  DEBUG((DEBUG_INFO, "  Microcode ID (0x%08x)  = 0x%016lx\n", MSR_IA32_BIOS_SIGN_ID, AsmReadMsr64 (MSR_IA32_BIOS_SIGN_ID)));
  DEBUG((DEBUG_INFO, "  Platform ID (0x%08x)   = 0x%016lx\n", MSR_IA32_PLATFORM_ID, AsmReadMsr64 (MSR_IA32_PLATFORM_ID)));

  DEBUG ((DEBUG_INFO, "==== TestPointDumpCpuInfo - Exit\n"));
}
