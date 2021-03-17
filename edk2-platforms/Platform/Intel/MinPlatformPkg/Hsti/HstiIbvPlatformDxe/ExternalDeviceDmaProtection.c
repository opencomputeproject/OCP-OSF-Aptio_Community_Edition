/** @file
  This file contains sample for setting external DMA protection bit
  in sample IBV HSTI structure. Actual test implementation is not present
  in this sample driver.

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "HstiIbvPlatformDxe.h"

/**
  This function sets the verified bit for ExternalDeviceDmaProtection.
  Actual test implementation is not present in this sample function.
**/
VOID
CheckExternalDeviceDmaProtection (
  IN UINT32                   Role
  )
{
  EFI_STATUS  Status;
  BOOLEAN     Result;

  if ((mFeatureImplemented[2] & HSTI_BYTE2_EXTERNAL_DEVICE_DMA_PROTECTION) == 0) {
    return ;
  }
  Result = TRUE;

  DEBUG ((EFI_D_INFO, "  External Device DMA Protection \n"));

  //
  // ALL PASS
  //
  if (Result) {
    Status = HstiLibSetFeaturesVerified (
               Role,
               NULL,
               2,
               HSTI_BYTE2_EXTERNAL_DEVICE_DMA_PROTECTION
               );
    ASSERT_EFI_ERROR (Status);
  }


  return ;
}
