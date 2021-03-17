/** @file
  This is the driver that initializes the Intel System Agent.

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#include "SaInitDxe.h"
#include "SaInit.h"
#include <Library/MmPciLib.h>
#include <MemInfoHob.h>

/**
  SystemAgent Dxe Initialization.

  @param[in] ImageHandle             Handle for the image of this driver
  @param[in] SystemTable             Pointer to the EFI System Table

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_OUT_OF_RESOURCES    No enough buffer to allocate
**/
EFI_STATUS
EFIAPI
SaInitEntryPointDxe (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                Status;
  VOID                      *Registration;

  DEBUG ((DEBUG_INFO, "SaInitDxe Start\n"));


  Status = SaAcpiInit (ImageHandle);

  ///
  /// Create PCI Enumeration Completed callback for SA
  ///
  EfiCreateProtocolNotifyEvent (
    &gEfiPciEnumerationCompleteProtocolGuid,
    TPL_CALLBACK,
    SaPciEnumCompleteCallback,
    NULL,
    &Registration
    );

  DEBUG ((DEBUG_INFO, "SaInitDxe End\n"));

  return EFI_SUCCESS;
}

/**
  This function gets registered as a callback to perform SA initialization before EndOfDxe

  @param[in] Event     - A pointer to the Event that triggered the callback.
  @param[in] Context   - A pointer to private data registered with the callback function.
**/
VOID
EFIAPI
SaPciEnumCompleteCallback (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
  EFI_STATUS          Status;
  VOID                *ProtocolPointer;

  DEBUG ((DEBUG_INFO, "SaPciEnumCompleteCallback Start\n"));
  ///
  /// Check if this is first time called by EfiCreateProtocolNotifyEvent() or not,
  /// if it is, we will skip it until real event is triggered
  ///
  Status = gBS->LocateProtocol (&gEfiPciEnumerationCompleteProtocolGuid, NULL, (VOID **) &ProtocolPointer);
  if (EFI_SUCCESS != Status) {
    return;
  }

  gBS->CloseEvent (Event);

  UpdateDmarEndOfDxe ();

  return;
}

