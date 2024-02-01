#/*****************************************************************************
# *
# * Copyright (C) 2021-2024 Advanced Micro Devices, Inc. All rights reserved.
# *
# *****************************************************************************/

#include "Cf9Reset.h"
#define FILECODE UEFI_DXE_CF9RESET_CF9RESET_FILECODE

extern EFI_GUID gEfiAmdAgesaSpecificWarmResetGuid;

UINT8
ReadPmio8 (
  IN       UINT8        Index
  )
{
  UINT8 Value8;

  Value8 = 0;

  Value8 = MmioRead8 ((UINT64)(ACPI_MMIO_BASE + PMIO_BASE + (UINT64)Index));

  return Value8;
}


VOID
WritePmio8 (
  IN       UINT8        Index,
  IN       UINT8        Value
)
{
  MmioWrite8 (
    (UINT64)(ACPI_MMIO_BASE + PMIO_BASE + (UINT64)Index),
    Value);
}


UINT16
ReadPmio16 (
  IN       UINT8        Index
  )
{
  UINT16 Value16;

  Value16 = 0;

  Value16= MmioRead16 ((UINT64)(ACPI_MMIO_BASE + PMIO_BASE + (UINT64)Index));

  return Value16;
}


VOID
WritePmio16 (
  IN       UINT8        Index,
  IN       UINT16       Value
  )
{
  MmioWrite16 (
    (UINT64)(ACPI_MMIO_BASE + PMIO_BASE + (UINT64)Index),
    Value
    );
}

VOID
SpecificWarmResetSystem (
  IN EFI_RESET_TYPE ResetType
)
{
  UINT8 InitialData;
  UINT8 OutputData;
  UINT8 PwrRsrCfg;

  DEBUG ((
    DEBUG_INFO,
    "[SpecificWarmResetSystem] ResetSystem invoked:  ResetType = %d\n",
    ResetType
    ));

  InitialData = HARDSTARTSTATE;
  OutputData = HARDRESET;
  PwrRsrCfg   = 0;

  PwrRsrCfg = ReadPmio8 (0x10);
  PwrRsrCfg = PwrRsrCfg & 0xFD; //clear ToggleAllPwrGoodOnCf9
  WritePmio8 (0x10, PwrRsrCfg);

  IoWrite8 (FCH_PCIRST_BASE_IO, InitialData);
  IoWrite8 (FCH_PCIRST_BASE_IO, OutputData);
  //
  // Given we should have reset getting here would be bad
  //
  CpuDeadLoop ();
}

/**
  This function causes a system-wide reset (cold reset), in which
  all circuitry within the system returns to its initial state. This type of
  reset is asynchronous to system operation and operates without regard to
  cycle boundaries.

  If this function returns, it means that the system does not support
  cold reset.
**/
VOID
EFIAPI
ResetCold (
  VOID
  )
{
  UINT8      PwrRsrCfg;

  PwrRsrCfg = ReadPmio8 (0x10);
  PwrRsrCfg = PwrRsrCfg | BIT1; //set ToggleAllPwrGoodOnCf9
  WritePmio8 (0x10, PwrRsrCfg);
  if (PcdGetBool (PcdFchFullHardReset)) {
    IoWrite8 (FCH_PCIRST_BASE_IO, FULLSTARTSTATE);
    IoWrite8 (FCH_PCIRST_BASE_IO, FULLRESET);
  } else {
    IoWrite8 (FCH_PCIRST_BASE_IO, HARDSTARTSTATE);
    IoWrite8 (FCH_PCIRST_BASE_IO, HARDRESET);
  }
  CpuDeadLoop ();
}

/**
  This function causes a system-wide initialization (warm reset), in which all
  processors are set to their initial state. Pending cycles are not corrupted.

  If this function returns, it means that the system does not support warm reset.
**/
VOID
EFIAPI
ResetWarm (
  VOID
  )
{
  UINT8      PwrRsrCfg;

  PwrRsrCfg = 0;
  PwrRsrCfg = ReadPmio8 (0x10);
  PwrRsrCfg = PwrRsrCfg & 0xFD; //clear ToggleAllPwrGoodOnCf9
  WritePmio8 (0x10, PwrRsrCfg);
  IoWrite8 (FCH_PCIRST_BASE_IO, HARDSTARTSTATE);
  IoWrite8 (FCH_PCIRST_BASE_IO, HARDRESET);
  CpuDeadLoop ();

}

/**
  This function causes the system to enter a power state equivalent
  to the ACPI G2/S5 or G3 states.

  If this function returns, it means that the system does not support
  shutdown reset.
**/
VOID
EFIAPI
ResetShutdown (
  VOID
  )
{
  UINT16     AcpiGpeBase;
  UINT16     AcpiPm1StsBase;
  UINT16     AcpiPm1CntBase;
  UINT32     Gpe0Enable;
  UINT16     PmCntl;
  UINT16     PwrSts;

  // Disable all GPE0 Event
  AcpiGpeBase = ReadPmio16 (FCH_PMIOA_REG68);
  AcpiGpeBase += 4; //Get enable base
  Gpe0Enable  = 0;
  IoWrite32 (AcpiGpeBase, Gpe0Enable);

  // Clear Power Button status.
  AcpiPm1StsBase = ReadPmio16 (FCH_PMIOA_REG60);
  PwrSts  = BIT8 | BIT15; //Clear WakeStatus with PwrBtnStatus
  IoWrite16 (AcpiPm1StsBase, PwrSts);

  // Transform system into S5 sleep state
  AcpiPm1CntBase = ReadPmio16 (FCH_PMIOA_REG62);
  PmCntl  = IoRead16 (AcpiPm1CntBase);
  PmCntl  = (PmCntl & ~SLP_TYPE) | SUS_S5 | SLP_EN;
  IoWrite16 (AcpiPm1CntBase, PmCntl);
  CpuDeadLoop ();
}

/**
  This function causes a systemwide reset. The exact type of the reset is
  defined by the EFI_GUID that follows the Null-terminated Unicode string
  passed into ResetData. If the platform does not recognize the EFI_GUID in
  ResetData the platform must pick a supported reset type to perform. The
  platform may optionally log the parameters from any non-normal reset that
  occurs.

  @param[in]  DataSize   The size, in bytes, of ResetData.
  @param[in]  ResetData  The data buffer starts with a Null-terminated string,
                         followed by the EFI_GUID.
**/
VOID
EFIAPI
ResetPlatformSpecific (
  IN UINTN   DataSize,
  IN VOID    *ResetData
  )
{
  UINTN      ResetDataStringSize;
  EFI_GUID   *ResetTypeGuid;

  if ((DataSize >= sizeof(EFI_GUID)) && (ResetData != NULL)) {
    ResetDataStringSize = StrnSizeS (ResetData, (DataSize / sizeof(CHAR16)));
    if ((ResetDataStringSize < DataSize) && ((DataSize - ResetDataStringSize) >= sizeof(EFI_GUID))) {
      ResetTypeGuid = (EFI_GUID *)((UINT8 *)ResetData + ResetDataStringSize);
      if (CompareGuid (&gEfiAmdAgesaSpecificWarmResetGuid, ResetTypeGuid)) {
        SpecificWarmResetSystem (EfiResetPlatformSpecific);
      }
    }
  }
  IoWrite8 (FCH_PCIRST_BASE_IO, HARDSTARTSTATE);
  IoWrite8 (FCH_PCIRST_BASE_IO, HARDRESET);
  CpuDeadLoop ();
}

/**
  The ResetSystem function resets the entire platform.

  @param[in] ResetType      The type of reset to perform.
  @param[in] ResetStatus    The status code for the reset.
  @param[in] DataSize       The size, in bytes, of ResetData.
  @param[in] ResetData      For a ResetType of EfiResetCold, EfiResetWarm,
                            or EfiResetShutdown the data buffer starts with a
                            Null-terminated string, optionally followed by
                            additional binary data. The string is a description
                            that the caller may use to further indicate the
                            reason for the system reset.
**/
VOID
EFIAPI
ResetSystem (
  IN EFI_RESET_TYPE               ResetType,
  IN EFI_STATUS                   ResetStatus,
  IN UINTN                        DataSize,
  IN VOID                         *ResetData OPTIONAL
  )
{
  DEBUG ((
    DEBUG_INFO,
    "[FchCf9Reset] ResetSystem invoked:  ResetType = %d\n",
    ResetType
    ));

  switch (ResetType) {
    //
    // For update resets, the reset data is a null-terminated string followed
    // by a VOID * to the capsule descriptors. Get the pointer and set the
    // capsule variable before we do a warm reset. Per the EFI 1.10 spec, the
    // reset data is only valid if ResetStatus != EFI_SUCCESS.
    //
    case EfiResetWarm:
      ResetWarm ();
      break;

    case EfiResetCold:
      ResetCold ();
      break;

    case EfiResetPlatformSpecific:
      ResetPlatformSpecific (DataSize, ResetData);
      break;

    case EfiResetShutdown:
      ResetShutdown ();
      return;

    default:
      return;
  }

  //
  // Given we should have reset getting here would be bad
  //
  CpuDeadLoop ();
}
