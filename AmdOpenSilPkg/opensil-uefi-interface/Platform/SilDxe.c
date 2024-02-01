/**
 * @file  SilDxe.c
 * @brief DXE driver created to execute SI init openSIL call.
 *
 */
/*
 * Copyright 2021-2024 Advanced Micro Devices, Inc. All rights reserved.
 *
 */

#include <Uefi.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Sil-api.h>
#include <SilCommon.h>
#include <xSIM-api.h>
#include <xPRF-api.h>
#include "SilDxe.h"
#include "SilHob.h"
#include <Include/Pi/PiHob.h>
#include <Include/Library/HobLib.h>
#include <Base.h>

#ifndef SIL_DEADLOOP
  #define SIL_DEADLOOP()    { volatile size_t __i; __i = 1; while (__i); }
#endif

#if defined (__GNUC__) || defined (__clang__)
  #define SIL_VA_LIST __builtin_va_list
  #define SIL_VA_START __builtin_va_start
  #define SIL_VA_ARG __builtin_va_arg
  #define SIL_VA_END __builtin_va_end
#else
  #define SIL_VA_LIST VA_LIST
  #define SIL_VA_START VA_START
  #define SIL_VA_ARG VA_ARG
  #define SIL_VA_END VA_END
#endif

/**
  Worker function that convert a __builtin_va_list to a BASE_LIST based on a
  Null-terminated format string.

  @param  Format          Null-terminated format string.
  @param  VaListMarker    __builtin_va_list style variable argument list consumed
                          by processing Format.
  @param  BaseListMarker  BASE_LIST style variable argument list consumed
                          by processing Format.
  @param  Size            The size, in bytes, of the BaseListMarker buffer.

  @return TRUE   The __builtin_va_list has been converted to BASE_LIST.
  @return FALSE  The __builtin_va_list has not been converted to BASE_LIST.

**/
bool
SilVaListToBaseList (
  const char          *Format,
  SIL_VA_LIST         VaListMarker,
  BASE_LIST           BaseListMarker,
  size_t              Size
  )
{
  BASE_LIST  BaseListStart;
  bool       Long;

  assert (Format != NULL);
  assert (BaseListMarker != NULL);

  BaseListStart = BaseListMarker;

  for ( ; *Format != '\0'; Format++) {

    // Only format with prefix % is processed.
    if (*Format != '%') {
      continue;
    }

    Long = false;

    // Parse Flags and Width
    for (Format++; true; Format++) {
      if ((*Format == '.') || (*Format == '-') || (*Format == '+') || (*Format == ' ')) {
        continue;   // These characters in format field are omitted.
      }

      if ((*Format >= '0') && (*Format <= '9')) {
        continue;   // These characters in format field are omitted.
      }

      if ((*Format == 'L') || (*Format == 'l')) {
        // 'L" or "l" in format field means the number being printed is a uint64_t
        Long = true;
        continue;
      }

      if (*Format == '*') {
        // '*' in format field means the precision of the field is specified by a size_t argument in the argument list.
        BASE_ARG (BaseListMarker, size_t) = SIL_VA_ARG (VaListMarker, size_t);
        continue;
      }

      if (*Format == '\0') {
        // Make no output if Format string terminates unexpectedly when looking up for flag, width, precision and type.
        Format--;
      }

      // When valid argument type detected or format string terminates unexpectedly, the inner loop is done.
      break;
    }

    // Pack variable arguments into the storage area following EFI_DEBUG_INFO.
    if ((*Format == 'p') && (sizeof (void *) > 4)) {
      Long = true;
    }

    if ((*Format == 'p') || (*Format == 'X') || (*Format == 'x') || (*Format == 'd') || (*Format == 'u')) {
      if (Long) {
        BASE_ARG (BaseListMarker, int64_t) = SIL_VA_ARG (VaListMarker, int64_t);
      } else {
        BASE_ARG (BaseListMarker, int32_t) = SIL_VA_ARG (VaListMarker, int32_t);
      }
    } else if ((*Format == 's') || (*Format == 'S') || (*Format == 'a') || (*Format == 'g') || (*Format == 't')) {
      BASE_ARG (BaseListMarker, void *) = SIL_VA_ARG (VaListMarker, void *);
    } else if (*Format == 'c') {
      BASE_ARG (BaseListMarker, size_t) = SIL_VA_ARG (VaListMarker, size_t);
    } else if (*Format == 'r') {
      BASE_ARG (BaseListMarker, RETURN_STATUS) = SIL_VA_ARG (VaListMarker, RETURN_STATUS);
    }

    //
    // If the converted BASE_LIST is larger than the size of BaseListMarker, then return FALSE
    //
    if (((size_t)BaseListMarker - (size_t)BaseListStart) > Size) {
      return false;
    }
  }

  return true;
}

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
  SIL_VA_LIST       variadicArgs;

  uint16_t          *MsgLvlString;
  uint32_t          HostMsgLevel;
  bool              Converted;
  uint64_t          BaseListMarker[256 / sizeof (uint64_t)];

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
  // Process variable arguments list
  SIL_VA_START (variadicArgs, Line);

  // Convert the va_list to BaseList and print it
  Converted = SilVaListToBaseList (
                Message,
                variadicArgs,
                (BASE_LIST)BaseListMarker,
                sizeof (BaseListMarker) - 8
                );

  if (Converted) {
    DebugBPrint (HostMsgLevel, Message, (BASE_LIST)BaseListMarker);
  }

  SIL_VA_END (variadicArgs);
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
 *  @param   FwInit       Platform FW init function pointer
 *
 *  @return  EFI_STATUS   Returns EFI_SUCCESS if executed successfully
 */
EFI_STATUS
SilFwDataInit (
  IN EFI_SYSTEM_TABLE           *SystemTable,
  IN SIL_FWINIT_FUNCTION        FwInit
  )
{
  EFI_STATUS         Status;
  SIL_STATUS         SilStatus;
  EFI_HOB_GUID_TYPE  *Hob;
  UINT64             DataPointer;
  SIL_DATA_HOB       *SilDataHob;

  Status = EFI_SUCCESS;

  DEBUG((DEBUG_ERROR, "SIL DXE entry point.\n"));

  // Init SIL DXE debug message service for each module
  SilStatus = SilDebugSetup (SilTracePoint);

  if (SilStatus != SilPass) {
    if (SilStatus == SilAborted) {
      DEBUG((DEBUG_INFO, "Host debug service is initialized already.\n"));
    } else {
      DEBUG((DEBUG_ERROR,
             "SIL:%a:%d:Error: xSIM DXE Debug Message service setup failed: %r\n",
             __FUNCTION__,
             __LINE__,
             Status));
    }
  }

  Hob = (EFI_HOB_GUID_TYPE *)GetFirstGuidHob(&gPeiOpenSilDataHobGuid);
  DEBUG ((DEBUG_INFO, "openSIL HOB Name  %g\n",Hob->Name));
  DEBUG ((DEBUG_INFO, "openSIL HOB HobType %d\n",Hob->Header.HobType));
  DEBUG ((DEBUG_INFO, "openSIL HOB HobLength %d\n",Hob->Header.HobLength));

  Hob++;

  SilDataHob = (SIL_DATA_HOB *)Hob;
  DataPointer = (UINT64)(SilDataHob->SilDataPointer);
  DEBUG ((DEBUG_INFO, "openSIL Data Block Location @0x%lx\n", DataPointer));

  Status = xSimAssignMemoryTp2 ((VOID *)DataPointer, 0);
  ASSERT_EFI_ERROR (Status);

  Status = TranslateStatus(SilStatus);

  return Status;
}
