/** @file
  Silicon DXE policy.

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
 
#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/SiliconPolicyInitLib.h>
#include <Library/SiliconPolicyUpdateLib.h>


/**
  Silicon Policy Init DXE module entry point
  
  @param[in] ImageHandle       Image handle of this driver.
  @param[in] SystemTable       Global system service table.

  @retval     EFI_SUCCESS          The function completes successfully
**/
EFI_STATUS
EFIAPI
SiliconPolicyDxeEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  VOID  *Policy;

  Policy = SiliconPolicyInitLate (NULL);
  SiliconPolicyUpdateLate (Policy);
  SiliconPolicyDoneLate (Policy);

  return EFI_SUCCESS;
}