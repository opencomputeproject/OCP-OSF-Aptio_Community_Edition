/******************************************************************************
 * Copyright (C) 2017-2024 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

/* This file includes code originally published under the following license. */

/** @file
  The Timer Library implementation which uses the Time Stamp Counter in the processor.

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

#include "TscTimerLibInternal.h"
#include <Library/DebugLib.h>

// FCH::PM::TMRVALUE_ETMRVALUE Offset
#define FCH_PMIOA_REG508          0x508

/**  Calculate TSC frequency.

  The TSC counting frequency is determined by comparing how far it counts
  during a 1ms period as determined by the ACPI timer. The ACPI timer is
  used because it counts at a known frequency.
  If ACPI I/O space not enabled, this function will enable it. Then the
  TSC is sampled, followed by waiting for 3579 clocks of the ACPI timer, or 1ms.
  The TSC is then sampled again. The difference multiplied by 1000 is the TSC
  frequency. There will be a small error because of the overhead of reading
  the ACPI timer. An attempt is made to determine and compensate for this error.

  @return The number of TSC counts per second.

**/
UINT64
InternalCalculateTscFrequency (
  VOID
  )
{
  UINT64      StartTSC;
  UINT64      EndTSC;
  UINT32      Ticks;
  UINT64      TscFrequency;

  DEBUG ((DEBUG_INFO, "InternalCalculateTscFrequency - Enter\n"));
  //
  // Capture intial Timer an TSC values.
  //
  Ticks    = MmioRead32 ((ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG508)) + (3579);   // Set Ticks to 1ms in the future

  StartTSC = AsmReadTsc();                    // Get base value for the TSC
  //
  // Wait until the ACPI timer has counted 1ms.
  // Timer wrap-arounds are handled correctly by this function.
  // When the current ACPI timer value is greater than 'Ticks', the while loop will exit.
  // AMD's ACPI PM Timer is 32 bit, hence check if BIT31 is set, after subtracting current counter value
  // from previous value (Ticks). If current value goes above previous, loop ends.
  while (((Ticks - MmioRead32 ((ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG508))) & BIT31) == 0) {
    CpuPause();
  }
  EndTSC = AsmReadTsc();    // TSC value 1ms later

  TscFrequency =   MultU64x32 (
                      (EndTSC - StartTSC),    // Number of TSC counts in 1ms
                      1000                    // Number of ms in a second
                    );

  DEBUG ((DEBUG_INFO, "InternalCalculateTscFrequency - Exit TscFrequency=0x%x\n", TscFrequency));
  return TscFrequency;
}

/**  Stalls the CPU for at least the given number of ticks.

  Stalls the CPU for at least the given number of ticks. It's invoked by
  MicroSecondDelay() and NanoSecondDelay().

  @param[in]  Delay     A period of time to delay in ticks.

**/
VOID
InternalX86Delay (
  IN      UINT64                    Delay
  )
{
  UINT64                             Ticks;

  //
  // The target timer count is calculated here
  //
  Ticks = AsmReadTsc() + Delay;

  //
  // Wait until time out
  // Timer wrap-arounds are NOT handled correctly by this function.
  // Thus, this function must be called within 10 years of reset since
  // Intel guarantees a minimum of 10 years before the TSC wraps.
  //
  while (AsmReadTsc() <= Ticks) CpuPause();
}

/**  Stalls the CPU for at least the specified number of MicroSeconds.

  @param[in]  MicroSeconds  The minimum number of microseconds to delay.

  @return The value of MicroSeconds input.

**/
UINTN
EFIAPI
MicroSecondDelay (
  IN      UINTN                     MicroSeconds
  )
{
  InternalX86Delay (
    DivU64x32 (
      MultU64x64 (
        InternalGetTscFrequency (),
        MicroSeconds
      ),
      1000000u
    )
  );
  return MicroSeconds;
}

/**  Stalls the CPU for at least the specified number of NanoSeconds.

  @param[in]  NanoSeconds The minimum number of nanoseconds to delay.

  @return The value of NanoSeconds input.

**/
UINTN
EFIAPI
NanoSecondDelay (
  IN      UINTN                     NanoSeconds
  )
{
  InternalX86Delay (
    DivU64x32 (
      MultU64x32 (
        InternalGetTscFrequency (),
        (UINT32)NanoSeconds
      ),
    1000000000u
    )
  );
  return NanoSeconds;
}

/**  Retrieves the current value of the 64-bit free running Time-Stamp counter.

  The time-stamp counter (as implemented in the P6 family, Pentium, Pentium M,
  Pentium 4, Intel Xeon, Intel Core Solo and Intel Core Duo processors and
  later processors) is a 64-bit counter that is set to 0 following a RESET of
  the processor.  Following a RESET, the counter increments even when the
  processor is halted by the HLT instruction or the external STPCLK# pin. Note
  that the assertion of the external DPSLP# pin may cause the time-stamp
  counter to stop.

  The properties of the counter can be retrieved by the
  GetPerformanceCounterProperties() function.

  @return The current value of the free running performance counter.

**/
UINT64
EFIAPI
GetPerformanceCounter (
  VOID
  )
{
  return AsmReadTsc();
}

/**  Retrieves the 64-bit frequency in Hz and the range of performance counter
  values.

  If StartValue is not NULL, then the value that the performance counter starts
  with, 0x0, is returned in StartValue. If EndValue is not NULL, then the value
  that the performance counter end with, 0xFFFFFFFFFFFFFFFF, is returned in
  EndValue.

  The 64-bit frequency of the performance counter, in Hz, is always returned.
  To determine average processor clock frequency, Intel recommends the use of
  EMON logic to count processor core clocks over the period of time for which
  the average is required.


  @param[out]   StartValue  Pointer to where the performance counter's starting value is saved, or NULL.
  @param[out]   EndValue    Pointer to where the performance counter's ending value is saved, or NULL.

  @return The frequency in Hz.

**/
UINT64
EFIAPI
GetPerformanceCounterProperties (
  OUT      UINT64                    *StartValue,  OPTIONAL
  OUT      UINT64                    *EndValue     OPTIONAL
  )
{
  if (StartValue != NULL) {
    *StartValue = 0;
  }
  if (EndValue != NULL) {
    *EndValue = 0xFFFFFFFFFFFFFFFFull;
  }

  return InternalGetTscFrequency ();
}

/**
  Converts elapsed ticks of performance counter to time in nanoseconds.

  This function converts the elapsed ticks of running performance counter to
  time value in unit of nanoseconds.

  @param  Ticks     The number of elapsed ticks of running performance counter.

  @return The elapsed time in nanoseconds.

**/
UINT64
EFIAPI
GetTimeInNanoSecond (
  IN      UINT64                     Ticks
  )
{
  UINT64  Frequency;
  UINT64  NanoSeconds;
  UINT64  Remainder;
  INTN    Shift;

  Frequency = GetPerformanceCounterProperties (NULL, NULL);

  //
  //          Ticks
  // Time = --------- x 1,000,000,000
  //        Frequency
  //
  NanoSeconds = MultU64x32 (DivU64x64Remainder (Ticks, Frequency, &Remainder), 1000000000u);

  //
  // Ensure (Remainder * 1,000,000,000) will not overflow 64-bit.
  // Since 2^29 < 1,000,000,000 = 0x3B9ACA00 < 2^30, Remainder should < 2^(64-30) = 2^34,
  // i.e. highest bit set in Remainder should <= 33.
  //
  Shift = MAX (0, HighBitSet64 (Remainder) - 33);
  Remainder = RShiftU64 (Remainder, (UINTN) Shift);
  Frequency = RShiftU64 (Frequency, (UINTN) Shift);
  NanoSeconds += DivU64x64Remainder (MultU64x32 (Remainder, 1000000000u), Frequency, NULL);

  return NanoSeconds;
}



