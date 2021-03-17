/** @file
  PCH Reset Platform Specific definitions.

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#ifndef _PCH_RESET_PLATFORM_SPECIFIC_H_
#define _PCH_RESET_PLATFORM_SPECIFIC_H_

#define PCH_PLATFORM_SPECIFIC_RESET_STRING   L"PCH_RESET"
#define PCH_RESET_DATA_STRING_MAX_LENGTH     (sizeof (PCH_PLATFORM_SPECIFIC_RESET_STRING) / sizeof (UINT16))

extern EFI_GUID gPchGlobalResetGuid;

typedef struct _RESET_DATA {
  CHAR16   Description[PCH_RESET_DATA_STRING_MAX_LENGTH];
  EFI_GUID Guid;
} PCH_RESET_DATA;

#endif // _PCH_RESET_PLATFORM_SPECIFIC_H_

