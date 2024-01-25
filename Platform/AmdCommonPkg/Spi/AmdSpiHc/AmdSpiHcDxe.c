/*****************************************************************************
 *
 * Copyright (C) 2018-2023 Advanced Micro Devices, Inc. All rights reserved.
 *
 *****************************************************************************/

#include <Base.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/PciLib.h>
#include <Protocol/DxeMmReadyToLock.h>
#include <Protocol/SpiHc.h>
#include <Protocol/SpiHcAdditional.h>
#include <Spi/SpiNorFlashJedec.h>
#include "AmdSpiHc.h"
#include "AmdSpiHcNull.h"
#include "AmdSpiHcInstance.h"
#include "AmdSpiHcInternal.h"
#include "AmdSpiHcNvData.h"

EFI_GUID mAmdSpiHcFormSetGuid = AMD_SPI_HC_FORMSET_GUID;
CHAR16  mAmdSpiHcNvDataVar[] = AMD_SPI_HC_NV_DATA_VARIABLE;

/**
  SPI host controller event notify callback to block the Write Enable Opcode
  and lock down the SPI chipset

  Other SMM code will enable the SPI Flash Write Enable Opcode during SMM

  @param
  @param

  @retval
**/
VOID
EFIAPI
AmdSpiHcEventNotify (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS                    Status;
  SPI_HOST_CONTROLLER_INSTANCE  *Instance;
  AMD_SPI_HC_NV_DATA            AmdSpiHcNvData;
  UINTN                         VariableSize;

  Instance = Context;
  Status = AmdSpiHcHiiInstallForms (Instance);

  VariableSize = sizeof (AMD_SPI_HC_NV_DATA);
  Status = gRT->GetVariable (
                  AMD_SPI_HC_NV_DATA_VARIABLE,
                  &mAmdSpiHcFormSetGuid,
                  NULL,
                  &VariableSize,
                  &AmdSpiHcNvData
                  );

  if (!EFI_ERROR (Status) && AmdSpiHcNvData.RomArmorEnable == 1) {
    Instance->PspMailboxSpiMode = TRUE;

    // Change Protocol structure to NULL version that returns EFI_UNSUPPORTED
    // Instead of uninstalling protocol in case a driver has the protocol
    // cached
    Instance->Protocol.ChipSelect = ChipSelectNull;
    Instance->Protocol.Clock = ClockNull;
    Instance->Protocol.Transaction = TransactionNull;
  }

  gBS->CloseEvent (Event);
}

/**
  Entry point of the AMD SPI Host Controller driver.

  @param ImageHandle  Image handle of this driver.
  @param SystemTable  Pointer to standard EFI system table.

  @retval EFI_SUCCESS       Succeed.
  @retval EFI_OUT_OF_RESOURCES  Fail to install EFI_SPI_SMM_HC_PROTOCOL protocol.
  @retval EFI_DEVICE_ERROR  Fail to install EFI_SPI_SMM_HC_PROTOCOL protocol.
**/
EFI_STATUS
EFIAPI
AmdSpiHcProtocolEntry (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                          Status;
  SPI_HOST_CONTROLLER_INSTANCE        *Instance;
  VOID                                *Registration;


  DEBUG((DEBUG_INFO, "%a - ENTRY\n", __FUNCTION__));

  // Allocate the SPI Host Controller Instance
  Instance = AllocateZeroPool (sizeof (SPI_HOST_CONTROLLER_INSTANCE));
  ASSERT (Instance != NULL);
  if (Instance == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  Instance->Signature = SPI_HOST_CONTROLLER_SIGNATURE;
  Instance->PspMailboxSpiMode = FALSE;

  // Fill in the SPI Host Controller Protocol
  Instance->HcAddress = (
      PciRead32 (
        PCI_LIB_ADDRESS (FCH_LPC_BUS, FCH_LPC_DEV, FCH_LPC_FUNC, FCH_LPC_REGA0)
        )
      ) & 0xFFFFFF00;
  Instance->Protocol.Attributes = HC_SUPPORTS_WRITE_THEN_READ_OPERATIONS |
                               HC_SUPPORTS_READ_ONLY_OPERATIONS |
                               HC_SUPPORTS_WRITE_ONLY_OPERATIONS;

  Instance->Protocol.FrameSizeSupportMask = FCH_SPI_FRAME_SIZE_SUPPORT_MASK;
  Instance->Protocol.MaximumTransferBytes = SPI_HC_MAXIMUM_TRANSFER_BYTES;
  Instance->Protocol.ChipSelect = ChipSelect;
  Instance->Protocol.Clock = Clock;
  Instance->Protocol.Transaction = Transaction;

  // Install Host Controller protocol
  Status = gBS->InstallProtocolInterface (
                    &Instance->Handle,
                    &gEfiSpiHcProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &Instance->Protocol
                    );

  if (FeaturePcdGet (PcdAmdSpiWriteDisable)) {
    Status = gBS->CreateEvent (
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    AmdSpiHcEventNotify,
                    Instance,
                    &Instance->Event
                    );

    //
    // Register for protocol notifications on this event
    //
    Status = gBS->RegisterProtocolNotify (
                    &gEfiDxeMmReadyToLockProtocolGuid,
                    Instance->Event,
                    &Registration
                    );

    ASSERT_EFI_ERROR (Status);
  }

  DEBUG((DEBUG_INFO, "%a - EXIT Status=%r\n", __FUNCTION__, Status));

  return Status;
}
