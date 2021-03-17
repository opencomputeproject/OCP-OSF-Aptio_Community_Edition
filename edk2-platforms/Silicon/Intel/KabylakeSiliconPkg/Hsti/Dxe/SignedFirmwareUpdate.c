/** @file
  This file contains the tests for the SignedFirmwareUpdate bit

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "HstiSiliconDxe.h"

/**
  Run tests for SignedFirmwareUpdate bit
**/
VOID
CheckSignedFirmwareUpdate (
  VOID
  )
{
  if ((mFeatureImplemented[0] & HSTI_BYTE0_SIGNED_FIRMWARE_UPDATE) == 0) {
    return;
  }

  return ;
}
