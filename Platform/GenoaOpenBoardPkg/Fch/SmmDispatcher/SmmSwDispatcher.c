/**
  AMD SW SMI Child Dispatcher

  Copyright (c) 2023, American Megatrends International LLC.
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <SmmDispatcher.h>

EFI_SMM_SW_DISPATCH2_PROTOCOL gEfiSmmSwDispatch2Protocol = {
  EfiSmmSwDispatch2Register,
  EfiSmmSwDispatch2UnRegister,
  (UINTN) MAX_SW_SMI_VALUE
};

/*----------------------------------------------------------------------------------------*/
/**
 * FCH SMM SW dispatcher handler
 *
 *
 * @param[in]       SmmImageHandle        Image Handle
 * @param[in, out]   OPTIONAL CommunicationBuffer   Communication Buffer (see PI 1.2 for more details)
 * @param[in, out]   OPTIONAL SourceSize            Buffer size (see PI 1.2 for more details)

 * @retval          EFI_SUCCESS           SMI handled by dispatcher
 * @retval          EFI_UNSUPPORTED       SMI not supported by dispcther
 */
/*----------------------------------------------------------------------------------------*/
EFI_STATUS
EFIAPI
SmmSwDispatchHandler (
  IN       EFI_HANDLE   SmmImageHandle,
  IN OUT   VOID         *CommunicationBuffer OPTIONAL,
  IN OUT   UINTN        *SourceSize OPTIONAL
  )
{
  EFI_STATUS                  Status = EFI_NOT_FOUND;
  SMM_SW_NODE                 *CurrentSmmSwNodePtr;
  UINT16                      SwSmiValue;
  UINT16                      SwSmiCmdAddress;  // acpismicmd is 16bit register
  UINT16                      SwSmiDispatched = 0;
  UINTN                       SizeOfSmmSwContext;
  UINTN                       Index;
  EFI_SMM_SAVE_STATE_IO_INFO  IoInfo;

  SwSmiCmdAddress = MmioRead16 (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG6A);
  SwSmiValue = IoRead16 (SwSmiCmdAddress);

  if (mSmmCpuProtocol == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: mSmmCpuProtocol Not Found !!!\n", __FUNCTION__));
    return Status;
  }

  Index = gSmst->NumberOfCpus;

  do {
    EfiSmmSwContext->SwSmiCpuIndex = Index - 1;
    Status = mSmmCpuProtocol->ReadSaveState (
                                  mSmmCpuProtocol,
                                  sizeof (EFI_SMM_SAVE_STATE_IO_INFO),
                                  EFI_SMM_SAVE_STATE_REGISTER_IO,
                                  EfiSmmSwContext->SwSmiCpuIndex,
                                  &IoInfo);

    if ((Status == EFI_SUCCESS) && (IoInfo.IoPort == SwSmiCmdAddress) && 
        ((UINT8) IoInfo.IoData == (UINT8) SwSmiValue)) {
      break;
    }
  } while (--Index);

  EfiSmmSwContext->CommandPort = (UINT8) SwSmiValue;
  EfiSmmSwContext->DataPort = (UINT8) (SwSmiValue >> 8);

  if (HeadSmmSwNodePtr->SmmSwNodePtr != NULL) {
    CurrentSmmSwNodePtr = HeadSmmSwNodePtr;
    while (CurrentSmmSwNodePtr->SmmSwNodePtr!= NULL) {
      if ((UINT8)CurrentSmmSwNodePtr->Context.SwSmiInputValue == (UINT8) SwSmiValue) {
        if (CurrentSmmSwNodePtr->CallBack2Function != NULL) {
          SizeOfSmmSwContext = (UINTN) sizeof (EFI_SMM_SW_CONTEXT);
          Status = CurrentSmmSwNodePtr->CallBack2Function (
                                                CurrentSmmSwNodePtr->DispatchHandle,
                                                &CurrentSmmSwNodePtr->Context,
                                                EfiSmmSwContext,
                                                &SizeOfSmmSwContext);
        }

        SwSmiDispatched++;
      }
      CurrentSmmSwNodePtr = CurrentSmmSwNodePtr->SmmSwNodePtr;
    }
    if (SwSmiDispatched > 0) {
      Status = EFI_SUCCESS;
    }
  }

  MmioAnd32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_STATUS2, FCH_SMI_CMD_PORT);

  return  Status;
}


BOOLEAN
STATIC
IsSwSmiValueUsed (
  IN UINTN          Value
  )
{
  BOOLEAN           Result;
  SMM_SW_NODE       *CurrentSmmSwNodePtr;

  Result = FALSE;
  CurrentSmmSwNodePtr = HeadSmmSwNodePtr;
  while ((CurrentSmmSwNodePtr != NULL) && (CurrentSmmSwNodePtr->SmmSwNodePtr != NULL) && !Result) {
    Result = (CurrentSmmSwNodePtr->Context.SwSmiInputValue == Value);
    CurrentSmmSwNodePtr = CurrentSmmSwNodePtr->SmmSwNodePtr;
  }
  return Result;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Register SW child handler
 *
 *
 * @param[in]       This                  Pointer to protocol
 * @param[in]       DispatchFunction      Callback Function
 * @param[in, out]  RegisterContext       Register contecxt (see PI 1.1 for more details)
 * @param[out]      DispatchHandle        Handle (see PI 1.1 for more details)
 *
 * @retval          EFI_SUCCESS           SMI handled by dispatcher
 * @retval          EFI_UNSUPPORTED       SMI not supported by dispcther
 */
/*----------------------------------------------------------------------------------------*/
EFI_STATUS
EFIAPI
EfiSmmSwDispatch2Register (
  IN  CONST EFI_SMM_SW_DISPATCH2_PROTOCOL       *This,
  IN        EFI_SMM_HANDLER_ENTRY_POINT2        DispatchFunction,
  IN  OUT   EFI_SMM_SW_REGISTER_CONTEXT         *RegisterContext,
  OUT       EFI_HANDLE                          *DispatchHandle
  )
{
  EFI_STATUS                  Status;
  SMM_SW_NODE                 *NewSmmSwNodePtr;
  SMM_SW_NODE                 *CurrentSmmSwNodePtr;
  SMM_SW_NODE                 *PreviousSmmSwNodePtr;
  UINTN                       Index;
  UINT32                      Data32;

  if (DispatchFunction == NULL || RegisterContext == NULL || DispatchHandle == NULL) {
    DEBUG ((DEBUG_ERROR, "%a Invalid Paramter!!!\n", __FUNCTION__));
    return EFI_INVALID_PARAMETER;
  }

  if (RegisterContext->SwSmiInputValue == (UINTN) -1) {
    for (Index = 1u; Index < MAX_SW_SMI_VALUE; Index++) {
      if (!IsSwSmiValueUsed (Index)) {
        RegisterContext->SwSmiInputValue = Index;
        DEBUG ((DEBUG_INFO, "%a Registering SW SMM handler: New SwValue = 0x%x\n", 
                __FUNCTION__, RegisterContext->SwSmiInputValue));
        break;
      }
    }
    if (Index == MAX_SW_SMI_VALUE) {
      DEBUG ((DEBUG_ERROR, "%a EFI_OUT_OF_RESOURCES!!!\n", __FUNCTION__));
      return EFI_OUT_OF_RESOURCES;
    }
  }

  // Enable CmdPort SMI
  Data32 = MmioRead32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGB0);
  Data32 |= BIT22;
  MmioWrite32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_TRIGGER, Data32);

  Status = gSmst->SmmAllocatePool (
                        EfiRuntimeServicesData,
                        sizeof (SMM_SW_NODE),
                        (VOID **)&NewSmmSwNodePtr);

  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "SmAllocatePool: EFI_OUT_OF_RESOURCES!!!\n"));
    return EFI_OUT_OF_RESOURCES;
  }
  
  NewSmmSwNodePtr->CallBack2Function = DispatchFunction;
  NewSmmSwNodePtr->Context = *RegisterContext;
  *DispatchHandle = &NewSmmSwNodePtr->DispatchHandle;
  NewSmmSwNodePtr->DispatchHandle = *DispatchHandle;

  if (HeadSmmSwNodePtr->SmmSwNodePtr == NULL) {
    NewSmmSwNodePtr->SmmSwNodePtr = HeadSmmSwNodePtr;
    HeadSmmSwNodePtr = NewSmmSwNodePtr;
  } else {
    PreviousSmmSwNodePtr = HeadSmmSwNodePtr;
    CurrentSmmSwNodePtr = HeadSmmSwNodePtr;

    while (CurrentSmmSwNodePtr->SmmSwNodePtr != NULL) {
      if (CurrentSmmSwNodePtr->Context.SwSmiInputValue == NewSmmSwNodePtr->Context.SwSmiInputValue) {

        if (PreviousSmmSwNodePtr == CurrentSmmSwNodePtr) {
          NewSmmSwNodePtr->SmmSwNodePtr = HeadSmmSwNodePtr;
          HeadSmmSwNodePtr = NewSmmSwNodePtr;
          return EFI_SUCCESS;
        }
        NewSmmSwNodePtr->SmmSwNodePtr = PreviousSmmSwNodePtr->SmmSwNodePtr;
        PreviousSmmSwNodePtr->SmmSwNodePtr = NewSmmSwNodePtr;

        return  EFI_SUCCESS;
      }
      PreviousSmmSwNodePtr = CurrentSmmSwNodePtr;
      CurrentSmmSwNodePtr = CurrentSmmSwNodePtr->SmmSwNodePtr;
    }

    PreviousSmmSwNodePtr->SmmSwNodePtr = NewSmmSwNodePtr;
    NewSmmSwNodePtr->SmmSwNodePtr = CurrentSmmSwNodePtr;
  }

  return  EFI_SUCCESS;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Unregister SW child handler
 *
 *
 * @param[in]       This                  Pointer to protocol
 * @param[in]       DispatchHandle        Dispatch Handle
 * @retval          EFI_SUCCESS           SMI handled by dispatcher
 * @retval          EFI_UNSUPPORTED       SMI not supported by dispcther
 */
/*----------------------------------------------------------------------------------------*/
EFI_STATUS
EFIAPI
EfiSmmSwDispatch2UnRegister (
  IN  CONST EFI_SMM_SW_DISPATCH2_PROTOCOL *This,
  IN  EFI_HANDLE                          DispatchHandle
  )
{

  SMM_SW_NODE               *CurrentSmmSwNodePtr;
  SMM_SW_NODE               *PreviousSmmSwNodePtr;

  if (DispatchHandle == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (HeadSmmSwNodePtr->SmmSwNodePtr == NULL) {
    return  EFI_NOT_FOUND;
  } 

  PreviousSmmSwNodePtr = HeadSmmSwNodePtr;
  CurrentSmmSwNodePtr = HeadSmmSwNodePtr;

  if (CurrentSmmSwNodePtr->DispatchHandle == DispatchHandle) {
      HeadSmmSwNodePtr = CurrentSmmSwNodePtr->SmmSwNodePtr;
  } else {
    while (CurrentSmmSwNodePtr->DispatchHandle != DispatchHandle) {
      PreviousSmmSwNodePtr = CurrentSmmSwNodePtr;
      CurrentSmmSwNodePtr = CurrentSmmSwNodePtr->SmmSwNodePtr;
      if ((CurrentSmmSwNodePtr == NULL ) || (CurrentSmmSwNodePtr->DispatchHandle == NULL)) {
        return  EFI_NOT_FOUND;
      }
    }
    PreviousSmmSwNodePtr->SmmSwNodePtr = CurrentSmmSwNodePtr->SmmSwNodePtr;
  }

  gSmst->SmmFreePool (CurrentSmmSwNodePtr);

  return  EFI_SUCCESS;
}


