/*****************************************************************************
 *
 * Copyright (C) 2020-2023 Advanced Micro Devices, Inc. All rights reserved.
 *
 *****************************************************************************/

#include "PciSsdt.h"
#include <Library/IoLib.h>


EFI_HANDLE                      mDriverHandle;

/**
  Create sorted Root Bridge instances from AGESA NBIO resources.

  Does not include the Root Bridge resources

  @param[in]      ImageHandle   - Standard UEFI entry point Image Handle
  @param[in]      SystemTable   - Standard UEFI entry point System Table
  @param[in,out]  RootBridgeListHead  - Root Bridge Instance linked list

  @retval         EFI_SUCCESS, various EFI FAILUREs.
**/
EFI_STATUS
EFIAPI
InternalCollectSortedRootBridges (
  IN      EFI_HANDLE        ImageHandle,
  IN      EFI_SYSTEM_TABLE  *SystemTable,
  IN OUT  LIST_ENTRY        *RootBridgeListHead
)
{
  EFI_STATUS                            Status;
  PCI_ROOT_BRIDGE_OBJECT_INSTANCE       *RootBridge;
  LIST_ENTRY                            ListHeadUnstortedReal;
  LIST_ENTRY                            *ListHeadUnsorted;
  LIST_ENTRY                            *Node;
  UINTN                                 LowestBbn;
  LIST_ENTRY                            *LowestBbnNode;
  UINTN                                 Index;
  UINTN                                 NumberOfRootBridges;
  UINTN                                 RBIndex;

  ListHeadUnsorted = &ListHeadUnstortedReal;

  InitializeListHead (ListHeadUnsorted);

  Status = AmdPciResourcesGetNumberOfRootBridges (&NumberOfRootBridges);

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to get Number Of Root Bridges: %r\n",
      __FUNCTION__,
      Status
    ));
    return Status;
  }

  // Collect Root Bridges to be sorted
  for (RBIndex = 1; RBIndex <= NumberOfRootBridges; RBIndex++) {
    RootBridge = AllocateZeroPool(sizeof (PCI_ROOT_BRIDGE_OBJECT_INSTANCE));
    if (RootBridge == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      DEBUG ((DEBUG_ERROR, "%a: ERROR: Root Bridge instance allocation failed\n", __FUNCTION__));
      return Status;
    }
    AmdPciResourcesGetRootBridgeInfo(RBIndex, &RootBridge->Object);
    RootBridge->Signature = PCI_ROOT_BRIDGE_OBJECT_INSTANCE_SIGNATURE;
    InsertTailList (ListHeadUnsorted, &RootBridge->Link);
  }

  // Sort by segment number and base bus number into RootBridgeListHead
  Index = 0;
  while (!IsListEmpty (ListHeadUnsorted)) {
    Node = GetFirstNode (ListHeadUnsorted);
    LowestBbn = ~(UINTN)0;
    LowestBbnNode = NULL;
    while (Node != ListHeadUnsorted) {
      RootBridge = SSDT_PCI_ROOT_BRIDGE_INSTANCE_FROM_LINK (Node);
      if (((RootBridge->Object->Segment * MAX_PCI_BUS_NUMBER_PER_SEGMENT) + RootBridge->Object->BaseBusNumber) < LowestBbn) {
        LowestBbn = ((RootBridge->Object->Segment * MAX_PCI_BUS_NUMBER_PER_SEGMENT) + RootBridge->Object->BaseBusNumber);
        LowestBbnNode = Node;
      }
      Node = GetNextNode (ListHeadUnsorted, Node);
    }
    if (LowestBbnNode != NULL) {
      RootBridge = SSDT_PCI_ROOT_BRIDGE_INSTANCE_FROM_LINK (LowestBbnNode);
      RootBridge->Uid = Index;
      // Move node to return list in ascending order
      Node = RemoveEntryList (LowestBbnNode);
      InsertTailList (RootBridgeListHead, LowestBbnNode);
    }
    Index++;
  }

  return Status;
}

/**
   Get IOAPIC redirection entry count

   @param[in]     IoApicAddress

   @retval        Interrupt redirection entry count
**/
UINT32
EFIAPI
GetIoApicRedirectionEntryCount (
  IN      UINTN     IoApic
)
{
  IO_APIC_VERSION_REGISTER                            IoApicVersionRegister;
  if (IoApic == 0) {
    DEBUG ((DEBUG_ERROR, "%a: ERROR - IOAPIC Address not valid: 0x%X\n", __FUNCTION__, IoApic));
    ASSERT (FALSE);
    return 0;
  }

  MmioWrite8(IoApic + IOAPIC_INDEX_OFFSET,
              IO_APIC_VERSION_REGISTER_INDEX);
  IoApicVersionRegister.Uint32 = MmioRead32( IoApic + IOAPIC_DATA_OFFSET);
  return IoApicVersionRegister.Bits.MaximumRedirectionEntry + 1;
}

/**
   Increment GlobalInterruptBase with size of RootBridge IOAPIC redirection
   entry count

   @param[in]     IoApicAddress

   @retval        Interrupt redirection entry count
**/
EFI_STATUS
EFIAPI
SetNextGlobalInterruptBase(
  IN      PCI_ROOT_BRIDGE_OBJECT_INSTANCE *RootBridge,
  IN OUT  UINTN                           *GlobalInterruptBase
)
{
  EFI_STATUS                      Status;
  UINTN                           Index;
  FIXED_RESOURCES_OBJECT          *RBFixedResource;
  UINTN                           NumberOfFixedResources;

  Status = AmdPciResourcesGetNumberOfFixedResources (
                                  RootBridge->Object->Index,
                                  &NumberOfFixedResources
                                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: ERROR: GetNumberOfFixedResources Failed: %r\n",
            __FUNCTION__, Status));
    return Status;
  }
  RBFixedResource = NULL;
  for (Index = 1; Index  <= NumberOfFixedResources; Index++) {
    Status = AmdPciResourcesGetFixedResourceInfo (
                  RootBridge->Object->Index,
                  Index,
                  &RBFixedResource
                  );
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: ERROR: GetFixedResourceInfo Failed: %r\n",
              __FUNCTION__, Status));
      return Status;
    }
    if (RBFixedResource->ResourceType == IOAPIC) {
      break;
    } else {
      RBFixedResource = NULL;
    }
  }
  if (RBFixedResource == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: ERROR: GetFixedResourceInfo Missing IOAPIC Entry: %r\n",
            __FUNCTION__, Status));
    return EFI_NOT_FOUND;
  }
  *GlobalInterruptBase += GetIoApicRedirectionEntryCount ( RBFixedResource->Address);
  return Status;
}

/**
  Insert Root Bridge interrupts into AML table

  @param[in]      RootBridge  - Single Root Bridge instance
  @param[in,out]  ListHead    - AmlLib table linked list

  @retval         EFI_SUCCESS, various EFI FAILUREs.
**/
EFI_STATUS
EFIAPI
InternalInsertRootBridgeInterrupts (
  IN      PCI_ROOT_BRIDGE_OBJECT_INSTANCE *RootBridge,
  IN OUT  UINTN                           *GlobalInterruptBase,
  IN OUT  LIST_ENTRY                      *ListHead
)
{
  EFI_STATUS                      Status;
  UINTN                           Index;
  UINTN                           NumberOfRootPorts;
  PCI_ROOT_PORT_OBJECT            *RootPort;

  Status = EFI_SUCCESS;


  // Name (_PRT, <Interrupt Packages>)
  Status |= AmlName (AmlStart, "_PRT", ListHead);
    Status |= AmlPackage (AmlStart, 0, ListHead);
      // Insert all FCH IOApic interrupts first
      if (RootBridge->Object->BaseBusNumber == 0 && RootBridge->Object->Segment == 0) {
        // Behind FCH IOAPIC
        // Package () {0x0014FFFF, 0, 0, 16},  // 0 + 16
        Status |= AmlPackage (AmlStart, 0, ListHead);
          Status |= AmlOPDataInteger (0x0014FFFF, ListHead);
          Status |= AmlOPDataInteger (0, ListHead);
          Status |= AmlOPDataInteger (0, ListHead);
          Status |= AmlOPDataInteger (*GlobalInterruptBase + 16, ListHead);
        Status |= AmlPackage (AmlClose, 0, ListHead);
        // Package () {0x0014FFFF, 1, 0, 17},  // 0 + 17
        Status |= AmlPackage (AmlStart, 0, ListHead);
          Status |= AmlOPDataInteger (0x0014FFFF, ListHead);
          Status |= AmlOPDataInteger (1, ListHead);
          Status |= AmlOPDataInteger (0, ListHead);
          Status |= AmlOPDataInteger (*GlobalInterruptBase + 17, ListHead);
        Status |= AmlPackage (AmlClose, 0, ListHead);
        // Package () {0x0014FFFF, 2, 0, 18},  // 0 + 18
        Status |= AmlPackage (AmlStart, 0, ListHead);
          Status |= AmlOPDataInteger (0x0014FFFF, ListHead);
          Status |= AmlOPDataInteger (2, ListHead);
          Status |= AmlOPDataInteger (0, ListHead);
          Status |= AmlOPDataInteger (*GlobalInterruptBase + 18, ListHead);
        Status |= AmlPackage (AmlClose, 0, ListHead);
        // Package () {0x0014FFFF, 3, 0, 19},  // 0 + 19
        Status |= AmlPackage (AmlStart, 0, ListHead);
          Status |= AmlOPDataInteger (0x0014FFFF, ListHead);
          Status |= AmlOPDataInteger (3, ListHead);
          Status |= AmlOPDataInteger (0, ListHead);
          Status |= AmlOPDataInteger (*GlobalInterruptBase + 19, ListHead);
        Status |= AmlPackage (AmlClose, 0, ListHead);

        *GlobalInterruptBase += GetIoApicRedirectionEntryCount (
                                  (UINTN)PcdGet32(PcdIoApicAddress));
      }
      Status = AmdPciResourcesGetNumberOfRootPorts (
                  RootBridge->Object->Index,
                  &NumberOfRootPorts
                  );
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: ERROR: GetNumberOfRootPorts Failed: %r\n",
                __FUNCTION__, Status));
        return Status;
      }

      for (Index = 1; Index <= NumberOfRootPorts; Index++) {
        Status = AmdPciResourcesGetRootPortInfo (
                    RootBridge->Object->Index,
                    Index,
                    &RootPort
        );
        if (EFI_ERROR (Status)) {
          DEBUG ((DEBUG_ERROR, "%a: ERROR: AmdPciResourcesGetRootPortInfo Failed: %r\n",
                  __FUNCTION__, Status));
          return Status;
        }

        if (RootPort->PortPresent == 0) {
          continue;
        }
        // Only insert for Functions 1 - 4 (minus 1)
        if (((RootPort->Function - 1) & ~0x3) == 0) {
          Status |= AmlPackage (AmlStart, 0, ListHead);
            Status |= AmlOPDataInteger (
                        (RootPort->Device << 16) | 0x0000FFFF,
                        ListHead
                        );
            Status |= AmlOPDataInteger (RootPort->Function - 1, ListHead);
            Status |= AmlOPDataInteger (0, ListHead);
            Status |= AmlOPDataInteger (*GlobalInterruptBase + RootPort->BridgeInterrupt, ListHead);
          Status |= AmlPackage (AmlClose, 0, ListHead);
        }
      }
    Status |= AmlPackage (AmlClose, 0, ListHead);
  Status |= AmlName (AmlClose, "_PRT", ListHead);
  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }
  return Status;
}

/**
  Insert Root Bridge resources into the AML table

  @param[in]      RootBridge  - Single Root Bridge instance
  @param[in,out]  ListHead    - AmlLib table linked list

  @retval         EFI_SUCCESS, various EFI FAILUREs.
**/
EFI_STATUS
EFIAPI
InternalInsertRootBridgeResources (
  IN      PCI_ROOT_BRIDGE_OBJECT_INSTANCE *RootBridge,
  IN OUT  LIST_ENTRY                      *ListHead
)
{
  EFI_STATUS                                Status;
  EFI_HANDLE                                *HandleBuffer;
  UINTN                                     NumHandles;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL           *Io;
  UINTN                                     Index;
  VOID                                      *Configuration; // Never free this buffer
  EFI_ACPI_QWORD_ADDRESS_SPACE_DESCRIPTOR   *LocalBuffer;
  UINTN                                     BaseBusNumber;

  BaseBusNumber = ~(UINTN)0;
  // Get EFI Pci Root Bridge I/O Protocols
  Status = gBS->LocateHandleBuffer (
                ByProtocol,
                &gEfiPciRootBridgeIoProtocolGuid,
                NULL,
                &NumHandles,
                &HandleBuffer
                );
  if(EFI_ERROR (Status)) {
    return Status;
  }

  // Locate the Root Bridge IO protocol for this root bridge.
  LocalBuffer = NULL;
  Configuration = NULL;
  for (Index = 0; Index < NumHandles; Index++) {
    Status = gBS->OpenProtocol (HandleBuffer[Index],
                                &gEfiPciRootBridgeIoProtocolGuid,
                                (VOID **)&Io,
                                mDriverHandle,
                                NULL,
                                EFI_OPEN_PROTOCOL_GET_PROTOCOL);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    if (Io->SegmentNumber == RootBridge->Object->Segment) {
      Status = Io->Configuration (Io, &Configuration);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: ERROR: Retrieve Root Bridge Configuration failed\n", __FUNCTION__));
        return Status;
      }
      LocalBuffer = Configuration;
      while (TRUE) {
        if (LocalBuffer->Header.Header.Byte == ACPI_END_TAG_DESCRIPTOR) {
          LocalBuffer = NULL;
          break;
        } else if (LocalBuffer->Header.Header.Byte == ACPI_QWORD_ADDRESS_SPACE_DESCRIPTOR) {
          if ((LocalBuffer->ResType == ACPI_ADDRESS_SPACE_TYPE_BUS) &&
              (LocalBuffer->AddrRangeMin == RootBridge->Object->BaseBusNumber)) {
            BaseBusNumber = LocalBuffer->AddrRangeMin;
            break;
          }
        }
        LocalBuffer++;
      }
      if (BaseBusNumber == RootBridge->Object->BaseBusNumber) {
        break;
      }
    }
  }
  if (Configuration == NULL || LocalBuffer == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: ERROR: Retrieve Root Bridge Configuration failed\n", __FUNCTION__));
    return EFI_NOT_FOUND;
  }
  LocalBuffer = Configuration;

  // All Elements are sizeof (EFI_ACPI_QWORD_ADDRESS_SPACE_DESCRIPTOR) except
  // for the End Tag
  // Parse through Root Bridge resources and insert them in the ACPI Table
  while (TRUE) {
    if (LocalBuffer->Header.Header.Byte == ACPI_END_TAG_DESCRIPTOR) {
      break;
    } else if (LocalBuffer->Header.Header.Byte == ACPI_QWORD_ADDRESS_SPACE_DESCRIPTOR) {
      if (LocalBuffer->ResType == ACPI_ADDRESS_SPACE_TYPE_BUS) {
        BaseBusNumber = LocalBuffer->AddrRangeMin;
        Status |= AmlOPWordBusNumber (
                    EFI_ACPI_GENERAL_FLAG_RESOURCE_PRODUCER,
                    EFI_ACPI_GENERAL_FLAG_MIN_IS_FIXED,
                    EFI_ACPI_GENERAL_FLAG_MAX_IS_FIXED,
                    EFI_ACPI_GENERAL_FLAG_DECODE_POSITIVE,
                    0,
                    (UINT16)LocalBuffer->AddrRangeMin,
                    (UINT16)LocalBuffer->AddrRangeMax,
                    (UINT16)LocalBuffer->AddrTranslationOffset,
                    (UINT16)LocalBuffer->AddrLen,
                    ListHead
                    );
      } else if (LocalBuffer->ResType == ACPI_ADDRESS_SPACE_TYPE_IO) {
        Status |= AmlOPWordIO (
                    EFI_ACPI_GENERAL_FLAG_RESOURCE_PRODUCER,
                    EFI_ACPI_GENERAL_FLAG_MIN_IS_FIXED,
                    EFI_ACPI_GENERAL_FLAG_MAX_IS_FIXED,
                    EFI_ACPI_GENERAL_FLAG_DECODE_POSITIVE,
                    EFI_ACPI_IO_RESOURCE_SPECIFIC_FLAG_TYPE_RANGE_ENTIRE,
                    0,
                    (UINT16)LocalBuffer->AddrRangeMin,
                    (UINT16)LocalBuffer->AddrRangeMax,
                    (UINT16)LocalBuffer->AddrTranslationOffset,
                    (UINT16)LocalBuffer->AddrLen,
                    ListHead
                    );
      } else if (LocalBuffer->ResType == ACPI_ADDRESS_SPACE_TYPE_MEM) {
        Status |= AmlOPQWordMemory (
                    EFI_ACPI_GENERAL_FLAG_RESOURCE_PRODUCER,
                    EFI_ACPI_GENERAL_FLAG_DECODE_POSITIVE,
                    EFI_ACPI_GENERAL_FLAG_MIN_IS_FIXED,
                    EFI_ACPI_GENERAL_FLAG_MAX_IS_FIXED,
                    EFI_ACPI_MEMORY_RESOURCE_SPECIFIC_FLAG_NON_CACHEABLE,
                    EFI_ACPI_MEMORY_RESOURCE_SPECIFIC_FLAG_READ_WRITE,
                    0,
                    LocalBuffer->AddrRangeMin,
                    LocalBuffer->AddrRangeMax,
                    LocalBuffer->AddrTranslationOffset,
                    LocalBuffer->AddrLen,
                    ListHead
                    );
      }
    } else {
      DEBUG ((DEBUG_ERROR, "%a: ERROR: Invalid Configuration Entry\n", __FUNCTION__));
      return EFI_NOT_FOUND;
    }
    LocalBuffer++;
  }
  if (RootBridge->Object->Segment == 0 && BaseBusNumber == 0) {
    Status |= AmlOPWordIO (
                EFI_ACPI_GENERAL_FLAG_RESOURCE_PRODUCER,
                EFI_ACPI_GENERAL_FLAG_MIN_IS_FIXED,
                EFI_ACPI_GENERAL_FLAG_MAX_IS_FIXED,
                EFI_ACPI_GENERAL_FLAG_DECODE_SUBTRACTIVE,
                EFI_ACPI_IO_RESOURCE_SPECIFIC_FLAG_TYPE_RANGE_ENTIRE,
                0, 0, 0x0FFF, 0, 0x1000,
                ListHead
                );
    Status |= AmlOPQWordMemory (
                EFI_ACPI_GENERAL_FLAG_RESOURCE_PRODUCER,
                EFI_ACPI_GENERAL_FLAG_DECODE_POSITIVE,
                EFI_ACPI_GENERAL_FLAG_MIN_IS_FIXED,
                EFI_ACPI_GENERAL_FLAG_MAX_IS_FIXED,
                EFI_ACPI_MEMORY_RESOURCE_SPECIFIC_FLAG_NON_CACHEABLE,
                EFI_ACPI_MEMORY_RESOURCE_SPECIFIC_FLAG_READ_WRITE,
                0x0, FCH_IOAPIC_ADDRESS, 0xFEDFFFFF, 0x0, 0x00200000,
                ListHead
                );
    Status |= AmlOPQWordMemory (
                EFI_ACPI_GENERAL_FLAG_RESOURCE_PRODUCER,
                EFI_ACPI_GENERAL_FLAG_DECODE_POSITIVE,
                EFI_ACPI_GENERAL_FLAG_MIN_IS_FIXED,
                EFI_ACPI_GENERAL_FLAG_MAX_IS_FIXED,
                EFI_ACPI_MEMORY_RESOURCE_SPECIFIC_FLAG_NON_CACHEABLE,
                EFI_ACPI_MEMORY_RESOURCE_SPECIFIC_FLAG_READ_WRITE,
                0x0, 0xFEE01000, 0xFEFFFFFF, 0x0, 0x1FF000,
                ListHead
                );
  }
  if (EFI_ERROR (Status)) {
    Status = EFI_DEVICE_ERROR;
    return Status;
  }
  return EFI_SUCCESS;
}

/**
  Insert Root Port into the AML table

  @param[in]      RootBridge  - Single Root Bridge instance
  @param[in]      GlobalInterruptBase - Base to add to IOAPIC interrupt offset
  @param[in,out]  ListHead    - AmlLib table linked list

  @retval         EFI_SUCCESS, various EFI FAILUREs.
**/
EFI_STATUS
EFIAPI
InternalInsertRootPorts (
  IN      PCI_ROOT_BRIDGE_OBJECT_INSTANCE *RootBridge,
  IN      UINTN                           GlobalInterruptBase,
  IN OUT  LIST_ENTRY                      *ListHead
)
{
  EFI_STATUS                    Status;
  CHAR8                         NameSeg[5];
  UINTN                         RPIndex;
  UINTN                         Index;
  UINTN                         NumberOfRootPorts;
  PCI_ROOT_PORT_OBJECT          *RootPort;

  Status = AmdPciResourcesGetNumberOfRootPorts (
              RootBridge->Object->Index,
              &NumberOfRootPorts
              );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: ERROR: AmdPciResourcesGetRootPortInfo Failed: %r\n",
            __FUNCTION__, Status));
    return Status;
  }

  for (RPIndex = 1; RPIndex <= NumberOfRootPorts; RPIndex ++) {
    Status = AmdPciResourcesGetRootPortInfo (
                RootBridge->Object->Index,
                RPIndex,
                &RootPort
                );
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: ERROR: AmdPciResourcesGetRootPortInfo Failed: %r\n",
              __FUNCTION__, Status));
      return Status;
    }

    if (RootPort->PortPresent == 0) {
      continue;
    }
    AsciiSPrint(NameSeg, 5, "RP%01X%01X",
                RootPort->Device, RootPort->Function);
    Status = AmlDevice(AmlStart, NameSeg, ListHead);
      Status |= AmlName (AmlStart, "_ADR", ListHead);
        Status |= AmlOPDataInteger ((RootPort->Device << 16) +
                                    RootPort->Function, ListHead);
      Status |= AmlName (AmlClose, "_ADR", ListHead);

      // Insert Slot User Number _SUN Record.
      if (RootPort->SlotNum != 0) {
        Status |= AmlName (AmlStart, "_SUN", ListHead);
            Status |= AmlOPDataInteger (RootPort->SlotNum, ListHead);
        Status |= AmlName (AmlClose, "_SUN", ListHead);
      }

      // Build Root Port _PRT entry and insert in main ACPI Object list
      Status |= AmlName (AmlStart, "_PRT", ListHead);
        Status |= AmlPackage (AmlStart, 0, ListHead);
        for (Index = 0; Index <= 3; Index++) {
          Status |= AmlPackage (AmlStart, 0, ListHead);
            Status |= AmlOPDataInteger (0x0000FFFF, ListHead);
            Status |= AmlOPDataInteger (Index, ListHead);
            Status |= AmlOPDataInteger (0, ListHead);
            Status |= AmlOPDataInteger (
                        GlobalInterruptBase + RootPort->EndpointInterruptArray[Index],
                        ListHead
                        );
          Status |= AmlPackage (AmlClose, 0, ListHead);
        }
        Status |= AmlPackage (AmlClose, 0, ListHead);
      Status |= AmlName (AmlClose, "_PRT", ListHead);
    Status = AmlDevice (AmlClose, NameSeg, ListHead);
  }

  if (EFI_ERROR (Status)) {
    Status = EFI_DEVICE_ERROR;
  }
  return Status;
}


/**
  Insert CXL Root Bridge Port into the AML table

  @param[in,out]  ListHead    - AmlLib table linked list

  @retval         EFI_SUCCESS, various EFI FAILUREs.
**/
EFI_STATUS
EFIAPI
InternalInsertCxlRootBridge (
  IN OUT  LIST_ENTRY                *ListHead
)
{

  EFI_STATUS                        Status;
  PCI_ROOT_BRIDGE_OBJECT_INSTANCE   *RootBridge;
  CHAR8                             Identifier [MAX_LOCAL_STRING_SIZE];
  //AMD_NBIO_CXL_SERVICES_PROTOCOL    *AmdNbioCxlServicesProtocol;
  UINT8                             Index;
  //AMD_CXL_PORT_INFO_STRUCT          *NbioPortInfo;
  UINTN                             CxlCount;



  //AmdNbioCxlServicesProtocol = NULL;
  Status = EFI_SUCCESS;
  CxlCount = 0;


  //Status = gBS->LocateProtocol (
  //                &gAmdNbioCxlServicesProtocolGuid,
  //                NULL,
  //                (VOID **)&AmdNbioCxlServicesProtocol
  //                );
  //if (EFI_ERROR (Status)) {
  //  DEBUG ((DEBUG_INFO, "%a: Failed to locate AmdNbioCxlServices Protocol: %r\n", __FUNCTION__, Status));
  //  Status = EFI_SUCCESS;
  ///  return Status;
  //}
  //NbioPortInfo = AllocateZeroPool (sizeof (AMD_CXL_PORT_INFO_STRUCT));
  RootBridge = AllocateZeroPool (sizeof (PCI_ROOT_BRIDGE_OBJECT_INSTANCE));

  //
  // Populate the data structure for the CXL devices in the system to add to
  // the ACPI Table
  //
  //CxlCount = AmdNbioCxlServicesProtocol->CxlCount;
  for (Index = 0; Index < CxlCount; Index++) {
    //Status = AmdNbioCxlServicesProtocol->CxlGetRootPortInformation (
    //                                       AmdNbioCxlServicesProtocol,
    //                                       Index,
    //                                       NbioPortInfo
    //                                       );
    //if (Status != EFI_SUCCESS) {
    //  break;
    //}

    //RootBridge->Object->BaseBusNumber = (UINTN )NbioPortInfo->EndPointBDF.Address.Bus;

    if (Index < 0x10) {
      AsciiSPrint (Identifier, MAX_LOCAL_STRING_SIZE, "CXL%01X", Index);
    } else {
      AsciiSPrint (Identifier, MAX_LOCAL_STRING_SIZE, "CXL%02X", Index);
    }

    Status |= AmlDevice (AmlStart, Identifier, ListHead); // RootBridge

      // Name (_HID, "ACPI0016)  A CXL Host Bridge
      Status |= AmlName (AmlStart, "_HID", ListHead);
        Status |= AmlOPDataString ("ACPI0016", ListHead);
      Status |= AmlName (AmlClose, "_HID", ListHead);

      //
      // Name(_CID, Package(2){
      //  EISAID("PNP0A03), // PCI Compatible Host Bridge
      //  EISAID("PNP0A08") // PCI Express Compatible Host Bridge
      // })
      //
      Status |= AmlName (AmlStart, "_CID", ListHead);
        Status |= AmlPackage (AmlStart, 2, ListHead);
          Status |= AmlOPEisaId ("PNP0A03", ListHead);
          Status |= AmlOPEisaId ("PNP0A08", ListHead);
        Status |= AmlPackage (AmlClose, 2, ListHead);
      Status |= AmlName (AmlClose, "_CID", ListHead);

      // Name (_ADR, <address>)
      Status |= AmlName (AmlStart, "_ADR", ListHead);
        //Status |= AmlOPDataInteger ((NbioPortInfo->EndPointBDF.Address.Device << 16) +
        //            NbioPortInfo->EndPointBDF.Address.Function, ListHead);
      Status |= AmlName (AmlClose, "_ADR", ListHead);

      // Name (_UID, <CXL number>)
      Status |= AmlName (AmlStart, "_UID", ListHead);    //CEDT CHBS UID = 0x0000
        Status |= AmlOPDataInteger (Index, ListHead);
      Status |= AmlName (AmlClose, "_UID", ListHead);

      // Name (_BBN, <base bus number>)
      Status |= AmlName (AmlStart, "_BBN", ListHead);
        //Status |= AmlOPDataInteger (NbioPortInfo->EndPointBDF.Address.Bus, ListHead);
      Status |= AmlName (AmlClose, "_BBN", ListHead);

      // Name (_SEG, 0)
      Status |= AmlName (AmlStart, "_SEG", ListHead);
        Status |= AmlOPDataInteger ((UINT64) (RootBridge->Object->BaseBusNumber /
                    MAX_PCI_BUS_NUMBER_PER_SEGMENT), ListHead);
      Status |= AmlName (AmlClose, "_SEG", ListHead);

      // Name (_PXM, <RootBridge->SocketId>)
      Status |= AmlName (AmlStart, "_PXM", ListHead);
        //Status |= AmlOPDataInteger (NbioPortInfo->SocketID, ListHead);
      Status |= AmlName (AmlClose, "_PXM", ListHead);

      // Name (_CRS, <CRS Resource Template>)
      Status |= AmlName (AmlStart, "_CRS", ListHead);
        Status |= AmlResourceTemplate (AmlStart, ListHead);
          Status |= InternalInsertRootBridgeResources (RootBridge, ListHead);
        Status |= AmlResourceTemplate (AmlClose, ListHead);
      Status |= AmlName (AmlClose, "_CRS", ListHead);

      Status = AmlMethod (AmlStart, "_OSC", 4, NotSerialized, 4, ListHead);
        Status = AmlReturn (AmlStart, ListHead);
          // Below statement creates a return statement with function call
          // Return (OSCX (Arg0, Arg1, Arg2, Arg3))
          Status = AmlOPNameString ("\\_SB.OSCX", ListHead);
          Status = AmlOpArgN (0, ListHead);  // Arg0
          Status = AmlOpArgN (1, ListHead);  // Arg1
          Status = AmlOpArgN (2, ListHead);  // Arg2
          Status = AmlOpArgN (3, ListHead);  // Arg3
        Status = AmlReturn (AmlClose, ListHead);
      Status = AmlMethod (AmlClose, "_OSC", 4, NotSerialized, 4, ListHead);

    Status |= AmlDevice (AmlClose, Identifier, ListHead); // RootBridge
  }

  FreePool (RootBridge);
  //FreePool (NbioPortInfo);

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "%a: Failed with Status: %r, Not Critical return SUCCESS\n", __FUNCTION__, Status));
    Status = EFI_SUCCESS;
  }

  return Status;
}

/**
  Release all the Root Bridge and Root Port data instances

  @param[in]      RootBridgeListHead - Root Bridge instance information

  @retval         EFI_SUCCESS, various EFI FAILUREs.
**/
EFI_STATUS
EFIAPI
InternalReleaseRootBridges (
  IN OUT  LIST_ENTRY        *RootBridgeListHead
)
{
  EFI_STATUS                        Status;
  LIST_ENTRY                        *RootBridgeNode;
  PCI_ROOT_BRIDGE_OBJECT_INSTANCE   *RootBridge;

  Status = EFI_SUCCESS;
  RootBridgeNode = GetFirstNode (RootBridgeListHead);
  while (RootBridgeNode != RootBridgeListHead) {
    RootBridge = SSDT_PCI_ROOT_BRIDGE_INSTANCE_FROM_LINK (RootBridgeNode);
    RootBridgeNode = RemoveEntryList (RootBridgeNode);
    FreePool (RootBridge);
  }
  return Status;
}

/**
  Install PCI devices scoped under \_SB into DSDT

  Determine all the PCI Root Bridges and PCI root ports and install resources
  including needed _HID, _CID, _UID, _ADR, _CRS and _PRT Nodes.

  @param[in]      ImageHandle   - Standard UEFI entry point Image Handle
  @param[in]      SystemTable   - Standard UEFI entry point System Table

  @retval         EFI_SUCCESS, various EFI FAILUREs.
**/
EFI_STATUS
EFIAPI
InstallPciAcpi (
  IN      EFI_HANDLE         ImageHandle,
  IN      EFI_SYSTEM_TABLE   *SystemTable
)
{
  EFI_STATUS                      Status;
  AML_OBJECT_INSTANCE             *MainObject;
  LIST_ENTRY                      AmlListHead;
  LIST_ENTRY                      *ListHead;
  LIST_ENTRY                      PciRootBridgeListReal;
  LIST_ENTRY                      *PciRootBridgeList;
  PCI_ROOT_BRIDGE_OBJECT_INSTANCE *RootBridge;
  UINTN                           GlobalInterruptBase;
  CHAR8                           Identifier[MAX_LOCAL_STRING_SIZE];
  CHAR8                           *String;
  LIST_ENTRY                      *Node;

  DEBUG ((DEBUG_INFO, "%a: Entry\n", __FUNCTION__));

  Status = EFI_SUCCESS;
  mDriverHandle = ImageHandle;
  ListHead = &AmlListHead;
  PciRootBridgeList = &PciRootBridgeListReal;
  String = &Identifier[0];
  GlobalInterruptBase = 0;

  InitializeListHead (ListHead);
  InitializeListHead (PciRootBridgeList);

  Status = InternalCollectSortedRootBridges (ImageHandle, SystemTable, PciRootBridgeList);
  ASSERT_EFI_ERROR(Status);

  Status |= AmlScope(AmlStart, "\\_SB", ListHead); // START: Scope (\_SB)
    // Create Root Bridge PCXX devices
    // Iterate through Linked List
    Node = GetFirstNode (PciRootBridgeList);
    while (Node != PciRootBridgeList) {
      RootBridge = SSDT_PCI_ROOT_BRIDGE_INSTANCE_FROM_LINK (Node);
      RootBridge->GlobalInterruptStart = GlobalInterruptBase;
      // Make sure there is always PCI0 since this is a defacto standard. And
      // therefore PCI0-PCIF and then PC10-PCFF
      if (RootBridge->Uid < 0x10) {
        AsciiSPrint (String, MAX_LOCAL_STRING_SIZE, "PCI%01X", RootBridge->Uid);
      } else {
        AsciiSPrint (String, MAX_LOCAL_STRING_SIZE, "PC%02X", RootBridge->Uid);
      }
      Status |= AmlDevice (AmlStart, String, ListHead); // RootBridge
        // Name (_HID, EISAID("PNP0A08"))
        Status |= AmlName (AmlStart, "_HID", ListHead);
          Status |= AmlOPEisaId ("PNP0A08", ListHead);
        Status |= AmlName (AmlClose, "_HID", ListHead);

        // Name (_CID, EISAID("PNP0A03"))
        Status |= AmlName (AmlStart, "_CID", ListHead);
          Status |= AmlOPEisaId ("PNP0A03", ListHead);
        Status |= AmlName (AmlClose, "_CID", ListHead);

        // Name (_UID, <root bridge number>)
        Status |= AmlName (AmlStart, "_UID", ListHead);
          Status |= AmlOPDataInteger (RootBridge->Uid, ListHead);
        Status |= AmlName (AmlClose, "_UID", ListHead);

        // Name (_BBN, <base bus number>)
        Status |= AmlName (AmlStart, "_BBN", ListHead);
          Status |= AmlOPDataInteger (RootBridge->Object->BaseBusNumber, ListHead);
        Status |= AmlName (AmlClose, "_BBN", ListHead);

        // Name (_SEG, <segment number>)
        Status |= AmlName (AmlStart, "_SEG", ListHead);
          Status |= AmlOPDataInteger (RootBridge->Object->Segment, ListHead);
        Status |= AmlName (AmlClose, "_SEG", ListHead);

        // Name (_PXM, <RootBridge->SocketId>)
        Status |= AmlName (AmlStart, "_PXM", ListHead);
          Status |= AmlOPDataInteger (RootBridge->Object->SocketId, ListHead);
        Status |= AmlName (AmlClose, "_PXM", ListHead);

        // Name (_CRS, <CRS Resource Template>)
        Status |= AmlName (AmlStart, "_CRS", ListHead);
          Status |= AmlResourceTemplate (AmlStart, ListHead);
            Status |= InternalInsertRootBridgeResources (RootBridge, ListHead);
          Status |= AmlResourceTemplate (AmlClose, ListHead);
        Status |= AmlName (AmlClose, "_CRS", ListHead);

        // Name (_PRT, <Interrupt Packages>)
        Status |= InternalInsertRootBridgeInterrupts (RootBridge, &GlobalInterruptBase, ListHead);

        // Create Root Port PXXX devices
          // Name (_ADR, <pci address>)
          // Name (_PRT, <Interrupt Packages>)
          //   Needs to be offset by previous IOAPICs interrupt count
        InternalInsertRootPorts (RootBridge, GlobalInterruptBase, ListHead);

        Status = AmlMethod (AmlStart, "_OSC", 4, NotSerialized, 4, ListHead);
          Status = AmlReturn (AmlStart, ListHead);
            // Below statement creates a return statement with function call
            // Return (OSCI (Arg0, Arg1, Arg2, Arg3))
            Status = AmlOPNameString ("\\_SB.OSCI", ListHead);
            Status = AmlOpArgN (0, ListHead);  // Arg0
            Status = AmlOpArgN (1, ListHead);  // Arg1
            Status = AmlOpArgN (2, ListHead);  // Arg2
            Status = AmlOpArgN (3, ListHead);  // Arg3
          Status = AmlReturn (AmlClose, ListHead);
        Status = AmlMethod (AmlClose, "_OSC", 4, NotSerialized, 4, ListHead);

      Status |= AmlDevice (AmlClose, String, ListHead); // RootBridge
      Status |= SetNextGlobalInterruptBase(RootBridge, &GlobalInterruptBase);
      Node = GetNextNode (PciRootBridgeList, Node);
    }

  //
  // CXL device are added as Root Bridges but are not part of
  // the AMD PCI Resource Protocol
  //
  Status |=  InternalInsertCxlRootBridge (ListHead);

  Status |= AmlScope (AmlClose, "\\_SB", ListHead); // CLOSE: Scope (\_SB)

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: ERROR Return Status failure\n", __FUNCTION__));
    ASSERT (FALSE);
    AmlFreeObjectList (ListHead);
    InternalReleaseRootBridges (PciRootBridgeList);
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
  InternalReleaseRootBridges (PciRootBridgeList);
  // Need to clean up RootBridge and RootPort linked lists

  return Status;
}
