/**
 
 @file  PspDirectoryLib.h
 @brief Interface to the PSP library header

  Copyright (c) 2023, American Megatrends International LLC.
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#pragma once


#pragma pack(push, 1)

/// Unified Boot BIOS Directory structure
enum _BIOS_DIRECTORY_ENTRY_TYPE
{
  BIOS_PUBLIC_KEY = 0x05,        ///< PSP entry points to BIOS public key stored in SPI space
  BIOS_RTM_SIGNATURE = 0x07,     ///< PSP entry points to signed BIOS RTM hash stored  in SPI space
  MAN_OS = 0x5C,                 ///< PSP entry points to manageability OS binary
  MAN_IP_LIB = 0x5D,             ///< PSP entry points to manageability proprietary IP library
  MAN_CONFIG = 0x5E,             ///< PSP entry points to manageability configuration inforamtion
  BIOS_APCB_INFO = 0x60,         ///< Agesa PSP Customization Block (APCB)
  BIOS_APOB_INFO = 0x61,         ///< Agesa PSP Output Block (APOB) target location
  BIOS_FIRMWARE = 0x62,          ///< BIOS Firmware volumes
  APOB_NV_COPY = 0x63,           ///< APOB data copy on non-volatile storage which will used by ABL during S3 resume
  PMU_INSTRUCTION = 0x64,        ///< Location field pointing to the instruction portion of PMU firmware
  PMU_DATA = 0x65,               ///< Location field pointing to the data portion of PMU firmware
  UCODE_PATCH = 0x66,            ///< Microcode patch
  CORE_MCEDATA = 0x67,           ///< Core MCE data
  BIOS_APCB_INFO_BACKUP = 0x68,  ///< Backup Agesa PSP Customization Block (APCB)
  BIOS_DIR_LV2 = 0x70,           ///< BIOS entry points to Level 2 BIOS DIR
  DISCRETE_USB4_FIRMWARE = 0x71, ///< Discrete USB4 Firmware volumes
};


/// Type attribute for BIOS Directory entry
typedef struct
{
  UINT32 Type : 8;           ///< [0:7], Type of BIOS entry
  UINT32 S3Reload : 1;       ///< [8], Flag to identify the entry need to reload during S3 exit
  UINT32 Reserved1 : 7;      ///< [9:15], Reserved
  UINT32 BiosResetImage : 1; ///< [16], Set for SEC or EL3 fw, which will be authenticate by PSP FW known as HVB
  UINT32 Copy : 1;           ///< [17], Copy: 1- copy BIOS image image from source to destination 0- Set region attribute based on <ReadOnly, Source, size> attributes
  UINT32 ReadOnly : 1;       ///< [18], 1: Set region to read-only 0: Set region to read/write
  UINT32 Compressed : 1;     ///< [19], 1: Compresed
  UINT32 Instance : 4;       ///< [20:23], Specify the Instance of an entry
  UINT32 SubProgram : 3;     ///< [24:26], Specify the SubProgram
  UINT32 RomId : 2;          ///< [27:28], Specify the RomId
  UINT32 Writable : 1;       ///< [29], 1: Region writable, 0: Region read only
  UINT32 Reserved : 2;       ///< [30:31], Reserve for future use
} TYPE_ATTRIB;

#pragma pack(pop)

/**
 *
 *  Get BIOS Directory Entry 's properties by 2 Attributes: EntryType, EntryInstance.
 *
 *  @param[in]     EntryType        BIOS Directory Entry type
 *  @param[in]     EntryInstance    Return the entry which both EntryType & EntryInstance matched
 *  @param[in,out] TypeAttrib       TypeAttrib of entry
 *  @param[in,out] EntryAddress     Address of entry
 *  @param[in,out] EntrySize        Size of entry
 *  @param[in,out] EntryDestination Destination of entry
 *
 *  @retval TRUE   Success to get the Entry 's properties
 *  @retval FALSE  Fail to get the Entry 's properties
 *
 **/
BOOLEAN
BIOSEntryInfo(
    IN UINT8 EntryType,
    IN UINT8 EntryInstance,
    IN OUT TYPE_ATTRIB *TypeAttrib,
    IN OUT UINT64 *EntryAddress,
    IN OUT UINT32 *EntrySize,
    IN OUT UINT64 *EntryDestination
    );
