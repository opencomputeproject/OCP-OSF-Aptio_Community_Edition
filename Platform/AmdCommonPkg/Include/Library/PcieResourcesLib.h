/******************************************************************************
 * Copyright (C) 2021-2024 Advanced Micro Devices, Inc. All rights reserved.
 *
 ***************************************************************************/
#ifndef __DXE_GET_PCI_RESOURCES_H__
#define __DXE_GET_PCI_RESOURCES_H__

#include <PiDxe.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <NBIO/NbioIp2Ip.h>
#include <NBIO/IOD/include/IoapicReg.h>
#include <NBIO/IOD/include/IohcReg.h>
#include <NBIO/IOD/GnbRegisters.h>


// Format of ioapic routing structure does not change across programs
typedef union {
  struct {
    UINT32                            Br_ext_Intr_grp:3;
    UINT32                            Reserved_3_3:1;
    UINT32                            Br_ext_Intr_swz:2;
    UINT32                            Reserved_15_6:10;
    UINT32                            Br_ext_Intr_map:5;
    UINT32                            Reserved_31_21:11;
  } Field;
  UINT32 Value;
} IOAPIC_BR_INTERRUPT_ROUTING_STRUCT;

#define PCI_ROOT_BRIDGE_INSTANCE_SIGNATURE SIGNATURE_32 ('a', 'p', 'r', 'b')

typedef struct {
  UINTN        Index;
  UINT8        SocketId;
  UINTN        Segment;
  UINTN        BaseBusNumber;
} PCI_ROOT_BRIDGE_OBJECT;

typedef struct {
  UINTN                    Signature;
  PCI_ROOT_BRIDGE_OBJECT   RootBridgeObject;
  LIST_ENTRY               *RootPortList;
  UINTN                    NumberOfRootPorts;
  LIST_ENTRY               *FixedResourcesList;
  UINTN                    NumberOfFixedResources;
  LIST_ENTRY               Link;
} PCI_ROOT_BRIDGE_INSTANCE;

#define PCI_ROOT_BRIDGE_INSTANCE_FROM_LINK(a) CR (a, PCI_ROOT_BRIDGE_INSTANCE, Link, PCI_ROOT_BRIDGE_INSTANCE_SIGNATURE)

#define PCI_ROOT_PORT_INSTANCE_SIGNATURE SIGNATURE_32 ('a', 'p', 'r', 'p')

typedef struct {
  UINTN        Index;
  UINT8        PortPresent;
  UINTN        Device;
  UINTN        Function;
  UINTN        SlotNum;
  // Interrupts are relative to IOAPIC 0->n
  UINTN        BridgeInterrupt;  // Redirection table entry for mapped bridge interrupt
  UINTN        EndpointInterruptArray[4]; //Redirection table entries for mapped INT A/B/C/D
} PCI_ROOT_PORT_OBJECT;

typedef struct {
  UINTN                 Signature;
  PCI_ROOT_PORT_OBJECT  RootPortObject;
  LIST_ENTRY            Link;
} PCI_ROOT_PORT_INSTANCE;

#define PCI_ROOT_PORT_INSTANCE_FROM_LINK(a) CR (a, PCI_ROOT_PORT_INSTANCE, Link, PCI_ROOT_PORT_INSTANCE_SIGNATURE)

#define PCI_FIXED_RESOURCES_INSTANCE_SIGNATURE SIGNATURE_32 ('a', 'p', 'r', 'r')
#define NBIO_MAX_FIXED_RESOURCES 2

typedef enum  {
  IOMMU = 0,
  IOAPIC
} FIXED_RESOURCE_TYPE;

typedef struct {
  UINTN                 Index;
  FIXED_RESOURCE_TYPE   ResourceType;
  UINTN                 Address;
  UINTN                 Limit;
} FIXED_RESOURCES_OBJECT;

typedef struct {
  UINTN                   Signature;
  FIXED_RESOURCES_OBJECT  FixedResourceObject;
  LIST_ENTRY              Link;
} FIXED_RESOURCES_INSTANCE;

#define PCI_FIXED_RESOURCE_INSTANCE_FROM_LINK(a) CR (a, FIXED_RESOURCES_INSTANCE, Link, PCI_FIXED_RESOURCES_INSTANCE_SIGNATURE)

EFI_STATUS
EFIAPI
AmdCollectPciResourcesInit (
     IN       PCIe_PLATFORM_CONFIG         *Pcie
);

EFI_STATUS
EFIAPI
AmdPciResourcesGetNumberOfRootBridges (
    OUT      UINTN                                 *NumberOfRootBridges
);

EFI_STATUS
EFIAPI
AmdPciResourcesGetRootBridgeInfo (
    IN       UINTN                                 RootBridgeIndex,
    OUT      PCI_ROOT_BRIDGE_OBJECT                **RootBridgeInfo
);

EFI_STATUS
EFIAPI
AmdPciResourcesGetNumberOfRootPorts (
    IN       UINTN                                 RootBridgeIndex,
    OUT      UINTN                                 *NumberOfRootPorts
);

EFI_STATUS
EFIAPI
AmdPciResourcesGetRootPortInfo (
    IN       UINTN                                 RootBridgeIndex,
    IN       UINTN                                 RootPortIndex,
    OUT      PCI_ROOT_PORT_OBJECT                  **RootPortInfo
);

EFI_STATUS
EFIAPI
AmdPciResourcesGetNumberOfFixedResources (
    IN       UINTN                                 RootBridgeIndex,
    OUT      UINTN                                 *NumberOfFixedResources
);

EFI_STATUS
EFIAPI
AmdPciResourcesGetFixedResourceInfo (
    IN       UINTN                                 RootBridgeIndex,
    IN       UINTN                                 FixedResourceIndex,
    OUT      FIXED_RESOURCES_OBJECT                **FixedResourceInfo
);

EFI_STATUS
EFIAPI
CollectFixedResourcesInfo (
  IN      GNB_HANDLE                      *GnbHandle,
  IN OUT  PCI_ROOT_BRIDGE_INSTANCE        *RootBridge
);

EFI_STATUS
EFIAPI
PcieGetTopology (
  OUT  PCIe_PLATFORM_CONFIG                                **Pcie
  );
  
BOOLEAN
GnbLibPciIsDevicePresent (
  UINT32              Address
  );

UINT8
GnbLibFindPciCapability (
  UINT32                Address,
  UINT8                 CapabilityId
  );

void 
ReadIoApicHiAddress (
  GNB_HANDLE  *GnbHandle, 
  UINT32      *Value
  );

void 
ReadIoApicLoAddress (
  GNB_HANDLE  *GnbHandle, 
  UINT32      *Value
  );

#endif
