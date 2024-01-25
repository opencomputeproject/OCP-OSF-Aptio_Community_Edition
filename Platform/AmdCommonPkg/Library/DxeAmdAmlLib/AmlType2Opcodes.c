/*****************************************************************************
 *
 * Copyright (C) 2020-2023 Advanced Micro Devices, Inc. All rights reserved.
 *
 *****************************************************************************/

#include "InternalAmlLib.h"

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
)
{
  EFI_STATUS          Status;
  AML_OBJECT_INSTANCE *Object;
  AML_OBJECT_INSTANCE *ChildObject;
  UINTN               ChildCount;
  UINTN               InternalBufferSize;

  Status = EFI_DEVICE_ERROR;
  Object = NULL;
  ChildObject = NULL;

  if (Phase >= AmlInvalid || ListHead == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  // iASL compiler 20200110 only keeps lower 32 bits of size.  We'll error if
  // someone requests something >= 4GB size.  Have a message with this to be
  // very specific
  if (BufferSize >= SIZE_4GB) {
    DEBUG ((DEBUG_ERROR, "%a: ERROR: BufferSize=0x%X >= 4GB\n", __FUNCTION__, BufferSize));
    return EFI_INVALID_PARAMETER;
  }

  switch (Phase)
  {
  case AmlStart:
    // Start the Buffer Object
    Status = InternalAppendNewAmlObject (&Object, "BUFFER", ListHead);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: ERROR: Start BUFFER object\n", __FUNCTION__));
      goto Done;
    }

    // Start required PkgLength
    Status = AmlPkgLength (AmlStart, ListHead);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: ERROR: Buffer PkgLength object\n", __FUNCTION__));
      goto Done;
    }

    // Start BufferSize
    Status = InternalAppendNewAmlObject (&Object, "BUFFERSIZE", ListHead);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: ERROR: Start BUFFERSIZE object\n", __FUNCTION__));
      goto Done;
    }

    // ByteList is too complicated and must be added outside
    break;

  case AmlClose:
    // ByteList items should be closed already

    // Close BufferSize
    Status = InternalAmlLocateObjectByIdentifier (&Object,
                                                  "BUFFERSIZE",
                                                  ListHead);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: ERROR: Fail locate BufferSize object\n", __FUNCTION__));
      goto Done;
    }

    // Get rid of original Identifier data
    InternalFreeAmlObjectData (Object);
    // Collect child data and delete children
    Status = InternalAmlCollapseAndReleaseChildren (
                    &ChildObject,
                    &ChildCount,
                    &Object->Link,
                    ListHead
                    );
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: ERROR: collect BufferSize children\n", __FUNCTION__));
      goto Done;
    }

    // Set BufferSize Object to correct value and size.
    // BufferSize should be from zero (no Child Data) to MAX of requested
    // BufferSize or size required for ChildObject->Data.
    InternalBufferSize = MAX (BufferSize, ChildObject->DataSize);
    // iASL compiler 20200110 only keeps lower 32 bits of size.  We'll error if
    // someone requests something >= 4GB size.
    if (InternalBufferSize >= SIZE_4GB) {
      Status = EFI_BAD_BUFFER_SIZE;
      DEBUG ((DEBUG_ERROR, "%a: ERROR: BufferSize 0x%X >= 4GB\n", __FUNCTION__,
               InternalBufferSize));
      goto Done;
    }
    Status = InternalAmlDataIntegerBuffer (
                InternalBufferSize,
                (VOID **)&Object->Data,
                &Object->DataSize
                );
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: ERROR: calc BufferSize\n", __FUNCTION__));
      goto Done;
    }

    if (ChildObject->DataSize !=0 && ChildObject->Data != NULL) {
      // Make room for ChildObject->Data
      Object->Data = ReallocatePool (Object->DataSize,
                                     Object->DataSize +
                                       ChildObject->DataSize,
                                     Object->Data);
      if (Object->Data == NULL) {
        DEBUG ((DEBUG_ERROR, "%a: ERROR: to reallocate BufferSize\n", __FUNCTION__));
        Status = EFI_OUT_OF_RESOURCES;
        goto Done;
      }
      CopyMem (&Object->Data[Object->DataSize],
               ChildObject->Data,
               ChildObject->DataSize);
      Object->DataSize += ChildObject->DataSize;
    }
    // Free Child Object since it has been consumed
    InternalFreeAmlObject (&ChildObject, ListHead);
    Object->Completed = TRUE;

    // Close required PkgLength before finishing Object
    Status = AmlPkgLength (AmlClose, ListHead);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: ERROR: close PkgLength object\n", __FUNCTION__));
      goto Done;
    }

    // Complete Buffer object with all data
    Status = InternalAmlLocateObjectByIdentifier (&Object, "BUFFER", ListHead);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: ERROR: locate Buffer object\n", __FUNCTION__));
      goto Done;
    }

    // Get rid of original Identifier data
    InternalFreeAmlObjectData (Object);
    // Collect child data and delete children
    Status = InternalAmlCollapseAndReleaseChildren (&ChildObject,
                                                    &ChildCount,
                                                    &Object->Link,
                                                    ListHead);

    // Buffer must have at least PkgLength BufferSize
    if (EFI_ERROR (Status) ||
        ChildObject->Data == NULL ||
        ChildObject->DataSize == 0) {
      DEBUG ((DEBUG_ERROR, "%a: ERROR: No Buffer Data\n", __FUNCTION__));
      goto Done;
    }

    //  BufferOp is one byte
    Object->DataSize = ChildObject->DataSize + 1;
    Object->Data = AllocatePool (Object->DataSize);
    if (Object->Data == NULL) {
      DEBUG ((DEBUG_ERROR, "%a: ERROR: Buffer allocate failed\n", __FUNCTION__));
      Status = EFI_OUT_OF_RESOURCES;
      goto Done;
    }

    Object->Data[0] = AML_BUFFER_OP;
    CopyMem (&Object->Data[1],
             ChildObject->Data,
             ChildObject->DataSize);
    // Free Child Object since it has been consumed
    InternalFreeAmlObject (&ChildObject, ListHead);
    Object->Completed = TRUE;

    Status = EFI_SUCCESS;
    break;

  default:
    Status = EFI_DEVICE_ERROR;
    break;
  }

Done:
  if (EFI_ERROR (Status)) {
    InternalFreeAmlObject (&ChildObject, ListHead);
    InternalFreeAmlObject (&Object, ListHead);
  }
  return Status;
}

/*
  Creates (NumElements) section of a Package: {PackageList} => Package

  Initializers must be created between AmlStart and AmlClose Phase
  Internal only function no public reference or documentation needed.

  NumElements        := ByteData
  VarNumElements     := TermArg => Integer
  PackageElementList := Nothing | <PackageElement PackageElementList>
  PackageElement     := DataRefObject | NameString

  @param[in]      Phase       - Either AmlStart or AmlClose
  @param[in,out]  NumElements - Number of elements in the package. If 0, size
                                is calculated from the PackageList.
  @param[in,out]  ListHead    - Linked list has completed Package Object after
                                AmlClose.

  @retval         EFI_SUCCESS
  @retval         Error status
*/
EFI_STATUS
EFIAPI
InternalAmlNumElements (
  IN      AML_FUNCTION_PHASE  Phase,
  IN OUT  UINTN               *NumElements,
  IN OUT  LIST_ENTRY          *ListHead
)
{
  EFI_STATUS          Status;
  AML_OBJECT_INSTANCE *Object;
  AML_OBJECT_INSTANCE *ChildObject;
  UINTN               ChildCount;

  Status = EFI_DEVICE_ERROR;
  Object = NULL;
  ChildObject = NULL;
  ChildCount = 0;

  if (Phase >= AmlInvalid || ListHead == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  switch (Phase) {
  case AmlStart:
    // Start Number of Elements Object
    Status = InternalAppendNewAmlObject (&Object, "NUM_ELEMENTS", ListHead);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: ERROR: Start NUM_ELEMENTS object\n", __FUNCTION__));
      goto Done;
    }

    // PackageList is too complicated and must be added outside
    break;

  case AmlClose:
    // PackageList items should be closed already

    // Close Number of Elements Object
    Status = InternalAmlLocateObjectByIdentifier (
               &Object,
               "NUM_ELEMENTS",
               ListHead
               );
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: ERROR: Fail locate NUM_ELEMENTS object\n", __FUNCTION__));
      goto Done;
    }

    // Get rid of original Identifier data
    InternalFreeAmlObjectData (Object);
    // Collect child data and delete children
    Status = InternalAmlCollapseAndReleaseChildren (
               &ChildObject,
               &ChildCount,
               &Object->Link,
               ListHead
               );
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: ERROR: collect NUM_ELEMENTS children\n", __FUNCTION__));
      goto Done;
    }

    // We don't have to change anything for NumElements >= Child Count
    if (*NumElements == 0) {
      *NumElements = ChildCount;
    } else if (*NumElements < ChildCount) {
      DEBUG ((DEBUG_ERROR, "%a: ERROR: NumElements < ChildCount.\n", __FUNCTION__));
      Status = EFI_INVALID_PARAMETER;
      goto Done;
    }

    if (*NumElements <= MAX_UINT8) {
      Object->DataSize = 1;
      Object->Data = AllocateZeroPool(Object->DataSize);
      if (Object->Data == NULL) {
        DEBUG ((DEBUG_ERROR, "%a: ERROR: NumElements allocate failed\n", __FUNCTION__));
        Status = EFI_OUT_OF_RESOURCES;
        goto Done;
      }
      Object->Data[0] = (UINT8)*NumElements;
    } else {
      Status = InternalAmlDataIntegerBuffer (
                 *NumElements,
                 (VOID **)&Object->Data,
                 &Object->DataSize
                 );
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: ERROR: calc NumElements\n", __FUNCTION__));
        goto Done;
      }
    }

    if (ChildObject->DataSize !=0 && ChildObject->Data != NULL) {
      // Make room for ChildObject->Data
      Object->Data = ReallocatePool (
                       Object->DataSize,
                       Object->DataSize +
                       ChildObject->DataSize,
                       Object->Data
                       );
      if (Object->Data == NULL) {
        DEBUG ((DEBUG_ERROR, "%a: ERROR: to reallocate NumElements\n", __FUNCTION__));
        Status = EFI_OUT_OF_RESOURCES;
        goto Done;
      }
      CopyMem (
        &Object->Data[Object->DataSize],
        ChildObject->Data,
        ChildObject->DataSize
        );
      Object->DataSize += ChildObject->DataSize;
    }

    // Free Child Object since it has been consumed
    InternalFreeAmlObject (&ChildObject, ListHead);
    Object->Completed = TRUE;

    Status = EFI_SUCCESS;
    break;

  default:
    Status = EFI_DEVICE_ERROR;
    break;
  }

Done:
  if (EFI_ERROR (Status)) {
    InternalFreeAmlObject (&ChildObject, ListHead);
    InternalFreeAmlObject (&Object, ListHead);
  }
  return Status;
}

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
  @param[in]      NumElements - Number of elements in the package. If 0, size
                                is calculated from the PackageList.
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
)
{
  EFI_STATUS          Status;
  AML_OBJECT_INSTANCE *Object;
  AML_OBJECT_INSTANCE *ChildObject;
  UINTN               ChildCount;
  UINT8               OpCode;

  Status = EFI_DEVICE_ERROR;
  Object = NULL;
  ChildObject = NULL;

  if (Phase >= AmlInvalid || ListHead == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  switch (Phase)
  {
  case AmlStart:
    // Start the Package Object
    Status = InternalAppendNewAmlObject (&Object, "PACKAGE", ListHead);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: ERROR: Start PACKAGE object\n", __FUNCTION__));
      goto Done;
    }

    // Start required PkgLength
    Status = AmlPkgLength (AmlStart, ListHead);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: ERROR: Package PkgLength object\n", __FUNCTION__));
      goto Done;
    }

    // Start Number of Elements Object
    Status = InternalAmlNumElements (AmlStart, &NumElements, ListHead);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: ERROR: Start NUM_ELEMENTS object\n", __FUNCTION__));
      goto Done;
    }

    // PackageList is too complicated and must be added outside
    break;

  case AmlClose:
    // PackageList items should be closed already

    // Close Number of Elements Object
    Status = InternalAmlNumElements (AmlClose, &NumElements, ListHead);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: ERROR: Close NUM_ELEMENTS object\n", __FUNCTION__));
      goto Done;
    }

    if (NumElements <= MAX_UINT8) {
      OpCode = AML_PACKAGE_OP;
    } else {
      OpCode = AML_VAR_PACKAGE_OP;
    }

    // Close required PkgLength before finishing Object
    Status = AmlPkgLength (AmlClose, ListHead);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: ERROR: close PkgLength object\n", __FUNCTION__));
      goto Done;
    }

    // Complete Package object with all data
    Status = InternalAmlLocateObjectByIdentifier (&Object, "PACKAGE", ListHead);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: ERROR: locate PACKAGE object\n", __FUNCTION__));
      goto Done;
    }


    // Get rid of original Identifier data
    InternalFreeAmlObjectData (Object);
    // Collect child data and delete children
    Status = InternalAmlCollapseAndReleaseChildren (&ChildObject,
                                                    &ChildCount,
                                                    &Object->Link,
                                                    ListHead);
    // Package must have at least PkgLength NumElements
    if (EFI_ERROR (Status) ||
        ChildObject->Data == NULL ||
        ChildObject->DataSize == 0) {
      DEBUG ((DEBUG_ERROR, "%a: ERROR: No Package Data\n", __FUNCTION__));
      goto Done;
    }

    //  PackageOp and VarPackageOp are both one byte
    Object->DataSize = ChildObject->DataSize + 1;
    Object->Data = AllocatePool (Object->DataSize);
    if (Object->Data == NULL) {
      DEBUG ((DEBUG_ERROR, "%a: ERROR: Package allocate failed\n", __FUNCTION__));
      Status = EFI_OUT_OF_RESOURCES;
      goto Done;
    }

    Object->Data[0] = OpCode;
    CopyMem (&Object->Data[1],
             ChildObject->Data,
             ChildObject->DataSize);
    // Free Child Object since it has been consumed
    InternalFreeAmlObject (&ChildObject, ListHead);
    Object->Completed = TRUE;

    Status = EFI_SUCCESS;
    break;

  default:
    Status = EFI_DEVICE_ERROR;
    break;
  }

Done:
  if (EFI_ERROR (Status)) {
    InternalFreeAmlObject (&ChildObject, ListHead);
    InternalFreeAmlObject (&Object, ListHead);
  }
  return Status;
}
