/******************************************************************************
 * Copyright (C) 2018-2024 Advanced Micro Devices, Inc. All rights reserved.
 *
 *
 ***************************************************************************/

#ifndef SPI_NOR_FLASH_JEDEC_H_
#define SPI_NOR_FLASH_JEDEC_H_

#include <Base.h>

#define SFDP_HEADER_SIGNATURE 0x50444653
#define SFDP_SUPPORTED_MAJOR_REVISION 0x1ul

#pragma pack (1)
typedef struct _SFDP_HEADER {
  UINT32 Signature;
  UINT32 MinorRev : 8;
  UINT32 MajorRev : 8;
  UINT32 NumParameterHeaders : 8;
  UINT32 AccessProtocol : 8;
} SFDP_HEADER;

typedef struct _SFDP_PARAMETER_HEADER {
  UINT32 IdLsb : 8;
  UINT32 MinorRev : 8;
  UINT32 MajorRev : 8;
  UINT32 Length : 8;
  UINT32 TablePointer : 24;
  UINT32 IdMsb : 8;
} SFDP_PARAMETER_HEADER;

typedef struct _SFDB_BASIC_FLASH_PARAMETER {
  // DWORD 1
  UINT32 EraseSizes : 2;
  UINT32 WriteGranularity : 1;
  UINT32 VolatileStatusBlockProtect : 1;
  UINT32 WriteEnableVolatileStatus : 1;
  UINT32 Unused1Dw1 : 3;
  UINT32 FourKEraseInstr : 8;
  UINT32 FastRead112 : 1;
  UINT32 AddressBytes : 2;
  UINT32 DtrClocking : 1;
  UINT32 FastRead122 : 1;
  UINT32 FastRead144 : 1;
  UINT32 FastRead114 : 1;
  UINT32 Unused2Dw1 : 9;
  // DWORD 2
  UINT32 Density;
  // DWORD 3
    // Fast Read 144
  UINT32 FastRead144Dummy : 5;
  UINT32 FastRead144ModeClk : 3;
  UINT32 FastRead144Instr : 8;
    // Fast Read 114
  UINT32 FastRead114Dummy : 5;
  UINT32 FastRead114ModeClk : 3;
  UINT32 FastRead114Instr : 8;
  // DWORD 4
    // Fast Read 112
  UINT32 FastRead112Dummy : 5;
  UINT32 FastRead112ModeClk : 3;
  UINT32 FastRead112Instr : 8;
    // Fast Read 122
  UINT32 FastRead122Dummy : 5;
  UINT32 FastRead122ModeClk : 3;
  UINT32 FastRead122Instr : 8;
  // DWORD 5
  UINT32 FastRead222 : 1;
  UINT32 Unused1Dw5 : 3;
  UINT32 FastRead444 : 1;
  UINT32 Unused2Dw5 : 27;
  // DWORD 6
  UINT32 UnusedDw6 : 16;
    // Fast Read 222
  UINT32 FastRead222Dummy : 5;
  UINT32 FastRead222ModeClk : 3;
  UINT32 FastRead222Instr : 8;
  // DWORD 7
  UINT32 UnusedDw7 : 16;
    // Fast Read 444
  UINT32 FastRead444Dummy : 5;
  UINT32 FastRead444ModeClk : 3;
  UINT32 FastRead444Instr : 8;
  // DWORD 8
  UINT32 Erase1Size : 8;
  UINT32 Erase1Instr : 8;
  UINT32 Erase2Size : 8;
  UINT32 Erase2Instr : 8;
  // DWORD 9
  UINT32 Erase3Size : 8;
  UINT32 Erase3Instr : 8;
  UINT32 Erase4Size : 8;
  UINT32 Erase4Instr : 8;
  // DWORD 10
  UINT32 EraseMultiplier : 4;
  UINT32 Erase1Time : 7;
  UINT32 Erase2Time : 7;
  UINT32 Erase3Time : 7;
  UINT32 Erase4Time : 7;
  // DWORD 11
  UINT32 ProgramMultiplier : 4;
  UINT32 PageSize : 4;
  UINT32 PPTime : 6;
  UINT32 BPFirstTime : 5;
  UINT32 BPAdditionalTime : 5;
  UINT32 ChipEraseTime : 7;
  UINT32 Unused1Dw11 : 1;
  // DWORD 12
  UINT32 ProgSuspendProhibit : 4;
  UINT32 EraseSuspendProhibit : 4;
  UINT32 Unused1Dw13 : 1;
  UINT32 ProgResumeToSuspend : 4;
  UINT32 ProgSuspendInProgressTime : 7;
  UINT32 EraseResumeToSuspend : 4;
  UINT32 EraseSuspendInProgressTime : 7;
  UINT32 SuspendResumeSupported : 1;
  // Don't care about remaining DWORDs
} SFDB_BASIC_FLASH_PARAMETER;
#pragma pack ()

// Number of address bytes opcode can support
#define SPI_ADDR_3BYTE_ONLY               0x00
#define SPI_ADDR_3OR4BYTE                 0x01
#define SPI_ADDR_4BYTE_ONLY               0x02

/**** Commented out commands need parameters verified before uncommenting ****/

// Read/Write Array Commands
#define SPI_FLASH_READ                    0x03
#define   SPI_FLASH_READ_DUMMY              0x00
#define   SPI_FLASH_READ_ADDR_BYTES         SPI_ADDR_3OR4BYTE
#define SPI_FLASH_FAST_READ               0x0B
#define   SPI_FLASH_FAST_READ_DUMMY         0x01
#define   SPI_FLASH_FAST_READ_ADDR_BYTES    SPI_ADDR_3OR4BYTE
//#define SPI_FLASH_DREAD                   0x3B
//#define   SPI_FLASH_DREAD_DUMMY             0x01
//#define   SPI_FLASH_DREAD_ADDR_BYTES        SPI_ADDR_3OR4BYTE
//#define SPI_FLASH_2READ                   0xBB
//#define   SPI_FLASH_2READ_DUMMY             0x01
//#define   SPI_FLASH_2READ_ADDR_BYTES        SPI_ADDR_3OR4BYTE
//#define SPI_FLASH_4READ                   0xEB
//#define   SPI_FLASH_4READ_DUMMY             0x01
//#define   SPI_FLASH_4READ_ADDR_BYTES        SPI_ADDR_3OR4BYTE
//#define SPI_FLASH_W4READ                  0xE7
//#define   SPI_FLASH_W4READ_ADDR_BYTES       SPI_ADDR_3OR4BYTE
//#define SPI_FLASH_QREAD                   0x6B
//#define   SPI_FLASH_QREAD_DUMMY             0x01
//#define   SPI_FLASH_QREAD_ADDR_BYTES        SPI_ADDR_3OR4BYTE
#define SPI_FLASH_PP                      0x02
#define   SPI_FLASH_PP_DUMMY                0x00
#define   SPI_FLASH_PP_ADDR_BYTES           SPI_ADDR_3OR4BYTE
#define   SPI_FLASH_PAGE_SIZE               256
//#define SPI_FLASH_4PP                     0x38
//#define   SPI_FLASH_4PP_DUMMY               0x00
//#define   SPI_FLASH_4PP_ADDR_BYTES          SPI_ADDR_3OR4BYTE
#define SPI_FLASH_SE                      0x20
#define   SPI_FLASH_SE_DUMMY                0x00
#define   SPI_FLASH_SE_ADDR_BYTES           SPI_ADDR_3OR4BYTE
#define SPI_FLASH_BE32K                   0x52
#define   SPI_FLASH_BE32K_DUMMY             0x00
#define   SPI_FLASH_BE32K_ADDR_BYTES        SPI_ADDR_3OR4BYTE
#define SPI_FLASH_BE                      0xD8
#define   SPI_FLASH_BE_DUMMY                0x00
#define   SPI_FLASH_BE_ADDR_BYTES           SPI_ADDR_3OR4BYTE
#define SPI_FLASH_CE                      0x60
#define   SPI_FLASH_CE_DUMMY                0x00
#define   SPI_FLASH_CE_ADDR_BYTES           SPI_ADDR_3OR4BYTE
#define SPI_FLASH_RDID                    0x9F
#define   SPI_FLASH_RDID_DUMMY              0x00
#define   SPI_FLASH_RDID_ADDR_BYTES         SPI_ADDR_3OR4BYTE
//Register Setting Commands
#define SPI_FLASH_WREN                    0x06
#define   SPI_FLASH_WREN_DUMMY              0x00
#define   SPI_FLASH_WREN_ADDR_BYTES         SPI_ADDR_3OR4BYTE
#define SPI_FLASH_WRDI                    0x04
#define   SPI_FLASH_WRDI_DUMMY              0x00
#define   SPI_FLASH_WRDI_ADDR_BYTES         SPI_ADDR_3OR4BYTE
#define SPI_FLASH_RDSR                    0x05
#define   SPI_FLASH_RDSR_DUMMY              0x00
#define   SPI_FLASH_RDSR_ADDR_BYTES         SPI_ADDR_3OR4BYTE
#define   SPI_FLASH_SR_NOT_WIP              0x0
#define   SPI_FLASH_SR_WIP                  BIT0
#define   SPI_FLASH_SR_WEL                  BIT1
#define SPI_FLASH_WRSR                    0x01
#define   SPI_FLASH_WRSR_DUMMY              0x00
#define   SPI_FLASH_WRSR_ADDR_BYTES         SPI_ADDR_3OR4BYTE
//#define SPI_FLASH_WPSEL                   0x68
//#define   SPI_FLASH_WPSEL_DUMMY             0x00
//#define   SPI_FLASH_WPSEL_ADDR_BYTES        SPI_ADDR_3OR4BYTE
//#define SPI_FLASH_EQIO                    0x35
//#define   SPI_FLASH_EQIO_DUMMY              0x00
//#define   SPI_FLASH_EQIO_ADDR_BYTES         SPI_ADDR_3OR4BYTE
//#define SPI_FLASH_RSTQIO                  0xF5
//#define   SPI_FLASH_RSTQIO_DUMMY            0x00
//#define   SPI_FLASH_RSTQIO_ADDR_BYTES       SPI_ADDR_3OR4BYTE
//#define SPI_FLASH_SUSPEND                 0xB0
//#define   SPI_FLASH_SUSPEND_DUMMY           0x00
//#define   SPI_FLASH_SUSPEND_ADDR_BYTES      SPI_ADDR_3OR4BYTE
//#define SPI_FLASH_RESUME                  0x30
//#define   SPI_FLASH_RESUME_DUMMY            0x00
//#define   SPI_FLASH_RESUME_ADDR_BYTES       SPI_ADDR_3OR4BYTE
//#define SPI_FLASH_DP                      0xB9
//#define   SPI_FLASH_DP_DUMMY                0x00
//#define   SPI_FLASH_DP_ADDR_BYTES           SPI_ADDR_3OR4BYTE
//#define SPI_FLASH_RDP                     0xAB
//#define   SPI_FLASH_RDP_DUMMY               0x00
//#define   SPI_FLASH_RDP_ADDR_BYTES          SPI_ADDR_3OR4BYTE
//#define SPI_FLASH_SBL                     0xC0
//#define   SPI_FLASH_SBL_DUMMY               0x00
//#define   SPI_FLASH_SBL_ADDR_BYTES          SPI_ADDR_3OR4BYTE
// ID/Security Commands
//#define SPI_FLASH_RES                     0xAB
//#define   SPI_FLASH_RES_DUMMY               0x00
//#define   SPI_FLASH_RES_ADDR_BYTES          SPI_ADDR_3OR4BYTE
//#define SPI_FLASH_REMS                    0x90
//#define   SPI_FLASH_REMS_DUMMY              0x00
//#define   SPI_FLASH_REMS_ADDR_BYTES         SPI_ADDR_3OR4BYTE
//#define SPI_FLASH_OPIID                   0xAF
//#define   SPI_FLASH_OPIID_DUMMY             0x00
//#define   SPI_FLASH_OPIID_ADDR_BYTES        SPI_ADDR_3OR4BYTE
#define SPI_FLASH_RDSFDP                  0x5A
#define   SPI_FLASH_RDSFDP_DUMMY            0x01
#define   SPI_FLASH_RDSFDP_ADDR_BYTES       SPI_ADDR_3BYTE_ONLY
//#define SPI_FLASH_ENSO                    0xB1
//#define   SPI_FLASH_ENSO_DUMMY              0x00
//#define   SPI_FLASH_ENSO_ADDR_BYTES         SPI_ADDR_3OR4BYTE
//#define SPI_FLASH_EXSO                    0xC1
//#define   SPI_FLASH_EXSO_DUMMY              0x00
//#define   SPI_FLASH_EXSO_ADDR_BYTES         SPI_ADDR_3OR4BYTE
//#define SPI_FLASH_RDSCUR                  0x2B
//#define   SPI_FLASH_RDSCUR_DUMMY            0x00
//#define   SPI_FLASH_RDSCUR_ADDR_BYTES       SPI_ADDR_3OR4BYTE
//#define SPI_FLASH_WRSCUR                  0x2F
//#define   SPI_FLASH_WRSCUR_DUMMY            0x00
//#define   SPI_FLASH_WRSCUR_ADDR_BYTES       SPI_ADDR_3OR4BYTE
//#define SPI_FLASH_SBLK                    0x36
//#define   SPI_FLASH_SBLK_DUMMY              0x00
//#define   SPI_FLASH_SBLK_ADDR_BYTES         SPI_ADDR_3OR4BYTE
//#define SPI_FLASH_SBULK                   0x39
//#define   SPI_FLASH_SBULK_DUMMY             0x00
//#define   SPI_FLASH_SBULK_ADDR_BYTES        SPI_ADDR_3OR4BYTE
//#define SPI_FLASH_RDBLOCK                 0x3C
//#define   SPI_FLASH_RDBLOCK_DUMMY           0x00
//#define   SPI_FLASH_RDBLOCK_ADDR_BYTES      SPI_ADDR_3OR4BYTE
//#define SPI_FLASH_GBLK                    0x7E
//#define   SPI_FLASH_GBLK_DUMMY              0x00
//#define   SPI_FLASH_GBLK_ADDR_BYTES         SPI_ADDR_3OR4BYTE
//#define SPI_FLASH_GBULK                   0x98
//#define   SPI_FLASH_GBULK_DUMMY             0x00
//#define   SPI_FLASH_GBULK_ADDR_BYTES        SPI_ADDR_3OR4BYTE
// Reset Commands
//#define SPI_FLASH_NOP                     0x00
//#define   SPI_FLASH_NOP_DUMMY               0x00
//#define   SPI_FLASH_NOP_ADDR_BYTES          SPI_ADDR_3OR4BYTE
//#define SPI_FLASH_RSTEN                   0x66
//#define   SPI_FLASH_RSTEN_DUMMY             0x00
//#define   SPI_FLASH_RSTEN_ADDR_BYTES        SPI_ADDR_3OR4BYTE
//#define SPI_FLASH_RST                     0x99
//#define   SPI_FLASH_RST_DUMMY               0x00
//#define   SPI_FLASH_RST_ADDR_BYTES          SPI_ADDR_3OR4BYTE
//#define SPI_FLASH_REL_RD_ENH              0xFF
//#define   SPI_FLASH_REL_RD_ENH_DUMMY        0x00
//#define   SPI_FLASH_REL_RD_ENH_ADDR_BYTES   SPI_ADDR_3OR4BYTE

#endif // SPI_NOR_FLASH_JEDEC_H_
