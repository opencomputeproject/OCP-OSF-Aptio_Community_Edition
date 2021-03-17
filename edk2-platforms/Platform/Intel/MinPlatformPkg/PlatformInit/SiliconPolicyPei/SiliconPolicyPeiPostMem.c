/** @file
  Silicon post-mem policy PEIM.

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/SiliconPolicyInitLib.h>
#include <Library/SiliconPolicyUpdateLib.h>


/**
  Silicon Policy Init after memory PEI module entry point

  @param[in]  FileHandle           Not used.
  @param[in]  PeiServices          General purpose services available to every PEIM.

  @retval     EFI_SUCCESS          The function completes successfully
**/
EFI_STATUS
EFIAPI
SiliconPolicyPeiPostMemEntryPoint (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  VOID  *Policy;

  Policy = SiliconPolicyInitPostMem (NULL);
  SiliconPolicyUpdatePostMem (Policy);
  SiliconPolicyDonePostMem (Policy);

  return EFI_SUCCESS;
}