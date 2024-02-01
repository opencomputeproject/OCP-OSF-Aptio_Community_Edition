/**
 * @file  CxlDataInit.c
 * @brief Initialize Cxl data prior to openSIL execution.
 *
 */
/**
 * Copyright 2021-2024 Advanced Micro Devices, Inc. All rights reserved.
 *
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <xSIM-api.h>
#include <Cxl/CxlClass-api.h>

/**
 * SetCxlData
 *
 * @brief Set the CXL input defaults
 * @details
 *      Locate the CXL - the resource initialization IP block
 * @return EFI_SUCCESS or EFI_DEVICE_ERROR
 */
EFI_STATUS
SetCxlData (
  void
  )
{
  CXLCLASS_DATA_BLK            *CxlData;
  
  CxlData = (CXLCLASS_DATA_BLK *)SilFindStructure (SilId_CxlClass,  0);
  DEBUG ((DEBUG_ERROR, "SIL CXL memory block is found at: 0x%x \n", CxlData));
  if (CxlData == NULL) {
    return EFI_NOT_FOUND; // Could not find the IP input block
  }

  CxlData->CxlInputBlock.ReportErrorsToRcec                  = PcdGetBool (PcdReportErrorsToRcec);
  CxlData->CxlInputBlock.CxlIoArbWeights                     = PcdGet8 (PcdCxlIoArbWeights);
  CxlData->CxlInputBlock.CxlCaMemArbWeights                  = PcdGet8 (PcdCxlCaMemArbWeights);
  CxlData->CxlInputBlock.CnliTokenAdvertisement              = PcdGet8 (PcdCnliTokenAdvertisement);
  CxlData->CxlInputBlock.AmdCxlProtocolErrorReporting        = PcdGet8 (PcdAmdCxlProtocolErrorReporting);
  CxlData->CxlInputBlock.AmdPcieAerReportMechanism           = PcdGet8 (PcdAmdPcieAerReportMechanism);
  CxlData->CxlInputBlock.CxlCamemRxOptimization              = PcdGetBool (PcdCxlCamemRxOptimization);
  CxlData->CxlInputBlock.CxlTxOptimizeDirectOutEn            = PcdGetBool (PcdCxlTxOptimizeDirectOutEn);

  DEBUG ((DEBUG_INFO, "SIL CXL PCD ReportErrorsToRcec: 0x%x \n", CxlData->CxlInputBlock.ReportErrorsToRcec));
  DEBUG ((DEBUG_INFO, "SIL CXL PCD CxlIoArbWeights: 0x%x \n", CxlData->CxlInputBlock.CxlIoArbWeights));
  DEBUG ((DEBUG_INFO, "SIL CXL PCD CxlCaMemArbWeights: 0x%x \n", CxlData->CxlInputBlock.CxlCaMemArbWeights));
  DEBUG ((DEBUG_INFO, "SIL CXL PCD CnliTokenAdvertisement: 0x%x \n", CxlData->CxlInputBlock.CnliTokenAdvertisement));
  DEBUG ((DEBUG_INFO, "SIL CXL PCD AmdCxlProtocolErrorReporting: 0x%x \n", CxlData->CxlInputBlock.AmdCxlProtocolErrorReporting));
  DEBUG ((DEBUG_INFO, "SIL CXL PCD AmdPcieAerReportMechanism: 0x%x \n", CxlData->CxlInputBlock.AmdPcieAerReportMechanism));
  DEBUG ((DEBUG_INFO, "SIL CXL PCD CxlCamemRxOptimization: 0x%x \n", CxlData->CxlInputBlock.CxlCamemRxOptimization));
  DEBUG ((DEBUG_INFO, "SIL CXL PCD CxlTxOptimizeDirectOutEn: 0x%x \n", CxlData->CxlInputBlock.CxlTxOptimizeDirectOutEn));

  return EFI_SUCCESS;
}

/**
 * CxlDataBackToHostFW
 *
 * @brief Send Updated CXL IP block's data to host FW
 * @return EFI_SUCCESS or EFI_NOT_FOUND
 *       EFI_SUCCESS   : Received valid address within the Host allocated memory block 
 *                       and succesfully update the PCD.
 *       EFI_NOT_FOUND : Indicates the requested block was not found
 */
EFI_STATUS
CxlDataBackToHostFW (
  void
  )
{
  EFI_STATUS status;
  CXLCLASS_DATA_BLK *CxlDataHostFw;

  CxlDataHostFw = (CXLCLASS_DATA_BLK *)SilFindStructure (SilId_CxlClass,  0);
  DEBUG ((DEBUG_ERROR, "CxlDataBackToHostFW SIL CXL memory block at: 0x%x \n", CxlDataHostFw));
  if (CxlDataHostFw == NULL) {
    return EFI_NOT_FOUND; // Could not find the IP input block
  }

  status = PcdSet8S(PcdAmdPcieAerReportMechanism, CxlDataHostFw->CxlOutputBlock.AmdPcieAerReportMechanism);
  DEBUG ((DEBUG_ERROR, "PcdAmdPcieAerReportMechanism update: %d\n", status));

  return EFI_SUCCESS;
}
