/*
*****************************************************************************
*
 * Copyright (C) 2008-2023 Advanced Micro Devices, Inc. All rights reserved.
 *
*******************************************************************************
*/
/* $NoKeywords:$ */
 /**
 * @file
 *
 * Amd Memory Info Hob for Genoa.
 *
 * Contains GUID Declaration for Memory Info Hob
 *
 */

/**
 * @brief Include guard to define Memory Info HOB macros and data structures
 */
#ifndef _AMD_MEMORY_INFO_HOB_H_
#define _AMD_MEMORY_INFO_HOB_H_

/**
 * @brief 128 bit Buffer containing UID Unique Identifier value for Memory Info HOB.
 * EFI_GUID defined in UefiBaseType.h
 */
extern EFI_GUID gAmdMemoryInfoHobGuid;

#pragma pack (push, 1)

/**
 * @brief Include guard to define General Memory Status Codes
 */
#ifndef _MEM_STATUS_CODE_GENERAL_INFO_
#define _MEM_STATUS_CODE_GENERAL_INFO_

/**
 * @brief  List Status constant used in the Memory Info HOB
 * @details Memory Status info List containing General Status Codes and  specific Codes related to
 * Memory interleaving,  BankGroup Swapping, MemBusFreqLimit, Dimms with ECC support etc.
 */
typedef enum {
  /// General Status Codes
  CfgStatusSuccess = 0xC000,                 ///< General Status Code indicating Success = 0xC000
  CfgStatusOptionNotEnabled,                 ///< General Status Code indicating the Memory option is not enabled = 0xC001
  CfgStatusDimmNotDetectedOrChannelDisabled, ///< General Status Code indicating Dimm not Detected or Channel not Enabled = 0xC002
  CfgStatusCbsOptionOverride,                ///< General Status Code indicating CBS option Overridden = 0xC003
  CfgStatusOptionNotSupported,               ///< General Status Code indicating Memory option is Not Supported = 0xC004
  /// ChipSelect Interleaving
  CfgStatusCsAddressingNotMatch = 0xC010,    ///< Specific Status Code indicating ChipSelect Interleaving Addressing Mismatch = 0xC010
  CfgStatusNotPowerOf2EnabledCs,             ///< Specific Status Code indicating ChipSelect enabled is not a power of 2 = 0xC011
  /// Dram Ecc
  CfgStatusNotAllDimmEccCapable,             ///< Specific Status Code indicating Not every Dimm is ECC Capable = 0xC012
  /// Parity
  CfgStatusRcdParityNotEnabled,              ///< Specific Status Code indicating Rcd Parity Is Not Enabled = 0xC013
  /// AutoREfFineGranMode
  CfgStatusRefreshFix2XOnSelectiveODMDimms,  ///< Specific Status Code indicating Refresh Rate 2X on Selective Dimms = 0xC014
  /// UserTimingMode & UserTimingValue & MemBusFreqLimit
  CfgStatusDdrRatePORLimited,                ///< Specific Status Code indicating DDR Freq is POR limited = 0xC015
  CfgStatusDdrRateUserTimingLimited,         ///< Specific Status Code indicating DDR Freq is limited by User Timing = 0xC016
  CfgStatusDdrRateUserTimingSpecific,        ///< Specific Status Code indicating DDR Freq is set to User Specific Value = 0xC017
  CfgStatusDdrRateEnforcedDdrRate,           ///< Specific Status Code indicating Enforced DDR Rate = 0xC018
  CfgStatusDdrRateMemoryBusFrequencyLimited, ///< Specific Status Code indicating DDR Rate is limited by Memory Bus Frequency = 0xC019
  /// EnableBankGroupSwap
  CfgStatusBankGroupSwapAltOverride,         ///< Specific Status Code indicating BankGroup Swap Enabled = 0xC01A
  /// Boundary check
  CfgMaxStatusCode = 0xCFFF                  ///< Boundary check Status Code = 0xCFFF
} MEM_STATUS_CODE_GENERAL_INFO;

#endif

/**
 * @brief Macro to get Dimm Presence info
 * @param[in] HobStart       (AMD_MEM_CFG_INFO_HOB *) Handle to Mem Config Info in HOB. Starting address of AMD_MEM_CFG_INFO_HOB
 * @param[in] DimmPresentMap UINT16                   HOB address of DimmPresentMap info
 * @param[in] Socket         UINT16                   Current Socket index
 * @param[in] Die            UINT16                   Current Die index in the current Socket
 * @param[in] Channel        UINT16                   Current Channel index
 * @param[in] Dimm           UINT16                   Current Dimm index
 */
#define GET_HOB_DIMM_PRESENCE_OF(HobStart, DimmPresentMap, Socket, Die, Channel, Dimm) \
       (UINT8) ((DimmPresentMap##[Socket * ((AMD_MEM_CFG_INFO_HOB_RS *)HobStart)->MaxDiePerSocket +\
                      Die] >> (Channel * ((AMD_MEM_CFG_INFO_HOB_RS *)HobStart)->MaxDimmPerChannel + Dimm)) & 1)

/**
 * @brief Macro to obtain HOB address
 * @param[in] HobStart (AMD_MEM_CFG_INFO_HOB *) Handle to Mem Config Info in HOB. Starting address of AMD_MEM_CFG_INFO_HOB
 * @param[in] Offset UINT16 Offset value to calculate the target HOB address for the requested Memory Config info
 */
#define GET_MEM_CFG_INFO_HOB_ADDR(HobStart, Offset) \
       (VOID *) ((UINT8 *)HobStart + (UINT16) ((AMD_MEM_CFG_INFO_HOB_RS *)HobStart)->Offset)

/**
 * @brief  Config structure to store the status value and Status Code of Memory Config info in HOB
 * @details Contains Value and Enabled fields as well as the enumerated Status Code value as listed in MEM_STATUS_CODE_GENERAL_INFO
 */
typedef struct _MEM_CFG_INFO_HOB {
   union {
     BOOLEAN Enabled;   ///< Status.Enabled - TRUE: Enabled.
     UINT16 Value;      ///< Status.Value - Configured value.
   } Status;
   UINT16 StatusCode;   ///< Status Code: MEM_STATUS_CODE_GENERAL_INFO
} MEM_CFG_INFO_HOB;

/**
 * @brief Macro that defines the Memory Config Info HOB version 0400
 */
#define AMD_MEM_CFG_INFO_HOB_VER_0400 0x0400

/**
 * @brief  Memory Config Info HOB structure
 * @details Indicates all the Memory Config Information both Fixed data as well as
 * the offset of Dynamic Data that can be accessed from the Mem Config Info HOB
 */
typedef struct _AMD_MEM_CFG_INFO_HOB_RS {
  /// Version
  UINT32 Version;            ///< Indicates the HOB version

  /// Max. number of Socket/Die/Channel/Dimm supported.
  UINT8  MaxSocketSupported; ///< Indicates max socket supported
  UINT8  MaxDiePerSocket;    ///< Indicates max die per socket
  UINT8  MaxChannelPerDie;   ///< Indicates max channel per die
  UINT8  MaxDimmPerChannel;  ///< Indicates max dimm per channel

  /// Fixed Data
  MEM_CFG_INFO_HOB MbistTestEnable;       ///< MbistTestEnable
  MEM_CFG_INFO_HOB MbistAggressorEnable;  ///< MbistAggressorEnable
  MEM_CFG_INFO_HOB MbistPerBitSlaveDieReport;   ///< MbistPerBitSlaveDieReport
  MEM_CFG_INFO_HOB DramTempControlledRefreshEn; ///< DramTempControlledRefreshEn
  MEM_CFG_INFO_HOB UserTimingMode;        ///< UserTimingMode
  MEM_CFG_INFO_HOB UserTimingValue;       ///< UserTimingValue
  MEM_CFG_INFO_HOB MemBusFreqLimit;       ///< MemBusFreqLimit
  MEM_CFG_INFO_HOB EnablePowerDown;       ///< EnablePowerDown
  MEM_CFG_INFO_HOB DramDoubleRefreshRate; ///< DramDoubleRefreshRate
  MEM_CFG_INFO_HOB PmuTrainMode;          ///< PmuTrainMode
  MEM_CFG_INFO_HOB EccSymbolSize;         ///< EccSymbolSize
  MEM_CFG_INFO_HOB UEccRetry;             ///< UEccRetry
  MEM_CFG_INFO_HOB IgnoreSpdChecksum;     ///< IgnoreSpdChecksum
  MEM_CFG_INFO_HOB EnableBankGroupSwapAlt;///< EnableBankGroupSwapAlt
  MEM_CFG_INFO_HOB EnableBankGroupSwap;   ///< EnableBankGroupSwap
  MEM_CFG_INFO_HOB DdrRouteBalancedTee;   ///< DdrRouteBalancedTee
  MEM_CFG_INFO_HOB NvdimmPowerSource;     ///< NvdimmPowerSource
  MEM_CFG_INFO_HOB OdtsCmdThrotEn;        ///< OdtsCmdThrotEn
  MEM_CFG_INFO_HOB OdtsCmdThrotCyc;       ///< OdtsCmdThrotCyc

  /// Offset of each item in the 'Dynamic data' below
  UINT16 DimmPresentMapOffset;            ///< Offset of DimmPresentMap in the struct
  UINT16 ChipselIntlvOffset;              ///< Offset of ChipselIntlv in the struct
  UINT16 DramEccOffset;                   ///< Offset of DramEcc in the struct
  UINT16 DramParityOffset;                ///< Offset of DramParity in the struct
  UINT16 AutoRefFineGranModeOffset;       ///< Offset of AutoRefFineGranMode in the struct

  /// Dynamic data (appended <in order> following this structure) - DO NOT UNCOMMENT BELOW
  /// Status reporting stuff
  //UINT16 DimmPresentMap[MaxSocketSupported * MaxDiePerSocket]; ///< DimmPresentMap
                                                                 ///< Bit[1:0] - Dimm[1:0] of Channel0, .. , Bit[15:14] - Dimm[1:0] of Channel7
  //MEM_CFG_INFO_HOB ChipselIntlv[MaxSocketSupported * MaxDiePerSocket * MaxChannelPerDie];  ///< ChipselIntlv (APCB_TOKEN_CONFIG_ENABLECHIPSELECTINTLV)
  //MEM_CFG_INFO_HOB DramEcc[MaxSocketSupported * MaxDiePerSocket]; ///< DramEcc (APCB_TOKEN_CONFIG_ENABLEECCFEATURE)
  //MEM_CFG_INFO_HOB DramParity[MaxSocketSupported * MaxDiePerSocket]; ///< DramParity (APCB_TOKEN_CONFIG_ENABLEPARITY)
  //MEM_CFG_INFO_HOB AutoRefFineGranMode[MaxSocketSupported * MaxDiePerSocket];  ///< AutoRefFineGranMode (APCB_TOKEN_CONFIG_AUTOREFFINEGRANMODE)

  /// Platform Tuning stuff
  // ...
} AMD_MEM_CFG_INFO_HOB_RS;

/**
 * @brief Memory descriptor structure for each memory range
 */
typedef struct {
  UINT64  Base;                             ///< Base address of memory rang
  UINT64  Size;                             ///< Size of memory rang
  UINT32  Attribute;                        ///< Attribute of memory rang
  UINT32  Reserved;                         ///< For alignment purpose
} AMD_MEMORY_RANGE_DESCRIPTOR;

/**
 * @brief Memory attribute in the memory range descriptor = AVAILABLE
 */
#define AMD_MEMORY_ATTRIBUTE_AVAILABLE            0x1
/**
 * @brief Memory attribute in the memory range descriptor = UMA
 */
#define AMD_MEMORY_ATTRIBUTE_UMA                  0x2
/**
 * @brief Memory attribute in the memory range descriptor = MMIO
 */
#define AMD_MEMORY_ATTRIBUTE_MMIO                 0x3
/**
 * @brief Memory attribute in the memory range descriptor = RESERVED
 */
#define AMD_MEMORY_ATTRIBUTE_RESERVED             0x4
/**
 * @brief Memory attribute in the memory range descriptor = GPUMEM
 */
#define AMD_MEMORY_ATTRIBUTE_GPUMEM               0x5
/**
 * @brief Memory attribute in the memory range descriptor = GPU_SP
 */
#define AMD_MEMORY_ATTRIBUTE_GPU_SP               0x6
/**
 * @brief Memory attribute in the memory range descriptor = GPU_RESERVED
 */
#define AMD_MEMORY_ATTRIBUTE_GPU_RESERVED         0x7
/**
 * @brief Memory attribute in the memory range descriptor = GPU_RESERVED_TMR
 */
#define AMD_MEMORY_ATTRIBUTE_GPU_RESERVED_TMR     0x8
/**
 * @brief Memory attribute in the memory range descriptor = RESERVED_SMUFEATURES
 */
#define AMD_MEMORY_ATTRIBUTE_Reserved_SmuFeatures 0x9
/**
 * @brief Memory attribute in the memory range descriptor = MMIO_RESERVED
 */
#define AMD_MEMORY_ATTRIBUTE_MMIO_RESERVED        0xA

/**
 * @brief Memory info HOB structure
 */
typedef struct  {
  UINT32              Version;                          ///< Version of HOB structure
  BOOLEAN             AmdMemoryVddioValid;              ///< This field determines if Vddio is valid
  UINT16              AmdMemoryVddio;                   ///< Vddio Voltage
  BOOLEAN             AmdMemoryVddpVddrValid;           ///< This field determines if VddpVddr is valid
  UINT8               AmdMemoryVddpVddr;                ///< VddpVddr voltage
  BOOLEAN             AmdMemoryFrequencyValid;          ///< Memory Frequency Valid
  UINT32              AmdMemoryFrequency;               ///< Memory Frquency
  UINT32              AmdMemoryDdrMaxRate;              ///< Memory DdrMaxRate
  UINT32              NumberOfDescriptor;               ///< Number of memory range descriptor
  AMD_MEMORY_RANGE_DESCRIPTOR Ranges[1];                ///< Memory ranges array
} AMD_MEMORY_INFO_HOB;

#pragma pack (pop)

/**
 * @brief Macro that defines the Memory Info HOB version
 */
#define AMD_MEMORY_INFO_HOB_VERISION        0x00000110ul  // Ver: 00.00.01.10

#endif // _AMD_MEMORY_INFO_HOB_H_



