/**
 * @file  RcMgrDataInit.c
 * @brief Initialize RC manager data prior to openSIL execution.
 *
 */
/**
 * Copyright 2021-2024 Advanced Micro Devices, Inc. All rights reserved.
 *
 */
#include <Library/DebugLib.h>
#include <RcMgr/DfX/RcManager4-api.h>
#include <Sil-api.h>
#include <xSIM-api.h>
#include <Library/PcdLib.h>

EFI_STATUS
PlatformFabricDataInit(
    IN OUT DFX_RCMGR_INPUT_BLK *DFRcmgrInputBlock);

/**
 * SetConfigRcMgr
 *
 * @brief Set the data in resource manager init IP block
 * @details
 *      1. Locate the RcMgrData - the resource initialization IP block
 *      2. Use the found IP block to call FabricResourceInit; the IP block data is updated in the FabricResourceInit call.
 *         There are no defaults for RcMgrData, the SetInput() function
 *      of this IP block is NULL.
 * @return EFI_SUCCESS or EFI_DEVICE_ERROR
 */

EFI_STATUS
SetConfigRcMgr(
    void)
{
  DFX_RCMGR_DATA_BLK *RcMgrData;
  EFI_STATUS Status;

  RcMgrData = (DFX_RCMGR_DATA_BLK *)SilFindStructure(SilId_RcManager, 0);
  DEBUG((DEBUG_ERROR, "SIL RC Init memory block is found blk at: 0x%x \n", RcMgrData));
  if (RcMgrData == NULL)
  {
    return EFI_NOT_FOUND; // Could not find the IP input block
  }

  RcMgrData->DFXRcmgrOutputBlock.AmdFabricCcxAsNumaDomain = PcdGetBool(PcdAmdFabricCcxAsNumaDomain);

  Status = PlatformFabricDataInit(&RcMgrData->DFXRcmgrInputBlock);
  if (EFI_ERROR(Status))
    return EFI_DEVICE_ERROR;

  return EFI_SUCCESS;
}
