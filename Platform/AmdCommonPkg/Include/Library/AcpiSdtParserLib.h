/******************************************************************************
  Copyright (C) 2020-2024 Advanced Micro Devices, Inc. All rights reserved.

 *****************************************************************************/

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
  );

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
  );

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
  );

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
  );
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
  );
