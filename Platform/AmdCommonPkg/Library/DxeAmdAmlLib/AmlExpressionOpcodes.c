/*****************************************************************************
 *
 * Copyright (C) 2020-2024 Advanced Micro Devices, Inc. All rights reserved.
 *
 *****************************************************************************/

#include "InternalAmlLib.h"


// ----------------------------------------------------------------------------
//  Expression Opcodes Encoding
// ----------------------------------------------------------------------------
//   ExpressionOpcode := DefAcquire | DefAdd | DefAnd | DefBuffer | DefConcat |
//     DefConcatRes | DefCondRefOf | DefCopyObject | DefDecrement |
//     DefDerefOf | DefDivide | DefFindSetLeftBit | DefFindSetRightBit |
//     DefFromBCD | DefIncrement | DefIndex | DefLAnd | DefLEqual |
//     DefLGreater | DefLGreaterEqual | DefLLess | DefLLessEqual | DefMid |
//     DefLNot | DefLNotEqual | DefLoadTable | DefLOr | DefMatch | DefMod |
//     DefMultiply | DefNAnd | DefNOr | DefNot | DefObjectType | DefOr |
//     DefPackage | DefVarPackage | DefRefOf | DefShiftLeft | DefShiftRight |
//     DefSizeOf | DefStore | DefSubtract | DefTimer | DefToBCD | DefToBuffer |
//     DefToDecimalString | DefToHexString | DefToInteger | DefToString |
//     DefWait | DefXOr | MethodInvocation
// ----------------------------------------------------------------------------

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
      // Start Store expression
      Status = InternalAppendNewAmlObject (&Object, "STORE", ListHead);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: ERROR: Cannot append STORE object\n", __FUNCTION__));
        goto Done;
      }
      // TermArg and SuperName are outside the scope of this object.  They must be
      // defined as part of a multi-tier call - in between AmlStore(AmlStart,..) and
      // AmlStore(AmlClose,...) - when creating the Store expression.
  
      break;

    case AmlClose:
      // TermArg and SuperName must have been created and closed by now.
      Status = InternalAmlLocateObjectByIdentifier (&Object, "STORE", ListHead);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: ERROR: locating STORE Object\n", __FUNCTION__));
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
        DEBUG ((DEBUG_ERROR, "%a: ERROR: Store() has no child data.\n", __FUNCTION__));
        goto Done;
      }

      Object->Data = AllocateZeroPool (ChildObject->DataSize + 1);
      if (Object->Data == NULL) {
        Status = EFI_OUT_OF_RESOURCES;
        DEBUG ((DEBUG_ERROR, "%a: ERROR: allocate Object->Data for Store()\n", __FUNCTION__));
        goto Done;
      }

      // Store Op is one byte
      Object->DataSize = ChildObject->DataSize + 1;

      // Fill out Store object
      Object->Data[0] = AML_STORE_OP;
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
