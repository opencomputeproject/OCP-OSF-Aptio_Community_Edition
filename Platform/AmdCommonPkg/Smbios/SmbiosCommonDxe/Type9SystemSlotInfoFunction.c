/******************************************************************************
 * Copyright (C) 2021-2023 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

#include "SmbiosCommon.h"
#include <Library/PrintLib.h>
#include <Library/HobLib.h>

/**
  This function returns SBDF information for a given slot number.

  @param  SlotNumInfo                Slot number to be provided.
  @param  SegInfo                    Segment number.
  @param  BusInfo                    Bus number.
  @param  DevFunInfo                 Bits 0-2 corresponds to function number & bits 3-7 corresponds
                                     to device number.

  @retval EFI_SUCCESS                All parameters were valid.
  @retval EFI_INVALID_PARAMETER      One or many parameters are invalid.
  @retval EFI_NOT_FOUND              SBDF information is not found for the given slot number.

**/
EFI_STATUS
EFIAPI
SlotBdfInfo(
  IN  UINT16   *SlotNumInfo,
  OUT UINT16   *SegInfo,
  OUT UINT8    *BusInfo,
  OUT UINT8    *DevFunInfo
  )
{
  /*PCIe_PLATFORM_CONFIG             *Pcie;
  PCIe_COMPLEX_CONFIG              *ComplexList;
  PCIe_SILICON_CONFIG              *SiliconList;
  PCIe_WRAPPER_CONFIG              *WrapperList;
  PCIe_ENGINE_CONFIG               *EngineList;

  if (SlotNumInfo == NULL || SegInfo == NULL || BusInfo == NULL || DevFunInfo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Pcie = NULL;
  //PcieGetPcieDxe (&Pcie);
  ComplexList = (PCIe_COMPLEX_CONFIG *) PcieConfigGetChild (DESCRIPTOR_COMPLEX, &Pcie->Header);

  while (ComplexList != NULL) {
    SiliconList = PcieConfigGetChildSilicon (ComplexList);
    while (SiliconList != NULL) {
      WrapperList = PcieConfigGetChildWrapper (SiliconList);
      while (WrapperList != NULL) {
        EngineList = PcieConfigGetChildEngine (WrapperList);
        while (EngineList != NULL) {
          if (EngineList->Type.Port.PortData.SlotNum == *SlotNumInfo) {
            *SegInfo = EngineList->Type.Port.Address.Address.Segment & 0xFFFF;
            *BusInfo = EngineList->Type.Port.Address.Address.Bus & 0xFF;
            *DevFunInfo = (((EngineList->Type.Port.Address.Address.Device) & 0x1F) << 3) |
                          ((EngineList->Type.Port.Address.Address.Function) & 0x7);
            return EFI_SUCCESS;
          }
          EngineList = PcieLibGetNextDescriptor (EngineList);
        }
        WrapperList = PcieLibGetNextDescriptor (WrapperList);
      }
      SiliconList = PcieLibGetNextDescriptor (SiliconList);
    }
    if ((ComplexList->Header.DescriptorFlags & DESCRIPTOR_TERMINATE_TOPOLOGY) == 0) {
      ComplexList++;
    } else {
      ComplexList = NULL;
    }
  }*/
  return EFI_NOT_FOUND;
}

/**
  This function allocates and populate system slot smbios record (Type 9).

  @param  MpioPortPtr              Pointer to Mpio port descriptor.
  @param  SmbiosRecordPtr          Pointer to smbios type 9 record.

  @retval EFI_SUCCESS              All parameters were valid.
  @retval EFI_INVALID_PARAMETER    One or many parameters are invalid.
  @retval EFI_OUT_OF_RESOURCES     Resource not available.

**/
EFI_STATUS
EFIAPI
CreateSmbiosSystemSlotRecord (
  IN MPIO_PORT_DESCRIPTOR          *MpioPortPtr,
  IN OUT SMBIOS_TABLE_TYPE9        **SmbiosRecordPtr
  )
{
  EFI_STATUS                       Status;
  CHAR8                            SlotDesignationStr[SMBIOS_STRING_MAX_LENGTH];
  UINTN                            SlotDesStrLen;
  UINTN                            TotalSize;
  UINTN                            StringOffset;
  SMBIOS_TABLE_TYPE9               *SmbiosRecord;
  UINT16                           SlotNumInfo;
  UINT16                           SegInfo;
  UINT8                            BusInfo;
  UINT8                            DevFunInfo;

  Status = EFI_SUCCESS;
  SegInfo = 0xFFFF;
  BusInfo = 0xFF;
  DevFunInfo = 0xFF;

  if (MpioPortPtr == NULL || SmbiosRecordPtr == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  SlotDesStrLen = AsciiSPrint (
                    SlotDesignationStr,
                    SMBIOS_STRING_MAX_LENGTH,
                    "PCIE-%d",
                    MpioPortPtr->Port.SlotNum
                    );

  // Two zeros following the last string.
  TotalSize = sizeof (SMBIOS_TABLE_TYPE9) + SlotDesStrLen + 2;
  SmbiosRecord = NULL;

  SmbiosRecord = AllocateZeroPool (TotalSize);
  if (SmbiosRecord == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
  } else {
    SmbiosRecord->Hdr.Type = SMBIOS_TYPE_SYSTEM_SLOTS;
    SmbiosRecord->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE9);
    SmbiosRecord->Hdr.Handle = 0;
    SmbiosRecord->SlotDesignation = 1;

    // Currently only map PCIE slots in system slot table.
    if (MpioPortPtr->EngineData.EngineType == MpioPcieEngine) {
      switch (MpioPortPtr->Port.LinkSpeedCapability) {
        case PcieGenMaxSupported:
          SmbiosRecord->SlotType = SlotTypePciExpressGen4;
          break;
        case PcieGen1:
          SmbiosRecord->SlotType = SlotTypePciExpress;
          break;
        case PcieGen2:
          SmbiosRecord->SlotType = SlotTypePciExpressGen2;
          break;
        case PcieGen3:
          SmbiosRecord->SlotType = SlotTypePciExpressGen3;
          break;
        case PcieGen4:
          SmbiosRecord->SlotType = SlotTypePciExpressGen4;
          break;
        default:
          SmbiosRecord->SlotType = SlotTypePciExpressGen4;
          break;
      }
    } else {
      SmbiosRecord->SlotType = SlotTypeOther;
    }

    switch (MpioPortPtr->EngineData.MpioEndLane -
      MpioPortPtr->EngineData.MpioStartLane) {
      case 15:
        SmbiosRecord->SlotDataBusWidth = SlotDataBusWidth16X;
        SmbiosRecord->DataBusWidth = 16;
        break;
      case 7:
        SmbiosRecord->SlotDataBusWidth = SlotDataBusWidth8X;
        SmbiosRecord->DataBusWidth = 8;
        break;
      case 3:
        SmbiosRecord->SlotDataBusWidth = SlotDataBusWidth4X;
        SmbiosRecord->DataBusWidth = 4;
        break;
      case 1:
        SmbiosRecord->SlotDataBusWidth = SlotDataBusWidth2X;
        SmbiosRecord->DataBusWidth = 2;
        break;
      default:
        SmbiosRecord->SlotDataBusWidth = SlotDataBusWidth1X;
        SmbiosRecord->DataBusWidth = 1;
        break;
    }
    SmbiosRecord->CurrentUsage = SlotUsageUnknown;
    SlotNumInfo = MpioPortPtr->Port.SlotNum;
    Status = SlotBdfInfo(
               &SlotNumInfo,
               &SegInfo,
               &BusInfo,
               &DevFunInfo
               );
    if (EFI_ERROR(Status)) {
      DEBUG((DEBUG_ERROR, "Could not get SBDF information %r\n", Status));
    }

    SmbiosRecord->SlotLength = SlotLengthUnknown;
    SmbiosRecord->SlotID = MpioPortPtr->Port.SlotNum;
    SmbiosRecord->SlotCharacteristics1.CharacteristicsUnknown = 0x01;
    SmbiosRecord->SegmentGroupNum = SegInfo;
    SmbiosRecord->BusNum = BusInfo;
    SmbiosRecord->DevFuncNum = DevFunInfo;
    SmbiosRecord->PeerGroupingCount = 0;

    StringOffset = SmbiosRecord->Hdr.Length;

    CopyMem ((UINT8 *)SmbiosRecord + StringOffset, SlotDesignationStr, SlotDesStrLen);
    *SmbiosRecordPtr = SmbiosRecord;
  }
  return Status;
}

/**
  This function checks for system slot info and adds smbios record (Type 9).

  @param  Smbios                     The EFI_SMBIOS_PROTOCOL instance.

  @retval EFI_SUCCESS                All parameters were valid.
  @retval EFI_OUT_OF_RESOURCES       Resource not available.

**/
EFI_STATUS
EFIAPI
SystemSlotInfoFunction(
  IN  EFI_SMBIOS_PROTOCOL   *Smbios
  )
{
  /*
  EFI_STATUS                       Status;
  EFI_SMBIOS_HANDLE                SmbiosHandle;
  SMBIOS_TABLE_TYPE9               *SmbiosRecord;
  AMD_CPM_TABLE_PROTOCOL           *CpmTableProtocolPtr;
  AMD_CPM_Mpio_TOPOLOGY_TABLE      *MpioTopologyTablePtr2[2];
  UINTN                            MpioPortIdx;
  UINTN                            SocketIdx;

  if (Smbios == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->LocateProtocol (
                  &gAmdCpmTableProtocolGuid,
                  NULL,
                  (VOID**)&CpmTableProtocolPtr
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to locate AmdCpmTableProtocol: %r\n", Status));
    return Status;
  }

  MpioTopologyTablePtr2[0] = NULL;
  MpioTopologyTablePtr2[0] = CpmTableProtocolPtr->CommonFunction.GetTablePtr2 (
                               CpmTableProtocolPtr,
                               CPM_SIGNATURE_Mpio_TOPOLOGY
                               );

  MpioTopologyTablePtr2[1] = NULL;
  MpioTopologyTablePtr2[1] = CpmTableProtocolPtr->CommonFunction.GetTablePtr2 (
                               CpmTableProtocolPtr,
                               CPM_SIGNATURE_Mpio_TOPOLOGY_S1
                               );

  // Add Smbios System Slot information for all sockets present.
  for (SocketIdx = 0; SocketIdx < FixedPcdGet32(PcdAmdNumberOfPhysicalSocket); SocketIdx++ ) {

    if (MpioTopologyTablePtr2[SocketIdx] != NULL) {

      for (MpioPortIdx = 0; MpioPortIdx < AMD_Mpio_PORT_DESCRIPTOR_SIZE;
        MpioPortIdx++) {
        // Check if Slot is present
        if (MpioTopologyTablePtr2[SocketIdx]->Port[MpioPortIdx].Port.SlotNum > 0 &&
            MpioTopologyTablePtr2[SocketIdx]->Port[MpioPortIdx].Port.PortPresent == 1) {

          SmbiosRecord = NULL;
          Status = CreateSmbiosSystemSlotRecord (
                     &MpioTopologyTablePtr2[SocketIdx]->Port[MpioPortIdx],
                     &SmbiosRecord
                     );

          if (EFI_ERROR (Status)) {
            DEBUG ((DEBUG_ERROR, "%a: Smbios system slot error: Status=%r\n",
            __FUNCTION__, Status));
          } else {
            Status = AddCommonSmbiosRecord (
                       Smbios,
                       &SmbiosHandle,
                       (EFI_SMBIOS_TABLE_HEADER *) SmbiosRecord
                       );
          }

        if (SmbiosRecord != NULL) {
            FreePool(SmbiosRecord);
          }
        }
        // Terminate if last port found.
        if((MpioTopologyTablePtr2[SocketIdx]->Port[MpioPortIdx].Flags & 0x80000000)) {
          break;
        }
      }
    }
  }

  return Status;*/
  return EFI_SUCCESS;
}
