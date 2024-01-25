/**
 * @file  SilSmm.c
 * @brief SMM driver created to execute openSIL call.
 *
 */
/*
 * Copyright 2021-2023 Advanced Micro Devices, Inc. All rights reserved.
 *
 */

#include <Uefi.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <xSIM-api.h>
#include <xPRF-api.h>
#include "SilSmm.h"
#include "SilHob.h"
#include <Include/Pi/PiHob.h>
#include <Include/Library/HobLib.h>

#ifndef SIL_DEADLOOP
  #define SIL_DEADLOOP()    { volatile size_t __i; __i = 1; while (__i); }
#endif

/**--------------------------------------------------------------------
 * SilTracePoint
 *
 * @brief  Forms output string, translates SIL message level, then calls host debug service
 *
 * @param MsgLevel   The SIL MsgLevel
 * @param Message    The Message to send to debug service
 * @param Function   The name of the calling function
 * @param Line       The line number or the caller
 *
 * @returns void
 **/
void
SilTracePoint (
  size_t      SilMsgLevel,
  const char  *SilPrefix,
  const char  *Message,
  const char  *Function,
  size_t      Line,
  ...
  )
{
  VA_LIST         variadicArgs;
  unsigned short  *MsgLvlString;
  uint32_t        HostMsgLevel;

  VA_START (variadicArgs, Line);

  switch (SilMsgLevel) {
  case SIL_TRACE_ERROR:
    MsgLvlString = L"Error: ";
    HostMsgLevel = DEBUG_ERROR;
    break;
  case SIL_TRACE_WARNING:
    MsgLvlString = L"Warn: ";
    HostMsgLevel = DEBUG_ERROR;
    break;
  case SIL_TRACE_ENTRY:
    MsgLvlString = L"Entry ";
    HostMsgLevel = DEBUG_ERROR;
    break;
  case SIL_TRACE_EXIT:
    MsgLvlString = L"Exit ";
    HostMsgLevel = DEBUG_ERROR;
    break;
  case SIL_TRACE_INFO:
    MsgLvlString = L"Info: ";
    HostMsgLevel = DEBUG_ERROR;
    break;
  default:
    MsgLvlString = L"Null: ";
    HostMsgLevel = DEBUG_ERROR;
    break;
  }

  // For RAW message level, skip the openSIL prefix
  if (SilMsgLevel != SIL_TRACE_RAW) {
    // Print the openSIL prefix
    DEBUG ((HostMsgLevel, SilPrefix));
    // Print function, line, and MsgLvlString
    DEBUG ((HostMsgLevel, "%a():%d:%s", Function, Line, MsgLvlString));
  }
  // Print the message with variable arguments
  DebugVPrint (HostMsgLevel, Message, variadicArgs);

  VA_END (variadicArgs);
}

/**--------------------------------------------------------------------
 * TranslateStatus
 *
 * @brief  Matches EFI_STATUS to a given SIL_STATUS
 *
 * @param SilStatus   Status to be translated to UEF status
 *
 * @returns UEFI status
 **/
EFI_STATUS TranslateStatus(
  SIL_STATUS SilStatus
)
{
  switch (SilStatus) {
    case SilPass:                 return EFI_SUCCESS;
    case SilDeviceError:          return EFI_DEVICE_ERROR;
    case SilInvalidParameter:     return EFI_INVALID_PARAMETER;
    case SilAborted:              return EFI_ABORTED;
    case SilOutOfResources:       return EFI_OUT_OF_RESOURCES;
    case SilNotFound:             return EFI_NOT_FOUND;
    case SilOutOfBounds:          return EFI_BUFFER_TOO_SMALL;  // @todo: confirm this is the best translation
    default:                      return EFI_UNSUPPORTED;
  }
}

/**
 *  Initialize openSIL services and execute SI
 *
 *  - Initialize debug services
 *  - Query openSIL memory requirements and allocate pool for openSIL
 *  - Execute IP initialization calls to fill IP blocks with the platform data
 *  - Call openSIL SI function
 *  - Install PPI to indicate openSIL execution completion
 *
 *  @param   SystemTable  Pointer to EFI system table
 *
 *  @return  EFI_STATUS   Returns EFI_SUCCESS if executed successfully
 */
EFI_STATUS
SilFwDataInit (
  EFI_SYSTEM_TABLE           *SystemTable
  )
{
  EFI_STATUS              Status;
  SIL_STATUS              SilStatus;
  EFI_HOB_GUID_TYPE       *Hob;
  SIL_INFO_BLOCK_HEADER   *SilData;
  UINT32                  SilDataBlockSize;
  SIL_DATA_HOB            *SilDataHob;

  Status = EFI_SUCCESS;

  DEBUG((DEBUG_ERROR, "SIL SMM entry point.\n"));

  // Init SIL DXE debug message service for each module
  SilStatus = SilDebugSetup (SilTracePoint);

  if (SilStatus != SilPass) {
    if (SilStatus == SilAborted) {
      DEBUG((DEBUG_INFO, "Host debug service for SMM already initialized.\n"));
    } else {
      DEBUG((DEBUG_ERROR, "Host debug service setup for SMM failed.\n"));
    }
  }

  Hob = (EFI_HOB_GUID_TYPE *)GetFirstGuidHob (&gPeiOpenSilDataHobGuid);
  DEBUG ((DEBUG_INFO, "openSIL HOB Name  %g\n",Hob->Name));
  DEBUG ((DEBUG_INFO, "openSIL HOB HobType %d\n",Hob->Header.HobType));
  DEBUG ((DEBUG_INFO, "openSIL HOB HobLength %d\n",Hob->Header.HobLength));
  Hob++;
  SilDataHob = (SIL_DATA_HOB *)Hob;

  DEBUG ((DEBUG_INFO, "openSIL Data HOB @0x%x\n", Hob));
  DEBUG ((DEBUG_INFO, "openSIL Data Block Location @0x%lx\n", (UINT64)SilDataHob->SilDataPointer));
  DEBUG ((DEBUG_INFO, "openSIL Data Block Size 0x%x\n", SilDataHob->DataSize));

  SilDataBlockSize = SilDataHob->DataSize;

  /*
   * Allocate SMM memory pool to store the SMM copy of the SIL Data Block
   */
  Status = gSmst->SmmAllocatePool (
    EfiRuntimeServicesData,
    SilDataBlockSize,
    (VOID **)&SilData
    );
  DEBUG((DEBUG_ERROR, "openSIL SMM Memory Allocation status = %r\n", Status));
  if (Status == EFI_SUCCESS) {
    DEBUG((DEBUG_ERROR, "SMM Buffer allocated size = 0x%x @%lx\n", SilDataBlockSize, SilData));
  }
  gBS->CopyMem ((VOID *)SilData, (VOID *)(SilDataHob->SilDataPointer), SilDataBlockSize);

  /*
   * Assign the SIL base pointer in openSIL with the newly allocated
   * SMM memory containing the SIL Data block.
   */
  SilStatus = xSimAssignMemoryTp2 ((VOID *)SilData, 0);

  Status = TranslateStatus (SilStatus);

  ASSERT_EFI_ERROR (Status);

  return Status;
}
