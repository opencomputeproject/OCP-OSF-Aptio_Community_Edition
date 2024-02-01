/*****************************************************************************
 *
 * Copyright (C) 2020-2024 Advanced Micro Devices, Inc. All rights reserved.
 *
 *****************************************************************************/

/**
  SMM Access Driver header
  
**/

#pragma once

#include <PiDxe.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HobLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/PcdLib.h>
#include <Guid/SmramMemoryReserve.h>
#include <Protocol/SmmAccess2.h>
#include <Protocol/MpService.h>
#include <Register/Amd/Msr.h>

#define  MSR_SYS_CFG          0xC0010010ul    // SYSCFG
#define  MSR_HWCR             0xC0010015ul
#define  SMMADDR_ADDRESS      0xC0010112ul
#define  SMMMASK_ADDRESS      0xC0010113ul
#define      SMMLOCK          0x0000000000000001ull
#define      SMMBASELOCK      0x0000000080000000ull
#define  FCH_MMIO_ADDRESS     0xFED80000ull
#define  SMI_REGISTER_SPACE   0x00000200ull
#define  SMI_TRIG0_OFFSET     0x00000098ull
#define      SMIENB           0x80000000ul

///  Struct for Tseg configuration
typedef struct {
  IN      UINT64 TsegBase;  ///< TsegBase
  IN      UINT64 TsegMask;  ///< TsegMask
} SMM_TSEG_CONFIG;


/**
 *  Opens the SMRAM area to be accessible by a boot-service driver
 *
 *  @param[in]     This                  The EFI_SMM_ACCESS2_PROTOCOL instance
 *
 *  @retval        EFI_SUCCESS           The operation was successful
 *  @retval        EFI_DEVICE_ERROR      SMRAM cannot be opened, perhaps because it is locked
 *  @retval        EFI_UNSUPPORTED       The system does not support opening and closing of SMRAM
 */
EFI_STATUS
EFIAPI
SmmAccess2Open (
  IN       EFI_SMM_ACCESS2_PROTOCOL *This
  );

/**
 *  Inhibits access to the SMRAM.
 *
 *  @param[in]     This                  The EFI_SMM_ACCESS2_PROTOCOL instance
 *
 *  @retval        EFI_SUCCESS           The operation was successful
 *  @retval        EFI_DEVICE_ERROR      SMRAM cannot be closed
 *  @retval        EFI_UNSUPPORTED       The system does not support opening and closing of SMRAM
 */
EFI_STATUS
EFIAPI
SmmAccess2Close (
  IN       EFI_SMM_ACCESS2_PROTOCOL *This
  );

/**
 *  Inhibits access to the SMRAM
 *
 *  @param[in]     This                  The EFI_SMM_ACCESS2_PROTOCOL instance
 *
 *  @retval        EFI_SUCCESS           The device was successfully locked
 *  @retval        EFI_UNSUPPORTED       The system does not support locking of SMRAM
 */
EFI_STATUS
EFIAPI
SmmAccess2Lock (
  IN       EFI_SMM_ACCESS2_PROTOCOL *This
  );

/**
 *  Queries the memory controller for the regions that will support SMRAM
 *
 *  @param[in]       This                The EFI_SMM_ACCESS2_PROTOCOL instance
 *  @param[in, out]  SmramMapSize        A pointer to the size, in bytes, of the SmramMemoryMap
 *                                       buffer. On input, this value is the size of the buffer
 *                                       that is allocated by the caller. On output, it is the
 *                                       size of the buffer that was returned if the buffer was
 *                                       large enough, or, if the buffer was too small, the size
 *                                       of the buffer that is needed to contain the map
 *  @param[in, out]  SmramMap            A pointer to the buffer in which firmware places the
 *                                       current memory map. The map is an array of EFI_SMRAM_DESCRIPTORs
 *
 *  @retval        EFI_SUCCESS           The operation was successful
 *  @retval        EFI_BUFFER_TOO_SMALL  SmramMapSize buffer is too small
 */
EFI_STATUS
EFIAPI
SmmAccess2GetCapabilities (
  IN CONST EFI_SMM_ACCESS2_PROTOCOL *This,
  IN OUT   UINTN                    *SmramMapSize,
  IN OUT   EFI_SMRAM_DESCRIPTOR     *SmramMap
  );
