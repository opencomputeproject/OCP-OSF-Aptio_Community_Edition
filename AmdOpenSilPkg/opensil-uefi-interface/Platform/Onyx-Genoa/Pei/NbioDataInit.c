/**
 * @file  NbioDataInit.c
 * @brief Initialize NBIO data prior to openSIL execution.
 *
 */
/**
 * Copyright 2021-2023 Advanced Micro Devices, Inc. All rights reserved.
 *
 */
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Sil-api.h>
#include <NBIO/NbioClass-api.h>

/**
 * NbioSetInputBlk
 * @brief Establish NBIO input defaults
 *
 * @retval SIL_STATUS
 */
EFI_STATUS
SetNbioData (
  VOID
  )
{
  NBIOCLASS_DATA_BLOCK *NbioData;

  NbioData = (NBIOCLASS_DATA_BLOCK *)SilFindStructure (SilId_NbioClass,  0);
  DEBUG ((DEBUG_INFO, "SIL NBIO memory block is found at: 0x%x \n", NbioData));
  if (NbioData == NULL) {
    return EFI_NOT_FOUND; // Could not find the IP input block
  }

  // Initialize NBIO input block with host configurations
  NbioData->NbioInputBlk.CfgHdAudioEnable           = PcdGetBool (PcdCfgHdAudioEnable);
  NbioData->NbioInputBlk.EsmEnableAllRootPorts      = PcdGetBool (PcdEsmEnableAllRootPorts);
  NbioData->NbioInputBlk.EsmTargetSpeed             = PcdGet8 (PcdEsmTargetSpeed);
  NbioData->NbioInputBlk.CfgRxMarginPersistenceMode = PcdGet8 (PcdCfgRxMarginPersistenceMode);
  NbioData->NbioInputBlk.CfgDxioFrequencyVetting    = PcdGetBool (PcdCfgDxioFrequencyVetting);
  NbioData->NbioInputBlk.CfgSkipPspMessage          = PcdGet8 (PcdCfgSkipPspMessage);
  NbioData->NbioInputBlk.CfgEarlyTrainTwoPcieLinks  = PcdGetBool (PcdCfgEarlyTrainTwoPcieLinks);
  NbioData->NbioInputBlk.EarlyBmcLinkTraining       = PcdGetBool (PcdEarlyBmcLinkTraining);
  NbioData->NbioInputBlk.EarlyBmcLinkSocket         = PcdGet8 (PcdEarlyBmcLinkSocket);
  NbioData->NbioInputBlk.EarlyBmcLinkLaneNum        = PcdGet8 (PcdEarlyBmcLinkLaneNum);
  NbioData->NbioInputBlk.EarlyBmcLinkDie            = PcdGet8 (PcdEarlyBmcLinkDie);
  NbioData->NbioInputBlk.EdpcEnable                 = PcdGet8 (PcdAmdEdpcEnable);
  NbioData->NbioInputBlk.PcieAerReportMechanism     = PcdGet8 (PcdAmdPcieAerReportMechanism);
  NbioData->NbioInputBlk.SevSnpSupport              = PcdGetBool (PcdCfgSevSnpSupport);

  /// @todo : NBIO_CONFIG_DATA - Need to scrub these.  Some may not be used for Genoa
  // Initiailize NBIO_CONFIG_DATA with host configurations
  NbioData->NbioConfigData.IOHCClkGatingSupport = PcdGetBool (PcdIOHCClkGatingSupport);
  NbioData->NbioConfigData.CfgNbifMgcgClkGating = PcdGetBool (PcdCfgNbifMgcgClkGating);
  NbioData->NbioConfigData.CfgSstunlClkGating = PcdGetBool (PcdCfgSstunlClkGating);
  NbioData->NbioConfigData.CfgSyshubMgcgClkGating = PcdGetBool (PcdCfgSyshubMgcgClkGating);
  NbioData->NbioConfigData.TPHCompleterEnable = PcdGet8 (PcdTPHCompleterEnable);
  NbioData->NbioConfigData.IoApicMMIOAddressReservedEnable = PcdGetBool (PcdCfgIoApicMMIOAddressReservedEnable);
  NbioData->NbioConfigData.IoApicIdPreDefineEn = PcdGetBool (PcdCfgIoApicIdPreDefineEn);
  NbioData->NbioConfigData.IoApicIdBase = PcdGet8 (PcdCfgIoApicIdBase);
  NbioData->NbioConfigData.NbifMgcgHysteresis = PcdGet8 (PcdNbifMgcgHysteresis);
  NbioData->NbioConfigData.SyshubMgcgHysteresis = PcdGet8 (PcdSyshubMgcgHysteresis);
  NbioData->NbioConfigData.IohcNonPCIBarInitDbg = PcdGetBool (PcdCfgIohcNonPCIBarInitDbg);
  NbioData->NbioConfigData.IohcNonPCIBarInitFastReg = PcdGetBool (PcdCfgIohcNonPCIBarInitFastReg);
  NbioData->NbioConfigData.IohcNonPCIBarInitFastRegCtl = PcdGetBool (PcdCfgIohcNonPCIBarInitFastRegCtl);
  NbioData->NbioConfigData.IommuMMIOAddressReservedEnable = PcdGetBool (PcdCfgIommuMMIOAddressReservedEnable);
  NbioData->NbioConfigData.AmdApicMode = PcdGet8 (PcdAmdApicMode);
  NbioData->NbioConfigData.IommuAvicSupport = PcdGetBool (PcdCfgIommuAvicSupport);
  NbioData->NbioConfigData.IommuL2ClockGatingEnable = PcdGetBool (PcdIommuL2ClockGatingEnable);
  NbioData->NbioConfigData.IommuL1ClockGatingEnable = PcdGetBool (PcdIommuL1ClockGatingEnable);
  NbioData->NbioConfigData.IOHCPgEnable = PcdGetBool (PcdIOHCPgEnable);
  NbioData->NbioConfigData.NbioGlobalCgOverride = PcdGet8 (PcdNbioGlobalCgOverride);
  NbioData->NbioConfigData.IommuSupport = PcdGetBool (PcdCfgIommuSupport);
  NbioData->NbioConfigData.CfgACSEnable = PcdGetBool (PcdCfgACSEnable);
  NbioData->NbioConfigData.CfgPCIeLTREnable = PcdGetBool (PcdCfgPCIeLTREnable);
  NbioData->NbioConfigData.CfgPcieAriSupport = PcdGetBool (PcdCfgPcieAriSupport);
  NbioData->NbioConfigData.AmdMaskDpcCapability = PcdGetBool (PcdAmdMaskDpcCapability);
  NbioData->NbioConfigData.PcieEcrcEnablement = PcdGetBool (PcdPcieEcrcEnablement);
  NbioData->NbioConfigData.CfgAutoSpeedChangeEnable = PcdGet8 (PcdCfgAutoSpeedChangeEnable);
  NbioData->NbioConfigData.EsmEnableAllRootPorts = PcdGetBool (PcdEsmEnableAllRootPorts);
  NbioData->NbioConfigData.EsmTargetSpeed = PcdGet8 (PcdEsmTargetSpeed);
  NbioData->NbioConfigData.CfgRxMarginPersistenceMode = PcdGet8 (PcdCfgRxMarginPersistenceMode);
  NbioData->NbioConfigData.CfgSriovEnDev0F1 = PcdGetBool (PcdCfgSriovEnDev0F1);
  NbioData->NbioConfigData.CfgAriEnDev0F1 = PcdGetBool (PcdCfgAriEnDev0F1);
  NbioData->NbioConfigData.CfgAerEnDev0F1 = PcdGetBool (PcdCfgAerEnDev0F1);
  NbioData->NbioConfigData.CfgAcsEnDev0F1 = PcdGetBool (PcdCfgAcsEnDev0F1);
  NbioData->NbioConfigData.CfgAtsEnDev0F1 = PcdGetBool (PcdCfgAtsEnDev0F1);
  NbioData->NbioConfigData.CfgPasidEnDev0F1 = PcdGetBool (PcdCfgPasidEnDev0F1);
  NbioData->NbioConfigData.CfgPwrEnDev0F1 = PcdGetBool (PcdCfgPwrEnDev0F1);
  NbioData->NbioConfigData.CfgRtrEnDev0F1 = PcdGetBool (PcdCfgRtrEnDev0F1);
  NbioData->NbioConfigData.CfgPriEnDev0F1 = PcdGetBool (PcdCfgPriEnDev0F1);
  NbioData->NbioConfigData.AtcEnable = PcdGetBool (PcdAtcEnable);
  NbioData->NbioConfigData.AcsEnRccDev0 = PcdGetBool (PcdAcsEnRccDev0);
  NbioData->NbioConfigData.AerEnRccDev0 = PcdGetBool (PcdAerEnRccDev0);
  NbioData->NbioConfigData.AcsSourceValStrap5 = PcdGetBool (PcdAcsSourceValStrap5);
  NbioData->NbioConfigData.AcsTranslationalBlockingStrap5 = PcdGetBool (PcdAcsTranslationalBlockingStrap5);
  NbioData->NbioConfigData.AcsP2pReqStrap5 = PcdGetBool (PcdAcsP2pReqStrap5);
  NbioData->NbioConfigData.AcsP2pCompStrap5 = PcdGetBool (PcdAcsP2pCompStrap5);
  NbioData->NbioConfigData.AcsUpstreamFwdStrap5 = PcdGetBool (PcdAcsUpstreamFwdStrap5);
  NbioData->NbioConfigData.AcsP2PEgressStrap5 = PcdGetBool (PcdAcsP2PEgressStrap5);
  NbioData->NbioConfigData.AcsDirectTranslatedStrap5 = PcdGetBool (PcdAcsDirectTranslatedStrap5);
  NbioData->NbioConfigData.AcsSsidEnStrap5 = PcdGetBool (PcdAcsSsidEnStrap5);
  NbioData->NbioConfigData.DlfEnStrap1 = PcdGetBool (PcdDlfEnStrap1);
  NbioData->NbioConfigData.Phy16gtStrap1 = PcdGetBool (PcdPhy16gtStrap1);
  NbioData->NbioConfigData.MarginEnStrap1 = PcdGetBool (PcdMarginEnStrap1);
  NbioData->NbioConfigData.PriEnPageReq = PcdGetBool (PcdPriEnPageReq);
  NbioData->NbioConfigData.PriResetPageReq = PcdGetBool (PcdPriResetPageReq);
  NbioData->NbioConfigData.AcsSourceVal = PcdGetBool (PcdAcsSourceVal);
  NbioData->NbioConfigData.AcsTranslationalBlocking = PcdGetBool (PcdAcsTranslationalBlocking);
  NbioData->NbioConfigData.AcsP2pReq = PcdGetBool (PcdAcsP2pReq);
  NbioData->NbioConfigData.AcsP2pComp = PcdGetBool (PcdAcsP2pComp);
  NbioData->NbioConfigData.AcsUpstreamFwd = PcdGetBool (PcdAcsUpstreamFwd);
  NbioData->NbioConfigData.AcsP2PEgress = PcdGetBool (PcdAcsP2PEgress);
  NbioData->NbioConfigData.RccDev0E2EPrefix = PcdGetBool (PcdRccDev0E2EPrefix);
  NbioData->NbioConfigData.RccDev0ExtendedFmtSupported = PcdGetBool (PcdRccDev0ExtendedFmtSupported);
  NbioData->NbioConfigData.DlfCapEn = PcdGetBool (PcdAmdDlfCapEn);
  NbioData->NbioConfigData.DlfExEn = PcdGetBool (PcdAmdDlfExEn);
  NbioData->NbioConfigData.PrecodeRequestEnable = PcdGetBool (PcdCfgPrecodeRequestEnable);
  NbioData->NbioConfigData.PcieSpeedControl = PcdGet8 (PcdAmdPcieSpeedControl);
  NbioData->NbioConfigData.AdvertiseEqToHighRateSupport = PcdGetBool (PcdAmdAdvertiseEqToHighRateSupport);
  NbioData->NbioConfigData.FabricSdci = PcdGetBool (PcdAmdFabricSdci);

  return EFI_SUCCESS;
}
