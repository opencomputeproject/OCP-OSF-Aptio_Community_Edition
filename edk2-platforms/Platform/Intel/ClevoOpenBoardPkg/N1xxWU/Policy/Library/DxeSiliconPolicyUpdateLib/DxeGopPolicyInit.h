/** @file
Header file for the GopPolicyInitDxe Driver.

Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#ifndef _GOP_POLICY_INIT_DXE_H_
#define _GOP_POLICY_INIT_DXE_H_

#include <Protocol/FirmwareVolume2.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>


/**
Initialize GOP DXE Policy

@param[in] ImageHandle          Image handle of this driver.

@retval EFI_SUCCESS             Initialization complete.
@retval EFI_UNSUPPORTED         The chipset is unsupported by this driver.
@retval EFI_OUT_OF_RESOURCES    Do not have enough resources to initialize the driver.
@retval EFI_DEVICE_ERROR        Device error, driver exits abnormally.
**/
EFI_STATUS
EFIAPI
GopPolicyInitDxe(
  IN EFI_HANDLE           ImageHandle
  );

#endif
