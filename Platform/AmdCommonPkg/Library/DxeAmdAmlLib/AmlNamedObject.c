/*****************************************************************************
 *
 * Copyright (C) 2020-2024 Advanced Micro Devices, Inc. All rights reserved.
 *
 *****************************************************************************/

#include "InternalAmlLib.h"
#include <Library/PrintLib.h>

#define    METHOD_ARGS_MAX      7
#define    MAX_SYNC_LEVEL       0x0F

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
)
{
  EFI_STATUS          Status;
  AML_OBJECT_INSTANCE *Object;
  AML_OBJECT_INSTANCE *ChildObject;
  UINTN               ChildCount;

  if (Phase >= AmlInvalid || ListHead == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status      = EFI_DEVICE_ERROR;
  Object      = NULL;
  ChildObject = NULL;

  switch (Phase)
  {
    case AmlStart:
      // Start CreateDWordField expression
      Status = InternalAppendNewAmlObject (&Object, "CREATEDWORDFIELD", ListHead);
      if (EFI_ERROR (Status)) {
        DEBUG ((
          DEBUG_ERROR,
          "%a: ERROR: Cannot append CREATEDWORDFIELD object\n",
          __FUNCTION__
          ));
        goto Done;
      }
      // SourceBuffer, ByteIndex, and DWordFieldName are outside the scope of this
      // object.  They must be defined as part of a multi-tier call - in between
      // AmlCreateDWordField (AmlStart,..) and AmlCreateDWordField (AmlClose,...) -
      // when creating the CreateDWordField expression.

      break;

    case AmlClose:
      // SourceBuffer, ByteIndex, DWordFieldName must have been created and closed by now.
      Status = InternalAmlLocateObjectByIdentifier (&Object, "CREATEDWORDFIELD", ListHead);
      if (EFI_ERROR (Status)) {
        DEBUG ((
          DEBUG_ERROR,
          "%a: ERROR: locating CREATEDWORDFIELD Object\n",
          __FUNCTION__
          ));
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
      if (EFI_ERROR (Status) ||
          ChildObject->Data == NULL ||
          ChildObject->DataSize == 0) {
        DEBUG ((
          DEBUG_ERROR,
          "%a: ERROR: CreateDWordField () has no child data.\n",
          __FUNCTION__
          ));
        goto Done;
      }

      Object->Data = AllocateZeroPool (ChildObject->DataSize + 1);
      if (Object->Data == NULL) {
        Status = EFI_OUT_OF_RESOURCES;
        DEBUG ((
          DEBUG_ERROR,
          "%a: ERROR: allocate Object->Data for CreateDWordField()\n",
          __FUNCTION__
          ));
        goto Done;
      }

      // CreateDWordField Op is one byte
      Object->DataSize = ChildObject->DataSize + 1;

      // Fill out CreateDWordField object
      Object->Data[0] = AML_CREATE_DWORD_FIELD_OP;
      CopyMem (&Object->Data[1],
               ChildObject->Data,
               ChildObject->DataSize);
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
    InternalFreeAmlObject (&Object, ListHead);
    InternalFreeAmlObject (&ChildObject, ListHead);
  }
  return Status;
}

/**
  Creates a Device (ObjectName, Object)

  Object must be created between AmlStart and AmlClose Phase

  DefName  := DeviceOp PkgLength NameString TermList
  NameOp   := ExtOpPrefix 0x82
  ExtOpPrefix  := 0x5B

  @param[in]      Phase     - Either AmlStart or AmlClose
  @param[in]      String    - Object name
  @param[in,out]  ListHead  - Linked list has completed String Object after
                              AmlClose.

  @retval         EFI_SUCCESS
  @retval         Error status
**/
EFI_STATUS
EFIAPI
AmlDevice (
  IN      AML_FUNCTION_PHASE  Phase,
  IN      CHAR8               *String,
  IN OUT  LIST_ENTRY          *ListHead
)
{
  EFI_STATUS          Status;
  AML_OBJECT_INSTANCE *Object;
  AML_OBJECT_INSTANCE *ChildObject;
  UINTN               ChildCount;

  if (Phase >= AmlInvalid || String == NULL || ListHead == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = EFI_DEVICE_ERROR;
  Object = NULL;
  ChildObject = NULL;

  switch (Phase)
  {
  case AmlStart:
    Status = InternalAppendNewAmlObject (&Object, String, ListHead);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: ERROR: Start Device for %a object\n", __FUNCTION__, String));
      goto Done;
    }

    // Start required PkgLength
    Status = AmlPkgLength (AmlStart, ListHead);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: ERROR: Start PkgLength for %a object\n", __FUNCTION__, String));
      goto Done;
    }

    // Insert required NameString
    Status = AmlOPNameString (String, ListHead);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: ERROR: Insert NameString for %a object\n", __FUNCTION__, String));
      goto Done;
    }

    // TermList is too complicated and must be added outside
    break;

  case AmlClose:
    // TermList should be closed already

    // Close required PkgLength before finishing Object
    Status = AmlPkgLength (AmlClose, ListHead);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: ERROR: Close PkgLength for %a object\n", __FUNCTION__, String));
      goto Done;
    }

    Status = InternalAmlLocateObjectByIdentifier (&Object, String, ListHead);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: ERROR: Locate %a object\n", __FUNCTION__, String));
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
    if (EFI_ERROR (Status) ||
        ChildObject->Data == NULL ||
        ChildObject->DataSize == 0) {
      DEBUG ((DEBUG_ERROR, "%a: ERROR: %a child data collection.\n", __FUNCTION__, String));
      goto Done;
    }

    // Device Op is two bytes
    Object->DataSize = ChildObject->DataSize + 2;
    Object->Data = AllocatePool (Object->DataSize);
    if (Object->Data == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      DEBUG ((DEBUG_ERROR, "%a: ERROR: allocate Object->Data for %a\n", __FUNCTION__, String));
      goto Done;
    }
    Object->Data[0] = AML_EXT_OP;
    Object->Data[1] = AML_EXT_DEVICE_OP;
    CopyMem (&Object->Data[2],
             ChildObject->Data,
             ChildObject->DataSize);
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
    InternalFreeAmlObject (&Object, ListHead);
    InternalFreeAmlObject (&ChildObject, ListHead);
  }
  return Status;
}

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
                                that MethodObj.
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
)
{
  EFI_STATUS          Status;
  AML_OBJECT_INSTANCE *Object;
  AML_OBJECT_INSTANCE *ChildObject;
  UINTN               ChildCount;


  if (Name == NULL ||
      NumArgs > METHOD_ARGS_MAX ||
      ObjectType >= InvalidObj ||
      ListHead == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = EFI_DEVICE_ERROR;
  Object = NULL;
  ChildObject = NULL;

  // Start EXTERNAL object
  Status = InternalAppendNewAmlObject (&Object, "EXTERNAL", ListHead);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: ERROR: Start %a object\n", __FUNCTION__, Name));
    goto Done;
  }

  // Insert required NameString
  Status = AmlOPNameString (Name, ListHead);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: ERROR: Start %a NameString object\n", __FUNCTION__, Name));
    goto Done;
  }


  Status = InternalAmlLocateObjectByIdentifier (&Object, "EXTERNAL", ListHead);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: ERROR: Locate %a object\n", __FUNCTION__, Name));
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
  if (EFI_ERROR (Status) ||
      ChildObject->Data == NULL ||
      ChildObject->DataSize == 0) {
    DEBUG ((DEBUG_ERROR, "%a: ERROR: %a has no child data.\n", __FUNCTION__, Name));
    goto Done;
  }

  Object->Data = AllocateZeroPool (ChildObject->DataSize + 3);
  // AML_EXTERNAL_OP + Name + ObjectType + ArgumentCount
  if (Object->Data == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    DEBUG ((DEBUG_ERROR, "%a: ERROR: allocate Object->Data for %a\n", __FUNCTION__, Name));
    goto Done;
  }

  Object->DataSize = 0;
  Object->Data[0] = AML_EXTERNAL_OP;
  Object->DataSize++;
  CopyMem (&Object->Data[Object->DataSize],
           ChildObject->Data,
           ChildObject->DataSize);
  Object->DataSize += ChildObject->DataSize;
  Object->Data[Object->DataSize] = ObjectType;
  Object->DataSize++;
  Object->Data[Object->DataSize] = NumArgs;
  Object->DataSize++;


  InternalFreeAmlObject (&ChildObject, ListHead);
  Object->Completed = TRUE;
  Status = EFI_SUCCESS;

  Done:
  if (EFI_ERROR (Status)) {
    InternalFreeAmlObject (&ChildObject, ListHead);
    InternalFreeAmlObject (&Object, ListHead);
  }
  return Status;
}

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
)
{
  EFI_STATUS          Status;
  AML_OBJECT_INSTANCE *Object;
  AML_OBJECT_INSTANCE *ChildObject;
  UINT8               MethodFlags;
  UINTN               ChildCount;

  if (Phase >= AmlInvalid ||
      Name == NULL ||
      NumArgs > METHOD_ARGS_MAX ||
      SyncLevel > MAX_SYNC_LEVEL ||
      SerializeRule >= FlagInvalid ||
      ListHead == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = EFI_DEVICE_ERROR;
  Object = NULL;
  ChildObject = NULL;

  switch (Phase)
  {
  case AmlStart:
    Status = InternalAppendNewAmlObject (&Object, "Method", ListHead);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: ERROR: Start Method for %a object\n", __FUNCTION__, Name));
      goto Done;
    }

    // Start required PkgLength
    Status = AmlPkgLength (AmlStart, ListHead);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: ERROR: Start PkgLength for %a object\n", __FUNCTION__, Name));
      goto Done;
    }

    // Insert required NameString
    Status = AmlOPNameString (Name, ListHead);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: ERROR: Start NameString for %a object\n", __FUNCTION__, Name));
      goto Done;
    }

    // Add Method Flags
    Status = InternalAppendNewAmlObject (&Object, "METHOD_FLAGS", ListHead);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: ERROR: Start METHOD_FLAGS for %a object\n", __FUNCTION__, Name));
      goto Done;
    }

    // TermList is too complicated and must be added outside
    break;

  case AmlClose:
    // TermList should be closed already
    // Add Method Flags
    Status = InternalAmlLocateObjectByIdentifier (&Object, "METHOD_FLAGS", ListHead);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: ERROR: locate METHOD_FLAGS for %a object\n", __FUNCTION__, Name));
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
      DEBUG ((DEBUG_ERROR, "%a: ERROR: %a METHOD_FLAGS child data collection.\n", __FUNCTION__, Name));
      goto Done;
    }
    // Method Flags is one byte
    Object->DataSize = ChildObject->DataSize + 1;
    Object->Data = AllocatePool (Object->DataSize);
    if (Object->Data == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      DEBUG ((DEBUG_ERROR, "%a: ERROR: allocate Object->Data for %a\n", __FUNCTION__, Name));
      goto Done;
    }
    MethodFlags = NumArgs & 0x07;
    if (SerializeRule) {
      MethodFlags |= BIT3;
    }
    MethodFlags |= (SyncLevel & 0x0F) << 4;
    Object->Data[0] = MethodFlags;
    CopyMem (&Object->Data[1],
             ChildObject->Data,
             ChildObject->DataSize);
    InternalFreeAmlObject (&ChildObject, ListHead);
    Object->Completed = TRUE;

    // Required NameString completed in one phase call

    // Close required PkgLength before finishing Object
    Status = AmlPkgLength (AmlClose, ListHead);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: ERROR: Close PkgLength for %a object\n", __FUNCTION__, Name));
      goto Done;
    }

    Status = InternalAmlLocateObjectByIdentifier (&Object, "Method", ListHead);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: ERROR: Locate %a object\n", __FUNCTION__, Name));
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
    if (EFI_ERROR (Status) ||
        ChildObject->Data == NULL ||
        ChildObject->DataSize == 0) {
      DEBUG ((DEBUG_ERROR, "%a: ERROR: %a child data collection.\n", __FUNCTION__, Name));
      goto Done;
    }

    // Method Op is one byte
    Object->DataSize = ChildObject->DataSize + 1;
    Object->Data = AllocatePool (Object->DataSize);
    if (Object->Data == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      DEBUG ((DEBUG_ERROR, "%a: ERROR: allocate Object->Data for %a\n", __FUNCTION__, Name));
      goto Done;
    }

    Object->Data[0] = AML_METHOD_OP;
    CopyMem (&Object->Data[1],
             ChildObject->Data,
             ChildObject->DataSize);
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
