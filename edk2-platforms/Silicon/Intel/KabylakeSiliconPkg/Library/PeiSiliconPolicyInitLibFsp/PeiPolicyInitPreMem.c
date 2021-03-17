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
PeiPolicyInitPreMem (
  VOID
  )
{
  EFI_STATUS                   Status;
  SI_PREMEM_POLICY_PPI         *SiPreMemPolicyPpi;

  DEBUG ((DEBUG_INFO, "Silicon PEI Policy Initialization Start in Pre-Memory...\n"));
  //
  // Call SiCreatePreMemConfigBlocks to initialize platform policy structure
  // and get all intel default policy settings.
  //
  Status = SiCreatePreMemConfigBlocks (&SiPreMemPolicyPpi);
  ASSERT_EFI_ERROR (Status);

  //
  // Install SiPreMemPolicyPpi.
  // While installed, RC assumes the Policy is ready and finalized. So please
  // update and override any setting before calling this function.
  //
  Status = SiPreMemInstallPolicyPpi (SiPreMemPolicyPpi);
  ASSERT_EFI_ERROR (Status);


  DEBUG ((DEBUG_INFO, "Silicon PEI Policy Initialization Done in Pre-Memory\n"));
}
