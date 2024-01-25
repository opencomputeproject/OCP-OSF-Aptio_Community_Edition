/**
  AMD SW SMI Child Dispatcher header

  Copyright (c) 2023, American Megatrends International LLC.
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#pragma once

#include <PiSmm.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Protocol/SmmCpu.h>

///
/// Soft SMI Node
///
typedef struct _SMM_SW_NODE {
  EFI_HANDLE                         DispatchHandle;         ///< Dispatch Handle
  EFI_SMM_SW_REGISTER_CONTEXT        Context;                ///< Register context
  EFI_SMM_HANDLER_ENTRY_POINT2       CallBack2Function;      ///< SMM handler entry point 2
  struct _SMM_SW_NODE                *SmmSwNodePtr;          ///< pointer to next node
} SMM_SW_NODE;

EFI_STATUS
EFIAPI
SmmSwDispatchHandler (
  IN      EFI_HANDLE     SmmImageHandle,
  IN OUT  VOID           *CommunicationBuffer OPTIONAL,
  IN OUT  UINTN          *SourceSize OPTIONAL
);

EFI_STATUS
EFIAPI
EfiSmmSwDispatch2UnRegister (
  IN  CONST EFI_SMM_SW_DISPATCH2_PROTOCOL *This,
  IN  EFI_HANDLE                          DispatchHandle
);

EFI_STATUS
EFIAPI
EfiSmmSwDispatch2Register (
  IN  CONST EFI_SMM_SW_DISPATCH2_PROTOCOL       *This,
  IN        EFI_SMM_HANDLER_ENTRY_POINT2        DispatchFunction,
  IN  OUT   EFI_SMM_SW_REGISTER_CONTEXT         *RegisterContext,
  OUT       EFI_HANDLE                          *DispatchHandle
);

#define MAX_SW_SMI_VALUE              0xFF

