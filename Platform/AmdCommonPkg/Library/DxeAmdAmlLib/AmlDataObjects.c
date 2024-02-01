/*****************************************************************************
 *
 * Copyright (C) 2020-2024 Advanced Micro Devices, Inc. All rights reserved.
 *
 *****************************************************************************/

#include "InternalAmlLib.h"
#include <Library/PrintLib.h>


/*
  Calculates the optimized integer value used by AmlOPDataInteger and others

  Forces max integer size UINT64

  @param[in]    Integer         - Integer value to encode
  @param[out]   ReturnData      - Allocated DataBuffer with encoded integer
  @param[out]   ReturnDataSize  - Size of ReturnData

  @return       EFI_SUCCESS     - Successful completion
  @return       EFI_OUT_OF_RESOURCES - Failed to allocate ReturnDataBuffer
*/
EFI_STATUS
EFIAPI
InternalAmlDataIntegerBuffer (
  IN      UINT64      Integer,
  OUT     VOID        **ReturnData,
  OUT     UINTN       *ReturnDataSize
)
{
  UINT8               *Data;
  UINTN               DataSize;

  // Max Data Size is 64 bit. Plus one Opcode byte
  Data = AllocateZeroPool (sizeof (UINT64) + 1);
  if (Data == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: ERROR: Integer Space Alloc Failed\n", __FUNCTION__));
    return EFI_OUT_OF_RESOURCES;
  }

  if (Integer == 0) {
    // ZeroOp
    DataSize = 1;
    Data[0] = AML_ZERO_OP;
  } else if (Integer == 1) {
    // OneOp
    DataSize = 1;
    Data[0] = AML_ONE_OP;
  } else if (Integer == ~0x0){
    // OnesOp
    DataSize = 1;
    Data[0] = AML_ONES_OP;
  } else if (Integer >= 0x100000000) {
    // QWordConst
    DataSize = sizeof (UINT64) + 1;
    Data[0] = AML_QWORD_PREFIX;
    *(UINT64 *)&Data[1] = (UINT64)Integer;
  } else if (Integer >= 0x10000) {
    // DWordConst
    DataSize = sizeof (UINT32) + 1;
    Data[0] = AML_DWORD_PREFIX;
    *(UINT32 *)&Data[1] = (UINT32)Integer;
  } else if (Integer >= 0x100) {
    // WordConst
    DataSize = sizeof (UINT16) + 1;
    Data[0] = AML_WORD_PREFIX;
    *(UINT16 *)&Data[1] = (UINT16)Integer;
  } else {
    // ByteConst
    DataSize = sizeof (UINT8) + 1;
    Data[0] = AML_BYTE_PREFIX;
    *(UINT8 *)&Data[1] = (UINT8)Integer;
  }

  // Reallocate the pool so size is exact
  Data = ReallocatePool(sizeof(UINT64) + 1, DataSize, Data);
  *ReturnData = (VOID *)Data;
  *ReturnDataSize = DataSize;

  return EFI_SUCCESS;
}

/**
  Creates an optimized integer object

  Forces max integer size UINT64

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

  @param[in]      Integer   - Number to be optimized and encoded
  @param[in,out]  ListHead  - Head of Linked List of all AML Objects

  @return   EFI_SUCCESS     - Success
  @return   all others      - Fail
**/
EFI_STATUS
EFIAPI
AmlOPDataInteger (
  IN      UINT64      Integer,
  IN OUT  LIST_ENTRY  *ListHead
)
{
  EFI_STATUS          Status;
  AML_OBJECT_INSTANCE *Object;

  if (ListHead == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = EFI_DEVICE_ERROR;
  Object = NULL;

  Status = InternalAppendNewAmlObjectNoData (&Object, ListHead);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: ERROR: Start %a object\n", __FUNCTION__, "DATA_INTEGER"));
    goto Done;
  }

  Status = InternalAmlDataIntegerBuffer (Integer,
                                         (VOID **)&(Object->Data),
                                         &(Object->DataSize));
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: ERROR: ACPI Integer 0x%X object\n", __FUNCTION__, Integer));
    goto Done;
  }
  Object->Completed = TRUE;

  Status = EFI_SUCCESS;

  Done:
  if (EFI_ERROR (Status)) {
    InternalFreeAmlObject (&Object, ListHead);
  }
  return Status;
}

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
)
{
  EFI_STATUS          Status;
  AML_OBJECT_INSTANCE *Object;
  UINT8               *Data;
  UINTN               DataSize;
  UINTN               Index;

  if (String == NULL || ListHead == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = EFI_DEVICE_ERROR;
  Object = NULL;

  // Validate all characters
  DataSize = AsciiStrLen (String);
  for (Index = 0; Index < DataSize; Index++) {
    if (String[Index] < 0x01 ||
        String[Index] > 0x7F) {
      Status = EFI_INVALID_PARAMETER;
      DEBUG ((DEBUG_ERROR, "%a: ERROR: Invalid character String[%d] : %a\n",
              __FUNCTION__, Index, String));
      return Status;
    }
  }

  Status = InternalAppendNewAmlObjectNoData (&Object, ListHead);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: ERROR: Start %a object\n", __FUNCTION__, String));
    goto Done;
  }

  // AML_STRING_PREFIX + String + NULL Terminator
  DataSize += 2;
  Data = AllocatePool (DataSize);
  if (Data == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    DEBUG ((DEBUG_ERROR, "%a: ERROR: String Space Allocation %a\n",
            __FUNCTION__, String));
    goto Done;
  }

  Data[0] = AML_STRING_PREFIX;
  CopyMem (&Data[1], String, DataSize - 1);

  // DataString Complete, Put into Object
  Object->Data = Data;
  Object->DataSize = DataSize;
  Object->Completed = TRUE;

  Status = EFI_SUCCESS;

  Done:
  if (EFI_ERROR (Status)) {
    InternalFreeAmlObject (&Object, ListHead);
  }
  return Status;
}

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
)
{
  EFI_STATUS          Status;
  AML_OBJECT_INSTANCE *Object;

  if (Buffer == NULL || BufferSize == 0 || ListHead == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Object = NULL;

  Status = InternalAppendNewAmlObjectNoData (&Object, ListHead);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: ERROR: Start Data Buffer object\n", __FUNCTION__));
    goto Done;
  }

  Object->Data = AllocatePool (BufferSize);
  Object->DataSize = BufferSize;
  if (Object->Data == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    DEBUG ((DEBUG_ERROR, "%a: ERROR: Data Buffer allocate failed\n", __FUNCTION__));
    goto Done;
  }
  CopyMem (Object->Data, Buffer, BufferSize);
  Object->Completed = TRUE;

  Status = EFI_SUCCESS;

  Done:
  if (EFI_ERROR (Status)) {
    InternalFreeAmlObject (&Object, ListHead);
  }
  return Status;
}

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
)
{
  EFI_STATUS            Status;
  UINT32                EncodedEisaId;
  UINT8                 i;

  EncodedEisaId = 0;

  if ((String == NULL) || (ListHead == NULL)) {
    DEBUG ((DEBUG_ERROR, "%a: ERROR: Invalid parameter, inputs cannot == NULL.\n", __FUNCTION__));
    return EFI_INVALID_PARAMETER;
  }

  if (AsciiStrLen (String) != 0x7) {
    DEBUG ((DEBUG_ERROR, "%a: ERROR: Invalid length for 'String' parameter.\n", __FUNCTION__));
    return EFI_INVALID_PARAMETER;
  }

  //
  // Verify String is formatted as "UUUNNNN".
  //
  for (i = 0; i <= 0x6; i++) {
    //
    // If first 3 characters are not uppercase alpha or last 4 characters are not hexadecimal
    //
    if (((i <= 0x2) && (!IS_ASCII_UPPER_ALPHA (String[i]))) ||
        ((i >= 0x3) && (!IS_ASCII_HEX_DIGIT (String[i])))) {
      DEBUG ((DEBUG_ERROR, "%a: ERROR: Invalid EISA ID string format!\n", __FUNCTION__));
      DEBUG ((DEBUG_ERROR, "  Input String must be formatted as 'UUUNNNN'.\n"));
      return EFI_INVALID_PARAMETER;
    }
  }

  //
  // Convert string to 4-byte EISA ID encoding.
  //   Ex: 'PNP0A03' encodes to '0x30AD041'
  //
  EncodedEisaId = ((((String[0] - AML_NAME_CHAR_A + 1) & 0x1f) << 10)
                 + (((String[1] - AML_NAME_CHAR_A + 1) & 0x1f) <<  5)
                 + (((String[2] - AML_NAME_CHAR_A + 1) & 0x1f) <<  0)
                 + (UINT32) (AsciiStrHexToUint64 (&String[3]) << 16));

  //
  // Swap bytes of upper and lower WORD to format EISA ID with proper endian-ness.
  //
  EncodedEisaId = Swap4Bytes(EncodedEisaId);

  //
  // Insert DWordPrefix into list.
  //   Note: EncodedEisaId will always be 32-bits, resulting in DWordConst.
  //
  Status = AmlOPDataInteger (EncodedEisaId, ListHead);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: ERROR: Unable to create ACPI DWordConst from Encoded EISA ID.\n", __FUNCTION__));
    return Status;
  }

  return Status;
}
