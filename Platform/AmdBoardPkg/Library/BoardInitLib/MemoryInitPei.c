/******************************************************************************
 * Copyright (C) 2021-2024 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

/* This file includes code originally published under the following license. */

/** @file

  Copyright (c) 2013 - 2016 Intel Corporation.
  Copyright (c) 2016, Linaro Ltd. All rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "MemoryInitPei.h"

//SMRAM Size
#define SMRAM_SIZE ((UINT64)SIZE_64MB)
//
// 640K-to-1MB legacy ROM space
//
#define LEGACY_ROM_BASE         0xA0000UL
#define LEGACY_ROM_SIZE         0x60000UL

EFI_STATUS
MemoryInit (
  IN      EFI_PEI_SERVICES   **PeiServices
  )
{
  EFI_STATUS                        Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI   *VariableServices;
  EFI_BOOT_MODE                     BootMode;

  //
  // Get necessary PPI
  //
  Status = PeiServicesLocatePpi(
             &gEfiPeiReadOnlyVariable2PpiGuid,           // GUID
             0,                                          // INSTANCE
             NULL,                                       // EFI_PEI_PPI_DESCRIPTOR
             (VOID **)&VariableServices                  // PPI
             );
  if (EFI_ERROR(Status)) {
    VariableServices = NULL;
  }

  //
  // Determine boot mode
  //
  Status = PeiServicesGetBootMode (&BootMode);
  ASSERT_EFI_ERROR (Status);

  Status = InstallEfiMemory (PeiServices, VariableServices, BootMode);
  ASSERT_EFI_ERROR (Status);

  return Status;
}

#define SYSTEM_MEMORY_ATTRIBUTES (                 \
  EFI_RESOURCE_ATTRIBUTE_PRESENT |                 \
  EFI_RESOURCE_ATTRIBUTE_INITIALIZED |             \
  EFI_RESOURCE_ATTRIBUTE_TESTED |                  \
  EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE |             \
  EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE |       \
  EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE | \
  EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE      \
  )

#define MEMORY_MAPPED_IO_ATTRIBUTES (  \
  EFI_RESOURCE_ATTRIBUTE_PRESENT |     \
  EFI_RESOURCE_ATTRIBUTE_INITIALIZED | \
  EFI_RESOURCE_ATTRIBUTE_TESTED |      \
  EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE   \
  )


EFI_STATUS
InstallEfiMemory (
  IN      EFI_PEI_SERVICES                     **PeiServices,
  IN      EFI_PEI_READ_ONLY_VARIABLE2_PPI      *VariableServices,
  IN      EFI_BOOT_MODE                        BootMode
  )
{
  EFI_STATUS                      Status;
  AMD_MEMORY_INFO_HOB             *AmdMemoryInfoHob;
  AMD_MEMORY_RANGE_DESCRIPTOR     *AmdMemoryInfoRange;
  EFI_HOB_GUID_TYPE               *GuidHob;
  EFI_PEI_HOB_POINTERS            Hob;
  UINTN                           Index;
  EFI_SMRAM_HOB_DESCRIPTOR_BLOCK  *SmramHobDescriptorBlock;
  EFI_PHYSICAL_ADDRESS            SmramBaseAddress;
  UINT8                           SmramRanges;
  UINT8                           MorControl;
  UINTN                           DataSize;

  SmramBaseAddress = 0;
  SmramRanges = 0;

  // Locate AMD_MEMORY_INFO_HOB Guided HOB and retrieve data
  AmdMemoryInfoHob = NULL;
  GuidHob = GetFirstGuidHob (&gAmdMemoryInfoHobGuid);
  if (GuidHob != NULL) {
    AmdMemoryInfoHob = GET_GUID_HOB_DATA (GuidHob);
  }

  if (AmdMemoryInfoHob == NULL) {
    DEBUG ((EFI_D_ERROR, "Error: Could not locate AMD_MEMORY_INFO_HOB.\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  DEBUG ((EFI_D_ERROR, "AMD_MEMORY_INFO_HOB at 0x%X\n", AmdMemoryInfoHob));
  DEBUG ((EFI_D_ERROR, "  Version: 0x%X\n", AmdMemoryInfoHob->Version));
  DEBUG ((EFI_D_ERROR, "  NumberOfDescriptor: 0x%X\n", AmdMemoryInfoHob->NumberOfDescriptor));

  //
  // Detect MOR request by the OS.
  //
  MorControl = 0;
  DataSize = sizeof(MorControl);
  if (VariableServices) {
    Status = VariableServices->GetVariable(
      VariableServices,
      MEMORY_OVERWRITE_REQUEST_VARIABLE_NAME,
      &gEfiMemoryOverwriteControlDataGuid,
      NULL,
      &DataSize,
      &MorControl
      );
    if (EFI_ERROR (Status)) {
      MorControl = 0;
    }
  }

  //
  // Build Descriptors
  //
  DEBUG ((EFI_D_ERROR, "\nAMD HOB Descriptors:"));
  for (Index = 0; Index < AmdMemoryInfoHob->NumberOfDescriptor; Index++) {
    AmdMemoryInfoRange = (AMD_MEMORY_RANGE_DESCRIPTOR*)&(AmdMemoryInfoHob->Ranges[Index]);

    DEBUG ((EFI_D_ERROR, "\n Index: %d\n", Index));
    DEBUG ((EFI_D_ERROR, "   Base: 0x%lX\n", AmdMemoryInfoRange->Base));
    DEBUG ((EFI_D_ERROR, "   Size: 0x%lX\n", AmdMemoryInfoRange->Size));
    DEBUG ((EFI_D_ERROR, "   Attribute: 0x%X\n", AmdMemoryInfoRange->Attribute));

    switch (AmdMemoryInfoRange->Attribute) {
    case AMD_MEMORY_ATTRIBUTE_AVAILABLE:
      //
      // If OS requested a memory overwrite perform it now.
      // Only do it for memory used by the OS.
      //
      //Fixed system hang at POST after abnormal shutdown >>
      //-   if (MOR_CLEAR_MEMORY_VALUE(MorControl))
      //-     ClearMemoryRange(AmdMemoryInfoRange->Base, AmdMemoryInfoRange->Size);
      //Fixed system hang at POST after abnormal shutdown <<

      if (AmdMemoryInfoRange->Base < SIZE_4GB) {
        SmramRanges = 1u;
        // Set SMRAM base at heighest range below 4GB
        SmramBaseAddress = AmdMemoryInfoRange->Base + AmdMemoryInfoRange->Size - SMRAM_SIZE;
        BuildResourceDescriptorHob(
          EFI_RESOURCE_MEMORY_RESERVED,
          (EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE | EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE),
          SmramBaseAddress,
          SMRAM_SIZE
          );
        DEBUG((EFI_D_ERROR, "SMRAM RESERVED_MEMORY: Base = 0x%lX, Size = 0x%lX\n",
          SmramBaseAddress, SMRAM_SIZE));

        AmdMemoryInfoRange->Size -= SMRAM_SIZE;
      }

      if (AmdMemoryInfoRange->Size) {
        BuildResourceDescriptorHob (
          EFI_RESOURCE_SYSTEM_MEMORY,
          SYSTEM_MEMORY_ATTRIBUTES,
          AmdMemoryInfoRange->Base,
          AmdMemoryInfoRange->Size
          );

        DEBUG ((EFI_D_ERROR, "SYSTEM_MEMORY: Base = 0x%lX, Size = 0x%lX\n",
          AmdMemoryInfoRange->Base, AmdMemoryInfoRange->Size));
      }
      break;

    case AMD_MEMORY_ATTRIBUTE_MMIO:
      BuildResourceDescriptorHob (
        EFI_RESOURCE_MEMORY_MAPPED_IO,
        MEMORY_MAPPED_IO_ATTRIBUTES,
        AmdMemoryInfoRange->Base,
        AmdMemoryInfoRange->Size
        );

      DEBUG ((EFI_D_ERROR, "MMIO: Base = 0x%lX, Size = 0x%lX\n",
        AmdMemoryInfoRange->Base, AmdMemoryInfoRange->Size));
      break;

    case AMD_MEMORY_ATTRIBUTE_RESERVED:
    case AMD_MEMORY_ATTRIBUTE_UMA:
    default:
      BuildResourceDescriptorHob(
        EFI_RESOURCE_MEMORY_RESERVED,
        0,
        AmdMemoryInfoRange->Base,
        AmdMemoryInfoRange->Size
        );

      DEBUG ((EFI_D_ERROR, "RESERVED_MEMORY: Base = 0x%lX, Size = 0x%lX\n",
        AmdMemoryInfoRange->Base, AmdMemoryInfoRange->Size));
      break;
    }
  }

  ASSERT (SmramRanges > 0);
  DataSize = sizeof (EFI_SMRAM_HOB_DESCRIPTOR_BLOCK);
  DataSize += ((SmramRanges - 1) * sizeof (EFI_SMRAM_DESCRIPTOR));

  Hob.Raw = BuildGuidHob (
            &gEfiSmmPeiSmramMemoryReserveGuid,
            DataSize
            );
  ASSERT (Hob.Raw);

  SmramHobDescriptorBlock = (EFI_SMRAM_HOB_DESCRIPTOR_BLOCK *) (Hob.Raw);
  SmramHobDescriptorBlock->NumberOfSmmReservedRegions = SmramRanges;
  SmramHobDescriptorBlock->Descriptor[0].PhysicalStart = SmramBaseAddress;
  SmramHobDescriptorBlock->Descriptor[0].CpuStart      = SmramBaseAddress;
  SmramHobDescriptorBlock->Descriptor[0].PhysicalSize  = SMRAM_SIZE;
  SmramHobDescriptorBlock->Descriptor[0].RegionState = EFI_SMRAM_CLOSED | EFI_CACHEABLE;

  return EFI_SUCCESS;
}

VOID
ClearMemoryRange(
IN      UINT64    MemoryRangeBase,
IN      UINT64    MemoryRangeSize
)
{
  DEBUG ((EFI_D_INFO, "Clear memory per MOR request.\n"));
  DEBUG ((EFI_D_INFO, " Base = 0x%lX, Size = 0x%lX\n",
    MemoryRangeBase, MemoryRangeSize));

  if (MemoryRangeSize > 0) {
    if (MemoryRangeBase == 0) {
      //
      // ZeroMem() generates an ASSERT() if Buffer parameter is NULL.
      // Clear byte at 0 and start clear operation at address 1.
      //
      *(UINT8 *)(0) = 0;
      ZeroMem((VOID *)1, (UINTN)MemoryRangeSize - 1);
    }
    else {
      ZeroMem((VOID *)(UINTN)MemoryRangeBase, (UINTN)MemoryRangeSize);
    }
  }
}
