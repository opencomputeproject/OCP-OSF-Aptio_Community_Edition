/** @file
  This file is SampleCode for Intel PEI Platform Policy initialization.

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PeiPolicyInit.h"

/**
  Performs silicon pre-mem policy initialization.

  The meaning of Policy is defined by silicon code.
  It could be the raw data, a handle, a PPI, etc.

  The returned data must be used as input data for SiliconPolicyDonePreMem(),
  and SiliconPolicyUpdateLib.SiliconPolicyUpdatePreMem().

  1) In FSP path, the input Policy should be FspmUpd.
  Value of FspmUpd has been initialized by FSP binary default value.
  Only a subset of FspmUpd needs to be updated for different silicon sku.
  The return data is same FspmUpd.

  2) In non-FSP path, the input policy could be NULL.
  The return data is the initialized policy.

  @param[in, out] Policy       Pointer to policy.

  @return the initialized policy.
**/
VOID *
EFIAPI
SiliconPolicyInitPreMem (
  IN OUT VOID *Policy
  )
{
  EFI_STATUS                   Status;
  SI_PREMEM_POLICY_PPI         *SiPreMemPolicyPpi;

  DEBUG ((DEBUG_INFO, "Silicon PEI Policy Initialization Start in Pre-Memory...\n"));

  ASSERT (Policy == NULL);

  //
  // Call SiCreatePreMemConfigBlocks to initialize platform policy structure
  // and get all intel default policy settings.
  //
  Status = SiCreatePreMemConfigBlocks (&SiPreMemPolicyPpi);
  ASSERT_EFI_ERROR (Status);

  return SiPreMemPolicyPpi;
}

/*
  The silicon pre-mem policy is finalized.
  Silicon code can do initialization based upon the policy data.

  The input Policy must be returned by SiliconPolicyInitPreMem().
  
  @param[in] Policy       Pointer to policy.

  @retval EFI_SUCCESS The policy is handled consumed by silicon code.
*/
EFI_STATUS
EFIAPI
SiliconPolicyDonePreMem (
  IN VOID *Policy
  )
{
  EFI_STATUS                   Status;
  SI_PREMEM_POLICY_PPI         *SiPreMemPolicyPpi;

  SiPreMemPolicyPpi = Policy;

  //
  // Install SiPreMemPolicyPpi.
  // While installed, RC assumes the Policy is ready and finalized. So please
  // update and override any setting before calling this function.
  //
  Status = SiPreMemInstallPolicyPpi (SiPreMemPolicyPpi);
  ASSERT_EFI_ERROR (Status);

  DEBUG ((DEBUG_INFO, "Silicon PEI Policy Initialization Done in Pre-Memory\n"));

  return Status;
}
