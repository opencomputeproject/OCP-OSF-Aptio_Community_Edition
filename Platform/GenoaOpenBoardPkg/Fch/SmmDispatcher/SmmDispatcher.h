/*****************************************************************************
 *
 * Copyright (C) 2020-2024 Advanced Micro Devices, Inc. All rights reserved.
 *
 *****************************************************************************/

/**
  AMD SMM Dispatcher Driver header

**/

#pragma once


#include <PiSmm.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/IoLib.h>
#include <SilCommon.h>
#include <FchCore/FchHwAcpi/FchHwAcpiReg.h>
#include <Library/IoLib.h>
#include <Library/SmmMemLib.h>
#include <SmmSwDispatcher.h>

#define FCH_SMI_CMD_PORT          BIT_32(11)
#define FCH_END_OF_SMI            BIT_32(28)

#define FCH_SMI_STATUS2           0x88
#define FCH_SMI_TRIGGER           0x98

typedef struct {
  EFI_GUID              *Guid;
  VOID                  *Interface;
} SMM_PROTOCOL_LIST;

typedef EFI_STATUS (EFIAPI *SM_SMM_CHILD_DISPATCHER_HANDLER) (
  IN      EFI_HANDLE     SmmImageHandle,
  IN OUT  VOID           *CommunicationBuffer OPTIONAL,
  IN OUT  UINTN          *SourceSize OPTIONAL
  );


///
/// AMD FCH SMM Dispatcher Structure
///
typedef struct {
  UINT32                                SmiStatusReg;   ///< Status Register
  UINT32                                SmiStatusBit;   ///< Status Bit
} SMM_COMMUNICATION_BUFFER;

extern  EFI_SMM_SW_DISPATCH2_PROTOCOL gEfiSmmSwDispatch2Protocol;
extern  EFI_SMM_CPU_PROTOCOL          *mSmmCpuProtocol;
extern  SMM_SW_NODE                   *HeadSmmSwNodePtr;
extern  EFI_SMM_SW_CONTEXT            *EfiSmmSwContext;

