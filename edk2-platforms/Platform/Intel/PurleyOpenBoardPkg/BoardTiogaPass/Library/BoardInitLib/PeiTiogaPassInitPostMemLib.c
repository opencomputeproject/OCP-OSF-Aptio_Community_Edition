/** @file

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

//***********************************************************************
//*                                                                     *
//*   Copyright (c) 1985 - 2021, American Megatrends International LLC. *
//*                                                                     *
//*      All rights reserved.                                           *
//*                                                                     *
//*      This program and the accompanying materials are licensed and   *
//*      made available under the terms and conditions of the BSD       *
//*      License that accompanies this distribution.  The full text of  *
//*      the license may be found at:                                   *
//*      http://opensource.org/licenses/bsd-license.php.                *
//*                                                                     *
//*      THIS PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN        *
//*      "AS IS" BASIS, WITHOUT WARRANTIES OR REPRESENTATIONS OF        *
//*      ANY KIND, EITHER EXPRESS OR IMPLIED.                           *
//*                                                                     *
//***********************************************************************

#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/IoLib.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/PciLib.h>
#include <Library/BoardInitLib.h>

#include <Setup/IioUniversalData.h>

#include "PeiTiogaPassInitLib.h"

VOID
GetIioUdsHob (
    IN IIO_UDS                  **UdsHobPtr
  )
{
  EFI_GUID           UniversalDataGuid = IIO_UNIVERSAL_DATA_GUID;
  EFI_HOB_GUID_TYPE  *GuidHob;

  ASSERT(UdsHobPtr);

  *UdsHobPtr = NULL;
  
  GuidHob       = GetFirstGuidHob (&UniversalDataGuid);
  if (GuidHob){
    *UdsHobPtr  = GET_GUID_HOB_DATA (GuidHob);
    return;
  }

  ASSERT(FALSE);
}

EFI_STATUS
EFIAPI
TiogaPassBoardInitBeforeSiliconInit (
  VOID
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
TiogaPassBoardInitAfterSiliconInit (
  VOID
  )
{
  IIO_UDS               *IioUds;

  DEBUG((EFI_D_ERROR, "TiogaPassBoardInitAfterSiliconInit\n"));

  GetIioUdsHob(&IioUds);

  DEBUG ((EFI_D_ERROR, "Memory TOLM: %X\n", IioUds->PlatformData.MemTolm));
  DEBUG (
    (EFI_D_ERROR,
    "PCIE   BASE: %lX     Size : %X\n",
    IioUds->PlatformData.PciExpressBase,
    IioUds->PlatformData.PciExpressSize)
    );
  DEBUG (
    (EFI_D_ERROR,
    "PCI32  BASE: %X     Limit: %X\n",
    IioUds->PlatformData.PlatGlobalMmiolBase,
    IioUds->PlatformData.PlatGlobalMmiolLimit)
    );
  DEBUG (
    (EFI_D_ERROR,
    "PCI64  BASE: %lX     Limit: %lX\n",
    IioUds->PlatformData.PlatGlobalMmiohBase,
    IioUds->PlatformData.PlatGlobalMmiohLimit)
    );
  DEBUG ((EFI_D_ERROR, "UC    START: %lX     End  : %lX\n", IioUds->PlatformData.PlatGlobalMmiohBase, (IioUds->PlatformData.PlatGlobalMmiohLimit + 1)));

  return EFI_SUCCESS;
}
