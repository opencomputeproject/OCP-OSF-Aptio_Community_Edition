/** @file

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _TEST_POINT_PRIVATE_H_
#define _TEST_POINT_PRIVATE_H_

#include <Uefi.h>
#include <PiSmm.h>
#include <Library/TestPointCheckLib.h>
#include <Library/DebugLib.h>

#define TEST_POINT_SMM_COMMUNICATION_VERSION                    0x1
#define TEST_POINT_SMM_COMMUNICATION_FUNC_ID_UEFI_GCD_MAP_INFO  0x1

typedef struct {
  UINT32     Version;
  UINT32     FuncId;
  UINT64     Size;
} TEST_POINT_SMM_COMMUNICATION_HEADER;

typedef struct {
  TEST_POINT_SMM_COMMUNICATION_HEADER  Header;
  UINT64                               UefiMemoryMapOffset;
  UINT64                               UefiMemoryMapSize;
  UINT64                               GcdMemoryMapOffset;
  UINT64                               GcdMemoryMapSize;
  UINT64                               GcdIoMapOffset;
  UINT64                               GcdIoMapSize;
  UINT64                               UefiMemoryAttributeTableOffset;
  UINT64                               UefiMemoryAttributeTableSize;
} TEST_POINT_SMM_COMMUNICATION_UEFI_GCD_MAP_INFO;

#define TEST_POINT_SMM_COMMUNICATION_GUID { \
  0x9cfa432a, 0x17cd, 0x4eb7, { 0x96, 0x54, 0x2e, 0xb2, 0x5, 0x91, 0xef, 0x8f } \
  }

extern EFI_GUID  mTestPointSmmCommunciationGuid;

#endif