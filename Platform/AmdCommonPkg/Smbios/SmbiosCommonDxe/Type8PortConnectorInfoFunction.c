/******************************************************************************
 * Copyright (C) 2021-2023 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

#include <Library/SmbiosMiscLib.h>
#include "SmbiosCommon.h"

/**
  This function adds port connector information smbios record (Type 8).

  @param  Smbios                     The EFI_SMBIOS_PROTOCOL instance.

  @retval EFI_SUCCESS                All parameters were valid.
  @retval EFI_OUT_OF_RESOURCES       Resource not available.
**/
EFI_STATUS
EFIAPI
PortConnectorInfoFunction (
  IN EFI_SMBIOS_PROTOCOL   *Smbios
  )
{
  EFI_STATUS                    Status;
  EFI_SMBIOS_HANDLE             SmbiosHandle;
  SMBIOS_TABLE_TYPE8            *SmbiosRecord;
  SMBIOS_PORT_CONNECTOR_RECORD  PortConnRecord;
  UINT8                         PortIdx;
  UINT8                         NumberOfPortConnector;
  UINTN                         StringOffset;
  CHAR8                         *IntPortConDesStr;
  UINTN                         IntPortConDesStrLen;
  CHAR8                         *ExtPortConDesStr;
  UINTN                         ExtPortConDesStrLen;

  if (Smbios == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //Get the total number of port connectors.
  Status = GetNumberOfPortConnectors (&NumberOfPortConnector);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "Could not get number of port connectors %r\n", Status));
    return Status;
  }

  if (NumberOfPortConnector == 0) {
    DEBUG((DEBUG_ERROR, "No port connectors found.\n"));
    return EFI_NOT_FOUND;
  }

  //Generate type8 smbios record for each connector and add it to Smbios table.
  for (PortIdx = 0; PortIdx < NumberOfPortConnector; PortIdx++) {

    Status = GetType8PortConnectorInfo(PortIdx, &PortConnRecord);
    if (EFI_ERROR(Status)) {
      DEBUG(
        (DEBUG_ERROR, "Could not get port connectors details for idx %d\n",PortIdx)
        );
      continue;
    }

    //Check whether Port connector designator strings are present or not.
    if (PortConnRecord.Type8Data.InternalReferenceDesignator != 0) {
       IntPortConDesStr = PortConnRecord.DesinatorStr.IntDesiganatorStr;
       IntPortConDesStrLen = AsciiStrLen (IntPortConDesStr) + 1;
    } else {
       IntPortConDesStr = NULL;
       IntPortConDesStrLen = 0;
    }

    if (PortConnRecord.Type8Data.ExternalReferenceDesignator != 0) {
       ExtPortConDesStr = PortConnRecord.DesinatorStr.ExtDesiganatorStr;
       ExtPortConDesStrLen = AsciiStrLen (ExtPortConDesStr) + 1;
    } else {
       ExtPortConDesStr = NULL;
       ExtPortConDesStrLen = 0;
    }

    SmbiosRecord = NULL;
    SmbiosRecord = AllocateZeroPool (
      sizeof (SMBIOS_TABLE_TYPE8) + IntPortConDesStrLen + ExtPortConDesStrLen + 1
      );

    if (SmbiosRecord == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      return Status;
    } else {

      SmbiosRecord->Hdr.Type = SMBIOS_TYPE_PORT_CONNECTOR_INFORMATION;
      SmbiosRecord->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE8);
      SmbiosRecord->Hdr.Handle = 0;

      SmbiosRecord->InternalReferenceDesignator =
            PortConnRecord.Type8Data.InternalReferenceDesignator;
      SmbiosRecord->InternalConnectorType =
            PortConnRecord.Type8Data.InternalConnectorType;
      SmbiosRecord->ExternalReferenceDesignator =
            PortConnRecord.Type8Data.ExternalReferenceDesignator;
      SmbiosRecord->ExternalConnectorType =
            PortConnRecord.Type8Data.ExternalConnectorType;
      SmbiosRecord->PortType =
            PortConnRecord.Type8Data.PortType;

      // Add strings to bottom of data block
      StringOffset = SmbiosRecord->Hdr.Length;
      if (IntPortConDesStr != NULL) {
        CopyMem (
            (UINT8 *)SmbiosRecord + StringOffset,
            IntPortConDesStr,
            IntPortConDesStrLen
            );
        StringOffset += IntPortConDesStrLen;
      }
      if (ExtPortConDesStr != NULL) {
        CopyMem (
            (UINT8 *)SmbiosRecord + StringOffset,
            ExtPortConDesStr,
            ExtPortConDesStrLen
            );
      }
      Status = AddCommonSmbiosRecord (
                   Smbios,
                   &SmbiosHandle,
                   (EFI_SMBIOS_TABLE_HEADER *) SmbiosRecord
                   );
      FreePool(SmbiosRecord);
    }
  }
  return Status;
}
