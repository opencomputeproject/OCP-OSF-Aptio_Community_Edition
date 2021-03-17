/** @file
  Prototype of the SiPolicyLib library.

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SI_POLICY_LIB_H_
#define _SI_POLICY_LIB_H_

#include <Ppi/SiPolicy.h>

/**
  Print whole SI_PREMEM_POLICY_PPI and serial out.

  @param[in] SiPreMemPolicyPpi          The RC PREMEM Policy PPI instance
**/
VOID
EFIAPI
SiPreMemPrintPolicyPpi (
  IN  SI_PREMEM_POLICY_PPI          *SiPreMemPolicyPpi
  );

/**
  Print whole SI_POLICY_PPI and serial out.

  @param[in] SiPolicyPpi          The RC Policy PPI instance
**/
VOID
EFIAPI
SiPrintPolicyPpi (
  IN  SI_POLICY_PPI          *SiPolicyPpi
  );

/**
  SiCreatePreMemConfigBlocks creates the config blocksg of Silicon Policy.
  It allocates and zero out buffer, and fills in the Intel default settings.

  @param[out] SiPreMemPolicyPpi   The pointer to get Silicon PREMEM Policy PPI instance

  @retval EFI_SUCCESS             The policy default is initialized.
  @retval EFI_OUT_OF_RESOURCES    Insufficient resources to create buffer
**/
EFI_STATUS
EFIAPI
SiCreatePreMemConfigBlocks (
  OUT  SI_PREMEM_POLICY_PPI         **SiPreMemPolicyPpi
  );

/**
  SiCreateConfigBlocks creates the config blocksg of Silicon Policy.
  It allocates and zero out buffer, and fills in the Intel default settings.

  @param[out] SiPolicyPpi         The pointer to get Silicon Policy PPI instance

  @retval EFI_SUCCESS             The policy default is initialized.
  @retval EFI_OUT_OF_RESOURCES    Insufficient resources to create buffer
**/
EFI_STATUS
EFIAPI
SiCreateConfigBlocks (
  OUT  SI_POLICY_PPI         **SiPolicyPpi
  );

/**
  SiPreMemInstallPolicyPpi installs SiPreMemPolicyPpi.
  While installed, RC assumes the Policy is ready and finalized. So please update and override
  any setting before calling this function.

  @param[in] SiPreMemPolicyPpi   The pointer to Silicon PREMEM Policy PPI instance

  @retval EFI_SUCCESS            The policy is installed.
  @retval EFI_OUT_OF_RESOURCES   Insufficient resources to create buffer
**/
EFI_STATUS
EFIAPI
SiPreMemInstallPolicyPpi (
  IN  SI_PREMEM_POLICY_PPI          *SiPreMemPolicyPpi
  );

/**
  SiInstallPolicyPpi installs SiPolicyPpi.
  While installed, RC assumes the Policy is ready and finalized. So please update and override
  any setting before calling this function.

  @param[in] SiPolicyPpi         The pointer to Silicon Policy PPI instance

  @retval EFI_SUCCESS            The policy is installed.
  @retval EFI_OUT_OF_RESOURCES   Insufficient resources to create buffer
**/
EFI_STATUS
EFIAPI
SiInstallPolicyPpi (
  IN  SI_POLICY_PPI          *SiPolicyPpi
  );

/**
  Print out all silicon policy information.

  @param[in] SiPolicyPpi         The pointer to Silicon Policy PPI instance

**/
VOID
DumpSiPolicy (
  IN  SI_POLICY_PPI *SiPolicyPpi
  );
#endif // _SI_PREMEM_POLICY_LIB_H_
