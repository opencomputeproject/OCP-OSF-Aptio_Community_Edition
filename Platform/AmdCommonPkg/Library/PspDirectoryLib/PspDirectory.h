/*****************************************************************************
 *
 * Copyright (C) 2020-2023 Advanced Micro Devices, Inc. All rights reserved.
 *
 *****************************************************************************/

/**
 
 @file  PspDirectory.h
 @brief Interface to the PSP library header.

**/

#include <Library/PspDirectoryLib.h>

#pragma once

#pragma pack(push, 1)


#define PSP_DIRECTORY_HEADER_SIGNATURE 0x50535024ul     ///< $PSP
#define PSP_LV2_DIRECTORY_HEADER_SIGNATURE 0x324C5024ul ///< $PL2

#define FORCE_SPIADDR_BIT24(SpiAddress) (SpiAddress | 0xFF000000)

#define FCH_ROM_INVALID_START_ADDRESS_2 0xFFFFFFFFul
#define FCH_ROM_START_ADDRESS_2 0xFF000000ul
#define FCH_ROM_END_ADDRESS_2 0xFFFFFFFFul



#define MAX_DIR_ENTRY_COUNT 64


/// Structure for PSP Entry
typedef struct
{
  TYPE_ATTRIB TypeAttrib; ///< Type of PSP entry; 32 bit long
  UINT32 Size;            ///< Size of PSP Entry in bytes
  UINT64 Location;        ///< Location of PSP Entry (byte offset from start of SPI-ROM)
  UINT64 Destination;     ///< Destination of PSP Entry copy to
} BIOS_DIRECTORY_ENTRY;

#define BIOS_DIRECTORY_HEADER_SIGNATURE 0x44484224ul     ///< $BHD BIOS Directory Signature
#define BIOS_LV2_DIRECTORY_HEADER_SIGNATURE 0x324C4224ul ///< $BL2 BIOS Directory Lv2 Signature

///
/// Define structure for 32 bits Additional Info field of PSP Directory Header
///
typedef struct
{
  UINT32 MaxSize : 10;     ///< Directory Size in 4K bytes
  UINT32 SpiBlockSize : 4; ///< Modifiable Entry alignment
  UINT32 BaseAddress : 15; ///< [26:12] of Directory Image Base Address
  UINT32 AddressMode : 2;  ///< Directory Address Mode (0, 1, 2, or 3)
  UINT32 Reserved : 1;     ///< Reserved
} PSP_DIRECTORY_HEADER_ADDITIONAL_INFO_FIELD;

///
/// Define union type for 32 bits Additional Info field
///
typedef union
{
  PSP_DIRECTORY_HEADER_ADDITIONAL_INFO_FIELD Field; // Definition of each field
  UINT32 Value;                                     // Group it as 32bits Int
} PSP_DIRECTORY_HEADER_ADDITIONAL_INFO;

///
/// Define structure for PSP directory
///
typedef struct
{
  UINT32 Cookie;                                       ///< "$PSP"
  UINT32 Checksum;                                     ///< 32 bit CRC of header items below and the entire table
  UINT32 TotalEntries;                                 ///< Number of PSP Entries
  PSP_DIRECTORY_HEADER_ADDITIONAL_INFO AdditionalInfo; ///< Additional Info
} PSP_DIRECTORY_HEADER;


/// Structure for BIOS directory
typedef struct
{
  PSP_DIRECTORY_HEADER Header;       ///< PSP directory header
  BIOS_DIRECTORY_ENTRY BiosEntry[MAX_DIR_ENTRY_COUNT]; ///< Array of PSP entries each pointing to a binary in SPI flash
                                     ///< The actual size of this array comes from the
                                     ///< header (PSP_DIRECTORY.Header.TotalEntries)
} BIOS_DIRECTORY;

#define IS_SPI_ROM2_OFFSET(a) (((a) < 0x1000000) ? TRUE : FALSE)
#define IS_IN_SPI_ROM2_WINDOW(a) ((((a) & ~(0xFFFFFF)) == 0xFF000000) ? TRUE : FALSE)

#define FIRMWARE_TABLE_SIGNATURE 0x55AA55AAul

#define FCH_ROM_SIZE_16M 0x1000000
#define FCH_ROM_SIZE_32M (2 * FCH_ROM_SIZE_16M)
#define FCH_ROM_SIZE_64M (4 * FCH_ROM_SIZE_16M)

#define BYTE0 0x00
#define BYTE1 0x01
#define BYTE2 0x02
#define BYTE3 0x03
#define BYTE4 0x04
#define BYTE5 0x05
#define BYTE6 0x06
#define BYTE7 0x07
#define BYTE_MASK 0xFF
#define BYTE_OFFSET 0x08


///
/// Define the structure OEM signature table
///
typedef struct _FIRMWARE_ENTRY_TABLEV2
{
  UINT32 Signature;        ///< 0x00 Signature should be 0x55AA55AAul
  UINT32 ImcRomBase;       ///< 0x04 Base Address for Imc Firmware
  UINT32 GecRomBase;       ///< 0x08 Base Address for Gmc Firmware
  UINT32 XHCRomBase;       ///< 0x0C Base Address for XHCI Firmware
  UINT32 LegacyPspDirBase; ///< 0x10 Base Address of PSP directory for legacy programs
  UINT32 PspDirBase;       ///< 0x14 Base Address for PSP directory
  UINT32 Reserved[3];
  UINT32 Config;           ///< 0x24 reserved for EFS configuration
  UINT32 NewBiosDirBase;   ///< 0x28 Generic Base address for all program start from RN
  UINT32 PspDirBackupBase; ///< 0x2C Backup PSP directory address
  UINT32 PTFW;             ///< 0x30 Point to promontory firmware
  UINT32 LPPTFW;           ///< 0x34 Point to LP promontory firmware
  UINT32 PT19FW;           ///< 0x38 Point to promontory19 firmware
} FIRMWARE_ENTRY_TABLEV2;

#define IS_ADDRESS_MODE_1(a) (RShiftU64((a), 62) == 1 ? TRUE : FALSE) // relative to BIOS image base 0
#define IS_ADDRESS_MODE_2(a) (RShiftU64((a), 62) == 2 ? TRUE : FALSE) // relative to current directory header
#define IS_ADDRESS_MODE_3(a) (RShiftU64((a), 62) == 3 ? TRUE : FALSE) // relative to active image slot address (as of now, active image slot address is equal to PSP L2 base address)
#define IS_SPI_OFFSET(a) (((a)&0xFF000000) != 0xFF000000 ? TRUE : FALSE)

//#define MaxDirEntryNumber 64
//#define MaxPspDirSize sizeof(PSP_DIRECTORY_HEADER) + (sizeof(BIOS_DIRECTORY_ENTRY) * MaxDirEntryNumber)

//#define ALIGNMENT_4K BASE_4KB
//#define ALIGN_CHECK(addr, alignment) ((((UINTN)(addr)) & ((alignment)-1)) == 0)
//#define ALIGN_4K_CHECK(addr) ALIGN_CHECK((addr), ALIGNMENT_4K)

//#define IS_VALID_ADDR32(addr) (((UINT32)(addr) != 0) && (UINT32)(addr) != 0xFFFFFFFF)
//#define MaxImageSlotInfoSize sizeof(IMAGE_SLOT_INFO)

#pragma pack(pop)

/**
 * @brief translate entry offset to correct location based on address mode
 *
 * @param[in] EntryLocation     The location of the entry before translation
 * @param[in] DirectoryHdrAddr  Directory header address
 * @param[in] ImageSlotAddr     Image slot address if applicable, if no image slot, leave it as 0
 *
 * @return UINT64           return translated entry location
 */
UINT64
TranslateEntryLocation(
    IN UINT64 EntryLocation,
    IN UINT64 DirectoryHdrAddr);


