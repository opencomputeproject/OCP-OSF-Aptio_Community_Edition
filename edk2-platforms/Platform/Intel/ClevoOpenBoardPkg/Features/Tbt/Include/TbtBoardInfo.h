/** @file

Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#ifndef _TBT_INFO_GUID_H_
#define _TBT_INFO_GUID_H_
#include <TbtPolicyCommonDefinition.h>

#pragma pack(1)
//
// TBT Info HOB
//
typedef struct _TBT_INFO_HOB {
  EFI_HOB_GUID_TYPE      EfiHobGuidType;
  DTBT_COMMON_CONFIG     DTbtCommonConfig;                                  ///< dTbt Common Configuration
  DTBT_CONTROLLER_CONFIG DTbtControllerConfig;                             ///< dTbt Controller Configuration
} TBT_INFO_HOB;
#pragma pack()

#endif
