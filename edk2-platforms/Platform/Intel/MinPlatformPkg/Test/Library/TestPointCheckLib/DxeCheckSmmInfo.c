/** @file

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <PiDxe.h>
#include <Library/TestPointCheckLib.h>
#include <Library/TestPointLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/SmmAccess2.h>

CHAR8 *mSmramStateName[] = {
  "Open",
  "Closed",
  "Locked",
  "Cacheable",
  "Allocated",
  "NeedTest",
  "NeedEccInit",
};

VOID
DumpSmramDescriptor (
  IN UINTN                 NumberOfSmmReservedRegions,
  IN EFI_SMRAM_DESCRIPTOR  *Descriptor
  )
{
  UINTN                                 Index;
  UINTN                                 BitIndex;

  for (Index = 0; Index < NumberOfSmmReservedRegions; Index++) {
    DEBUG ((DEBUG_INFO,
      "  BA=%016lx (A=%016lx) L=%016lx  State=%016lx",
      Descriptor[Index].PhysicalStart,
      Descriptor[Index].CpuStart,
      Descriptor[Index].PhysicalSize,
      Descriptor[Index].RegionState
      ));
    DEBUG ((DEBUG_INFO, "  ("));
    for (BitIndex = 0; BitIndex < sizeof(mSmramStateName)/sizeof(mSmramStateName[0]); BitIndex++) {
      if ((Descriptor[Index].RegionState & LShiftU64 (1, BitIndex)) != 0) {
        DEBUG ((DEBUG_INFO, mSmramStateName[BitIndex]));
        DEBUG ((DEBUG_INFO, ","));
      }
    }
    DEBUG ((DEBUG_INFO, ")\n"));
  }
}

EFI_STATUS
CheckSmramDescriptor (
  IN UINTN                 NumberOfSmmReservedRegions,
  IN EFI_SMRAM_DESCRIPTOR  *Descriptor
  )
{
  UINTN   Index;
  UINT64  Base;
  UINT64  Length;

  Base = 0;
  Length = 0;
  for (Index = 0; Index < NumberOfSmmReservedRegions; Index++) {
    if (Base == 0) {
      Base   = Descriptor[Index].PhysicalStart;
      Length = Descriptor[Index].PhysicalSize;
    } else {
      if (Base + Length == Descriptor[Index].PhysicalStart) {
        Length = Length + Descriptor[Index].PhysicalSize;
      } else if (Descriptor[Index].PhysicalStart + Descriptor[Index].PhysicalSize == Base) {
        Base = Descriptor[Index].PhysicalStart;
        Length = Length + Descriptor[Index].PhysicalSize;
      } else {
        DEBUG ((DEBUG_ERROR, "Smram is not adjacent\n"));
        TestPointLibAppendErrorString (
          PLATFORM_TEST_POINT_ROLE_PLATFORM_IBV,
          NULL,
          TEST_POINT_BYTE7_DXE_SMM_READY_TO_LOCK_SMRAM_ALIGNED_ERROR_CODE \
            TEST_POINT_DXE_SMM_READY_TO_LOCK 
            TEST_POINT_BYTE7_DXE_SMM_READY_TO_LOCK_SMRAM_ALIGNED_ERROR_STRING
          );
        return EFI_INVALID_PARAMETER;
      }
    }
  }        

  if (Length != GetPowerOfTwo64 (Length)) {
    DEBUG ((DEBUG_ERROR, "Smram is not aligned\n"));
    TestPointLibAppendErrorString (
      PLATFORM_TEST_POINT_ROLE_PLATFORM_IBV,
      NULL,
      TEST_POINT_BYTE7_DXE_SMM_READY_TO_LOCK_SMRAM_ALIGNED_ERROR_CODE \
        TEST_POINT_DXE_SMM_READY_TO_LOCK 
        TEST_POINT_BYTE7_DXE_SMM_READY_TO_LOCK_SMRAM_ALIGNED_ERROR_STRING
      );
    return EFI_INVALID_PARAMETER;
  }
  return EFI_SUCCESS;
}

EFI_STATUS
TestPointCheckSmmInfo (
  VOID
  )
{
  EFI_STATUS               Status;
  EFI_SMM_ACCESS2_PROTOCOL *SmmAccess;
  UINTN                    Size;
  EFI_SMRAM_DESCRIPTOR     *SmramRanges;
  
  DEBUG ((DEBUG_INFO, "==== TestPointCheckSmmInfo - Enter\n"));
  
  Status = gBS->LocateProtocol (&gEfiSmmAccess2ProtocolGuid, NULL, (VOID **)&SmmAccess);
  if (EFI_ERROR (Status)) {
    goto Done ;
  }
  
  Size = 0;
  Status = SmmAccess->GetCapabilities (SmmAccess, &Size, NULL);
  ASSERT (Status == EFI_BUFFER_TOO_SMALL);

  SmramRanges = (EFI_SMRAM_DESCRIPTOR *) AllocateZeroPool (Size);
  ASSERT (SmramRanges != NULL);

  Status = SmmAccess->GetCapabilities (SmmAccess, &Size, SmramRanges);
  ASSERT_EFI_ERROR (Status);
  
  DEBUG ((DEBUG_INFO, "SMRAM Info\n"));
  DumpSmramDescriptor (Size / sizeof (EFI_SMRAM_DESCRIPTOR), SmramRanges);

  Status = CheckSmramDescriptor (Size / sizeof (EFI_SMRAM_DESCRIPTOR), SmramRanges);

  FreePool (SmramRanges);

Done:
  DEBUG ((DEBUG_INFO, "==== TestPointCheckSmmInfo - Exit\n"));

  return Status;
}