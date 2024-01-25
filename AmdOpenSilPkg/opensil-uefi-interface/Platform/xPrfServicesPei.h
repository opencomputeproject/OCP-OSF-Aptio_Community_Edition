
/**
 * @file  xPrfServicesPei.h
 * @brief xPRF services data structures and definitions
 *
 */
/*
 * Copyright 2021-2023 Advanced Micro Devices, Inc. All rights reserved.
 *
 */

/**
 * xPrf services Ppi function prototypes
 */
EFI_STATUS
xPrfServicesPpiInstall (VOID);

EFI_STATUS
DummyFunction1 (VOID);

VOID*
SilGetNbiotopologyStructure (VOID);

/**
 * SilGetSystemMemoryMap
 *
 * @details  Get top of memory (Tom2) for the UEFI Host along with
 *           memory map, and number of holes
 *
 * @param    NumberOfHoles      Number of memory holes
 * @param    TopOfSystemMemory  Top of memory address
 * @param    MemHoleDescPtr     Memory descriptor structure
 *
 * @retval   EFI_SUCCESS             Info extracted successfully.
 * @retval   EFI_INVALID_PARAMETER   Failure.
 **/
EFI_STATUS
SilGetSystemMemoryMap (
  UINT32                     *NumberOfHoles,
  UINT64                     *TopOfSystemMemory,
  VOID                       **MemHoleDescPtr
);

/**
 * SilGetSmbiosMemInfo
 *
 * @details This function gets memory related information that
 *          gets used to populate SMBIOS Type 16, Type
 *          17, Type 19 and Type 20.
 *
 * @param   DmiInfo  Point to the DMI records (Type 16, 17, 19, and 20)
 *          Based on Struct SIL_DMI_INFO
 *
 * @retval  EFI_SUCCESS              Info extracted successfully.
 * @retval  EFI_INVALID_PARAMETER    Failure.
 **/
EFI_STATUS
SilGetSmbiosMemInfo(
  VOID *DmiInfo
  );

/**
 * SilGetMemInitInfo
 *
 * @details  This function populate AMD_MEMORY_SUMMARY
 *           structure which is used to report DRAM
 *           info to the Host
 *
 * @param    *MemInitTable   - Pointer to AMD_MEMORY_SUMMARY
 *
 * @retval    EFI_SUCCESS             Info extracted successfully.
 * @retval    EFI_INVALID_PARAMETER   Failure.
 **/
EFI_STATUS
SilGetMemInitInfo(
   VOID *MemInitTable
  );
