/*****************************************************************************
 *
 * Copyright (C) 2020-2024 Advanced Micro Devices, Inc. All rights reserved.
 *
 *****************************************************************************/

#include <Library/PcieResourcesLib.h>
#include <SilHob.h>
#include <Include/Library/HobLib.h>


LIST_ENTRY                      ListHeadReal;
LIST_ENTRY                      *RootBridgeListHead = NULL;
UINTN                           TotalRootBridges = 0;
NBIO_IP2IP_API                  *NbioIp2Ip;
NORTH_BRIDGE_PCIE_SIB           *NbPcieData;

BOOLEAN
GnbLibPciIsDevicePresent (
  UINT32              Address
  )
{
  UINT32        DeviceId;

  xUSLPciRead (Address, AccessWidth32, &DeviceId);
  if (DeviceId == 0xffffffff) {
    return false;
  } else {
    return true;
  }
}

UINT8
GnbLibFindPciCapability (
  UINT32                Address,
  UINT8                 CapabilityId
  )
{
  UINT8     CapabilityPtr;
  UINT8     CurrentCapabilityId;
  
  CapabilityPtr = 0x34;
  if (!GnbLibPciIsDevicePresent (Address)) {
    return  0;
  }
  while (CapabilityPtr != 0) {
    xUSLPciRead (Address | CapabilityPtr, AccessWidth8 , &CapabilityPtr);
    if (CapabilityPtr != 0) {
      xUSLPciRead (Address | CapabilityPtr , AccessWidth8 , &CurrentCapabilityId);
      if (CurrentCapabilityId == CapabilityId) {
        break;
      }
      CapabilityPtr++;
    }
  }
  return  CapabilityPtr;
}

/**
  *function to get the IOAPIC bridge interrupt routing address
  *
  *@param[in]      GnbHandle       Instance of GNB handle
  *@param[in]      PortId          Port Id
  *@param[out]     Value           Return Value
**/
void ReadIoApicBrInterruptRouting(GNB_HANDLE  *GnbHandle, UINT8 PortId, VOID* Value)
{
    UINT32      SmnReg;

    SmnReg = SMN_IOHUB0_N0NBIO0_IOAPIC_BR_INTERRUPT_ROUTING_ADDRESS + PortId;

    *((UINT32 *)Value) = (UINT32)xUSLSmnRead (
                                    GnbHandle->Address.Address.Segment, 
                                    GnbHandle->Address.Address.Bus,
                                    NBIO_SPACE(GnbHandle, SmnReg));
}

/**
  *function to get the IOAPIC high address
  *
  *@param[in]      GnbHandle       Instance of GNB handle
  *@param[out]     Value           Return Value
**/
void ReadIoApicHiAddress(GNB_HANDLE  *GnbHandle, UINT32 *Value)
{
    *Value = xUSLSmnRead (
                   GnbHandle->Address.Address.Segment, 
                   GnbHandle->Address.Address.Bus,
                   NBIO_SPACE(GnbHandle, SMN_IOHUB0NBIO0_IOAPIC_BASE_ADDR_HI_ADDRESS));
}

/**
  *function to get the IOAPIC low address
  *
  *@param[in]      GnbHandle       Instance of GNB handle
  *@param[out]     Value           Return Value
**/
void ReadIoApicLoAddress(GNB_HANDLE  *GnbHandle, UINT32 *Value)
{
    *Value = xUSLSmnRead (
                  GnbHandle->Address.Address.Segment, 
                  GnbHandle->Address.Address.Bus,
                  NBIO_SPACE(GnbHandle, SMN_IOHUB0NBIO0_IOAPIC_BASE_ADDR_LO_ADDRESS));
}

/**
  *Internal function which gets the IOAPIC base
  *
  *@param[in]      GnbHandle       Instance of GNB handle
  *@param[in,out]  FixedResource - Single Fixed Resource Instance
  *
  *@retval         EFI_SUCCESS, various EFI FAILUREs.
**/
EFI_STATUS
GetIoApicBase (
  IN       GNB_HANDLE                       *GnbHandle,
  IN OUT   FIXED_RESOURCES_INSTANCE         *FixedResource
  )
{
    UINT64        BaseAddress = 0x0;
    UINT32        Value;

    ReadIoApicHiAddress(GnbHandle, &Value);

    BaseAddress = (UINT64) LShiftU64(Value, 32);

    ReadIoApicLoAddress(GnbHandle, &Value);

    BaseAddress |= Value;
    BaseAddress &= ~0xF;

    FixedResource->FixedResourceObject.Address = BaseAddress;

    return EFI_SUCCESS;
}

/**
  *Internal function which gets the IOMMU base
  *
  *@param[in]      GnbHandle       Instance of GNB handle
  *@param[in,out]  FixedResource - Single Fixed Resource Instance
  *
  *@retval         EFI_SUCCESS, various EFI FAILUREs.
**/
EFI_STATUS
GetIommuBase (
  IN       GNB_HANDLE                      *GnbHandle,
  IN OUT   FIXED_RESOURCES_INSTANCE        *FixedResource
  )
{
    UINT16                  CapabilityOffset;
    UINT64                  BaseAddress = 0x0;
    UINT32                  Value;
    PCI_ADDR                GnbIommuPciAddress;

    GnbIommuPciAddress = NbioIp2Ip->GetHostPciAddress (GnbHandle);
    GnbIommuPciAddress.Address.Function = 0x2;

    if (GnbLibPciIsDevicePresent (GnbIommuPciAddress.AddressValue)) {

      CapabilityOffset = GnbLibFindPciCapability (GnbIommuPciAddress.AddressValue, 0xF);
      xUSLPciRead (GnbIommuPciAddress.AddressValue | (CapabilityOffset + 0x8), AccessWidth32, &Value);
      BaseAddress = (UINT64) LShiftU64(Value, 32);
      xUSLPciRead (GnbIommuPciAddress.AddressValue | (CapabilityOffset + 0x4), AccessWidth32, &Value);
      BaseAddress |= Value;
    }
    FixedResource->FixedResourceObject.Address = BaseAddress;

    return EFI_SUCCESS;
}

/**
  *Internal function which calculates Redirection table entry for mapped bridge interrupt and
  *redirection table entries for mapped INT A/B/C/D for endpoint interrupts
  *
  *@param[in]      GnbHandle    Instance of GNB handle
  *@param[in,out]  RootBridge - Single Root Port Instance
  *@param[in]      PcieEngine   Single PcieEngine Instance
  *
**/
VOID
EFIAPI
CollectInterruptInfo (
  IN      GNB_HANDLE                      *GnbHandle,
  IN OUT  PCI_ROOT_PORT_INSTANCE          *RootPort,
  IN      PCIe_ENGINE_CONFIG              *PcieEngine
)
{
    UINTN                                 RelativeInterrupt;
    UINT32                                InterruptSwizzle;
    UINT8                                 *InterruptSwizzleArray;
    IOAPIC_BR_INTERRUPT_ROUTING_STRUCT    IoapicBrInterruptRouting;
    UINTN                                 Index;

    InterruptSwizzleArray = (UINT8 *)&InterruptSwizzle;

    ReadIoApicBrInterruptRouting(GnbHandle, (PcieEngine->Type.Port.PortId * 4), &(IoapicBrInterruptRouting.Value));
    RootPort->RootPortObject.BridgeInterrupt = IoapicBrInterruptRouting.Field.Br_ext_Intr_map;

    // Swizzle Interrupts 0->3 (INTA->INTD) according to the swizzle bits.
    //rotates the interrupts in to the proper swizzle position (4 interrupts 0->3 and then rotates 8 bits times swz)
    InterruptSwizzle = LRotU32 (0x03020100, IoapicBrInterruptRouting.Field.Br_ext_Intr_swz * 8);

    //4 interrupts per group
    RelativeInterrupt = IoapicBrInterruptRouting.Field.Br_ext_Intr_grp * 4;
    for (Index = 0; Index < 4; Index++) {
         RootPort->RootPortObject.EndpointInterruptArray[Index] = RelativeInterrupt + InterruptSwizzleArray[Index];
    }
}

/**
  *Internal function to Collect fixed resource instances under each Root Bridge instance
  *Includes IOAPIC and IOMMU base addresses.

  *@param[in]      GnbHandle    Instance of GNB handle
  *@param[in,out]  RootBridge - Single Root Bridge Instance

  *@retval         EFI_SUCCESS, various EFI FAILUREs.
**/
EFI_STATUS
EFIAPI
CollectFixedResourcesInfo (
  IN      GNB_HANDLE                      *GnbHandle,
  IN OUT  PCI_ROOT_BRIDGE_INSTANCE        *RootBridge
)
{

    FIXED_RESOURCES_INSTANCE            *FixedResource;
    UINTN                                Index;

    RootBridge->NumberOfFixedResources = NBIO_MAX_FIXED_RESOURCES;

    EFI_STATUS (*fun_ptr_arr[])(GNB_HANDLE *, FIXED_RESOURCES_INSTANCE *) = {GetIommuBase, GetIoApicBase};

    for (Index = 0; Index < NBIO_MAX_FIXED_RESOURCES; Index ++) {
       FixedResource = AllocatePool (sizeof (FIXED_RESOURCES_INSTANCE));

       if(FixedResource == NULL) {
         DEBUG ((DEBUG_ERROR, "%a ERROR: Fixed Resource Instance Allocation Failed\n", __FUNCTION__));
         return EFI_OUT_OF_RESOURCES;
       }
       FixedResource->Signature = PCI_FIXED_RESOURCES_INSTANCE_SIGNATURE;
       FixedResource->FixedResourceObject.Index = Index + 1;
       FixedResource->FixedResourceObject.ResourceType = Index;
       (*fun_ptr_arr[Index])(GnbHandle, FixedResource);

       //Insert to the tail
       InsertTailList (RootBridge->FixedResourcesList, &FixedResource->Link);
    }

    return EFI_SUCCESS;
}

/**
  *Internal function to collect Root Port instances under each Root Bridge instance
  *Device, Function numbers, bridge and endpoint interrupts relative to IOPIC.

  *@param[in]      GnbHandle    Instance of GNB handle
  *@param[in,out]  RootBridge - Single Root Bridge Instance

  *@retval         EFI_SUCCESS, various EFI FAILUREs.
**/
EFI_STATUS
EFIAPI
CollectRootPortInfo (
  IN      GNB_HANDLE                      *GnbHandle,
  IN OUT  PCI_ROOT_BRIDGE_INSTANCE        *RootBridge
)
{
    PCIe_ENGINE_CONFIG                  *PcieEngine;
    PCIe_WRAPPER_CONFIG                 *PcieWrapper;
    PCI_ROOT_PORT_INSTANCE              *RootPort;
    UINTN                               Index = 0;

    PcieWrapper = (PCIe_WRAPPER_CONFIG *) NbioIp2Ip->PcieConfigGetChild (
                                                            DESCRIPTOR_ALL_WRAPPERS, 
                                                            &(GnbHandle->Header));

    while (PcieWrapper != NULL) {
        PcieEngine = (PCIe_ENGINE_CONFIG *) NbioIp2Ip->PcieConfigGetChild (
                                                                DESCRIPTOR_ALL_ENGINES,
                                                                &(PcieWrapper->Header));
        while (PcieEngine != NULL) {

            RootPort = AllocatePool (sizeof (PCI_ROOT_PORT_INSTANCE));

            if (RootPort == NULL) {
                DEBUG ((DEBUG_ERROR, "%a ERROR: Root Port Instance Allocation Failed\n", __FUNCTION__));
                return EFI_OUT_OF_RESOURCES;
            }
            RootPort->Signature = PCI_ROOT_PORT_INSTANCE_SIGNATURE;
            ++Index;
            RootPort->RootPortObject.Index = Index;
            RootPort->RootPortObject.PortPresent = PcieEngine->Type.Port.PortData.PortPresent;
            RootPort->RootPortObject.Device = PcieEngine->Type.Port.PortData.DeviceNumber;
            RootPort->RootPortObject.Function = PcieEngine->Type.Port.PortData.FunctionNumber;
            RootPort->RootPortObject.SlotNum = PcieEngine->Type.Port.PortData.SlotNum;

            CollectInterruptInfo(GnbHandle, RootPort, PcieEngine);

            //Insert root port to the tail
            InsertTailList (RootBridge->RootPortList, &RootPort->Link);

            RootBridge->NumberOfRootPorts++;
            //Move to the next root port
            PcieEngine = PcieLibGetNextDescriptor (PcieEngine);
        }
        PcieWrapper = PcieLibGetNextDescriptor (PcieWrapper);
    }

    return EFI_SUCCESS;
}

/**
 * Init function to collect pci resources. This gets called from CallbackAfterPciIo in the dxe phase after pcie enumeration is done
 * Includes root bridges, root ports and fixed resources(IOMMU and IOAPIC base) for each root bridge
 * Does not include root bridge MMIO/PMMIO/IO resources since they can be found via the EDKII PciRootBridge Protocols
 *
 * @param[in]      PCIe_PLATFORM_CONFIG   Pointer to PCIe_PLATFORM_CONFIG
 *
 * @retval         EFI_SUCCESS, various EFI FAILUREs.
**/
EFI_STATUS
EFIAPI
AmdCollectPciResourcesInit (
     IN       PCIe_PLATFORM_CONFIG      *Pcie
  )
{
  PCI_ROOT_BRIDGE_INSTANCE              *RootBridge;
  GNB_HANDLE                            *GnbHandle;
  UINTN                                 Index = 0;

  RootBridgeListHead = &ListHeadReal;
  InitializeListHead (RootBridgeListHead);

  GnbHandle = NbioIp2Ip->NbioGetHandle (Pcie);

  while (GnbHandle != NULL) {
    RootBridge = AllocateZeroPool(sizeof (PCI_ROOT_BRIDGE_INSTANCE));
    if (RootBridge == NULL) {
      DEBUG ((DEBUG_ERROR, "%a ERROR: Root Bridge instance allocation failed\n", __FUNCTION__));
      return EFI_OUT_OF_RESOURCES;
    }

    ++Index;
    RootBridge->Signature = PCI_ROOT_BRIDGE_INSTANCE_SIGNATURE;
    RootBridge->RootBridgeObject.BaseBusNumber = GnbHandle->Address.Address.Bus;
    RootBridge->RootBridgeObject.Segment = GnbHandle->Address.Address.Segment;
    RootBridge->RootBridgeObject.SocketId = GnbHandle->SocketId;
    RootBridge->RootBridgeObject.Index = Index;

    RootBridge->RootPortList = AllocatePool (sizeof (LIST_ENTRY));

    if (RootBridge->RootPortList == NULL) {
      DEBUG ((DEBUG_ERROR, "%a ERROR: Root Bridge instance allocation failed\n", __FUNCTION__));
      return EFI_OUT_OF_RESOURCES;
    }

    InitializeListHead (RootBridge->RootPortList);
    CollectRootPortInfo (GnbHandle, RootBridge);

    RootBridge->FixedResourcesList = AllocatePool (sizeof (LIST_ENTRY));
    if (RootBridge->FixedResourcesList == NULL) {
      DEBUG ((DEBUG_ERROR, "%a ERROR: Root Bridge instance allocation failed\n", __FUNCTION__));
      return EFI_OUT_OF_RESOURCES;
    }

    InitializeListHead (RootBridge->FixedResourcesList);
    CollectFixedResourcesInfo (GnbHandle, RootBridge);

    //Insert root bridge to the tail
    InsertTailList (RootBridgeListHead, &RootBridge->Link);

    ++TotalRootBridges;
    //Move to the next root bridge
    GnbHandle = GnbGetNextHandle (GnbHandle);
    DEBUG ((DEBUG_ERROR, " GnbGetNextHandle GnbHandle: %x\n", GnbHandle));
  }
  return EFI_SUCCESS;
}

/**
  *Public function to get the number of root bridges
  *
  *@param[in,out]  NumberOfRootBridges   Number of root bridges returned
  *
**/
EFI_STATUS
EFIAPI
AmdPciResourcesGetNumberOfRootBridges (
    OUT      UINTN                                 *NumberOfRootBridges
)
{
   *NumberOfRootBridges = TotalRootBridges;
   return EFI_SUCCESS;
}

/**
  *Public function to get the root bridge info
  *
  *@param[in]     RootBridgeIndex        Root bridge index for which the root bridge info will be returned
  *@param[out]    RootBridgeInfo         Returned Root bridge info
  *
**/
EFI_STATUS
EFIAPI
AmdPciResourcesGetRootBridgeInfo (
    IN       UINTN                                 RootBridgeIndex,
    OUT      PCI_ROOT_BRIDGE_OBJECT                **RootBridgeInfo
)
{
    LIST_ENTRY                            *RbNode;
    PCI_ROOT_BRIDGE_INSTANCE              *RootBridge;

    RbNode = GetFirstNode (RootBridgeListHead);
    while (RbNode != RootBridgeListHead) {
        RootBridge = PCI_ROOT_BRIDGE_INSTANCE_FROM_LINK(RbNode);

        if (RootBridge->RootBridgeObject.Index == RootBridgeIndex) {
            *RootBridgeInfo = &(RootBridge->RootBridgeObject);
            return EFI_SUCCESS;
        }
        RbNode = GetNextNode (RootBridgeListHead, RbNode);
    }
    return EFI_NOT_FOUND;
}

/**
  *Public function to get the number of root ports
  *
  *@param[in]      RootBridgeIndex     Root bridge index for which the no of root ports are returned
  *@param[in,out]  NumberOfRootPorts   Number of root ports returned
  *
**/
EFI_STATUS
EFIAPI
AmdPciResourcesGetNumberOfRootPorts (
    IN       UINTN                                 RootBridgeIndex,
    OUT      UINTN                                 *NumberOfRootPorts
)
{
    LIST_ENTRY                            *RbNode;
    PCI_ROOT_BRIDGE_INSTANCE              *RootBridge;

    *NumberOfRootPorts = 0;
    RbNode = GetFirstNode (RootBridgeListHead);

    while (RbNode != RootBridgeListHead){
        RootBridge = PCI_ROOT_BRIDGE_INSTANCE_FROM_LINK(RbNode);

        if (RootBridge->RootBridgeObject.Index == RootBridgeIndex) {
            *NumberOfRootPorts = RootBridge->NumberOfRootPorts;
            return EFI_SUCCESS;
        }
        RbNode = GetNextNode (RootBridgeListHead, RbNode);
    }
    return EFI_NOT_FOUND;
}

/**
  *Public function to get the root port info
  *
  *@param[in]     RootBridgeIndex        Root bridge index for which the root port info will be returned
  *@param[in]     RootPortIndex          Root port index for which the root port info will be returned
  *@param[out]    RootPortInfo           Returned Root port info
  *
**/
EFI_STATUS
EFIAPI
AmdPciResourcesGetRootPortInfo (
    IN       UINTN                                 RootBridgeIndex,
    IN       UINTN                                 RootPortIndex,
    OUT      PCI_ROOT_PORT_OBJECT                  **RootPortInfo
)
{
    LIST_ENTRY                            *RbNode;
    LIST_ENTRY                            *RpNode;
    PCI_ROOT_BRIDGE_INSTANCE              *RootBridge;
    PCI_ROOT_PORT_INSTANCE                *RootPort;

    RbNode = GetFirstNode (RootBridgeListHead);

    while (RbNode != RootBridgeListHead) {
        RootBridge = PCI_ROOT_BRIDGE_INSTANCE_FROM_LINK(RbNode);
        if (RootBridge->RootBridgeObject.Index == RootBridgeIndex) {
            RpNode = GetFirstNode(RootBridge->RootPortList);
            while (RpNode != RootBridge->RootPortList) {
                RootPort = PCI_ROOT_PORT_INSTANCE_FROM_LINK(RpNode);
                if (RootPort->RootPortObject.Index == RootPortIndex) {
                    *RootPortInfo = &(RootPort->RootPortObject);
                    return EFI_SUCCESS;
                }
                RpNode = GetNextNode (RootBridge->RootPortList, RpNode);
            }
        }
        RbNode = GetNextNode (RootBridgeListHead, RbNode);
    }
    return EFI_NOT_FOUND;
}

/**
  *Public function to get the number of fixed resources
  *
  *@param[in]      RootBridgeIndex          Root bridge index for which the no of fixed resources are returned
  *@param[in,out]  NumberOfFixedResources   Number of fixed resources returned
  *
**/
EFI_STATUS
EFIAPI
AmdPciResourcesGetNumberOfFixedResources (
    IN       UINTN                                 RootBridgeIndex,
    OUT      UINTN                                 *NumberOfFixedResources
)
{
    LIST_ENTRY                            *RbNode;
    PCI_ROOT_BRIDGE_INSTANCE              *RootBridge;


    *NumberOfFixedResources = 0;
    RbNode = GetFirstNode (RootBridgeListHead);

    while (RbNode != RootBridgeListHead){
        RootBridge = PCI_ROOT_BRIDGE_INSTANCE_FROM_LINK(RbNode);

        if (RootBridge->RootBridgeObject.Index == RootBridgeIndex) {
            *NumberOfFixedResources = RootBridge->NumberOfFixedResources;
            return EFI_SUCCESS;
        }
        RbNode = GetNextNode (RootBridgeListHead, RbNode);
    }

    return EFI_NOT_FOUND;
}

/**
  *Public function to get the fixed resource info
  *
  *@param[in]     RootBridgeIndex        Root bridge index for which the fixed resource info will be returned
  *@param[in]     FixedResourceIndex     Fixed resource index for which the fixed resource info will be returned
  *@param[out]    FixedResourceInfo      Returned fixed resource info
  *
**/
EFI_STATUS
EFIAPI
AmdPciResourcesGetFixedResourceInfo (
    IN       UINTN                                 RootBridgeIndex,
    IN       UINTN                                 FixedResourceIndex,
    OUT      FIXED_RESOURCES_OBJECT                **FixedResourceInfo
)
{
    LIST_ENTRY                            *RbNode;
    LIST_ENTRY                            *FrNode;
    PCI_ROOT_BRIDGE_INSTANCE              *RootBridge;
    FIXED_RESOURCES_INSTANCE              *FixedResource;

    RbNode = GetFirstNode (RootBridgeListHead);

    while (RbNode != RootBridgeListHead){
        RootBridge = PCI_ROOT_BRIDGE_INSTANCE_FROM_LINK(RbNode);
        if (RootBridge->RootBridgeObject.Index == RootBridgeIndex) {
            FrNode = GetFirstNode(RootBridge->FixedResourcesList);
            while(FrNode != RootBridge->FixedResourcesList) {
                FixedResource = PCI_FIXED_RESOURCE_INSTANCE_FROM_LINK(FrNode);
                if (FixedResource->FixedResourceObject.Index == FixedResourceIndex) {
                    *FixedResourceInfo = &(FixedResource->FixedResourceObject);
                    return EFI_SUCCESS;
                }
                FrNode = GetNextNode (RootBridge->FixedResourcesList, FrNode);
            }
        }
        RbNode = GetNextNode (RootBridgeListHead, RbNode);
    }

    return EFI_NOT_FOUND;
}

EFI_STATUS
EFIAPI
PcieGetTopology (
  OUT  PCIe_PLATFORM_CONFIG                                **Pcie
  )
{
  *Pcie = &(NbPcieData->PciePlatformConfig);
  return EFI_SUCCESS;
}

GNB_HANDLE* 
PcieNbioGetHandle (
  IN PCIe_PLATFORM_CONFIG *Pcie
  )
{
  GNB_HANDLE            *GnbHandle;

  GnbHandle = NbioIp2Ip->NbioGetHandle (Pcie);

  return GnbHandle;
}

/*---------------------------------------------------------------------------------------*/
/**
 * PcieResourcesLibConstructor
 *
 * Set Sil Memory Base and 
 *
 * @param[in, out]    VOID
 *
 * @retval            EFI_SUCCESS           Success to set Sil Memory baase
 */
EFI_STATUS
EFIAPI
PcieResourcesLibConstructor (
  VOID
  )
{
  EFI_HOB_GUID_TYPE  *Hob;
  UINT64             DataPointer;
  SIL_DATA_HOB       *SilDataHob;
  EFI_STATUS         Status;

  // the following code is required to initialize API pointers for openSIL IPs
  Hob = (EFI_HOB_GUID_TYPE *)GetFirstGuidHob(&gPeiOpenSilDataHobGuid);
  Hob++;
  SilDataHob = (SIL_DATA_HOB *)Hob;
  DataPointer = (UINT64)(SilDataHob->SilDataPointer);

  SilSetMemoryBase ((VOID*)DataPointer);     // Set the global var

  NbPcieData = (NORTH_BRIDGE_PCIE_SIB *)SilFindStructure (SilId_NorthBridgePcie, 0);

  if (NbPcieData == NULL) {
    DEBUG ((DEBUG_ERROR, "%a  NbPcieData Not Found!!!..\n", __FUNCTION__));
    return EFI_NOT_FOUND; // Could not find the IP input block
  }

  if (SilGetIp2IpApi (SilId_NbioClass, (void **)(&NbioIp2Ip)) != SilPass) {
    DEBUG ((DEBUG_ERROR, " NBIO API is not found.\n"));
    return EFI_NOT_FOUND;
  }

  DEBUG ((DEBUG_ERROR, " NBIO API is found. NbioIp2Ip: %x\n", NbioIp2Ip));

  Status = AmdCollectPciResourcesInit (&(NbPcieData->PciePlatformConfig));

  return Status;
}
