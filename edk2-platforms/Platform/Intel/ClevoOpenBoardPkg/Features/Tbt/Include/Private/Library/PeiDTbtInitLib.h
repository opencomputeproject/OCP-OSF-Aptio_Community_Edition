/**@file
  PEI DTBT Init Dispatch library Header file

Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#ifndef __PEI_DTBT_INIT_LIB_H__
#define __PEI_DTBT_INIT_LIB_H__

#include <Ppi/PeiTbtPolicy.h>

/**
  set tPCH25 Timing to 10 ms for DTBT.

  @param[in]  PEI_TBT_POLICY   PeiTbtConfig

  @retval     EFI_SUCCESS  The function completes successfully
  @retval     EFI_UNSUPPORTED  dTBT is not supported.
**/
EFI_STATUS
EFIAPI
DTbtSetTPch25Timing (
  IN  PEI_TBT_POLICY  *PeiTbtConfig
);

/**
  Do ForcePower for DTBT Controller

  @param[in]  PEI_TBT_POLICY   PeiTbtConfig

  @retval     EFI_SUCCESS  The function completes successfully
  @retval     EFI_UNSUPPORTED  dTBT is not supported.
**/
EFI_STATUS
EFIAPI
DTbtForcePower (
  IN  PEI_TBT_POLICY  *PeiTbtConfig
);

/**
  Clear VGA Registers for DTBT.

  @param[in]  PEI_TBT_POLICY   PeiTbtConfig

  @retval     EFI_SUCCESS  The function completes successfully
  @retval     EFI_UNSUPPORTED  dTBT is not supported.
**/
EFI_STATUS
EFIAPI
DTbtClearVgaRegisters (
  IN  PEI_TBT_POLICY  *PeiTbtConfig
);

/**
  Exectue Mail box command "Boot On".

  @param[in]  PEI_TBT_POLICY   PeiTbtConfig

  @retval     EFI_SUCCESS  The function completes successfully
  @retval     EFI_UNSUPPORTED  dTBT is not supported.
**/
EFI_STATUS
EFIAPI
DTbtBootOn (
  IN  PEI_TBT_POLICY  *PeiTbtConfig
);

/**
  Exectue Mail box command "USB On".

  @param[in]  PEI_TBT_POLICY   PeiTbtConfig

  @retval     EFI_SUCCESS  The function completes successfully
  @retval     EFI_UNSUPPORTED  dTBT is not supported.
**/
EFI_STATUS
EFIAPI
DTbtUsbOn (
  IN  PEI_TBT_POLICY  *PeiTbtConfig
);

/**
  Exectue Mail box command "Sx Exit".

  @param[in]  PEI_TBT_POLICY   PeiTbtConfig

  @retval     EFI_SUCCESS  The function completes successfully
  @retval     EFI_UNSUPPORTED  dTBT is not supported.
**/
EFI_STATUS
EFIAPI
DTbtSxExitFlow (
  IN  PEI_TBT_POLICY  *PeiTbtConfig
);
/**
  Initialize Thunderbolt(TM)

  @retval     EFI_SUCCESS  The function completes successfully
  @retval     others
**/
EFI_STATUS
EFIAPI
TbtInit (
  VOID
);

#endif