/** @file

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _PCH_PMC_LIB_H_
#define _PCH_PMC_LIB_H_

typedef enum {
  PchWarmBoot       = 1,
  PchColdBoot,
  PwrFlr,
  PwrFlrSys,
  PwrFlrPch,
  PchPmStatusMax
} PCH_PM_STATUS;

/**
  Query PCH to determine the Pm Status

  @param[in] PmStatus - The Pch Pm Status to be probed

  @retval Status TRUE if Status querried is Valid or FALSE if otherwise
**/
BOOLEAN
GetPchPmStatus (
  PCH_PM_STATUS PmStatus
  );

/**
  Funtion to check if Battery lost or CMOS cleared.

  @reval TRUE  Battery is always present.
  @reval FALSE CMOS is cleared.
**/
BOOLEAN
EFIAPI
PchIsRtcBatteryGood (
  VOID
  );

/**
  Funtion to check if DWR occurs

  @reval TRUE DWR occurs
  @reval FALSE Normal boot flow
**/
BOOLEAN
EFIAPI
PchIsDwrFlow (
  VOID
  );

#endif // _PCH_PMC_LIB_H_