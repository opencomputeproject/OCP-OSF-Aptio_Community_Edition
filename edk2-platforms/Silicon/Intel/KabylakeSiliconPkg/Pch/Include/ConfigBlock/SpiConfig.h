/** @file
  Spi policy

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#ifndef _SPI_CONFIG_H_
#define _SPI_CONFIG_H_

#define SPI_CONFIG_REVISION 1
extern EFI_GUID gSpiConfigGuid;

#pragma pack (push,1)

/**
  This structure contains the policies which are related to SPI.
**/
typedef struct {
  CONFIG_BLOCK_HEADER   Header;                   ///< Config Block Header
  /**
    Force to show SPI controller.
    <b>0: FALSE</b>, 1: TRUE
    NOTE: For Windows OS, it MUST BE false. It's optional for other OSs.
  **/
  UINT32    ShowSpiController           :  1;
  UINT32    RsvdBits                    : 31;     ///< Reserved bits
} PCH_SPI_CONFIG;

#pragma pack (pop)

#endif // _SPI_CONFIG_H_
