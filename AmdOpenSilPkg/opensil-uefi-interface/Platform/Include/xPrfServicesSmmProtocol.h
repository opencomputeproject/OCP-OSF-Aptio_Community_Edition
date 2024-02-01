/**
 * @file  xPrfServicesSmmProtocol.h
 * @brief xPRF services protocol definition for SMM
 *
 */
/*
 * Copyright 2021-2024 Advanced Micro Devices, Inc. All rights reserved.
 *
 */
#include <RAS/Common/RasClass-api.h>

/// BSP/AP MSR Access
typedef struct {
  UINT32 RegisterAddress;   ///< MSR Address
  UINT64 RegisterValue;     ///< BSC's MSR Value
  UINT64 RegisterMask;      ///< MSR mask
} XPRF_RAS_MSR_ACCESS;

typedef struct {
  UINTN                 McaBankNumber;
  BOOLEAN               AllBanks;
  BOOLEAN               ThresholdControl;
  UINT16                ThresholdCount;
  UINT8                 ThresholdIntType;
  BOOLEAN               OvrflwChk;
  UINT8                 ErrCounterEn;
} XPRF_SET_MCA_THRESHOLD_ARG;

typedef struct {
  UINT32    LocalSmiStatus;
  UINTN     SmmSaveStateBase;
} XPRF_GET_LOCAL_SMI_STATUS_DATA;

/**
 * Function pointer type definitions for xPRF protocol
 * The xPRF protocol will contain pointers to all xPRF services provided by
 * openSIL.
 */

typedef EFI_STATUS (*XPRF_MCA_ERROR_ADDR_TRANS) (
  UINT64                  *SystemMemoryAddress,
  SIL_NORMALIZED_ADDRESS  *NormalizedAddress,
  SIL_DIMM_INFO           *DimmInfo,
  SIL_ADDR_DATA           *AddrData
  );


typedef EFI_STATUS (*XPRF_TRANS_SYS_ADDR_TO_CS) (
  UINT64                  *SystemMemoryAddress,
  SIL_NORMALIZED_ADDRESS  *NormalizedAddress,
  SIL_DIMM_INFO           *DimmInfo,
  SIL_ADDR_DATA           *AddrData
  );

typedef VOID (*XPRF_GET_LOCAL_SMI_STATUS) (
  VOID *LocalSmiStatus
  );

typedef VOID (*XPRF_COLLECT_MCA_ERROR_INFO) (
  VOID *SilRasMcaErrorInfo
  );

typedef EFI_STATUS (*XPRF_TRANS_SYS_ADDR_TO_DPA) (
  UINT64        *SystemMemoryAddress,
  UINT64        *Dpa,
  SIL_ADDR_DATA *AddrData
  );

typedef EFI_STATUS (*XPRF_UPDATE_DIMM_FRU_TEXT_TO_MCA) (
  SIL_DIMM_FRUTEXT_TABLE   *DimmFruTextTable
  );

typedef VOID (*XPRF_SET_IP_MCA_CTL_MASK) (
  VOID  *Buffer
  );

/*
 * xPRF Services Protocol definition
 */
typedef struct {
  XPRF_MCA_ERROR_ADDR_TRANS         McaErrorAddrTranslate;
  XPRF_TRANS_SYS_ADDR_TO_CS         TranslateSysAddrToCS;
  XPRF_GET_LOCAL_SMI_STATUS         GetLocalSmiStatus;
  XPRF_COLLECT_MCA_ERROR_INFO       CollectMcaErrorInfo;
  XPRF_TRANS_SYS_ADDR_TO_DPA        TranslateSysAddrToDpa;
  XPRF_UPDATE_DIMM_FRU_TEXT_TO_MCA  UpdateDimmFruTextToMca;
  XPRF_SET_IP_MCA_CTL_MASK          SetIpMcaCtlMask;
} AMD_OPENSIL_XPRF_SMM_SERVICES_PROTOCOL;
