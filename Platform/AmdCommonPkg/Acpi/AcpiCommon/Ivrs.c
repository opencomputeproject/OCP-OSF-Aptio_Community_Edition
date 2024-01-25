/*****************************************************************************
 *
 * Copyright (C) 2021-2023 Advanced Micro Devices, Inc. All rights reserved.
 *
 *****************************************************************************/

#include <AcpiCommon.h>
#include <Ivrs.h>
#include <Library/PcieResourcesLib.h>
#include <NBIO/NbioIp2Ip.h>
#include <CommonLib/Mmio.h>

UINT8                               mGnbIoApicIdBase;
UINT8                               mFchIoApicIdBase;

IOMMU_IVRS_HEADER IvrsHeader = {
  {'I', 'V', 'R', 'S'},
  sizeof (IOMMU_IVRS_HEADER),
  2,
  0,
  {'A', 'M', 'D', ' ', ' ', 0},
  {'A', 'M', 'D', 'I', 'O', 'M', 'M', 'U'},
  1,
  {'A','M','D',' '},
  1,
  0,
  0
};

MPDMA_IVRS_MAP RbIvrsMap[] = {
  {0, 2, IVRS_DEVICE_UIDINT(0x0004, 0x30, "AMDI0095", 0, 0)},
  {0, 0, IVRS_DEVICE_UIDINT(0x0004, 0x30, "AMDI0096", 0, 1)},
  {1, 2, IVRS_DEVICE_UIDINT(0x0004, 0x30, "AMDI0096", 0, 2)},
  {1, 0, IVRS_DEVICE_UIDINT(0x0004, 0x30, "AMDI0096", 0, 3)}
};

IVRS_DEVICE_LIST  OemIvrsDeviceList[] = {                                   ///< Device list starts with this macro
    IVRS_DEVICE_UIDSTR (0x00A5, 0x40, "AMDI0020", 0, 9, "\\_SB.FUR0"),      ///< Use this macro to identify a device with a "string" _UID
    IVRS_DEVICE_UIDSTR (0x00A5, 0x40, "AMDI0020", 0, 9, "\\_SB.FUR1"),      ///< Use this macro to identify a device with a "string" _UID
    IVRS_DEVICE_UIDSTR (0x00A5, 0x40, "AMDI0020", 0, 9, "\\_SB.FUR2"),      ///< Use this macro to identify a device with a "string" _UID
    IVRS_DEVICE_UIDSTR (0x00A5, 0x40, "AMDI0020", 0, 9, "\\_SB.FUR3"),      ///< Use this macro to identify a device with a "string" _UID
                                                                            ///< Additional entries go here
    {0xFFFF}
};                                         

/**
 * Dump buffer to HDTOUT
 *
 *
 * @param[in]     Buffer          Buffer pointer
 * @param[in]     Count           Count of data elements
 * @param[in]     DataWidth       DataWidth 1 - Byte; 2 - Word; 3 - DWORD; 4 - QWORD
 * @param[in]     LineWidth       Number of data item per line
 */
VOID
DebugDumpBuffer (
  IN       VOID             *Buffer,
  IN       UINT32           Count,
  IN       UINT8            DataWidth,
  IN       UINT8            LineWidth
  )
{
  UINT32  Index;
  UINT32  DataItemCount;
  ASSERT (LineWidth != 0);
  ASSERT (DataWidth >= 1 && DataWidth <= 4);
  DataItemCount = 0;
  for (Index = 0; Index < Count; ) {
    switch (DataWidth) {
    case 1:
      DEBUG ((DEBUG_INFO, "%02x ", *((UINT8 *) Buffer + Index)));
      Index += 1;
      break;
    case 2:
      DEBUG ((DEBUG_INFO, "%04x ", *(UINT16 *) ((UINT8 *) Buffer + Index)));
      Index += 2;
      break;
    case 3:
      DEBUG ((DEBUG_INFO, "%08x ", *(UINT32 *) ((UINT8 *) Buffer + Index)));
      Index += 4;
      break;
    case 4:
      DEBUG ((DEBUG_INFO, "%08x%08", *(UINT32 *) ((UINT8 *) Buffer + Index), *(UINT32 *) ((UINT8 *) Buffer + Index + 4)));
      Index += 8;
      break;
    default:
      DEBUG ((DEBUG_INFO, "ERROR! Incorrect Data Width\n"));
      return;
    }
    if (++DataItemCount >= LineWidth) {
      DEBUG ((DEBUG_INFO, "\n"));
      DataItemCount = 0;
    }
  }
}

/**
 * Check if IOMMU unit present and enabled
 * @param[in]  GnbHandle       Gnb handle
 * @retval     BOOLEAN
 *
 */
BOOLEAN
CheckIommuPresent (
  IN       GNB_HANDLE                 *GnbHandle
  )
{
  PCI_ADDR          IommuPciAddress;

  if (GnbHandle->Address.Address.Bus != 0xFF) {
    IommuPciAddress = NbioGetHostPciAddress (GnbHandle);
    IommuPciAddress.Address.Function = 0x2;

    if (GnbLibPciIsDevicePresent (IommuPciAddress.AddressValue)) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
 * Create IVMD entry
 * @param[in]  Type            Root type for IVMD (IvrsIvmdBlock or IvrsIvmdrBlock)
 * @param[in]  StartDevice     Device ID of start device range
 *                             Use 0x0000 for ALL
 * @param[in]  EndDevice       Device ID of end device range
 *                             Use 0xFFFF for ALL
 *                             Use == StartDevice for specific device
 * @param[in]  BlockAddress    Address of memory block to be excluded
 * @param[in]  BlockLength     Length of memory block go be excluded
 * @param[in]  Ivmd            Pointer to IVMD entry
 * @param[in]  StdHeader       Standard configuration header
 *
 */
VOID
IvmdAddEntry (
  IN       IVRS_BLOCK_TYPE      Type,
  IN       UINT16               StartDevice,
  IN       UINT16               EndDevice,
  IN       UINT64               BlockAddress,
  IN       UINT64               BlockLength,
  IN       IVRS_IVMD_ENTRY      *Ivmd
  )
{
  Ivmd->Flags = IVMD_FLAG_EXCLUSION_RANGE;
  Ivmd->Length = sizeof (IVRS_IVMD_ENTRY);
  Ivmd->DeviceId = StartDevice;
  Ivmd->AuxiliaryData = 0x0;
  Ivmd->Reserved = 0x0000000000000000;
  Ivmd->BlockStart = BlockAddress;
  Ivmd->BlockLength = BlockLength;
  if (Type == IvrsIvmdBlock) {
    if (StartDevice == EndDevice) {
      Ivmd->Type = IvrsIvmdBlockSingle;
    } else if ((StartDevice == 0x0000) && (EndDevice == 0xFFFF)) {
      Ivmd->Type = IvrsIvmdBlock;
    } else {
      Ivmd->Type = IvrsIvmdBlockRange;
      Ivmd->AuxiliaryData = EndDevice;
    }
  } else {
    if (StartDevice == EndDevice) {
      Ivmd->Type = IvrsIvmdrBlockSingle;
    } else {
      Ivmd->Type = IvrsIvmdrBlock;
    }
  }
}

/**
 * Build IVMD list
 * @param[in]  Type            Entry type
 * @param[in]  Ivrs            IVRS table pointer
 * @param[in]  StdHeader       Standard configuration header
 *
 */

EFI_STATUS
BuildIvmdList (
  IN       IVRS_BLOCK_TYPE            Type,
  IN       VOID                       *Ivrs
  )
{
  IOMMU_EXCLUSION_RANGE_DESCRIPTOR  *IvrsExclusionRangeList;
  IVRS_IVMD_ENTRY                   *Ivmd;
  UINT16                            StartId;
  UINT16                            EndId;

  DEBUG ((DEBUG_INFO, "BuildIvmdList Entry\n"));

  IvrsExclusionRangeList = NULL;

  if (IvrsExclusionRangeList != NULL) {
    // Process the entire IvrsExclusionRangeList here and create an IVMD for eache entry
    DEBUG ((DEBUG_INFO, "Process Exclusion Range List\n"));
    while ((IvrsExclusionRangeList->Flags & DESCRIPTOR_TERMINATE_LIST) == 0) {
      if ((IvrsExclusionRangeList->Flags & DESCRIPTOR_IGNORE) == 0) {
        // Address of IVMD entry
        Ivmd = (IVRS_IVMD_ENTRY*) ((UINT8 *)Ivrs + ((IOMMU_IVRS_HEADER *) Ivrs)->TableLength);
        StartId =
          (IvrsExclusionRangeList->RequestorIdStart.Bus << 8) +
          (IvrsExclusionRangeList->RequestorIdStart.Device << 3) +
          (IvrsExclusionRangeList->RequestorIdStart.Function);
        EndId =
          (IvrsExclusionRangeList->RequestorIdEnd.Bus << 8) +
          (IvrsExclusionRangeList->RequestorIdEnd.Device << 3) +
          (IvrsExclusionRangeList->RequestorIdEnd.Function);
        IvmdAddEntry (
          Type,
          StartId,
          EndId,
          IvrsExclusionRangeList->RangeBaseAddress,
          IvrsExclusionRangeList->RangeLength,
          Ivmd);
        // Add entry size to existing table length
        ((IOMMU_IVRS_HEADER *)Ivrs)->TableLength += sizeof (IVRS_IVMD_ENTRY);
      }
      // Point to next entry in IvrsExclusionRangeList
      IvrsExclusionRangeList++;
    }
  }

  DEBUG ((DEBUG_INFO, "GnbBuildIvmdList Exit\n"));
  return EFI_SUCCESS;
}

VOID
MpdmaBusIdDynUpdate (
  IN       GNB_HANDLE        *GnbHandle,
  IN OUT   IVRS_DEVICE_LIST   **DeviceInfo
) {
  UINT8              DevInfoCnt = 0;
  IVRS_DEVICE_LIST   *DevInfo = NULL;
  UINT32             Index;

  for (Index = 0; Index < (sizeof(RbIvrsMap) / sizeof(MPDMA_IVRS_MAP)); Index++) {
    if ((RbIvrsMap[Index].SocketId == GnbHandle->SocketId) &&
          (RbIvrsMap[Index].RBIndex == GnbHandle->RBIndex)) {
      break;
    }
  }

  if (Index == (sizeof(RbIvrsMap) / sizeof(MPDMA_IVRS_MAP))) {
    // not found the target RB
    DEBUG ((DEBUG_INFO, "%a not found the target RB\n", __FUNCTION__));
    return;
  }

  DevInfo = *DeviceInfo;

  while (DevInfo[DevInfoCnt].DeviceId != 0xFFFF) {
    DevInfoCnt++;
  }

  DevInfo = NULL;

  //Add 2 info with End sign (0xFFFF) and Mpdma Ivrs data
  DevInfo = AllocateZeroPool (sizeof (IVRS_DEVICE_LIST) * (DevInfoCnt + 2));
  if (DevInfo == NULL) {
    return;
  }
  // Copy original data
  if (DevInfoCnt) {
    CopyMem (DevInfo, *DeviceInfo, sizeof (IVRS_DEVICE_LIST) * DevInfoCnt);
  }
  // Add new MPDMA device info data
  CopyMem (&DevInfo[DevInfoCnt], &RbIvrsMap[Index].IvrsEntry, sizeof (IVRS_DEVICE_LIST));
  // Update DeviceId
  DevInfo[DevInfoCnt].DeviceId &= 0xFF;
  DevInfo[DevInfoCnt].DeviceId |= (UINT16)(GnbHandle->Address.Address.Bus << 8);
  DevInfo[DevInfoCnt+1].DeviceId = 0xFFFF;

  *DeviceInfo = DevInfo;
}

/**
 * Create IVHDR entry for F0 device
 * @param[in]  Ivhd            Pointer to IVHD entry
 *
 */
VOID
NbioIvhdAddF0DeviceEntries (
  IN       GNB_HANDLE           *GnbHandle,
  IN       IVRS_IVHD_ENTRY      *Ivhd
  )
{
  IVRS_DEVICE_LIST      *DeviceInfo;
  IVHD_TYPEF0_ENTRY     *TypeF0Entry;
  UINT16                Offset;
  UINT8                 *UidPointer;
  UINT8                 Index;

  DEBUG ((DEBUG_INFO, "%a Enter\n", __FUNCTION__));

  DeviceInfo = &OemIvrsDeviceList[0];

  //if (PcdGetBool(PcdMpdmaAcpiIvrsSupport)) {
  MpdmaBusIdDynUpdate(GnbHandle, &DeviceInfo);
  //}

  DEBUG ((DEBUG_INFO, "Returned from IVRS Device Info\n"));

  if (DeviceInfo == NULL) {
    DEBUG ((DEBUG_ERROR, "No IVRS Device Info\n"));
    return;
  }

  DebugDumpBuffer (DeviceInfo, 2 * sizeof (IVRS_DEVICE_LIST), 1, sizeof (IVRS_DEVICE_LIST));

  Offset = (Ivhd->Length + 0x7) & (~ 0x7);

  while (DeviceInfo->DeviceId != 0xFFFF) {
    TypeF0Entry = (IVHD_TYPEF0_ENTRY *) ((UINT8 *) Ivhd + Offset);

    DEBUG ((DEBUG_INFO, "Processing Entry.. \n - DeviceId = 0x%x\n - UidFormat = 0x%x\n - UidLength = 0x%x\n", 
            DeviceInfo->DeviceId, DeviceInfo->UidFormat, DeviceInfo->UidLength));

    TypeF0Entry->Type = 0xF0;

    TypeF0Entry->DeviceId = DeviceInfo->DeviceId;
    TypeF0Entry->DataSetting = DeviceInfo->DataSetting;
    TypeF0Entry->HardwareId.IdByte0 = DeviceInfo->HardwareId[0];
    TypeF0Entry->HardwareId.IdByte1 = DeviceInfo->HardwareId[1];
    TypeF0Entry->HardwareId.IdByte2 = DeviceInfo->HardwareId[2];
    TypeF0Entry->HardwareId.IdByte3 = DeviceInfo->HardwareId[3];
    TypeF0Entry->HardwareId.IdByte4 = DeviceInfo->HardwareId[4];
    TypeF0Entry->HardwareId.IdByte5 = DeviceInfo->HardwareId[5];
    TypeF0Entry->HardwareId.IdByte6 = DeviceInfo->HardwareId[6];
    TypeF0Entry->HardwareId.IdByte7 = DeviceInfo->HardwareId[7];

    TypeF0Entry->CompatibleId.IdByte0 = DeviceInfo->CompatibleId[0];
    TypeF0Entry->CompatibleId.IdByte1 = DeviceInfo->CompatibleId[1];
    TypeF0Entry->CompatibleId.IdByte2 = DeviceInfo->CompatibleId[2];
    TypeF0Entry->CompatibleId.IdByte3 = DeviceInfo->CompatibleId[3];
    TypeF0Entry->CompatibleId.IdByte4 = DeviceInfo->CompatibleId[4];
    TypeF0Entry->CompatibleId.IdByte5 = DeviceInfo->CompatibleId[5];
    TypeF0Entry->CompatibleId.IdByte6 = DeviceInfo->CompatibleId[6];
    TypeF0Entry->CompatibleId.IdByte7 = DeviceInfo->CompatibleId[7];

    TypeF0Entry->UidFormat = DeviceInfo->UidFormat;
    TypeF0Entry->UidLength = DeviceInfo->UidLength;

    UidPointer = (UINT8 *) TypeF0Entry;
    UidPointer += sizeof (IVHD_TYPEF0_ENTRY);
    for (Index = 0; Index < DeviceInfo->UidLength; Index++) {
      *UidPointer = DeviceInfo->Uid[Index];
      UidPointer++;
    }
    DebugDumpBuffer (TypeF0Entry, sizeof (IVHD_TYPEF0_ENTRY) + DeviceInfo->UidLength, 1, 32);
    DEBUG ((DEBUG_INFO, "\n"));
    Offset += sizeof (IVHD_TYPEF0_ENTRY) + DeviceInfo->UidLength;
    DeviceInfo++;
  }
  Ivhd->Length = Offset;
  DEBUG ((DEBUG_INFO, "Ivhd Length = 0x%x\n", Ivhd->Length));
  return;
}

/**
 * Create IVHDR entry for special device
 * @param[in]  SpecialDevice   Special device Type
 * @param[in]  Device          Address of requestor ID for special device
 * @param[in]  Id              Apic ID/ Hpet ID
 * @param[in]  DataSetting     Data setting
 * @param[in]  Ivhd            Pointer to IVHD entry
 *
 */
VOID
NbioIvhdAddSpecialDeviceEntry (
  IN       IVHD_SPECIAL_DEVICE  SpecialDevice,
  IN       PCI_ADDR             Device,
  IN       UINT8                Id,
  IN       UINT8                DataSetting,
  IN       IVRS_IVHD_ENTRY      *Ivhd
  )
{
  IVHD_SPECIAL_ENTRY  *SpecialEntry;
  UINT16               Offset;

  Offset = (Ivhd->Length + 0x7) & (~ 0x7);
  SpecialEntry = (IVHD_SPECIAL_ENTRY *) ((UINT8 *) Ivhd + Offset);
  SpecialEntry->Type = IvhdEntrySpecialDevice;
  SpecialEntry->AliasDeviceId = DEVICE_ID (Device);
  SpecialEntry->Variety = (UINT8) SpecialDevice;
  SpecialEntry->Handle = Id;
  SpecialEntry->DataSetting = DataSetting;
  Ivhd->Length = sizeof (IVHD_SPECIAL_ENTRY) + Offset;
}

/**
 * Create IVHDR entry for aliased range
 * @param[in]  StartRange      Address of start range
 * @param[in]  EndRange        Address of end range
 * @param[in]  Alias           Address of alias requestor ID for range
 * @param[in]  DataSetting     Data setting
 * @param[in]  Ivhd            Pointer to IVHD entry
 *
 */
VOID
NbioIvhdAddDeviceAliasRangeEntry (
  IN       PCI_ADDR             StartRange,
  IN       PCI_ADDR             EndRange,
  IN       PCI_ADDR             Alias,
  IN       UINT8                DataSetting,
  IN       IVRS_IVHD_ENTRY      *Ivhd
  )
{
  IVHD_ALIAS_ENTRY    *RangeEntry;
  IVHD_GENERIC_ENTRY  *Entry;
  UINT16              Offset;

  Offset = (Ivhd->Length + 0x7) & (~ 0x7);
  RangeEntry = (IVHD_ALIAS_ENTRY *) ((UINT8 *) Ivhd + Offset);
  RangeEntry->Type = IvhdEntryAliasStartRange;
  RangeEntry->DeviceId = DEVICE_ID (StartRange);
  RangeEntry->AliasDeviceId = DEVICE_ID (Alias);
  RangeEntry->DataSetting = DataSetting;
  Ivhd->Length = sizeof (IVHD_ALIAS_ENTRY) + Offset;
  Entry = (IVHD_GENERIC_ENTRY *) ((UINT8 *) Ivhd + Ivhd->Length);
  Entry->Type = IvhdEntryEndRange;
  Entry->DeviceId = DEVICE_ID (EndRange);
  Ivhd->Length += sizeof (IVHD_GENERIC_ENTRY);
}

/**
 * Create IVHD entry
 * @param[in]  Ivhd            IVHD header pointer
 *
 */
VOID
SbCreateIvhdEntries (
     OUT   IVRS_IVHD_ENTRY            *Ivhd
  )
{
  PCI_ADDR  Start;
  PCI_ADDR  End;
  PCI_ADDR  PciAddress;
  UINT32    Value;

  DEBUG ((DEBUG_INFO, "SbCreateIvhdEntries Entry\n"));

  // FCH UART Device
  PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0x14, 5, 0);
// P2P alias entry
  xUSLPciRead (PciAddress.AddressValue | 0x18, AccessWidth32, &Value);
  Start.AddressValue = MAKE_SBDFO (0, (Value >> 8) & 0xff, 0, 0, 0);
  End.AddressValue = MAKE_SBDFO (0, (Value >> 16) & 0xff, 0x1f, 0x7, 0);
  NbioIvhdAddDeviceAliasRangeEntry (Start, End, PciAddress, 0, Ivhd);
  PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0x14, 0, 0);
// HPET
  NbioIvhdAddSpecialDeviceEntry (IvhdSpecialDeviceHpet, PciAddress, 0, 0, Ivhd);

// APIC
  if (PcdGet8 (PcdCfgFchIoapicId) != 0xff) {
    NbioIvhdAddSpecialDeviceEntry (
                        IvhdSpecialDeviceIoapic,
                        PciAddress,
                        PcdGet8 (PcdCfgFchIoapicId),
                        0xD7,
                        Ivhd);
  }
  DEBUG ((DEBUG_INFO, "SbCreateIvhdEntries Exit\n"));
}

/**
 * Create IVHDR entry for device range
 * @param[in]  StartRange      Address of start range
 * @param[in]  EndRange        Address of end range
 * @param[in]  DataSetting     Data setting
 * @param[in]  Ivhd            Pointer to IVHD entry
 *
 */
VOID
NbioIvhdAddDeviceRangeEntry (
  IN       PCI_ADDR             StartRange,
  IN       PCI_ADDR             EndRange,
  IN       UINT8                DataSetting,
  IN       IVRS_IVHD_ENTRY      *Ivhd
  )
{
  IVHD_GENERIC_ENTRY  *Entry;
  Entry = (IVHD_GENERIC_ENTRY *) ((UINT8 *) Ivhd + Ivhd->Length);
  Entry->Type = IvhdEntryStartRange;
  Entry->DeviceId = DEVICE_ID (StartRange);
  Entry->DataSetting = DataSetting;
  Ivhd->Length += sizeof (IVHD_GENERIC_ENTRY);
  Entry = (IVHD_GENERIC_ENTRY *) ((UINT8 *) Ivhd + Ivhd->Length);
  Entry->Type = IvhdEntryEndRange;
  Entry->DeviceId = DEVICE_ID (EndRange);
  Ivhd->Length += sizeof (IVHD_GENERIC_ENTRY);
}

/**
 * Get bus range decoded by GNB
 *
 * Final bus allocation can not be assumed until AmdInitMid
 *
 * @param[in]   GnbHandle       GNB handle
 * @param[out]  SegmentNumber   The segment number
 * @param[out]  StartBusNumber  Beginning of the Bus Range
 * @param[out]  EndBusNumber    End of the Bus Range
 * @retval                      Status
 */
EFI_STATUS
GnbGetBusDecodeRange (
  IN    GNB_HANDLE                 *GnbHandle,
  OUT   UINT16                     *SegmentNumber,
  OUT   UINT8                      *StartBusNumber,
  OUT   UINT8                      *EndBusNumber
  )
{
  PCI_ADDR            GnbPciAddress;
  GNB_HANDLE          *NextHandle;

  GnbPciAddress = NbioGetHostPciAddress (GnbHandle);
  *SegmentNumber = (UINT16)GnbPciAddress.Address.Segment;
  *StartBusNumber = (UINT8)GnbPciAddress.Address.Bus;
  if (GnbHandle->BusNumberLimit !=0) {
    *EndBusNumber = GnbHandle->BusNumberLimit;
  } else {
    NextHandle = GnbGetNextHandle(GnbHandle);

    if (NextHandle == NULL) {
      *EndBusNumber = 0xFF;
    } else {
      GnbPciAddress = NbioGetHostPciAddress (NextHandle);
      *EndBusNumber = (UINT8)(GnbPciAddress.Address.Bus - 1);
    }
  }

  return EFI_SUCCESS;
}

/**
 * Create gnb ioapic IVHD entry
 * @param[in]  GnbHandle       Gnb handle
 * @param[in]  Ivhd            IVHD header pointer
 *
 */
VOID
NbioIvhdAddApicEntry (
  IN       GNB_HANDLE                 *GnbHandle,
     OUT   IVRS_IVHD_ENTRY            *Ivhd
  )
{
  UINT32              AddressLow;
  UINT32              AddressHigh;
  UINT64              IoapicAddress;
  PCI_ADDR            GnbPciAddress;
  PCI_ADDR            GnbIoapicPciId;

  // Get the PCI address of the GNB
  GnbPciAddress.AddressValue = GnbHandle->Address.AddressValue;

  ReadIoApicHiAddress(GnbHandle, &AddressHigh);
  ReadIoApicLoAddress(GnbHandle, &AddressLow);

  IoapicAddress = ((UINT64) AddressHigh) << 32;
  IoapicAddress |= ((UINT64) AddressLow) & 0xffffff00;

  if ((IoapicAddress != 0) && ((AddressLow & 0x01) == 1) && (PcdGet8 (PcdCfgGnbIoapicId) != 0xff)) {
    GnbIoapicPciId.AddressValue = GnbPciAddress.AddressValue;
    GnbIoapicPciId.Address.Function = 1;
    NbioIvhdAddSpecialDeviceEntry (
                      IvhdSpecialDeviceIoapic,
                      GnbIoapicPciId,
                      mGnbIoApicIdBase,
                      0,
                      Ivhd);
  }
}

/**
 * Create IVHD entry
 * @param[in]  GnbHandle       Gnb handle
 * @param[in]  Ivhd            IVHD header pointer
 *
 */
VOID
CreateIvhd (
  IN    GNB_HANDLE                 *GnbHandle,
  OUT   IVRS_IVHD_ENTRY            *Ivhd
  )
{
  EFI_STATUS    Status;
  PCI_ADDR      Start;
  PCI_ADDR      End;
  UINT16        SegmentNumber;
  UINT8         StartBusNumber;
  UINT8         EndBusNumber;

  Status = GnbGetBusDecodeRange (GnbHandle, &SegmentNumber, &StartBusNumber, &EndBusNumber);
  ASSERT (Status == EFI_SUCCESS);
  Start.AddressValue = MAKE_SBDFO (SegmentNumber, StartBusNumber, 0, 3, 0);
  End.AddressValue = MAKE_SBDFO (SegmentNumber, EndBusNumber, 0x1F, 6, 0);
  DEBUG ((DEBUG_INFO, "StartBusNumber = %x     EndBusNumber = %x  \n", StartBusNumber, EndBusNumber));
  NbioIvhdAddDeviceRangeEntry (Start, End, 0, Ivhd);
  if (GnbHandle->Address.AddressValue == 0) {
    SbCreateIvhdEntries (Ivhd);
  }
  NbioIvhdAddApicEntry (GnbHandle, Ivhd);
  return;
}

/**
 * Create IVHD entry
 * @param[in]  GnbHandle       Gnb handle
 * @param[in]  Type            Block type
 * @param[in]  Ivhd            IVHD header pointer
 *
 */
VOID
CreateIvhdHeader11h (
  IN    GNB_HANDLE                 *GnbHandle,
  IN    IVRS_BLOCK_TYPE            Type,
  OUT   IVRS_IVHD_ENTRY_11H        *Ivhd
  )
{
  UINT32            Value;
  PCI_ADDR          IommuPciAddress;
  UINT64            BaseAddress;
  MMIO_0x30         MMIO_x30_Value;
  MMIO_0x18         MMIO_x18_Value;
  CAPABILITY_REG   CapValue;

  IommuPciAddress = NbioGetHostPciAddress (GnbHandle);
  IommuPciAddress.Address.Function = 0x2;

  Ivhd->Ivhd.Type = (UINT8) Type;
  Ivhd->Ivhd.Length = sizeof (IVRS_IVHD_ENTRY_11H);
  Ivhd->Ivhd.DeviceId = (UINT16) (((NbioGetHostPciAddress (GnbHandle).AddressValue) >> 12) | 2);
  Ivhd->Ivhd.CapabilityOffset = GnbLibFindPciCapability (IommuPciAddress.AddressValue, IOMMU_CAP_ID);
  Ivhd->Ivhd.PciSegment = (UINT16) NbioGetHostPciAddress (GnbHandle).Address.Segment;
  xUSLPciRead (IommuPciAddress.AddressValue | (Ivhd->Ivhd.CapabilityOffset + 0x4), AccessWidth32, &Ivhd->Ivhd.BaseAddress);
  xUSLPciRead (IommuPciAddress.AddressValue | (Ivhd->Ivhd.CapabilityOffset + 0x8), AccessWidth32, (UINT8 *) &Ivhd->Ivhd.BaseAddress + 4);
  Ivhd->Ivhd.BaseAddress = Ivhd->Ivhd.BaseAddress & 0xfffffffffffffffe;
  ASSERT (Ivhd->Ivhd.BaseAddress != 0x0);

  MMIO_x30_Value.Value = (UINT64)xUSLMemRead64 ((VOID *)(UINTN)(Ivhd->Ivhd.BaseAddress + 0x30));
  MMIO_x18_Value.Value = (UINT64)xUSLMemRead64 ((VOID *)(UINTN)(Ivhd->Ivhd.BaseAddress + 0x18));
  xUSLPciRead (IommuPciAddress.AddressValue | Ivhd->Ivhd.CapabilityOffset, AccessWidth32, &(CapValue.Value));
  Ivhd->Ivhd.Flags |= ((MMIO_x18_Value.Field.Coherent != 0) ? IVHD_FLAG_COHERENT : 0);
  Ivhd->Ivhd.Flags |= ((CapValue.Field.IommuIoTlbsup != 0) ? IVHD_FLAG_IOTLBSUP : 0);
  Ivhd->Ivhd.Flags |= ((MMIO_x18_Value.Field.Isoc != 0) ? IVHD_FLAG_ISOC : 0);
  Ivhd->Ivhd.Flags |= ((MMIO_x18_Value.Field.ResPassPW != 0) ? IVHD_FLAG_RESPASSPW : 0);
  Ivhd->Ivhd.Flags |= ((MMIO_x18_Value.Field.PassPW != 0) ? IVHD_FLAG_PASSPW : 0);
  Ivhd->Ivhd.Flags |= ((MMIO_x30_Value.Field.PPRSup != 0) ? IVHD_FLAG_PPRSUB : 0);
  Ivhd->Ivhd.Flags |= ((MMIO_x30_Value.Field.PreFSup != 0) ? IVHD_FLAG_PREFSUP : 0);
  Ivhd->Ivhd.Flags |= ((MMIO_x18_Value.Field.HtTunEn != 0) ? IVHD_FLAG_HTTUNEN : 0);

  xUSLPciRead (IommuPciAddress.AddressValue | (Ivhd->Ivhd.CapabilityOffset + 0x10), AccessWidth32, &Value);
  Ivhd->Ivhd.IommuInfo = (UINT16) (Value & 0x1F);
  xUSLPciRead (IommuPciAddress.AddressValue | (Ivhd->Ivhd.CapabilityOffset + 0xC), AccessWidth32, &Value);
  Ivhd->Ivhd.IommuInfo |= ((Value & 0x1F) << IVHD_INFO_UNITID_OFFSET);

  // Assign attributes
  xUSLPciRead (IommuPciAddress.AddressValue | (Ivhd->Ivhd.CapabilityOffset + 0x10), AccessWidth32, &Value);
  Ivhd->IommuAttributes = ((Value & 0xf8000000) >> 27) << 23;
  DEBUG ((DEBUG_INFO, "Attribute cap offset 0x10 = %x\n", Value));
  BaseAddress = Ivhd->Ivhd.BaseAddress;
  Value = (UINT32)xUSLMemRead32 ((VOID *)(UINTN)(BaseAddress + 0x4000));

  Ivhd->IommuAttributes |= (((Value & 0x3f000) >> 12) << 17) | (((Value & 0x780) >> 7) << 7);
  DEBUG ((DEBUG_INFO, "Attribute MMIO 0x4000 = %x\n", Value));

  // Assign 64bits EFR for type 11, 41h
  Ivhd->IommuEfr = (UINT64)xUSLMemRead64 ((VOID *)(UINTN)(BaseAddress + 0x30));
  Ivhd->IommuEfr |= BIT46;

  Ivhd->IommuEfr2 = (UINT32)xUSLMemRead32 ((VOID *)(UINTN)(BaseAddress + 0x1A0));

  DEBUG ((DEBUG_INFO, "IommuEfr = %x\n", Ivhd->IommuEfr));

}

/**
 * Create IVHD entry
 * @param[in]  GnbHandle       Gnb handle
 * @param[in]  Type            Block type
 * @param[in]  Ivhd            IVHD header pointer
 *
 */
VOID
CreateIvhdHeader10h (
  IN  GNB_HANDLE                 *GnbHandle,
  IN  IVRS_BLOCK_TYPE            Type,
  OUT IVRS_IVHD_ENTRY_10H        *Ivhd
  )
{
  UINT32           Value;
  PCI_ADDR         IommuPciAddress;
  MMIO_0x30        MMIO_x30_Value;
  MMIO_0x18        MMIO_x18_Value;
  MMIO_0x4000      MMIO_x4000_Value;
  CAPABILITY_REG   CapValue;
  UINT32           MsiNumPPR;

  IommuPciAddress = NbioGetHostPciAddress (GnbHandle);
  IommuPciAddress.Address.Function = 0x2;

  Ivhd->Ivhd.Type = (UINT8) Type;
  Ivhd->Ivhd.Length = sizeof (IVRS_IVHD_ENTRY_10H);
  Ivhd->Ivhd.DeviceId = (UINT16) (((NbioGetHostPciAddress (GnbHandle).AddressValue) >> 12) | 2);
  Ivhd->Ivhd.CapabilityOffset = GnbLibFindPciCapability (IommuPciAddress.AddressValue, IOMMU_CAP_ID);
  Ivhd->Ivhd.PciSegment = (UINT16) NbioGetHostPciAddress (GnbHandle).Address.Segment;
  xUSLPciRead (IommuPciAddress.AddressValue | (Ivhd->Ivhd.CapabilityOffset + 0x4), AccessWidth32, &Ivhd->Ivhd.BaseAddress);
  xUSLPciRead (IommuPciAddress.AddressValue | (Ivhd->Ivhd.CapabilityOffset + 0x8), AccessWidth32, (UINT8 *) &Ivhd->Ivhd.BaseAddress + 4);
  DEBUG ((DEBUG_INFO, "CreateIvhdHeader10h Ivhd->Ivhd.BaseAddress : %lx\n", Ivhd->Ivhd.BaseAddress));
  Ivhd->Ivhd.BaseAddress = Ivhd->Ivhd.BaseAddress & 0xfffffffffffffffe;
  ASSERT (Ivhd->Ivhd.BaseAddress != 0x0);

  MMIO_x30_Value.Value = (UINT64)xUSLMemRead64 ((VOID *)(UINTN)(Ivhd->Ivhd.BaseAddress + 0x30));
  DEBUG ((DEBUG_INFO, "CreateIvhdHeader10h MMIO_x30_Value.Value : %lx\n", MMIO_x30_Value.Value));

  MMIO_x18_Value.Value = (UINT64)xUSLMemRead64 ((VOID *)(UINTN)(Ivhd->Ivhd.BaseAddress + 0x18));
  DEBUG ((DEBUG_INFO, "CreateIvhdHeader10h MMIO_x18_Value.Value : %lx\n", MMIO_x18_Value.Value));

  xUSLPciRead (IommuPciAddress.AddressValue | Ivhd->Ivhd.CapabilityOffset, AccessWidth32, &(CapValue.Value));
  DEBUG ((DEBUG_INFO, "CreateIvhdHeader10h CapValue.Value : %x\n", CapValue.Value));

  DEBUG ((DEBUG_INFO, "CreateIvhdHeader10h Ivhd->Ivhd.Flags : %x\n", Ivhd->Ivhd.Flags));
  Ivhd->Ivhd.Flags |= ((MMIO_x18_Value.Field.Coherent != 0) ? IVHD_FLAG_COHERENT : 0);
  Ivhd->Ivhd.Flags |= ((CapValue.Field.IommuIoTlbsup != 0) ? IVHD_FLAG_IOTLBSUP : 0);
  Ivhd->Ivhd.Flags |= ((MMIO_x18_Value.Field.Isoc != 0) ? IVHD_FLAG_ISOC : 0);
  Ivhd->Ivhd.Flags |= ((MMIO_x18_Value.Field.ResPassPW != 0) ? IVHD_FLAG_RESPASSPW : 0);
  Ivhd->Ivhd.Flags |= ((MMIO_x18_Value.Field.PassPW != 0) ? IVHD_FLAG_PASSPW : 0);
  Ivhd->Ivhd.Flags |= ((MMIO_x30_Value.Field.PPRSup != 0) ? IVHD_FLAG_PPRSUB : 0);
  Ivhd->Ivhd.Flags |= ((MMIO_x30_Value.Field.PreFSup != 0) ? IVHD_FLAG_PREFSUP : 0);
  Ivhd->Ivhd.Flags |= ((MMIO_x18_Value.Field.HtTunEn != 0) ? IVHD_FLAG_HTTUNEN : 0);
  DEBUG ((DEBUG_INFO, "CreateIvhdHeader10h Ivhd->Ivhd.Flags : %x\n", Ivhd->Ivhd.Flags));

  xUSLPciRead (IommuPciAddress.AddressValue | (Ivhd->Ivhd.CapabilityOffset + 0x10), AccessWidth32, &Value);
  DEBUG ((DEBUG_INFO, "CreateIvhdHeader10h Ivhd->Ivhd.CapabilityOffset + 0x10.Value : %x\n", Value));

  DEBUG ((DEBUG_INFO, "CreateIvhdHeader10h Ivhd->Ivhd.IommuInfo : %x\n", Ivhd->Ivhd.IommuInfo));
  Ivhd->Ivhd.IommuInfo = (UINT16) (Value & 0x1F);
  DEBUG ((DEBUG_INFO, "CreateIvhdHeader10h Ivhd->Ivhd.IommuInfo : %x\n", Ivhd->Ivhd.IommuInfo));
  MsiNumPPR = Value >> 27;
  xUSLPciRead (IommuPciAddress.AddressValue | (Ivhd->Ivhd.CapabilityOffset + 0xC), AccessWidth32, &Value);
  DEBUG ((DEBUG_INFO, "CreateIvhdHeader10h Ivhd->Ivhd.CapabilityOffset + 0xC.Value : %x\n", Value));
  DEBUG ((DEBUG_INFO, "CreateIvhdHeader10h Ivhd->Ivhd.IommuInfo : %x\n", Ivhd->Ivhd.IommuInfo));
  Ivhd->Ivhd.IommuInfo |= ((Value & 0x1F) << IVHD_INFO_UNITID_OFFSET);
  DEBUG ((DEBUG_INFO, "CreateIvhdHeader10h Ivhd->Ivhd.IommuInfo : %x\n", Ivhd->Ivhd.IommuInfo));

  MMIO_x4000_Value.Value = (UINT64)xUSLMemRead64 ((VOID *)(UINTN)(Ivhd->Ivhd.BaseAddress + 0x4000));
  DEBUG ((DEBUG_INFO, "CreateIvhdHeader10h MMIO_x4000_Value.Value : %lx\n", MMIO_x4000_Value.Value));

  DEBUG ((DEBUG_INFO, "CreateIvhdHeader10h Ivhd->IommuEfr : %x\n", Ivhd->IommuEfr));
  Ivhd->IommuEfr = (UINT32) ((MMIO_x30_Value.Field.XTSup << IVHD_EFR_XTSUP_OFFSET) |
                   (MMIO_x30_Value.Field.NXSup << IVHD_EFR_NXSUP_OFFSET) |
                   (MMIO_x30_Value.Field.GTSup << IVHD_EFR_GTSUP_OFFSET) |
                   (MMIO_x30_Value.Field.GLXSup << IVHD_EFR_GLXSUP_OFFSET) |
                   (MMIO_x30_Value.Field.IASup << IVHD_EFR_IASUP_OFFSET) |
                   (MMIO_x30_Value.Field.GASup << IVHD_EFR_GASUP_OFFSET) |
                   (MMIO_x30_Value.Field.HESup << IVHD_EFR_HESUP_OFFSET) |
                   (MMIO_x30_Value.Field.PASmax << IVHD_EFR_PASMAX_OFFSET) |
                   (MMIO_x4000_Value.Field.NCounter << IVHD_EFR_PNCOUNTERS_OFFSET) |
                   (MMIO_x4000_Value.Field.NCounterBanks << IVHD_EFR_PNBANKS_OFFSET) |
                   (MsiNumPPR << IVHD_EFR_MSINUMPPR_OFFSET) |
                   (MMIO_x30_Value.Field.GATS << IVHD_EFR_GATS_OFFSET) |
                   (MMIO_x30_Value.Field.HATS << IVHD_EFR_HATS_OFFSET));
  DEBUG ((DEBUG_INFO, "CreateIvhdHeader10h Ivhd->IommuEfr : %x\n", Ivhd->IommuEfr));
}

/**
 * Create IVRS entry
 * @param[in]  GnbHandle       Gnb handle
 * @param[in]  Type            Entry type
 * @param[in]  Ivrs            IVRS table pointer
 * @retval     EFI_STATUS
 *
 */
EFI_STATUS
CreateIvrsEntry (
  IN       GNB_HANDLE                 *GnbHandle,
  IN       IVRS_BLOCK_TYPE            Type,
  IN       VOID                       *Ivrs
  )
{
  IVRS_IVHD_ENTRY           *Ivhd;
  UINT8                     IommuCapabilityOffset;
  UINT32                    Value;
  PCI_ADDR                  IommuPciAddress;

  DEBUG ((DEBUG_INFO, "CreateIvrsEntry Entry : Type - %x\n", Type));

  if (Type == IvrsIvhdBlock10h || Type == IvrsIvhdBlock11h || Type == IvrsIvhdrBlock40h) {

    IommuPciAddress = NbioGetHostPciAddress (GnbHandle);
    IommuPciAddress.Address.Function = 0x2;

    // Update IVINFO
    IommuCapabilityOffset = GnbLibFindPciCapability (IommuPciAddress.AddressValue, IOMMU_CAP_ID);
    DEBUG ((DEBUG_INFO, "CreateIvrsEntry IommuCapabilityOffset : %x\n", IommuCapabilityOffset));
    xUSLPciRead (IommuPciAddress.AddressValue | (IommuCapabilityOffset + 0x10), AccessWidth32, &Value);
    DEBUG ((DEBUG_INFO, "CreateIvrsEntry IommuCapabilityOffset + 0x10 IvInfo:Value : %x\n", Value));
    ((IOMMU_IVRS_HEADER *) Ivrs)->IvInfo = Value & (IVINFO_HTATSRESV_MASK | IVINFO_VASIZE_MASK | IVINFO_GASIZE_MASK | IVINFO_PASIZE_MASK);

    // EFRSup: IVINFO[0] = bit 27 of Cap+0x10
    xUSLPciRead (IommuPciAddress.AddressValue | (IommuCapabilityOffset + 0x00), AccessWidth32, &Value);
    DEBUG ((DEBUG_INFO, "CreateIvrsEntry IommuCapabilityOffset + 0x00 IvInfo:Value : %x\n", Value));
    if ((Value & BIT27) != 0) {
      ((IOMMU_IVRS_HEADER *) Ivrs)->IvInfo |= IVINFO_EFRSUP_MASK;
    }
    //if (PcdGetBool (PcdDmaProtection)) {
    //  ((IOMMU_IVRS_HEADER *) Ivrs)->IvInfo |= IVINFO_DMAREMAP_MASK;
    //}

    // Address of IVHD entry
    Ivhd = (IVRS_IVHD_ENTRY *) ((UINT8 *)Ivrs + ((IOMMU_IVRS_HEADER *) Ivrs)->TableLength);
    if (Type == IvrsIvhdBlock10h) {
      CreateIvhdHeader10h (GnbHandle, Type, (IVRS_IVHD_ENTRY_10H *) Ivhd);
    }
    if (Type == IvrsIvhdBlock11h || Type == IvrsIvhdrBlock40h) {
      CreateIvhdHeader11h (GnbHandle, Type, (IVRS_IVHD_ENTRY_11H *) Ivhd);
    }

    switch(Type){
      case IvrsIvhdBlock10h:
      case IvrsIvhdBlock11h:
        CreateIvhd (GnbHandle, Ivhd);
        break;
      case IvrsIvhdrBlock40h:
        CreateIvhd (GnbHandle, Ivhd);
        NbioIvhdAddF0DeviceEntries (GnbHandle, Ivhd);
        break;
      default:
        break;
      }
    ((IOMMU_IVRS_HEADER *) Ivrs)->TableLength = ((IOMMU_IVRS_HEADER *) Ivrs)->TableLength + Ivhd->Length;
  }

  DEBUG ((DEBUG_INFO, "CreateIvrsEntry Exit\n"));

  return EFI_SUCCESS;
}

/**
 *----------------------------------------------------------------------------------------
 * Enable IOMMU base address. (MMIO space )
 * @param[in]     GnbHandle       GNB handle
 * @retval        AGESA_SUCCESS
 * @retval        AGESA_ERROR
 **/
EFI_STATUS
EnableIommuMmio (
  IN       GNB_HANDLE           *GnbHandle
  )
{
  UINT16                  CapabilityOffset;
  UINT64                  BaseAddress;
  UINT32                  Value;
  PCI_ADDR                IommuPciAddress;

  IommuPciAddress = NbioGetHostPciAddress (GnbHandle);
  IommuPciAddress.Address.Function = 0x2;

  CapabilityOffset = GnbLibFindPciCapability (IommuPciAddress.AddressValue, IOMMU_CAP_ID); //*NBIO_TODO : Need to do something about IOMMU_CAP_ID (0x0F) duplicate define

  xUSLPciRead (IommuPciAddress.AddressValue | (CapabilityOffset + 0x8), AccessWidth32, &Value);
  BaseAddress = (UINT64) Value << 32;
  xUSLPciRead (IommuPciAddress.AddressValue | (CapabilityOffset + 0x4), AccessWidth32, &Value);
  BaseAddress |= Value;

  if ((BaseAddress & 0xfffffffffffffffe) != 0x0) {
    DEBUG ((DEBUG_INFO, "Enable IOMMU MMIO at address %x for Socket %d Silicon %d\n", 
          BaseAddress, GnbHandle->SocketId, GnbHandle->DieNumber));
    xUSLPciRMW (IommuPciAddress.AddressValue | (CapabilityOffset + 0x8), AccessWidth32, 0xFFFFFFFF, 0x0);
    xUSLPciRMW (IommuPciAddress.AddressValue | (CapabilityOffset + 0x4), AccessWidth32, 0xFFFFFFFE, 0x1);
  } else {
    DEBUG ((DEBUG_ERROR, "No base address assigned - IOMMU disabled\n"));
    return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;

}

/**
 * Build and install IVRS table
 * @retval        AGESA_SUCCESS
 * @retval        AGESA_ERROR
 */

EFI_STATUS
InstallIommuIvrsTable (
  IN PCIe_PLATFORM_CONFIG                *Pcie
  )
{
  EFI_STATUS                          Status = EFI_UNSUPPORTED;
  VOID                                *Ivrs;
  BOOLEAN                             IvrsSupport = FALSE;
  GNB_HANDLE                          *GnbHandle;
  UINTN                               TableHandle;
  BOOLEAN                             AlternateList;
  PCI_ADDR                            GnbPciAddress;

  Ivrs = AllocateZeroPool (IVRS_TABLE_LENGTH);

  CopyMem (Ivrs, &IvrsHeader, sizeof(IvrsHeader));

  // Update table OEM fields.
  CopyMem (
    (VOID *) &((EFI_ACPI_DESCRIPTION_HEADER *) Ivrs)->OemId,
    (VOID *) PcdGetPtr (PcdAmdAcpiTableHeaderOemId),
    AsciiStrnLenS ((CHAR8 *)PcdGetPtr (PcdAmdAcpiTableHeaderOemId), 6));

  CopyMem (
    (VOID *) &((EFI_ACPI_DESCRIPTION_HEADER *) Ivrs)->OemTableId,
    (VOID *) PcdGetPtr (PcdAmdAcpiIvrsTableHeaderOemTableId),
    AsciiStrnLenS ((CHAR8 *)PcdGetPtr (PcdAmdAcpiIvrsTableHeaderOemTableId), 8));

  if ((PcdGet8 (PcdCfgGnbIoapicId) == 00) || (PcdGet8 (PcdCfgGnbIoapicId) == 0xFF)) {
    DEBUG ((DEBUG_ERROR, "%a Invalid NBIO APIC ID : %x!!!\n", __FUNCTION__, PcdGet8 (PcdCfgGnbIoapicId)));
    return Status;
  }

  if ((PcdGet8 (PcdCfgFchIoapicId) == 00) || (PcdGet8 (PcdCfgFchIoapicId) == 0xFF)) {
    DEBUG ((DEBUG_ERROR, "%a Invalid FCH APIC ID : %x!!!\n", __FUNCTION__, PcdGet8 (PcdCfgFchIoapicId)));
    return Status;
  }

  GnbHandle = NbioGetHandle (Pcie);
  AlternateList = FALSE;
  
  while (GnbHandle != NULL) {
    if (CheckIommuPresent (GnbHandle)) {
      DEBUG ((DEBUG_INFO, "Build IVRS for Socket %d Silicon %d\n", GnbHandle->SocketId , GnbHandle->DieNumber));
      IvrsSupport = TRUE;

      mGnbIoApicIdBase = PcdGet8 (PcdCfgGnbIoapicId);
      mGnbIoApicIdBase += (GnbHandle->SocketId * 4) + GnbHandle->RBIndex;

      mFchIoApicIdBase = PcdGet8 (PcdCfgFchIoapicId);

      DEBUG ((DEBUG_INFO, "GnbIoApicIdBase 0x%x  ", mGnbIoApicIdBase));
      DEBUG ((DEBUG_INFO, "FchIoApicIdBase 0x%x\n", mFchIoApicIdBase));

      GnbPciAddress = NbioGetHostPciAddress (GnbHandle);
  
      DEBUG ((DEBUG_INFO, "GnbPciAddress.Address.Bus 0x%x\n", GnbPciAddress.Address.Bus));

      if (!EFI_ERROR(EnableIommuMmio (GnbHandle))) {
        CreateIvrsEntry (GnbHandle, IvrsIvhdBlock10h, Ivrs);
        CreateIvrsEntry (GnbHandle, IvrsIvhdBlock11h, Ivrs);
        BuildIvmdList (IvrsIvmdBlock, Ivrs);
      }
    }

    GnbHandle = GnbGetNextHandle (GnbHandle);
    if ((GnbHandle == NULL) && (AlternateList == FALSE)) {
      GnbHandle = (GNB_HANDLE *)PcieConfigGetPeer (DESCRIPTOR_SILICON, &Pcie->Header);
      AlternateList = TRUE;
    }
  }

  if (IvrsSupport == TRUE) {
    //GnbIommuIvrsTableDump (Ivrs);
  
    // Set checksum to 0 first
    ((EFI_ACPI_DESCRIPTION_HEADER*) Ivrs)->Checksum = 0;
    // Update checksum value
    ((EFI_ACPI_DESCRIPTION_HEADER*) Ivrs)->Checksum = CalculateCheckSum8 (
                                                                (UINT8 *)Ivrs, 
                                                                ((EFI_ACPI_DESCRIPTION_HEADER*) Ivrs)->Length);

    Status = mAcpiTableProtocol->InstallAcpiTable (
                                          mAcpiTableProtocol, 
                                          Ivrs, 
                                          ((EFI_ACPI_DESCRIPTION_HEADER*) Ivrs)->Length, 
                                          &TableHandle);
  } 
  
  DEBUG ((DEBUG_INFO, "IVRS table generated Status : %r\n", Status));

  return Status;
}

/**
  Installs the ACPI IVRS Table to the System Table.
**/
VOID
EFIAPI
InstallAcpiIvrsTable (
  VOID
  )
{

  EFI_STATUS                          Status;
  PCIe_PLATFORM_CONFIG                *Pcie;

  DEBUG ((DEBUG_INFO, "%a Entry!!!\n", __FUNCTION__));

  Status = PcieGetTopology (&Pcie);

  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "%a Pcie Topology Not Found!!!\n", __FUNCTION__));
    return;
  }

  // Create IVRS SSDT Table
  if (PcdGetBool (PcdCfgIommuSupport)) {
    Status = InstallIommuIvrsTable (Pcie);
  }

  DEBUG ((DEBUG_INFO, "%a Exit Status : %r\n", __FUNCTION__, Status));

}