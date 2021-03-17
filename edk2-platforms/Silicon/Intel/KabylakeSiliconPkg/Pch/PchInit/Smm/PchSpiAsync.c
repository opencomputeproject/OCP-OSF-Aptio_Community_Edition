/** @file
  PCH SPI Async SMI handler.

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#include "PchInitSmm.h"

///
/// Global variables
///
GLOBAL_REMOVE_IF_UNREFERENCED PCH_SMI_DISPATCH_PROTOCOL     *mPchSmiDispatchProtocol;

/**
  This hardware SMI handler will be run every time the flash write/earse happens.

  @param[in] DispatchHandle       Not used

**/
VOID
EFIAPI
PchSpiAsyncCallback (
  IN  EFI_HANDLE                              DispatchHandle
  )
{
  //
  // Dummy SMI handler
  //
}

/**
  This fuction install SPI ASYNC SMI handler.

  @retval EFI_SUCCESS             Initialization complete.
**/
EFI_STATUS
EFIAPI
InstallPchSpiAsyncSmiHandler (
  VOID
  )
{
  EFI_STATUS              Status;
  EFI_HANDLE              Handle;

  DEBUG ((DEBUG_INFO, "InstallPchSpiAsyncSmiHandler()\n"));

  ///
  /// Get the PCH SMM dispatch protocol
  ///
  mPchSmiDispatchProtocol = NULL;
  Status = gSmst->SmmLocateProtocol (&gPchSmiDispatchProtocolGuid, NULL, (VOID **) &mPchSmiDispatchProtocol);
  ASSERT_EFI_ERROR (Status);

  ///
  /// Register an SpiAsync callback function
  ///
  Handle = NULL;
  Status = mPchSmiDispatchProtocol->SpiAsyncRegister (
                                      mPchSmiDispatchProtocol,
                                      PchSpiAsyncCallback,
                                      &Handle
                                      );
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}

