/** @file
  Header file for the PeiSaPolicy library.

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#ifndef _PEI_SA_POLICY_LIBRARY_H_
#define _PEI_SA_POLICY_LIBRARY_H_

#include <CpuRegs.h>
#include <SaPolicyCommon.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/SmbusLib.h>
#include <Library/PcdLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MmPciLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/CpuPlatformLib.h>
#include <Library/ConfigBlockLib.h>
#include <Ppi/SiPolicy.h>
#include <Library/PeiSaPolicyLib.h>
#include <Library/SiConfigBlockLib.h>

/**
  SaLoadSamplePolicyPreMem - Load some policy default for reference board.

  @param[in] ConfigBlockTableAddress    The pointer for SA config blocks

**/
VOID
SaLoadSamplePolicyPreMem (
  IN VOID           *ConfigBlockTableAddress
  );
#endif // _PEI_SA_POLICY_LIBRARY_H_
