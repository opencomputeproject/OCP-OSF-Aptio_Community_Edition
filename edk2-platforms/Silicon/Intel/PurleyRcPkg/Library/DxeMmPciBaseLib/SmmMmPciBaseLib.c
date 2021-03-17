/** @file

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiDxe.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Protocol/SiliconRegAccess.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>

USRA_PROTOCOL     *mPciUsra   = NULL;

/**
  The constructor function initialize UsraProtocol.
  @retval EFI_SUCCESS   The constructor always returns EFI_SUCCESS.

**/
EFI_STATUS
EFIAPI
SmmMmPciLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  if (mPciUsra == NULL) 
  {
    //
    // USRA protocol need to be installed before the module access USRA.
    // 
    Status = gSmst->SmmLocateProtocol (&gUsraProtocolGuid, NULL, &mPciUsra);
    ASSERT_EFI_ERROR (Status);
    ASSERT (mPciUsra != NULL);
  }
  return EFI_SUCCESS;
}

/**
  This procedure will get PCIE address
  
  @param[in] Bus                  Pci Bus Number
  @param[in] Device               Pci Device Number
  @param[in] Function             Pci Function Number

  @retval PCIE address
**/
UINTN
MmPciBase (
  IN UINT32                       Bus,
  IN UINT32                       Device,
  IN UINT32                       Function
)
{
  USRA_ADDRESS Address;
  USRA_PCIE_ADDRESS(Address, UsraWidth32, Bus, Device, Function, 0);
  return mPciUsra->GetRegAddr (&Address);
}

/**
  This procedure will get PCIE address
  
  @param[in] Seg                  Pcie Segment Number
  @param[in] Bus                  Pcie Bus Number
  @param[in] Device               Pcie Device Number
  @param[in] Function             Pcie Function Number

  @retval PCIE address
**/
UINTN
MmPciAddress(
IN UINT32                       Seg,
IN UINT32                       Bus,
IN UINT32                       Device,
IN UINT32                       Function,
IN UINT32                       Register
)
{
  USRA_ADDRESS Address;
  USRA_PCIE_SEG_ADDRESS(Address, UsraWidth32, Seg, Bus, Device, Function, Register);
  return mPciUsra->GetRegAddr (&Address);
}
