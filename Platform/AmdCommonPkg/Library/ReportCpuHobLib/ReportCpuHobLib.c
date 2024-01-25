/******************************************************************************
 * Copyright (C) 2021-2023 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

/* This file includes code originally published under the following license. */

/** @file
  Source code file for Report CPU HOB library.

Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Register/Intel/Cpuid.h>

#define DEFAULT_MAX_MEMORY_ADDRESS_LINES  52
#define DEFAULT_MAX_IO_ADDRESS_LINES      25

VOID
ReportCpuHob (
  VOID
  )
{
  UINT8   CpuAddressWidth;
  UINT32  RegEax;

  //
  // Create a CPU hand-off information
  //
  CpuAddressWidth = DEFAULT_MAX_MEMORY_ADDRESS_LINES;
  AsmCpuid (CPUID_EXTENDED_FUNCTION, &RegEax, NULL, NULL, NULL);
  if (RegEax >= CPUID_VIR_PHY_ADDRESS_SIZE) {
    AsmCpuid (CPUID_VIR_PHY_ADDRESS_SIZE, &RegEax, NULL, NULL, NULL);
    CpuAddressWidth = (UINT8)(RegEax & 0xFF);
  }
  DEBUG ((DEBUG_INFO, " Memory Address Width: %d\n", CpuAddressWidth));
  DEBUG ((DEBUG_INFO, " IO Address Width: %d\n", DEFAULT_MAX_IO_ADDRESS_LINES));

  BuildCpuHob (CpuAddressWidth, DEFAULT_MAX_IO_ADDRESS_LINES);
}
