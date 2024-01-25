/**
  Copyright (c) 2023, American Megatrends International LLC.
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

/* This file includes code originally published under the following license. */

/** @file

Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>
#include <Sil-api.h>
#include <Mpio/MpioClass-api.h>
#include <Mpio/Common/MpioStructs.h>


MPIO_PORT_DESCRIPTOR MpioPortDescriptor[] = {
    { // P4 - x1 NIC
      0,
      MPIO_ENGINE_DATA_INITIALIZER (MpioPcieEngine, 135, 135, PcieHotplugBasic, 0),
      MPIO_PORT_DATA_INITIALIZER_PCIE (
        MpioPortEnabled,                      // Port Present
        0,                                    // Requested Device
        0,                                    // Requested Function
        PcieHotplugDisabled,                  // Hotplug
        PcieGenMaxSupported,                  // Max Link Speed
        PcieGenMaxSupported,                  // Max Link Capability
        AspmL1,                               // ASPM
        AspmDisabled,                         // ASPM L1.1 disabled
        AspmDisabled,                         // ASPM L1.2 disabled
        MpioClkPmSupportDisabled              // Clock PM
      )
    },
    { // P4 - x1 M.2
      0,
      MPIO_ENGINE_DATA_INITIALIZER (MpioPcieEngine, 128, 131, PcieHotplugBasic, 0),
      MPIO_PORT_DATA_INITIALIZER_PCIE (
        MpioPortEnabled,                      // Port Present
        0,                                    // Requested Device
        0,                                    // Requested Function
        PcieHotplugDisabled,                  // Hotplug
        PcieGenMaxSupported,                  // Max Link Speed
        PcieGenMaxSupported,                  // Max Link Capability
        AspmL1,                               // ASPM
        AspmDisabled,                         // ASPM L1.1 disabled
        AspmDisabled,                         // ASPM L1.2 disabled
        MpioClkPmSupportDisabled              // Clock PM
      )
    },
    { // P5 - x2 WAFL
      DESCRIPTOR_TERMINATE_LIST,
      MPIO_ENGINE_DATA_INITIALIZER (MpioPcieEngine, 132, 133, PcieHotplugBasic, 0),
      MPIO_PORT_DATA_INITIALIZER_PCIE (
        MpioPortEnabled,                      // Port Present
        0,                                    // Requested Device
        0,                                    // Requested Function
        PcieHotplugDisabled,                  // Hotplug
        PcieGenMaxSupported,                  // Max Link Speed
        PcieGenMaxSupported,                  // Max Link Capability
        AspmL1,                               // ASPM
        AspmDisabled,                         // ASPM L1.1 disabled
        AspmDisabled,                         // ASPM L1.2 disabled
        MpioClkPmSupportDisabled              // Clock PM
      )
    },
};

MPIO_COMPLEX_DESCRIPTOR MpioComplexDescriptor = {
  DESCRIPTOR_TERMINATE_LIST,
  0,
  MpioPortDescriptor,
  NULL,
  0,
  0,
  {0,0}
};
/**
 * PlatformSetMpioData
 *
 * @brief Set the MPIO input defaults based on platform type
 * @details
 *      MpioData  Pointer to Mpio Input Data block
 * @return EFI_SUCCESS or EFI_DEVICE_ERROR
 */
EFI_STATUS
PlatformSetMpioData (
  MPIOCLASS_INPUT_BLK           *MpioData
  )
{
  CopyMem (&MpioData->PcieTopologyData.PlatformData, &MpioComplexDescriptor, sizeof (MPIO_COMPLEX_DESCRIPTOR));
  MpioData->PcieTopologyData.PlatformData[0].PciePortList = MpioData->PcieTopologyData.PortList;
  CopyMem (MpioData->PcieTopologyData.PortList, &MpioPortDescriptor, sizeof (MpioPortDescriptor));

  return EFI_SUCCESS;
}