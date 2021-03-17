/** @file
Do Platform Stage System Agent initialization.

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PeiSaPolicyUpdate.h"
#include <Guid/MemoryTypeInformation.h>
#include <Library/HobLib.h>
#include <PchAccess.h>
#include <SaAccess.h>
#include <Pi/PiFirmwareFile.h>
#include <Pi/PiPeiCis.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PeiSaPolicyLib.h>
#include <Library/PeiLib.h>

/**
  Performs FSP SA PEI Policy initialization.

  @param[in][out]  FspsUpd             Pointer to FSP UPD Data.

  @retval          EFI_SUCCESS         FSP UPD Data is updated.
  @retval          EFI_NOT_FOUND       Fail to locate required PPI.
  @retval          Other               FSP UPD Data update process fail.
**/
EFI_STATUS
EFIAPI
PeiFspSaPolicyUpdate (
  IN OUT FSPS_UPD    *FspsUpd
  )
{
  VOID                            *Buffer;
  VOID                            *MemBuffer;
  UINT32                          Size;

  DEBUG((DEBUG_INFO, "\nUpdating SA Policy in Post Mem\n"));

    FspsUpd->FspsConfig.PeiGraphicsPeimInit = 1;

    Size   = 0;
    Buffer = NULL;
    PeiGetSectionFromAnyFv (PcdGetPtr (PcdGraphicsVbtGuid), EFI_SECTION_RAW, 0, &Buffer, &Size);
    if (Buffer == NULL) {
      DEBUG((DEBUG_WARN, "Could not locate VBT\n"));
    } else {
      MemBuffer = (VOID *)AllocatePages (EFI_SIZE_TO_PAGES ((UINTN)Size));
      if ((MemBuffer != NULL) && (Buffer != NULL)) {
        CopyMem (MemBuffer, (VOID *)Buffer, (UINTN)Size);
        FspsUpd->FspsConfig.GraphicsConfigPtr = (UINT32)(UINTN)MemBuffer;
      } else {
        DEBUG((DEBUG_WARN, "Error in locating / copying VBT.\n"));
        FspsUpd->FspsConfig.GraphicsConfigPtr = 0;
      }
    }
    DEBUG((DEBUG_INFO, "Vbt Pointer from PeiGetSectionFromFv is 0x%x\n", FspsUpd->FspsConfig.GraphicsConfigPtr));
    DEBUG((DEBUG_INFO, "Vbt Size from PeiGetSectionFromFv is 0x%x\n", Size));

    Size   = 0;
    Buffer = NULL;
    PeiGetSectionFromAnyFv (&gTianoLogoGuid, EFI_SECTION_RAW, 0, &Buffer, &Size);
    if (Buffer == NULL) {
      DEBUG((DEBUG_WARN, "Could not locate Logo\n"));
    } else {
      MemBuffer = (VOID *)AllocatePages (EFI_SIZE_TO_PAGES ((UINTN)Size));
      if ((MemBuffer != NULL) && (Buffer != NULL)) {
        CopyMem (MemBuffer, (VOID *)Buffer, (UINTN)Size);
        FspsUpd->FspsConfig.LogoPtr = (UINT32)(UINTN)MemBuffer;
        FspsUpd->FspsConfig.LogoSize = Size;
      } else {
        DEBUG((DEBUG_WARN, "Error in locating / copying LogoPtr.\n"));
        FspsUpd->FspsConfig.LogoPtr = 0;
        FspsUpd->FspsConfig.LogoSize = 0;
      }
    }
    DEBUG((DEBUG_INFO, "LogoPtr from PeiGetSectionFromFv is 0x%x\n", FspsUpd->FspsConfig.LogoPtr));
    DEBUG((DEBUG_INFO, "LogoSize from PeiGetSectionFromFv is 0x%x\n", FspsUpd->FspsConfig.LogoSize));

  return EFI_SUCCESS;
}

