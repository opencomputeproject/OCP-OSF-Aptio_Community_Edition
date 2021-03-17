/** @file
  WatchDog policy

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#ifndef _WATCH_DOG_CONFIG_H_
#define _WATCH_DOG_CONFIG_H_

#define WATCH_DOG_PREMEM_CONFIG_REVISION 1
extern EFI_GUID gWatchDogPreMemConfigGuid;

#pragma pack (push,1)

/**
  This policy clears status bits and disable watchdog, then lock the
  WDT registers.
  while WDT is designed to be disabled and locked by Policy,
  bios should not enable WDT by WDT PPI. In such case, bios shows the
  warning message but not disable and lock WDT register to make sure
  WDT event trigger correctly.
**/
typedef struct {
  CONFIG_BLOCK_HEADER   Header;         ///< Config Block Header
  UINT32    DisableAndLock    :  1;     ///< <b>(Test)</b> Set 1 to clear WDT status, then disable and lock WDT registers. <b>0: Disable</b>; 1: Enable.
  UINT32    RsvdBits          : 31;
} PCH_WDT_PREMEM_CONFIG;

#pragma pack (pop)

#endif // _WATCH_DOG_CONFIG_H_
