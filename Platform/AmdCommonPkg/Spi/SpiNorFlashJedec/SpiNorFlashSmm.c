/*****************************************************************************
 *
 * Copyright (C) 2018-2023 Advanced Micro Devices, Inc. All rights reserved.
 *
 *****************************************************************************/

#include <Base.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Protocol/SpiSmmConfiguration.h>
#include <Protocol/SpiSmmNorFlash.h>
#include <Protocol/SpiIo.h>
#include <Protocol/SpiIoAdditional.h>
#include <Spi/SpiNorFlashJedec.h>
#include "SpiNorFlash.h"
#include "SpiNorFlashInstance.h"

/**
  Entry point of the Macronix SPI NOR Flash driver.

  @param ImageHandle  Image handle of this driver.
  @param SystemTable  Pointer to standard EFI system table.

  @retval EFI_SUCCESS       Succeed.
  @retval EFI_DEVICE_ERROR  Fail to install EFI_SPI_SMM_NOR_FLASH_PROTOCOL.
**/
EFI_STATUS
EFIAPI
SpiNorFlashEntry (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                          Status;
  SPI_NOR_FLASH_INSTANCE              *Instance;
  EFI_SPI_NOR_FLASH_PROTOCOL          *Protocol;

  DEBUG((DEBUG_INFO, "%a - ENTRY\n", __FUNCTION__));

  // Allocate the Board SPI Configuration Instance
  Instance = AllocateZeroPool (sizeof (SPI_NOR_FLASH_INSTANCE));
  ASSERT (Instance != NULL);
  if (Instance == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  Instance->Signature = SPI_NOR_FLASH_SIGNATURE;

  // Locate the SPI IO Protocol
  Status = gSmst->SmmLocateProtocol (
                    &gAmdJedecSpiSmmIoProtocolGuid,
                    NULL,
                    (VOID **)&Instance->SpiIo
                    );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status) ||
      (Instance->SpiIo->Attributes & (SPI_IO_TRANSFER_SIZE_INCLUDES_ADDRESS |
                                      SPI_IO_TRANSFER_SIZE_INCLUDES_OPCODE)) != 0) {
    FreePool (Instance);
    Status = EFI_UNSUPPORTED;
  } else {
    // Allocate write buffer for SPI IO transactions with extra room for Opcode
    // and Address
    Instance->SpiTransactionWriteBuffer = AllocatePool (
        Instance->SpiIo->MaximumTransferBytes + 10 // Add extra room
        );
    Protocol = &Instance->Protocol;
    Protocol->SpiPeripheral = Instance->SpiIo->SpiPeripheral;
    Protocol->GetFlashid = GetFlashId;
    Protocol->ReadData = ReadData;
    Protocol->LfReadData = LfReadData;
    Protocol->ReadStatus = ReadStatus;
    Protocol->WriteStatus = WriteStatus;
    Protocol->WriteData = WriteData;
    Protocol->Erase = Erase;
    Protocol->EraseBlockBytes = SIZE_4KB;
    Status = Protocol->GetFlashid (Protocol,
        (UINT8 *)&Protocol->Deviceid);
    ASSERT_EFI_ERROR (Status);
    DEBUG((DEBUG_INFO, "%a: Flash ID: Manufacturer=0x%02X, Device=0x%02X%02X\n",
          __FUNCTION__,
          Protocol->Deviceid[0],
          Protocol->Deviceid[1],
          Protocol->Deviceid[2]));

    Status = ReadSfdpBasicParmeterTable (Instance);
    ASSERT_EFI_ERROR (Status);

    // SFDP DWORD 2
    Protocol->FlashSize = (Instance->SfdpBasicFlash->Density + 1) / 8;
    DEBUG((DEBUG_INFO, "%a: Flash Size=0x%X\n", __FUNCTION__, Protocol->FlashSize));

    Status = gSmst->SmmInstallProtocolInterface(
                      &Instance->Handle,
                      &gEfiSpiSmmNorFlashProtocolGuid,
                      EFI_NATIVE_INTERFACE,
                      &Instance->Protocol
                      );
  }

  DEBUG((DEBUG_INFO, "%a: EXIT - Status=%r\n", __FUNCTION__, Status));

  return Status;
}
