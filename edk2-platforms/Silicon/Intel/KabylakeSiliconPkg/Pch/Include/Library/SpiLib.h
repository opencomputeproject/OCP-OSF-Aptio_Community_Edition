/** @file
  Library to initialize SPI services for future SPI accesses.

Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SPI_LIB_H_
#define _SPI_LIB_H_

/**
  Initializes SPI for access from future services.

  @retval EFI_SUCCESS         The SPI service was initialized successfully.
  @retval EFI_OUT_OF_RESOUCES Insufficient memory available to allocate structures required for initialization.
  @retval Others              An error occurred initializing SPI services.

**/
EFI_STATUS
EFIAPI
SpiServiceInit (
  VOID
  );

#endif
