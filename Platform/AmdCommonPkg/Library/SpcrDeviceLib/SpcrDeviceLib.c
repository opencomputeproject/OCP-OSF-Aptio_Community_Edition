/*****************************************************************************
 *
 * Copyright (C) 2021-2023 Advanced Micro Devices, Inc. All rights reserved.
 *
 *****************************************************************************/
/** @file
  Implementation for SpcrDeviceLib Library.
  SpcrDeviceLib is usd for Serial Port Console Redirection Table (SPCR) device.
**/

#include <Include/Uefi.h>
#include <Protocol/DevicePath.h>
#include <Library/UefiLib.h>
#include <Library/DevicePathLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>

EFI_GUID  TerminalTypeGuid[4] = {
  DEVICE_PATH_MESSAGING_PC_ANSI,
  DEVICE_PATH_MESSAGING_VT_100,
  DEVICE_PATH_MESSAGING_VT_100_PLUS,
  DEVICE_PATH_MESSAGING_VT_UTF8
};

/**
  Get a Serial Port device used for SPCR.

  @retval NULL       Fails to get the DevicePath
          DevicePath If success

**/
EFI_DEVICE_PATH_PROTOCOL*
EFIAPI
GetSpcrDevice (
  VOID
  )
{
  EFI_DEVICE_PATH_PROTOCOL  *VarConsole;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *TmpDevicePath;
  VENDOR_DEVICE_PATH        *Vendor;
  UINTN                     Size;
  BOOLEAN                   Found;
  UINTN                     Index;

  // look for supported terminal type GUID in the device path
  if (GetEfiGlobalVariable2 (L"ConOut", (VOID **) &VarConsole, NULL) == EFI_SUCCESS) {
    do {
      // Get the Single Device Path
      DevicePath = GetNextDevicePathInstance (&VarConsole, &Size);
      TmpDevicePath = DevicePath;
      Found = FALSE;
      while (!IsDevicePathEnd (TmpDevicePath)) {
        // search for terminal type
        Vendor = (VENDOR_DEVICE_PATH *) TmpDevicePath;
        for (Index = 0; Index < 4; Index++) {
          if (CompareGuid (&Vendor->Guid, &TerminalTypeGuid[Index])) {
            Found = TRUE;
            break;
          }
        }
        if (Found) {
          break;
        }
        TmpDevicePath = NextDevicePathNode (TmpDevicePath);
      }
      if (Found) {
        return (DevicePath);
      }
      FreePool (DevicePath);
    } while (VarConsole != NULL);
  }
  return NULL;
}
