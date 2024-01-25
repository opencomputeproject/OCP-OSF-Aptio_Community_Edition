/*****************************************************************************
 *
 * Copyright (C) 2020-2023 Advanced Micro Devices, Inc. All rights reserved.
 *
 *****************************************************************************/

#include <Library/SmbiosMiscLib.h>

SMBIOS_ONBOARD_DEV_EXT_INFO_RECORD gOnboardDevExtInfo[] = {
  {
    0x01,                              // Reference Designation
    OnBoardDeviceExtendedTypeEthernet, // 0x03, Device Type
    1,                                 // Device Enabled
    1,                                 // Device Instance
    0x14E4,                            // LOM ethernet controller vendor id
    0x165F,                            // LOM ethernet controller device id
    "Onboard Ethernet"                 // Designation String
  }
};

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
  )
{
  if (NumberOfDevices == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //Calculate total number of onboard devices
  *NumberOfDevices = sizeof (gOnboardDevExtInfo) / sizeof (SMBIOS_ONBOARD_DEV_EXT_INFO_RECORD);

  return EFI_SUCCESS;
}


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
  )
{
  UINT8 NumberOfDevices;

  if (DevExtInfoRecord == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //Calculate total number of onboard devices
  NumberOfDevices = sizeof (gOnboardDevExtInfo) / sizeof (SMBIOS_ONBOARD_DEV_EXT_INFO_RECORD);

  if (DevIdx >= NumberOfDevices) {
    return EFI_INVALID_PARAMETER;
  }

  *DevExtInfoRecord = gOnboardDevExtInfo[DevIdx];

  return EFI_SUCCESS;
}