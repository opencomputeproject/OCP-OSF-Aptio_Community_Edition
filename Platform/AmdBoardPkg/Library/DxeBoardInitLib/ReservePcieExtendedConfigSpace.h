/******************************************************************************
 * Copyright (C) 2021-2024 Advanced Micro Devices, Inc. All rights reserved.
 *
 ***************************************************************************/
#ifndef _RESERVE_PCIE_EXTENDED_CONFIG_SPACE_
#define _RESERVE_PCIE_EXTENDED_CONFIG_SPACE_

#include <Library/DxeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <PiDxe.h>

//
// 48-bit MMIO space (MB-aligned)
//
#define AMD_MMIO_CFG_MSR_ADDR   0xC0010058UL
#define AMD_MMIO_CFG_ADDR_MASK  0xFFFFFFF00000ULL

#pragma pack(1)
typedef union {
  struct {
    // HACK-HACK: Use UINT32 to keep compiler from using SHIFT intrinsics on NOOPT build
    UINT32  Enable : 1;               // [0]
    UINT32  Reserved1 : 1;            // [1]
    UINT32  BusRange : 4;             // [5:2]
    UINT32  Reserved2 : 14;           // [19:6]
    UINT32  MmioCfgBaseAddr : 28;     // [47:20]
    UINT32  Reserved3 : 16;           // [63:48]
  } AsBits;

  UINT64  AsUint64;
} AMD_MMIO_CFG_MSR;
#pragma pack()

/**
  Reserve PCIe Extended Config Space MMIO in the GCD and mark it runtime

  @param[in]  ImageHandle  ImageHandle of the loaded driver.
  @param[in]  SystemTable  Pointer to the EFI System Table.

  @retval  EFI_SUCCESS  One or more of the drivers returned a success code.
  @retval  !EFI_SUCCESS  Error initializing the Legacy PIC.
**/
EFI_STATUS
EFIAPI
ReservePcieExtendedConfigSpace (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
);

#endif    // #ifndef _RESERVE_PCIE_EXTENDED_CONFIG_SPACE_