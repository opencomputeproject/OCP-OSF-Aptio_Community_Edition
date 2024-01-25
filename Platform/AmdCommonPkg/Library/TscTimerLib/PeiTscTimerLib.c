/******************************************************************************
 * Copyright (C) 2017-2023 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

/* This file includes code originally published under the following license. */

/** @file
  PEI Timer Library implementation which uses the Time Stamp Counter in the processor.

  For AMD processors the time-stamp counter increments at a constant rate.
  That rate may be set by the maximum core-clock to bus-clock ratio of the processor or may be set by
  the maximum resolved frequency at which the processor is booted. The maximum resolved frequency may
  differ from the maximum qualified frequency of the processor.

  The specific processor configuration determines the behavior. Constant TSC behavior ensures that the
  duration of each clock tick is uniform and supports the use of the TSC as a wall clock timer even if
  the processor core changes frequency. This is the architectural behavior moving forward.

  A Processor's support for invariant TSC is indicated by CPUID.0x80000007.EDX[8].

  Copyright (c) 2009 - 2011, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution. The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiPei.h>
#include <Library/HobLib.h>
#include <Guid/TscFrequency.h>
#include "TscTimerLibInternal.h"

/**  Get TSC frequency from TSC frequency GUID HOB, if the HOB is not found, build it.

  @return The number of TSC counts per second.

**/
UINT64
InternalGetTscFrequency (
  VOID
  )
{
  EFI_HOB_GUID_TYPE       *GuidHob;
  VOID        *DataInHob;
  UINT64      TscFrequency;

  //
  // Get TSC frequency from TSC frequency GUID HOB.
  //
  GuidHob = GetFirstGuidHob (&gAmdTscFrequencyGuid);
  if (GuidHob != NULL) {
    DataInHob = GET_GUID_HOB_DATA (GuidHob);
    TscFrequency = * (UINT64 *) DataInHob;
    return TscFrequency;
  }

  //
  // TSC frequency GUID HOB is not found, build it.
  //

  TscFrequency = InternalCalculateTscFrequency ();
  //
  // TscFrequency is now equal to the number of TSC counts per second, build GUID HOB for it.
  //
  BuildGuidDataHob (
    &gAmdTscFrequencyGuid,
    &TscFrequency,
    sizeof (UINT64)
    );

  return TscFrequency;
}




