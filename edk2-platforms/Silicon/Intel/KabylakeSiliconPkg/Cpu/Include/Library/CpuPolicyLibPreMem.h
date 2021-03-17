/** @file
  Prototype of the CpuPolicy library.

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#ifndef _CPU_PREMEM_POLICY_LIB_H_
#define _CPU_PREMEM_POLICY_LIB_H_

#include <Ppi/SiPolicy.h>

/**
  Print whole CPU related config blocks of SI_PREMEM_POLICY_PPI and serial out.

  @param[in] SiPreMemPolicyPpi             The Si PreMem Policy PPI instance
**/
VOID
CpuPreMemPrintPolicy (
IN  SI_PREMEM_POLICY_PPI *SiPreMemPolicyPpi
);

/**
  Get CPU PREMEM config block table total size.

  @retval Size of CPU PREMEM config block table
**/
UINT16
EFIAPI
CpuGetPreMemConfigBlockTotalSize (
  VOID
  );

/**
  CpuAddPreMemConfigBlocks add all CPU PREMEM config blocks.

  @param[in] ConfigBlockTableAddress    The pointer to add CPU PREMEM config blocks

  @retval EFI_SUCCESS                   The policy default is initialized.
  @retval EFI_OUT_OF_RESOURCES          Insufficient resources to create buffer
**/
EFI_STATUS
EFIAPI
CpuAddPreMemConfigBlocks (
  IN     VOID      *ConfigBlockTableAddress
  );

#endif // _CPU_PREMEM_POLICY_LIB_H_
