/** @file
  Header file for OC WDT Library.

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#ifndef _OC_WDT_LIB_H_
#define _OC_WDT_LIB_H_

/**
  Check for unexpected reset.
  If there was an unexpected reset, enforces WDT expiration.
**/
VOID
OcWdtResetCheck (
  VOID
  );

/**
  This function install WDT PPI

  @retval EFI_STATUS  Results of the installation of the WDT PPI
**/
EFI_STATUS
EFIAPI
OcWdtInit (
  VOID
  );

#endif
