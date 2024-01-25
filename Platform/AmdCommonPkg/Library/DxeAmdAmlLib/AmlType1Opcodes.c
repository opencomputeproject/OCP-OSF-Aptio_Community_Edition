/*****************************************************************************
 *
 * Copyright (C) 2020-2023 Advanced Micro Devices, Inc. All rights reserved.
 *
 *****************************************************************************/

#include "InternalAmlLib.h"


/**
  Creates a Return object

  Object must be created between AmlStart and AmlClose Phase

  DefReturn  := ReturnOp ArgObject
  ReturnOp   := 0xA4
  ArgObject  := TermArg => DataRefObject

  @param[in]      Phase     - Either AmlStart or AmlClose
  @param[in,out]  ListHead  - Linked list has completed String Object after
                              AmlClose.

  @retval         EFI_SUCCESS
  @retval         Error status
**/
EFI_STATUS
EFIAPI
AmlReturn (
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

  Status = EFI_DEVICE_ERROR;
  Object = NULL;
  ChildObject = NULL;

  switch (Phase)
  {
  case AmlStart:
    Status = InternalAppendNewAmlObject (&Object, "Return", ListHead);
    // DataRefObject is outside the scope of this object
    break;
  case AmlClose:
    // DataRefObject should be closed already
    Status = InternalAmlLocateObjectByIdentifier (&Object, "Return", ListHead);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: ERROR: locating Return Object\n", __FUNCTION__));
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
      DEBUG ((DEBUG_ERROR, "%a: ERROR: collecting Child data\n", __FUNCTION__));
      goto Done;
    }

    // Handle Return with no arguments
    if(ChildObject->Data == NULL || ChildObject->DataSize == 0) {
      // Return without arguments is treated like Return(0)
      // Zeroed byte = ZeroOp
      ChildObject->Data = AllocateZeroPool(sizeof(UINT8));
      if(ChildObject->Data == NULL) {
        Status = EFI_OUT_OF_RESOURCES;
        DEBUG ((DEBUG_ERROR, "%a: ERROR: allocate Zero Child for Return\n", __FUNCTION__));
        goto Done;
      }
      ChildObject->DataSize = 1;
    }

    // Allocate buffer for Return object
    Object->Data = AllocatePool (ChildObject->DataSize + 1);
    Object->DataSize = ChildObject->DataSize + 1;
    if (Object->Data == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      DEBUG ((DEBUG_ERROR, "%a: ERROR: allocate Object=Return\n", __FUNCTION__));
      goto Done;
    }

    // Fill out Return object
    Object->Data[0] = AML_RETURN_OP;
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
