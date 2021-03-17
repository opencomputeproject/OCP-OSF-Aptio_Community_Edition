/** @file

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SEC_BOARD_INIT_LIB_H_
#define _SEC_BOARD_INIT_LIB_H_

#include <PiPei.h>
#include <Uefi.h>

/**
  This is stackless function in 32bit.

  return address - ESP.
  All other registers can be used.
**/
VOID
EFIAPI
BoardBeforeTempRamInit (
  VOID
  );

EFI_STATUS
EFIAPI
BoardAfterTempRamInit (
  VOID
  );

#endif
