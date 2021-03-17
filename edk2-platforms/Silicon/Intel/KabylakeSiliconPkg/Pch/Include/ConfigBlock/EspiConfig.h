/** @file
  Espi policy

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#ifndef _ESPI_CONFIG_H_
#define _ESPI_CONFIG_H_

#define ESPI_CONFIG_REVISION 1
extern EFI_GUID gEspiConfigGuid;

#pragma pack (push,1)

/**
  This structure contains the policies which are related to ESPI.

  <b>Revision 1</b>:
  - Initial version. Add BmeMasterSlaveEnabled support.
**/
typedef struct {
  CONFIG_BLOCK_HEADER   Header;                   ///< Config Block Header
  /**
    eSPI Master and Slave BME settings.
    When TRUE, then the BME bit enabled in eSPI Master and Slave.
    0: FALSE, <b>1: TRUE </b>
  **/
  UINT32    BmeMasterSlaveEnabled :  1;
  UINT32    RsvdBits              : 31;     ///< Reserved bits
} PCH_ESPI_CONFIG;

#pragma pack (pop)

#endif // _ESPI_CONFIG_H_
