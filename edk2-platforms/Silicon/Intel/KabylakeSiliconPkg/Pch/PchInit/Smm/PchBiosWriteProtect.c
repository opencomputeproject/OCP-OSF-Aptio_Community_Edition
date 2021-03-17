/** @file
  PCH BIOS Write Protect Driver.

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
GLOBAL_REMOVE_IF_UNREFERENCED PCH_TCO_SMI_DISPATCH_PROTOCOL     *mPchTcoSmiDispatchProtocol;
GLOBAL_REMOVE_IF_UNREFERENCED UINTN                             mSpiRegBase;
GLOBAL_REMOVE_IF_UNREFERENCED PCH_ESPI_SMI_DISPATCH_PROTOCOL    *mEspiSmmDispatchProtocol;
GLOBAL_REMOVE_IF_UNREFERENCED UINTN                             mLpcRegBase;

/**
  This hardware SMI handler will be run every time the BIOS Write Enable bit is set.

  @param[in] DispatchHandle       Not used

**/
VOID
EFIAPI
PchSpiBiosWpCallback (
  IN  EFI_HANDLE                              DispatchHandle
  )
{
  //
  // Disable BIOSWE bit to protect BIOS
  //
  MmioAnd8 ((UINTN) (mSpiRegBase + R_PCH_SPI_BC), (UINT8) ~B_PCH_SPI_BC_WPD);
}

/**
  This hardware SMI handler will be run every time the BIOS Write Enable bit is set.

  @param[in] DispatchHandle       Not used

**/
VOID
EFIAPI
PchLpcBiosWpCallback (
  IN  EFI_HANDLE                              DispatchHandle
  )
{
  //
  // Disable BIOSWE bit to protect BIOS
  //
  MmioAnd8 ((UINTN) (mLpcRegBase + R_PCH_LPC_BC), (UINT8) ~B_PCH_LPC_BC_WPD);
}

/**
  Entry point for Pch Bios Write Protect driver.

  @param[in] ImageHandle          Image handle of this driver.
  @param[in] SystemTable          Global system service table.

  @retval EFI_SUCCESS             Initialization complete.
**/
EFI_STATUS
EFIAPI
InstallPchBiosWriteProtect (
  IN EFI_HANDLE            ImageHandle,
  IN EFI_SYSTEM_TABLE      *SystemTable
  )
{
  EFI_STATUS              Status;
  EFI_HANDLE              Handle;

  DEBUG ((DEBUG_INFO, "InstallPchBiosWriteProtect()\n"));

  if (mPchConfigHob->LockDown.BiosLock != TRUE) {
    return EFI_SUCCESS;
  }

  mSpiRegBase = MmPciBase (
                  DEFAULT_PCI_BUS_NUMBER_PCH,
                  PCI_DEVICE_NUMBER_PCH_SPI,
                  PCI_FUNCTION_NUMBER_PCH_SPI
                  );

  mLpcRegBase = MmPciBase (
                   DEFAULT_PCI_BUS_NUMBER_PCH,
                   PCI_DEVICE_NUMBER_PCH_LPC,
                   PCI_FUNCTION_NUMBER_PCH_LPC
                   );

  DEBUG ((DEBUG_INFO, "Installing BIOS Write Protect SMI handler\n"));

  //
  // Get the PCH TCO SMM dispatch protocol
  //
  mPchTcoSmiDispatchProtocol = NULL;
  Status = gSmst->SmmLocateProtocol (&gPchTcoSmiDispatchProtocolGuid, NULL, (VOID **) &mPchTcoSmiDispatchProtocol);
  ASSERT_EFI_ERROR (Status);
  //
  // Always register an SPI BiosWp callback function to handle TCO BIOSWR SMI
  // NOTE: No matter the BIOS resides behind SPI or not, it needs to handle the SPI BIOS WP SMI
  //       to avoid SMI deadloop on SPI WPD write.
  //
  Handle = NULL;
  Status = mPchTcoSmiDispatchProtocol->SpiBiosWpRegister (
                                         mPchTcoSmiDispatchProtocol,
                                         PchSpiBiosWpCallback,
                                         &Handle
                                         );
  ASSERT_EFI_ERROR (Status);

    //
    // Register an LPC BiosWp callback function to handle TCO BIOSWR SMI
    //
    Handle = NULL;
    Status = mPchTcoSmiDispatchProtocol->LpcBiosWpRegister (
                                           mPchTcoSmiDispatchProtocol,
                                           PchLpcBiosWpCallback,
                                           &Handle
                                           );
    ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}

