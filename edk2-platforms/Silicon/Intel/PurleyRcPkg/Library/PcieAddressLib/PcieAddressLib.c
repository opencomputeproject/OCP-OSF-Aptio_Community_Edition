/** @file

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/PcieAddress.h>
#include <Library/BaseMemoryLib/MemLibInternals.h>

#pragma optimize ("",off)
//////////////////////////////////////////////////////////////////////////
//
// Pcie Address Library
// This Lib provide the way use platform Library instance
//
//////////////////////////////////////////////////////////////////////////

PCIE_MMCFG_TABLE_TYPE mMmcfgTable =\
  {
    {
      {'M', 'C', 'F', 'G'},   // Signature
      0x00000090,             // Length
      0x01,                   // Revision
      0x08,                   // The Maximum number of Segments
      0x00FF,                 // Valid Segment Bit Map, LSB Bit0 for Seg0, bit1 for seg1 ...
      {0x00,0x00,0x00,0x00}   // Reserved
    },
    {
      0x00000000,             // Base Address Low
      0x00000000,             // Base Address High
      0x0000,                 // Segment 0 
      0x00,                   // Start Bus
      0xFF,                   // End Bus
      {0x00,0x00,0x00,0x00}   // Reserved
    }
};
//
// Segment 1 ~ 7
//
PCIE_MMCFG_BASE_ADDRESS_TYPE mMmcfgAddr[] = \
{
  {
    0x00000000,             // Base Address Low
    0x00000000,             // Base Address High
    0x0001,                 // Segment 1
    0x00,                   // Start Bus
    0xFF,                   // End Bus
    {0x00,0x00,0x00,0x00}   // Reserved
  },
  {
    0x00000000,             // Base Address Low
    0x00000000,             // Base Address High
    0x0002,                 // Segment 2
    0x00,                   // Start Bus
    0xFF,                   // End Bus
    {0x00,0x00,0x00,0x00}   // Reserved
  },
  {
    0x00000000,             // Base Address Low
    0x00000000,             // Base Address High
    0x0003,                 // Segment 3
    0x00,                   // Start Bus
    0xFF,                   // End Bus
    {0x00,0x00,0x00,0x00}   // Reserved
  },

  {
    0x00000000,             // Base Address Low
    0x00000000,             // Base Address High
    0x0004,                 // Segment 4
    0x00,                   // Start Bus
    0xFF,                   // End Bus
    {0x00,0x00,0x00,0x00}   // Reserved
  },
  {
    0x00000000,             // Base Address Low
    0x00000000,             // Base Address High
    0x0005,                 // Segment 5
    0x00,                   // Start Bus
    0xFF,                   // End Bus
    {0x00,0x00,0x00,0x00}   // Reserved
  },

  {
    0x00000000,             // Base Address Low
    0x00000000,             // Base Address High
    0x0006,                 // Segment 6
    0x00,                   // Start Bus
    0xFF,                   // End Bus
    {0x00,0x00,0x00,0x00}   // Reserved
  },
  {
    0x00000000,             // Base Address Low
    0x00000000,             // Base Address High
    0x0007,                 // Segment 7
    0x00,                   // Start Bus
    0xFF,                   // End Bus
    {0x00,0x00,0x00,0x00}   // Reserved
  }
};

/**
  This Lib is used for platform to set platform specific Pcie MMCFG Table
  
  @param[in] MmcfgTable           A pointer of the MMCFG Table structure for PCIE_MMCFG_TABLE_TYPE type
  @param[in] NumOfSeg             Number of Segments in the table

  @retval NULL                    The function completed successfully.
  @retval <>NULL                  Return Error
**/
UINTN
EFIAPI
SetPcieSegMmcfgTable (
  IN PCIE_MMCFG_TABLE_TYPE *MmcfgTable,
  IN UINT32                 NumOfSeg
  )
{
  UINT32                  MmcfgTableSize;
  PCIE_MMCFG_TABLE_TYPE   *HobMmcfgTable;

  union {
    UINTN   D64;
    UINT32  D32[2];
  } Data;

  Data.D32[0] = Data.D32[1] = 0;
  MmcfgTableSize = sizeof(PCIE_MMCFG_HEADER_TYPE) + (NumOfSeg * sizeof(PCIE_MMCFG_BASE_ADDRESS_TYPE));

  HobMmcfgTable = (PCIE_MMCFG_TABLE_TYPE *) PcdGetPtr (PcdPcieMmcfgTablePtr); 
  ASSERT (MmcfgTableSize < PcdGetSize (PcdPcieMmcfgTablePtr));

  InternalMemCopyMem(HobMmcfgTable, MmcfgTable, PcdGetSize (PcdPcieMmcfgTablePtr));
  MmcfgTable->Header.Length = MmcfgTableSize;
  if((MmcfgTable->MmcfgBase[0].BaseAddressL == 0) && (MmcfgTable->MmcfgBase[0].BaseAddressH == 0))
  {
    //
    // The first time default should be the PcdPciExpressBaseAddress
    //
    Data.D64 = (UINTN) PcdGet64 (PcdPciExpressBaseAddress);
    HobMmcfgTable->MmcfgBase[0].BaseAddressL = Data.D32[0];
    HobMmcfgTable->MmcfgBase[0].BaseAddressH = Data.D32[1];
  };
  return 0;
};

/**
  This Lib return PCIE MMCFG Base Address 
  
  @param[in] Address              A pointer of the address of the USRA Address Structure for PCIE type

  @retval NULL                    The function completed successfully.
  @retval <>NULL                  Return Error
**/
UINTN
EFIAPI
GetPcieSegMmcfgBaseAddress (
  IN USRA_ADDRESS         *Address
  )
{
  PCIE_MMCFG_TABLE_TYPE *MmcfgTable=NULL;
  UINTN SegMmcfgBase;

  if(Address->Attribute.HostPtr == 0)
  {
    MmcfgTable = (PCIE_MMCFG_TABLE_TYPE *) PcdGetPtr (PcdPcieMmcfgTablePtr);
    if(MmcfgTable->Header.Length == 0)
    {
      //
      // if it is not valid MMCFG pointer, initialize it to use the predefined default MMCFG Table
      //
      SetPcieSegMmcfgTable(&mMmcfgTable, PcdGet32 (PcdNumOfPcieSeg));
    }
  }
  else
  {
    ((UINT32*)&MmcfgTable)[0] = Address->Attribute.HostPtr;
  }
  ASSERT(Address->Pcie.Seg < MmcfgTable->Header.SegMax);
  ASSERT( (1<<Address->Pcie.Seg) &  MmcfgTable->Header.ValidSegMap);
  return SegMmcfgBase = *((UINTN*)(&MmcfgTable->MmcfgBase[Address->Pcie.Seg].BaseAddressL));
};

