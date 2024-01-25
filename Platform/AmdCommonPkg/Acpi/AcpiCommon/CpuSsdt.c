/*****************************************************************************
 *
 * Copyright (C) 2020-2023 Advanced Micro Devices, Inc. All rights reserved.
 *
 *****************************************************************************/

#include "AcpiCommon.h"

#include <Protocol/MpService.h>
#include <Register/Intel/Cpuid.h> // for CPUID_EXTENDED_TOPOLOGY

#define MAX_TEST_CPU_STRING_SIZE 20
#define OEM_REVISION_NUMBER      1
#define CREATOR_REVISION         2

/**
  Install CPU devices scoped under \_SB into DSDT

  Determine all the CPU threads and create ACPI Device nodes for each thread.

  @param[in]      ImageHandle   - Standard UEFI entry point Image Handle
  @param[in]      SystemTable   - Standard UEFI entry point System Table

  @retval         EFI_SUCCESS, various EFI FAILUREs.
**/
EFI_STATUS
EFIAPI
InstallCpuAcpi (
  IN      EFI_HANDLE         ImageHandle,
  IN      EFI_SYSTEM_TABLE   *SystemTable
)
{
  EFI_STATUS                  Status;
  LIST_ENTRY                  *ListHead;
  LIST_ENTRY                  AmlListHead;
  AML_OBJECT_INSTANCE         *MainObject;
  UINTN                       ProcessorIndex;
  CHAR8                       Identifier[MAX_TEST_CPU_STRING_SIZE];
  CHAR8                       *ScopeContainer;
  CHAR8                       *String;
  UINT8                       ScopeName0;
  UINTN                       NumberOfLogicProcessors;
  UINTN                       NumberOfEnabledProcessors;
  EFI_MP_SERVICES_PROTOCOL    *MpServices;
  EFI_PROCESSOR_INFORMATION   ProcessorInfoBuffer;
  LIST_ENTRY                  *Node;
  BOOLEAN                     ScopeWithContainer;
  UINTN                       DeviceStatus;
  UINTN                       Index;
  UINT32                      Socket;
  UINT32                      NumOfBitShift;

  DEBUG ((DEBUG_INFO, "%a: Entry\n", __FUNCTION__));

  Status = EFI_SUCCESS;
  ListHead = &AmlListHead;
  String = &Identifier[0];

  // Check if we support Dynamic CPU SSDT Generation
  if(!PcdGetBool(PcdAmdAcpiCpuSsdt) || !PcdGetBool(PcdAmdAcpiCpuSsdtProcessorScopeInSb)) {
    ASSERT (FALSE);
    return EFI_UNSUPPORTED;
  }

  // Get MP service
  MpServices = NULL;
  Status = gBS->LocateProtocol (&gEfiMpServiceProtocolGuid, NULL, (VOID **)&MpServices);
  if(EFI_ERROR (Status) || (MpServices == NULL)) {
    return EFI_NOT_FOUND;
  }

  // Get first half of Processor ID string
  // Valid character check done later
  ScopeName0 = PcdGet8(PcdAmdAcpiCpuSsdtProcessorScopeName0);

  // Is Processor in Container
  ScopeWithContainer = PcdGetBool (PcdAmdAcpiCpuSsdtProcessorContainerInSb);
  if (ScopeWithContainer) {
    ScopeContainer = PcdGetPtr(PcdAmdAcpiCpuSsdtProcessorContainerName);
  }

  // Load MpServices
  Status = MpServices->GetNumberOfProcessors (MpServices, &NumberOfLogicProcessors, &NumberOfEnabledProcessors);
  if(EFI_ERROR (Status)) {
    return Status;
  }

  InitializeListHead (ListHead);

  NumOfBitShift = 0;
  AsmCpuidEx (CPUID_EXTENDED_TOPOLOGY, 1, &NumOfBitShift, NULL, NULL, NULL);
  NumOfBitShift &= 0x1F;

  Status |= AmlScope(AmlStart, "\\_SB", ListHead); // START: Scope (\_SB)
    if (ScopeWithContainer) {
      Status |= AmlDevice(AmlStart, ScopeContainer, ListHead); // START: Device (ContainerScope)
    }
    for (Socket = 0; Socket < FixedPcdGet32 (PcdMaxCpuSocketCount); Socket++) {
      for (ProcessorIndex = 0, Index = 0; ProcessorIndex < NumberOfLogicProcessors; ProcessorIndex++) {
        // Get Info for current processor
        Status |= MpServices->GetProcessorInfo (
                    MpServices,
                    ProcessorIndex | CPU_V2_EXTENDED_TOPOLOGY,
                    &ProcessorInfoBuffer
                    );
        // Check for valid Processor under the current socket
        if (!ProcessorInfoBuffer.StatusFlag || (ProcessorInfoBuffer.Location.Package != Socket)) {
          continue;
        }
        AsciiSPrint (String, MAX_TEST_CPU_STRING_SIZE, "%c%03X", (CHAR8)ScopeName0, ProcessorIndex);
        Status |= AmlDevice(AmlStart, String, ListHead); // START: Device (CXXX)

          //_HID
          Status |= AmlName (AmlStart, "_HID", ListHead); // START: Name (_HID, Object)
            Status |= AmlOPDataString ("ACPI0007", ListHead); // Object
          Status |= AmlName (AmlClose, "_HID", ListHead); // CLOSE: Name (_HID, Object)

          DeviceStatus = DEVICE_PRESENT_BIT | DEVICE_IN_UI_BIT;
          if (ProcessorInfoBuffer.StatusFlag & PROCESSOR_ENABLED_BIT) {
            DeviceStatus |= DEVICE_ENABLED_BIT;
          }
          if (ProcessorInfoBuffer.StatusFlag & PROCESSOR_HEALTH_STATUS_BIT) {
            DeviceStatus |= DEVICE_HEALTH_BIT;
          }

          // _UID - Must match APIC Processor ID in MADT
          Status |= AmlName (AmlStart, "_UID", ListHead); // START: Name (_UID, Object)
            Status |= AmlOPDataInteger (((ProcessorInfoBuffer.Location.Package << NumOfBitShift) + Index), ListHead); // START: Object
          Status |= AmlName (AmlClose, "_UID", ListHead); // CLOSE: Name (_UID, Object)

          // _STA - As defined by 6.3.7
          Status |= AmlMethod (AmlStart, "_STA", 0, NotSerialized, 0, ListHead); // START: Method (_STA, Object)
            Status |= AmlReturn(AmlStart, ListHead); // START: Return (DeviceStatus)
              Status |= AmlOPDataInteger (DeviceStatus, ListHead); // START: DeviceStatus
            Status |= AmlReturn(AmlClose, ListHead); // CLOSE: Return (DeviceStatus)
          Status |= AmlMethod (AmlClose, "_STA", 0, NotSerialized, 0, ListHead); // CLOSE: Method (_STA, Object)

          // PACK -> Package
          Status |= AmlName (AmlStart, "PACK", ListHead); // START: Name (PACK, Object)
            Status |= AmlOPDataInteger (
                        ProcessorInfoBuffer.ExtendedInformation.Location2.Package,
                        ListHead
                        ); // START: Object
          Status |= AmlName (AmlClose, "PACK", ListHead); // CLOSE: Name (PACK, Object)

          // DIE_ -> Die
          Status |= AmlName (AmlStart, "DIE_", ListHead); // START: Name (DIE_, Object)
            Status |= AmlOPDataInteger (
                        ProcessorInfoBuffer.ExtendedInformation.Location2.Module,
                        ListHead
                        ); // START: Object
          Status |= AmlName (AmlClose, "DIE_", ListHead); // CLOSE: Name (DIE_, Object)

          // CCD_ -> Tile
          Status |= AmlName (AmlStart, "CCD_", ListHead); // START: Name (CCD_, Object)
            Status |= AmlOPDataInteger (
                        ProcessorInfoBuffer.ExtendedInformation.Location2.Tile,
                        ListHead
                        ); // START: Object
          Status |= AmlName (AmlClose, "CCD_", ListHead); // CLOSE: Name (CCD_, Object)

          // CCX_ -> Die
          Status |= AmlName (AmlStart, "CCX_", ListHead); // START: Name (CCX_, Object)
            Status |= AmlOPDataInteger (
                        ProcessorInfoBuffer.ExtendedInformation.Location2.Die,
                        ListHead
                        ); // START: Object
          Status |= AmlName (AmlClose, "CCX_", ListHead); // CLOSE: Name (CCX_, Object)

          // CORE -> Core Number
          Status |= AmlName (AmlStart, "CORE", ListHead); // START: Name (CORE, Object)
            Status |= AmlOPDataInteger (
                        ProcessorInfoBuffer.ExtendedInformation.Location2.Core,
                        ListHead
                        ); // START: Object
          Status |= AmlName (AmlClose, "CORE", ListHead); // CLOSE: Name (CORE, Object)

          // THRD  -> Thread
          Status |= AmlName (AmlStart, "THRD", ListHead); // START: Name (THRD, Object)
            Status |= AmlOPDataInteger (
                        ProcessorInfoBuffer.ExtendedInformation.Location2.Thread,
                        ListHead
                        ); // START: Object
          Status |= AmlName (AmlClose, "THRD", ListHead); // CLOSE: Name (THRD, Object)
        Status |= AmlDevice(AmlClose, String, ListHead); // CLOSE: Device (CXXX)
        Index++;
      }
    }
    if (ScopeWithContainer) {
      Status |= AmlDevice(AmlClose, ScopeContainer, ListHead); // CLOSE: Device (ContainerScope)
    }
  Status |= AmlScope(AmlClose, "\\_SB", ListHead); // CLOSE: Scope (\_SB)

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: ERROR Return Status failure\n", __FUNCTION__));
    ASSERT (FALSE);
    AmlFreeObjectList (ListHead);
    return EFI_DEVICE_ERROR;
  }

  if (!EFI_ERROR (Status)) {
    AmlDebugPrintLinkedObjects (ListHead);
  }

  // Get Main Object from List
  Node = GetFirstNode(ListHead);
  MainObject = AML_OBJECT_INSTANCE_FROM_LINK(Node);

  Status = AppendExistingAcpiTable (
              EFI_ACPI_6_3_DIFFERENTIATED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE,
              AMD_DSDT_OEMID,
              MainObject
              );

  AmlFreeObjectList (ListHead);

  return Status;
}