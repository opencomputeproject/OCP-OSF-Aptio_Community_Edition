/** @file
  Prototype of the MePolicyLibPei library.

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#ifndef _PEI_ME_POLICY_LIB_H_
#define _PEI_ME_POLICY_LIB_H_

#include <Ppi/SiPolicy.h>
#include <Library/ConfigBlockLib.h>

/**
  This function prints the PEI phase PreMem policy.

  @param[in] SiPolicyPreMemPpi              The RC PreMem Policy PPI instance
**/
VOID
EFIAPI
MePrintPolicyPpiPreMem (
  IN  SI_PREMEM_POLICY_PPI *SiPolicyPreMemPpi
  );

/**
  This function prints the PEI phase policy.

  @param[in] SiPolicyPpi              The RC Policy PPI instance
**/
VOID
EFIAPI
MePrintPolicyPpi (
  IN  SI_POLICY_PPI     *SiPolicyPpi
  );

/**
  Get Me config block table total size.

  @retval     Size of Me config block table
**/
UINT16
EFIAPI
MeGetConfigBlockTotalSize (
  VOID
  );

/**
  Get ME config block table total size.

  @retval      Size of ME config block table
**/
UINT16
EFIAPI
MeGetConfigBlockTotalSizePreMem (
  VOID
  );

/**
  MeAddConfigBlocksPreMem add all ME config blocks.

  @param[in] ConfigBlockTableAddress    The pointer to add ME config blocks

  @retval EFI_SUCCESS                   The policy default is initialized.
  @retval EFI_OUT_OF_RESOURCES          Insufficient resources to create buffer
**/
EFI_STATUS
EFIAPI
MeAddConfigBlocksPreMem (
  IN VOID           *ConfigBlockTableAddress
  );

/**
  MeAddConfigBlocks add all ME config blocks.

  @param[in] ConfigBlockTableAddress    The pointer to add ME config blocks

  @retval EFI_SUCCESS                   The policy default is initialized.
  @retval EFI_OUT_OF_RESOURCES          Insufficient resources to create buffer
**/
EFI_STATUS
EFIAPI
MeAddConfigBlocks (
  IN VOID           *ConfigBlockTableAddress
  );

#endif // _PEI_ME_POLICY_LIB_H_
