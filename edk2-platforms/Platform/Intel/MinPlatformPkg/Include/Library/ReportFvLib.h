/** @file

  Report Firmware Volume (FV) library

  This library installs pre-memory and post-memory firmware volumes.

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _REPORT_FV_LIB_H_
#define _REPORT_FV_LIB_H_

#include <PiPei.h>
#include <Uefi.h>

VOID
ReportPreMemFv (
  VOID
  );

VOID
ReportPostMemFv (
  VOID
  );

#endif