/******************************************************************************
 * Copyright (C) 2021-2024 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

/** @file
  Smbios common entry point.
**/

#include "SmbiosCommon.h"

EFI_STATUS
EFIAPI
IpmiDeviceInformation(
  IN  EFI_SMBIOS_PROTOCOL   *Smbios
  );

EFI_STATUS
EFIAPI
SystemSlotInfoFunction(
  IN  EFI_SMBIOS_PROTOCOL   *Smbios
  );

EFI_STATUS
EFIAPI
PortConnectorInfoFunction (
  IN  EFI_SMBIOS_PROTOCOL   *Smbios
  );

EFI_STATUS
EFIAPI
OemStringsFunction (
  IN EFI_SMBIOS_PROTOCOL   *Smbios
  );

EFI_STATUS
EFIAPI
SystemCfgOptionsFunction(
  IN EFI_SMBIOS_PROTOCOL   *Smbios
  );

EFI_STATUS
EFIAPI
BiosLanguageInfoFunction(
  IN  EFI_SMBIOS_PROTOCOL   *Smbios
  );

EFI_STATUS
EFIAPI
OnboardDevExtInfoFunction (
  IN EFI_SMBIOS_PROTOCOL   *Smbios
  );

typedef
EFI_STATUS
(EFIAPI EFI_COMMON_SMBIOS_DATA_FUNCTION) (
  IN  EFI_SMBIOS_PROTOCOL  *Smbios
  );

typedef struct {
  EFI_COMMON_SMBIOS_DATA_FUNCTION *Function;
} EFI_COMMON_SMBIOS_DATA;

EFI_COMMON_SMBIOS_DATA mSmbiosCommonDataFuncTable[] = {
  {&IpmiDeviceInformation},
  {&SystemSlotInfoFunction},
  {&PortConnectorInfoFunction},
  {&OemStringsFunction},
  {&SystemCfgOptionsFunction},
  {&BiosLanguageInfoFunction}
};

/**
  EFI driver entry point. This driver parses mSmbiosCommonDataFuncTable
  structure and generates common platform smbios records.

  @param  ImageHandle     Handle for the image of this driver
  @param  SystemTable     Pointer to the EFI System Table

  @retval  EFI_SUCCESS    The data was successfully stored.

**/
EFI_STATUS
EFIAPI
SmbiosCommonEntryPoint(
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  UINTN                Index;
  EFI_STATUS           EfiStatus;
  EFI_SMBIOS_PROTOCOL  *Smbios;
  EFI_EVENT            ProtocolNotifyEvent;
  VOID                 *Registration;

  DEBUG((DEBUG_ERROR, "%a: Entry.\n", __FUNCTION__));

  EfiStatus = gBS->LocateProtocol (
                     &gEfiSmbiosProtocolGuid,
                     NULL,
                     (VOID**)&Smbios
                     );
  if (EFI_ERROR(EfiStatus)) {
    DEBUG((DEBUG_ERROR, "Could not locate SMBIOS protocol.  %r\n", EfiStatus));
    return EfiStatus;
  }

  ProtocolNotifyEvent = EfiCreateProtocolNotifyEvent (
                          &gEfiPciEnumerationCompleteProtocolGuid,
                          TPL_CALLBACK,
                          OnPciEnumerationComplete,
                          NULL,
                          &Registration
                          );
  if (ProtocolNotifyEvent == NULL) {
      DEBUG ((DEBUG_ERROR, "Could not create PCI enumeration complete event\n"));
  }

  for (Index = 0; Index < sizeof(mSmbiosCommonDataFuncTable)/sizeof(mSmbiosCommonDataFuncTable[0]); ++Index) {
    EfiStatus = (*mSmbiosCommonDataFuncTable[Index].Function) (Smbios);
    if (EFI_ERROR(EfiStatus)) {
      // continue installing remaining tables if one table fails.
      DEBUG ((
        DEBUG_INFO,
        "Skip installing SMBIOS Table Index=%d, ReturnStatus=%r\n",
        Index,
        EfiStatus
        ));
      continue;
    }
  }

  return EFI_SUCCESS;
}

/**
  Add an SMBIOS record.

  @param  Smbios                The EFI_SMBIOS_PROTOCOL instance.
  @param  SmbiosHandle          A unique handle will be assigned to the SMBIOS record.
  @param  Record                The data for the fixed portion of the SMBIOS record. The format of the record is
                                determined by EFI_SMBIOS_TABLE_HEADER.Type. The size of the formatted area is defined
                                by EFI_SMBIOS_TABLE_HEADER.Length and either followed by a double-null (0x0000) or
                                a set of null terminated strings and a null.

  @retval EFI_SUCCESS           Record was added.
  @retval EFI_OUT_OF_RESOURCES  Record was not added due to lack of system resources.

**/
EFI_STATUS
AddCommonSmbiosRecord (
  IN EFI_SMBIOS_PROTOCOL        *Smbios,
  OUT EFI_SMBIOS_HANDLE         *SmbiosHandle,
  IN EFI_SMBIOS_TABLE_HEADER    *Record
  )
{
  *SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  return Smbios->Add (
                   Smbios,
                   NULL,
                   SmbiosHandle,
                   Record
                   );
}

/**
  PciEnumerationComplete Protocol notification event handler.

  @param[in] Event    Event whose notification function is being invoked.
  @param[in] Context  Pointer to the notification function's context.
**/
VOID
EFIAPI
OnPciEnumerationComplete (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS           EfiStatus;
  EFI_SMBIOS_PROTOCOL  *Smbios;

  EfiStatus = gBS->LocateProtocol (
                     &gEfiSmbiosProtocolGuid,
                     NULL,
                     (VOID**)&Smbios
                     );
  if (EFI_ERROR(EfiStatus)) {
    DEBUG((DEBUG_ERROR, "Could not locate SMBIOS protocol.  %r\n", EfiStatus));
  }

  // Install Type 41 when PCI enumeration is complete
  EfiStatus = OnboardDevExtInfoFunction (Smbios);
  if (EFI_ERROR(EfiStatus)) {
    DEBUG ((
      DEBUG_INFO,
      "Skip installing SMBIOS Table 41, ReturnStatus=%r\n",
      EfiStatus
      ));
  }
}
