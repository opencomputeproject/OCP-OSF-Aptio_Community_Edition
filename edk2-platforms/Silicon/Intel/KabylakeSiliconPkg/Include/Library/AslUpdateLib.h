/** @file
  ASL dynamic update library definitions.
  This library provides dymanic update to various ASL structures.
  There may be different libraries for different environments (PEI, BS, RT, SMM).
  Make sure you meet the requirements for the library (protocol dependencies, use
  restrictions, etc).

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _ASL_UPDATE_LIB_H_
#define _ASL_UPDATE_LIB_H_

//
// Include files
//
#include <IndustryStandard/Acpi.h>
#include <Protocol/AcpiTable.h>
#include <Protocol/AcpiSystemDescriptionTable.h>

//
// AML parsing definitions
//
#define AML_RESRC_TEMP_END_TAG  0x0079

//
// ASL PSS package structure layout
//
#pragma pack (1)
typedef struct {
  UINT8     NameOp;           // 12h ;First opcode is a NameOp.
  UINT8     PackageLead;      // 20h ;First opcode is a NameOp.
  UINT8     NumEntries;       // 06h ;First opcode is a NameOp.
  UINT8     DwordPrefix1;     // 0Ch
  UINT32    CoreFrequency;    // 00h
  UINT8     DwordPrefix2;     // 0Ch
  UINT32    Power;            // 00h
  UINT8     DwordPrefix3;     // 0Ch
  UINT32    TransLatency;     // 00h
  UINT8     DwordPrefix4;     // 0Ch
  UINT32    BmLatency;        // 00h
  UINT8     DwordPrefix5;     // 0Ch
  UINT32    Control;          // 00h
  UINT8     DwordPrefix6;     // 0Ch
  UINT32    Status;           // 00h
} PSS_PACKAGE_LAYOUT;
#pragma pack()

/**
  Initialize the ASL update library state.
  This must be called prior to invoking other library functions.


  @retval EFI_SUCCESS                   The function completed successfully.
**/
EFI_STATUS
InitializeAslUpdateLib (
  VOID
  );

/**
  This procedure will update immediate value assigned to a Name

  @param[in] AslSignature               The signature of Operation Region that we want to update.
  @param[in] Buffer                     source of data to be written over original aml
  @param[in] Length                     length of data to be overwritten

  @retval EFI_SUCCESS                   The function completed successfully.
**/
EFI_STATUS
UpdateNameAslCode(
  IN     UINT32                        AslSignature,
  IN     VOID                          *Buffer,
  IN     UINTN                         Length
  );

/**
  This function uses the ACPI support protocol to locate an ACPI table using the .
  It is really only useful for finding tables that only have a single instance,
  e.g. FADT, FACS, MADT, etc.  It is not good for locating SSDT, etc.
  Matches are determined by finding the table with ACPI table that has
  a matching signature and version.

  @param[in] Signature                  Pointer to an ASCII string containing the Signature to match
  @param[in, out] Table                 Updated with a pointer to the table
  @param[in, out] Handle                AcpiSupport protocol table handle for the table found
  @param[in, out] Version               On input, the version of the table desired,
                                        on output, the versions the table belongs to
                                        @see AcpiSupport protocol for details

  @retval EFI_SUCCESS                   The function completed successfully.
**/
EFI_STATUS
LocateAcpiTableBySignature (
  IN      UINT32                        Signature,
  IN OUT  EFI_ACPI_DESCRIPTION_HEADER   **Table,
  IN OUT  UINTN                         *Handle
  );

/**
  This function uses the ACPI support protocol to locate an ACPI SSDT table.
  The table is located by searching for a matching OEM Table ID field.
  Partial match searches are supported via the TableIdSize parameter.

  @param[in] TableId                    Pointer to an ASCII string containing the OEM Table ID from the ACPI table header
  @param[in] TableIdSize                Length of the TableId to match.  Table ID are 8 bytes long, this function
                                        will consider it a match if the first TableIdSize bytes match
  @param[in, out] Table                 Updated with a pointer to the table
  @param[in, out] Handle                AcpiSupport protocol table handle for the table found
  @param[in, out] Version               See AcpiSupport protocol, GetAcpiTable function for use

  @retval EFI_SUCCESS                   The function completed successfully.
**/
EFI_STATUS
LocateAcpiTableByOemTableId (
  IN      UINT8                         *TableId,
  IN      UINT8                         TableIdSize,
  IN OUT  EFI_ACPI_DESCRIPTION_HEADER   **Table,
  IN OUT  UINTN                         *Handle
  );

/**
  This function calculates and updates an UINT8 checksum.

  @param[in] Buffer                     Pointer to buffer to checksum
  @param[in] Size                       Number of bytes to checksum
  @param[in] ChecksumOffset             Offset to place the checksum result in

  @retval EFI_SUCCESS                   The function completed successfully.
**/
EFI_STATUS
AcpiChecksum (
  IN VOID       *Buffer,
  IN UINTN      Size,
  IN UINTN      ChecksumOffset
  );

#endif
