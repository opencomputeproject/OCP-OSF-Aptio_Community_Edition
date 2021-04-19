/** @file
  Boot service DXE ASL update library implementation.

  These functions in this file can be called during DXE and cannot be called during runtime
  or in SMM which should use a RT or SMM library.

  This library uses the ACPI Support protocol.

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <Uefi/UefiSpec.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>

#include <Library/AslUpdateLib.h>

//
// Function implemenations
//

/**
  Initialize the ASL update library state.
  This must be called prior to invoking other library functions.

  @retval EFI_SUCCESS          - The function completed successfully.
**/
EFI_STATUS
InitializeAslUpdateLib (
  VOID
  )
{
  return EFI_SUCCESS;
}


/**
  This procedure will update immediate value assigned to a Name

  @param[in] AslSignature      - The signature of Operation Region that we want to update.
  @param[in] Buffer            - source of data to be written over original aml
  @param[in] Length            - length of data to be overwritten

  @retval EFI_SUCCESS          - The function completed successfully.
**/
EFI_STATUS
UpdateNameAslCode (
  IN     UINT32                        AslSignature,
  IN     VOID                          *Buffer,
  IN     UINTN                         Length
  )
{
  return EFI_SUCCESS;
}


/**
  This function uses the ACPI SDT protocol to locate an ACPI table.
  It is really only useful for finding tables that only have a single instance,
  e.g. FADT, FACS, MADT, etc.  It is not good for locating SSDT, etc.

  @param[in] Signature           - Pointer to an ASCII string containing the OEM Table ID from the ACPI table header
  @param[in, out] Table          - Updated with a pointer to the table
  @param[in, out] Handle         - AcpiSupport protocol table handle for the table found
  @param[in, out] Version        - The version of the table desired

  @retval EFI_SUCCESS            - The function completed successfully.
**/
EFI_STATUS
LocateAcpiTableBySignature (
  IN      UINT32                        Signature,
  IN OUT  EFI_ACPI_DESCRIPTION_HEADER   **Table,
  IN OUT  UINTN                         *Handle
  )
{
  return EFI_SUCCESS;
}

/**
  This function uses the ACPI SDT protocol to locate an ACPI SSDT table.

  @param[in] TableId           - Pointer to an ASCII string containing the OEM Table ID from the ACPI table header
  @param[in] TableIdSize       - Length of the TableId to match.  Table ID are 8 bytes long, this function
                                 will consider it a match if the first TableIdSize bytes match
  @param[in, out] Table        - Updated with a pointer to the table
  @param[in, out] Handle       - AcpiSupport protocol table handle for the table found
  @param[in, out] Version      - See AcpiSupport protocol, GetAcpiTable function for use

  @retval EFI_SUCCESS          - The function completed successfully.
**/
EFI_STATUS
LocateAcpiTableByOemTableId (
  IN      UINT8                         *TableId,
  IN      UINT8                         TableIdSize,
  IN OUT  EFI_ACPI_DESCRIPTION_HEADER   **Table,
  IN OUT  UINTN                         *Handle
  )
{
  return EFI_SUCCESS;
}

/**
  This function calculates and updates an UINT8 checksum.

  @param[in] Buffer          Pointer to buffer to checksum
  @param[in] Size            Number of bytes to checksum
  @param[in] ChecksumOffset  Offset to place the checksum result in

  @retval EFI_SUCCESS        The function completed successfully.
**/
EFI_STATUS
AcpiChecksum (
  IN VOID       *Buffer,
  IN UINTN      Size,
  IN UINTN      ChecksumOffset
  )
{
  return EFI_SUCCESS;
}
