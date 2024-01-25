/*****************************************************************************
 *
 * Copyright (C) 2020-2023 Advanced Micro Devices, Inc. All rights reserved.
 *
 *****************************************************************************/

/**
 * AMD instance of the PCI Host Bridge Library.
 *
 */

/** @file
  Library instance of PciHostBridgeLib library class for coreboot.

  Copyright (C) 2016, Red Hat, Inc.
  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials are licensed and made available
  under the terms and conditions of the BSD License which accompanies this
  distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS, WITHOUT
  WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#include <PiDxe.h>
#include <Library/PciHostBridgeLib.h>
#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/PcdLib.h>
#include <Library/FabricResourceManagerLib.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/PciHostBridgeResourceAllocation.h>
#include <Apob.h>

#define SUPPORTED_ATTRIBUTE (EFI_PCI_ATTRIBUTE_IDE_PRIMARY_IO | EFI_PCI_ATTRIBUTE_IDE_SECONDARY_IO | \
                            EFI_PCI_ATTRIBUTE_ISA_IO_16 | EFI_PCI_ATTRIBUTE_ISA_MOTHERBOARD_IO | \
                            EFI_PCI_ATTRIBUTE_VGA_MEMORY | EFI_PCI_ATTRIBUTE_VGA_IO | \
                            EFI_PCI_ATTRIBUTE_VGA_PALETTE_IO)

#pragma pack(1)
typedef struct {
  ACPI_HID_DEVICE_PATH     AcpiDevicePath;
  EFI_DEVICE_PATH_PROTOCOL EndDevicePath;
} EFI_PCI_ROOT_BRIDGE_DEVICE_PATH;

typedef struct {
  UINT64            Base;
  UINT64            Length;
  UINT64            Alignment;
} RESOURCE_NODE;

typedef struct {
  RESOURCE_NODE     Mem32[MAX_SOCKETS_SUPPORTED * DFX_MAX_RBS_PER_SOCKET];
  RESOURCE_NODE     PMem32[MAX_SOCKETS_SUPPORTED * DFX_MAX_RBS_PER_SOCKET];
  RESOURCE_NODE     Mem64[MAX_SOCKETS_SUPPORTED * DFX_MAX_RBS_PER_SOCKET];
  RESOURCE_NODE     PMem64[MAX_SOCKETS_SUPPORTED * DFX_MAX_RBS_PER_SOCKET];
  RESOURCE_NODE     Io[MAX_SOCKETS_SUPPORTED * DFX_MAX_RBS_PER_SOCKET];
  RESOURCE_NODE     PciBus[MAX_SOCKETS_SUPPORTED * DFX_MAX_RBS_PER_SOCKET];
} ROOT_BRIDGE_RESOURCE;
#pragma pack ()

EFI_STATUS
AmdInitResourceSize (
  IN       ROOT_BRIDGE_RESOURCE                   *CurRbRes,
  IN OUT   DFX_FABRIC_RESOURCE_FOR_EACH_RB        *FabricResourceSize
  );

STATIC EFI_PCI_ROOT_BRIDGE_DEVICE_PATH mEfiPciRootBridgeDevicePath = {
    {
      {
        ACPI_DEVICE_PATH,
        ACPI_DP,
        {
          (UINT8)(sizeof (ACPI_HID_DEVICE_PATH)),
          (UINT8)((sizeof (ACPI_HID_DEVICE_PATH)) >> 8)
        }
      },
      EISA_PNP_ID (0x0A03), // PCI-to-PCI Bridge
      0
    },

    {
      END_DEVICE_PATH_TYPE,
      END_ENTIRE_DEVICE_PATH_SUBTYPE,
      {
        END_DEVICE_PATH_LENGTH,
        0
      }
    }
};

/**
  Return all the root bridge instances in an array.

  @param Count  Return the count of root bridge instances.

  @return All the root bridge instances in an array.
          The array should be passed into PciHostBridgeFreeRootBridges()
          when it's not used.
**/
PCI_ROOT_BRIDGE *
EFIAPI
PciHostBridgeGetRootBridges (
  UINTN *Count
  )
{

  PCI_ROOT_BRIDGE                       *RootBridge;
  PCI_ROOT_BRIDGE                       *FirstRootBridge;
  EFI_PCI_ROOT_BRIDGE_DEVICE_PATH       *DevicePath;
  CHAR16                                *DevicePathStr;
  UINT32                                SocketLoop;
  UINT32                                DieLoop;
  UINT32                                RootBridgeLoop;
  UINT32                                RootBridgeInSocket;
  UINT32                                DiePerSocketCount;
  UINT32                                RootBridgePerDieCount;
  UINT32                                RootBridgeAllocCount;
  UINT32                                PciBusBase;
  UINT32                                PciBusLimit;
  UINT64                                Base;
  UINT64                                Size;
  UINT64                                PBase;
  UINT64                                PSize;
  UINT32                                FixedRootBridgePerDieCount;
  EFI_STATUS                            Status;
  UINT32                                SocketCount;
  UINT32                                RootBridgeCount;
  DFX_FABRIC_RESOURCE_FOR_EACH_RB       FabricResource;
  SIL_STATUS                            SilStatus;
  DF_IP2IP_API                          *DfIp2IpApi;


  ZeroMem (&FabricResource, sizeof (DFX_FABRIC_RESOURCE_FOR_EACH_RB));

  Status = FabricGetAvailableResource (&FabricResource);
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "%a  FabricGetAvailableResource : %r!!!..\n", __FUNCTION__, Status));
    return NULL;
  }

  // Get Number of Socket information of the overall system.
  Status = FabricGetSystemInfo (&SocketCount, NULL, &RootBridgeCount, NULL, NULL);
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "%a  FabricGetSystemInfo : %r!!!..\n", __FUNCTION__, Status));
    return NULL;
  }

  SilStatus = SilGetIp2IpApi (SilId_DfClass, (void**)&DfIp2IpApi);
  if ((SilStatus != SilPass) || (DfIp2IpApi == NULL)) {
    DEBUG ((DEBUG_ERROR, "%a  DfIp2IpApi Not Found!!!..\n", __FUNCTION__));
    return NULL;
  }

  FixedRootBridgePerDieCount = 0;

  // Get number of root bridges from Sil
  *Count = 0;

  // Allocate RootBridge(s)
  RootBridge = RootBridgeCount ? AllocateZeroPool (RootBridgeCount * sizeof (PCI_ROOT_BRIDGE)) : NULL;
  if (RootBridge == NULL) {
    DEBUG ((EFI_D_ERROR, "%a: %r\n", __FUNCTION__, EFI_OUT_OF_RESOURCES));
    return NULL;
  }
  FirstRootBridge = RootBridge;

  // Allocate DevicePath(s)
  DevicePath = AllocateZeroPool (RootBridgeCount * sizeof (EFI_PCI_ROOT_BRIDGE_DEVICE_PATH));
  if (DevicePath == NULL) {
    DEBUG ((EFI_D_ERROR, "%a: %r\n", __FUNCTION__, EFI_OUT_OF_RESOURCES));
    FreePool (FirstRootBridge);
    return NULL;
  }

  RootBridgeAllocCount = RootBridgeCount;
  for (SocketLoop = 0; SocketLoop < SocketCount && RootBridgeAllocCount; SocketLoop++) {
    DEBUG ((EFI_D_INFO, "Socket# = %d\n", SocketLoop));

    RootBridgeInSocket = 0;
    DfIp2IpApi->DfGetProcessorInfo (SocketLoop, &DiePerSocketCount, NULL);
    for (DieLoop = 0; DieLoop < DiePerSocketCount && RootBridgeAllocCount; DieLoop++) {
      DEBUG ((EFI_D_INFO, " Die# = %d\n", DieLoop));

      DfIp2IpApi->DfGetDieInfo (SocketLoop, DieLoop, &FixedRootBridgePerDieCount, NULL, NULL);
      RootBridgePerDieCount = FixedRootBridgePerDieCount;

      for (RootBridgeLoop = 0; RootBridgeLoop < RootBridgePerDieCount && RootBridgeAllocCount; RootBridgeLoop++) {
        DEBUG ((EFI_D_INFO, "  RootBridge# = %d\n", RootBridgeLoop));

        RootBridge->Supports = SUPPORTED_ATTRIBUTE;
        RootBridge->Attributes = RootBridge->Supports;
        RootBridge->AllocationAttributes = EFI_PCI_HOST_BRIDGE_MEM64_DECODE;
        RootBridge->DmaAbove4G = TRUE;
        RootBridge->NoExtendedConfigSpace = FALSE;

        if (RootBridgeLoop >= FixedRootBridgePerDieCount) {
         PciBusBase = FabricResource.PciBusNumber[SocketLoop][RootBridgeInSocket];
         PciBusLimit = FabricResource.PciBusNumber[SocketLoop][RootBridgeInSocket];
         RootBridge->Segment = (UINT32) (PciBusBase / MAX_PCI_BUS_NUMBER_PER_SEGMENT);
        } else {
          DfIp2IpApi->DfGetRootBridgeInfo (SocketLoop, DieLoop, RootBridgeLoop, NULL, &PciBusBase, &PciBusLimit, NULL, NULL, NULL);
          RootBridge->Segment = (UINT32) (PciBusBase / MAX_PCI_BUS_NUMBER_PER_SEGMENT);
          PciBusBase %= MAX_PCI_BUS_NUMBER_PER_SEGMENT;
          PciBusLimit %= MAX_PCI_BUS_NUMBER_PER_SEGMENT;
        }
        RootBridge->Bus.Base = PciBusBase;
        RootBridge->Bus.Limit = PciBusLimit;

        if (FabricResource.IO[SocketLoop][RootBridgeInSocket].Size != 0) {
          RootBridge->Io.Base = FabricResource.IO[SocketLoop][RootBridgeInSocket].Base;
          RootBridge->Io.Limit = RootBridge->Io.Base + FabricResource.IO[SocketLoop][RootBridgeInSocket].Size - 1;
        } else {
          RootBridge->Io.Base = 0xFFFF;
          RootBridge->Io.Limit = 0;
        }

        //
        // NOTE: MMIO below 4G is assigned from top to bottom.
        // This way, the FCH on Primary RootBridge gets MMIO reosurces right below 4GB's.
        //
        if ((RootBridge->Bus.Base == 0) && (RootBridge->Bus.Limit != 0)) {
          Base = (FabricResource.PrimaryRbSecondNonPrefetchableMmioSizeBelow4G.Size > FabricResource.NonPrefetchableMmioSizeBelow4G[SocketLoop][RootBridgeInSocket].Size) ?
            FabricResource.PrimaryRbSecondNonPrefetchableMmioSizeBelow4G.Base : FabricResource.NonPrefetchableMmioSizeBelow4G[SocketLoop][RootBridgeInSocket].Base;
          Size = (FabricResource.PrimaryRbSecondNonPrefetchableMmioSizeBelow4G.Size > FabricResource.NonPrefetchableMmioSizeBelow4G[SocketLoop][RootBridgeInSocket].Size) ?
            FabricResource.PrimaryRbSecondNonPrefetchableMmioSizeBelow4G.Size : FabricResource.NonPrefetchableMmioSizeBelow4G[SocketLoop][RootBridgeInSocket].Size;

          PBase = (FabricResource.PrimaryRbSecondPrefetchableMmioSizeBelow4G.Size > FabricResource.PrefetchableMmioSizeBelow4G[SocketLoop][RootBridgeInSocket].Size) ?
            FabricResource.PrimaryRbSecondPrefetchableMmioSizeBelow4G.Base : FabricResource.PrefetchableMmioSizeBelow4G[SocketLoop][RootBridgeInSocket].Base;
          PSize = (FabricResource.PrimaryRbSecondPrefetchableMmioSizeBelow4G.Size > FabricResource.PrefetchableMmioSizeBelow4G[SocketLoop][RootBridgeInSocket].Size) ?
            FabricResource.PrimaryRbSecondPrefetchableMmioSizeBelow4G.Size : FabricResource.PrefetchableMmioSizeBelow4G[SocketLoop][RootBridgeInSocket].Size;
        } else {
          Base = FabricResource.NonPrefetchableMmioSizeBelow4G[SocketLoop][RootBridgeInSocket].Base;
          Size = FabricResource.NonPrefetchableMmioSizeBelow4G[SocketLoop][RootBridgeInSocket].Size;

          PBase = FabricResource.PrefetchableMmioSizeBelow4G[SocketLoop][RootBridgeInSocket].Base;
          PSize = FabricResource.PrefetchableMmioSizeBelow4G[SocketLoop][RootBridgeInSocket].Size;
        }

        if (Size != 0) {
          RootBridge->Mem.Base = Base;
          RootBridge->Mem.Limit = Base + Size - 1;
        } else {
          RootBridge->Mem.Base = 0xFFFFFFFFFFFFFFFF;
          RootBridge->Mem.Limit = 0;
        }
        if (FabricResource.NonPrefetchableMmioSizeAbove4G[SocketLoop][RootBridgeInSocket].Size != 0) {
          RootBridge->MemAbove4G.Base = FabricResource.NonPrefetchableMmioSizeAbove4G[SocketLoop][RootBridgeInSocket].Base;
          RootBridge->MemAbove4G.Limit = RootBridge->MemAbove4G.Base + FabricResource.NonPrefetchableMmioSizeAbove4G[SocketLoop][RootBridgeInSocket].Size - 1;
        } else {
          RootBridge->MemAbove4G.Base = 0xFFFFFFFFFFFFFFFF;
          RootBridge->MemAbove4G.Limit = 0;
        }

        if (PSize != 0) {
          RootBridge->PMem.Base = PBase;
          RootBridge->PMem.Limit = PBase + PSize - 1;
        } else {
          RootBridge->PMem.Base = 0xFFFFFFFFFFFFFFFF;
          RootBridge->PMem.Limit = 0;
        }

        if (FabricResource.PrefetchableMmioSizeAbove4G[SocketLoop][RootBridgeInSocket].Size != 0) {
          RootBridge->PMemAbove4G.Base = FabricResource.PrefetchableMmioSizeAbove4G[SocketLoop][RootBridgeInSocket].Base;
          RootBridge->PMemAbove4G.Limit = RootBridge->PMemAbove4G.Base + FabricResource.PrefetchableMmioSizeAbove4G[SocketLoop][RootBridgeInSocket].Size - 1;
        } else {
          RootBridge->PMemAbove4G.Base = 0xFFFFFFFFFFFFFFFF;
          RootBridge->PMemAbove4G.Limit = 0;
        }

        CopyMem (DevicePath, &mEfiPciRootBridgeDevicePath, sizeof (EFI_PCI_ROOT_BRIDGE_DEVICE_PATH));
        DevicePath->AcpiDevicePath.UID = RootBridge->Segment * MAX_PCI_BUS_NUMBER_PER_SEGMENT + ((UINT32) PciBusBase);
        RootBridge->DevicePath = (EFI_DEVICE_PATH_PROTOCOL *)DevicePath;

        DevicePathStr = ConvertDevicePathToText (RootBridge->DevicePath, FALSE, FALSE);
        DEBUG ((EFI_D_INFO, "  RootBridge Path: %s\n", DevicePathStr));
        DEBUG ((EFI_D_INFO, "           Bus: %lx - %lx\n", RootBridge->Bus.Base, RootBridge->Bus.Limit));
        DEBUG ((EFI_D_INFO, "            Io: %lx - %lx  (Size = %lx)\n",
          RootBridge->Io.Base, RootBridge->Io.Limit, RootBridge->Io.Limit - RootBridge->Io.Base + 1));
        DEBUG ((EFI_D_INFO, "           Mem: %lx - %lx  (Size = %lx)\n", RootBridge->Mem.Base, RootBridge->Mem.Limit,
          RootBridge->Mem.Limit ? (RootBridge->Mem.Limit - RootBridge->Mem.Base + 1) : 0));
        DEBUG ((EFI_D_INFO, "    MemAbove4G: %lx - %lx  (Size = %lx)\n", RootBridge->MemAbove4G.Base, RootBridge->MemAbove4G.Limit,
          RootBridge->MemAbove4G.Limit ? (RootBridge->MemAbove4G.Limit - RootBridge->MemAbove4G.Base + 1) : 0));
        DEBUG ((EFI_D_INFO, "          PMem: %lx - %lx  (Size = %lx)\n", RootBridge->PMem.Base, RootBridge->PMem.Limit,
          RootBridge->PMem.Limit ? (RootBridge->PMem.Limit - RootBridge->PMem.Base + 1) : 0));
        DEBUG ((EFI_D_INFO, "   PMemAbove4G: %lx - %lx  (Size = %lx)\n", RootBridge->PMemAbove4G.Base, RootBridge->PMemAbove4G.Limit,
          RootBridge->PMemAbove4G.Limit ? (RootBridge->PMemAbove4G.Limit - RootBridge->PMemAbove4G.Base + 1) : 0));

        RootBridge++;
        DevicePath++;
        RootBridgeAllocCount--;
        RootBridgeInSocket++;
      }
    }
  }

  *Count = RootBridgeCount;
  return FirstRootBridge;
}

/**
  Free the root bridge instances array returned from PciHostBridgeGetRootBridges().

  @param Bridges The root bridge instances array.
  @param Count   The count of the array.
**/
VOID
EFIAPI
PciHostBridgeFreeRootBridges (
  PCI_ROOT_BRIDGE *Bridges,
  UINTN           Count
  )
{
  FreePool (Bridges);
}

/**
  Inform the platform that the resource conflict happens.

  @param HostBridgeHandle Handle of the Host Bridge.
  @param Configuration    Pointer to PCI I/O and PCI memory resource
                          descriptors. The Configuration contains the resources
                          for all the root bridges. The resource for each root
                          bridge is terminated with END descriptor and an
                          additional END is appended indicating the end of the
                          entire resources. The resource descriptor field
                          values follow the description in
                          EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL
                          .SubmitResources().
**/
VOID
EFIAPI
PciHostBridgeResourceConflict (
  EFI_HANDLE                        HostBridgeHandle,
  VOID                              *Configuration
  )
{
  /*
  EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *Descriptor;
  UINTN                             RootBridgeIndex;
  UINT32                            TokenSize;
  UINT64                            TOM;
  UINT64                            TomToken;
  EFI_STATUS                        Status;
  FABRIC_RESOURCE_FOR_EACH_RB           AmdResourceSize;
  AMD_FABRIC_TOPOLOGY_SERVICES2_PROTOCOL *FabricTopology;
  FABRIC_RESOURCE_MANAGER_PROTOCOL      *FabricResource;
  ROOT_BRIDGE_RESOURCE                  CurRbRes;
  FABRIC_ADDR_SPACE_SIZE                SpaceStatus;
  AMD_APCB_SERVICE_PROTOCOL             *ApcbDxeServiceProtocol;

  // Zero CurRbRes
  SetMem (&CurRbRes, sizeof (ROOT_BRIDGE_RESOURCE), 0);

  Status = gBS->LocateProtocol (&gAmdFabricTopologyServices2ProtocolGuid, NULL, (VOID **)&FabricTopology);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return;
  }
  Status = gBS->LocateProtocol (&gAmdFabricResourceManagerServicesProtocolGuid, NULL, (VOID **)&FabricResource);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return;
  }

  // Get current resource for each RootBridge
  RootBridgeIndex = 0;
  Descriptor = (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *) Configuration;
  while (Descriptor->Desc == ACPI_ADDRESS_SPACE_DESCRIPTOR) {
    for (; Descriptor->Desc == ACPI_ADDRESS_SPACE_DESCRIPTOR; Descriptor++) {
      switch (Descriptor->ResType) {
      case ACPI_ADDRESS_SPACE_TYPE_MEM:
        if (Descriptor->AddrSpaceGranularity == 32) {
          if (Descriptor->SpecificFlag == EFI_ACPI_MEMORY_RESOURCE_SPECIFIC_FLAG_CACHEABLE_PREFETCHABLE) {
            // PMem32
            CurRbRes.PMem32[RootBridgeIndex].Base = Descriptor->AddrRangeMin;
            CurRbRes.PMem32[RootBridgeIndex].Length = Descriptor->AddrLen;
            CurRbRes.PMem32[RootBridgeIndex].Alignment = Descriptor->AddrRangeMax;
          } else {
            // Mem32
            CurRbRes.Mem32[RootBridgeIndex].Base = Descriptor->AddrRangeMin;
            CurRbRes.Mem32[RootBridgeIndex].Length = Descriptor->AddrLen;
            CurRbRes.Mem32[RootBridgeIndex].Alignment = Descriptor->AddrRangeMax;
          }
        } else {
          if (Descriptor->SpecificFlag == EFI_ACPI_MEMORY_RESOURCE_SPECIFIC_FLAG_CACHEABLE_PREFETCHABLE) {
            // PMem64
            CurRbRes.PMem64[RootBridgeIndex].Base = Descriptor->AddrRangeMin;
            CurRbRes.PMem64[RootBridgeIndex].Length = Descriptor->AddrLen;
            CurRbRes.PMem64[RootBridgeIndex].Alignment = Descriptor->AddrRangeMax;
          } else {
            // Mem64
            CurRbRes.Mem64[RootBridgeIndex].Base = Descriptor->AddrRangeMin;
            CurRbRes.Mem64[RootBridgeIndex].Length = Descriptor->AddrLen;
            CurRbRes.Mem64[RootBridgeIndex].Alignment = Descriptor->AddrRangeMax;
          }
        }
        break;
      case ACPI_ADDRESS_SPACE_TYPE_IO:
        // IO
        CurRbRes.Io[RootBridgeIndex].Base = Descriptor->AddrRangeMin;
        CurRbRes.Io[RootBridgeIndex].Length = Descriptor->AddrLen;
        CurRbRes.Io[RootBridgeIndex].Alignment = Descriptor->AddrRangeMax;
        break;
      case ACPI_ADDRESS_SPACE_TYPE_BUS:
        // PCI BUS
        CurRbRes.PciBus[RootBridgeIndex].Base = Descriptor->AddrRangeMin;
        CurRbRes.PciBus[RootBridgeIndex].Length = Descriptor->AddrLen;
        CurRbRes.PciBus[RootBridgeIndex].Alignment = Descriptor->AddrRangeMax;
        break;
      default:
        break;
      }
    }
    RootBridgeIndex++;
    //
    // Skip the END descriptor for root bridge
    //
    ASSERT (Descriptor->Desc == ACPI_END_TAG_DESCRIPTOR);
    Descriptor = (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *) (
                   (EFI_ACPI_END_TAG_DESCRIPTOR *)Descriptor + 1
                   );
  }

  // Init AmdResourceSize
  AmdInitResourceSize (&CurRbRes, &AmdResourceSize, FabricTopology);

  //
  if (FabricResource->FabricReallocateResourceForEachRb (FabricResource, &AmdResourceSize, &SpaceStatus) == EFI_SUCCESS) {
    gRT->ResetSystem (EfiResetWarm, EFI_SUCCESS, 0, NULL);
  } else {
    if (SpaceStatus.MmioSizeBelow4GReqInc != 0) {
      //lower TOM
      TOM = AsmReadMsr64 (0xC001001A);
      TOM = (TOM - SpaceStatus.MmioSizeBelow4GReqInc) & 0xF0000000; // TOM must be 256MB alignment

      gBS->LocateProtocol (&gAmdApcbDxeServiceProtocolGuid, NULL, (VOID **)&ApcbDxeServiceProtocol);
      TomToken = TOM >> 24; // Set APCB_TOKEN_CONFIG_DF_BOTTOMIO to TOM[31:24]
      TokenSize = sizeof(UINT8);
      Status = ApcbDxeServiceProtocol->ApcbSetConfigParameter (ApcbDxeServiceProtocol, APCB_ID_CONFIG_DF_BOTTOMIO, &TokenSize, &TomToken);
      Status = ApcbDxeServiceProtocol->ApcbFlushData (ApcbDxeServiceProtocol);
    }
    if (SpaceStatus.MmioSizeAbove4GReqInc != 0) {
      // Normally, MMIO above 4G should be enough. In case we run out of resource, put a debug message here
      DEBUG ((EFI_D_INFO, "  No enough MMIO space above 4G, need 0x%X more\n", SpaceStatus.MmioSizeAbove4GReqInc));
    }
    gRT->ResetSystem (EfiResetWarm, EFI_SUCCESS, 0, NULL);
  }*/
}

EFI_STATUS
AmdInitResourceSize (
  IN       ROOT_BRIDGE_RESOURCE                   *CurRbRes,
  IN OUT   DFX_FABRIC_RESOURCE_FOR_EACH_RB        *FabricResourceSize
  )
{
  /*
  UINTN  i;
  UINTN  SocketCount;
  UINTN  SocketLoop;
  UINTN  DiePerSocket;
  UINTN  DieLoop;
  UINTN  RbPerDie;
  UINTN  RbLoop;
  UINTN  PciBusBase[MAX_SOCKETS_SUPPORTED][MAX_RBS_PER_SOCKET];
  UINTN  PciBusLimit[MAX_SOCKETS_SUPPORTED][MAX_RBS_PER_SOCKET];
  BOOLEAN PciBusReassign;

  FabricTopology->GetSystemInfo (FabricTopology, &SocketCount, NULL, NULL, NULL, NULL);
  DiePerSocket = 1;
  RbPerDie = 1;
  // Zero out AmdResourceSize
  SetMem (AmdResourceSize, sizeof (FABRIC_RESOURCE_FOR_EACH_RB), 0);

  // Set PciBusBase to 0xFF
  SetMem (PciBusBase, (sizeof (UINTN) * MAX_SOCKETS_SUPPORTED * MAX_RBS_PER_SOCKET), 0xFF);

  // Zero out PciBusLimit
  SetMem (PciBusLimit, (sizeof (UINTN) * MAX_SOCKETS_SUPPORTED * MAX_RBS_PER_SOCKET), 0);

  // Get PciBusLimit for each RootBridge
  for (SocketLoop = 0; SocketLoop < SocketCount; SocketLoop++) {
    FabricTopology->GetProcessorInfo (FabricTopology, SocketLoop, &DiePerSocket, NULL);
    for (DieLoop = 0; DieLoop < DiePerSocket; DieLoop++) {
      FabricTopology->GetDieInfo (FabricTopology, SocketLoop, DieLoop, &RbPerDie, NULL, NULL);
      for (RbLoop = 0; RbLoop < RbPerDie; RbLoop++) {
        FabricTopology->GetRootBridgeInfo (FabricTopology, SocketLoop, DieLoop, RbLoop, NULL, &PciBusBase[SocketLoop][DieLoop * RbPerDie + RbLoop], &PciBusLimit[SocketLoop][DieLoop * RbPerDie + RbLoop], NULL, NULL, NULL);
      }
    }
  }
  PciBusReassign = FALSE;
  // By comparing CurRbRes->PciBusLimit with PciBusLimit get from FabricTopology, we could know Mem and IO resource is on which socket and which RootBridge
  for (i = 0; i < (MAX_SOCKETS_SUPPORTED * MAX_RBS_PER_SOCKET); i++) {
    if (CurRbRes->PciBus[i].Length != 0) {
      for (SocketLoop = 0; SocketLoop < SocketCount; SocketLoop++) {
        for (RbLoop = 0; RbLoop < (DiePerSocket * RbPerDie); RbLoop++) {
          if ((PciBusBase[SocketLoop][RbLoop] != 0xFF) &&
              (CurRbRes->PciBus[i].Base == PciBusBase[SocketLoop][RbLoop])) {
            // Mem32
            AmdResourceSize->NonPrefetchableMmioSizeBelow4G[SocketLoop][RbLoop].Base = CurRbRes->Mem32[i].Base;
            AmdResourceSize->NonPrefetchableMmioSizeBelow4G[SocketLoop][RbLoop].Size = CurRbRes->Mem32[i].Length;
            AmdResourceSize->NonPrefetchableMmioSizeBelow4G[SocketLoop][RbLoop].Alignment = CurRbRes->Mem32[i].Alignment;
            // PMem32
            AmdResourceSize->PrefetchableMmioSizeBelow4G[SocketLoop][RbLoop].Base = CurRbRes->PMem32[i].Base;
            AmdResourceSize->PrefetchableMmioSizeBelow4G[SocketLoop][RbLoop].Size = CurRbRes->PMem32[i].Length;
            AmdResourceSize->PrefetchableMmioSizeBelow4G[SocketLoop][RbLoop].Alignment = CurRbRes->PMem32[i].Alignment;
            // Mem64
            AmdResourceSize->NonPrefetchableMmioSizeAbove4G[SocketLoop][RbLoop].Base = CurRbRes->Mem64[i].Base;
            AmdResourceSize->NonPrefetchableMmioSizeAbove4G[SocketLoop][RbLoop].Size = CurRbRes->Mem64[i].Length;
            AmdResourceSize->NonPrefetchableMmioSizeAbove4G[SocketLoop][RbLoop].Alignment = CurRbRes->Mem64[i].Alignment;
            // PMem64
            AmdResourceSize->PrefetchableMmioSizeAbove4G[SocketLoop][RbLoop].Base = CurRbRes->PMem64[i].Base;
            AmdResourceSize->PrefetchableMmioSizeAbove4G[SocketLoop][RbLoop].Size = CurRbRes->PMem64[i].Length;
            AmdResourceSize->PrefetchableMmioSizeAbove4G[SocketLoop][RbLoop].Alignment = CurRbRes->PMem64[i].Alignment;
            // IO
            AmdResourceSize->IO[SocketLoop][RbLoop].Base = CurRbRes->Io[i].Base;
            AmdResourceSize->IO[SocketLoop][RbLoop].Size = CurRbRes->Io[i].Length;
            AmdResourceSize->IO[SocketLoop][RbLoop].Alignment = CurRbRes->Io[i].Alignment;
            // PCI BUS
            // Compare with currently allocated number of PCI bus
            // dont downgrade/reduced the existing number of PCI bus
            if (CurRbRes->PciBus[i].Length <= (PciBusLimit[SocketLoop][RbLoop] - PciBusBase[SocketLoop][RbLoop] + 1)) {
              AmdResourceSize->PciBusNumber[SocketLoop][RbLoop] = (UINT16) (PciBusLimit[SocketLoop][RbLoop] - PciBusBase[SocketLoop][RbLoop] + 1);
            } else {
              DEBUG ((
                DEBUG_INFO,
                "Socket[%d]:RB[%d] number of PCI Bus allocated = 0x%x, requires 0x%x. reassigning...\n",
                SocketLoop,
                RbLoop,
                PciBusLimit[SocketLoop][RbLoop] - PciBusBase[SocketLoop][RbLoop] + 1,
                CurRbRes->PciBus[i].Length
                ));
              PciBusReassign = TRUE;
            }
          }
        }
      }
    }
  }
  if (PciBusReassign) {
    for (i = 0; i < (MAX_SOCKETS_SUPPORTED * MAX_RBS_PER_SOCKET); i++) {
      if (CurRbRes->PciBus[i].Length != 0) {
        for (SocketLoop = 0; SocketLoop < SocketCount; SocketLoop++) {
          for (RbLoop = 0; RbLoop < (DiePerSocket * RbPerDie); RbLoop++) {
            if ((PciBusBase[SocketLoop][RbLoop] != 0xFF) &&
                (CurRbRes->PciBus[i].Base == PciBusBase[SocketLoop][RbLoop])) {
              // PCI BUS
              AmdResourceSize->PciBusNumber[SocketLoop][RbLoop] = (UINT16) CurRbRes->PciBus[i].Length;
            }
          }
        }
      }
    }
  }
*/
  return EFI_SUCCESS;
}
