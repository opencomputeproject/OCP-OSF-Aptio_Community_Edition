/** @file
  Silicon Policy PPI is used for specifying platform
  related Intel silicon information and policy setting.
  This PPI is consumed by the silicon PEI modules and carried
  over to silicon DXE modules.

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SI_POLICY_PPI_H_
#define _SI_POLICY_PPI_H_

#include <SiPolicyStruct.h>
#include <PchAccess.h>
#include <PchPolicyCommon.h>
#include <PchPreMemPolicyCommon.h>
#include <MePolicyCommon.h>
#include <SaPolicyCommon.h>
#include <CpuPolicyCommon.h>

extern EFI_GUID gSiPreMemPolicyPpiGuid;
extern EFI_GUID gSiPolicyPpiGuid;

typedef struct _SI_PREMEM_POLICY_STRUCT SI_PREMEM_POLICY_PPI;
typedef struct _SI_POLICY_STRUCT SI_POLICY_PPI;

#endif // _SI_POLICY_PPI_H_

