/*****************************************************************************
 *
 * Copyright (C) 2020-2024 Advanced Micro Devices, Inc. All rights reserved.
 *
 *****************************************************************************/

#include "InternalAmlLib.h"

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
)
{
  LIST_ENTRY            *Node;
  AML_OBJECT_INSTANCE   *Object;

  Node = GetNextNode (ListHead, ListHead);
  while (Node != ListHead) {
    Object = AML_OBJECT_INSTANCE_FROM_LINK (Node);
    // Get next node before freeing current Object
    Node = GetNextNode(ListHead, Node);
    // Free Object
    InternalFreeAmlObject(&Object, ListHead);
  }
  return EFI_SUCCESS;
}