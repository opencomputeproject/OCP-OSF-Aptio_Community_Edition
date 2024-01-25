/*****************************************************************************
 *
 * Copyright (C) 2018-2023 Advanced Micro Devices, Inc. All rights reserved.
 *
 *****************************************************************************/

#include <PiDxe.h>

#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/SmmServicesTableLib.h>


#include <Protocol/SpiSmmNorFlash.h>
#include <Protocol/SmmFirmwareVolumeBlock.h>

#define BLOCK_SIZE              (FixedPcdGet32 (PcdFlashNvStorageBlockSize))

extern EFI_SPI_NOR_FLASH_PROTOCOL *mSpiNorFlashProtocol;
extern EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL mSpiFvbProtocol;

extern EFI_PHYSICAL_ADDRESS mNvStorageBase;
extern EFI_LBA mNvStorageLbaOffset;

STATIC EFI_HANDLE mSpiFvbHandle;

// *** Rome Hack Start ***
VOID
TempSetmSpiHackOffset (
  VOID
  );
// *** Rome Hack End ***

/**
  EntryPoint

  @param[in] ImageHandle    Driver Image Handle
  @param[in] MmSystemTable  MM System Table

  @retval EFI_SUCCESS           Driver initialization succeeded
  @retval all others            Driver initialization failed

**/
EFI_STATUS
EFIAPI
SpiFvbSmmEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS      Status;

  DEBUG((DEBUG_INFO, "%a - ENTRY\n", __FUNCTION__));

  // Retrieve SPI NOR flash driver
  Status = gSmst->SmmLocateProtocol( &gEfiSpiSmmNorFlashProtocolGuid,
      NULL,
      (VOID **)&mSpiNorFlashProtocol);

  if (EFI_ERROR(Status)) {
    return Status;
  }

// *** Rome Hack Start ***
  // Need to set the offset based on SPI ROM part size and ROME A0
  TempSetmSpiHackOffset ();
// *** Rome Hack End ***

  mNvStorageBase = (EFI_PHYSICAL_ADDRESS)PcdGet32 (PcdFlashNvStorageVariableBase);
  DEBUG((DEBUG_INFO, "%a - mNvStorageBase = %X\n", __FUNCTION__, mNvStorageBase));
  mNvStorageLbaOffset = (EFI_LBA)((PcdGet32 (PcdFlashNvStorageVariableBase)
                                  - FixedPcdGet32 (PcdFlashAreaBaseAddress))
                                  / FixedPcdGet32 (PcdFlashNvStorageBlockSize));
  DEBUG((DEBUG_INFO, "%a - mNvStorageLbaOffset = %X\n", __FUNCTION__, mNvStorageLbaOffset));

  mSpiFvbHandle=NULL;
  Status = gSmst->SmmInstallProtocolInterface(&mSpiFvbHandle,
                &gEfiSmmFirmwareVolumeBlockProtocolGuid, EFI_NATIVE_INTERFACE,
                &mSpiFvbProtocol);

  DEBUG((DEBUG_INFO, "%a - EXIT (Status = %r)\n", __FUNCTION__, Status));
  return Status;
}
