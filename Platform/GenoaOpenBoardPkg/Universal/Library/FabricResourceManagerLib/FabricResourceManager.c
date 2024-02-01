/*****************************************************************************
 *
 * Copyright (C) 2020-2024 Advanced Micro Devices, Inc. All rights reserved.
 *
 *****************************************************************************/

/**
 * Base Fabric MMIO map manager Lib implementation for Genoa
 */

#include <Library/FabricResourceManagerLib.h>
#include <SilHob.h>
#include <Include/Library/HobLib.h>


/**
 * This service retrieves information about the overall system with respect to data fabric.
 *
 * @param[out] NumberOfInstalledProcessors    Pointer to the total number of populated
 *                                            processor sockets in the system.
 * @param[out] TotalNumberOfDie               Pointer to the total number of die in the system.
 * @param[out] TotalNumberOfRootBridges       Pointer to the total number of root PCI bridges in
 *                                            the system.
 * @param[out] SystemFchRootBridgeLocation    System primary FCH location.
 * @param[out] SystemSmuRootBridgeLocation    System primary SMU location.
 *
 * @retval EFI_SUCCESS                        The system topology information was successfully retrieved.
 * @retval EFI_INVALID_PARAMETER              All output parameter pointers are NULL.
 *
 **/
EFI_STATUS
EFIAPI
FabricGetSystemInfo (
  OUT   UINT32                                 *NumberOfInstalledProcessors,
  OUT   UINT32                                 *TotalNumberOfDie,
  OUT   UINT32                                 *TotalNumberOfRootBridges,
  OUT   ROOT_BRIDGE_LOCATION                   *SystemFchRootBridgeLocation,
  OUT   ROOT_BRIDGE_LOCATION                   *SystemSmuRootBridgeLocation
  )
{

  if ((NumberOfInstalledProcessors == NULL) && (TotalNumberOfDie == NULL) && (TotalNumberOfRootBridges == NULL) &&
      (SystemFchRootBridgeLocation == NULL) && (SystemSmuRootBridgeLocation == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  // Get Number of Socket information of the overall system.
  DfGetSystemInfo (
                      NumberOfInstalledProcessors,
                      TotalNumberOfDie,
                      TotalNumberOfRootBridges,
                      SystemFchRootBridgeLocation,
                      SystemSmuRootBridgeLocation);

  return EFI_SUCCESS;
}
/*---------------------------------------------------------------------------------------*/
/**
 * FabricGetAvailableResource
 *
 * Get available DF resource (MMIO/IO) for each Rb
 *
 * @param[in, out]    ResourceForEachRb     Avaiable DF resource (MMIO/IO) for each Rb
 *
 * @retval            EFI_SUCCESS           Success to get available resource
 *                    EFI_ABORTED           Cannot get information of MMIO or IO
 */
EFI_STATUS
FabricGetAvailableResource (
  IN  DFX_FABRIC_RESOURCE_FOR_EACH_RB    *ResourceForEachRb
  )
{

  DFX_RCMGR_INPUT_BLK       *RcMgrData;
  DFX_FABRIC_MMIO_MANAGER   *FabricMmioManager;
  DFX_FABRIC_IO_MANAGER     *FabricIoManager;
  FABRIC_MMIO_REGION        *MmioRegion;
  FABRIC_IO_REGION          *IoRegion;
  UINT8                     SocIndex;
  UINT8                     HbIndex;
  UINT32                    SocketNumber;
  UINT32                    RootBridgePerSocket;
  UINT32                    RbPerDie;
  UINT8                     TempSocket;
  UINT8                     TempRb;
  UINT32                    PciBase;
  UINT32                    PciLimit;

  DEBUG ((DEBUG_INFO, "%a Entry..\n", __FUNCTION__));
  RcMgrData = (DFX_RCMGR_INPUT_BLK *)SilFindStructure(SilId_RcManager,  0);
  
  if (RcMgrData == NULL) {
    DEBUG ((DEBUG_ERROR, "%a  RcMgrData Not Found!!!..\n", __FUNCTION__));
    return EFI_NOT_FOUND; // Could not find the IP input block
  }

  FabricMmioManager = &RcMgrData->MmioRcMgr;
  FabricIoManager = &RcMgrData->IoRcMgr;
  DEBUG ((DEBUG_ERROR, " RcMgrData 0x%x FabricMmioManager 0x%x  FabricIoManager 0x%x\n", RcMgrData, FabricMmioManager, FabricIoManager));
  if ((FabricMmioManager == NULL) || (FabricIoManager == NULL)) {
    DEBUG ((DEBUG_ERROR, "%a  FabricMmioManager ||  FabricIoManager Not Found!!!..\n", __FUNCTION__));
    ASSERT (FALSE);
    return EFI_NOT_FOUND;
  }
  SocketNumber = DfXGetNumberOfProcessorsPresent ();
  RootBridgePerSocket = DfXGetNumberOfRootBridgesOnSocket (0);
  RbPerDie = DfXGetNumberOfRootBridgesOnDie (0);
  for (SocIndex = 0; SocIndex < MAX_SOCKETS_SUPPORTED; SocIndex++) {
    for (HbIndex = 0; HbIndex < DFX_MAX_HOST_BRIDGES_PER_SOCKET; HbIndex++) {
      ResourceForEachRb->NonPrefetchableMmioSizeAbove4G[SocIndex][HbIndex].Alignment = 0;
      ResourceForEachRb->PrefetchableMmioSizeAbove4G[SocIndex][HbIndex].Alignment = 0;
      ResourceForEachRb->NonPrefetchableMmioSizeBelow4G[SocIndex][HbIndex].Alignment = 0;
      ResourceForEachRb->PrefetchableMmioSizeBelow4G[SocIndex][HbIndex].Alignment = 0;

      if ((SocIndex >= SocketNumber) || (HbIndex >= RootBridgePerSocket)) {
        // Rb does not exist
        ResourceForEachRb->NonPrefetchableMmioSizeAbove4G[SocIndex][HbIndex].Size = 0;
        ResourceForEachRb->NonPrefetchableMmioSizeBelow4G[SocIndex][HbIndex].Size = 0;
        ResourceForEachRb->PrefetchableMmioSizeAbove4G[SocIndex][HbIndex].Size = 0;
        ResourceForEachRb->PrefetchableMmioSizeBelow4G[SocIndex][HbIndex].Size = 0;
        ResourceForEachRb->NonPrefetchableMmioSizeAbove4G[SocIndex][HbIndex].Base = 0;
        ResourceForEachRb->NonPrefetchableMmioSizeBelow4G[SocIndex][HbIndex].Base = 0;
        ResourceForEachRb->PrefetchableMmioSizeAbove4G[SocIndex][HbIndex].Base = 0;
        ResourceForEachRb->PrefetchableMmioSizeBelow4G[SocIndex][HbIndex].Base = 0;

        ResourceForEachRb->IO[SocIndex][HbIndex].Size = 0;
        ResourceForEachRb->IO[SocIndex][HbIndex].Base = 0;
      } else {
        DEBUG ((DEBUG_INFO,"\n---Socket%x Rb%x available resource---\n", SocIndex, HbIndex));
        if ((FabricMmioManager->AllocateMmioAbove4GOnThisRb[SocIndex][HbIndex]) == FALSE) {
          // No MMIO on this Rb
          ResourceForEachRb->NonPrefetchableMmioSizeAbove4G[SocIndex][HbIndex].Size = 0;
          ResourceForEachRb->PrefetchableMmioSizeAbove4G[SocIndex][HbIndex].Size = 0;
          ResourceForEachRb->NonPrefetchableMmioSizeAbove4G[SocIndex][HbIndex].Base = 0;
          ResourceForEachRb->PrefetchableMmioSizeAbove4G[SocIndex][HbIndex].Base = 0;
        } else {
          // Only report prefetchable & non-prefetchable MMIO size
          MmioRegion = &FabricMmioManager->MmioRegionAbove4G[SocIndex][HbIndex];
          ResourceForEachRb->NonPrefetchableMmioSizeAbove4G[SocIndex][HbIndex].Size = MmioRegion->SizeNonPrefetch - MmioRegion->UsedSizeNonPrefetch;
          ResourceForEachRb->PrefetchableMmioSizeAbove4G[SocIndex][HbIndex].Size = MmioRegion->SizePrefetch - MmioRegion->UsedSizePrefetch;
          ResourceForEachRb->NonPrefetchableMmioSizeAbove4G[SocIndex][HbIndex].Base = MmioRegion->BaseNonPrefetch + MmioRegion->UsedSizeNonPrefetch;
          ResourceForEachRb->PrefetchableMmioSizeAbove4G[SocIndex][HbIndex].Base = MmioRegion->BasePrefetch + MmioRegion->UsedSizePrefetch;
          DEBUG ((DEBUG_INFO, "  MMIO above 4G\n    non-prefetchable 0x%lX ~ 0x%lX size 0x%lX align %x\n", 
                ResourceForEachRb->NonPrefetchableMmioSizeAbove4G[SocIndex][HbIndex].Base, 
                (ResourceForEachRb->NonPrefetchableMmioSizeAbove4G[SocIndex][HbIndex].Base + ResourceForEachRb->NonPrefetchableMmioSizeAbove4G[SocIndex][HbIndex].Size), 
                ResourceForEachRb->NonPrefetchableMmioSizeAbove4G[SocIndex][HbIndex].Size, 
                ResourceForEachRb->NonPrefetchableMmioSizeAbove4G[SocIndex][HbIndex].Alignment));
          DEBUG ((DEBUG_INFO, "        prefetchable 0x%lX ~ 0x%lX size 0x%lX align %x\n", 
                ResourceForEachRb->PrefetchableMmioSizeAbove4G[SocIndex][HbIndex].Base, 
                (ResourceForEachRb->PrefetchableMmioSizeAbove4G[SocIndex][HbIndex].Base + ResourceForEachRb->PrefetchableMmioSizeAbove4G[SocIndex][HbIndex].Size), 
                ResourceForEachRb->PrefetchableMmioSizeAbove4G[SocIndex][HbIndex].Size, 
                ResourceForEachRb->PrefetchableMmioSizeAbove4G[SocIndex][HbIndex].Alignment));
        }

        if ((FabricMmioManager->AllocateMmioBelow4GOnThisRb[SocIndex][HbIndex]) == FALSE) {
          // No MMIO on this Rb
          ResourceForEachRb->NonPrefetchableMmioSizeBelow4G[SocIndex][HbIndex].Size = 0;
          ResourceForEachRb->PrefetchableMmioSizeBelow4G[SocIndex][HbIndex].Size = 0;
          ResourceForEachRb->NonPrefetchableMmioSizeBelow4G[SocIndex][HbIndex].Base = 0;
          ResourceForEachRb->PrefetchableMmioSizeBelow4G[SocIndex][HbIndex].Base = 0;
        } else {
          // Only report prefetchable & non-prefetchable MMIO size
          MmioRegion = &FabricMmioManager->MmioRegionBelow4G[SocIndex][HbIndex];
          ResourceForEachRb->NonPrefetchableMmioSizeBelow4G[SocIndex][HbIndex].Size = MmioRegion->SizeNonPrefetch - MmioRegion->UsedSizeNonPrefetch;
          ResourceForEachRb->PrefetchableMmioSizeBelow4G[SocIndex][HbIndex].Size =  MmioRegion->SizePrefetch - MmioRegion->UsedSizePrefetch;
          ResourceForEachRb->NonPrefetchableMmioSizeBelow4G[SocIndex][HbIndex].Base = MmioRegion->BaseNonPrefetch + MmioRegion->UsedSizeNonPrefetch;
          ResourceForEachRb->PrefetchableMmioSizeBelow4G[SocIndex][HbIndex].Base =  MmioRegion->BasePrefetch + MmioRegion->UsedSizePrefetch;
          DEBUG ((DEBUG_INFO, "  MMIO below 4G\n    non-prefetchable 0x%lX ~ 0x%lX size 0x%lX align %x\n", 
                ResourceForEachRb->NonPrefetchableMmioSizeBelow4G[SocIndex][HbIndex].Base, (ResourceForEachRb->NonPrefetchableMmioSizeBelow4G[SocIndex][HbIndex].Base + ResourceForEachRb->NonPrefetchableMmioSizeBelow4G[SocIndex][HbIndex].Size), 
                ResourceForEachRb->NonPrefetchableMmioSizeBelow4G[SocIndex][HbIndex].Size, 
                ResourceForEachRb->NonPrefetchableMmioSizeBelow4G[SocIndex][HbIndex].Alignment));
          DEBUG ((DEBUG_INFO, "        prefetchable 0x%lX ~ 0x%lX size 0x%lX align %x\n", 
                ResourceForEachRb->PrefetchableMmioSizeBelow4G[SocIndex][HbIndex].Base, (ResourceForEachRb->PrefetchableMmioSizeBelow4G[SocIndex][HbIndex].Base + ResourceForEachRb->PrefetchableMmioSizeBelow4G[SocIndex][HbIndex].Size), 
                ResourceForEachRb->PrefetchableMmioSizeBelow4G[SocIndex][HbIndex].Size, 
                ResourceForEachRb->PrefetchableMmioSizeBelow4G[SocIndex][HbIndex].Alignment));
        }

        IoRegion = &FabricIoManager->IoRegion[SocIndex][HbIndex];
        ResourceForEachRb->IO[SocIndex][HbIndex].Base = IoRegion->IoBase;
        ResourceForEachRb->IO[SocIndex][HbIndex].Size = IoRegion->IoSize - IoRegion->IoUsed;
        DEBUG ((DEBUG_INFO, "  IO 0x%lX ~ 0x%lX size 0x%X\n", ResourceForEachRb->IO[SocIndex][HbIndex].Base, 
              (ResourceForEachRb->IO[SocIndex][HbIndex].Base + ResourceForEachRb->IO[SocIndex][HbIndex].Size), 
              ResourceForEachRb->IO[SocIndex][HbIndex].Size));

        PciBase = DfXGetHostBridgeBusBase (SocIndex, (HbIndex % RbPerDie));
        PciLimit = DfXGetHostBridgeBusLimit (SocIndex, (HbIndex % RbPerDie));
        ResourceForEachRb->PciBusNumber[SocIndex][HbIndex] = (UINT16) (PciLimit - PciBase + 1);
        DEBUG ((DEBUG_INFO, "  PCI bus 0x%lX ~ 0x%lX size 0x%X\n", 
              PciBase, PciLimit, ResourceForEachRb->PciBusNumber[SocIndex][HbIndex]));
      }
    }
  }

  // RootBridge0's 2nd MMIO
  if (FabricMmioManager->PrimaryRbHas2ndMmioBelow4G) {
    TempSocket = (FabricMmioManager->PrimaryRb2ndMmioPairBelow4G >> 4) & 0xF;
    TempRb = FabricMmioManager->PrimaryRb2ndMmioPairBelow4G & 0xF;
    MmioRegion = &FabricMmioManager->MmioRegionBelow4G[TempSocket][TempRb];

    ResourceForEachRb->PrimaryRbSecondNonPrefetchableMmioSizeBelow4G.Size = MmioRegion->SizeNonPrefetch - MmioRegion->UsedSizeNonPrefetch;
    ResourceForEachRb->PrimaryRbSecondPrefetchableMmioSizeBelow4G.Size = MmioRegion->SizePrefetch - MmioRegion->UsedSizePrefetch;
    ResourceForEachRb->PrimaryRbSecondNonPrefetchableMmioSizeBelow4G.Base = MmioRegion->BaseNonPrefetch + MmioRegion->UsedSizeNonPrefetch;
    ResourceForEachRb->PrimaryRbSecondPrefetchableMmioSizeBelow4G.Base = MmioRegion->BasePrefetch + MmioRegion->UsedSizePrefetch;
    DEBUG ((DEBUG_INFO, "  RootBridge0's 2nd MMIO\n    non-prefetch 0x%lX ~ 0x%lX size 0x%lX align %x MMIO\n",
           ResourceForEachRb->PrimaryRbSecondNonPrefetchableMmioSizeBelow4G.Base, 
           (ResourceForEachRb->PrimaryRbSecondNonPrefetchableMmioSizeBelow4G.Base + ResourceForEachRb->PrimaryRbSecondNonPrefetchableMmioSizeBelow4G.Size), 
           ResourceForEachRb->PrimaryRbSecondNonPrefetchableMmioSizeBelow4G.Size, 
           ResourceForEachRb->PrimaryRbSecondNonPrefetchableMmioSizeBelow4G.Alignment));
    DEBUG ((DEBUG_INFO, "      prefetchable 0x%lX ~ 0x%lX size 0x%lX align %x MMIO\n", 
          ResourceForEachRb->PrimaryRbSecondPrefetchableMmioSizeBelow4G.Base, 
          (ResourceForEachRb->PrimaryRbSecondPrefetchableMmioSizeBelow4G.Base + ResourceForEachRb->PrimaryRbSecondPrefetchableMmioSizeBelow4G.Size), 
          ResourceForEachRb->PrimaryRbSecondPrefetchableMmioSizeBelow4G.Size, 
          ResourceForEachRb->PrimaryRbSecondPrefetchableMmioSizeBelow4G.Alignment));
  } else {
    ResourceForEachRb->PrimaryRbSecondNonPrefetchableMmioSizeBelow4G.Size = 0;
    ResourceForEachRb->PrimaryRbSecondPrefetchableMmioSizeBelow4G.Size = 0;
    ResourceForEachRb->PrimaryRbSecondNonPrefetchableMmioSizeBelow4G.Base = 0;
    ResourceForEachRb->PrimaryRbSecondPrefetchableMmioSizeBelow4G.Base = 0;
  }
  ResourceForEachRb->PrimaryRbSecondNonPrefetchableMmioSizeBelow4G.Alignment = 0;
  ResourceForEachRb->PrimaryRbSecondPrefetchableMmioSizeBelow4G.Alignment = 0;

  FabricIoManager->GlobalCtrl = FALSE;

  return EFI_SUCCESS;
}


/*---------------------------------------------------------------------------------------*/
/**
 * FabricResourceManagerConstructor
 *
 * Set Sil Memory Base
 *
 * @param[in, out]    VOID
 *
 * @retval            EFI_SUCCESS           Success to set Sil Memory baase
 */
EFI_STATUS
EFIAPI
FabricResourceManagerLibConstructor (
  VOID
  )
{
  EFI_HOB_GUID_TYPE  *Hob;
  UINT64             DataPointer;
  SIL_DATA_HOB       *SilDataHob;

  // the following code is required to initialize API pointers for openSIL IPs
  Hob = (EFI_HOB_GUID_TYPE *)GetFirstGuidHob(&gPeiOpenSilDataHobGuid);
  Hob++;
  SilDataHob = (SIL_DATA_HOB *)Hob;
  DataPointer = (UINT64)(SilDataHob->SilDataPointer);

  SilSetMemoryBase ((VOID*)DataPointer);     // Set the global var

  return EFI_SUCCESS;
}
  