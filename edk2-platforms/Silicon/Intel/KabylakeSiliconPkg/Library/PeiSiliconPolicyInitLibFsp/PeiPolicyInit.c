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
  Initialize Intel PEI Platform Policy

**/
VOID
EFIAPI
PeiPolicyInit (
  VOID
  )
{
  EFI_STATUS                   Status;
  SI_POLICY_PPI                *SiPolicyPpi;

  //
  // Call SiCreateConfigBlocks to initialize Silicon Policy structure
  // and get all Intel default policy settings.
  //
  Status = SiCreateConfigBlocks (&SiPolicyPpi);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) {
    return;
  }

  //
  // Install SiPolicyPpi.
  // While installed, RC assumes the Policy is ready and finalized. So please
  // update and override any setting before calling this function.
  //
  Status = SiInstallPolicyPpi (SiPolicyPpi);
  ASSERT_EFI_ERROR (Status);
}
