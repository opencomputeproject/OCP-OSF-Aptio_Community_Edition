/* Copyright 2021-2024 Advanced Micro Devices, Inc. All rights reserved. */
/**
 * @file  NbioDataInit.c
 * @brief Initialize NBIO data prior to openSIL execution.
 *
 */
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <FCH/Common/FchCommonCfg.h>
#include "string.h"
#define FEATURE_CPPC_MASK                    (1 << 22)
#define FCH_SATA_CONTROLLER_NUM          4

/**
 * SetFchHwAcpiData
 * @brief Establish FCH HwAcpi input
 *
 * @retval SIL_STATUS
 */
EFI_STATUS
SetFchHwAcpiData (
  void
  )
{
  FCHHWACPI_INPUT_BLK *FchHwAcpiData;

  FchHwAcpiData = (FCHHWACPI_INPUT_BLK *)SilFindStructure (SilId_FchHwAcpiP,  0);
  DEBUG ((DEBUG_ERROR, "SIL Fch HwAcpi memory block is found at: 0x%x \n", FchHwAcpiData));
  if (FchHwAcpiData == NULL) {
    return EFI_NOT_FOUND; // Could not find the IP input block
  }

  FchHwAcpiData->I2c0SdaHold           = PcdGet32 (PcdAmdFchI2c0SdaHold);
  FchHwAcpiData->I2c1SdaHold           = PcdGet32 (PcdAmdFchI2c1SdaHold);
  FchHwAcpiData->I2c2SdaHold           = PcdGet32 (PcdAmdFchI2c2SdaHold);
  FchHwAcpiData->I2c3SdaHold           = PcdGet32 (PcdAmdFchI2c3SdaHold);
  FchHwAcpiData->I2c4SdaHold           = PcdGet32 (PcdAmdFchI2c4SdaHold);
  FchHwAcpiData->I2c5SdaHold           = PcdGet32 (PcdAmdFchI2c5SdaHold);
  FchHwAcpiData->SpreadSpectrum        = PcdGetBool (PcdSpreadSpectrum);
  FchHwAcpiData->PwrFailShadow         = PcdGet8 (PcdPwrFailShadow);
  FchHwAcpiData->StressResetMode       = PcdGet8 (PcdStressResetMode);
  FchHwAcpiData->NoClearThermalTripSts = PcdGetBool (PcdNoClearThermalTripSts);
  FchHwAcpiData->FchAlinkRasSupport    = PcdGetBool (PcdAmdFchAlinkRasSupport);
  FchHwAcpiData->SpdHostCtrlRelease    = PcdGetBool (PcdAmdFchSpdHostCtrlRelease);
  FchHwAcpiData->BootTimerEnable       = PcdGetBool (PcdBootTimerEnable);
  FchHwAcpiData->BootTimerResetType    = PcdGet8 (PcdBootTimerResetType);
  FchHwAcpiData->ToggleAllPwrGoodOnCf9 = PcdGetBool (PcdToggleAllPwrGoodOnCf9);
  FchHwAcpiData->OemProgrammingTablePtr = PcdGetPtr (PcdOemProgrammingTablePtr);//???

  return EFI_SUCCESS;
}

/**
 * SetFchUsbData
 * @brief Establish FCH HwAcpi input
 *
 * @retval SIL_STATUS
 */
EFI_STATUS
SetFchUsbData (
  void
  )
{
  FCHUSB_INPUT_BLK *FchUsbData;

  FchUsbData = (FCHUSB_INPUT_BLK *)SilFindStructure (SilId_FchUsb,  0);
  DEBUG ((DEBUG_ERROR, "SIL Fch USB memory block is found at: 0x%x \n", FchUsbData));
  if (FchUsbData == NULL) {
    return EFI_NOT_FOUND; // Could not find the IP input block
  }

  FchUsbData->Xhci0Enable                    = PcdGetBool (PcdXhci0Enable);
  FchUsbData->Xhci1Enable                    = PcdGetBool (PcdXhci1Enable);
  FchUsbData->XhciSsid                       = PcdGet32 (PcdXhciSsid);
  FchUsbData->Xhci0DevRemovable              = PcdGet32 (PcdXhci0DevRemovable);
  FchUsbData->DisableXhciPortLate            = PcdGetBool (PcdDisableXhciPortLate);
  FchUsbData->XhciUsb3PortDisable            = PcdGet32 (PcdXhciUsb3PortDisable);
  FchUsbData->XhciUsb2PortDisable            = PcdGet32 (PcdXhciUsb2PortDisable);
  FchUsbData->XhciOCpinSelect[0].Usb20OcPin  = (UINT32)PcdGet64 (PcdXhciUsb20OcPinSelect);
  FchUsbData->XhciOCpinSelect[0].Usb31OcPin  = (UINT16)PcdGet32 (PcdXhciUsb31OcPinSelect);
  FchUsbData->XhciOCpinSelect[1].Usb20OcPin  = (UINT32)RShiftU64 (PcdGet64 (PcdXhciUsb20OcPinSelect), 8);
  FchUsbData->XhciOCpinSelect[1].Usb31OcPin  = (UINT16)(PcdGet32 (PcdXhciUsb31OcPinSelect) >> 8);
  FchUsbData->XhciOcPolarityCfgLow           = PcdGetBool (PcdXhciOcPolarityCfgLow);
  FchUsbData->Usb3PortForceGen1              = PcdGet8 (PcdXhciForceGen1);
  FchUsbData->UsbSparseModeEnable            = PcdGetBool (PcdUsbSparseModeEnable);

  memcpy (&(FchUsbData->OemUsbConfigurationTable),
    PcdGetPtr (PcdUsbGenoaOemConfigurationTable),
    sizeof (FCH_USB_OEM_PLATFORM_TABLE));

  return EFI_SUCCESS;
}

/**
 * SetFchAbData
 * @brief Establish FCH AB input
 *
 * @retval SIL_STATUS
 */
EFI_STATUS
SetFchAbData (
  void
  )
{
  FCHAB_INPUT_BLK  *FchAbData;

  FchAbData = (FCHAB_INPUT_BLK *)SilFindStructure (SilId_FchAb,  0);
  DEBUG ((DEBUG_ERROR, "SIL Fch AB memory block is found at: 0x%x \n", FchAbData));
  if (FchAbData == NULL) {
    return EFI_NOT_FOUND; // Could not find the IP input block
  }

  FchAbData->AbClockGating                = PcdGet8 (PcdAbClockGating);
  FchAbData->ALinkClkGateOff              = PcdGet8 (PcdALinkClkGateOff);
  FchAbData->BLinkClkGateOff              = PcdGet8 (PcdBLinkClkGateOff);
  FchAbData->SbgMemoryPowerSaving         = PcdGetBool (PcdSbgMemoryPowerSaving);
  FchAbData->SbgClockGating               = PcdGetBool (PcdSbgClockGating);
  FchAbData->XdmaDmaWrite16ByteMode       = PcdGetBool (PcdXdmaDmaWrite16ByteMode);
  FchAbData->XdmaMemoryPowerSaving        = PcdGetBool (PcdXdmaMemoryPowerSaving);
  FchAbData->XdmaPendingNprThreshold      = PcdGet8 (PcdXdmaPendingNprThreshold);
  FchAbData->XdmaDncplOrderDis            = PcdGetBool (PcdXdmaDncplOrderDis);
  FchAbData->SdphostBypassDataPack        = PcdGetBool (PcdSdphostBypassDataPack);
  FchAbData->SdphostDisNpmwrProtect       = PcdGetBool (PcdSdphostDisNpmwrProtect);
  if (PcdGetBool (PcdResetCpuOnSyncFlood) && (!PcdGetBool (PcdSyncFloodToApml))) {
    FchAbData->ResetCpuOnSyncFlood        = TRUE;
  } else {
    FchAbData->ResetCpuOnSyncFlood        = FALSE;
  }

  return EFI_SUCCESS;
}

/**
 * SetFchSataData
 * @brief Establish FCH Sata input
 *
 * @retval SIL_STATUS
 */
EFI_STATUS
SetFchSataData (
  void
  )
{
  uint8_t  SataController;
  uint8_t  SataEnable2;
  uint8_t  SataSgpioEnable;
  uint64_t SataRxPolarity;
  uint64_t SataEspEnable;
  uint64_t SataPortShutdown;
  uint64_t SataPortMode;
  FCHSATA_INPUT_BLK  *FchSataData;

  FchSataData = (FCHSATA_INPUT_BLK *)SilFindStructure (SilId_FchSata,  0);
  DEBUG ((DEBUG_ERROR, "SIL Fch SATA memory block is found at: 0x%x \n", FchSataData));
  if (FchSataData == NULL) {
    return EFI_NOT_FOUND; // Could not find the IP input block
  }

  if (PcdGetBool (PcdSataEnable)) {
    SataEnable2      = PcdGet8 (PcdSataEnable2);
    SataRxPolarity   = PcdGet64 (PcdSataMultiDiePortRxPolarity);
    SataEspEnable    = PcdGet64 (PcdSataMultiDiePortESP);
    SataPortShutdown = PcdGet64 (PcdSataMultiDiePortShutDown);
    SataPortMode     = PcdGet64 (PcdSataIoDie0PortMode);
    SataSgpioEnable  = PcdGet8 (PcdSataSgpioMultiDieEnable);

    for (SataController = 0; SataController < FCH_SATA_CONTROLLER_NUM; SataController++) {
      if (SataEnable2 & (1 << SataController)) {
        FchSataData[SataController].SataEnable = true;
      } else {
        FchSataData[SataController].SataEnable = false;
      }
    }

    //Sata controller
    for (SataController = 0; SataController < FCH_SATA_CONTROLLER_NUM;
      SataController++) {
      if (FchSataData[SataController].SataEnable) {
        FchSataData[SataController].SataSetMaxGen2              = PcdGetBool (PcdSataSetMaxGen2);
        FchSataData[SataController].SataClass                   = PcdGet8 (PcdSataClass);
        FchSataData[SataController].SataAggrLinkPmCap           = PcdGet8 (PcdSataAggrLinkPmCap);
        FchSataData[SataController].SataPortMultCap             = PcdGet8 (PcdSataPortMultCap);
        FchSataData[SataController].SataPscCap                  = PcdGet8 (PcdSataPscCap);
        FchSataData[SataController].SataSscCap                  = PcdGet8 (PcdSataSscCap);
        FchSataData[SataController].SataClkAutoOff              = PcdGet8 (PcdSataClkAutoOff);
        FchSataData[SataController].SataFisBasedSwitching       = PcdGet8 (PcdSataFisBasedSwitching);
        FchSataData[SataController].SataCccSupport              = PcdGet8 (PcdSataCccSupport);
        FchSataData[SataController].SataDisableGenericMode      = PcdGet8 (PcdSataDisableGenericMode);
        FchSataData[SataController].SataTargetSupport8Device    = PcdGet8 (PcdSataTargetSupport8Device);
        FchSataData[SataController].SataAhciEnclosureManagement = PcdGet8 (PcdSataAhciEnclosureManagement);
        FchSataData[SataController].SataMsiEnable               = PcdGetBool (PcdSataMsiEnable);
        FchSataData[SataController].SataRasSupport              = PcdGetBool (PcdSataRasSupport);
        FchSataData[SataController].SataAhciDisPrefetchFunction = PcdGetBool (PcdSataAhciDisPrefetchFunction);
        FchSataData[SataController].SataAhciSsid                = PcdGet32 (PcdSataAhciSsid);
        FchSataData[SataController].SataRaid5Ssid               = PcdGet32 (PcdSataRaid5Ssid);
        FchSataData[SataController].SataRaidSsid                = PcdGet32 (PcdSataRaidSsid);
        FchSataData[SataController].SataDevSlpPort0             = PcdGetBool (PcdSataDevSlpPort0);
        FchSataData[SataController].SataDevSlpPort1             = PcdGetBool (PcdSataDevSlpPort1);
        FchSataData[SataController].SataDevSlpPort0Num          = PcdGet8 (PcdSataDevSlpPort0Num);
        FchSataData[SataController].SataDevSlpPort1Num          = PcdGet8 (PcdSataDevSlpPort1Num);
        FchSataData[SataController].SataSgpio0                  = PcdGet8 (PcdSataSgpio0);
        FchSataData[SataController].SataControllerAutoShutdown  = PcdGetBool (PcdSataControllerAutoShutdown);

        FchSataData[SataController].SataEspPort    = (uint8_t)(SataEspEnable >> (8 * SataController));
        FchSataData[SataController].SataPortPower  = (uint8_t)(SataPortShutdown >> (8 * SataController));
        FchSataData[SataController].SataPortMd     = (uint16_t)(SataPortMode >> (16 * SataController));
        FchSataData[SataController].SataSgpio0     = (uint8_t)((SataSgpioEnable >> SataController) & BIT_32(0));
        FchSataData[SataController].SataRxPolarity = (uint8_t) (SataRxPolarity >> (8 * SataController));
        FchSataData[SataController].SataBISTLComplianceMode = PcdGet8 (PcdSataBISTLComplianceMode);
      }
    }
  } else {
    PcdSet8S (PcdSataEnable2, 0);
  }

  return EFI_SUCCESS;
}

/**
 * SetFchSpiData
 * @brief Establish FCH SPI input
 *
 * @retval SIL_STATUS
 */
EFI_STATUS
SetFchSpiData (
  void
  )
{
  FCHSPI_INPUT_BLK  *FchSpiData;

  FchSpiData = (FCHSPI_INPUT_BLK *)SilFindStructure (SilId_FchSpi,  0);
  DEBUG ((DEBUG_ERROR, "SIL Fch SPI memory block is found at: 0x%x \n", FchSpiData));
  if (FchSpiData == NULL) {
    return EFI_NOT_FOUND; // Could not find the IP input block
  }

  FchSpiData->LpcEnable                  = PcdGetBool (PcdLpcEnable);
  FchSpiData->LpcClk0                    = PcdGetBool (PcdLpcClk0);
  FchSpiData->LpcClk1                    = PcdGetBool (PcdLpcClk1);
  FchSpiData->LpcSsid                    = PcdGet32 (PcdLpcSsid);
  FchSpiData->SpiSpeed                   = PcdGet8 (PcdResetSpiSpeed);
  FchSpiData->WriteSpeed                 = PcdGet8 (PcdResetWriteSpeed);
  FchSpiData->SpiTpmSpeed                = PcdGet8 (PcdResetSpiTpmSpeed);

  return EFI_SUCCESS;
}

/**
 * SetFchData
 * @brief Establish FCH input
 *
 * @retval SIL_STATUS
 */
EFI_STATUS
SetFchData (
  void
  )
{
  uint8_t     UartLegacy[4];
  uint16_t    IoEnable16;
  uint16_t    UartChannel;
  FCHCLASS_INPUT_BLK *FchData;
  EFI_STATUS Status;

  FchData = (FCHCLASS_INPUT_BLK *)SilFindStructure (SilId_FchClass,  0);
  DEBUG ((DEBUG_ERROR, "SIL Fch Class memory block is found at: 0x%x \n", FchData));
  if (FchData == NULL) {
    return EFI_NOT_FOUND; // Could not find the IP input block
  }

  FchData->FchBldCfg.CfgSmbus0BaseAddress = PcdGet16 (PcdAmdFchCfgSmbus0BaseAddress);
  FchData->FchBldCfg.CfgSioPmeBaseAddress = PcdGet16 (PcdAmdFchCfgSioPmeBaseAddress);
  FchData->FchBldCfg.CfgAcpiPm1EvtBlkAddr = PcdGet16 (PcdAmdFchCfgAcpiPm1EvtBlkAddr);
  FchData->FchBldCfg.CfgAcpiPm1CntBlkAddr = PcdGet16 (PcdAmdFchCfgAcpiPm1CntBlkAddr);
  FchData->FchBldCfg.CfgAcpiPmTmrBlkAddr  = PcdGet16 (PcdAmdFchCfgAcpiPmTmrBlkAddr);
  FchData->FchBldCfg.CfgCpuControlBlkAddr = PcdGet16 (PcdAmdFchCfgCpuControlBlkAddr);
  FchData->FchBldCfg.CfgAcpiGpe0BlkAddr   = PcdGet16 (PcdAmdFchCfgAcpiGpe0BlkAddr);
  FchData->FchBldCfg.CfgSmiCmdPortAddr    = PcdGet16 (PcdAmdFchCfgSmiCmdPortAddr);

  FchData->LegacyFree                    = PcdGetBool (PcdLegacyFree);
  FchData->FchOscout1ClkContinous        = PcdGetBool (PcdFchOscout1ClkContinous);
  FchData->LpcClockDriveStrength         = PcdGet8 (PcdLpcClockDriveStrength);
  FchData->LpcClockDriveStrengthRiseTime = PcdGet8 (PcdLpcClockDriveStrengthRiseTime);
  FchData->LpcClockDriveStrengthFallTime = PcdGet8 (PcdLpcClockDriveStrengthFallTime);
  FchData->WdtEnable                     = PcdGetBool (PcdFchWdtEnable);
  FchData->SerialIrqEnable               = PcdGetBool (PcdSerialIrqEnable);
  FchData->CfgIoApicIdPreDefEnable       = PcdGetBool (PcdCfgIoApicIdPreDefineEn);
  FchData->FchIoApicId                   = PcdGet8 (PcdCfgFchIoapicId);
  FchData->FchAsfCfg.DisableSecondary        = PcdGetBool (PcdAmdFchDisableAsfSecondary);

  FchData->Hpet.HpetEnable               = PcdGetBool (PcdHpetEnable);
  FchData->Hpet.HpetMsiDis               = PcdGetBool (PcdHpetMsiDis);
  FchData->Hpet.HpetBase                 = 0xFED00000;

  FchData->Gcpu.TimerTickTrack           = PcdGet8 (PcdTimerTickTrack);
  FchData->Gcpu.ClockInterruptTag        = PcdGet8 (PcdClockInterruptTag);

  FchData->Misc.NoneSioKbcSupport        = PcdGetBool (PcdNoneSioKbcSupport);
  FchData->Misc.NativePcieSupport        = PcdGetBool (PcdNativePcieSupport);
  FchData->Misc.Cppc.CppcSupport         = FALSE;



  FchData->FchRunTime.FchDeviceEnableMap = PcdGet32 (FchRTDeviceEnableMap);
  FchData->FchRunTime.PcieMmioBase       = (UINT32) PcdGet64 (PcdPciExpressBaseAddress);
  if (FchData->FchRunTime.FchDeviceEnableMap & BIT_32(11)) {
    UartLegacy[0] = PcdGet8 (FchUart0LegacyEnable);;
  } else {
    UartLegacy[0] = 0;
  }
  if (FchData->FchRunTime.FchDeviceEnableMap & BIT_32(12)) {
    UartLegacy[1] = PcdGet8 (FchUart1LegacyEnable);;
  } else {
    UartLegacy[1] = 0;
  }
  if (FchData->FchRunTime.FchDeviceEnableMap & BIT_32(16)) {
    UartLegacy[2] = PcdGet8 (FchUart2LegacyEnable);;
  } else {
    UartLegacy[2] = 0;
  }
  if (FchData->FchRunTime.FchDeviceEnableMap & BIT_32(26)) {
    UartLegacy[3] = PcdGet8 (FchUart3LegacyEnable);;
  } else {
    UartLegacy[3] = 0;
  }

  IoEnable16 = 0;
  for (UartChannel = 0; UartChannel < 4; UartChannel++ ) {
    if (UartLegacy[UartChannel]) {
      IoEnable16 |= (BIT_32(0) << (UartLegacy[UartChannel] - 1)) +
        (UartChannel << (8 + ((UartLegacy[UartChannel] - 1) * 2)));
    }
  }

  FchData->FchRunTime.Al2AhbLegacyUartIoEnable = IoEnable16;

  FchData->Smbus.SmbusSsid               = PcdGet32 (PcdSmbusSsid);

  FchData->FchReset.SataEnable           = PcdGetBool (PcdSataEnable);
  FchData->SataSetMaxGen2                = PcdGetBool (PcdSataSetMaxGen2);
  FchData->SataClkMode                   = PcdGet8 (PcdSataClkMode);

  Status = SetFchHwAcpiData ();
  ASSERT_EFI_ERROR (Status);

  Status = SetFchUsbData ();
  ASSERT_EFI_ERROR (Status);

  Status = SetFchAbData ();
  ASSERT_EFI_ERROR (Status);

  Status = SetFchSataData ();
  ASSERT_EFI_ERROR (Status);

  Status = SetFchSpiData ();
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}