/******************************************************************************
 * Copyright (C) 2021-2024 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

/** @file
  Miscellaneous smbios data structures.
**/

#ifndef __SMBIOSMISC_LIB_H__
#define __SMBIOSMISC_LIB_H__

#include <IndustryStandard/SmBios.h>

typedef struct {
  CHAR8  IntDesiganatorStr[SMBIOS_STRING_MAX_LENGTH];
  CHAR8  ExtDesiganatorStr[SMBIOS_STRING_MAX_LENGTH];
} PORT_CONNECTOR_STR;

typedef struct {
  SMBIOS_TABLE_TYPE8  Type8Data;
  PORT_CONNECTOR_STR  DesinatorStr;
} SMBIOS_PORT_CONNECTOR_RECORD;

typedef struct {
  SMBIOS_TABLE_STRING  ReferenceDesignation;
  UINT8                DeviceType:7;
  UINT8                DeviceEnabled:1;
  UINT8                DeviceTypeInstance;
  UINT16               VendorId;
  UINT16               DeviceId;
  CHAR8                RefDesignationStr[SMBIOS_STRING_MAX_LENGTH];
} SMBIOS_ONBOARD_DEV_EXT_INFO_RECORD;

/**
  Calculate and output total number of port connector present on board.

  @param[out] NumPortConnector   Pointer to output variable.

  @retval EFI_SUCCESS            Function successfully executed.
  @retval EFI_INVALID_PARAMETER  If NumPortConnector == NULL.

**/
EFI_STATUS
EFIAPI
GetNumberOfPortConnectors (
  OUT UINT8   *NumPortConnector
  );

/**
  Output the Type8 Smbios related information for indexed port connector.

  @param[in]  PortConnectorIdNum  Port connector identifier number.
  @param[out] PortConnRecord      Pointer to output type8 port connector record.

  @retval EFI_SUCCESS             Function successfully executed.
  @retval EFI_INVALID_PARAMETER   If PortConnRecord == NULL.

**/
EFI_STATUS
EFIAPI
GetType8PortConnectorInfo (
  IN  UINT8                         PortConnectorIdNum,
  OUT SMBIOS_PORT_CONNECTOR_RECORD  *PortConnRecord
  );

/**
  Calculate and output total number of onboard devices present on board.

  @param[out] NumberOfDevices    Pointer to output variable.

  @retval EFI_SUCCESS            Function successfully executed.
  @retval EFI_INVALID_PARAMETER  If NumberOfDevices == NULL.

**/
EFI_STATUS
EFIAPI
GetNumberOfOnboardDevices (
  OUT UINT8   *NumberOfDevices
  );

/**
  Output the Type41 Smbios related information for indexed onboard device.

  @param[in]  DevIdx              Device identifier number.
  @param[out] DevExtInfoRecord    Pointer to output type41 port connector record.

  @retval EFI_SUCCESS             Function successfully executed.
  @retval EFI_INVALID_PARAMETER   If DevExtInfoRecord == NULL.

**/
EFI_STATUS
EFIAPI
GetType41OnboardDevExtInfo (
  IN  UINT8                              DevIdx,
  OUT SMBIOS_ONBOARD_DEV_EXT_INFO_RECORD *DevExtInfoRecord
  );

#endif
