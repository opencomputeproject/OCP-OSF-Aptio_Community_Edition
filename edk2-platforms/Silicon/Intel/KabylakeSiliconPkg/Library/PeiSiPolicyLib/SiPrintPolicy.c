/** @file
  This file is PeiSiPolicyLib library for printing Policy settings.

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PeiSiPolicyLibrary.h"

/**
  Print whole SI_POLICY_PPI and serial out.

  @param[in] SiPolicyPpi The RC Policy PPI instance
**/
VOID
EFIAPI
SiPrintPolicyPpi (
  IN  SI_POLICY_PPI          *SiPolicyPpi
  )
{
  DEBUG_CODE_BEGIN ();
  SI_CONFIG     *SiConfig;
  EFI_STATUS    Status;

  Status = GetConfigBlock ((VOID *) SiPolicyPpi, &gSiConfigGuid, (VOID *) &SiConfig);
  ASSERT_EFI_ERROR (Status);

  DEBUG ((DEBUG_INFO, "------------------------ Silicon Print Policy Start ------------------------\n"));
  DEBUG ((DEBUG_INFO, " TempPciBusMin= %x\n", SiConfig->TempPciBusMin));
  DEBUG ((DEBUG_INFO, " TempPciBusMax= %x\n", SiConfig->TempPciBusMax));
  DEBUG ((DEBUG_INFO, " TempMemBaseAddr= %x\n", SiConfig->TempMemBaseAddr));
  DEBUG ((DEBUG_INFO, " TempMemSize= %x\n", SiConfig->TempMemSize));
  DEBUG ((DEBUG_INFO, " TempIoBaseAddr= %x\n", SiConfig->TempIoBaseAddr));
  DEBUG ((DEBUG_INFO, " TempIoSize= %x\n", SiConfig->TempIoSize));
  DEBUG ((DEBUG_INFO, " CsmFlag= %x\n", SiConfig->CsmFlag));
  DEBUG ((DEBUG_INFO, "------------------------ Silicon Print Policy End --------------------------\n"));
  DEBUG_CODE_END ();
}

