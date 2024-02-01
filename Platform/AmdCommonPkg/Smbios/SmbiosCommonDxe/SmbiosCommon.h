/******************************************************************************
 * Copyright (C) 2021-2024 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

/** @file
  Smbios common header file.
**/

#ifndef _SMBIOS_COMMON_DRIVER_H
#define _SMBIOS_COMMON_DRIVER_H

#include <SilCommon.h>
#include <PiDxe.h>
#include <Protocol/Smbios.h>
#include <IndustryStandard/SmBios.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/PciSegmentLib.h>
#include <Include/MsrReg.h>
#include <Mpio/Common/MpioStructs.h>

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
  );

/**
  This function gets the Bus, Device and Segment number of a PCI device when Vendor ID, Device ID and instance
  are provided.

  @param  Smbios                     The EFI_SMBIOS_PROTOCOL instance.
  @param  VendorId                   Vendor ID of the PCI device to be provided.
  @param  DeviceId                   Device ID of the PCI device to be provided
  @param  Instance                   Instance of the PCI device. If more than one devices with same vendor
                                     and device ID is present, instance number is used.
  @param  Segment                    Segment number of the PCI device is assigned.
  @param  Bus                        Bus number of the PCI device is assigned.
  @param  Device                     Device number of the PCI device is assigned.
  @param  Functions                  Bits 0-7 of the Functions variable correspond to respective function numbers.
  @param  DeviceFound                Set to 1 if the device is found.

  @retval EFI_SUCCESS                All parameters were valid.
**/
EFI_STATUS
EFIAPI
GetBusDeviceInfo (
  IN  UINT16 *VendorId,
  IN  UINT16 *DeviceId,
  IN  UINT8  *Instance,
  OUT UINT16 *Segment,
  OUT UINT8  *Bus,
  OUT UINT8  *Device,
  OUT UINT8  *Functions,
  OUT UINT8  *DeviceFound
  );

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
  );

#endif
