/** @file
  Header file for the PeiPchPolicy library.

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#ifndef _PEI_PCH_POLICY_LIBRARY_H_
#define _PEI_PCH_POLICY_LIBRARY_H_

#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/ConfigBlockLib.h>
#include <Library/SiConfigBlockLib.h>
#include <Library/PchInfoLib.h>
#include <Ppi/SiPolicy.h>
#include <PchAccess.h>
#include <Library/PchSerialIoLib.h>
#include <Library/PchPolicyLib.h>

#define PCH_HPET_BASE_ADDRESS             0xFED00000

#endif // _PEI_PCH_POLICY_LIBRARY_H_
