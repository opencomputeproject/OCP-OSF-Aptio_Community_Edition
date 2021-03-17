/** @file
  IPMI FRU Driver.

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/IpmiCommandLib.h>
#include <IndustryStandard/Ipmi.h>

EFI_STATUS
InitializeFru (
  IN EFI_HANDLE             ImageHandle,
  IN EFI_SYSTEM_TABLE       *SystemTable
  )
/*++

Routine Description:

  Initialize SM Redirection Fru Layer

Arguments:

  ImageHandle - ImageHandle of the loaded driver
  SystemTable - Pointer to the System Table

Returns:

  EFI_STATUS

--*/
{
  EFI_STATUS                                 Status;
  IPMI_GET_DEVICE_ID_RESPONSE                ControllerInfo;
  IPMI_GET_FRU_INVENTORY_AREA_INFO_REQUEST   GetFruInventoryAreaInfoRequest;
  IPMI_GET_FRU_INVENTORY_AREA_INFO_RESPONSE  GetFruInventoryAreaInfoResponse;

  //
  //  Get all the SDR Records from BMC and retrieve the Record ID from the structure for future use.
  //
  Status = IpmiGetDeviceId (&ControllerInfo);
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR, "!!! IpmiFru  IpmiGetDeviceId Status=%x\n", Status));
    return Status;
  }

  DEBUG((DEBUG_ERROR, "!!! IpmiFru  FruInventorySupport %x\n", ControllerInfo.DeviceSupport.Bits.FruInventorySupport));

  if (ControllerInfo.DeviceSupport.Bits.FruInventorySupport) {
    GetFruInventoryAreaInfoRequest.DeviceId = 0;
    Status = IpmiGetFruInventoryAreaInfo (&GetFruInventoryAreaInfoRequest, &GetFruInventoryAreaInfoResponse);
    if (EFI_ERROR (Status)) {
      DEBUG((DEBUG_ERROR, "!!! IpmiFru  IpmiGetFruInventoryAreaInfo Status=%x\n", Status));
      return Status;
    }
    DEBUG((DEBUG_ERROR, "!!! IpmiFru  InventoryAreaSize=%x\n", GetFruInventoryAreaInfoResponse.InventoryAreaSize));
  }

  return EFI_SUCCESS;
}
