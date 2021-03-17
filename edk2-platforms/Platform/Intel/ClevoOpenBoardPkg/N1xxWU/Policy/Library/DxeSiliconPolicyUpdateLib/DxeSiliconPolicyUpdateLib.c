/** @file
  DXE silicon policy update library.

Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/SiliconPolicyUpdateLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>

#include "DxeSaPolicyInit.h"
#include "DxeGopPolicyInit.h"

/**
  Performs silicon late policy update.

  The meaning of Policy is defined by silicon code.
  It could be the raw data, a handle, a Protocol, etc.

  The input Policy must be returned by SiliconPolicyDoneLate().

  In FSP or non-FSP path, the board may use additional way to get
  the silicon policy data field based upon the input Policy.

  @param[in, out] Policy       Pointer to policy.

  @return the updated policy.
**/
VOID *
EFIAPI
SiliconPolicyUpdateLate (
  IN VOID *Policy
  )
{
  SA_POLICY_PROTOCOL       *SaPolicy;
  EFI_STATUS               Status;

  SaPolicy = Policy;
  UpdateDxeSaPolicy (SaPolicy);

  if (PcdGetBool(PcdIntelGopEnable)) {
    //
    // GOP Dxe Policy Initialization
    //
    Status = GopPolicyInitDxe(gImageHandle);
    DEBUG((DEBUG_INFO, "GOP Dxe Policy Initialization done\n"));
    ASSERT_EFI_ERROR(Status);
  }

  return Policy;
}

