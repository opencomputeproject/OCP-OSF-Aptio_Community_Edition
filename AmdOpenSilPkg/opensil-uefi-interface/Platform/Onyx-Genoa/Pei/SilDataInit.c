/* Copyright 2021-2023 Advanced Micro Devices, Inc. All rights reserved. */
/**
 * @file  SilDataInit.c
 * @brief PEIM created to execute Onyx SI OpenSIL call.
 *
 */

#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
#include <Sil-api.h>
#include <SilPei.h>
#include <xPrfServicesPei.h>

EFI_STATUS SetConfigRcMgr (VOID);
EFI_STATUS SetCcxData (VOID);
EFI_STATUS SetFchData (VOID);
EFI_STATUS CcxDataBackToHostFW (VOID);
EFI_STATUS SetMpioData (VOID);
EFI_STATUS SetSdciData (VOID);
EFI_STATUS SetCxlData (VOID);
EFI_STATUS CxlDataBackToHostFW (VOID);
EFI_STATUS SetNbioData (VOID);

/**
 * OnyxlIpBlocksInit
 *
 * Initialize IP blocks with the host FW specific data: PCDs, Setup variables, etc.
 */
EFI_STATUS OnyxIpBlocksInit (VOID)
{
  EFI_STATUS Status;

  // Place Onyx IP data init code that depends on Host FW (PCDs, Setup questions, etc.)
  Status = SetCcxData ();
  ASSERT_EFI_ERROR (Status);

  Status = SetConfigRcMgr ();
  ASSERT_EFI_ERROR (Status);

  Status = SetFchData ();
  ASSERT_EFI_ERROR (Status);

  Status = SetNbioData ();
  ASSERT_EFI_ERROR (Status);

  Status = SetMpioData ();
  ASSERT_EFI_ERROR (Status);

  Status = SetSdciData ();
  ASSERT_EFI_ERROR (Status);

  Status = SetCxlData ();
  ASSERT_EFI_ERROR (Status);

  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

/**
 * IPBlockDataBackToHostFW
 *
 * Send Updated IP blocks Data to host FW specific data: PCDs, Setup variables, etc.
 */
EFI_STATUS IPBlockDataBackToHostFW (VOID)
{
  EFI_STATUS Status;

  Status = CcxDataBackToHostFW ();
  ASSERT_EFI_ERROR (Status);

  Status = CxlDataBackToHostFW ();
  ASSERT_EFI_ERROR (Status);
  return Status;
}

/**
 *  Onyx openSIL FW PEI init driver entry point
 *
 *  @param FileHandle   This file handle
 *  @param PeiServices  Pointer to PEI services
 *
 *  @return EFI_SUCCESS if Ethanol IP block data initialization is successful
 */
EFI_STATUS
EFIAPI
SilPeiEntryPoint (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS  Status;

  Status = SilFwDataInit (PeiServices, OnyxIpBlocksInit);
  ASSERT_EFI_ERROR (Status);

  xPrfServicesPpiInstall();

  return Status;
}
