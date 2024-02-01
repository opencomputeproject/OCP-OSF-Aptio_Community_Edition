/**
 * @file  MpioDataInit.c
 * @brief Initialize Mpio data prior to openSIL execution.
 *
 */
/**
 * Copyright 2021-2024 Advanced Micro Devices, Inc. All rights reserved.
 *
 */
#include <PiPei.h>
#include <Sil-api.h>
#include <Mpio/MpioClass-api.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/PeiServicesTablePointerLib.h>

EFI_STATUS
PlatformSetMpioData (
  MPIOCLASS_INPUT_BLK           *MpioData
  );

/**
 * SetMpioData
 *
 * @brief Set the MPIO input defaults
 * @details
 *      Locate the MPIO - the resource initialization IP block
 * @return EFI_SUCCESS or EFI_DEVICE_ERROR
 */
EFI_STATUS
SetMpioData (
  void
  )
{
  EFI_STATUS                    Status;
  MPIOCLASS_INPUT_BLK           *MpioData;

  MpioData = (MPIOCLASS_INPUT_BLK *)SilFindStructure (SilId_MpioClass,  0);
  DEBUG ((DEBUG_ERROR, "SIL MPIO memory block is found at: 0x%x \n", MpioData));
  if (MpioData == NULL) {
    return EFI_NOT_FOUND; // Could not find the IP input block
  }

  MpioData->CfgDxioClockGating                  = PcdGetBool (PcdCfgDxioClockGating);
  MpioData->PcieDxioTimingControlEnable         = PcdGetBool (PcdPcieDxioTimingControlEnable);
  MpioData->PCIELinkReceiverDetectionPolling    = PcdGet32 (PcdPCIELinkReceiverDetectionPolling);
  MpioData->PCIELinkResetToTrainingTime         = PcdGet32 (PcdPCIELinkResetToTrainingTime);
  MpioData->PCIELinkL0Polling                   = PcdGet32 (PcdPCIELinkL0Polling);
  MpioData->PCIeExactMatchEnable                = PcdGetBool (PcdPCIeExactMatchEnable);
  MpioData->DxioPhyValid                        = PcdGet8 (PcdDxioPhyValid);
  MpioData->DxioPhyProgramming                  = PcdGet8 (PcdDxioPhyProgramming);
  MpioData->CfgSkipPspMessage                   = PcdGet8 (PcdCfgSkipPspMessage);
  MpioData->DxioSaveRestoreModes                = PcdGet8 (PcdDxioSaveRestoreModes);
  MpioData->AmdAllowCompliance                  = PcdGet16 (PcdAmdAllowCompliance);
  MpioData->SrisEnableMode                      = PcdGet8 (PcdSrisEnableMode);
  MpioData->SrisSkipInterval                    = PcdGet8 (PcdSrisSkipInterval);
  MpioData->SrisSkpIntervalSel                  = PcdGet8 (PcdSrisSkpIntervalSel);
  MpioData->SrisCfgType                         = PcdGet8 (PcdSrisCfgType);
  MpioData->SrisAutoDetectMode                  = PcdGet8 (PcdSrisAutoDetectMode);
  MpioData->SrisAutodetectFactor                = PcdGet8 (PcdSrisAutodetectFactor);
  MpioData->SrisLowerSkpOsGenSup                = PcdGet8 (PcdSrisLowerSkpOsGenSup);
  MpioData->SrisLowerSkpOsRcvSup                = PcdGet8( PcdSrisLowerSkpOsRcvSup);
  MpioData->AmdCxlOnAllPorts                    = PcdGetBool (PcdAmdCxlOnAllPorts);
  MpioData->CxlCorrectableErrorLogging          = PcdGetBool (PcdCxlCorrectableErrorLogging);
  MpioData->CxlUnCorrectableErrorLogging        = PcdGetBool (PcdCxlUnCorrectableErrorLogging);
  // This is also available in Nbio. How to handle duplicate entries?
  MpioData->CfgAEREnable                        = PcdGetBool (PcdCfgAEREnable);
  MpioData->CfgMcCapEnable                      = PcdGetBool (PcdCfgMcCapEnable);
  MpioData->CfgRcvErrEnable                     = PcdGetBool( PcdCfgRcvErrEnable);
  // This is also available in Nbio. How to handle duplicate entries?
  MpioData->EarlyBmcLinkTraining                = PcdGetBool (PcdEarlyBmcLinkTraining);
  MpioData->EarlyBmcLinkSocket                  = PcdGet8 (PcdEarlyBmcLinkSocket);
  MpioData->EarlyBmcLinkLaneNum                 = PcdGet8 (PcdEarlyBmcLinkLaneNum);
  MpioData->EarlyBmcLinkDie                     = PcdGet8 (PcdEarlyBmcLinkDie);
  MpioData->SurpriseDownFeature                 = PcdGetBool (PcdSurpriseDownFeature);
  MpioData->LcMultAutoSpdChgOnLastRateEnable    = PcdGetBool (PcdLcMultAutoSpdChgOnLastRateEnable);
  MpioData->AmdRxMarginEnabled                  = PcdGetBool (PcdAmdRxMarginEnabled);
  MpioData->CfgPcieCVTestWA                     = PcdGet8 (PcdCfgPcieCVTestWA);
  // This is also available in Nbio. How to handle duplicate entries?
  MpioData->CfgPcieAriSupport                   = PcdGetBool (PcdCfgPcieAriSupport);
  MpioData->CfgNbioCTOtoSC                      = PcdGetBool (PcdCfgNbioCTOtoSC);
  MpioData->CfgNbioCTOIgnoreError               = PcdGetBool (PcdCfgNbioCTOIgnoreError);
  MpioData->CfgNbioSsid                         = PcdGet32 (PcdCfgNbioSsid);
  MpioData->CfgIommuSsid                        = PcdGet32 (PcdCfgIommuSsid);
  MpioData->CfgPspccpSsid                       = PcdGet32 (PcdCfgPspccpSsid);
  MpioData->CfgNtbccpSsid                       = PcdGet32 (PcdCfgNtbccpSsid);
  MpioData->CfgNbifF0Ssid                       = PcdGet32 (PcdCfgNbifF0Ssid);
  MpioData->CfgNtbSsid                          = PcdGet32 (PcdCfgNtbSsid);
  MpioData->AmdPcieSubsystemDeviceID            = PcdGet16 (PcdAmdPcieSubsystemDeviceID);
  MpioData->AmdPcieSubsystemVendorID            = PcdGet16 (PcdAmdPcieSubsystemVendorID);
  MpioData->GppAtomicOps                        = PcdGet8 (PcdGppAtomicOps);
  MpioData->GfxAtomicOps                        = PcdGet8 (PcdGfxAtomicOps);
  MpioData->AmdNbioReportEdbErrors              = PcdGetBool (PcdAmdNbioReportEdbErrors);
  // This is also available in Nbio. How to handle duplicate entries?
  MpioData->OpnSpare                            = PcdGet32 (PcdOpnSpare);
  MpioData->AmdPreSilCtrl0                      = PcdGet32 (PcdAmdPreSilCtrl0);
  MpioData->MPIOAncDataSupport                  = PcdGetBool (PcdMPIOAncDataSupport);
  MpioData->AfterResetDelay                     = PcdGet16 (PcdAfterResetDelay);
  MpioData->CfgEarlyLink                        = PcdGetBool (PcdCfgEarlyLink);
  MpioData->AmdCfgExposeUnusedPciePorts         = PcdGet8 (PcdAmdCfgExposeUnusedPciePorts);
  MpioData->CfgForcePcieGenSpeed                = PcdGet8 (PcdCfgForcePcieGenSpeed);
  MpioData->CfgSataPhyTuning                    = PcdGet8 (PcdCfgSataPhyTuning);
  MpioData->PcieLinkComplianceModeAllPorts      = PcdGetBool (PcdPcieLinkComplianceModeAllPorts);
  MpioData->AmdMCTPEnable                       = PcdGetBool (PcdAmdMCTPEnable);
  MpioData->SbrBrokenLaneAvoidanceSup           = PcdGetBool (PcdSbrBrokenLaneAvoidanceSup);
  MpioData->AutoFullMarginSup                   = PcdGetBool (PcdAutoFullMarginSup);
  // A getter and setter, both are needed for this PCD.
  MpioData->AmdPciePresetMask8GtAllPort         = PcdGet32 (PcdAmdPciePresetMask8GtAllPort);
  // A getter and setter, both are needed for this PCD.
  MpioData->AmdPciePresetMask16GtAllPort        = PcdGet32 (PcdAmdPciePresetMask16GtAllPort);
  // A getter and setter, both are needed for this PCD.
  MpioData->AmdPciePresetMask32GtAllPort        = PcdGet32 (PcdAmdPciePresetMask32GtAllPort);
  MpioData->PcieLinkAspmAllPort                 = PcdGet8 (PcdPcieLinkAspmAllPort);

  // Is this needed? Ideally we only need to pass back the assigned value to the host. 
  // MpioData->AmdMCTPMasterSeg               = PcdGet8(PcdAmdMCTPMasterSeg);

  // Is this needed? Ideally we only need to pass back the assigned value to the host.
  // MpioData->AmdMCTPMasterID                = PcdGet16(PcdAmdMCTPMasterID);

  MpioData->SyncHeaderByPass                    = PcdGetBool (PcdSyncHeaderByPass);
  MpioData->CxlTempGen5AdvertAltPtcl            = PcdGetBool (PcdCxlTempGen5AdvertAltPtcl);

  DEBUG ((DEBUG_INFO, "SIL MPIO PCD CfgDxioClockGating: 0x%x \n", MpioData->CfgDxioClockGating));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD PcieDxioTimingControlEnable: 0x%x \n", MpioData->PcieDxioTimingControlEnable));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD PCIELinkReceiverDetectionPolling: 0x%x \n", MpioData->PCIELinkReceiverDetectionPolling));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD PCIELinkResetToTrainingTime: 0x%x \n", MpioData->PCIELinkResetToTrainingTime));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD PCIELinkL0Polling: 0x%x \n", MpioData->PCIELinkL0Polling));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD PCIeExactMatchEnable: 0x%x \n", MpioData->PCIeExactMatchEnable));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD DxioPhyValid: 0x%x \n", MpioData->DxioPhyValid));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD DxioPhyProgramming: 0x%x \n", MpioData->DxioPhyProgramming));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD CfgSkipPspMessage: 0x%x \n", MpioData->CfgSkipPspMessage));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD DxioSaveRestoreModes: 0x%x \n", MpioData->DxioSaveRestoreModes));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD AmdAllowCompliance: 0x%x \n", MpioData->AmdAllowCompliance));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD AmdHotPlugHandlingMode: 0x%x \n", MpioData->AmdHotPlugHandlingMode));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD SrisEnableMode: 0x%x \n", MpioData->SrisEnableMode));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD SrisSkipInterval: 0x%x \n", MpioData->SrisSkipInterval));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD SrisSkpIntervalSel: 0x%x \n", MpioData->SrisSkpIntervalSel));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD SrisCfgType: 0x%x \n", MpioData->SrisCfgType));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD SrisAutoDetectMode: 0x%x \n", MpioData->SrisAutoDetectMode));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD SrisAutodetectFactor: 0x%x \n", MpioData->SrisAutodetectFactor));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD SrisLowerSkpOsGenSup: 0x%x \n", MpioData->SrisLowerSkpOsGenSup));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD SrisLowerSkpOsRcvSup: 0x%x \n", MpioData->SrisLowerSkpOsRcvSup));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD AmdCxlOnAllPorts: 0x%x \n", MpioData->AmdCxlOnAllPorts));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD CxlCorrectableErrorLogging: 0x%x \n", MpioData->CxlCorrectableErrorLogging));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD CxlUnCorrectableErrorLogging: 0x%x \n", MpioData->CxlUnCorrectableErrorLogging));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD CfgAEREnable: 0x%x \n", MpioData->CfgAEREnable));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD CfgMcCapEnable: 0x%x \n", MpioData->CfgMcCapEnable));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD CfgRcvErrEnable: 0x%x \n", MpioData->CfgRcvErrEnable));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD EarlyBmcLinkTraining: 0x%x \n", MpioData->EarlyBmcLinkTraining));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD EarlyBmcLinkSocket: 0x%x \n", MpioData->EarlyBmcLinkSocket));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD EarlyBmcLinkLaneNum: 0x%x \n", MpioData->EarlyBmcLinkLaneNum));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD SurpriseDownFeature: 0x%x \n", MpioData->SurpriseDownFeature));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD LcMultAutoSpdChgOnLastRateEnable: 0x%x \n", MpioData->LcMultAutoSpdChgOnLastRateEnable));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD AmdRxMarginEnabled: 0x%x \n", MpioData->AmdRxMarginEnabled));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD CfgPcieCVTestWA: 0x%x \n", MpioData->CfgPcieCVTestWA));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD CfgPcieAriSupport: 0x%x \n", MpioData->CfgPcieAriSupport));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD CfgNbioCTOtoSC: 0x%x \n", MpioData->CfgNbioCTOtoSC));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD CfgNbioCTOIgnoreError: 0x%x \n", MpioData->CfgNbioCTOIgnoreError));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD CfgNbioSsid: 0x%x \n", MpioData->CfgNbioSsid));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD CfgIommuSsid: 0x%x \n", MpioData->CfgIommuSsid));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD CfgPspccpSsid: 0x%x \n", MpioData->CfgPspccpSsid));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD CfgNtbccpSsid: 0x%x \n", MpioData->CfgNtbccpSsid));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD CfgNbifF0Ssid: 0x%x \n", MpioData->CfgNbifF0Ssid));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD CfgNtbSsid: 0x%x \n", MpioData->CfgNtbSsid));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD AmdPcieSubsystemDeviceID: 0x%x \n", MpioData->AmdPcieSubsystemDeviceID));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD AmdPcieSubsystemVendorID: 0x%x \n", MpioData->AmdPcieSubsystemVendorID));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD GppAtomicOps: 0x%x \n", MpioData->GppAtomicOps));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD GfxAtomicOps: 0x%x \n", MpioData->GfxAtomicOps));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD AmdNbioReportEdbErrors: 0x%x \n", MpioData->AmdNbioReportEdbErrors));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD OpnSpare: 0x%x \n", MpioData->OpnSpare));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD AmdPreSilCtrl0: 0x%x \n", MpioData->AmdPreSilCtrl0));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD MPIOAncDataSupport: 0x%x \n", MpioData->MPIOAncDataSupport));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD AfterResetDelay: 0x%x \n", MpioData->AfterResetDelay));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD CfgEarlyLink: 0x%x \n", MpioData->CfgEarlyLink));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD AmdCfgExposeUnusedPciePorts: 0x%x \n", MpioData->AmdCfgExposeUnusedPciePorts));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD CfgForcePcieGenSpeed: 0x%x \n", MpioData->CfgForcePcieGenSpeed));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD CfgSataPhyTuning: 0x%x \n", MpioData->CfgSataPhyTuning));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD PcieLinkComplianceModeAllPorts: 0x%x \n", MpioData->PcieLinkComplianceModeAllPorts));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD AmdMCTPEnable: 0x%x \n", MpioData->AmdMCTPEnable));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD SbrBrokenLaneAvoidanceSup: 0x%x \n", MpioData->SbrBrokenLaneAvoidanceSup));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD AutoFullMarginSup: 0x%x \n", MpioData->AutoFullMarginSup));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD AmdPciePresetMask8GtAllPort: 0x%x \n", MpioData->AmdPciePresetMask8GtAllPort));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD AmdPciePresetMask16GtAllPort: 0x%x \n", MpioData->AmdPciePresetMask16GtAllPort));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD AmdPciePresetMask32GtAllPort: 0x%x \n", MpioData->AmdPciePresetMask32GtAllPort));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD PcieLinkAspmAllPort: 0x%x \n", MpioData->PcieLinkAspmAllPort));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD AmdMCTPMasterID: 0x%x \n", MpioData->AmdMCTPMasterID));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD SyncHeaderByPass: 0x%x \n", MpioData->SyncHeaderByPass));
  DEBUG ((DEBUG_INFO, "SIL MPIO PCD CxlTempGen5AdvertAltPtcl: 0x%x \n", MpioData->CxlTempGen5AdvertAltPtcl));
  Status = PlatformSetMpioData (MpioData);

  return Status;
}

/**
 * MpioDataBackToHostFW
 *
 * @brief Send Updated MPIO IP block's data to host FW
 * @return EFI_SUCCESS or EFI_NOT_FOUND
 *       EFI_SUCCESS   : Received valid address within the Host allocated memory block 
 *                       and succesfully update the PCD.
 *       EFI_NOT_FOUND : Indicates the requested block was not found
 */
EFI_STATUS
MpioDataBackToHostFW (
  void
  )
{
  EFI_STATUS            Status;
  MPIOCLASS_INPUT_BLK   *MpioDataHostFw;

  MpioDataHostFw = (MPIOCLASS_INPUT_BLK *)SilFindStructure (SilId_MpioClass,  0);
  DEBUG ((DEBUG_INFO, "MpioDataBackToHostFW SIL MPIO memory block at: 0x%x \n", MpioDataHostFw));
  if (MpioDataHostFw == NULL) {
    return EFI_NOT_FOUND; // Could not find the IP input block
  }

  Status = PcdSet8S(PcdAmdMCTPMasterSeg, MpioDataHostFw->AmdMCTPMasterSeg);
  DEBUG ((DEBUG_INFO, "PcdAmdMCTPMasterSeg update: %d\n", Status));
  Status = PcdSet16S(PcdAmdMCTPMasterID, MpioDataHostFw->AmdMCTPMasterID);
  DEBUG ((DEBUG_INFO, "PcdAmdMCTPMasterID update: %d\n", Status));

  return Status;
}
