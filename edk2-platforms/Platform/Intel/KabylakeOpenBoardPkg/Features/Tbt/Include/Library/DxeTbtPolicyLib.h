/** @file
  Prototype of the DxeTbtPolicyLib library.

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#ifndef _DXE_TBT_POLICY_LIB_H_
#define _DXE_TBT_POLICY_LIB_H_


/**
  Install TBT Policy.

  @param[in] ImageHandle                Image handle of this driver.

  @retval EFI_SUCCESS                   The policy is installed.
  @retval EFI_OUT_OF_RESOURCES          Insufficient resources to create buffer

**/
EFI_STATUS
EFIAPI
InstallTbtPolicy (
  IN  EFI_HANDLE                    ImageHandle
  );

/**
  Update Tbt Policy Callback.

  @param[in] Event         A pointer to the Event that triggered the callback.
  @param[in] Context       A pointer to private data registered with the callback function.

**/
VOID
EFIAPI
UpdateTbtPolicyCallback (
  VOID
  );

/**
  Print DXE TBT Policy
**/
VOID
TbtPrintDxePolicyConfig (
  VOID
  );
#endif // _DXE_TBT_POLICY_LIB_H_
