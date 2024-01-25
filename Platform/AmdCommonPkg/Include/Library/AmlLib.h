/*****************************************************************************
 *
 * Copyright (C) 2019-2023 Advanced Micro Devices, Inc. All rights reserved.
 *
 *****************************************************************************/

#ifndef _AML_LIB_H_
#define _AML_LIB_H_

#include <Uefi.h>
#include <IndustryStandard/Acpi.h>

typedef enum {
  AmlStart,
  AmlClose,
  AmlInvalid
} AML_FUNCTION_PHASE;

#define AML_OBJECT_INSTANCE_SIGNATURE SIGNATURE_32 ('a', 'm', 'l', 'o')

//
//  Device Status Bitmap (Section 6.3.7 ACPI version 6.3)
//    Bit [0] - Set if the device is present.
//    Bit [1] - Set if the device is enabled and decoding its resources.
//    Bit [2] - Set if the device should be shown in the UI.
//    Bit [3] - Set if the device is functioning properly (cleared if device
//              failed its diagnostics).
//    Bit [4] - Set if the battery is present.
//    Bits [31:5] - Reserved (must be cleared).
//
#define DEVICE_PRESENT_BIT        0x0001
#define DEVICE_ENABLED_BIT        0x0002
#define DEVICE_IN_UI_BIT          0x0004
#define DEVICE_HEALTH_BIT         0x0008
#define DEVICE_BATTERY_BIT        0x0010  // Control Method Battery Device Only

typedef enum {
  UnknownObj,
  IntObj,
  StrObj,
  BuffObj,
  PkgObj,
  FieldUnitObj,
  DeviceObj,
  EventObj,
  MethodObj,
  MutexObj,
  OpRegionObj,
  PowerResObj,
  ProcessorObj,
  ThermalZoneObj,
  BuffFieldObj,
  DDBHandlObj,
  InvalidObj
} OBJECT_TYPE_KEYWORD;

typedef struct {
  UINT32      Signature;
  BOOLEAN     Completed;
  UINTN       DataSize;
  UINT8       *Data;
  LIST_ENTRY  Link;
} AML_OBJECT_INSTANCE;

// ***************************************************************************
//  AML defines to be consistent with already existing
//  MdePkg/Include/IndustryStandard/Acpi*.h defines.
//  *** These could be upstreamed at some point.
// ***************************************************************************
// Limits of (DWord|Word|QWord)Space ResourceType
#define EFI_ACPI_SPACE_RESOURCE_TYPE_MIN                                    0xC0
#define EFI_ACPI_SPACE_RESOURCE_TYPE_MAX                                    0xFF

// General Flags:  Flags that are common to all resource types
//   Bits[7:4] Reserved(must be 0)
//   Bit[3] Max Address Fixed, _MAF:
//     1 The specified maximum address is fixed
//     0 The specified maximum address is not fixed and can be changed
#define EFI_ACPI_GENERAL_FLAG_MAX_IS_FIXED                              (1 << 3)
#define EFI_ACPI_GENERAL_FLAG_MAX_IS_NOT_FIXED                          (0 << 3)
#define EFI_ACPI_GENERAL_FLAG_MASK_MAF                                  EFI_ACPI_GENERAL_FLAG_MAX_IS_FIXED
//   Bit[2] Min Address Fixed, _MIF:
//     1 The specified minimum address is fixed
//     0 The specified minimum address is not fixed and can be changed
#define EFI_ACPI_GENERAL_FLAG_MIN_IS_FIXED                              (1 << 2)
#define EFI_ACPI_GENERAL_FLAG_MIN_IS_NOT_FIXED                          (0 << 2)
#define EFI_ACPI_GENERAL_FLAG_MASK_MIF                                  EFI_ACPI_GENERAL_FLAG_MIN_IS_FIXED
//   Bit[1] Decode Type, _DEC:
//     1 This bridge subtractively decodes this address(top level bridges only)
//     0 This bridge positively decodes this address
#define EFI_ACPI_GENERAL_FLAG_DECODE_SUBTRACTIVE                        (1 << 1)
#define EFI_ACPI_GENERAL_FLAG_DECODE_POSITIVE                           (0 << 1)
#define EFI_ACPI_GENERAL_FLAG_MASK_DEC                                  EFI_ACPI_GENERAL_FLAG_DECODE_SUBTRACTIVE
//   Bit[0] Consumer / Producer:
//     1 This device consumes this resource
//     0 This device produces and consumes this resource
#define EFI_ACPI_GENERAL_FLAG_RESOURCE_CONSUMER                         (1 << 0)
#define EFI_ACPI_GENERAL_FLAG_RESOURCE_PRODUCER                         (0 << 0)
#define EFI_ACPI_GENERAL_FLAG_MASK_USAGE                                EFI_ACPI_GENERAL_FLAG_RESOURCE_CONSUMER

// Memory Resource Flag (Resource Type = 0) DefinitionsBits
// Memory Resource Flag Masks
//   Bits[7:6] Reserved(must be 0)
//   Bit[5] Memory to I/O Translation, _TTP:
//     0 TypeStatic
//     1 Type Translation
#define EFI_ACPI_MEMORY_RESOURCE_SPECIFIC_MASK_TTP                      (1 << 5)
//   Bit[4:3] Memory Attributes, _MTP:
//     0 AddressRangeMemory
//     1 AddressRangeReserved
//     2 AddressRangeACPI
//     3 AddressRangeNVS
#define EFI_ACPI_MEMORY_RESOURCE_SPECIFIC_MASK_MTP                      (3 << 3)
//   Bit[2:1] Memory Attributes, _MEM:
//     0 The memory is non-cacheable
//     1 The memory is cacheable
//     2 The memory is cacheable and supports write-combining
//     3 The memory is cacheable and prefetchable
#define EFI_ACPI_MEMORY_RESOURCE_SPECIFIC_MASK_MEM                      (3 << 1)
//   Bit[0] Write Status, _RW:
//     0 This memory range is read-only
//     1 This memory is read-write
#define EFI_ACPI_MEMORY_RESOURCE_SPECIFIC_MASK_RW                       (1 << 0)

// I/O Resource Flag (Resource Type = 1) DefinitionsBits
// I/O Resource Flags
//   Bit [7:6] Reserved (must be 0)
//   Bit [5] Sparse Translation, _TRS. This bit is only meaningful if Bit [4] is set.
//     1 SparseTranslation: The primary-side memory address of any specific I/O port
//       within the secondary-side range can be found using the following function.
//       address = (((port & 0xFFFc) << 10) || (port & 0xFFF)) + _TRA In the address
//       used to access the I/O port, bits[11:2] must be identical to bits[21:12],
//       this gives four bytes of I/O ports on each 4 KB page.
//     0 DenseTranslation: The primary-side memory address of any specific I/O port
//       within the secondary-side range can be found using the following function.
//       address = port + _TRA
#define EFI_ACPI_IO_RESOURCE_SPECIFIC_FLAG_SPARSE_TRANSLATION           (1 << 5)
#define EFI_ACPI_IO_RESOURCE_SPECIFIC_FLAG_DENSE_TRANSLATION            (0 << 5)
//   Bit [4] I/O to Memory Translation, _TTP
//     1 TypeTranslation: This resource, which is I/O on the secondary side of the
//       bridge, is memory on the primary side of the bridge.
//     0 TypeStatic: This resource, which is I/O on the secondary side of the
//       bridge, is also I/O on the primary side of the bridge.
#define EFI_ACPI_IO_RESOURCE_SPECIFIC_FLAG_TYPE_TRANSLATION             (1 << 4)
#define EFI_ACPI_IO_RESOURCE_SPECIFIC_FLAG_TYPE_STATIC                  (0 << 4)
//   Bit [3:2] Reserved (must be 0)
//   Bit [1:0] _RNG
//     3 Memory window covers the entire range
//     2 ISARangesOnly. This flag is for bridges on systems with multiple bridges.
//       Setting this bit means the memory window specified in this descriptor is
//       limited to the ISA I/O addresses that fall within the specified window. The
//       ISA I/O ranges are: n000-n0FF, n400-n4FF, n800-n8FF, nC00-nCFF. This bit can
//       only be set for bridges entirely configured throughACPI namespace.
//     1 NonISARangesOnly. This flag is for bridges on systems with multiple
//       bridges. Setting this bit means the memory window specified in this
//       descriptor is limited to the non-ISA I/O addresses that fall within the
//       specified window. The non-ISA I/O ranges are: n100-n3FF, n500-n7FF,
//       n900-nBFF, nD00-nFFF. This bit can only be set for bridges entirely
//       configured through ACPI namespace.
//     0 Reserved
#define EFI_ACPI_IO_RESOURCE_SPECIFIC_FLAG_TYPE_RANGE_ENTIRE            (3 << 0)
#define EFI_ACPI_IO_RESOURCE_SPECIFIC_FLAG_TYPE_RANGE_ISA_ONLY          (2 << 0)
#define EFI_ACPI_IO_RESOURCE_SPECIFIC_FLAG_TYPE_RANGE_NON_ISA_ONLY      (1 << 0)

#define AML_OBJECT_INSTANCE_FROM_LINK(a) CR (a, AML_OBJECT_INSTANCE, Link, AML_OBJECT_INSTANCE_SIGNATURE)

// Method Serialize Flag Values
typedef enum {
  NotSerialized,
  Serialized,
  FlagInvalid
} METHOD_SERIALIZE_FLAG;

//
// Resource Type Specific Flags
// Ref ACPI specification 6.4.3.5.5
//
//
// DMA Information
// Ref ACPI specification 6.4.2.2
//
// DmaType values
typedef enum {
  Compatibility = EFI_ACPI_DMA_SPEED_TYPE_COMPATIBILITY,
  TypeA         = EFI_ACPI_DMA_SPEED_TYPE_A,
  TypeB         = EFI_ACPI_DMA_SPEED_TYPE_B,
  TypeF         = EFI_ACPI_DMA_SPEED_TYPE_F
} EFI_ACPI_DMA_SPEED_TYPE_KEYWORDS;

// IsBusMaster values
typedef enum {
  NotBusMaster = 0,
  BusMaster    = EFI_ACPI_DMA_BUS_MASTER
} EFI_ACPI_DMA_BUS_MASTER_KEYWORDS;

// DmaTransferSize values
typedef enum {
  Transfer8    = EFI_ACPI_DMA_TRANSFER_TYPE_8_BIT,
  Transfer8_16 = EFI_ACPI_DMA_TRANSFER_TYPE_8_BIT_AND_16_BIT,
  Transfer16   = EFI_ACPI_DMA_TRANSFER_TYPE_16_BIT
} EFI_ACPI_DMA_TRANSFER_TYPE_KEYWORDS;

//
// Interrupt Resource Descriptor Information
// Ref ACPI specification 6.4.2.1
//
// IRQ Information - Wake Capability
//
#define EFI_ACPI_IRQ_WAKE_CAPABLE_MASK  0x20
#define   EFI_ACPI_IRQ_NOT_WAKE_CAPABLE 0x0
#define   EFI_ACPI_IRQ_WAKE_CAPABLE     0x20

typedef enum {
  NotWakeCapable = EFI_ACPI_IRQ_NOT_WAKE_CAPABLE,
  WakeCapable    = EFI_ACPI_IRQ_WAKE_CAPABLE
} EFI_ACPI_IRQ_WAKE_CAPABILITY_KEYWORDS;

//
// IRQ Information - Interrupt Sharing
//
#define EFI_ACPI_IRQ_EXCLUSIVE 0x0

typedef enum {
  Exclusive        = EFI_ACPI_IRQ_EXCLUSIVE,
  Shared           = EFI_ACPI_IRQ_SHARABLE,
  ExclusiveAndWake = EFI_ACPI_IRQ_WAKE_CAPABLE | EFI_ACPI_IRQ_EXCLUSIVE,
  SharedAndWake    = EFI_ACPI_IRQ_WAKE_CAPABLE | EFI_ACPI_IRQ_SHARABLE
} EFI_ACPI_IRQ_INTERRUPT_SHARING_KEYWORDS;

//
// IRQ Information - Interrupt Polarity
//
typedef enum {
  ActiveHigh = EFI_ACPI_IRQ_HIGH_TRUE,
  ActiveLow  = EFI_ACPI_IRQ_LOW_FALSE
} EFI_ACPI_IRQ_INTERRUPT_POLARITY_KEYWORDS;

//
// IRQ Information - Interrupt Mode
//
#define EFI_ACPI_IRQ_MODE_MASK 0x1
typedef enum {
  LevelTriggered = EFI_ACPI_IRQ_LEVEL_TRIGGERED,
  EdgeTriggered  = EFI_ACPI_IRQ_EDGE_TRIGGERED
} EFI_ACPI_IRQ_INTERRUPT_MODE_KEYWORDS;

// IO Port Descriptor Information
// Ref ACPI specification 6.4.2.5
//
typedef enum {
  Decode16 = EFI_ACPI_IO_DECODE_16_BIT,
  Decode10 = EFI_ACPI_IO_DECODE_10_BIT
} EFI_ACPI_IO_PORT_DESCRIPTOR_INFORMATION;

//
// Memory Resource Information
// Ref ACPI specification 6.4.3.5
//
// Consumer/Producer Bit[0]
typedef enum {
  ResourceProducer = EFI_ACPI_GENERAL_FLAG_RESOURCE_PRODUCER,
  ResourceConsumer = EFI_ACPI_GENERAL_FLAG_RESOURCE_CONSUMER
} RESOURCE_USAGE_FLAG;

// Decode Type (_DEC) Bit[1]
typedef enum {
  PosDecode = EFI_ACPI_GENERAL_FLAG_DECODE_POSITIVE,
  SubDecode = EFI_ACPI_GENERAL_FLAG_DECODE_SUBTRACTIVE
} MEM_DECODE_FLAG;

// Min Address Fixed (_MIF) Bit[2]
typedef enum {
  MinNotFixed = EFI_ACPI_GENERAL_FLAG_MIN_IS_NOT_FIXED,
  MinFixed = EFI_ACPI_GENERAL_FLAG_MIN_IS_FIXED
} MIN_FIXED_FLAG;

// Max Address Fixed (_MAF) Bit[3]
typedef enum {
  MaxNotFixed = EFI_ACPI_GENERAL_FLAG_MAX_IS_NOT_FIXED,
  MaxFixed = EFI_ACPI_GENERAL_FLAG_MAX_IS_FIXED
} MAX_FIXED_FLAG;

// Memory Attributes (_MEM) Bits[2:1]
typedef enum {
  NonCacheable = EFI_ACPI_MEMORY_RESOURCE_SPECIFIC_FLAG_NON_CACHEABLE,
  Cacheable = EFI_ACPI_MEMORY_RESOURCE_SPECIFIC_FLAG_CACHEABLE,
  WriteCombining = EFI_ACPI_MEMORY_RESOURCE_SPECIFIC_FLAG_CACHEABLE_WRITE_COMBINING,
  Prefetchable = EFI_ACPI_MEMORY_RESOURCE_SPECIFIC_FLAG_CACHEABLE_PREFETCHABLE
} CACHEABLE_FLAG;

// Write Status (_RW) Bit[0]
typedef enum {
  ReadOnly = EFI_ACPI_MEMORY_RESOURCE_SPECIFIC_FLAG_READ_ONLY,
  ReadWrite = EFI_ACPI_MEMORY_RESOURCE_SPECIFIC_FLAG_READ_WRITE
} READ_WRITE_FLAG;

// Memory Attributes (_MTP) Bits[4:3]
typedef enum {
  AddressRangeMemory = EFI_ACPI_MEMORY_RESOURCE_SPECIFIC_FLAG_ADDRESS_RANGE_MEMORY,
  AddressRangeReserved = EFI_ACPI_MEMORY_RESOURCE_SPECIFIC_FLAG_ADDRESS_RANGE_RESERVED,
  AddressRangeACPI = EFI_ACPI_MEMORY_RESOURCE_SPECIFIC_FLAG_ADDRESS_RANGE_ACPI,
  AddressRangeNVS = EFI_APCI_MEMORY_RESOURCE_SPECIFIC_FLAG_ADDRESS_RANGE_NVS
} MEMORY_RANGE_TYPE;

// Memory to IO Translation (_TTP) Bit[5]
// Note: IO and Memory Resources use different bits for this.
//       Value must be handled at function level when implemented.
typedef enum {
  TypeStatic = 0,
  TypeTranslation = 1
} MEMORY_TRANSLATION_TYPE;

// Memory Window Attributes (_RNG) Bits[1:0]
typedef enum {
  Reserved = 0,
  NonISAOnly = EFI_ACPI_IO_RESOURCE_SPECIFIC_FLAG_TYPE_RANGE_NON_ISA_ONLY,
  ISAOnly = EFI_ACPI_IO_RESOURCE_SPECIFIC_FLAG_TYPE_RANGE_ISA_ONLY,
  EntireRange = EFI_ACPI_IO_RESOURCE_SPECIFIC_FLAG_TYPE_RANGE_ENTIRE,
  ISARangeMax
} IO_ISA_RANGES;

// ***************************************************************************
//  AML Objects
// ***************************************************************************

// ---------------------------------------------------------------------------
//  Table and Table Header Encoding
// ---------------------------------------------------------------------------
/**
  Creates an AML Encoded Table
  Object must be created between AmlStart and AmlClose Phase

  DefBlockHeader  := TableSignature TableLength SpecCompliance CheckSum OemID
                     OemTableID OemRevision CreatorID CreatorRevision

  TableSignature  := DWordData      // As defined in section 5.2.3.
  TableLength     := DWordData      // Length of the table in bytes including the
                                    // block header
  SpecCompliance  := ByteData       // The revision of the structure.
  CheckSum        := ByteData       // Byte checksum of the entire table.
  OemID           := ByteData(6)    // OEM ID of up to 6 characters.
                                    // If the OEM ID is shorter than 6
                                    // characters, it can be terminated with a
                                    // NULL character.
  OemTableID      := ByteData(8)    // OEM Table ID of up to 8 characters.
                                    // If the OEM Table ID is shorter than
                                    // 8 characters, it can be terminated with
                                    // a NULL character.
  OemRevision     := DWordData      // OEM Table Revision.
  CreatorID       := DWordData      // Vendor ID of the ASL compiler.
  CreatorRevision := DWordData      // Revision of the ASL compiler.

  @param[in]      Phase           - Either AmlStart or AmlClose
  @param[in]      TableNameString - Table Name
  @param[in]      ComplianceRev   - Compliance Revision
  @param[in]      OemId           - OEM ID
  @param[in]      OemTableId      - OEM ID of table
  @param[in]      OemRevision     - OEM Revision number
  @param[in]      CreatorId       - Vendor ID of the ASL compiler
  @param[in]      CreatorRevision - Vendor Revision of the ASL compiler
  @param[in,out]  ListHead        - Linked list has completed String Object after
                                    AmlClose.

  @retval         EFI_SUCCESS
  @retval         Error status
**/
EFI_STATUS
EFIAPI
AmlDefinitionBlock (
  IN      AML_FUNCTION_PHASE  Phase,
  IN      CHAR8               *TableNameString,
  IN      UINT8               ComplianceRev,
  IN      CHAR8               *OemId,
  IN      CHAR8               *OemTableId,
  IN      UINT32              OemRevision,
  IN      CHAR8               *CreatorId,
  IN      UINT32              CreatorRevision,
  IN OUT  LIST_ENTRY          *ListHead
);

// ---------------------------------------------------------------------------
//  Name Objects Encoding
// ---------------------------------------------------------------------------

/**
  Creates a Namestring AML Object and inserts it into the linked list

  Completes NameString in one call as "one phase"
  LeadNameChar      := 'A'-'Z' | '_'
  DigitChar         := '0'-'9'
  NameChar          := DigitChar | LeadNameChar
  RootChar          := '\'
  ParentPrefixChar  := '^'

  'A'-'Z'           := 0x41 - 0x5A
  '_'               := 0x5F
  '0'-'9'           := 0x30 - 0x39
  '\'               := 0x5C
  '^'               := 0x5E

  NameSeg           := <LeadNameChar NameChar NameChar NameChar>
                      // Notice that NameSegs shorter than 4 characters are filled with
                      // trailing underscores ('_'s).
  NameString        := <RootChar NamePath> | <PrefixPath NamePath>
  PrefixPath        := Nothing | <'^' PrefixPath>
  NamePath          := NameSeg | DualNamePath | MultiNamePath | NullName

  DualNamePath      := DualNamePrefix NameSeg NameSeg
  DualNamePrefix    := 0x2E
  MultiNamePath     := MultiNamePrefix SegCount NameSeg(SegCount)
  MultiNamePrefix   := 0x2F

  SegCount          := ByteData

  Note:SegCount can be from 1 to 255. For example: MultiNamePrefix(35) is
      encoded as 0x2f 0x23 and followed by 35 NameSegs. So, the total encoding
      length will be 1 + 1 + 35*4 = 142. Notice that: DualNamePrefix NameSeg
      NameSeg has a smaller encoding than the encoding of: MultiNamePrefix(2)
      NameSeg NameSeg

  SimpleName := NameString | ArgObj | LocalObj
  SuperName := SimpleName | DebugObj | Type6Opcode
  NullName := 0x00
  Target := SuperName | NullName

  @param[in]      String    - Null Terminated NameString Representation
  @param[in,out]  ListHead  - Head of Linked List of all AML Objects

  @return   EFI_SUCCESS     - Success
  @return   all others      - Fail
  **/
EFI_STATUS
EFIAPI
AmlOPNameString (
  IN      CHAR8               *String,
  IN OUT  LIST_ENTRY          *ListHead
);

// ---------------------------------------------------------------------------
//  Data Objects Encoding
// ---------------------------------------------------------------------------

/**
  Creates an optimized integer object

  ComputationalData := ByteConst | WordConst | DWordConst | QWordConst | String |
                       ConstObj | RevisionOp | DefBuffer
  DataObject        := ComputationalData | DefPackage | DefVarPackage
  DataRefObject     := DataObject | ObjectReference | DDBHandle
  ByteConst         := BytePrefix ByteData
  BytePrefix        := 0x0A
  WordConst         := WordPrefix WordData
  WordPrefix        := 0x0B
  DWordConst        := DWordPrefix DWordData
  DWordPrefix       := 0x0C
  QWordConst        := QWordPrefix QWordData
  QWordPrefix       := 0x0E
  ConstObj          := ZeroOp | OneOp | OnesOp
  ByteData          := 0x00 - 0xFF
  WordData          := ByteData[0:7] ByteData[8:15]
                       // 0x0000-0xFFFF
  DWordData         := WordData[0:15] WordData[16:31]
                       // 0x00000000-0xFFFFFFFF
  QWordData         := DWordData[0:31] DWordData[32:63]
                       // 0x0000000000000000-0xFFFFFFFFFFFFFFFF
  ZeroOp            := 0x00
  OneOp             := 0x01
  OnesOp            := 0xFF

  @param[in]      Phase     - Example: AmlStart, AmlClose
  @param[in]      Integer   - Number to be optimized and encoded
  @param[in,out]  ListHead  - Head of Linked List of all AML Objects

  @return   EFI_SUCCESS     - Success
  @return   all others      - Fail
**/
EFI_STATUS
EFIAPI
AmlOPDataInteger (
  IN      UINT64              Integer,
  IN OUT  LIST_ENTRY          *ListHead
);

/**
  Creates a data string object

  ComputationalData   := String

  String              := StringPrefix AsciiCharList NullChar
  StringPrefix        := 0x0D
  AsciiCharList       := Nothing | <AsciiChar AsciiCharList>
  AsciiChar           := 0x01 - 0x7F
  NullChar            := 0x00

  @param[in]      String    - String to be encoded
  @param[in,out]  ListHead  - Head of Linked List of all AML Objects

  @return   EFI_SUCCESS     - Success
  @return   all others      - Fail
**/
EFI_STATUS
EFIAPI
AmlOPDataString (
  IN      CHAR8               *String,
  IN OUT  LIST_ENTRY          *ListHead
);

/**
  Creates a data buffer AML object from an array

  This will take the passed in buffer and generate an AML Object from that
  buffer

  @param[in]      Buffer      - Buffer to be placed in AML Object
  @param[in]      BufferSize  - Size of Buffer to be copied into Object
  @param[in,out]  ListHead    - Head of Linked List of all AML Objects

  @return   EFI_SUCCESS       - Success
  @return   all others        - Fail
**/
EFI_STATUS
EFIAPI
AmlOPDataBufferFromArray (
  IN      VOID                *Buffer,
  IN      UINTN               BufferSize,
  IN OUT  LIST_ENTRY          *ListHead
);

// ---------------------------------------------------------------------------
//  Package Length Encoding
// ---------------------------------------------------------------------------

/**
  Creates a Package Length AML Object and inserts it into the linked list

  PkgLength := PkgLeadByte |
               <PkgLeadByte ByteData> |
               <PkgLeadByte ByteData ByteData> |
               <PkgLeadByte ByteData ByteData ByteData>

  PkgLeadByte := <bit 7-6: ByteData count that follows (0-3)>
                 <bit 5-4: Only used if PkgLength < 63>
                 <bit 3-0: Least significant package length nybble>

  Note: The high 2 bits of the first byte reveal how many follow bytes are in
  the PkgLength. If the PkgLength has only one byte, bit 0 through 5 are used
  to encode the package length (in other words, values 0-63). If the package
  length value is more than 63, more than one byte must be used for the encoding
  in which case bit 4 and 5 of the PkgLeadByte are reserved and must be zero.

  If the multiple bytes encoding is used, bits 0-3 of the PkgLeadByte become
  the least significant 4 bits of the resulting package length value. The next
  ByteData will become the next least significant 8 bits of the resulting value
  and so on, up to 3 ByteData bytes. Thus, the maximum package length is 2**28.

  @param[in]      Phase     - Example: AmlStart, AmlClose
  @param[in,out]  ListHead  - Head of Linked List of all AML Objects

  @return   EFI_SUCCESS     - Success
  @return   all others      - Fail
**/
EFI_STATUS
EFIAPI
AmlPkgLength (
  IN      AML_FUNCTION_PHASE  Phase,
  IN OUT  LIST_ENTRY          *ListHead
);

// ---------------------------------------------------------------------------
//  Term Objects Encoding
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  Namespace Modifier Objects Encoding
// ---------------------------------------------------------------------------

/**
  Creates a Scope (ObjectName, Object)

  Object must be created between AmlStart and AmlClose Phase

  DefScope  := ScopeOp PkgLength NameString TermList
  ScopeOp   := 0x10

  @param[in]      Phase     - Either AmlStart or AmlClose
  @param[in]      String    - Location
  @param[in,out]  ListHead  - Linked list has completed String Object after
                              AmlClose.

  @retval         EFI_SUCCESS
  @retval         Error status
**/
EFI_STATUS
EFIAPI
AmlScope (
  IN      AML_FUNCTION_PHASE  Phase,
  IN      CHAR8               *String,
  IN OUT  LIST_ENTRY          *ListHead
);

/**
  Creates a Name (ObjectName, Object)

  Object must be created between AmlStart and AmlClose Phase

  DefName  := NameOp NameString DataRefObject
  NameOp   := 0x08

  @param[in]      Phase     - Either AmlStart or AmlClose
  @param[in]      Name      - Named Object name
  @param[in,out]  ListHead  - Linked list has completed Name Object after AmlClose

  @retval         EFI_SUCCESS
  @retval         Error status
**/
EFI_STATUS
EFIAPI
AmlName (
  IN      AML_FUNCTION_PHASE  Phase,
  IN      CHAR8               *String,
  IN OUT  LIST_ENTRY          *ListHead
);

/**
  Creates an Alias (SourceObject, AliasObject)

  DefAlias  := AliasOp NameString NameString
  AliasOp   := 0x06

  @param[in]      SourceName - Any named Source Object NameString
  @param[in]      AliasName  - Alias Object NameString
  @param[in,out]  ListHead   - Linked list has completed the Alias Object

  @retval         EFI_SUCCESS
  @retval         Error status
**/
EFI_STATUS
EFIAPI
AmlOPAlias (
  IN      CHAR8               *SourceName,
  IN      CHAR8               *AliasName,
  IN OUT  LIST_ENTRY          *ListHead
);

// ---------------------------------------------------------------------------
//  Named Objects Encoding
// ---------------------------------------------------------------------------

/**
  Creates a CreateDWordField AML Object and inserts it into the linked list

  Syntax:
  CreateDWordField ( SourceBuffer, ByteIndex, DWordFieldName )

  CreateDWordField must be created between AmlStart and AmlClose Phase.

  DefCreateDWordField := CreateDWordFieldOp SourceBuff ByteIndex NameString
  CreateDWordFieldOp := 0x8A

  @param[in]      Phase           - Either AmlStart or AmlClose
  @param[in,out]  ListHead        - Linked list has completed String Object after
                                    AmlClose.

  @retval         EFI_SUCCESS
  @retval         Error status
**/
EFI_STATUS
EFIAPI
AmlCreateDWordField (
  IN      AML_FUNCTION_PHASE  Phase,
  IN OUT  LIST_ENTRY          *ListHead
);

/**
  Creates a Device (ObjectName, Object)

  Object must be created between AmlStart and AmlClose Phase

  DefName  := DeviceOp PkgLength NameString TermList
  NameOp   := ExtOpPrefix 0x82
  ExtOpPrefix  := 0x5B

  @param[in]      Phase     - Either AmlStart or AmlClose
  @param[in]      Name      - Named Object name
  @param[in,out]  ListHead  - Linked list has completed Name Object after AmlClose

  @retval         EFI_SUCCESS
  @retval         Error status
**/
EFI_STATUS
EFIAPI
AmlDevice (
  IN      AML_FUNCTION_PHASE  Phase,
  IN      CHAR8               *String,
  IN OUT  LIST_ENTRY          *ListHead
);

/**
  Creates an External Object

  External (ObjectName, ObjectType, ReturnType, ParameterTypes)

  Note: ReturnType is not used for AML encoding and is therefore not passed in
        to this function.
        ParameterTypes is only used if the ObjectType is a MethodObj. It
        specifies MethodObj's argument types in a list.  For the purposes of
        this library, we are passing in the the number of input parameters for
        that MethodObj.

  DefExternal    := ExternalOp NameString ObjectType ArgumentCount
  ExternalOp     := 0x15
  ObjectType     := ByteData
  ArgumentCount  := ByteData (0 - 7)

  @param[in]      Name        - Object name
  @param[in]      ObjectType  - Type of object declared
  @param[in]      NumArgs     - Only used if ObjectType is MethodObj.
                                Specifies the number of input parameters for
                                that MethodObj since AML doesn't store
                                ArgTypes here.
                                Otherwise, ignored.
  @param[in,out]  ListHead    - Linked list that has completed External Object
                                after AmlClose.

  @retval         EFI_SUCCESS
  @retval         Error status
**/
EFI_STATUS
EFIAPI
AmlOPExternal (
  IN      CHAR8               *Name,
  IN      UINT8               ObjectType,
  IN      UINT8               NumArgs,
  IN OUT  LIST_ENTRY          *ListHead
);

/**
  Creates a Method

  Method (MethodName, NumArgs, SerializeRule, SyncLevel, ReturnType,
          ParameterTypes) {TermList}

  TermList must be created between AmlStart and AmlClose Phase

  Note: ReturnType and ParameterTypes are not used for AML encoding
        and are therefore not passed in to this function.

  DefMethod    := MethodOp PkgLength NameString MethodFlags TermList
  MethodOp     := 0x14
  MethodFlags  := ByteData  // bit 0-2: ArgCount (0-7)
                            // bit 3: SerializeFlag
                            // 0 NotSerialized
                            // 1 Serialized
                            // bit 4-7: SyncLevel (0x00-0x0f)

  @param[in]      Phase         - Either AmlStart or AmlClose
  @param[in]      Name          - Method name
  @param[in]      NumArgs       - Number of arguments passed in to method
  @param[in]      SerializeRule - Flag indicating whether method is serialized
                                  or not
  @param[in]      SyncLevel     - Number of arguments passed in to method
  @param[in,out]  ListHead      - Linked list has completed String Object after
                                  AmlClose.

  @retval         EFI_SUCCESS
  @retval         Error status
**/
EFI_STATUS
EFIAPI
AmlMethod (
  IN      AML_FUNCTION_PHASE     Phase,
  IN      CHAR8                  *Name,
  IN      UINT8                  NumArgs,
  IN      METHOD_SERIALIZE_FLAG  SerializeRule,
  IN      UINT8                  SyncLevel,
  IN OUT  LIST_ENTRY             *ListHead
);

// ---------------------------------------------------------------------------
//  Type 1 Opcodes Encoding
// ---------------------------------------------------------------------------
/**
  Creates a Return object

  Object must be created between AmlStart and AmlClose Phase

  DefReturn  := ReturnOp ArgObject
  ReturnOp   := 0xA4
  ArgObject  := TermArg => DataRefObject

  @param[in]      Phase     - Either AmlStart or AmlClose
  @param[in,out]  ListHead  - Head of linked list of Objects

  @retval         EFI_SUCCESS
  @retval         Error status
**/
EFI_STATUS
EFIAPI
AmlReturn (
  IN      AML_FUNCTION_PHASE  Phase,
  IN OUT  LIST_ENTRY          *ListHead
);

// ---------------------------------------------------------------------------
//  Type 2 Opcodes Encoding
// ---------------------------------------------------------------------------

/**
  Creates a  Buffer (BufferSize) {Initializer} => Buffer Object

  Initializers must be created between AmlStart and AmlClose Phase

  DefBuffer   := BufferOp PkgLength BufferSize ByteList
  BufferOp    := 0x11
  BufferSize  := TermArg => Integer

  @param[in]      Phase       - Either AmlStart or AmlClose
  @param[in]      BufferSize  - Requested BufferSize, Encoded value will be
                                MAX (BufferSize OR Child->DataSize)
  @param[in,out]  ListHead    - Linked list has completed Buffer Object after
                                AmlClose.

  @retval         EFI_SUCCESS
  @retval         Error status
**/
EFI_STATUS
EFIAPI
AmlBuffer (
  IN      AML_FUNCTION_PHASE  Phase,
  IN      UINTN               BufferSize,
  IN OUT  LIST_ENTRY          *ListHead
);

/**
  Creates a  Package (NumElements) {PackageList} => Package

  Initializers must be created between AmlStart and AmlClose Phase

  DefPackage         := PackageOp PkgLength NumElements PackageElementList
  PackageOp          := 0x12
  DefVarPackage      := VarPackageOp PkgLength VarNumElements PackageElementList
  VarPackageOp       := 0x13
  NumElements        := ByteData
  VarNumElements     := TermArg => Integer
  PackageElementList := Nothing | <PackageElement PackageElementList>
  PackageElement     := DataRefObject | NameString

  @param[in]      Phase       - Either AmlStart or AmlClose
  @param[in]      NumElements - Number of elements in the package
  @param[in,out]  ListHead    - Linked list has completed Package Object after
                                AmlClose.

  @retval         EFI_SUCCESS
  @retval         Error status
**/
EFI_STATUS
EFIAPI
AmlPackage (
  IN      AML_FUNCTION_PHASE  Phase,
  IN      UINTN               NumElements,
  IN OUT  LIST_ENTRY          *ListHead
);

// ---------------------------------------------------------------------------
//  Resource Descriptor Objects Encoding
// ---------------------------------------------------------------------------

/**
  ResourceTemplate (Resource To Buffer Conversion Macro)

  Syntax:
  ResourceTemplate () {ResourceMacroList} => Buffer

  @param[in]      Phase     - Either AmlStart or AmlClose
  @param[in,out]  ListHead  - Linked list has completed ResourceTemplate Object
                              after AmlClose.

  @retval         EFI_SUCCESS
  @retval         Error status
**/
EFI_STATUS
EFIAPI
AmlResourceTemplate (
  IN      AML_FUNCTION_PHASE  Phase,
  IN OUT  LIST_ENTRY          *ListHead
);

/**
  19.6.32 DMA (DMA Resource Descriptor Macro)

  This function only requires a single call and therefore no Phases
  Syntax
  DMA (DmaType, IsBusMaster, DmaTransferSize, DescriptorName) {DmaChannelList} => Buffer (BitMask)

  Generates: 6.4.2.2 DMA Descriptor

  @param[in]      DmaType         - DMA channel speed supported
  @param[in]      IsBusMaster     - Logical device bus master status
  @param[in]      DmaTransferSize - DMA transfer type preference (8-bit, 16-bit, both)
  @param[in]      DmaChannelList  - DMA channel mask bits [7:0] (channels 0 - 7), _DMA
                                    Bit [0] is channel 0, etc.
  //              DescriptorName  - Optional - NOT IMPLEMENTED
  @param[in,out]  ListHead  - Linked list has completed DWordIO buffer

  @retval         EFI_SUCCESS
  @retval         Error status
**/
EFI_STATUS
EFIAPI
AmlOPDma (
  IN      EFI_ACPI_DMA_SPEED_TYPE_KEYWORDS    DmaType,
  IN      EFI_ACPI_DMA_BUS_MASTER_KEYWORDS    IsBusMaster,
  IN      EFI_ACPI_DMA_TRANSFER_TYPE_KEYWORDS DmaTransferSize,
  IN      UINT8                               DmaChannelList,
  //                                          DescriptorName - NOT IMPLEMENTED
  IN OUT  LIST_ENTRY                          *ListHead
);

/**
  19.6.33 DWordIO (DWord IO Resource Descriptor Macro)

  This function only requires a single call and therefore no Phases
  Syntax
  DWordIO (ResourceUsage, IsMinFixed, IsMaxFixed, Decode, ISARanges,
           AddressGranularity, AddressMinimum, AddressMaximum,
           AddressTranslation, RangeLength, ResourceSourceIndex,
           ResourceSource, DescriptorName, TranslationType,
           TranslationDensity)

  defines for pass in parameters can be found in:
  MdePkg/Include/IndustryStandard/Acpi10.h

  @param[in]      ResourceUsage,
  @param[in]      IsMinFixed,
  @param[in]      IsMaxFixed,
  @param[in]      Decode,
  @param[in]      ISARanges,
  @param[in]      AddressGranularity,
  @param[in]      AddressMinimum,
  @param[in]      AddressMaximum,
  @param[in]      AddressTranslation,
  @param[in]      RangeLength,
                  ResourceSourceIndex - NOT IMPLEMENTED
                  ResourceSource - NOT IMPLEMENTED
                  DescriptorName - NOT IMPLEMENTED
                  TranslationType - NOT IMPLEMENTED
                  TranslationDensity - NOT IMPLEMENTED
  @param[in,out]  ListHead  - Linked list has completed DWordIO buffer

  @retval         EFI_SUCCESS
  @retval         Error status
**/
EFI_STATUS
EFIAPI
AmlOPDWordIO (
  IN      UINT8       ResourceUsage,
  IN      UINT8       IsMinFixed,
  IN      UINT8       IsMaxFixed,
  IN      UINT8       Decode,
  IN      UINT8       ISARanges,
  IN      UINT32      AddressGranularity,
  IN      UINT32      AddressMinimum,
  IN      UINT32      AddressMaximum,
  IN      UINT32      AddressTranslation,
  IN      UINT32      RangeLength,
//                    ResourceSourceIndex - NOT IMPLEMENTED
//                    ResourceSource - NOT IMPLEMENTED
//                    DescriptorName - NOT IMPLEMENTED
//                    TranslationType - NOT IMPLEMENTED
//                    TranslationDensity - NOT IMPLEMENTED
  IN OUT  LIST_ENTRY  *ListHead
);

/**
  19.6.34 DWordMemory (DWord Memory Resource Descriptor Macro)

  This function only requires a single call and therefore no Phases
  Syntax
  DWordMemory (ResourceUsage, Decode, IsMinFixed, IsMaxFixed, Cacheable,
               ReadAndWrite, AddressGranularity, AddressMinimum, AddressMaximum,
               AddressTranslation, RangeLength, ResourceSourceIndex,
               ResourceSource, DescriptorName, MemoryRangeType, TranslationType)

  defines for pass in parameters can be found in:
  MdePkg/Include/IndustryStandard/Acpi10.h

  @param[in]      ResourceUsage,
  @param[in]      Decode,
  @param[in]      IsMinFixed,
  @param[in]      IsMaxFixed,
  @param[in]      Cacheable,
  @param[in]      ReadAndWrite,
  @param[in]      AddressGranularity,
  @param[in]      AddressMinimum,
  @param[in]      AddressMaximum,
  @param[in]      AddressTranslation,
  @param[in]      RangeLength,
                  ResourceSourceIndex - NOT IMPLEMENTED
                  ResourceSource - NOT IMPLEMENTED
                  DescriptorName - NOT IMPLEMENTED
                  MemoryRangeType - NOT IMPLEMENTED
                  TranslationType - NOT IMPLEMENTED
  @param[in,out]  ListHead  - Linked list has completed DWordMemory buffer

  @retval         EFI_SUCCESS
  @retval         Error status
**/
EFI_STATUS
EFIAPI
AmlOPDWordMemory (
  IN      UINT8       ResourceUsage,
  IN      UINT8       Decode,
  IN      UINT8       IsMinFixed,
  IN      UINT8       IsMaxFixed,
  IN      UINT8       Cacheable,
  IN      UINT8       ReadAndWrite,
  IN      UINT32      AddressGranularity,
  IN      UINT32      AddressMinimum,
  IN      UINT32      AddressMaximum,
  IN      UINT32      AddressTranslation,
  IN      UINT32      RangeLength,
  //                  ResourceSourceIndex - NOT IMPLEMENTED
  //                  ResourceSource - NOT IMPLEMENTED
  //                  DescriptorName - NOT IMPLEMENTED
  //                  MemoryRangeType - NOT IMPLEMENTED
  //                  TranslationType - NOT IMPLEMENTED
  IN OUT  LIST_ENTRY  *ListHead
);

/**
  AmlDWordSpace ()

  19.6.35 DWordSpace (DWord Space Resource Descriptor Macro)

  This function only requires a single call and therefore no Phases
  Syntax
  DWordSpace (ResourceType, ResourceUsage, Decode, IsMinFixed, IsMaxFixed,
              TypeSpecificFlags, AddressGranularity, AddressMinimum,
              AddressMaximum, AddressTranslation, RangeLength,
              ResourceSourceIndex, ResourceSource, DescriptorName)

  Generates:
  6.4.3.5.2 DWord Address Space Descriptor
  Type 1, Large Item Value 0x7

  @param[in]      ResourceType
  @param[in]      ResourceUsage,
  @param[in]      Decode,
  @param[in]      IsMinFixed,
  @param[in]      IsMaxFixed,
  @param[in]      TypeSpecificFlags,
  @param[in]      AddressGranularity,
  @param[in]      AddressMinimum,
  @param[in]      AddressMaximum,
  @param[in]      AddressTranslation,
  @param[in]      RangeLength,
                  ResourceSourceIndex - NOT IMPLEMENTED
                  ResourceSource - NOT IMPLEMENTED
                  DescriptorName - NOT IMPLEMENTED
  @param[in,out]  ListHead  - Linked list has completed DWordSpace buffer

  @retval         EFI_SUCCESS
  @retval         Error status
**/
EFI_STATUS
EFIAPI
AmlOPDWordSpace (
  IN      UINT8       ResourceType,
  IN      UINT8       ResourceUsage,
  IN      UINT8       Decode,
  IN      UINT8       IsMinFixed,
  IN      UINT8       IsMaxFixed,
  IN      UINT8       TypeSpecificFlags,
  IN      UINT32      AddressGranularity,
  IN      UINT32      AddressMinimum,
  IN      UINT32      AddressMaximum,
  IN      UINT32      AddressTranslation,
  IN      UINT32      RangeLength,
  //                  ResourceSourceIndex - NOT IMPLEMENTED
  //                  ResourceSource - NOT IMPLEMENTED
  //                  DescriptorName - NOT IMPLEMENTED
  IN OUT  LIST_ENTRY  *ListHead
);

/**
  19.6.36 EISAID (EISA ID String To Integer Conversion Macro)

  Syntax:
    EISAID (EisaIdString) => DWordConst

  Arguments:
    The EisaIdString must be a String object of the form "UUUNNNN", where "U"
    is an uppercase letter and "N" is a hexadecimal digit. No asterisks or other
    characters are allowed in the string.

  Description:
    Converts EisaIdString, a 7-character text string argument, into its
    corresponding 4-byte numeric EISA ID encoding. It can be used when declaring
    IDs for devices that have EISA IDs.

    Encoded EISA ID Definition - 32-bits
     bits[15:0] - three character compressed ASCII EISA ID. *
     bits[31:16] - binary number
      * Compressed ASCII is 5 bits per character 0b00001 = 'A' 0b11010 = 'Z'

  Example:
    EISAID ("PNP0C09") // This is a valid invocation of the macro.

  @param[in]      String    - EISA ID string.
  @param[in,out]  ListHead  - Head of Linked List of all AML Objects
**/
EFI_STATUS
EFIAPI
AmlOPEisaId (
  IN      CHAR8               *String,
  IN OUT  LIST_ENTRY          *ListHead
);

/**
  19.6.82 Memory32Fixed (Memory Resource Descriptor Macro)

  Syntax:
    Memory32Fixed (ReadAndWrite, AddressBase, RangeLength, DescriptorName)

  Arguments:
    ReadAndWrite: Specifies whether or not the memory region is read-only (ReadOnly)
    or read/write (ReadWrite). If nothing is specified, then ReadWrite is assumed.
    The 1-bit field DescriptorName._RW is automatically created to refer to this
    portion of the resource descriptor, where '1' is ReadWrite and '0' is ReadOnly.

    AddressBase: Evaluates to a 32-bit integer that specifies the base address
    of the memory range. The 32-bit field DescriptorName. _BAS is automatically
    created to refer to this portion of the resource descriptor.  RangeLength
    evaluates to a 32-bit integer that specifies the total number of bytes decoded
    in the memory range. The 32-bit field DescriptorName. _LEN is automatically
    created to refer to this portion of the resource descriptor.

    DescriptorName: Is an optional argument that specifies a name for an integer
    constant that will be created in the current scope that contains the offset
    of this resource descriptor within the current resource template buffer. The
    predefined descriptor field names may be appended to this name to access
    individual fields within the descriptor via the Buffer Field operators.

  Description:
    The Memory32Fixed macro evaluates to a buffer which contains a 32-bit memory
    descriptor, which describes a fixed range of memory addresses. The format of
    the fixed 32-bit memory descriptor can be found in 32-Bit Fixed Memory Range
    Descriptor. The macro is designed to be used inside of a ResourceTemplate.

  Generates:
    6.4.3.4 32-Bit Fixed Memory Range Descriptor
    Type 1, Large Item Value 0x6
    This memory range descriptor describes a device's memory resources within a
    32-bit address space.

  @param[in]      ReadAndWrite,
  @param[in]      AddressBase,
  @param[in]      RangeLength,
                  DescriptorName - NOT IMPLEMENTED
  @param[in,out]  ListHead  - Linked list has completed memory resource descriptor

  @retval         EFI_SUCCESS
  @retval         Error status
**/
EFI_STATUS
EFIAPI
AmlOPMemory32Fixed (
 IN      READ_WRITE_FLAG ReadAndWrite,
 IN      UINT32          AddressBase,
 IN      UINT32          RangeLength,
 IN OUT  LIST_ENTRY      *ListHead
);

/**
  19.6.64 IO (IO Resource Descriptor Macro)

  Syntax:
    IO (Decode, AddressMin, AddressMax, AddressAlignment, RangeLength, DescriptorName) => Buffer

  Arguments:
    Decode:
      Describes whether the I/O range uses 10-bit decode (Decode10) or 16-bit
      decode (Decode16).  The field DescriptorName. _DEC is automatically created
      to refer to this portion of the resource descriptor, where '1' is Decode16
      and '0' is Decode10.

    AddressMin:
      Evaluates to a 16-bit integer that specifies the minimum acceptable starting
      address for the I/O range. It must be an even multiple of AddressAlignment.
      The field DescriptorName._MIN is automatically created to refer to this
      portion of the resource descriptor.

    AddressMax:
      Evaluates to a 16-bit integer that specifies the maximum acceptable starting
      address for the I/O range. It must be an even multiple of AddressAlignment.
      The field DescriptorName._MAX is automatically created to refer to this
      portion of the resource descriptor.

    AddressAlignment:
      Evaluates to an 8-bit integer that specifies the alignment granularity
      for the I/O address assigned. The field DescriptorName. _ALN is automatically
      created to refer to this portion of the resource descriptor.

    RangeLength:
      Evaluates to an 8-bit integer that specifies the number of bytes in the
      I/O range. The field DescriptorName. _LEN is automatically created to refer
      to this portion of the resource descriptor.

    DescriptorName:
      An optional argument that specifies a name for an integer constant that
      will be created in the current scope that contains the offset of this
      resource descriptor within the current resource template buffer. The
      predefined descriptor field names may be appended to this name to access
      individual fields within the descriptor via the Buffer Field operators.

  Description:
    The IO macro evaluates to a buffer which contains an IO resource descriptor.
    The format of the IO descriptor can be found in the ACPI Specification section
    "I/O Port Descriptor".  The macro is designed to be used inside of a ResourceTemplate.

  Generates:
    6.4.2.5 I/O Port Descriptor
    Type 0, Small Item Name 0x8, Length = 7

  @param[in]      Decode,
  @param[in]      AddressMin,
  @param[in]      AddressMax,
  @param[in]      AddressAlignment,
  @param[in]      RangeLength,
                  DescriptorName - NOT IMPLEMENTED
  @param[in,out]  ListHead  - Linked list has completed IO buffer

  @retval         EFI_SUCCESS
  @retval         Error status
**/
EFI_STATUS
EFIAPI
AmlOPIO (
  IN      EFI_ACPI_IO_PORT_DESCRIPTOR_INFORMATION  Decode,
  IN      UINT16                                   AddressMin,
  IN      UINT16                                   AddressMax,
  IN      UINT8                                    AddressAlignment,
  IN      UINT8                                    RangeLength,
  //                                               DescriptorName - NOT IMPLEMENTED
  IN OUT  LIST_ENTRY                               *ListHead
);

/**
  19.6.109 QWordIO (QWord IO Resource Descriptor Macro)

  This function only requires a single call and therefore no Phases
  Syntax
  QWordIO (ResourceUsage, IsMinFixed, IsMaxFixed, Decode, ISARanges,
           AddressGranularity, AddressMinimum, AddressMaximum,
           AddressTranslation, RangeLength, ResourceSourceIndex,
           ResourceSource, DescriptorName, TranslationType,
           TranslationDensity)

  defines for pass in parameters can be found in:
  MdePkg/Include/IndustryStandard/Acpi10.h

  @param[in]      ResourceUsage,
  @param[in]      IsMinFixed,
  @param[in]      IsMaxFixed,
  @param[in]      Decode,
  @param[in]      ISARanges,
  @param[in]      AddressGranularity,
  @param[in]      AddressMinimum,
  @param[in]      AddressMaximum,
  @param[in]      AddressTranslation,
  @param[in]      RangeLength,
                  ResourceSourceIndex - NOT IMPLEMENTED
                  ResourceSource - NOT IMPLEMENTED
                  DescriptorName - NOT IMPLEMENTED
                  TranslationType - NOT IMPLEMENTED
                  TranslationDensity - NOT IMPLEMENTED
  @param[in,out]  ListHead  - Linked list has completed QWordIO buffer

  @retval         EFI_SUCCESS
  @retval         Error status
**/
EFI_STATUS
EFIAPI
AmlOPQWordIO (
  IN      UINT8       ResourceUsage,
  IN      UINT8       IsMinFixed,
  IN      UINT8       IsMaxFixed,
  IN      UINT8       Decode,
  IN      UINT8       ISARanges,
  IN      UINT64      AddressGranularity,
  IN      UINT64      AddressMinimum,
  IN      UINT64      AddressMaximum,
  IN      UINT64      AddressTranslation,
  IN      UINT64      RangeLength,
//                    ResourceSourceIndex - NOT IMPLEMENTED
//                    ResourceSource - NOT IMPLEMENTED
//                    DescriptorName - NOT IMPLEMENTED
//                    TranslationType - NOT IMPLEMENTED
//                    TranslationDensity - NOT IMPLEMENTED
  IN OUT  LIST_ENTRY  *ListHead
);

/**
  19.6.110 QWordMemory (QWord Memory Resource Descriptor Macro)

  This function only requires a single call and therefore no Phases
  Syntax
  QWordMemory (ResourceUsage, Decode, IsMinFixed, IsMaxFixed, Cacheable,
               ReadAndWrite, AddressGranularity, AddressMinimum, AddressMaximum,
               AddressTranslation, RangeLength, ResourceSourceIndex,
               ResourceSource, DescriptorName, MemoryRangeType, TranslationType)

  defines for pass in parameters can be found in:
  MdePkg/Include/IndustryStandard/Acpi10.h

  @param[in]      ResourceUsage,
  @param[in]      Decode,
  @param[in]      IsMinFixed,
  @param[in]      IsMaxFixed,
  @param[in]      Cacheable,
  @param[in]      ReadAndWrite,
  @param[in]      AddressGranularity,
  @param[in]      AddressMinimum,
  @param[in]      AddressMaximum,
  @param[in]      AddressTranslation,
  @param[in]      RangeLength,
                  ResourceSourceIndex - NOT IMPLEMENTED
                  ResourceSource - NOT IMPLEMENTED
                  DescriptorName - NOT IMPLEMENTED
                  MemoryRangeType - NOT IMPLEMENTED
                  TranslationType - NOT IMPLEMENTED
  @param[in,out]  ListHead  - Linked list has completed QWordMemory buffer

  @retval         EFI_SUCCESS
  @retval         Error status
**/
EFI_STATUS
EFIAPI
AmlOPQWordMemory (
  IN      UINT8       ResourceUsage,
  IN      UINT8       Decode,
  IN      UINT8       IsMinFixed,
  IN      UINT8       IsMaxFixed,
  IN      UINT8       Cacheable,
  IN      UINT8       ReadAndWrite,
  IN      UINT64      AddressGranularity,
  IN      UINT64      AddressMinimum,
  IN      UINT64      AddressMaximum,
  IN      UINT64      AddressTranslation,
  IN      UINT64      RangeLength,
  //                  ResourceSourceIndex - NOT IMPLEMENTED
  //                  ResourceSource - NOT IMPLEMENTED
  //                  DescriptorName - NOT IMPLEMENTED
  //                  MemoryRangeType - NOT IMPLEMENTED
  //                  TranslationType - NOT IMPLEMENTED
  IN OUT  LIST_ENTRY  *ListHead
);

/**
  AmlQWordSpace ()

  19.6.111 QWordSpace (QWord Space Resource Descriptor Macro)

  This function only requires a single call and therefore no Phases
  Syntax
  QWordSpace (ResourceType, ResourceUsage, Decode, IsMinFixed, IsMaxFixed,
              TypeSpecificFlags, AddressGranularity, AddressMinimum,
              AddressMaximum, AddressTranslation, RangeLength,
              ResourceSourceIndex, ResourceSource, DescriptorName)

  Generates:
  6.4.3.5.1 QWord Address Space Descriptor
  Type 1, Large Item Value 0xA
  The QWORD address space descriptor is used to report resource usage in a
  64-bit address space (like memory and I/O).

  @param[in]      ResourceType
  @param[in]      ResourceUsage,
  @param[in]      Decode,
  @param[in]      IsMinFixed,
  @param[in]      IsMaxFixed,
  @param[in]      TypeSpecificFlags,
  @param[in]      AddressGranularity,
  @param[in]      AddressMinimum,
  @param[in]      AddressMaximum,
  @param[in]      AddressTranslation,
  @param[in]      RangeLength,
                  ResourceSourceIndex - NOT IMPLEMENTED
                  ResourceSource - NOT IMPLEMENTED
                  DescriptorName - NOT IMPLEMENTED
  @param[in,out]  ListHead  - Linked list has completed QWordSpace buffer

  @retval         EFI_SUCCESS
  @retval         Error status
**/
EFI_STATUS
EFIAPI
AmlOPQWordSpace (
  IN      UINT8       ResourceType,
  IN      UINT8       ResourceUsage,
  IN      UINT8       Decode,
  IN      UINT8       IsMinFixed,
  IN      UINT8       IsMaxFixed,
  IN      UINT8       TypeSpecificFlags,
  IN      UINT64      AddressGranularity,
  IN      UINT64      AddressMinimum,
  IN      UINT64      AddressMaximum,
  IN      UINT64      AddressTranslation,
  IN      UINT64      RangeLength,
  //                  ResourceSourceIndex - NOT IMPLEMENTED
  //                  ResourceSource - NOT IMPLEMENTED
  //                  DescriptorName - NOT IMPLEMENTED
  IN OUT  LIST_ENTRY  *ListHead
);

/**
  19.6.65 IRQ (Interrupt Resource Descriptor Macro)

  Syntax:
    IRQ (EdgeLevel, ActiveLevel, Shared, DescriptorName) {InterruptList} => Buffer

  Arguments:
    EdgeLevel:
      Describes whether the interrupt is edge triggered (Edge) or level triggered
      (Level). The field DescriptorName. _HE is automatically created to refer to
      this portion of the resource descriptor, where '1' is Edge and ActiveHigh
      and '0' is Level and ActiveLow.

    ActiveLevel:
      Describes whether the interrupt is active-high (ActiveHigh) or active-low
      (ActiveLow). The field DescriptorName. _LL is automatically created to refer
      to this portion of the resource descriptor, where '1' is Edge and ActiveHigh
      and '0' is Level and ActiveLow.

    Shared:
      Describes whether the interrupt can be shared with other devices (Shared) or
      not (Exclusive), and whether it is capable of waking the system from a
      low-power idle or system sleep state (SharedAndWake or ExclusiveAndWake).
      The field DescriptorName. _SHR is automatically created to refer to this portion
      of the resource descriptor, where '1' is Shared and '0' is Exclusive. If nothing
      is specified, then Exclusive is assumed.

    InterruptList:
      IRQ mask bits [15:0]
      Bit[0] represents IRQ0, bit[1] is IRQ1, etc.

    DescriptorName:
      Is an optional argument that specifies a name for an integer constant that
      will be created in the current scope that contains the offset of this resource
      descriptor within the current resource template buffer. The predefined
      descriptor field names may be appended to this name to access individual
      fields within the descriptor via the Buffer Field operators.

    Description:
      The IRQ macro evaluates to a buffer that contains an IRQ resource descriptor.
      The format of the IRQ descriptor can be found in "IRQ Descriptor". The macro
      produces the three-byte form of the descriptor. The macro is designed to be
      used inside of a ResourceTemplate.

  Generates: 6.4.2.1 IRQ Descriptor

  @param[in]      EdgeLevel       - trigger level supported
  @param[in]      ActiveLevel     - interrupt polarity
  @param[in]      Shared          - interrupt exclusivity
  @param[in]      InterruptList   - IRQ mask bits[7:0], _INT
                                      Bit [0] represents IRQ0,
                                      bit[1] is IRQ1, and so on.
                                    IRQ mask bits[15:8], _INT
                                      Bit [0] represents IRQ8,
                                      bit[1] is IRQ9, and so on.
  //              DescriptorName  - Optional - NOT IMPLEMENTED
  @param[in,out]  ListHead  - Linked list has completed IRQ buffer

  @retval         EFI_SUCCESS
  @retval         Error status

**/
EFI_STATUS
EFIAPI
AmlOPIRQ (
  IN      EFI_ACPI_IRQ_INTERRUPT_MODE_KEYWORDS      EdgeLevel,
  IN      EFI_ACPI_IRQ_INTERRUPT_POLARITY_KEYWORDS  ActiveLevel,
  IN      EFI_ACPI_IRQ_INTERRUPT_SHARING_KEYWORDS   Shared,
  IN      UINT16                                    InterruptList,
  //                                                DescriptorName - NOT IMPLEMENTED
  IN OUT  LIST_ENTRY                                *ListHead
);

/**
  19.6.150 WordBusNumber (Word Bus Number Resource Descriptor Macro)

  This function only requires a single call and therefore no Phases
  Syntax
  WordBusNumber (ResourceUsage, IsMinFixed, IsMaxFixed, Decode,
                 AddressGranularity, AddressMinimum, AddressMaximum,
                 AddressTranslation, RangeLength, ResourceSourceIndex,
                 ResourceSource, DescriptorName)

  defines for pass in parameters can be found in:
  MdePkg/Include/IndustryStandard/Acpi10.h

  @param[in]      ResourceUsage,
  @param[in]      IsMinFixed,
  @param[in]      IsMaxFixed,
  @param[in]      Decode,
  @param[in]      AddressGranularity,
  @param[in]      AddressMinimum,
  @param[in]      AddressMaximum,
  @param[in]      AddressTranslation,
  @param[in]      RangeLength,
                  ResourceSourceIndex - NOT IMPLEMENTED
                  ResourceSource - NOT IMPLEMENTED
                  DescriptorName - NOT IMPLEMENTED
  @param[in,out]  ListHead  - Linked list has completed WordBusNumber
                              Descriptor

  @retval         EFI_SUCCESS
  @retval         Error status
**/
EFI_STATUS
EFIAPI
AmlOPWordBusNumber (
  IN      UINT8       ResourceUsage,
  IN      UINT8       IsMinFixed,
  IN      UINT8       IsMaxFixed,
  IN      UINT8       Decode,
  IN      UINT16      AddressGranularity,
  IN      UINT16      AddressMinimum,
  IN      UINT16      AddressMaximum,
  IN      UINT16      AddressTranslation,
  IN      UINT16      RangeLength,
//                    ResourceSourceIndex - NOT IMPLEMENTED
//                    ResourceSource - NOT IMPLEMENTED
//                    DescriptorName - NOT IMPLEMENTED
  IN OUT  LIST_ENTRY  *ListHead
);

/**
  19.6.151 WordIO (Word IO Resource Descriptor Macro)

  This function only requires a single call and therefore no Phases
  Syntax
  WordIO (ResourceUsage, IsMinFixed, IsMaxFixed, Decode, ISARanges,
          AddressGranularity, AddressMinimum, AddressMaximum,
          AddressTranslation, RangeLength, ResourceSourceIndex,
          ResourceSource, DescriptorName, TranslationType, TranslationDensity)

  defines for pass in parameters can be found in:
  MdePkg/Include/IndustryStandard/Acpi10.h

  @param[in]      ResourceUsage,
  @param[in]      IsMinFixed,
  @param[in]      IsMaxFixed,
  @param[in]      Decode,
  @param[in]      ISARanges,
  @param[in]      AddressGranularity,
  @param[in]      AddressMinimum,
  @param[in]      AddressMaximum,
  @param[in]      AddressTranslation,
  @param[in]      RangeLength,
                  ResourceSourceIndex - NOT IMPLEMENTED
                  ResourceSource - NOT IMPLEMENTED
                  DescriptorName - NOT IMPLEMENTED
                  TranslationType - NOT IMPLEMENTED
                  TranslationDensity - NOT IMPLEMENTED
  @param[in,out]  ListHead  - Linked list has completed WordIO Descriptor

  @retval         EFI_SUCCESS
  @retval         Error status
**/
EFI_STATUS
EFIAPI
AmlOPWordIO (
  IN      UINT8       ResourceUsage,
  IN      UINT8       IsMinFixed,
  IN      UINT8       IsMaxFixed,
  IN      UINT8       Decode,
  IN      UINT8       ISARanges,
  IN      UINT16      AddressGranularity,
  IN      UINT16      AddressMinimum,
  IN      UINT16      AddressMaximum,
  IN      UINT16      AddressTranslation,
  IN      UINT16      RangeLength,
//                    ResourceSourceIndex - NOT IMPLEMENTED
//                    ResourceSource - NOT IMPLEMENTED
//                    DescriptorName - NOT IMPLEMENTED
//                    TranslationType - NOT IMPLEMENTED
//                    TranslationDensity - NOT IMPLEMENTED
  IN OUT  LIST_ENTRY  *ListHead
);

/**
  19.6.152 WordSpace (Word Space Resource Descriptor Macro) )
  Syntax
  WordSpace (ResourceType, ResourceUsage, Decode, IsMinFixed, IsMaxFixed,
             TypeSpecificFlags, AddressGranularity, AddressMinimum,
             AddressMaximum, AddressTranslation, RangeLength,
             ResourceSourceIndex, ResourceSource, DescriptorName)

  Generates:
  6.4.3.5.3 Word Address Space Descriptor
  Type 1, Large Item Value 0x8

  @param[in]      ResourceType
  @param[in]      ResourceUsage,
  @param[in]      Decode,
  @param[in]      IsMinFixed,
  @param[in]      IsMaxFixed,
  @param[in]      TypeSpecificFlags,
  @param[in]      AddressGranularity,
  @param[in]      AddressMinimum,
  @param[in]      AddressMaximum,
  @param[in]      AddressTranslation,
  @param[in]      RangeLength,
                  ResourceSourceIndex - NOT IMPLEMENTED
                  ResourceSource - NOT IMPLEMENTED
                  DescriptorName - NOT IMPLEMENTED
  @param[in,out]  ListHead  - Linked list has completed WordSpace Descriptor

  @retval         EFI_SUCCESS
  @retval         Error status
**/
EFI_STATUS
EFIAPI
AmlOPWordSpace (
  IN      UINT8       ResourceType,
  IN      UINT8       ResourceUsage,
  IN      UINT8       Decode,
  IN      UINT8       IsMinFixed,
  IN      UINT8       IsMaxFixed,
  IN      UINT8       TypeSpecificFlags,
  IN      UINT16      AddressGranularity,
  IN      UINT16      AddressMinimum,
  IN      UINT16      AddressMaximum,
  IN      UINT16      AddressTranslation,
  IN      UINT16      RangeLength,
  //                  ResourceSourceIndex - NOT IMPLEMENTED
  //                  ResourceSource - NOT IMPLEMENTED
  //                  DescriptorName - NOT IMPLEMENTED
  IN OUT  LIST_ENTRY  *ListHead
);

// ---------------------------------------------------------------------------
//  Expression Opcodes Encoding
// ---------------------------------------------------------------------------

/**
  Creates a Store expression

  Syntax:
  Store (Source, Destination) => DataRefObject Destination = Source => DataRefObject

  Store expression must be created between AmlStart and AmlClose Phase.

  DefStore := StoreOp TermArg SuperName
  StoreOp := 0x70

  @param[in]      Phase           - Either AmlStart or AmlClose
  @param[in,out]  ListHead        - Linked list has completed String Object after
                                    AmlClose.

  @retval         EFI_SUCCESS
  @retval         Error status
**/
EFI_STATUS
EFIAPI
AmlStore (
  IN      AML_FUNCTION_PHASE  Phase,
  IN OUT  LIST_ENTRY          *ListHead
);

// ---------------------------------------------------------------------------
//  Miscellaneous Objects Encoding
// ---------------------------------------------------------------------------

// ***************************************************************************
//  AML Assistance Functions
// ***************************************************************************

/**
  Free all the children AML_OBJECT_INSTANCE(s) of ListHead.
  Will not free ListHead nor an Object containing ListHead.

  @param[in,out]  ListHead  - Head of linked list of Objects

  @retval         EFI_SUCCESS
**/
EFI_STATUS
EFIAPI
AmlFreeObjectList (
  IN OUT  LIST_ENTRY    *ListHead
);

// ***************************************************************************
//  AML Debug Functions
// ***************************************************************************

/**
  DEBUG print a (VOID *)buffer in an array of HEX bytes

       00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F
  0000 54 48 49 53 20 49 53 20 41 20 53 41 4D 50 4C 45  THIS IS A SAMPLE
  0010 5F 42 55 46 46 45 52 01 02 5E 5C 30 31           _BUFFER..^\01
  Completed=(TRUE|FALSE)

  @param[in]      Buffer      - Buffer containing buffer
  @param[in]      BufferSize  - Number of bytes to print
**/
EFI_STATUS
EFIAPI
AmlDebugPrintBuffer (
  IN      VOID    *Buffer,
  IN      UINTN   BufferSize
);

/**
  DEBUG print a buffer in an array of HEX bytes

       00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F
  0000 54 48 49 53 20 49 53 20 41 20 53 41 4D 50 4C 45  THIS.IS.A.SAMPLE
  0010 5F 42 55 46 46 45 52 01 02 5E 5C 30 31           _BUFFER..^\01
  Completed=(TRUE|FALSE)

  @param[in]      Object - AML_OBJECT_INSTANCE

  @retval         EFI_SUCCESS
  @retval         Error status
**/
EFI_STATUS
EFIAPI
AmlDebugPrintObject (
  IN      AML_OBJECT_INSTANCE *Object
);

/**
  DEBUG print a linked list of AML buffer Objects in an array of HEX bytes

  @param[in]      ListHead - Head of AML_OBJECT_INSTANCE Linked List

  @retval         EFI_SUCCESS
  @retval         Error status
**/
EFI_STATUS
EFIAPI
AmlDebugPrintLinkedObjects (
  IN      LIST_ENTRY *ListHead
);

/**
  Creates an ArgN Opcode object

  Arg Objects Encoding
    ArgObj := Arg0Op | Arg1Op | Arg2Op | Arg3Op | Arg4Op |Arg5Op | Arg6Op
    Arg0Op := 0x68
    Arg1Op := 0x69
    Arg2Op := 0x6A
    Arg3Op := 0x6B
    Arg4Op := 0x6C
    Arg5Op := 0x6D
    Arg6Op := 0x6E

  @param[in]      ArgN      - Argument Number to be encoded
  @param[in,out]  ListHead  - Head of Linked List of all AML Objects

  @return   EFI_SUCCESS     - Success
  @return   all others      - Fail
**/
EFI_STATUS
EFIAPI
AmlOpArgN (
  IN      UINT8       ArgN,
  IN OUT  LIST_ENTRY  *ListHead
);
#endif // _AML_LIB_H_
