/** @file
  Struct definition for CpuPrivateData.

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#ifndef _CPU_PRIVATE_DATA_H_
#define _CPU_PRIVATE_DATA_H_

#include <CpuInitDataHob.h>

///
/// CPU Private Data saved and restored for S3.
///
typedef struct {
  UINT64                    ProcessorTraceAddress[MAX_PROCESSOR_THREADS];
  UINT32                    S3BspMtrrTablePointer;
} CPU_PRIVATE_DATA;

#endif
