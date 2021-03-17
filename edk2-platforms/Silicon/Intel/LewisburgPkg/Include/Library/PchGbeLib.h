/** @file

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _PCH_GBE_LIB_H_
#define _PCH_GBE_LIB_H_

/**
  Check whether GbE region is valid
  Check SPI region directly since GBE might be disabled in SW.

  @retval TRUE                    Gbe Region is valid
  @retval FALSE                   Gbe Region is invalid
**/
BOOLEAN
PchIsGbeRegionValid (
  VOID
  );

/**
  Returns GbE over PCIe port number based on a soft strap.

  @return                         Root port number (1-based)
  @retval 0                       GbE over PCIe disabled
**/
UINT32
PchGetGbePortNumber (
  VOID
  );

/**
  Check whether LAN controller is enabled in the platform.

  @retval TRUE                    GbE is enabled
  @retval FALSE                   GbE is disabled
**/
BOOLEAN
PchIsGbePresent (
  VOID
  );

/**
  Check whether LAN controller is enabled in the platform.

  @deprecated Use PchIsGbePresent instead.

  @retval TRUE                    GbE is enabled
  @retval FALSE                   GbE is disabled
**/
BOOLEAN
PchIsGbeAvailable (
  VOID
  );

#endif // _PCH_GBE_LIB_H_