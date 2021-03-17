/** @file

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef  _CPU_CSR_ACCESS_DEFINE_H_
#define  _CPU_CSR_ACCESS_DEFINE_H_

#include <CsrToPcieAddress.h>
#include <CpuPciAccessCommon.h>


typedef enum {
  BUS_CLASS = 0,
  DEVICE_CLASS = 1,
  FUNCTION_CLASS = 2
} BDF_CLASS;

UINT32
GetSegmentNumber (
  IN USRA_ADDRESS    *Address
  );

UINT32
GetBDFNumber (
  IN USRA_ADDRESS    *Address,
  CPU_CSR_ACCESS_VAR          *CpuCsrAccessVar,
  IN UINT8                    BDFType
  );

UINT32
GetCpuCsrAddress (
  UINT8    SocId,
  UINT8    BoxInst,
  UINT32   Offset,
  UINT8    *Size
  );

UINT32
GetMmcfgAddress(
  PSYSHOST host
  );

VOID
GetCpuCsrAccessVar_RC (
  PSYSHOST host,
  CPU_CSR_ACCESS_VAR *CpuCsrAccessVar
  );

#endif   // _CPU_CSR_ACCESS_DEFINE_H_
