/*****************************************************************************
 *
 * Copyright (C) 2020-2023 Advanced Micro Devices, Inc. All rights reserved.
 *
 *****************************************************************************/

/**
  AMD SMM Dispatcher

**/

#include <SmmDispatcher.h>

EFI_SMM_CPU_PROTOCOL          *mSmmCpuProtocol;
SMM_SW_NODE                   *HeadSmmSwNodePtr;
SMM_COMMUNICATION_BUFFER      *CommunicationBufferPtr;
EFI_SMM_SW_CONTEXT            *EfiSmmSwContext;


/*----------------------------------------------------------------------------------------*/
/**
 * FCH SMM dispatcher handler
 * @param[in]       SmmImageHandle        Image Handle
 * @param[in]       SmmEntryContext         (see PI 1.2 for more details)
 * @param[in, out]   OPTIONAL CommunicationBuffer   Communication Buffer (see PI 1.1 for more details)
 * @param[in, out]   OPTIONAL SourceSize            Buffer size (see PI 1.1 for more details)

 * @retval          EFI_SUCCESS           SMI handled by dispatcher
 * @retval          EFI_UNSUPPORTED       SMI not supported by dispcther
 */
/*----------------------------------------------------------------------------------------*/
EFI_STATUS
EFIAPI
SmmDispatchHandler (
  IN       EFI_HANDLE                   SmmImageHandle,
  IN       CONST VOID                   *SmmEntryContext,
  IN OUT   VOID                         *CommunicationBuffer OPTIONAL,
  IN OUT   UINTN                        *CommunicationBufferSize OPTIONAL
  )
{
  EFI_STATUS              Status;
  UINT32                  SmiStatusData;
  UINT32                  EosStatus;

  if ((CommunicationBuffer != NULL) && (CommunicationBufferSize != NULL) &&
    !SmmIsBufferOutsideSmmValid ((UINTN)CommunicationBuffer, (UINT64)(*CommunicationBufferSize))) {
    DEBUG ((EFI_D_ERROR, "[%a]SMM communication data buffer in SMRAM or overflow!\n", __FUNCTION__));
    return EFI_INVALID_PARAMETER;
  }

  Status = EFI_WARN_INTERRUPT_SOURCE_PENDING; //Updated to be compliant with UDK2010.SR1.UP1

  do {
    SmiStatusData = MmioRead32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_STATUS2);
    if ((SmiStatusData &= FCH_SMI_CMD_PORT) != 0) {
      CommunicationBufferPtr->SmiStatusReg = FCH_SMI_STATUS2;
      CommunicationBufferPtr->SmiStatusBit = SmiStatusData;
      CommunicationBuffer = (VOID *) CommunicationBufferPtr;
      Status = SmmSwDispatchHandler (
                                      SmmImageHandle, 
                                      CommunicationBuffer, 
                                      CommunicationBufferSize);
      if (Status != EFI_SUCCESS) {
        Status = EFI_WARN_INTERRUPT_SOURCE_PENDING;
      }
    }

    //Set end of SMI Bit
    MmioOr32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_TRIGGER, FCH_END_OF_SMI);

    //Read EndOfSmi status
    EosStatus = MmioRead32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_TRIGGER) & FCH_END_OF_SMI;

  } while ((EosStatus != FCH_END_OF_SMI));

  return  Status;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Entry point of the AMD FCH SMM dispatcher driver
 * Example of dispatcher driver that handled IO TRAP requests only
 *
 * @param[in]     ImageHandle    Pointer to the firmware file system header
 * @param[in]     SystemTable    Pointer to System table
 *
 * @retval        EFI_SUCCESS    Module initialized successfully
 * @retval        EFI_ERROR      Initialization failed (see error for more details)
 */
/*----------------------------------------------------------------------------------------*/
EFI_STATUS
EFIAPI
SmmDispatcherEntry (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                    Status;
  EFI_HANDLE                    DispatchHandle;
  EFI_HANDLE                    SmmDispatcherHandle;

  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmCpuProtocolGuid,
                    NULL,
                    (VOID **)&mSmmCpuProtocol
                    );
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "ERROR: locate gEfiSmmCpuProtocolGuid : %r\n", Status));
    return Status;
  }

  Status = gSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    sizeof (SMM_SW_NODE),
                    (VOID **)&HeadSmmSwNodePtr
                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ZeroMem (HeadSmmSwNodePtr, sizeof (SMM_SW_NODE));

  Status = gSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    sizeof (EFI_SMM_SW_CONTEXT),
                    (VOID **)&EfiSmmSwContext
                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ZeroMem (EfiSmmSwContext, sizeof (EFI_SMM_SW_CONTEXT));

  Status = gSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    sizeof (SMM_COMMUNICATION_BUFFER),
                    (VOID **)&CommunicationBufferPtr
                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SmmDispatcherHandle =  NULL;
  Status = gSmst->SmmInstallProtocolInterface (
                                    &SmmDispatcherHandle,
                                    &gEfiSmmSwDispatch2ProtocolGuid,
                                    EFI_NATIVE_INTERFACE,
                                    &gEfiSmmSwDispatch2Protocol);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "ERROR: SmmInstallProtocolInterface installation: %r\n", Status));
    return Status;
  }

  Status = gSmst->SmiHandlerRegister (
                    SmmDispatchHandler,
                    NULL,
                    &DispatchHandle);

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "ERROR: SmiHandler registration : %r\n", Status));
    return Status;
  }

  // Clear all handled SMI status bit
  MmioAnd32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_STATUS2, FCH_SMI_CMD_PORT);

  // Clear SmiEnb bit and set EndOfSmi bit
  MmioAndThenOr32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_TRIGGER, ~BIT31, BIT28);

  return Status;
}