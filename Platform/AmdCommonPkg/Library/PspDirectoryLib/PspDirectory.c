/*****************************************************************************
 *
 * Copyright (C) 2020-2023 Advanced Micro Devices, Inc. All rights reserved.
 *
 *****************************************************************************/

/**
 
 @file  PspDirectory.c
 @brief Library to retrieve the PSP Level2 BIOS directory entries.

**/

#include <Uefi.h>
#include <Base.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <SilSocLogicalId.h>
#include <CommonLib/CpuLib.h>
#include <Pci.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <CommonLib/SmnAccess.h>
#include <Pi/PiBootMode.h>
#include <Pi/PiHob.h>
#include <Library/HobLib.h>
#include "PspDirectory.h"


typedef struct {
  UINT64          PspLv2Entry;
  BIOS_DIRECTORY  PspBiosDir;
} BIOS_DIRECTORY_HOB;

BIOS_DIRECTORY_HOB  *BiosDirectoryHob;

/**
* FchSpiRomRead - Read the SPI ROM via ROM2 24b address
*
*
* @param[in] Offset - the offset address of host ROM2 address window
* @param[out]*Data - buffer to save the data
* @param[in] Length - the size to read from ROM
*
* @retval BOOLEAN - success read or not
*/
BOOLEAN
FchSpiRomRead (
  IN      UINT32            Offset,
  OUT     UINT8             *Data,
  IN      UINT32            Length
)
{
  UINT64          Value;
  UINT32          Index, DataCount, ByteCount;
  UINT32          StartAddr, EndAddr;
  UINT32          StartAlign, EndAlign;
  UINT32          FrontPad, BackPad;

  if ((Data == NULL) || (Length == 0)) {
    return FALSE;
  }

  if ((Offset >= FCH_ROM_SIZE_16M) || (Length > FCH_ROM_SIZE_16M) || ((Offset + Length) > FCH_ROM_SIZE_16M)) {
    return FALSE;
  }

  StartAddr = Offset;
  EndAddr = Offset + Length - 1;

  if (StartAddr % BIT3) {
    StartAlign = (StartAddr / BIT3) * BIT3;
    FrontPad = StartAddr - StartAlign;
  } else {
    StartAlign = StartAddr;
    FrontPad = BYTE0;
  }

  if (EndAddr % BIT3) {
    EndAlign = (EndAddr / BIT3) * BIT3;
    BackPad = EndAddr - EndAlign;
  } else {
    EndAlign = EndAddr;
    BackPad = BYTE0;
  }

  DataCount = 0;
  for (Index = StartAlign; Index <= EndAlign; Index += sizeof(UINT64)) {
    Value = MmioRead64 (FCH_ROM_START_ADDRESS_2 + Index);
    if (StartAlign == EndAlign) {
      for (ByteCount = FrontPad; ByteCount <= BackPad; ByteCount++, DataCount++) {
        *(Data + DataCount) = (RShiftU64 (Value, (ByteCount * BYTE_OFFSET))) & BYTE_MASK;
      }
    } else {
      if (Index == StartAlign) {
        for (ByteCount = FrontPad; ByteCount <= BYTE7; ByteCount++, DataCount++) {
          *(Data + DataCount) = (RShiftU64 (Value, (ByteCount * BYTE_OFFSET))) & BYTE_MASK;
        }
      } else if (Index == EndAlign) {
        for (ByteCount = BYTE0; ByteCount <= BackPad; ByteCount++, DataCount++) {
          *(Data + DataCount) = (RShiftU64 (Value, (ByteCount * BYTE_OFFSET))) & BYTE_MASK;
        }
      } else {
        *(UINT64 *)(Data + DataCount) = Value;
        DataCount += sizeof(UINT64)/sizeof(UINT8);
      }
    }
  }

  return TRUE;
}


/**
 * ReadSpiRom - Extended function to read the SPI ROM
 *
 *
 * @param[in] Location - Offset of host SPI ROM2 address window
 *                     - Host address in ROM2 window to SPI ROM
 * @param[out]*Data - Buffer address to save the data
 * @param[in] Length - The number of byte to read
 *
 * @retval BOOLEAN success read or not
 */
BOOLEAN
ReadSpiRom (
    IN      UINTN             Location,
    OUT     UINT8             *Data,
    IN      UINT32            Length
  )
{
  BOOLEAN    Status = FALSE;

  if (IS_SPI_ROM2_OFFSET(Location)) {
    Status = FchSpiRomRead ((UINT32)Location, Data, Length);
  } else if (IS_IN_SPI_ROM2_WINDOW(Location)) {
    CopyMem (Data, (VOID *)Location, Length);
    Status = TRUE;
  }

  return Status;
}

BOOLEAN
IsPspDirAddrRom2Decoded (
  IN      UINT64                      EntryLocation
  )
{
  if (PcdGetBool (PcdPspDirUsing16MAddress) == TRUE) {
      return TRUE;
  } else {
      return FALSE;
  }
}

UINT64
TryToTranslateOffsetToPhysicalAddress (
  IN      UINT64                      EntryLocation
  )
{
  if (IS_SPI_OFFSET (EntryLocation)) {
    if (IsPspDirAddrRom2Decoded (EntryLocation)) {
      EntryLocation = FORCE_SPIADDR_BIT24 (EntryLocation);
    }
  }
  return EntryLocation;
}


/**
 * @brief translate entry offset to correct location based on address mode
 *
 * @param[in] EntryLocation     The location of the entry before translation
 * @param[in] DirectoryHdrAddr  Directory header address
 *
 * @return UINT64           return translated entry location
 */
UINT64
TranslateEntryLocation (
  IN       UINT64                      EntryLocation,
  IN       UINT64                      DirectoryHdrAddr
  )
{
  UINT64 Entry;

  Entry = EntryLocation;

  //address mode 1, relative to BIOS image base
  if (IS_ADDRESS_MODE_1 (Entry)) {
      Entry = Entry & ~BIT62;
  } else if (IS_ADDRESS_MODE_2 (Entry)) { //address mode 2, relative to current directory header
      Entry = Entry & ~BIT63;
      //get the relative offset compare to directory header
      Entry += DirectoryHdrAddr;
  } else if (IS_ADDRESS_MODE_3 (Entry)) { //address mode 3, relative to current image slot
      Entry = Entry & ~(BIT63 | BIT62);
  }

  // translate offset to physical address
  if (IS_SPI_OFFSET (Entry)) {
    if (IsPspDirAddrRom2Decoded (Entry)) {
      Entry = FORCE_SPIADDR_BIT24 (Entry);
    }
  }
  return Entry;
}

UINT64
GetBiosDirectory (
  IN BIOS_DIRECTORY     *BiosDir
  )
{
  FIRMWARE_ENTRY_TABLEV2      FirmwareTableBase;
  UINT32                      BiosDirBase;
  UINT32                      BiosDirEfsOffset;
  UINT32                      i;
  UINT64                      PspLv2Entry;

  BiosDirEfsOffset = OFFSET_OF(FIRMWARE_ENTRY_TABLEV2, NewBiosDirBase);

  if (FchSpiRomRead (0x20000,  (UINT8 *)&FirmwareTableBase, sizeof (FIRMWARE_ENTRY_TABLEV2)) != TRUE) {
    ASSERT (FALSE);
    return 0;
  }
  // Search flash for unique signature 0x55AA55AA
  if (FirmwareTableBase.Signature  != FIRMWARE_TABLE_SIGNATURE) {
    ASSERT (FALSE);
    return 0;
  }

  BiosDirBase = *((UINT32 *) ((UINT8 *)&FirmwareTableBase + BiosDirEfsOffset));

  //Get the BIOS Dir
  if (ReadSpiRom (BiosDirBase, (UINT8*)BiosDir, sizeof(BIOS_DIRECTORY)) != TRUE) {
    return FALSE;
  }

  if (BiosDir->Header.Cookie != BIOS_DIRECTORY_HEADER_SIGNATURE) {
    ASSERT (FALSE);
    return 0;
  }

  for (i = 0; i < BiosDir->Header.TotalEntries; i++) {
    DEBUG ((DEBUG_INFO, "Entry %03d: 0x%x\n", i, BiosDir->BiosEntry[i].TypeAttrib.Type));
  }

  // Find level 2 entry address
  for (i = 0; i < BiosDir->Header.TotalEntries; i++) {
    if (BiosDir->BiosEntry[i].TypeAttrib.Type == BIOS_DIR_LV2) {
      PspLv2Entry = TryToTranslateOffsetToPhysicalAddress (BiosDir->BiosEntry[i].Location);
      break;
    }
  }
  ASSERT (i != BiosDir->Header.TotalEntries);

  DEBUG ((DEBUG_INFO, "PspLv2Entry: %lx\n", PspLv2Entry));

  // Read BIOS LVL2 directory into the HOB, LVL1 data is discarded
  if (ReadSpiRom ((UINT32)PspLv2Entry, (UINT8*)BiosDir, sizeof(BIOS_DIRECTORY)) != TRUE) {
    ASSERT(FALSE);
    return 0;
  }

  return PspLv2Entry;
}

/**
 * @brief   Check for presence of gAmdPspDirHobGuid HOB. If found, assign BiosDirectory. If not found, create a HOB,
 *          read BIOS LVL2 directory into it and assign BiosDirectory variable.
 *
 * @param[in]  BiosDir      Pointer to BIOS directory
 * @param[in]  Signature    Value of the signaure
 *
 * @retval TRUE            The BIOS directory signature is valid.
 * @retval FALSE           The BIOS directory signature is not valid.
 * 
**/
EFI_STATUS
EFIAPI
PspDirectoryLibConstructor (
  VOID
)
{
  EFI_HOB_GUID_TYPE  *Hob;
  BIOS_DIRECTORY     *BiosDirectory;
  UINT64             Lv2Entry;

  Hob = (EFI_HOB_GUID_TYPE *)GetFirstGuidHob(&gAmdPspDirHobGuid);
  if (Hob != NULL) {
    Hob++;
    BiosDirectoryHob = (BIOS_DIRECTORY_HOB *)Hob;
    DEBUG ((DEBUG_INFO, "PSP directory HOB has already been created at: 0x%x\n", BiosDirectoryHob));
    return EFI_SUCCESS;
  }

  BiosDirectoryHob = BuildGuidHob (&gAmdPspDirHobGuid, sizeof(BIOS_DIRECTORY_HOB));

  // Read BIOS directory Level2 data into the HOB and return PSP BIOS directory level2 entry
  BiosDirectory = &BiosDirectoryHob->PspBiosDir;

  DEBUG ((DEBUG_INFO, "PSP directory HOB has been created at: 0x%x, data size 0x%x, BiosDirectory: 0x%x\n",
    BiosDirectoryHob, sizeof(BIOS_DIRECTORY_HOB), BiosDirectory));

  Lv2Entry = GetBiosDirectory (BiosDirectory);
  ASSERT (Lv2Entry != 0);
  if (Lv2Entry == 0) return EFI_NOT_FOUND;

  BiosDirectoryHob->PspLv2Entry = Lv2Entry;

  return EFI_SUCCESS;
}


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
BIOSEntryInfo (
  IN       UINT8                       EntryType,
  IN       UINT8                       EntryInstance,
  IN OUT   TYPE_ATTRIB                 *TypeAttrib,
  IN OUT   UINT64                      *EntryAddress,
  IN OUT   UINT32                      *EntrySize,
  IN OUT   UINT64                      *EntryDestination
  )
{
  BIOS_DIRECTORY          *BiosDir;
  UINT64                  BiosLevel2BaseAddress;
  UINTN                   i;

  BiosDir = &BiosDirectoryHob->PspBiosDir;
  BiosLevel2BaseAddress = BiosDirectoryHob->PspLv2Entry;

  for (i = 0; i < BiosDir->Header.TotalEntries; i++) {
    if (BiosDir->BiosEntry[i].TypeAttrib.Type == EntryType) {
      if (BiosDir->BiosEntry[i].TypeAttrib.Instance == EntryInstance) {
        if (TypeAttrib != NULL) {
          *TypeAttrib = BiosDir->BiosEntry[i].TypeAttrib;
        }
        if (EntrySize != NULL) {
          *EntrySize = BiosDir->BiosEntry[i].Size;
        }
        if (EntryAddress != NULL) {
          *EntryAddress = TranslateEntryLocation (BiosDir->BiosEntry[i].Location, BiosLevel2BaseAddress);
        }
        if (EntryDestination != NULL) {
          *EntryDestination = BiosDir->BiosEntry[i].Destination;
        }
        return TRUE;
      }
    }
  }
  return FALSE;
}

