/*****************************************************************************
 *
 * Copyright (C) 2018-2024 Advanced Micro Devices, Inc. All rights reserved.
 *
 *****************************************************************************/

#include <PiSmm.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/SmmCorePlatformHookLib.h>
#include <Protocol/AmdSpiSmmHcState.h>

VOID                 *mSmmCorePlatformHookHcStateRegistration = NULL;
struct SmmCorePlatformHookContext {
  VOID *SmmSpiHcState;
  EFI_HANDLE SmmSpiHcStateHandle;
} mSmmCorePlatformHookContext;

/**
  Performs platform specific tasks before invoking registered SMI handlers.

  This function performs platform specific tasks before invoking registered SMI handlers.

  @retval EFI_SUCCESS       The platform hook completes successfully.
  @retval Other values      The paltform hook cannot complete due to some error.

**/
EFI_STATUS
EFIAPI
PlatformHookBeforeSmmDispatch (
  VOID
  )
{
  EFI_STATUS Status;
  SMM_EFI_SPI_HC_STATE_PROTOCOL *SpiHcState;

  Status = EFI_SUCCESS;
  SpiHcState = mSmmCorePlatformHookContext.SmmSpiHcState;
  if (SpiHcState != NULL) {
    Status = SpiHcState->SaveState (SpiHcState);
    // Open up SPI HC for SMM, Restore state will automatically return back to
    // state on SMM entry
    Status = SpiHcState->Unlock (SpiHcState);
    Status = SpiHcState->UnblockAllOpcodes (SpiHcState);
  }
  return Status;
}


/**
  Performs platform specific tasks after invoking registered SMI handlers.

  This function performs platform specific tasks after invoking registered SMI handlers.

  @retval EFI_SUCCESS       The platform hook completes successfully.
  @retval Other values      The paltform hook cannot complete due to some error.

**/
EFI_STATUS
EFIAPI
PlatformHookAfterSmmDispatch (
  VOID
  )
{
  EFI_STATUS Status;
  SMM_EFI_SPI_HC_STATE_PROTOCOL *SpiHcState;

  Status = EFI_SUCCESS;
  SpiHcState = mSmmCorePlatformHookContext.SmmSpiHcState;
  if (SpiHcState != NULL) {
    Status = SpiHcState->RestoreState (SpiHcState);
  }
  return Status;
}

/**
  Notification for SMM ReadyToLock protocol.

  @param[in] Protocol   Points to the protocol's unique identifier.
  @param[in] Interface  Points to the interface instance.
  @param[in] Handle     The handle on which the interface was installed.

  @retval EFI_SUCCESS   Notification runs successfully.
**/
EFI_STATUS
EFIAPI
SmmCorePlatformHookHcStateNotify (
  IN CONST EFI_GUID  *Protocol,
  IN VOID            *Interface,
  IN EFI_HANDLE      Handle
  )
{
  mSmmCorePlatformHookContext.SmmSpiHcState = Interface;
  mSmmCorePlatformHookContext.SmmSpiHcStateHandle = Handle;

  return EFI_SUCCESS;
}

/**
  Constructor for SmmLockBox library.
  This is used to set SmmLockBox context, which will be used in PEI phase in S3 boot path later.

  @param[in] ImageHandle  Image handle of this driver.
  @param[in] SystemTable  A Pointer to the EFI System Table.

  @retval EFI_SUCEESS
  @return Others          Some error occurs.
**/
EFI_STATUS
EFIAPI
SmmCorePlatformHookConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS           Status;

  SetMem (&mSmmCorePlatformHookContext,
          sizeof (mSmmCorePlatformHookContext),
          0);
  //
  // Register gAmdSpiHcStateProtocolGuid notification.
  //
  Status = gSmst->SmmRegisterProtocolNotify (
                    &gAmdSpiHcStateProtocolGuid,
                    SmmCorePlatformHookHcStateNotify,
                    &mSmmCorePlatformHookHcStateRegistration
                    );
  ASSERT_EFI_ERROR (Status);
  return Status;
}
