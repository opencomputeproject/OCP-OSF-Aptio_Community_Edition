/*****************************************************************************
 *
 * Copyright (C) 2020-2023 Advanced Micro Devices, Inc. All rights reserved.
 *
 *****************************************************************************/

#ifndef __ACPI_COMMON_H__
#define __ACPI_COMMON_H__

#include <Library/AmlLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <IndustryStandard/Acpi.h>
#include <Library/PrintLib.h>
#include <Library/PcdLib.h>
#include <Uefi.h>
#include <Protocol/AcpiTable.h>
#include <Protocol/AcpiSystemDescriptionTable.h>

#include <Library/UefiBootServicesTableLib.h>

#define MAX_LOCAL_STRING_SIZE 20
#define OEM_REVISION_NUMBER      1
#define CREATOR_REVISION         2

#define AMD_DSDT_OEMID  SIGNATURE_64 ('A', 'M', 'D', '_', 'E', 'D', 'K', '2')

extern  EFI_ACPI_TABLE_PROTOCOL   *mAcpiTableProtocol;
extern  EFI_ACPI_SDT_PROTOCOL     *mAcpiSdtProtocol;

/**
  Appends generated AML to an existing ACPI Table

  1. Locate the existing ACPI table
  2. Allocate pool for original table plus new data size
  3. Copy original table to new buffer
  4. Append new data to buffer
  5. Update Table header length (Checksum will be calculated on install)
  6. Uninstall original ACPI table
  7. Install appended table
  8. Free new table buffer since ACPI made a copy.

  @param[in]      Signature     - The Acpi table signature
  @param[in]      OemId         - The Acpi table OEM ID
  @param[in]      AmlData       - The AML data to append

  @retval         EFI_SUCCESS, various EFI FAILUREs.
**/
EFI_STATUS
EFIAPI
AppendExistingAcpiTable (
  IN      UINT32                  Signature,
  IN      UINT64                  OemId,
  IN      AML_OBJECT_INSTANCE     *AmlData
);

/**
  Install CPU devices scoped under \_SB into DSDT

  Determine all the CPU threads and create ACPI Device nodes for each thread.
  AGESA will scope to these CPU records when installing CPU power and
  performance capabilities.

  @param[in]      ImageHandle   - Standard UEFI entry point Image Handle
  @param[in]      SystemTable   - Standard UEFI entry point System Table

  @retval         EFI_SUCCESS, various EFI FAILUREs.
**/
EFI_STATUS
EFIAPI
InstallCpuAcpi (
  IN      EFI_HANDLE         ImageHandle,
  IN      EFI_SYSTEM_TABLE   *SystemTable
);

/**
  Install PCI devices scoped under \_SB into DSDT

  Determine all the PCI Root Bridges and PCI root ports and install resources
  including needed _HID, _CID, _UID, _ADR, _CRS and _PRT Nodes.

  @param[in]      ImageHandle   - Standard UEFI entry point Image Handle
  @param[in]      SystemTable   - Standard UEFI entry point System Table

  @retval         EFI_SUCCESS, various EFI FAILUREs.
**/
EFI_STATUS
EFIAPI
InstallPciAcpi (
  IN      EFI_HANDLE         ImageHandle,
  IN      EFI_SYSTEM_TABLE   *SystemTable
);

VOID
EFIAPI
InstallAcpiSpmiTable (
  VOID
  );

VOID
EFIAPI
InstallAcpiIvrsTable (
  VOID
  );
#endif // __ACPI_COMMON_H__