/*****************************************************************************
 *
 * Copyright (C) 2018-2024 Advanced Micro Devices, Inc. All rights reserved.
 *
 *****************************************************************************/

#include <Base.h>
#include <PiMm.h>
#include <Library/DebugLib.h>
#include <Library/PciLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/PciLib.h>
#include <Protocol/SpiSmmHc.h>
#include <Protocol/SpiHcAdditional.h>
#include <Protocol/AmdSpiSmmHcState.h>
#include <Protocol/MmReadyToLock.h>
#include <Protocol/SmmVariable.h>
#include "AmdSpiHc.h"
#include "AmdSpiHcInternal.h"
#include "AmdSpiHcInstance.h"
#include "AmdSpiHcSmmState.h"
#include "AmdSpiHcNvData.h"
#include <Fch/FchIsaReg.h>

EFI_GUID mAmdSpiHcFormSetGuid = AMD_SPI_HC_FORMSET_GUID;
CHAR16  mAmdSpiHcNvDataVar[] = AMD_SPI_HC_NV_DATA_VARIABLE;

/**
  SPI host controller event notify callback to lock down the SPI chipset

  @param
  @param

  @retval
**/
EFI_STATUS
EFIAPI
AmdSpiHcEventNotify (
  IN CONST  EFI_GUID    *Protocol,
  IN        VOID        *Interface,
  IN        EFI_HANDLE  Handle
  )
{
  EFI_STATUS                    Status;
  EFI_SMM_VARIABLE_PROTOCOL     *SmmVariable;
  EFI_SPI_HC_PROTOCOL           *SpiHc;
  AMD_SPI_HC_NV_DATA            AmdSpiHcNvData;
  UINTN                         VariableSize;

  // There can only be one SPI host controller driver in SMM
  Status = gSmst->SmmLocateProtocol (
                    &gEfiSpiSmmHcProtocolGuid,
                    NULL,
                    (VOID **)&SpiHc
                    );

  ZeroMem (&AmdSpiHcNvData, sizeof (AmdSpiHcNvData));
  //
  // Locate SmmVariableProtocol.
  //
  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmVariableProtocolGuid,
                    NULL,
                    (VOID**)&SmmVariable
                    );
  if (!EFI_ERROR (Status)) {
    Status = SmmVariable->SmmGetVariable (
                    mAmdSpiHcNvDataVar,
                    &mAmdSpiHcFormSetGuid,
                    NULL,
                    &VariableSize,
                    &AmdSpiHcNvData
                    );
  }

  return Status;
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
  SPI_SMM_HC_STATE_INSTANCE           *SpiState;


  DEBUG((DEBUG_INFO, "%a - ENTRY\n", __FUNCTION__));

  // Allocate the SPI Host Controller Instance
  Instance = AllocateZeroPool (sizeof (SPI_HOST_CONTROLLER_INSTANCE));
  ASSERT (Instance != NULL);
  if (Instance == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  Instance->Signature = SPI_HOST_CONTROLLER_SIGNATURE;
  Instance->PspMailboxSpiMode = FALSE;

  SpiState = AllocateZeroPool (sizeof (SPI_SMM_HC_STATE_INSTANCE));
  ASSERT (SpiState != NULL);
  if (SpiState == NULL) {
    FreePool (Instance);
    return EFI_OUT_OF_RESOURCES;
  }
  SpiState->Signature = SPI_SMM_HC_STATE_SIGNATURE;
  SpiState->HcInstance = Instance;

  // Allocate Host Controller save state space
  Status = AllocateState (SpiState);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    FreePool (SpiState);
    FreePool (Instance);
    return EFI_OUT_OF_RESOURCES;
  }

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

  // Fill in the SPI HC Save State Protocol
  SpiState->HcAddress = Instance->HcAddress;
  SpiState->Protocol.SaveState = SaveState;
  SpiState->Protocol.RestoreState = RestoreState;
  SpiState->Protocol.Lock = FchSpiLockSpiHostControllerRegisters;
  SpiState->Protocol.Unlock = FchSpiUnlockSpiHostControllerRegisters;
  SpiState->Protocol.BlockOpcode = FchSpiBlockOpcode;
  SpiState->Protocol.UnblockOpcode = FchSpiUnblockOpcode;
  SpiState->Protocol.UnblockAllOpcodes = FchSpiUnblockAllOpcodes;

  // Install Host Controller protocol and Save State Protocol
  Status = gSmst->SmmInstallProtocolInterface(
                    &Instance->Handle,
                    &gEfiSpiSmmHcProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &Instance->Protocol
                    );
  Status = gSmst->SmmInstallProtocolInterface(
                    &Instance->Handle,
                    &gAmdSpiHcStateProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &SpiState->Protocol
                    );
  Status = gSmst->SmmRegisterProtocolNotify (
                    &gEfiMmReadyToLockProtocolGuid,
                    AmdSpiHcEventNotify,
                    &Instance->Registration
                    );

  DEBUG((DEBUG_INFO, "%a - EXIT Status=%r\n", __FUNCTION__, Status));

  return Status;
}
