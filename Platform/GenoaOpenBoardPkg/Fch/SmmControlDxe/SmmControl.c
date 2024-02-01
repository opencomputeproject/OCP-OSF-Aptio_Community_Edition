/*****************************************************************************
 *
 * Copyright (C) 2020-2024 Advanced Micro Devices, Inc. All rights reserved.
 *
 *****************************************************************************/

/**
  SMM Control Driver
  
**/

#include <SmmControl.h>

UINT32                           mAcpiMmioBase;
UINT16                           mAcpiPmBase;
UINT16                           mAcpiSmiCmd;
EFI_PHYSICAL_ADDRESS             mAcpiMmioBase64;
STATIC EFI_SMM_CONTROL2_PROTOCOL mSmmControl;
EFI_EVENT                        mVirtualAddressChangeEvent = NULL;


/**
 * @brief Early SMM control init
 *
 * @details This function called from this driver's entry point
 *
 * @param[in] ImageHandle   Driver's image handle
 *
 * @returns Status of early SMM control initialization
 */
EFI_STATUS
SmmControlDxePreInit (
  IN       EFI_HANDLE              ImageHandle
  )
{
  EFI_STATUS          Status;
  UINT16              SmmControlData16;
  UINT16              SmmControlMask16;
  UINT32              SmmControlData32;
  UINT8               Index;
  EFI_HANDLE          mDriverImageHandle;
  UINT64              Length;
  UINT64              Attributes;
  EFI_GCD_MEMORY_SPACE_DESCRIPTOR GcdMemorySpaceDescriptor;

  // Enable ACPI MMIO space
  MmioOr32 (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG04, BIT1);

  // Get ACPI MMIO base and AcpiPm1EvtBlk address into global variables
  FchGetAcpiMmioBase (&mAcpiMmioBase);
  FchGetAcpiPmBase (&mAcpiPmBase);

  if ((mAcpiMmioBase == 0) || (mAcpiPmBase == 0)) {
    return EFI_LOAD_ERROR;
  }

  mAcpiSmiCmd = MmioRead16 (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG6A);

  mAcpiMmioBase64 = (EFI_PHYSICAL_ADDRESS) mAcpiMmioBase;
  Length = 0x1000;
  mDriverImageHandle = ImageHandle;

  // Attempt to Add and Allocate the memory region for mAcpiMmioBase
  Status = gDS->GetMemorySpaceDescriptor (mAcpiMmioBase, &GcdMemorySpaceDescriptor);
  if (!EFI_ERROR (Status)) {
    if (GcdMemorySpaceDescriptor.GcdMemoryType == EfiGcdMemoryTypeNonExistent) {
      Status = gDS->AddMemorySpace (
                      EfiGcdMemoryTypeMemoryMappedIo,
                      mAcpiMmioBase64,
                      Length,
                      EFI_MEMORY_RUNTIME | EFI_MEMORY_UC
                      );
      if (!EFI_ERROR (Status)) {
        Status = gDS->AllocateMemorySpace (
                        EfiGcdAllocateAddress,
                        EfiGcdMemoryTypeMemoryMappedIo,
                        12,
                        Length,
                        &mAcpiMmioBase64,
                        mDriverImageHandle,
                        NULL
                        );
        if (!EFI_ERROR (Status)) {
          Status = gDS->GetMemorySpaceDescriptor (mAcpiMmioBase64, &GcdMemorySpaceDescriptor);
        }
      }
    }
  }
  // Attempt to set runtime attribute
  if (!EFI_ERROR (Status)) {
    if (GcdMemorySpaceDescriptor.GcdMemoryType == EfiGcdMemoryTypeMemoryMappedIo) {
      Attributes = GcdMemorySpaceDescriptor.Attributes | EFI_MEMORY_RUNTIME | EFI_MEMORY_UC;
      Status = gDS->SetMemorySpaceAttributes (
                      mAcpiMmioBase,
                      Length,
                      Attributes
                      );
    }
  }
  // Failed to Allocate MMIO region as Runtime
  if (EFI_ERROR (Status)) {
    return (EFI_OUT_OF_RESOURCES);
  }

  //
  // Clean up all SMI status and enable bits
  //
  // Clear all SmiControl registers
  SmmControlData32 = 0;
  for (Index = FCH_SMI_REGA0; Index <= FCH_SMI_REGC4; Index += 4) {
    MmioWrite32 (mAcpiMmioBase + SMI_BASE + Index, SmmControlData32);
  }

  // Clear all SmiStatus registers (SmiStatus0-4)
  SmmControlData32 = 0xFFFFFFFF;
  MmioWrite32 (mAcpiMmioBase + SMI_BASE + FCH_SMI_REG80, SmmControlData32);
  MmioWrite32 (mAcpiMmioBase + SMI_BASE + FCH_SMI_REG84, SmmControlData32);
  MmioWrite32 (mAcpiMmioBase + SMI_BASE + FCH_SMI_REG88, SmmControlData32);
  MmioWrite32 (mAcpiMmioBase + SMI_BASE + FCH_SMI_REG8C, SmmControlData32);
  MmioWrite32 (mAcpiMmioBase + SMI_BASE + FCH_SMI_REG90, SmmControlData32);

  // Clear SciSmiEn and SciSmiSts
  SmmControlData32 = 0xFFFFFFFF;
  MmioWrite32 (mAcpiMmioBase + SMI_BASE + FCH_SMI_REG10, SmmControlData32);
  SmmControlData32 = 0;
  MmioWrite32 (mAcpiMmioBase + SMI_BASE + FCH_SMI_REG14, SmmControlData32);

  //
  // If SCI is not enabled, clean up all ACPI PM status/enable registers
  //
  SmmControlData16 = IoRead16 (mAcpiPmBase + R_FCH_ACPI_PM_CONTROL);
  if (!(SmmControlData16 & BIT0)) {
    // Clear WAKE_EN, RTC_EN, SLPBTN_EN, PWRBTN_EN, GBL_EN and TMR_EN
    SmmControlData16 = 0;
    SmmControlMask16 = (UINT16)~(BIT15 + BIT10 + BIT9 + BIT8 + BIT5 + BIT0);
    IoAndThenOr16 ((mAcpiPmBase + R_FCH_ACPI_PM1_ENABLE), SmmControlMask16, SmmControlData16);

    // Clear WAKE_STS, RTC_STS, SLPBTN_STS, PWRBTN_STS, GBL_STS and TMR_STS
    SmmControlData16 = BIT15 + BIT10 + BIT9 + BIT8 + BIT5 + BIT0;
    IoWrite16 ((mAcpiPmBase + R_FCH_ACPI_PM1_STATUS), SmmControlData16);

    // Clear SLP_TYPx
    SmmControlData16 = 0;
    SmmControlMask16 = (UINT16)~(BIT12 + BIT11 + BIT10);
    IoAndThenOr16 ((mAcpiPmBase + R_FCH_ACPI_PM_CONTROL), SmmControlMask16, SmmControlData16);

    // Clear GPE0 Enable Register
    SmmControlData32 = 0;
    IoWrite32 ((mAcpiPmBase + R_FCH_ACPI_EVENT_ENABLE), SmmControlData32);

    // Clear GPE0 Status Register
    SmmControlData32 = 0xFFFFFFFF;
    IoWrite32 ((mAcpiPmBase + R_FCH_ACPI_EVENT_STATUS), SmmControlData32);
  }

  //
  // Set the EOS Bit
  // Clear SmiEnB to enable SMI function
  //
  MmioAndThenOr32 (mAcpiMmioBase64 + SMI_BASE + FCH_SMI_REG98, ~BIT31, BIT28);

  return EFI_SUCCESS;
}


/**
 * @brief Clear SMI hardware status
 *
 */
EFI_STATUS
ClearSmi (
  VOID
  )
{
  UINT32              SmmControlData32;

  //
  // Clear SmiCmdPort Status Bit
  //
  SmmControlData32 = BIT11;
  MmioWrite32 (mAcpiMmioBase64 + SMI_BASE + FCH_SMI_REG88, SmmControlData32);

  //
  // Set the EOS Bit if it is currently cleared so we can get an SMI otherwise
  // leave the register alone
  //
  SmmControlData32 = MmioRead32 (mAcpiMmioBase64 + SMI_BASE + FCH_SMI_REG98);
  if ((SmmControlData32 & BIT28) == 0) {
    SmmControlData32 |= BIT28;
    MmioWrite32 (mAcpiMmioBase64 + SMI_BASE + FCH_SMI_REG98, SmmControlData32);
  }

  return EFI_SUCCESS;
}


/**
 * @brief Trigger SMI protocol function
 *
 * @details  Invoke SMI activation from either preboot or runtime environment. Use mAcpiMmioBase64 for runtime
 *           because this variable is virtualized during OS loading.
 *
 */
//
//
EFI_STATUS
EFIAPI
SmmTrigger (
  IN       CONST EFI_SMM_CONTROL2_PROTOCOL                *This,
  IN OUT   UINT8                      *CommandPort        OPTIONAL,
  IN OUT   UINT8                      *DataPort           OPTIONAL,
  IN       BOOLEAN                    Periodic            OPTIONAL,
  IN       UINTN                      ActivationInterval  OPTIONAL
  )
{
  UINT8                bIndex;
  UINT8                bData;
  UINT8                bIrqMask;
  UINT8                bIrqMask1;
  UINT8                Count = 3;

  if (Periodic) {
    return EFI_INVALID_PARAMETER;
  }

  bIndex = (CommandPort == NULL)? 0xff : *CommandPort;
  bData = (DataPort == NULL)? 0xff : *DataPort;

  // Clear any pending the SMI
  ClearSmi ();

  // Enable CmdPort SMI
  MmioAndThenOr32 (mAcpiMmioBase64 + SMI_BASE + FCH_SMI_REGB0, (UINT32)~(BIT22 + BIT23), BIT22);

  // Enable Global SMI
  MmioAnd32 (mAcpiMmioBase64 + SMI_BASE + FCH_SMI_REG98, 0x7FFFFFFF);

  // Temporal WA-Ensure IRQ0 is masked off when entering software SMI
  bIrqMask = IoRead8 (FCH_8259_MASK_REG_MASTER);
  if ((bIrqMask & BIT0) == 0) {
    bIrqMask1 = bIrqMask | BIT0;
    IoWrite8 (FCH_8259_MASK_REG_MASTER, bIrqMask1);
  }
  //-WA
  // Removing ClearSmi to fix false and missing SMI issue
  //
  // Issue command port SMI
  IoWrite16 (mAcpiSmiCmd, (bData << 8) + bIndex);

  // Removing ClearSmi to fix false and missing SMI issue
  while (Count--) {
    IoWrite8 (FCH_IOMAP_REGED, FCH_SMI_DUMMY_IO_VALUE);
  }
  // WA-Restore IRQ0 mask if needed
  if ((bIrqMask & BIT0) == 0) {
    IoWrite8 (FCH_8259_MASK_REG_MASTER, bIrqMask);
  }
  //-WA

  ClearSmi ();
  
  return EFI_SUCCESS;
}


/**
 * @brief Clear SMI protocol function
 *
 * @details  Clear any system state that was created in response to the Trigger call
 *
 */
EFI_STATUS
EFIAPI
SmmClear (
  IN       CONST EFI_SMM_CONTROL2_PROTOCOL *This,
  IN       BOOLEAN                         Periodic OPTIONAL
  )
{
  if (Periodic) {
    return EFI_INVALID_PARAMETER;
  }

  return ClearSmi ();
}


/**
 * @brief Call back function for Virtual Address Change Event.
 *
 * @details This function convert the virtual addreess to support 
 * runtime access.
 *
 * @param[in] Event   virtual Address Change Event.
 * @param[in] Context virtual Address Change call back Context.
 *
 * @returns VOID
 */
VOID
EFIAPI
VariableVirtualAddressChangeCallBack (
  IN EFI_EVENT Event,
  IN VOID *Context
  )
{
  EfiConvertPointer (0, (VOID **) &mAcpiMmioBase64);
  EfiConvertPointer (0, (VOID *) &(mSmmControl.Trigger));
  EfiConvertPointer (0, (VOID *) &(mSmmControl.Clear));
}


/**
 * @brief This DXE driver produces the SMM Control Protocol.
 *
 * @details Copy FCH_INIT_PROTOCOL to SMM, and register Fch Smm callbacks
 *
 * @param[in] ImageHandle EFI Image Handle for the DXE driver
 * @param[in] SystemTable Pointer to the EFI system table
 *
 * @returns EFI_STATUS
 * @retval EFI_SUCCESS   Module initialized successfully
 * @retval EFI_ERROR     Initialization failed (see error for more details)
 */
EFI_STATUS
EFIAPI
InstallSmmControlEntry (
  IN       EFI_HANDLE              ImageHandle,
  IN       EFI_SYSTEM_TABLE        *SystemTable
  )
{
  EFI_STATUS    Status;

  // Initialize EFI library
  Status = SmmControlDxePreInit (ImageHandle);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  VariableVirtualAddressChangeCallBack,
                  NULL,
                  &gEfiEventVirtualAddressChangeGuid,
                  &mVirtualAddressChangeEvent
                  );
  ASSERT_EFI_ERROR (Status);

  mSmmControl.Trigger              = SmmTrigger;
  mSmmControl.Clear                = SmmClear;
  mSmmControl.MinimumTriggerPeriod = 0;

  // Finally install the protocol
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gEfiSmmControl2ProtocolGuid,
                  &mSmmControl,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

