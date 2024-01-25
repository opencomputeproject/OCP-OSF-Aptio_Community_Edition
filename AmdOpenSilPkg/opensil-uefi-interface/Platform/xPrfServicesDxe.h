/**
 * @file  xPrfServicesDxe.h
 * @brief xPRF services data structures and definitions
 *
 */
/*
 * Copyright 2021-2023 Advanced Micro Devices, Inc. All rights reserved.
 *
 */

/**
 * xPRF services protocol function prototypes
 */
EFI_STATUS
xPrfServicesProtocolInstall (VOID);

EFI_STATUS
DummyFunction (VOID);

EFI_STATUS
SilCollectDimmMap (
  SIL_ADDR_DATA *DimmMap
  );

UINT32
SilGetMaxCpus (VOID);

EFI_STATUS
SilCollectCpuMap (
  SIL_CPU_INFO  *RasCpuMap,
  UINT32        CpuMapSize,
  UINT32        *TotalCpus
  );

VOID
SilCollectCoreMcaInfo (
  VOID    *Buffer
  );

VOID
SilCollectMcaErrorInfo (
  VOID    *Buffer
  );

EFI_STATUS
SilMcaErrorAddrTranslate (
  uint64_t                *SystemMemoryAddress,
  SIL_NORMALIZED_ADDRESS  *NormalizedAddress,
  SIL_DIMM_INFO           *DimmInfo,
  SIL_ADDR_DATA           *AddrData
  );

EFI_STATUS
SilTranslateSysAddrToCS (
  uint64_t                *SystemMemoryAddress,
  SIL_NORMALIZED_ADDRESS  *NormalizedAddress,
  SIL_DIMM_INFO           *DimmInfo,
  SIL_ADDR_DATA           *AddrData
  );

EFI_STATUS
SilCoreMcaIpIdInstanceIdInit (
  SIL_CPU_INFO  *RasCpuInfo
  );

VOID
SilProgramCoreMcaConfigUmc (
  VOID  *Buffer
  );

UINT8
SilGetThreadsPerCore (
  VOID
  );

UINT32
SilGetPStatePower (
  VOID
  );

EFI_STATUS
SilCreateSratApicEntry (
  VOID    *SratApic,
  UINT32  SratApicSize,
  UINT8   ApicModeValue,
  UINT32  *SratTableLength
  );

EFI_STATUS
SilPStateGatherData (
  VOID    *PStateData,
  UINT32  PStateDataSize
  );

EFI_STATUS
SilGetCratHsaProcInfo (
  VOID    *CratHsaProcData,
  UINT32  CratHsaProcDataSize
  );

EFI_STATUS
SilCratCacheEntry (
  VOID    *CratCacheEntry,
  UINT32  CratCacheEntrySize
  );
