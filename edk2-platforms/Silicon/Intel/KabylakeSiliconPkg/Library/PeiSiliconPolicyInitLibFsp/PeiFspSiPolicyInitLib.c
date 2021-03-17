/** @file
  Implementation of Fsp SI Policy Initialization.

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PeiFspPolicyInitLib.h>
#include <Ppi/SiPolicy.h>

/**
  Performs FSP SI PEI Policy initialization.

  @param[in][out]  FspmUpd             Pointer to FSP UPD Data.

  @retval          EFI_SUCCESS         FSP UPD Data is updated.
  @retval          EFI_NOT_FOUND       Fail to locate required PPI.
  @retval          Other               FSP UPD Data update process fail.
**/
EFI_STATUS
EFIAPI
PeiFspSiPolicyInitPreMem (
  IN OUT FSPM_UPD    *FspmUpd
  )
{
  EFI_STATUS                Status;
  SI_PREMEM_POLICY_PPI      *SiPreMemPolicyPpi;

  //
  // Locate SiPolicyPpi
  //
  SiPreMemPolicyPpi = NULL;
  Status = PeiServicesLocatePpi (
             &gSiPreMemPolicyPpiGuid,
             0,
             NULL,
             (VOID **) &SiPreMemPolicyPpi
             );
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}
