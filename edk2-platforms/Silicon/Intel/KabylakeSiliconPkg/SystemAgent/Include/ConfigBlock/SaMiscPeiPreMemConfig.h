/** @file
  Policy details for miscellaneous configuration in System Agent

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _SA_MISC_PEI_PREMEM_CONFIG_H_
#define _SA_MISC_PEI_PREMEM_CONFIG_H_

#pragma pack(push, 1)

#ifndef SA_MC_MAX_SOCKETS
#define SA_MC_MAX_SOCKETS 4
#endif

#define SA_MISC_PEI_PREMEM_CONFIG_REVISION 1

/**
  This configuration block is to configure SA Miscellaneous variables during PEI Pre-Mem phase like programming
  different System Agent BARs, TsegSize, IedSize, MmioSize required etc.
  <b>Revision 1</b>:
  - Initial version.
**/
typedef struct {
  CONFIG_BLOCK_HEADER  Header;               ///< Offset 0-27 Config Block Header
  UINT8   SpdAddressTable[SA_MC_MAX_SOCKETS];///< Offset 28 Memory DIMMs' SPD address for reading SPD data. <b>example: SpdAddressTable[0]=0xA2(C0D0), SpdAddressTable[1]=0xA0(C0D1), SpdAddressTable[2]=0xA2(C1D0), SpdAddressTable[3]=0xA0(C1D1)</b>
  UINT32  MchBar;                            ///< Offset 36 Address of System Agent MCHBAR: <b>0xFED10000</b>
} SA_MISC_PEI_PREMEM_CONFIG;
#pragma pack(pop)

#endif // _SA_MISC_PEI_PREMEM_CONFIG_H_
