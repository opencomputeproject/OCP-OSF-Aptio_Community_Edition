/** @file

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/BoardInitLib.h>
#include <Library/MultiBoardInitSupportLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PeiServicesLib.h>

EFI_STATUS
EFIAPI
RegisterBoardDetect (
  IN BOARD_DETECT_FUNC  *BoardDetect
  )
{
  EFI_STATUS                 Status;
  EFI_PEI_PPI_DESCRIPTOR     *PpiListBoardDetect;

  PpiListBoardDetect = AllocatePool (sizeof(EFI_PEI_PPI_DESCRIPTOR));
  ASSERT (PpiListBoardDetect != NULL);

  PpiListBoardDetect->Flags = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
  PpiListBoardDetect->Guid  = &gBoardDetectGuid;
  PpiListBoardDetect->Ppi   = BoardDetect;

  Status = PeiServicesInstallPpi (PpiListBoardDetect);
  ASSERT_EFI_ERROR(Status);
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
RegisterBoardPreMemInit (
  IN BOARD_PRE_MEM_INIT_FUNC  *BoardPreMemInit
  )
{
  EFI_STATUS                 Status;
  EFI_PEI_PPI_DESCRIPTOR     *PpiListBoardInitPreMem;

  PpiListBoardInitPreMem = AllocatePool (sizeof(EFI_PEI_PPI_DESCRIPTOR));
  ASSERT (PpiListBoardInitPreMem != NULL);

  PpiListBoardInitPreMem->Flags = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
  PpiListBoardInitPreMem->Guid  = &gBoardPreMemInitGuid;
  PpiListBoardInitPreMem->Ppi   = BoardPreMemInit;

  Status = PeiServicesInstallPpi (PpiListBoardInitPreMem);
  ASSERT_EFI_ERROR(Status);
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
RegisterBoardPostMemInit (
  IN BOARD_POST_MEM_INIT_FUNC  *BoardPostMemInit
  )
{
  EFI_STATUS                 Status;
  EFI_PEI_PPI_DESCRIPTOR     *PpiListBoardInitPostMem;

  PpiListBoardInitPostMem = AllocatePool (sizeof(EFI_PEI_PPI_DESCRIPTOR));
  ASSERT (PpiListBoardInitPostMem != NULL);

  PpiListBoardInitPostMem->Flags = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
  PpiListBoardInitPostMem->Guid  = &gBoardPostMemInitGuid;
  PpiListBoardInitPostMem->Ppi   = BoardPostMemInit;

  Status = PeiServicesInstallPpi (PpiListBoardInitPostMem);
  ASSERT_EFI_ERROR(Status);
  return EFI_SUCCESS;
}
