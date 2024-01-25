/**
 * @file  xPrfServicesPpi.h
 * @brief xPRF services protocol definition
 *
 */
/*
 * Copyright 2021-2023 Advanced Micro Devices, Inc. All rights reserved.
 *
 */

/**
 * xPrf services definitions
 */
typedef EFI_STATUS (*AMD_XPRF_DUMMY_FUNCTION) (VOID);

typedef VOID* (*XPRF_GET_PCIE_TOPOLOGY_STRUCT) (VOID);
typedef EFI_STATUS (*XPRF_GET_SYSTEM_MEM_MAP) (UINT32*, UINT64*, VOID**);
typedef EFI_STATUS (*XPRF_GET_SMBIOS_MEM_INFO) (VOID*);
typedef EFI_STATUS (*XPRF_GET_MEM_INIT_INFO) (VOID*);

/*
 * xPRF Services Protocol definition
 */
typedef struct {
  AMD_XPRF_DUMMY_FUNCTION            Dummy;
  XPRF_GET_PCIE_TOPOLOGY_STRUCT      SilGetNbioTopologyStructure;
  XPRF_GET_SYSTEM_MEM_MAP            SilGetSystemMemoryMap;
  XPRF_GET_SMBIOS_MEM_INFO           SilGetSmbiosMemInfo;
  XPRF_GET_MEM_INIT_INFO             SilGetMemInitInfo;
} AMD_OPENSIL_XPRF_SERVICES_PPI;
