/**
 * @file  SilPei.c
 * @brief PEIM created to execute SI init openSIL call.
 *
 */
/*
 * Copyright 2021-2024 Advanced Micro Devices, Inc. All rights reserved.
 *
 */

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
#include <Ppi/Reset2.h>
#include <SilCommon.h> ///@todo not be needed after Debug moved to Sil-api
#include <xSIM-api.h>
#include "SilPei.h"
#include "SilPpi.h"
#include "SilHob.h"
#include <Library/BaseMemoryLib.h>

#ifndef SIL_DEADLOOP
#define SIL_DEADLOOP()   \
  {                      \
    volatile size_t __i; \
    __i = 1;             \
    while (__i)          \
      ;                  \
  }
#endif

STATIC PEI_AMD_SIL_INIT_COMPLETE_PPI mSilPeiInitCompletePpi = {
    AMD_SIL_PPI_REVISION};

STATIC EFI_PEI_PPI_DESCRIPTOR mSilPeiInitCompletePpiList =
    {
        (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
        &gAmdSilPeiInitCompletePpiGuid,
        &mSilPeiInitCompletePpi};

EFI_STATUS IPBlockDataBackToHostFW(VOID);

/**--------------------------------------------------------------------
 * SilPeiTracePoint
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
SilPeiTracePoint (
    size_t      SilMsgLevel,
    const char  *SilPrefix,
    const char  *Message,
    const char  *Function,
    size_t      Line,
    ...)
{
  VA_LIST variadicArgs;
  unsigned short *MsgLvlString;
  uint32_t HostMsgLevel;

  VA_START(variadicArgs, Line);

  switch (SilMsgLevel)
  {
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
    DEBUG((HostMsgLevel, SilPrefix));
    // Print function, line, and MsgLvlString
    DEBUG((HostMsgLevel, "%a():%d:%s", Function, Line, MsgLvlString));
  }
  // Print the message with variable arguments
  DebugVPrint(HostMsgLevel, Message, variadicArgs);

  VA_END(variadicArgs);
}

/**--------------------------------------------------------------------
 *  SilHandleReset
 *
 *  @brief  Requests the host to perform the specified reset.
 *
 *  @param  PeiServices Input PeiServices pointer
 *  @param  SilStatus   Input to check if SilStatus contains any reset request
 *
 *  @return void
 */
void SilHandleReset(
    IN CONST EFI_PEI_SERVICES **PeiServices,
    IN SIL_STATUS ResetType)
{
  EFI_PEI_RESET2_PPI *Reset2;
  Reset2 = NULL;

  /*
   * If any reset request was made, handle it.  This statement will compare
   * the input reset request type (ResetType) to the lowest reset request type
   * defined in the SIL_STATUS enum in openSIL.  If the enum changes, it is
   * the responsibility of the user to verify this logic is still valid.
   */
  if (ResetType >= SilResetRequestColdImm)
  {
    (*PeiServices)->LocatePpi(PeiServices, &gEfiPeiReset2PpiGuid, 0, NULL, (VOID **) &Reset2);
    if (Reset2 != NULL)
    {
      if ((ResetType == SilResetRequestColdDef) || (ResetType == SilResetRequestColdImm))
      {
        DEBUG((DEBUG_ERROR, "%a:%d Performing cold reset\n", __FUNCTION__, __LINE__));
        Reset2->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, NULL);
        // We should never get this far
        SIL_DEADLOOP();
      }

      if ((ResetType == SilResetRequestWarmDef) || (ResetType == SilResetRequestWarmImm))
      {
        DEBUG((DEBUG_ERROR, "%a:%d Performing warm reset\n", __FUNCTION__, __LINE__));
        Reset2->ResetSystem(EfiResetWarm, EFI_SUCCESS, 0, NULL);
        // We should never get this far
        SIL_DEADLOOP();
      }
    }
  }
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
    SIL_STATUS SilStatus)
{
  switch (SilStatus)
  {
  case SilPass:
    return EFI_SUCCESS;
  case SilDeviceError:
    return EFI_DEVICE_ERROR;
  case SilInvalidParameter:
    return EFI_INVALID_PARAMETER;
  case SilAborted:
    return EFI_ABORTED;
  case SilOutOfResources:
    return EFI_OUT_OF_RESOURCES;
  case SilNotFound:
    return EFI_NOT_FOUND;
  case SilOutOfBounds:
    return EFI_BUFFER_TOO_SMALL; // @todo: confirm this is the best translation
  default:
    return EFI_UNSUPPORTED;
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
 *  @param PeiServices  Pointer to PEI services
 *  @param FwInit       Platform FW init function pointer
 *
 *  @return EFI_STATUS Returns EFI_SUCCESS if PPI was successfully installed
 */
EFI_STATUS
SilFwDataInit(
    IN CONST EFI_PEI_SERVICES **PeiServices,
    IN SIL_FWINIT_FUNCTION    FwInit)
{
  EFI_STATUS            Status;
  SIL_STATUS            SilStatus;
  size_t                RequiredMemorySize;
  EFI_PHYSICAL_ADDRESS  SilDataPointer;
  EFI_HOB_GUID_TYPE     *Hob;
  SIL_DATA_HOB          SilDataHob;

  DEBUG((DEBUG_ERROR, "SIL PEI entry point..\n"));

  // Init SIL debug message service for each module
  SilStatus = SilDebugSetup(SilPeiTracePoint);

  if (SilStatus != SilPass)
  {
    if (SilStatus == SilAborted)
    {
      DEBUG((DEBUG_INFO, "Host debug service is initialized already.\n"));
    }
    else
    {
      Status = TranslateStatus(SilStatus);
      DEBUG((DEBUG_ERROR, "SIL:%a:%d:Error: xSIM Debug Message service setup failed: %r\n",
        __FUNCTION__, __LINE__, Status));
      ASSERT (FALSE);
    }
  }

  RequiredMemorySize = xSimQueryMemoryRequirements();
  DEBUG((DEBUG_INFO, "openSIL RequiredMemorySize: 0x%x\n", RequiredMemorySize));

  // Allocate PEI memory block and create a HOB that has the pointer to this memory
  Status = PeiServicesAllocatePages(
      EfiBootServicesData,
      EFI_SIZE_TO_PAGES (RequiredMemorySize),
      &SilDataPointer);
  ASSERT_EFI_ERROR(Status);

  // Create openSIL hob with base address of SIL Data as well as the SIL Data size.
  CopyMem(&(SilDataHob.SilDataPointer), &SilDataPointer, sizeof(SilDataPointer));
  SilDataHob.DataSize = (UINT32)RequiredMemorySize;

  DEBUG ((DEBUG_INFO, "SilPei: IP block data pointer is 0x%llx.\n", SilDataPointer));

  Status = PeiServicesCreateHob (
      EFI_HOB_TYPE_GUID_EXTENSION,
      (UINT16)(sizeof(EFI_HOB_GUID_TYPE) + sizeof(SilDataHob)),
      (VOID **)&Hob);
  ASSERT_EFI_ERROR (Status);

  CopyMem(&Hob->Name, &gPeiOpenSilDataHobGuid, sizeof(EFI_GUID));
  Hob++;
  CopyMem (Hob, &SilDataHob, sizeof(SilDataHob));

  DEBUG ((DEBUG_INFO, "openSIL Data HOB @0x%x\n", Hob));
  DEBUG ((DEBUG_INFO, "openSIL Data Block Location @0x%lx\n", (UINT64)SilDataHob.SilDataPointer));
  DEBUG ((DEBUG_INFO, "openSIL Data Block Size 0x%x\n", SilDataHob.DataSize));

  // Let SIL assign the memory to the IPs
  SilStatus = xSimAssignMemoryTp1 ((VOID *)(UINT32)SilDataPointer, RequiredMemorySize);
  // ToDo: inject a return code translator SIL --> UEFI
  if (SilStatus != SilPass)
  {
    DEBUG ((DEBUG_ERROR, "ERROR: Memory assignment status %d.\n", SilStatus));
    return EFI_DEVICE_ERROR;
  }

  if (FwInit != NULL)
  {
    Status = FwInit ();
    ASSERT_EFI_ERROR (Status);
  }

  // All IP blocks are configured, pass control to openSIL
  SilStatus = InitializeSiTp1();

  if (SilStatus != SilPass) {
    DEBUG((DEBUG_ERROR, "ERROR: InitializeSiTp1 Status %d.\n", SilStatus));
    ASSERT (FALSE);
  }

  // Send All updated IP block info to HostFW
  Status = IPBlockDataBackToHostFW ();
  ASSERT_EFI_ERROR (Status);

  SilHandleReset (PeiServices, SilStatus);

  // ToDo: inject a return code translator SIL --> UEFI
  //  Status = TranslateStatus(SilStatus);

  Status = (**PeiServices).InstallPpi (PeiServices, &mSilPeiInitCompletePpiList);

  return Status;
}
