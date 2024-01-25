/*****************************************************************************
 * Copyright (C) 2017-2023 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

/* This file includes code originally published under the following license. */

/** @file
Platform SEC Library.

Copyright (c) 2013-2015 Intel Corporation.

This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiPei.h>

#include <Ppi/SecPlatformInformation.h>
#include <Ppi/TemporaryRamSupport.h>
#include <Library/PcdLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HobLib.h>
#include <Library/MtrrLib.h>
#include <Library/SecBoardInitLib.h>
#include <Library/TestPointCheckLib.h>
#include <Register/Amd/Msr.h>

#define  MSR_SYS_CFG                          0xC0010010ul    // SYSCFG
#define SYS_CFG_MtrrFixDramModEn_OFFSET       19

VOID
AsmSecPlatformDisableTemporaryMemory(
    VOID
);

UINT32 *
AsmSecPlatformGetTemporaryStackBase(
   VOID
);

/**

  Entry point to the C language phase of SEC. After the SEC assembly
  code has initialized some temporary memory and set up the stack,
  the control is transferred to this function.

  @param SizeOfRam           Size of the temporary memory available for use.
  @param TempRamBase         Base address of temporary ram
  @param BootFirmwareVolume  Base address of the Boot Firmware Volume.

**/
VOID
EFIAPI
SecStartup (
  IN UINT32                   SizeOfRam,
  IN UINT32                   TempRamBase,
  IN VOID                     *BootFirmwareVolume
  );

/**
  Auto-generated function that calls the library constructors for all of the module's
  dependent libraries.  This function must be called by the SEC Core once a stack has
  been established.

**/
VOID
EFIAPI
ProcessLibraryConstructorList (
  VOID
  );

/**

  Entry point to the C language phase of PlatformSecLib.  After the SEC assembly
  code has initialized some temporary memory and set up the stack, control is
  transferred to this function.

**/
VOID
EFIAPI
PlatformSecLibStartup (
  VOID
  )
{
  //
  // Process all library constructor functions linked to SecCore.
  // This function must be called before any library functions are called
  //
  ProcessLibraryConstructorList ();

  AsmMsrBitFieldOr64 (MSR_SYS_CFG, SYS_CFG_MtrrFixDramModEn_OFFSET,
      SYS_CFG_MtrrFixDramModEn_OFFSET, 0x1);
  AsmWriteMsr64 (MSR_IA32_MTRR_FIX64K_00000, 0x1E1E1E1E1E1E1E1E);
  AsmWriteMsr64 (MSR_IA32_MTRR_FIX16K_80000, 0x1E1E1E1E1E1E1E1E);
  AsmWriteMsr64 (MSR_IA32_MTRR_FIX16K_A0000, 0x1E1E1E1E1E1E1E1E);
  AsmWriteMsr64 (MSR_IA32_MTRR_FIX4K_C0000, 0x1E1E1E1E1E1E1E1E);
  AsmWriteMsr64 (MSR_IA32_MTRR_FIX4K_C8000, 0x1E1E1E1E1E1E1E1E);
  AsmWriteMsr64 (MSR_IA32_MTRR_FIX4K_D0000, 0x1E1E1E1E1E1E1E1E);
  AsmWriteMsr64 (MSR_IA32_MTRR_FIX4K_D8000, 0x1E1E1E1E1E1E1E1E);
  AsmWriteMsr64 (MSR_IA32_MTRR_FIX4K_E0000, 0x1E1E1E1E1E1E1E1E);
  AsmWriteMsr64 (MSR_IA32_MTRR_FIX4K_E8000, 0x1E1E1E1E1E1E1E1E);
  AsmWriteMsr64 (MSR_IA32_MTRR_FIX4K_F0000, 0x1E1E1E1E1E1E1E1E);
  AsmWriteMsr64 (MSR_IA32_MTRR_FIX4K_F8000, 0x1E1E1E1E1E1E1E1E);
  AsmMsrBitFieldAnd64 (MSR_SYS_CFG, SYS_CFG_MtrrFixDramModEn_OFFSET,
      SYS_CFG_MtrrFixDramModEn_OFFSET, 0x0);

  //
  // Pass control to SecCore module passing in the base address and size
  // of the temporary RAM
  //
  SecStartup (
    PcdGet32(PcdTempRamSize),
    PcdGet32(PcdTempRamBase),
    (VOID *)(UINTN)PcdGet32(PcdBootFvBase)
    );
}

/**
  This interface conveys state information out of the Security (SEC) phase into PEI.

  @param  PeiServices               Pointer to the PEI Services Table.
  @param  StructureSize             Pointer to the variable describing size of the input buffer.
  @param  PlatformInformationRecord Pointer to the EFI_SEC_PLATFORM_INFORMATION_RECORD.

  @retval EFI_SUCCESS           The data was successfully returned.
  @retval EFI_BUFFER_TOO_SMALL  The buffer was too small.

**/
EFI_STATUS
EFIAPI
SecPlatformInformation (
  IN CONST EFI_PEI_SERVICES                     **PeiServices,
  IN OUT   UINT64                               *StructureSize,
     OUT   EFI_SEC_PLATFORM_INFORMATION_RECORD  *PlatformInformationRecord
  )
{
  UINT32             *BIST_Pointer;
  UINT32             BIST_Size;
  UINT32             Count;
  EFI_HOB_GUID_TYPE  *GuidHob;
  UINT32             *TopOfStack;

  DEBUG((EFI_D_INFO, "%a() - ENTRY\n", __FUNCTION__));

  ASSERT(StructureSize);

  GuidHob = GetFirstGuidHob (&gEfiSecPlatformInformationPpiGuid);
  if (GuidHob != NULL) {
    DEBUG((EFI_D_INFO, " Found GuidHob!\n"));
    BIST_Size = GET_GUID_HOB_DATA_SIZE (GuidHob);
    BIST_Pointer = GET_GUID_HOB_DATA (GuidHob);
    DEBUG((EFI_D_INFO, "  BIST_Size = %d, BIST_Pointer = 0x%X\n",
      BIST_Size, BIST_Pointer));
  } else {
    //
    // The entries of BIST information, together with the number of them,
    // reside in the bottom of stack, left untouched by normal stack operation.
    // This routine copies the BIST information to the buffer pointed by
    // PlatformInformationRecord for output.
    //
    TopOfStack = AsmSecPlatformGetTemporaryStackBase ();
    Count = *(TopOfStack - 1);
    BIST_Size = Count * sizeof (IA32_HANDOFF_STATUS);
    BIST_Pointer = (UINT32 *)(UINTN)((UINTN)TopOfStack - sizeof (Count) - BIST_Size);

    //
    // Copy Data from Stack to Hob to avoid data is lost after memory is ready.
    //
    DEBUG((EFI_D_INFO, " Building GuidHob:\n"));
    DEBUG((EFI_D_INFO, "  BEFORE: BIST_Size = %d, BIST_Pointer = 0x%X\n",
      BIST_Size, BIST_Pointer));
    BuildGuidDataHob (
      &gEfiSecPlatformInformationPpiGuid,
      BIST_Pointer,
      (UINTN)BIST_Size
    );

    GuidHob = GetFirstGuidHob (&gEfiSecPlatformInformationPpiGuid);
    DEBUG((EFI_D_INFO, " Reading the built GuidHob... "));
    if (GuidHob != NULL) {
      DEBUG((EFI_D_INFO, "OK!\n"));
      BIST_Size = GET_GUID_HOB_DATA_SIZE(GuidHob);
      BIST_Pointer = GET_GUID_HOB_DATA(GuidHob);
    }
    else {
      DEBUG((EFI_D_INFO, "FAILED!\n"));
    }
    DEBUG((EFI_D_INFO, "  AFTER: BIST_Size = %d, BIST_Pointer = 0x%X\n",
      BIST_Size, BIST_Pointer));
  }

  DEBUG((EFI_D_INFO, " StructureSize: 0x%X, [ 0x%X ]\n",
    StructureSize, *StructureSize));
  DEBUG((EFI_D_INFO, " PlatformInformationRecord: 0x%X\n",
    PlatformInformationRecord));

  if ((*StructureSize) < (UINT64)BIST_Size ||
    PlatformInformationRecord == NULL) {
    *StructureSize = BIST_Size;
    return EFI_BUFFER_TOO_SMALL;
  }

  CopyMem(PlatformInformationRecord, BIST_Pointer, BIST_Size);
  *StructureSize = BIST_Size;

  DEBUG((EFI_D_INFO, "%a() - EXIT\n", __FUNCTION__));
  return EFI_SUCCESS;
}

/**
  This interface disables temporary memory in SEC Phase.
**/
VOID
EFIAPI
SecPlatformDisableTemporaryMemory (
  VOID
  )
{
  DEBUG((DEBUG_ERROR, "%a() - ENTRY\n", __FUNCTION__));

  AsmSecPlatformDisableTemporaryMemory();

  DEBUG((DEBUG_ERROR, "%a() - EXIT\n", __FUNCTION__));
}
/**
  Wrapper function to Early Board Init interface in SEC Phase.
**/

VOID
EFIAPI
BoardAfterTempRamInitWrapper (
  VOID
)
{

  BoardAfterTempRamInit ();

  TestPointTempMemoryFunction (
    (VOID *) PcdGet32 (PcdTempRamBase),
    (VOID *) (PcdGet32 (PcdTempRamBase) + PcdGet32 (PcdTempRamSize))
  );
}

