/** @file

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _PEI_PCH_POLICY_LIB_H_
#define _PEI_PCH_POLICY_LIB_H_

#include <Ppi/PchPolicy.h>

/**
  Print whole PCH_POLICY_PPI and serial out.

  @param[in] PchPolicyPpi               The RC Policy PPI instance
**/
VOID
EFIAPI
PchPrintPolicyPpi (
  IN  PCH_POLICY_PPI           *PchPolicyPpi
  );

/**
  PchCreatePolicyDefaults creates the default setting of PCH Policy.
  It allocates and zero out buffer, and fills in the Intel default settings.

  @param[out] PchPolicyPpi              The pointer to get PCH Policy PPI instance

  @retval EFI_SUCCESS                   The policy default is initialized.
  @retval EFI_OUT_OF_RESOURCES          Insufficient resources to create buffer
**/
EFI_STATUS
EFIAPI
PchCreatePolicyDefaults (
  OUT  PCH_POLICY_PPI          **PchPolicyPpi
  );

/**
  PchInstallPolicyPpi installs PchPolicyPpi.
  While installed, RC assumes the Policy is ready and finalized. So please update and override
  any setting before calling this function.

  @param[in] PchPolicyPpi               The pointer to PCH Policy PPI instance

  @retval EFI_SUCCESS                   The policy is installed.
  @retval EFI_OUT_OF_RESOURCES          Insufficient resources to create buffer
**/
EFI_STATUS
EFIAPI
PchInstallPolicyPpi (
  IN  PCH_POLICY_PPI           *PchPolicyPpi
  );

/*
  Apply RVP3 PCH specific default settings

  @param[in] PchPolicyPpi      The pointer to PCH Policy PPI instance
*/
VOID
EFIAPI
PchRvp3DefaultPolicy (
  IN  PCH_POLICY_PPI           *PchPolicy
  );

#endif // _PEI_PCH_POLICY_LIB_H_
