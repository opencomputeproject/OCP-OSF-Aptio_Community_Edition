/** @file
  This file defines the PCH NVS Protocol.

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#ifndef _PCH_NVS_H_
#define _PCH_NVS_H_

#include "PchNvsArea.h"

//
// Extern the GUID for protocol users.
//
extern EFI_GUID                         gPchNvsAreaProtocolGuid;

/**
  This protocol is used to sync PCH information from POST to runtime ASL.
  This protocol exposes the pointer of PCH NVS Area only. Please refer to
  ASL definition for PCH NVS AREA.
**/
typedef struct {
  PCH_NVS_AREA                          *Area;
} PCH_NVS_AREA_PROTOCOL;

#endif
