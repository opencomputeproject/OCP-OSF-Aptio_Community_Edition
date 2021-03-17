/** @file
  Platform Hook Library instances

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Base.h>
#include <Uefi.h>
#include <PiDxe.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/BoardAcpiEnableLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>

EFI_STATUS
EFIAPI
KabylakeRvp3BoardEnableAcpi (
  IN BOOLEAN  EnableSci
  );

EFI_STATUS
EFIAPI
KabylakeRvp3BoardDisableAcpi (
  IN BOOLEAN  DisableSci
  );

EFI_STATUS
EFIAPI
SiliconEnableAcpi (
  IN BOOLEAN  EnableSci
  );

EFI_STATUS
EFIAPI
SiliconDisableAcpi (
  IN BOOLEAN  DisableSci
  );

EFI_STATUS
EFIAPI
BoardEnableAcpi (
  IN BOOLEAN  EnableSci
  )
{
  SiliconEnableAcpi (EnableSci);
  return KabylakeRvp3BoardEnableAcpi (EnableSci);
}

EFI_STATUS
EFIAPI
BoardDisableAcpi (
  IN BOOLEAN  DisableSci
  )
{
  SiliconDisableAcpi (DisableSci);
  return KabylakeRvp3BoardDisableAcpi (DisableSci);
}


