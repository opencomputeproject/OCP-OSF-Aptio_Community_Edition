/**
 * @file  SdciDataInit.c
 * @brief Initialize Sdci data prior to openSIL execution.
 *
 */
/**
 * Copyright 2021-2024 Advanced Micro Devices, Inc. All rights reserved.
 *
 */

#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Sil-api.h>
#include <xSIM.h>
#include <Sdci/SdciClass-api.h>
#include <PiPei.h>

/**
 * SetSdciData
 *
 * @brief Set the SDCI input defaults
 * @details
 *      Locate the SDCI - the resource initialization IP block
 * @return EFI_SUCCESS or EFI_DEVICE_ERROR
 */
EFI_STATUS
SetSdciData (
  void
  )
{
  SDCICLASS_INPUT_BLK    *SdciData;
  
  SdciData = (SDCICLASS_INPUT_BLK *)SilFindStructure (SilId_SdciClass,  0);
  DEBUG ((DEBUG_ERROR, "SIL SDCI memory block is found at: 0x%x \n", SdciData));
  if (SdciData == NULL) {
    return EFI_NOT_FOUND; // Could not find the IP input block
  }

  SdciData->AmdFabricSdci = PcdGetBool (PcdAmdFabricSdci);

  DEBUG ((DEBUG_INFO, "SIL SDCI PCD AmdFabricSdci: 0x%x \n", SdciData->AmdFabricSdci));

  return EFI_SUCCESS;
}
