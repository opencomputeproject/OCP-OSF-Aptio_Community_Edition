/** @file
  Source code file for Platform Init PEI module

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/IoLib.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesLib.h>
#include <IndustryStandard/Pci30.h>
#include <Ppi/EndOfPeiPhase.h>
#include <Library/MtrrLib.h>
#include <Guid/SmramMemoryReserve.h>

#include <Guid/FirmwareFileSystem2.h>
#include <Protocol/FirmwareVolumeBlock.h>

#include <Library/TimerLib.h>
#include <Library/BoardInitLib.h>
#include <Library/TestPointCheckLib.h>

EFI_STATUS
EFIAPI
PlatformInitEndOfPei (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  );

static EFI_PEI_NOTIFY_DESCRIPTOR  mEndOfPeiNotifyList = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiEndOfPeiSignalPpiGuid,
  (EFI_PEIM_NOTIFY_ENTRY_POINT) PlatformInitEndOfPei
};

/**
  Update MTRR setting and set write back as default memory attribute.

  @retval  EFI_SUCCESS  The function completes successfully.
  @retval  Others       Some error occurs.
**/
EFI_STATUS
EFIAPI
SetCacheMtrrAfterEndOfPei (
  VOID
  )
{
  EFI_STATUS                            Status;
  MTRR_SETTINGS                         MtrrSetting;
  EFI_PEI_HOB_POINTERS                  Hob;
  UINT64                                MemoryBase;
  UINT64                                MemoryLength;
  UINT64                                Power2Length;
  EFI_BOOT_MODE                         BootMode;
  UINTN                                 Index;
  UINT64                                SmramSize;
  UINT64                                SmramBase;
  EFI_SMRAM_HOB_DESCRIPTOR_BLOCK        *SmramHobDescriptorBlock;
  Status = PeiServicesGetBootMode (&BootMode);
  ASSERT_EFI_ERROR (Status);

  if (BootMode == BOOT_ON_S3_RESUME) {
    return EFI_SUCCESS;
  }
  //
  // Clear the CAR Settings
  //
  ZeroMem(&MtrrSetting, sizeof(MTRR_SETTINGS));

  //
  // Default Cachable attribute will be set to WB to support large memory size/hot plug memory
  //
  MtrrSetting.MtrrDefType &= ~((UINT64)(0xFF));
  MtrrSetting.MtrrDefType |= (UINT64) CacheWriteBack;

  //
  // Set fixed cache for memory range below 1MB
  //
  Status = MtrrSetMemoryAttributeInMtrrSettings (
                         &MtrrSetting,
                         0x0,
                         0xA0000,
                         CacheWriteBack
                         );
  ASSERT_EFI_ERROR (Status);

  Status = MtrrSetMemoryAttributeInMtrrSettings (
                         &MtrrSetting,
                         0xA0000,
                         0x20000,
                         CacheUncacheable
                         );
  ASSERT_EFI_ERROR (Status);

  Status = MtrrSetMemoryAttributeInMtrrSettings (
                         &MtrrSetting,
                         0xC0000,
                         0x40000,
                         CacheWriteProtected
                         );
  ASSERT_EFI_ERROR ( Status);

  //
  // PI SMM IPL can't set SMRAM to WB because at that time CPU ARCH protocol is not available.
  // Set cacheability of SMRAM to WB here to improve SMRAM initialization performance.
  //
  SmramSize = 0;
  SmramBase = 0;
  Status = PeiServicesGetHobList ((VOID **) &Hob.Raw);
  while (!END_OF_HOB_LIST (Hob)) {
    if (Hob.Header->HobType == EFI_HOB_TYPE_GUID_EXTENSION) {
      if (CompareGuid (&Hob.Guid->Name, &gEfiSmmPeiSmramMemoryReserveGuid)) {
        SmramHobDescriptorBlock = (EFI_SMRAM_HOB_DESCRIPTOR_BLOCK *) (Hob.Guid + 1);
        for (Index = 0; Index < SmramHobDescriptorBlock->NumberOfSmmReservedRegions; Index++) {
          if (SmramHobDescriptorBlock->Descriptor[Index].PhysicalStart > 0x100000) {
            SmramSize += SmramHobDescriptorBlock->Descriptor[Index].PhysicalSize;
            if (SmramBase == 0 || SmramBase > SmramHobDescriptorBlock->Descriptor[Index].CpuStart) {
              SmramBase = SmramHobDescriptorBlock->Descriptor[Index].CpuStart;
            }
          }
        }
        break;
      }
    }
    Hob.Raw = GET_NEXT_HOB (Hob);
  }

  //
  // Set non system memory as UC
  //
  MemoryBase   = 0x100000000;

  //
  // Add IED size to set whole SMRAM as WB to save MTRR count
  //
  MemoryLength = MemoryBase - (SmramBase + SmramSize);
  while (MemoryLength != 0) {
    Power2Length = GetPowerOfTwo64 (MemoryLength);
    MemoryBase -= Power2Length;
    Status = MtrrSetMemoryAttributeInMtrrSettings (
                &MtrrSetting,
                MemoryBase,
                Power2Length,
                CacheUncacheable
                );
    ASSERT_EFI_ERROR (Status);
    MemoryLength -= Power2Length;
  }

  DEBUG ((DEBUG_INFO, "PcdPciReservedMemAbove4GBLimit - 0x%lx\n", PcdGet64 (PcdPciReservedMemAbove4GBLimit)));
  DEBUG ((DEBUG_INFO, "PcdPciReservedMemAbove4GBBase - 0x%lx\n", PcdGet64 (PcdPciReservedMemAbove4GBBase)));
  if (PcdGet64 (PcdPciReservedMemAbove4GBLimit) > PcdGet64 (PcdPciReservedMemAbove4GBBase)) {
    Status = MtrrSetMemoryAttributeInMtrrSettings (
                           &MtrrSetting,
                           PcdGet64 (PcdPciReservedMemAbove4GBBase),
                           PcdGet64 (PcdPciReservedMemAbove4GBLimit) - PcdGet64 (PcdPciReservedMemAbove4GBBase) + 1,
                           CacheUncacheable
                           );
    ASSERT_EFI_ERROR ( Status);
  }

  DEBUG ((DEBUG_INFO, "PcdPciReservedPMemAbove4GBLimit - 0x%lx\n", PcdGet64 (PcdPciReservedPMemAbove4GBLimit)));
  DEBUG ((DEBUG_INFO, "PcdPciReservedPMemAbove4GBBase - 0x%lx\n", PcdGet64 (PcdPciReservedPMemAbove4GBBase)));
  if (PcdGet64 (PcdPciReservedPMemAbove4GBLimit) > PcdGet64 (PcdPciReservedPMemAbove4GBBase)) {
    Status = MtrrSetMemoryAttributeInMtrrSettings (
                           &MtrrSetting,
                           PcdGet64 (PcdPciReservedPMemAbove4GBBase),
                           PcdGet64 (PcdPciReservedPMemAbove4GBLimit) - PcdGet64 (PcdPciReservedPMemAbove4GBBase) + 1,
                           CacheUncacheable
                           );
    ASSERT_EFI_ERROR ( Status);
  }

  //
  // Update MTRR setting from MTRR buffer
  //
  MtrrSetAllMtrrs (&MtrrSetting);

  return Status;
}

/**
  This function handles PlatformInit task at the end of PEI

  @param[in]  PeiServices  Pointer to PEI Services Table.
  @param[in]  NotifyDesc   Pointer to the descriptor for the Notification event that
                           caused this function to execute.
  @param[in]  Ppi          Pointer to the PPI data associated with this function.

  @retval     EFI_SUCCESS  The function completes successfully
  @retval     others
**/
EFI_STATUS
EFIAPI
PlatformInitEndOfPei (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_STATUS                    Status;
  
  Status = BoardInitAfterSiliconInit ();
  ASSERT_EFI_ERROR (Status);

  TestPointEndOfPeiSystemResourceFunctional ();

  TestPointEndOfPeiPciBusMasterDisabled ();

  Status = SetCacheMtrrAfterEndOfPei ();
  ASSERT_EFI_ERROR (Status);

  TestPointEndOfPeiMtrrFunctional ();

  return Status;
}


/**
  Platform Init PEI module entry point

  @param[in]  FileHandle           Not used.
  @param[in]  PeiServices          General purpose services available to every PEIM.

  @retval     EFI_SUCCESS          The function completes successfully
  @retval     EFI_OUT_OF_RESOURCES Insufficient resources to create database
**/
EFI_STATUS
EFIAPI
PlatformInitPostMemEntryPoint (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                       Status;

  Status = BoardInitBeforeSiliconInit ();
  ASSERT_EFI_ERROR (Status);

  //
  // Performing PlatformInitEndOfPei after EndOfPei PPI produced
  //
  Status = PeiServicesNotifyPpi (&mEndOfPeiNotifyList);

  return Status;
}
