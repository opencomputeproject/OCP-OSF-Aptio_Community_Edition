/** @file

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/SiliconPolicyUpdateLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>

VOID *
EFIAPI
SiliconPolicyUpdatePreMem (
  IN VOID *Policy
  )
{
  return Policy;
}

VOID *
EFIAPI
SiliconPolicyUpdatePostMem (
  IN VOID *Policy
  )
{
  return Policy;
}

VOID *
EFIAPI
SiliconPolicyUpdateLate (
  IN VOID *Policy
  )
{
  return Policy;
}
