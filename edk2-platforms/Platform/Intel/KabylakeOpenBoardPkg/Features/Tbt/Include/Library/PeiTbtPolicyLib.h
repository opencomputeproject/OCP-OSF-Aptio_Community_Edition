/** @file
  Prototype of the PeiTbtPolicyLib library.

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#ifndef _PEI_TBT_POLICY_LIB_H_
#define _PEI_TBT_POLICY_LIB_H_

/**
  Install Tbt Policy

  @retval EFI_SUCCESS                   The policy is installed.
  @retval EFI_OUT_OF_RESOURCES          Insufficient resources to create buffer

**/
EFI_STATUS
EFIAPI
InstallPeiTbtPolicy (
  VOID
  );

/**
  Update PEI TBT Policy
**/
VOID
EFIAPI
UpdatePeiTbtPolicy (
  VOID
  );

/**
  Print PEI TBT Policy
**/
VOID
EFIAPI
TbtPrintPeiPolicyConfig (
  VOID
  );
#endif // _DXE_TBT_POLICY_LIB_H_
