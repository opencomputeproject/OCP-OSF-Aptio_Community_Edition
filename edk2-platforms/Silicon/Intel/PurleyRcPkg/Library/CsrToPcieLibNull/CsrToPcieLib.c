/** @file

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/CsrToPcieAddress.h>

//////////////////////////////////////////////////////////////////////////
//
// USRA Csr to PCIE  Address Library
// This Lib provide the way use platform Library instance
//
//////////////////////////////////////////////////////////////////////////

/**
  This Lib Convert the logical address (CSR type, e.g. CPU ID, Boxtype, Box instance etc.) into physical address
  
  @param[in] Global               Global pointer
  @param[in] Virtual              Virtual address
  @param[in] Address              A pointer of the address of the USRA Address Structure
  @param[out] AlignedAddress      A pointer of aligned address converted from USRA address

  @retval NULL                    The function completed successfully.
  @retval <>NULL                  Return Error
**/
UINTN
EFIAPI
CsrGetPcieAlignAddress (
  IN VOID                     *Global,
  IN BOOLEAN                  Virtual,
  IN USRA_ADDRESS             *Address,
  OUT UINTN                   *AlignedAddress
  )
{
  USRA_ADDRESS PcieAddress;
  USRA_ZERO_ADDRESS_TYPE(PcieAddress, AddrTypePCIE);
  return 0;
};

