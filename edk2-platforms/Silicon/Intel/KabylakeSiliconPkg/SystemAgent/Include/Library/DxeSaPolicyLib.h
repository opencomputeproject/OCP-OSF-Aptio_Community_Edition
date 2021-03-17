/** @file
  Prototype of the DxeSaPolicyLib library.

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#ifndef _DXE_SA_POLICY_LIB_H_
#define _DXE_SA_POLICY_LIB_H_

#include <Protocol/SaPolicy.h>

/**
  This function prints the DXE phase policy.

  @param[in] SaPolicy    - SA DXE Policy protocol
**/
VOID
SaPrintPolicyProtocol (
  IN  SA_POLICY_PROTOCOL         *SaPolicy
  )
;

/**
  CreateSaDxeConfigBlocks generates the config blocksg of SA DXE Policy.
  It allocates and zero out buffer, and fills in the Intel default settings.

  @param[out] SaPolicy                  The pointer to get SA Policy Protocol instance

  @retval EFI_SUCCESS                   The policy default is initialized.
  @retval EFI_OUT_OF_RESOURCES          Insufficient resources to create buffer
**/
EFI_STATUS
EFIAPI
CreateSaDxeConfigBlocks(
  IN OUT  SA_POLICY_PROTOCOL      **SaPolicy
);

/**
  SaInstallPolicyProtocol installs SA Policy.
  While installed, RC assumes the Policy is ready and finalized. So please update and override
  any setting before calling this function.

  @param[in] ImageHandle                Image handle of this driver.
  @param[in] SaPolicy                   The pointer to SA Policy Protocol instance

  @retval EFI_SUCCESS                   The policy is installed.
  @retval EFI_OUT_OF_RESOURCES          Insufficient resources to create buffer

**/
EFI_STATUS
EFIAPI
SaInstallPolicyProtocol (
  IN  EFI_HANDLE                  ImageHandle,
  IN  SA_POLICY_PROTOCOL         *SaPolicy
  )
;

#endif // _DXE_SA_POLICY_LIB_H_
