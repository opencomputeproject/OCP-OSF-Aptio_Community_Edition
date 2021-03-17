/** @file
  Header file for initialization of GT PowerManagement

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#ifndef _GRAPHICS_INIT_H_
#define _GRAPHICS_INIT_H_

#include <Library/DxeServicesTableLib.h>
#include <Guid/DxeServices.h>
#include <Protocol/PciHostBridgeResourceAllocation.h>
#include <Library/MmPciLib.h>
#include <SaAccess.h>
#include <Protocol/SaPolicy.h>
#include "SaInitDxe.h"
#include "SaInit.h"
#include <PchAccess.h>
#include <CpuRegs.h>
#include <Library/CpuPlatformLib.h>


/**
  Initialize GT ACPI tables

  @param[in] ImageHandle - Handle for the image of this driver
  @param[in] SaPolicy    - SA DXE Policy protocol

  @retval EFI_SUCCESS          - GT ACPI initialization complete
  @retval EFI_NOT_FOUND        - Dxe System Table not found.
  @retval EFI_OUT_OF_RESOURCES - Mmio not allocated successfully.
**/
EFI_STATUS
GraphicsInit (
  IN EFI_HANDLE             ImageHandle,
  IN SA_POLICY_PROTOCOL     *SaPolicy
  );

#endif
