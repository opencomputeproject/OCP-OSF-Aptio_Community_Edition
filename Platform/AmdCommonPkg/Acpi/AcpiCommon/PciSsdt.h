/*****************************************************************************
 *
 * Copyright (C) 2020-2023 Advanced Micro Devices, Inc. All rights reserved.
 *
 *****************************************************************************/

#ifndef __PCISSDT_H__
#define __PCISSDT_H__
#include "AcpiCommon.h"
#include <Library/PcdLib.h>
#include <Register/AmdIoApic.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Library/PcieResourcesLib.h>

#define OEM_REVISION_NUMBER   1
#define CREATOR_REVISION      2
#define FCH_IOAPIC_ADDRESS    0xFEC00000
#define NBIO_MAX_ROOT_PORTS   19
#define MAX_PCI_BUS_NUMBER_PER_SEGMENT 0x100


#define PCI_ROOT_BRIDGE_OBJECT_INSTANCE_SIGNATURE SIGNATURE_32 ('a', 'p', 'r', 'b')
typedef struct {
  UINT32                            Signature;
  EFI_HANDLE                        Handle;
  UINTN                             Uid;
  UINTN                             GlobalInterruptStart;
  UINTN                             IoapicInterruptStart;
  UINTN                             InterruptCount;
  VOID                              *Configuration; // Never free this buffer
  PCI_ROOT_BRIDGE_OBJECT            *Object;        // Never free this object
  LIST_ENTRY                        Link;
} PCI_ROOT_BRIDGE_OBJECT_INSTANCE;

#define SSDT_PCI_ROOT_BRIDGE_INSTANCE_FROM_LINK(a) \
        CR (a, PCI_ROOT_BRIDGE_OBJECT_INSTANCE, Link, \
        PCI_ROOT_BRIDGE_OBJECT_INSTANCE_SIGNATURE)

#endif // __PCISSDT_H__
