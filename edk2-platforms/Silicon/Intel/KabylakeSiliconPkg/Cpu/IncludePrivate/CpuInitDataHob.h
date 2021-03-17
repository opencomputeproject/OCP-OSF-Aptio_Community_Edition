/** @file
  Struct and GUID definitions for CpuInitDataHob.

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#ifndef _CPU_INIT_DATA_HOB_H_
#define _CPU_INIT_DATA_HOB_H_

#include <Ppi/SiPolicy.h>

extern EFI_GUID gCpuInitDataHobGuid;

#define MAX_PROCESSOR_THREADS  0x40

///
/// CPU Configuration Structure passed from PEI to DXE phase
///
typedef struct {
  UINT32 ApHandoffManner           : 2;
  UINT32 ApIdleManner              : 2;
  UINT32 EnableDts                 : 2;
  UINT32 HdcControl                : 2;
  UINT32 Hwp                       : 2;
  UINT32 ConfigTdpBios             : 1;
  UINT32 RsvdBits                  :21;
  UINT8  SmmbaseSwSmiNumber;
  UINT8  Rsvd[3];
} CPU_CONFIG_DATA;

///
/// This HOB is used to pass only the required information from PEI for DXE consumption.
///
typedef struct {
  UINT32                 Revision;
  EFI_PHYSICAL_ADDRESS   CpuConfigData;     ///< CPU RC Config for DXE consumption
  EFI_PHYSICAL_ADDRESS   CpuGnvsPointer;    ///< CPU_GLOBAL_NVS_AREA Pointer.
  EFI_PHYSICAL_ADDRESS   MpData;            ///< Deprecated. Points to ACPI_CPU_DATA structure with multiprocessor data.
  EFI_PHYSICAL_ADDRESS   FvidTable;         ///< FVID Table.
  UINT32                 SiliconInfo;       ///< SILICON_INFO data
} CPU_INIT_DATA_HOB;

#endif
