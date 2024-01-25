/******************************************************************************
  Copyright (C) 2020-2023 Advanced Micro Devices, Inc. All rights reserved.

 *****************************************************************************/

#include <Base.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <IndustryStandard/AcpiAml.h>
#include <Library/DebugLib.h>
#include <Library/AcpiSdtParserLib.h>

/*
  Calculate PkgLength

  @param(in,out)  Buffer      Current ACPI Table Pointer
  @param(in,out)  Size        Current Size of ACPI Table
  @param(in)      NameString  NameString to Locate

  @retval Number of bytes that make up the PkgLength
*/
UINT32
AmlGetPkgLength (
  IN UINT8    *Buffer,
  OUT UINT32  *PkgLength
  )
{
  UINT32 Bytes, Length;

  Bytes = (UINT32)((Buffer[0] >> 6) & 0x3) + 1;
  switch (Bytes) {
    case 1:
      Length = (UINT32)Buffer[0];
      break;

    case 2:
      Length =  (UINT32)(Buffer[0] & 0x0F);
      Length += (UINT32)(Buffer[1] & 0xFF) << 4;
      break;

    case 3:
      Length =  (UINT32)(Buffer[0] & 0x0F);
      Length += (UINT32)(Buffer[1] & 0xFF) << 4;
      Length += (UINT32)(Buffer[2] & 0xFF) << 12;
      break;

    default: /* 4 bytes */
      Length =  (UINT32)(Buffer[0] & 0x0F);
      Length += (UINT32)(Buffer[1] & 0xFF) << 4;
      Length += (UINT32)(Buffer[2] & 0xFF) << 12;
      Length += (UINT32)(Buffer[3] & 0xFF) << 20;
      break;
  }

  *PkgLength = Length;
  return Bytes;
}

/*
  Find ACPI NameString through brute force.

  @param(in,out)  Buffer      Current ACPI Table Pointer
  @param(in,out)  Size        Current Size of ACPI Table
  @param(in)      NameString  NameString to Locate

  @retval EFI_SUCCESS       Requested NameString found.
  @retval EFI_NOT_FOUND     Requested NameString not found.
*/
EFI_STATUS
EFIAPI
ScanAcpiTableNameString(
  IN OUT  VOID    **Buffer,
  IN OUT  UINT32  *Size,
  IN      CHAR8   *NameString
  )
{
  UINT32  RemainingSize;
  UINTN   NameStringLength;
  UINT8   *CurrentLocation;

  RemainingSize = *Size;
  CurrentLocation = (UINT8 *)*Buffer;
  NameStringLength = AsciiStrLen (NameString);
  while (RemainingSize > 0) {
    if (CompareMem (CurrentLocation, NameString, NameStringLength) == 0) {
      break;
    }
    CurrentLocation++;
    RemainingSize--;
  }
  if (RemainingSize == 0) {
    DEBUG((DEBUG_ERROR, "%a: Did Not Find NameString %a\n", __FUNCTION__, NameString));
    *Buffer = NULL;
    *Size = 0;
    return EFI_NOT_FOUND;
  }
  *Buffer = CurrentLocation;
  *Size = RemainingSize;
  return EFI_SUCCESS;
}

/*
  Verify SDT

  @param(in,out)  Buffer      Current ACPI Table Pointer
  @param(in,out)  Size        Current Size of ACPI Table
  @param(in)      NameString  NameString to Locate

  @retval EFI_SUCCESS       Requested Scope found.
  @retval EFI_NOT_FOUND     Requested Scope not found.
*/
EFI_STATUS
EFIAPI
VerifySdtTableHeader (
  IN OUT  VOID    *Buffer,
  IN OUT  UINT32  Size,
  IN      CHAR8   *NameString
  )
{
  EFI_STATUS                  Status;
  EFI_ACPI_DESCRIPTION_HEADER *CurrentTable;
  UINT32                      Length;

  Status = EFI_NOT_FOUND;
  CurrentTable = (EFI_ACPI_DESCRIPTION_HEADER *)Buffer;
  Length = (UINT32)AsciiStrLen (NameString);
  if (CompareMem (&CurrentTable->Signature, NameString, Length) == 0 &&
      CurrentTable->Length == Size) {
    Status = EFI_SUCCESS;
  }
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Table provided does not match %a\n", __FUNCTION__, NameString));
  }
  return  Status;
}

/*
  Locate ACPI Device

  DefDevice := DeviceOp PkgLength NameString TermList
  DeviceOp := ExtOpPrefix 0x82

  ExtOpPrefix := 0x5B

  PkgLength := PkgLeadByte |
               <PkgLeadByte ByteData> |
               <PkgLeadByte ByteData ByteData> |
               <PkgLeadByte ByteData ByteData ByteData>

  PkgLeadByte := <bit 7-6: ByteData count that follows (0-3)>
               <bit 5-4: Only used if PkgLength < 63>
               <bit 3-0: Least significant package length nybble>

  @param(in,out)  Buffer      Current ACPI Table Pointer
  @param(in,out)  Size        Current Size of ACPI Table
  @param(in)      NameString  NameString to Locate

  @retval EFI_SUCCESS       Requested Scope found.
  @retval EFI_NOT_FOUND     Requested Scope not found.
*/
EFI_STATUS
EFIAPI
LocateAcpiDefDeviceTermList (
  IN OUT  VOID    **Buffer,
  IN OUT  UINT32  *Size,
  IN      CHAR8   *NameString
  )
{
  EFI_STATUS  Status;
  UINT32      RemainingSize;
  UINT8       *CurrentLocation;
  UINT8       *OpLocation;
  UINT32      TermListSize;
  UINT32      PkgLengthSize;
  UINT32      NameStringLength;

  CurrentLocation = *Buffer;
  RemainingSize = *Size;
  NameStringLength = (UINT32)AsciiStrnLenS (NameString, RemainingSize);
  // Locate NameString
  while (RemainingSize > 0) {
    Status = ScanAcpiTableNameString ((VOID **)&CurrentLocation,
                                      &RemainingSize,
                                      NameString);
    if (EFI_ERROR (Status)) {
      ASSERT_EFI_ERROR (Status);
      return Status;
    }
    // Look backwards for Op Code
    for (OpLocation = CurrentLocation - 5;
          OpLocation < CurrentLocation;
          OpLocation++) {
      if (*(UINT16 *)OpLocation == (AML_EXT_OP | (AML_EXT_DEVICE_OP << 8))) {
        break;
      }
    }
    if (OpLocation < CurrentLocation) {
      PkgLengthSize = AmlGetPkgLength(OpLocation + 2, &TermListSize);
      // Point to TermList and set size
      CurrentLocation += NameStringLength;
      TermListSize -= (PkgLengthSize + NameStringLength);
      *Buffer = CurrentLocation;
      *Size = TermListSize;
      return EFI_SUCCESS;
    } else {
      // Not DeviceOp, so skip NameString and keep searching.
      CurrentLocation += NameStringLength;
      RemainingSize -= NameStringLength;
    }
    CurrentLocation++;
    RemainingSize--;
  }
  ASSERT (FALSE);
  return EFI_NOT_FOUND;
}

/*
  Locate ACPI Named Object

  DefName := NameOp NameString DataRefObject
  NameOp := 0x08

  @param(in,out)  Buffer      Current ACPI Table Pointer
  @param(in,out)  Size        Current Size of ACPI Table
  @param(in)      NameString  NameString to Locate

  @retval EFI_SUCCESS       Requested Scope found.
  @retval EFI_NOT_FOUND     Requested Scope not found.
*/
EFI_STATUS
EFIAPI
LocateAcpiDefNameDataRefObject (
  IN OUT  VOID    **Buffer,
  IN OUT  UINT32  *Size,
  IN      CHAR8   *NameString
  )
{
  EFI_STATUS  Status;
  UINT32      RemainingSize;
  UINT8       *CurrentLocation;
  UINT32      NameStringLength;

  CurrentLocation = *Buffer;
  RemainingSize = *Size;
  NameStringLength = (UINT32)AsciiStrnLenS (NameString, RemainingSize);
  // Locate NameString
  while (RemainingSize > 0) {
    Status = ScanAcpiTableNameString ((VOID **)&CurrentLocation,
                                      &RemainingSize,
                                      NameString);
    if (EFI_ERROR (Status)) {
      ASSERT_EFI_ERROR (Status);
      return Status;
    }
    // Look backwards for Op Code
    if (*(CurrentLocation - 1) == AML_NAME_OP) {
      // Point to DataRefObject and set size
      CurrentLocation += NameStringLength;
      *Buffer = CurrentLocation;
      *Size = RemainingSize - NameStringLength;
      return EFI_SUCCESS;
    } else {
      // Not DeviceOp, so skip NameString and keep searching.
      CurrentLocation += NameStringLength;
      RemainingSize -= NameStringLength;
    }
    CurrentLocation++;
    RemainingSize--;
  }
  ASSERT (FALSE);
  return EFI_NOT_FOUND;
}

/*
  Locate ACPI Buffer ByteList

  DefBuffer := BufferOp PkgLength BufferSize ByteList
  BufferOp := 0x11
  BufferSize := TermArg => Integer

TermArg := Type2Opcode | DataObject | ArgObj | LocalObj

  @param(in,out)  Buffer      Current ACPI Table Pointer
  @param(in,out)  Size        Current Size of ACPI Table
  @param(in)      NameString  NameString to Locate

  @retval EFI_SUCCESS       Requested Scope found.
  @retval EFI_NOT_FOUND     Requested Scope not found.
*/
EFI_STATUS
EFIAPI
LocateAcpiDefBufferByteList (
  IN OUT  VOID    **Buffer,
  IN OUT  UINT32  *Size
  )
{
  UINT32      RemainingSize;
  UINT8       *CurrentLocation;
  UINT32      PkgLength;
  UINT32      PkgLengthSize;

  CurrentLocation = *Buffer;
  RemainingSize = *Size;
  if (*CurrentLocation != AML_BUFFER_OP) {
    ASSERT (FALSE);
    return EFI_NOT_FOUND;
  }
  CurrentLocation++;
  RemainingSize--;

  // Get PackageLength and number PkgLenght Bytes
  PkgLengthSize = AmlGetPkgLength (CurrentLocation, &PkgLength);
  CurrentLocation += PkgLengthSize;
  RemainingSize -= PkgLengthSize;

  // Get Buffer Size and point to ByteList
  CurrentLocation += 1;
  switch (*(CurrentLocation - 1)) {
    case AML_BYTE_PREFIX:
      RemainingSize = *CurrentLocation;
      CurrentLocation += sizeof (UINT8);
      break;
    case AML_WORD_PREFIX:
      RemainingSize = *(UINT16 *)CurrentLocation;
      CurrentLocation += sizeof (UINT16);
      break;
    case AML_DWORD_PREFIX:
      RemainingSize = *(UINT32 *)CurrentLocation;
      CurrentLocation += sizeof (UINT32);
      break;
    default:
      ASSERT (FALSE);
      return EFI_NOT_FOUND;
  }
  *Buffer = CurrentLocation;
  *Size = RemainingSize;

  return EFI_SUCCESS;
}

/*
  Locate ACPI DefName Buffer ByteList

  @param(in,out)  Buffer      Current ACPI Table Pointer
  @param(in,out)  Size        Current Size of ACPI Table
  @param(in)      NameString  NameString to Locate

  @retval EFI_SUCCESS       Requested Scope found.
  @retval EFI_NOT_FOUND     Requested Scope not found.
*/
EFI_STATUS
EFIAPI
LocateAcpiDefNameDefBufferByteList (
  IN OUT  VOID    **Buffer,
  IN OUT  UINT32  *Size,
  IN      CHAR8   *NameString
  )
{
  EFI_STATUS  Status;

  Status = LocateAcpiDefNameDataRefObject (
              Buffer,
              Size,
              NameString
              );
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }
  Status = LocateAcpiDefBufferByteList (Buffer, Size);
  ASSERT_EFI_ERROR (Status);
  return Status;
}
