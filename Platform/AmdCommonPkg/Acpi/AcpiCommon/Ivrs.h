/*****************************************************************************
 * Copyright (C) 2021-2024 Advanced Micro Devices, Inc. All rights reserved.
 *
*****************************************************************************
*/
/**
 * @file
 *
 * IVRS definitions from the AMD IOMMU Specification
 *
 */
#ifndef _IVRS_H_
#define _IVRS_H_

typedef struct {
  UINT16   DeviceId;
  UINT8    DataSetting;
  UINT8    HardwareId[9];
  UINT8    CompatibleId[9];
  UINT8    UidFormat;
  UINT8    UidLength;
  UINT8    Uid[20];
} IVRS_DEVICE_LIST;

typedef struct {
  UINT8    SocketId;
  UINT8    RBIndex;
  IVRS_DEVICE_LIST  IvrsEntry;
} MPDMA_IVRS_MAP;

#define  IVRS_DEVICE_UIDSTR(mDeviceId, mmDataSetting, mHardwareId, mCompatibleId, mUidLength, mUid) \
                    {mDeviceId, mmDataSetting, {mHardwareId}, {mCompatibleId}, 2, mUidLength, {mUid}}

#define  IVRS_DEVICE_UIDINT(mDeviceId, mmDataSetting, mHardwareId, mCompatibleId, mUid) \
                    {mDeviceId, mmDataSetting, {mHardwareId}, {mCompatibleId}, 1, 2, {mUid & 0xff, mUid >> 8, 0}}

#pragma pack (push, 1)

#define IOMMU_CAP_ID                                    0x0F
#define IVRS_TABLE_LENGTH                               8 * 1024

#define PCICFG_AMD_IOMMU_CAP_BASE_HI_OFFSET              0x48UL
#define PCICFG_AMD_IOMMU_CAP_BASE_LO_OFFSET              0x44UL
#define PCICFG_AMD_IOMMU_FUNCTION                        0x2

/// IVRS block
typedef enum {
  IvrsIvhdBlock10h            = 0x10,       ///< I/O Virtualization Hardware Definition Block
  IvrsIvhdBlock11h            = 0x11,       ///< I/O Virtualization Hardware Definition Block
  IvrsIvmdBlock               = 0x20,       ///< I/O Virtualization Memory Definition Block for all peripherals
  IvrsIvmdBlockSingle         = 0x21,       ///< IVMD block for specified peripheral
  IvrsIvmdBlockRange          = 0x22,       ///< IVMD block for peripheral range
  IvrsIvhdrBlock40h           = 0x40,       ///< IVHDR (Relative) block
  IvrsIvhdrBlock41h           = 0x41,       ///< IVHDR (Relative) block
  IvrsIvmdrBlock              = 0x50,       ///< IVMDR (Relative) block for all peripherals
  IvrsIvmdrBlockSingle        = 0x51        ///< IVMDR block for last IVHDR
} IVRS_BLOCK_TYPE;

#define DEVICE_ID(PciAddress) (UINT16) (((PciAddress).Address.Bus << 8) | ((PciAddress).Address.Device << 3) | (PciAddress).Address.Function)

/// IVHD entry types
typedef enum {
  IvhdEntryPadding            =  0,         ///< Table padding
  IvhdEntrySelect             =  2,         ///< Select
  IvhdEntryStartRange         =  3,         ///< Start Range
  IvhdEntryEndRange           =  4,         ///< End Range
  IvhdEntryAliasSelect        =  66,        ///< Alias select
  IvhdEntryAliasStartRange    =  67,        ///< Alias start range
  IvhdEntryExtendedSelect     =  70,        ///< Extended select
  IvhdEntryExtendedStartRange =  71,        ///< Extended Start range
  IvhdEntrySpecialDevice      =  72,        ///< Special device
  IvhdEntryF0Type             =  240        /// Type 40h Fields
} IVHD_ENTRY_TYPE;

/// Special device variety
typedef enum {
  IvhdSpecialDeviceIoapic     = 0x1,        ///< IOAPIC
  IvhdSpecialDeviceHpet       = 0x2         ///< HPET
} IVHD_SPECIAL_DEVICE;


#define IVHD_FLAG_PPRSUB            BIT7
#define IVHD_FLAG_PREFSUP           BIT6
#define IVHD_FLAG_COHERENT          BIT5
#define IVHD_FLAG_IOTLBSUP          BIT4
#define IVHD_FLAG_ISOC              BIT3
#define IVHD_FLAG_RESPASSPW         BIT2
#define IVHD_FLAG_PASSPW            BIT1
#define IVHD_FLAG_HTTUNEN           BIT0

#define IVHD_EFR_XTSUP_OFFSET       0
#define IVHD_EFR_NXSUP_OFFSET       1
#define IVHD_EFR_GTSUP_OFFSET       2
#define IVHD_EFR_GLXSUP_OFFSET      3
#define IVHD_EFR_IASUP_OFFSET       5
#define IVHD_EFR_GASUP_OFFSET       6
#define IVHD_EFR_HESUP_OFFSET       7
#define IVHD_EFR_PASMAX_OFFSET      8
#define IVHD_EFR_PNCOUNTERS_OFFSET  13
#define IVHD_EFR_PNBANKS_OFFSET     17
#define IVHD_EFR_MSINUMPPR_OFFSET   23
#define IVHD_EFR_GATS_OFFSET        28
#define IVHD_EFR_HATS_OFFSET        30

#define IVINFO_HTATSRESV_MASK       0x00400000ul
#define IVINFO_VASIZE_MASK          0x003F8000ul
#define IVINFO_PASIZE_MASK          0x00007F00ul
#define IVINFO_GASIZE_MASK          0x000000E0ul
#define IVINFO_EFRSUP_MASK          0x00000001ul
#define IVINFO_DMAREMAP_MASK        0x00000002ul

#define IVHD_INFO_MSINUM_OFFSET     0
#define IVHD_INFO_UNITID_OFFSET     8

#define IVMD_FLAG_EXCLUSION_RANGE   BIT3
#define IVMD_FLAG_IW                BIT2
#define IVMD_FLAG_IR                BIT1
#define IVMD_FLAG_UNITY             BIT0

/// IVRS header
typedef struct {
  UINT8   Sign[4];           ///< Signature
  UINT32  TableLength;       ///< Table Length
  UINT8   Revision;          ///< Revision
  UINT8   Checksum;          ///< Checksum
  UINT8   OemId[6];          ///< OEM ID
  UINT8   OemTableId[8];     ///< OEM Tabled ID
  UINT32  OemRev;            ///< OEM Revision
  UINT8   CreatorId[4];      ///< Creator ID
  UINT32  CreatorRev;        ///< Creator Revision
  UINT32  IvInfo;            ///< IvInfo
  UINT64  Reserved;          ///< Reserved
} IOMMU_IVRS_HEADER;

/// IVRS IVHD Entry
typedef struct {
  UINT8   Type;               ///< Type
  UINT8   Flags;              ///< Flags
  UINT16  Length;             ///< Length
  UINT16  DeviceId;           ///< DeviceId
  UINT16  CapabilityOffset;   ///< CapabilityOffset
  UINT64  BaseAddress;        ///< BaseAddress
  UINT16  PciSegment;         ///< Pci segment
  UINT16  IommuInfo;          ///< IOMMU info
} IVRS_IVHD_ENTRY;

/// IVRS IVHD Entry
typedef struct {
  IVRS_IVHD_ENTRY Ivhd;       ///< Ivhd
  UINT32          IommuEfr;   ///< Extended Features Register
} IVRS_IVHD_ENTRY_10H;

/// IVRS IVHD Entry
typedef struct {
  IVRS_IVHD_ENTRY Ivhd;               ///< Ivhd
  UINT32          IommuAttributes;    ///< Attributes
  UINT64          IommuEfr;           ///< Extended Features Register
  UINT64          IommuEfr2;          ///< Extended Features Register 2
} IVRS_IVHD_ENTRY_11H;

/// IVRS IVHD Entry
typedef struct {
  IVRS_IVHD_ENTRY Ivhd;               ///< Ivhd
  UINT32          IommuAttributes;    ///< Attributes
  UINT64          IommuEfr;           ///< Extended Features Register
  UINT64          IommuEfr2;          ///< Extended Features Register 2
} IVRS_IVHD_ENTRY_40H;

/// IVHD generic entry
typedef struct {
  UINT8   Type;               ///< Type
  UINT16  DeviceId;           ///< Device id
  UINT8   DataSetting;        ///< Data settings
} IVHD_GENERIC_ENTRY;

///IVHD alias entry
typedef struct {
  UINT8   Type;               ///< Type
  UINT16  DeviceId;           ///< Device id
  UINT8   DataSetting;        ///< Data settings
  UINT8   Reserved;           ///< Reserved
  UINT16  AliasDeviceId;      ///< Alias device id
  UINT8   Reserved2;          ///< Reserved
} IVHD_ALIAS_ENTRY;

///IVHD extended entry
typedef struct {
  UINT8   Type;               ///< Type
  UINT16  DeviceId;           ///< Device id
  UINT8   DataSetting;        ///< Data settings
  UINT32  ExtSetting;         ///< Extended settings
} IVHD_EXT_ENTRY;

/// IVHD special entry
typedef struct {
  UINT8   Type;               ///< Type
  UINT16  Reserved;           ///< Reserved
  UINT8   DataSetting;        ///< Data settings
  UINT8   Handle;             ///< Handle
  UINT16  AliasDeviceId;      ///< Alis device id
  UINT8   Variety;            ///< Variety
} IVHD_SPECIAL_ENTRY;

/// IVHD special entry
typedef struct {
  UINT8   IdByte0;            ///< ID Byte0
  UINT8   IdByte1;            ///< ID Byte1
  UINT8   IdByte2;            ///< ID Byte2
  UINT8   IdByte3;            ///< ID Byte3
  UINT8   IdByte4;            ///< ID Byte4
  UINT8   IdByte5;            ///< ID Byte5
  UINT8   IdByte6;            ///< ID Byte6
  UINT8   IdByte7;            ///< ID Byte7
} IVHD_ID;

/// IVHD special entry
typedef struct {
  UINT8   Type;               ///< Type
  UINT16  DeviceId;           ///< DeviceID
  UINT8   DataSetting;        ///< Data settings
  IVHD_ID HardwareId;         ///< ACPI Hardware ID
  IVHD_ID CompatibleId;       ///< ACPI Compatible ID
  UINT8   UidFormat;          ///< Unique ID Format
  UINT8   UidLength;          ///< Unique ID Length
} IVHD_TYPEF0_ENTRY;

/// IVRS IVMD Entry
typedef struct {
  UINT8   Type;               ///< Type
  UINT8   Flags;              ///< Flags
  UINT16  Length;             ///< Length
  UINT16  DeviceId;           ///< DeviceId
  UINT16  AuxiliaryData;      ///< Auxiliary data
  UINT64  Reserved;           ///< Reserved (0000_0000_0000_0000)
  UINT64  BlockStart;         ///< IVMD start address
  UINT64  BlockLength;        ///< IVMD memory block length
} IVRS_IVMD_ENTRY;

/// MMIO Extended Feature Register (Offset 0x30)
#define MMIO_0x30_OFFET 0x30
typedef union {
  struct {                                           ///<
    UINT64                PreFSup:1;                 ///<
    UINT64                PPRSup:1;                  ///<
    UINT64                XTSup:1;                   ///<
    UINT64                NXSup:1;                   ///<
    UINT64                GTSup:1;                   ///<
    UINT64                EFRignored:1;              ///<
    UINT64                IASup:1;                   ///<
    UINT64                GASup:1;                   ///<
    UINT64                HESup:1;                   ///<
    UINT64                PCSup:1;                   ///<
    UINT64                HATS:2;                    ///<
    UINT64                GATS:2;                    ///<
    UINT64                GLXSup:2;                  ///<
    UINT64                SmiFSup:2;                 ///<
    UINT64                SmiFRC:3;                  ///<
    UINT64                GAMSup:3;                  ///<
    UINT64                Reserved_31_24:8;          ///<
    UINT64                PASmax:5;                  ///<
    UINT64                Reserved_63_37:27;         ///<
  } Field;

  UINT64 Value;
} MMIO_0x30;

/// MMIO Offset 0x18
typedef union {
  struct {                                           ///<
    UINT64                IommuEn:1;                 ///<
    UINT64                HtTunEn:1;                 ///<
    UINT64                Field_7_2:6;               ///<
    UINT64                PassPW:1;                  ///<
    UINT64                ResPassPW:1;               ///<
    UINT64                Coherent:1;                ///<
    UINT64                Isoc:1;                    ///<
    UINT64                Field_63_12:52;            ///<
  } Field;

  UINT64 Value;
} MMIO_0x18;

/// MMIO Offset 0x4000
typedef union {
  struct {                                           ///<
    UINT64               Reserved_6_0:7;             ///<
    UINT64               NCounter:4;                 ///<
    UINT64               Reserved_11:1;              ///<
    UINT64               NCounterBanks:6;            ///<
    UINT64               Reserved_63_18:46;          ///<
  } Field;

  UINT64 Value;
} MMIO_0x4000;

/// MMIO Offset 0x1A0
typedef union {
  struct {                                           ///<
    UINT64              CXLIO_SUP:1;                 ///<
    UINT64              Reserved_1_1:1;              ///<
    UINT64              PageMigrationSup:1;          ///<
    UINT64              GCR3TRPModeSup:1;            ///<
    UINT64              GAPPIDisSup:1;               ///<
    UINT64              SNPAVICSup:3;                ///<
    UINT64              Reserved_63_8:24;            ///<
  } Field;

  UINT64 Value;
} MMIO_0x1A0;



/// Capability offset 0
typedef union {
  struct {                                           ///<
    UINT32                IommuCapId:8;              ///<
    UINT32                IommuCapPtr:8;             ///<
    UINT32                IommuCapType:3;            ///<
    UINT32                IommuCapRev:5;             ///<
    UINT32                IommuIoTlbsup:1;           ///<
    UINT32                IommuHtTunnelSup:1;        ///<
    UINT32                IommuNpCache:1;            ///<
    UINT32                IommuEfrSup:1;             ///<
    UINT32                Reserved_31_28:4;          ///<
  } Field;

  UINT32 Value;
} CAPABILITY_REG;

/**
 * These structures are described in the IOMMU spec. The fields have a brief description.
 * Consult the spec for more information
 */
#define IOMMU_MAX_DEVICE_TABLE_ENTRIES        512

typedef union {
  struct {
    UINT64    V:1;                        ///< Device table entry bits [127:1] validity
    UINT64    Tv:1;                       ///< Page translation information validity
    UINT64    Reserved1:5;                ///< Not used with AMD64
    UINT64    Had:2;                      ///< Host Access Dirty Bit
    UINT64    Mode:3;                     ///< Paging mode. Specifies 1-6 levels
    UINT64    HostPageTablePtr:40;        ///< The page table root pointer
    UINT64    Ppr:1;                      ///< PPR enable
    UINT64    Gprp:1;                     ///< Guest PPR response with PASID
    UINT64    Glov:1;                     ///< Guest I/O protection valid
    UINT64    Gv:1;                       ///< Guest translation valid
    UINT64    Glx:2;                      ///< Guest levels translated
    UINT64    Gcr3:3;                     ///< Guest CR3 table root pointer
    UINT64    Ir:1;                       ///< I/O read permission
    UINT64    Iw:1;                       ///< I/O write permission
    UINT64    Reserved2:1;                ///< Reserved
    UINT64    DomainId:16;                ///< Each IO device that shares page tables has the same domain ID
    UINT64    Gcr3TableRootPtrLo:16;      ///< When guest translations are supported, this field contains the SPA
    UINT64    I:1;                        ///< IOTLB enable
    UINT64    Se:1;                       ///< Suppress I/O page fault events
    UINT64    Sa:1;                       ///< Suppress all I/O page fault events
    UINT64    IoCtl:2;                    ///< Port I/O control. Specifies blocked, forwarded or translated
    UINT64    Cache:1;                    ///< IOTLB cache hint
    UINT64    Sd:1;                       ///< Snoop Disable
    UINT64    Ex:1;                       ///< Allow Exclusion
    UINT64    SysMgt:2;                   ///< System management message enable
    UINT64    Reserved3:1;                ///< Reserved
    UINT64    Gcr3TableRootPointerHi:21;  ///< When guest translations are supported, this field contains the SPA of the guest
    UINT64    Iv:1;                       ///< Interrupt map valid
    UINT64    IntTblLen:4;                ///< Interrupt table length
    UINT64    Ig:1;                       ///< Ignore unmapped interrupts
    UINT64    IntTableRootPtr:46;         ///< Only used when interrupt translation is enabled
    UINT64    Reserved4:4;                ///< Reserved
    UINT64    InitPass:1;                 ///< INIT pass-through
    UINT64    EinitPass:1;                ///< ExtInt pass-through
    UINT64    NmiPass:1;                  ///< NMI pass-through
    UINT64    Reserved5:1;                ///< Reserved
    UINT64    IntCtl:2;                   ///< Interrupt control
    UINT64    Linit0Pass:1;               ///< LINT0 (legacy PIC ExtInt) pass-through
    UINT64    Linit1Pass:1;               ///< LINT1 (legacy PIC NMI) pass-through.
    UINT64    VmTag:12;
    UINT64    Reserved6:2;
    UINT64    Vmgtw:1;
    UINT64    VimmuEn:1;
    UINT64    GDeviceId:16;
    UINT64    Reserved7:32;
  } Field;
  struct {
    UINT32   Dword_0_31;
    UINT32   Dword_32_63;
    UINT32   Dword_64_95;
    UINT32   Dword_96_127;
    UINT32   Dword_128_159;
    UINT32   Dword_160_191;
    UINT32   Dword_192_223;
    UINT32   Dword_224_255;
  } Dwords;
} IOMMU_DEVICE_TABLE;


/**
  Used to index the device table
**/
typedef union {
  struct {
    UINT8    Function:3;
    UINT8    Device:5;
    UINT8    Bus;
  } Bits;
  UINT16     IndexNumber;
} IOMMU_DEVICE_TABLE_INDEX;

/**
* Page table related structures
**/

typedef union {
  struct {
    UINT64    Pr:1;           ///< Present. Zero nullifies all other fields
    UINT64    Ignored1:4;     ///< Ignored
    UINT64    A:1;            ///< Accessed
    UINT64    D:1;            ///< Dirty
    UINT64    Ignored2:2;     ///< Ignored
    UINT64    NextLevel:3;    ///< Next page translation level
    UINT64    PageAddress:40; ///< Specifies the SPA of the page
    UINT64    Reserved:7;     ///< Reserved
    UINT64    U:1;            ///< Attribute bit passed to peripheral in ATS response to GPA->SPA
    UINT64    Fc:1;           ///< Force Coherent
    UINT64    Ir:1;           ///< Read Permission
    UINT64    Iw:1;           ///< Write Permission
    UINT64    Ignored3:1;     ///< Ignored
  } Field;
  UINT64 Value;
} IOMMU_PAGE_TRANSLATION_ENTRY;


typedef union {
  struct {
    UINT64    Pr:1;            ///< Present, Zero nullifies all other fields in table
    UINT64    Ignored1:4;      ///< Not used
    UINT64    A:1;             ///< Accessed
    UINT64    Ignored2:3;      ///< Not used
    UINT64    NextLevel:3;     ///< Specifies level of page translation used in this section
    UINT64    NextAddress:40;  ///< Specifies SPA of next page when Level = 000b or 111b
    UINT64    Reserved:9;      ///< Reserved
    UINT64    Ir:1;            ///< Read Permission
    UINT64    Iw:1;            ///< Write Permission
    UINT64    Ignored3:1;      ///< Ignored
  } Field;
  UINT64 Value;
} IOMMU_PAGE_DIRECTORY_ENTRY;

typedef union {
  struct {
    UINT64    Pr:1;           ///< Present. Zero nullifies all other fields
    UINT64    Reserved:63;
  } Field;
  UINT64 Value;
} IOMMU_PDPT_ENTRY;

#define  IOMMU_MMIO_DEVTBL_BASE_STRUCT_OFFSET  0
typedef union {
  struct {
    UINT64          DEV_TBL_SIZE:9;    ///<
    UINT64          Reserved_11_9:3;   ///<
    UINT64          DEV_TBL_BASE:40;   ///< DevTableBase[51:12]
    UINT64          Reserved_31_20:12; ///<
  } Field;
  struct {
    UINT32          DwordLo;
    UINT32          DwordHi;
  } Dword;
} IOMMU_MMIO_DEVTBL_BASE_STRUCT;

#define IOMMU_MMIO_CMD_BASE_STRUCT_OFFSET  0x8
typedef union {
  struct {
    UINT64           Reserved_11_0:12;  ///<
    UINT64           COM_BASE:40;       ///<
    UINT64           Reserved_23_20:4 ; ///<
    UINT64           COM_LEN:4 ;        ///<
    UINT64           Reserved_31_28:4 ; ///<
  } Field;
  struct {
    UINT32          DwordLo;
    UINT32          DwordHi;
  } Dword;
} IOMMU_MMIO_CMD_BASE_STRUCT;

#define IOMMU_MMIO_CMD_BUF_HDPTR_STRUCT_OFFSET  0x2008
typedef union {
  struct {
    UINT64            Reserved_3_0:4 ; ///<
    UINT64               CMD_HDPTR:15; ///<
    UINT64          Reserved_31_19:13; ///<
    UINT64           Reserved_31_0:32; ///<
  } Field;                             ///<
  struct {
    UINT32          DwordLo;
    UINT32          DwordHi;
  } Dword;
} IOMMU_MMIO_CMD_BUF_HDPTR_STRUCT;

#define IOMMU_MMIO_EVENT_BASE_STRUCT_OFFSET  0x10
typedef union {
  struct {
    UINT64             Reserved_11_0:12; ///<
    UINT64             EVENT_BASE:40;    ///<
    UINT64            Reserved_23_20:4 ; ///<
    UINT64                 EVENT_LEN:4 ; ///<
    UINT64            Reserved_31_28:4 ; ///<
  } Field;                               ///<
  struct {
    UINT32          DwordLo;
    UINT32          DwordHi;
  } Dword;                                                   ///<
} IOMMU_MMIO_EVENT_BASE_STRUCT;


// Invalid device table entry
typedef union {
  struct {
    UINT64     DeviceId:16;
    UINT64     PASID_9_16:4;
    UINT64     Reserved1:12;
    UINT64     PASID_15_0:16;
    UINT64     GN:1;
    UINT64     Reserved2:2;
    UINT64     I:1;
    UINT64     Reserved3:1;
    UINT64     RW:1;
    UINT64     Reserved4:1;
    UINT64     RZ:1;
    UINT64     TR:1;
    UINT64     Reserved5:3;
    UINT64     Code:4;
    UINT64     AddressLow:32;
    UINT64     AddressHigh:32;
  } Fields;
  struct {
    UINT64   Op1;
    UINT64   Op2;
  } Ops;
} IOMMU_EVENT_LOG_CODE1_ENTRY;

// IO Page fault
typedef union {
  struct {
    UINT64     DeviceId:16;
    UINT64     PASID_9_16:4;
    UINT64     Reserved1:12;
    UINT64     PASID_15_0:16;
    UINT64     GN:1;
    UINT64     NX:1;
    UINT64     US:1;
    UINT64     I:1;
    UINT64     PR:1;
    UINT64     RW:1;
    UINT64     PE:1;
    UINT64     RZ:1;
    UINT64     TR:1;
    UINT64     Reserved5:3;
    UINT64     Code:4;
    UINT64     AddressLow:32;
    UINT64     AddressHigh:32;
  } Fields;
  struct {
    UINT64   Op1;
    UINT64   Op2;
  } Ops;
} IOMMU_EVENT_LOG_CODE2_ENTRY;

/**
  Master IOMMU control structure

**/
#define IOMMU_MMIO_CNTRL_STRUCT_OFFSET 0x18
typedef union {
  struct {
    UINT32                                                IOMMU_EN:1 ; ///<
    UINT32                                               HT_TUN_EN:1 ; ///<
    UINT32                                            EVENT_LOG_EN:1 ; ///<
    UINT32                                            EVENT_INT_EN:1 ; ///<
    UINT32                                          COM_WAIT_INTEN:1 ; ///<
    UINT32                                             INV_TIMEOUT:3 ; ///<
    UINT32                                                 PASS_PW:1 ; ///<
    UINT32                                             RES_PASS_PW:1 ; ///<
    UINT32                                                COHERENT:1 ; ///<
    UINT32                                                    ISOC:1 ; ///<
    UINT32                                              CMD_BUF_EN:1 ; ///<
    UINT32                                              PPR_LOG_EN:1 ; ///<
    UINT32                                              PPR_INT_EN:1 ; ///<
    UINT32                                                  PPR_EN:1 ; ///<
    UINT32                                                   GT_EN:1 ; ///<
    UINT32                                                   GA_EN:1 ; ///<
    UINT32                                                    TLPT:4 ; ///<
    UINT32                                                 SMIF_EN:1 ; ///<
    UINT32                                          Reserved_23_23:1 ; ///<
    UINT32                                             SMIF_LOG_EN:1 ; ///<
    UINT32                                                  GAM_EN:3 ; ///<
    UINT32                                               GA_LOG_EN:1 ; ///<
    UINT32                                               GA_INT_EN:1 ; ///<
    UINT32                                                    PPRQ:2 ; ///<
    UINT32                                                  EVENTQ:2 ; ///<
    UINT32                                              DTE_SEG_EN:2 ; ///<
    UINT32                                            Reserved_4_4:1 ; ///<
    UINT32                                           PRIV_ABORT_EN:2 ; ///<
    UINT32                                        PPR_Auto_resp_en:1 ; ///<
    UINT32                                                 MARC_en:1 ; ///<
    UINT32                                       Block_StopMark_En:1 ; ///<
    UINT32                                       PPR_Auto_resp_AON:1 ; ///<
    UINT32                                          Reserved_12_11:2 ; ///<
    UINT32                                                  EPH_EN:1 ; ///<
    UINT32                                          HW_Prefetch_AD:2 ; ///<
    UINT32                                               V2_HD_Dis:1 ; ///<
    UINT32                                          Reserved_17_17:1 ; ///<
    UINT32                                                   XT_EN:1 ; ///<
    UINT32                                             IMUXTInt_EN:1 ; ///<
    UINT32                                                  vCmdEn:1 ; ///<
    UINT32                                                vIommuEn:1 ; ///<
    UINT32                                               V2_HA_Dis:1 ; ///<
    UINT32                                          Reserved_31_23:9 ; ///<
  } Field;                                                             ///<
  struct {
    UINT32          DwordLo;
    UINT32          DwordHi;
  } Dword;
  UINT64 Value;
} IOMMU_MMIO_CNTRL_STRUCT;

typedef union {
  struct {
    UINT64      Offset:12;
    UINT64      Level1:9;
    UINT64      Level2:9;
    UINT64      Level3:9;
    UINT64      Level4:9;
    UINT64      Level5:9;
    UINT64      Level6:7;
  } Field;
  UINT64 Value;
} IOMMU_DEVICE_BUFFER_ADDRESS;

#define IOMMU_COMMAND_BUFFER_POINTER_HEAD_STRUCT_OFFSET  0x2000
#define IOMMU_COMMAND_BUFFER_POINTER_TAIL_STRUCT_OFFSET  0x2008
#define IOMMU_EVENT_LOG_POINTER_HEAD_STRUCT_OFFSET       0x2010
#define IOMMU_EVENT_LOG_POINTER_TAIL_STRUCT_OFFSET       0x2018
#define IOMMU_STATUS_OFFSET                              0x2020

// TODO: Make these macros auto generated based on entries you want
//       because right now you cannot change one of these without changing the others
#define IOMMU_COMMAND_BUFFER_ENTRIES          1024  // Must be power of 2
#define IOMMU_COMMAND_BUFFER_ENTRIES_PER_PAGE 256
#define IOMMU_EVENT_LOG_ENTRIES               1024  // Must be power of 2
#define IOMMU_EVENT_LOG_ENTRIES_PER_PAGE      256

#define IOMMU_COMMAND_OR_EVENT_256_ENTRIES    0x8
#define IOMMU_COMMAND_OR_EVENT_512_ENTRIES    0x9
#define IOMMU_COMMAND_OR_EVENT_1024_ENTRIES   0xA

#define IOMMU_COMMAND_OR_EVENT_ENTRY_SIZE     16

typedef union {
  struct {
    UINT64      Value:19;
    UINT64      Reserved2:45;
  } Field;
  UINT64 Value;
} IOMMU_COMMAND_OR_LOG_POINTER_STRUCT;

typedef struct {
  UINT64  FirstEventCodeOperand1:60;
  UINT64  EventCode:4;
  UINT64  SecondEventCodeOperand;
} IOMMU_EVENT_LOG_ENTRY;

#define IOMMU_INVALIDATE_ALL_COMMAND  0x08
typedef struct {
  UINT64    Reserved1:60;
  UINT64    Command:4;
  UINT64    Reserved3;
} IOMMU_INVALIDATE_ALL_COMMAND_STRUCT;

#define IOMMU_INVALIDATE_PAGES_COMMAND  0x03
typedef struct {
  UINT64    PASID:20;
  UINT64    Reserved1:12;
  UINT64    DomainId:16;
  UINT64    Reserved2:12;
  UINT64    Command:4;
  UINT64    S:1;
  UINT64    PDE:1;
  UINT64    GN:1;
  UINT64    Reserved3:9;
  UINT64    Address:52;
} IOMMU_INVALIDATE_PAGES_COMMAND_STRUCT;

#define IOMMU_COMPLETION_WAIT_COMMAND  0x01
#define IOMMU_COMPLETION_WAIT_TIMEOUT  10000
typedef struct {
  UINT64    Cmp_Store:1;
  UINT64    Cmp_Intr:1;
  UINT64    Flush_Queue:1;
  UINT64    StoreAddr:49;
  UINT64    Reserved1:8;
  UINT64    Command:4;
  UINT64    StoreDataLo:32;
  UINT64    StoreDataHi:32;
} IOMMU_COMPLETION_WAIT_COMMAND_STRUCT;

#define AMD_IOMMU_TPL_LEVEL TPL_NOTIFY

#define SHIFT_4K_BOUNDARY                           12
#define ALIGNMENT_4K_REMAINDER_MASK                 0xFFF
#define PAGE_SIZE                                   0x1000
#define PAGE_SIZE_ENCODING                          0x1FFF

/**
 * Page Level Address constants
**/
#define LEVEL1_PAGE_TABLE_OFFSET    12
#define LEVEL1_PAGES_AVAILABLE      512

#define LEVEL2_PAGE_TABLE_OFFSET    21
#define LEVEL2_PAGES_AVAILABLE      512*512

#define LEVEL3_PAGE_TABLE_OFFSET    30
#define LEVEL3_PAGES_AVAILABLE      512*512*512

#define LEVEL4_PAGE_TABLE_OFFSET    39
#define LEVEL4_PAGES_AVAILABLE      512*512*512*512ull

#define LEVEL5_PAGE_TABLE_OFFSET    48
#define LEVEL5_PAGES_AVAILABLE      512*512*512*512*512ull

#define LEVEL6_PAGE_TABLE_OFFSET    57
#define LEVEL6_PAGES_AVAILABLE      512*512*512*512*512*512ull

// Do not change the below directory levels without examining the implications.
#define DXE_FIRST_PAGE_DIR_LEVEL64   4
#define DXE_FIRST_PAGE_DIR_LEVEL_2M  3
#define DXE_FIRST_PAGE_DIR_LEVEL_4K  2

#define PEI_FIRST_PAGE_DIR_LEVEL32  2
#define DOMAIN_ID32                 32
#define DOMAIN_ID64                 64

#define PAGE_LEVEL_ZERO_EXTEND      7

/**
 * Other constants used in loops and such
**/
#define TRANSLATION_OR_DIRECTORY_ENTRIES_PER_PAGE  512    ///9 Bits of address space per level, 512 entries per block
#define IOMMU_CONTEXT_DATA_ENTRIES                 8      ///At some point change to sockets*IOMMU
#define IOMMU_INVALID_ALL_PAGES_ADDRESS            0x7ffffffffffffULL

#pragma pack (pop)

#pragma pack (push, 8)
// For tracking IOMMU context, so that we can properly update different NBIO
typedef struct {
  BOOLEAN                                      Present;
  IOMMU_MMIO_CMD_BASE_STRUCT                   *CommandBufferAddressStruct;
  IOMMU_COMMAND_OR_LOG_POINTER_STRUCT          *CommandBufferHead;
  IOMMU_COMMAND_OR_LOG_POINTER_STRUCT          *CommandBufferTail;
  IOMMU_MMIO_EVENT_BASE_STRUCT                 *EventLogBaseAddressStruct;
  IOMMU_COMMAND_OR_LOG_POINTER_STRUCT          *EventLogHead;
  IOMMU_COMMAND_OR_LOG_POINTER_STRUCT          *EventLogTail;
  IOMMU_MMIO_CNTRL_STRUCT                      *IommuControlRegister;
  UINT64                                       CommandStore;
} IOMMU_CONTEXT_DATA;

typedef struct {
  BOOLEAN                                      Lock;
  BOOLEAN                                      InterruptState;
} SPIN_LOCK_DATA;

#pragma pack (pop)

#endif /* _GNBIOMMU_H_ */

