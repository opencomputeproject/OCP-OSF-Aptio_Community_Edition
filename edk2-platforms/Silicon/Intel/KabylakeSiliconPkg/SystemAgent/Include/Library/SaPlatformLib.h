/** @file
  Header file for SaPlatformLib.

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SA_PLATFORM_LIB_H_
#define _SA_PLATFORM_LIB_H_

#include <SaAccess.h>
#include <CpuAccess.h>

/**
  Determine if PCH Link is DMI/OPI

  @param[in] CpuModel             CPU model

  @retval TRUE                    DMI
  @retval FALSE                   OPI
**/
BOOLEAN
IsPchLinkDmi (
  IN CPU_FAMILY  CpuModel
  );

#endif
