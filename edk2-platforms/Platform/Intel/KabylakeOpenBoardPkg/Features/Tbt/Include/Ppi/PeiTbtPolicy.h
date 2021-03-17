/** @file
TBT PEI Policy

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _PEI_TBT_POLICY_H_
#define _PEI_TBT_POLICY_H_

#include <TbtPolicyCommonDefinition.h>

#pragma pack(push, 1)

#define PEI_TBT_POLICY_REVISION 1

/**
 TBT PEI configuration\n
  <b>Revision 1</b>:
  - Initial version.
**/
typedef struct _PEI_TBT_POLICY {
  DTBT_COMMON_CONFIG     DTbtCommonConfig;                  ///< dTbt Common Configuration
  DTBT_CONTROLLER_CONFIG DTbtControllerConfig;              ///< dTbt Controller Configuration
} PEI_TBT_POLICY;

#pragma pack(pop)

#endif
