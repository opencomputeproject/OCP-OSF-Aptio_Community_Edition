/** @file
  Header file for the SaPolicyInitDxe Driver.

Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#ifndef _SA_POLICY_INIT_DXE_H_
#define _SA_POLICY_INIT_DXE_H_

#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/SaPolicy.h>
#include <Library/DxeSaPolicyLib.h>

#include <SaAccess.h>


/**
  <b>SA DXE Policy Driver Entry Point</b> \n
  - <b>Introduction</b> \n
    System Agent DXE drivers behavior can be controlled by platform policy without modifying reference code directly.
    Platform policy Protocol is initialized with default settings in this funciton.
    This policy Protocol has to be initialized prior to System Agent initialization DXE drivers execution.

  - @pre
    - Runtime variable service should be ready if policy initialization required.

  - @result
    SA_POLICY_PROTOCOL will be installed successfully and ready for System Agent reference code use.

  - <b>Porting Recommendations</b> \n
    Policy should be initialized basing on platform design or user selection (like BIOS Setup Menu)

  @param[in] ImageHandle - Image handle of this driver.

  @retval EFI_SUCCESS           Initialization complete.
  @exception EFI_UNSUPPORTED    The chipset is unsupported by this driver.
  @retval EFI_OUT_OF_RESOURCES  Do not have enough resources to initialize the driver.
  @retval EFI_DEVICE_ERROR      Device error, driver exits abnormally.
**/
EFI_STATUS
EFIAPI
SaPolicyInitDxe (
  IN EFI_HANDLE           ImageHandle
  );

/**
  Get data for platform policy from setup options.

  @param[in] SaPolicy                  The pointer to get SA Policy protocol instance

  @retval EFI_SUCCESS                  Operation success.

**/
EFI_STATUS
EFIAPI
UpdateDxeSaPolicy (
  IN OUT  SA_POLICY_PROTOCOL    *SaPolicy
  );

#endif
