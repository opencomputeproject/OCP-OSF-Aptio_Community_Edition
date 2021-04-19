/** @file
  SA PciHostBridge Library

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <PiDxe.h>
#include <IndustryStandard/Pci.h>
#include <Protocol/PciHostBridgeResourceAllocation.h>
#include <Library/BaseLib.h>
#include <Library/DevicePathLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/PciHostBridgeLib.h>

GLOBAL_REMOVE_IF_UNREFERENCED CHAR16 *mPciHostBridgeLibAcpiAddressSpaceTypeStr[] = {
  L"Mem", L"I/O", L"Bus"
};
ACPI_HID_DEVICE_PATH mRootBridgeDeviceNode = {
  {
    ACPI_DEVICE_PATH,
    ACPI_DP,
    {
      (UINT8) (sizeof (ACPI_HID_DEVICE_PATH)),
      (UINT8) ((sizeof (ACPI_HID_DEVICE_PATH)) >> 8)
    }
  },
  EISA_PNP_ID (0x0A03),
  0
};

PCI_ROOT_BRIDGE mRootBridge = {
  0,
  EFI_PCI_ATTRIBUTE_ISA_MOTHERBOARD_IO |
  EFI_PCI_ATTRIBUTE_IDE_PRIMARY_IO |
  EFI_PCI_ATTRIBUTE_ISA_IO |
  EFI_PCI_ATTRIBUTE_ISA_IO_16 |
  EFI_PCI_ATTRIBUTE_VGA_PALETTE_IO |
  EFI_PCI_ATTRIBUTE_VGA_PALETTE_IO_16 |
  EFI_PCI_ATTRIBUTE_VGA_MEMORY |
  EFI_PCI_ATTRIBUTE_VGA_IO |
  EFI_PCI_ATTRIBUTE_VGA_IO_16, // Supports;
  0, // Attributes;
  FALSE, // DmaAbove4G;
  FALSE, // NoExtendedConfigSpace;
  FALSE, // ResourceAssigned;
  EFI_PCI_HOST_BRIDGE_COMBINE_MEM_PMEM, // AllocationAttributes
  { 0, 255 }, // Bus
  { 0, 0 }, // Io - to be fixed later
  { 0, 0 }, // Mem - to be fixed later
  { 0, 0 }, // MemAbove4G - to be fixed later
  { 0, 0 }, // PMem - COMBINE_MEM_PMEM indicating no PMem and PMemAbove4GB
  { 0, 0 }, // PMemAbove4G
  NULL // DevicePath;
};

PCI_ROOT_BRIDGE *
EFIAPI
PciHostBridgeGetRootBridges (
  UINTN                                 *Count
  )
{
  mRootBridge.Mem.Base = PcdGet32 (PcdPciReservedMemBase);
  if (PcdGet32(PcdPciReservedMemLimit) != 0) {
    mRootBridge.Mem.Limit = PcdGet32 (PcdPciReservedMemLimit);
  } else {
    mRootBridge.Mem.Limit = (UINT32)PcdGet64 (PcdPciExpressBaseAddress);
  }

  mRootBridge.MemAbove4G.Base = PcdGet64 (PcdPciReservedMemAbove4GBBase);
  mRootBridge.MemAbove4G.Limit = PcdGet64 (PcdPciReservedMemAbove4GBLimit);
  
  mRootBridge.PMem.Base = PcdGet32 (PcdPciReservedPMemBase);
  mRootBridge.PMem.Limit = PcdGet32 (PcdPciReservedPMemLimit);
  mRootBridge.PMemAbove4G.Base = PcdGet64 (PcdPciReservedPMemAbove4GBBase);
  mRootBridge.PMemAbove4G.Limit = PcdGet64 (PcdPciReservedPMemAbove4GBLimit);

  if (mRootBridge.MemAbove4G.Base < mRootBridge.MemAbove4G.Limit) {
    mRootBridge.AllocationAttributes |= EFI_PCI_HOST_BRIDGE_MEM64_DECODE;
  }

  mRootBridge.Io.Base = PcdGet16 (PcdPciReservedIobase);
  mRootBridge.Io.Limit = PcdGet16 (PcdPciReservedIoLimit);

  mRootBridge.DmaAbove4G = PcdGetBool (PcdPciDmaAbove4G);
  mRootBridge.NoExtendedConfigSpace = PcdGetBool (PcdPciNoExtendedConfigSpace);
  mRootBridge.ResourceAssigned = PcdGetBool (PcdPciResourceAssigned);

  mRootBridge.DevicePath = AppendDevicePathNode (NULL, &mRootBridgeDeviceNode.Header);
  *Count = 1;
  return &mRootBridge;
}

VOID
EFIAPI
PciHostBridgeFreeRootBridges (
  PCI_ROOT_BRIDGE *Bridges,
  UINTN           Count
  )
{
  ASSERT (Count == 1);
  FreePool (Bridges->DevicePath);
}

/**
  Inform the platform that the resource conflict happens.

  @param HostBridgeHandle Handle of the Host Bridge.
  @param Configuration    Pointer to PCI I/O and PCI memory resource descriptors.
                          The Configuration contains the resources for all the
                          root bridges. The resource for each root bridge is
                          terminated with END descriptor and an additional END
                          is appended indicating the end of the whole resources.
                          The resource descriptor field values follow the description
                          in EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL.SubmitResources().
**/

VOID
EFIAPI
PciHostBridgeResourceConflict (
  EFI_HANDLE                        HostBridgeHandle,
  VOID                              *Configuration
  )
{
  EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *Descriptor;
  UINTN                             RootBridgeIndex;
  DEBUG ((EFI_D_ERROR, "PciHostBridge: Resource conflict happens!\n"));

  RootBridgeIndex = 0;
  Descriptor = (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *) Configuration;
  while (Descriptor->Desc == ACPI_ADDRESS_SPACE_DESCRIPTOR) {
    DEBUG ((EFI_D_ERROR, "RootBridge[%d]:\n", RootBridgeIndex++));
    for (; Descriptor->Desc == ACPI_ADDRESS_SPACE_DESCRIPTOR; Descriptor++) {
      ASSERT (Descriptor->ResType <
              sizeof (mPciHostBridgeLibAcpiAddressSpaceTypeStr) / sizeof (mPciHostBridgeLibAcpiAddressSpaceTypeStr[0])
              );
      DEBUG ((EFI_D_ERROR, " %s: Length/Alignment = 0x%lx / 0x%lx\n",
              mPciHostBridgeLibAcpiAddressSpaceTypeStr[Descriptor->ResType], Descriptor->AddrLen, Descriptor->AddrRangeMax));
      if (Descriptor->ResType == ACPI_ADDRESS_SPACE_TYPE_MEM) {
        DEBUG ((EFI_D_ERROR, "     Granularity/SpecificFlag = %ld / %02x%s\n",
                Descriptor->AddrSpaceGranularity, Descriptor->SpecificFlag,
                ((Descriptor->SpecificFlag & EFI_ACPI_MEMORY_RESOURCE_SPECIFIC_FLAG_CACHEABLE_PREFETCHABLE) != 0) ? L" (Prefetchable)" : L""
                ));
      }
    }
    //
    // Skip the END descriptor for root bridge
    //
    ASSERT (Descriptor->Desc == ACPI_END_TAG_DESCRIPTOR);
    Descriptor = (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *) ((EFI_ACPI_END_TAG_DESCRIPTOR *) Descriptor + 1);
  }

  ASSERT (FALSE);
}
