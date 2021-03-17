/** @file
  HSIO policy

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#ifndef _HSIO_CONFIG_H_
#define _HSIO_CONFIG_H_

#define HSIO_PREMEM_CONFIG_REVISION 1
extern EFI_GUID gHsioPreMemConfigGuid;

#pragma pack (push,1)

/**
  The PCH_HSIO_PREMEM_CONFIG block provides HSIO message related settings.
**/
typedef struct {
  CONFIG_BLOCK_HEADER   Header;         ///< Config Block Header

  /**
  <b>(Test)</b>
  0- Disable, disable will prevent the HSIO version check and ChipsetInit HECI message from being sent
  <b>1- Enable</b> ChipsetInit HECI message
  **/
  UINT32    ChipsetInitMessage :  1;
  /**
  <b>(Test)</b>
  <b>0- Disable</b>
  1- Enable When eanbled, this is used to bypass the reset after ChipsetInit HECI message.
  **/
  UINT32    BypassPhySyncReset :  1;
  UINT32    RsvdBits           : 30;
} PCH_HSIO_PREMEM_CONFIG;

#pragma pack (pop)

#endif // _HSIO_CONFIG_H_
