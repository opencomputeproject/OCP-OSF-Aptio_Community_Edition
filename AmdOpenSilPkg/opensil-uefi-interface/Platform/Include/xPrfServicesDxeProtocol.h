/**
 * @file  xPrfServicesDxeProtocol.h
 * @brief xPRF services protocol definition
 *
 */
/*
 * Copyright 2021-2024 Advanced Micro Devices, Inc. All rights reserved.
 *
 */

#pragma once

#include <RAS/Common/RasClass-api.h>
#include <CcxClass-api.h>

/**
 * Function pointer type definitions for xPRF protocol
 * The xPRF protocol will contain pointers to all xPRF services provided by
 * openSIL.
 */
typedef EFI_STATUS (*AMD_XPRF_DUMMY_FUNC) (
  VOID);

/**
 * CCX xPRF services definitions
 */
typedef UINT8 (*XPRF_GET_THREADSPER_CORE) (
  VOID
  );

typedef UINT32 (*XPRF_GET_PSTATEPOWER) (
  VOID
  );

typedef EFI_STATUS (*XPRF_CREATE_SRATAPIC_ENTRY) (
  VOID    *SratApic,
  UINT32  SratApicSize,
  UINT8   ApicModeValue,
  UINT32  *SratTableLength
  );

typedef EFI_STATUS (*XPRF_GATHER_PSTATEDATA) (
  VOID    *PStateData,
  UINT32  PStateDataSize
  );

typedef EFI_STATUS (*XPRF_GETCRAT_HSAPROCINFO) (
  VOID    *CratHsaProcData,
  UINT32  CratHsaProcDataSize
  );
  
typedef EFI_STATUS (*XPRF_CRAT_CACHEENTRY) (
  VOID    *CratCacheEntry,
  UINT32  CratCacheEntrySize
  );

/**
 * RAS xPRF services definitions
 */
typedef EFI_STATUS (*XPRF_COLLECT_DIMM_MAP) (
  SIL_ADDR_DATA *DimmMap
  );

typedef UINT32 (*XPRF_GET_MAX_CPUS) (VOID);

typedef EFI_STATUS (*XPRF_COLLECT_CPU_MAP) (
  SIL_CPU_INFO  *RasCpuMap,
  UINT32        CpuMapSize,
  UINT32        *TotalCpus
  );

typedef VOID (*XPRF_COLLECT_CORE_MCA_INFO) (
  VOID    *Buffer
  );

typedef VOID (*XPRF_COLLECT_MCA_ERROR_INFO) (
  VOID    *Buffer
  );

typedef EFI_STATUS (*XPRF_MCA_ERROR_ADDR_TRANS) (
  uint64_t                *SystemMemoryAddress,
  SIL_NORMALIZED_ADDRESS  *NormalizedAddress,
  SIL_DIMM_INFO           *DimmInfo,
  SIL_ADDR_DATA           *AddrData
  );

typedef EFI_STATUS (*XPRF_TRANS_SYS_ADDR_TO_CS) (
  uint64_t                *SystemMemoryAddress,
  SIL_NORMALIZED_ADDRESS  *NormalizedAddress,
  SIL_DIMM_INFO           *DimmInfo,
  SIL_ADDR_DATA           *AddrData
  );

typedef EFI_STATUS (*XPRF_CORE_MCA_IPID_INST_INIT) (
  SIL_CPU_INFO  *CpuInfo
  );

typedef VOID (*XPRF_CORE_MCA_CFG_UMC) (
  VOID    *Buffer
  );

typedef VOID (*XPRF_SET_NBIO_MCA_CTL_MASK) (
  VOID    *Buffer
  );

/*
 * xPRF Services Protocol definition
 */
typedef struct {
// RAS xPRF services start
  XPRF_COLLECT_DIMM_MAP        CollectDimmMap;
  XPRF_GET_MAX_CPUS            GetMaxCpus;
  XPRF_COLLECT_CPU_MAP         CollectCpuMap;
  XPRF_COLLECT_CORE_MCA_INFO   CollectCoreMcaInfo;
  XPRF_COLLECT_MCA_ERROR_INFO  CollectMcaErrorInfo;
  XPRF_MCA_ERROR_ADDR_TRANS    McaErrorAddrTranslate;
  XPRF_TRANS_SYS_ADDR_TO_CS    TranslateSysAddrToCS;
  XPRF_CORE_MCA_IPID_INST_INIT CoreMcaIpIdInstanceIdInit;
  XPRF_CORE_MCA_CFG_UMC        ProgramCoreMcaConfigUmc;
// RAS xPRF services end
  XPRF_GET_THREADSPER_CORE     GetThreadsPerCore;
  XPRF_GET_PSTATEPOWER         GetPStatePower;
  XPRF_CREATE_SRATAPIC_ENTRY   CreateSratApicEntry;
  XPRF_GATHER_PSTATEDATA       PStateGatherData;
  XPRF_GETCRAT_HSAPROCINFO     GetCratHsaProcInfo;
  XPRF_CRAT_CACHEENTRY         CratCacheEntry;
} AMD_OPENSIL_XPRF_SERVICES_PROTOCOL;
