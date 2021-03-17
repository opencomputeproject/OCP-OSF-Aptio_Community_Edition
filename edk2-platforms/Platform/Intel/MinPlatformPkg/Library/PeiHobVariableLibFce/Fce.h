/**@file

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __FCE_H__
#define __FCE_H__

extern EFI_GUID gDefaultDataFileGuid;
extern EFI_GUID gDefaultDataOptSizeFileGuid;

#pragma pack(1)

typedef struct {
  UINT16 DefaultId;
  UINT16 BoardId;
} DEFAULT_INFO;

typedef struct {
  UINT16 Offset;
  UINT8  Value;
} DATA_DELTA;

typedef struct {
  //
  // HeaderSize includes HeaderSize fields and DefaultInfo arrays
  //
  UINT16 HeaderSize;
  //
  // DefaultInfo arrays those have the same default setting.
  //
  DEFAULT_INFO DefaultInfo[1];
  //
  // Default data is stored as variable storage or the array of DATA_DELTA. 
  //
} DEFAULT_DATA;

#pragma pack()

#endif
