/*****************************************************************************
 * Copyright (C) 2016-2023 Advanced Micro Devices, Inc. All rights reserved.
 *
*****************************************************************************
*/
/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Memory Info Hob PPI prototype definition
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AgesaPkg
 * @e sub-project:  Memory
 */

/**
 * @brief Include guard to define Memory Info HOB PPI macros and data structures
 */
#ifndef _AMD_MEMORY_INFO_HOB_PPI_H_
#define _AMD_MEMORY_INFO_HOB_PPI_H_

/**
 * @brief 128 bit Buffer containing UID Unique Identifier value for Memory PPI Info in HOB. 
 * EFI_GUID defined in UefiBaseType.h
 */
extern EFI_GUID gAmdMemoryInfoHobPpiGuid;

/**
 * @brief Memory PPI info HOB structure
 */ 
typedef struct _AMD_MEMORY_INFO_HOB_PPI {
  UINT32    Revision;                                              ///< revision
} AMD_MEMORY_INFO_HOB_PPI;

/**
 * @brief Macro that defines the Memory Config Info HOB Revision
 */
#define AMD_MEMORY_INFO_HOB_PPI_REVISION   0x01

/**
 * @brief Macro that defines the Memory PPI Info HOB version 0400
 */
#define AMD_MEMORY_INFO_HOB_PPI_REV_0400   0x0400

#endif


