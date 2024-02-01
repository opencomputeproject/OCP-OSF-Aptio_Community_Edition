/**
 * @file  xPrfServicesSmm.h
 * @brief xPRF services data structures and definitions for SMM
 *
 */
/*
 * Copyright 2021-2024 Advanced Micro Devices, Inc. All rights reserved.
 *
 */
#pragma once

#include <Uefi.h>
#include <xPRF-api.h>
#include <Include/xPrfServicesSmmProtocol.h>

EFI_STATUS
SilMcaErrorAddrTranslate (
  UINT64                  *SystemMemoryAddress,
  SIL_NORMALIZED_ADDRESS  *NormalizedAddress,
  SIL_DIMM_INFO           *DimmInfo,
  SIL_ADDR_DATA           *AddrData
  );

EFI_STATUS
SilTranslateSysAddrToCS (
  UINT64                  *SystemMemoryAddress,
  SIL_NORMALIZED_ADDRESS  *NormalizedAddress,
  SIL_DIMM_INFO           *DimmInfo,
  SIL_ADDR_DATA           *AddrData
  );

VOID
SilGetLocalSmiStatus (
  VOID *LocalSmiStatus
  );

VOID
SilCollectMcaErrorInfo (
  VOID *SilRasMcaErrorInfo
  );

EFI_STATUS
SilTranslateSysAddrToDpa (
  UINT64        *SystemMemoryAddress,
  UINT64        *Dpa,
  SIL_ADDR_DATA *AddrData
  );

EFI_STATUS
SilUpdateDimmFruTextToMca (
  SIL_DIMM_FRUTEXT_TABLE   *DimmFruTextTable
  );

void
SilSetIpMcaCtlMask (
  void  *Buffer
  );

EFI_STATUS
xPrfServicesSmmProtocolInstall (VOID);
