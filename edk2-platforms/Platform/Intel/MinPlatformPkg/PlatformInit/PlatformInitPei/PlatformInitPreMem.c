/** @file
  Source code file for Platform Init Pre-Memory PEI module

Copyright (c) 2017 - 2018, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <IndustryStandard/Pci30.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/TimerLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/MtrrLib.h>
#include <Library/ReportFvLib.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Ppi/MemoryDiscovered.h>
#include <Ppi/FirmwareVolumeInfo.h>
#include <Ppi/BootInRecoveryMode.h>
#include <Ppi/MasterBootMode.h>
#include <Guid/FirmwareFileSystem2.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/BoardInitLib.h>
#include <Library/TestPointCheckLib.h>
#include <Guid/MemoryTypeInformation.h>
#include <Ppi/PlatformMemorySize.h>
#include <Ppi/BaseMemoryTest.h>

EFI_STATUS
EFIAPI
MemoryDiscoveredPpiNotifyCallback (
  IN CONST EFI_PEI_SERVICES      **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR   *NotifyDescriptor,
  IN VOID                        *Ppi
  );

EFI_STATUS
EFIAPI
GetPlatformMemorySize (
  IN      EFI_PEI_SERVICES                       **PeiServices,
  IN      PEI_PLATFORM_MEMORY_SIZE_PPI           *This,
  IN OUT  UINT64                                 *MemorySize
  );

/**

  This function checks the memory range in PEI.

  @param PeiServices     Pointer to PEI Services.
  @param This            Pei memory test PPI pointer.
  @param BeginAddress    Beginning of the memory address to be checked.
  @param MemoryLength    Bytes of memory range to be checked.
  @param Operation       Type of memory check operation to be performed.
  @param ErrorAddress    Return the address of the error memory address.

  @retval EFI_SUCCESS         The operation completed successfully.
  @retval EFI_DEVICE_ERROR    Memory test failed. It's not safe to use this range of memory.

**/
EFI_STATUS
EFIAPI
BaseMemoryTest (
  IN  EFI_PEI_SERVICES                   **PeiServices,
  IN  PEI_BASE_MEMORY_TEST_PPI           *This,
  IN  EFI_PHYSICAL_ADDRESS               BeginAddress,
  IN  UINT64                             MemoryLength,
  IN  PEI_MEMORY_TEST_OP                 Operation,
  OUT EFI_PHYSICAL_ADDRESS               *ErrorAddress
  );

static EFI_PEI_NOTIFY_DESCRIPTOR mMemDiscoveredNotifyList = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiMemoryDiscoveredPpiGuid,
  (EFI_PEIM_NOTIFY_ENTRY_POINT) MemoryDiscoveredPpiNotifyCallback
};

GLOBAL_REMOVE_IF_UNREFERENCED EFI_PEI_PPI_DESCRIPTOR mPpiListRecoveryBootMode = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiBootInRecoveryModePpiGuid,
  NULL
};

GLOBAL_REMOVE_IF_UNREFERENCED EFI_PEI_PPI_DESCRIPTOR mPpiBootMode = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiMasterBootModePpiGuid,
  NULL
};

static PEI_BASE_MEMORY_TEST_PPI     mPeiBaseMemoryTestPpi = { BaseMemoryTest };

static PEI_PLATFORM_MEMORY_SIZE_PPI mMemoryMemorySizePpi  = { GetPlatformMemorySize };

static EFI_PEI_PPI_DESCRIPTOR       mMemPpiList[] = {
  {
    EFI_PEI_PPI_DESCRIPTOR_PPI,
    &gPeiBaseMemoryTestPpiGuid,
    &mPeiBaseMemoryTestPpi
  },
  {
    (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gPeiPlatformMemorySizePpiGuid,
    &mMemoryMemorySizePpi
  },
};

///
/// Memory Reserved should be between 125% to 150% of the Current required memory
/// otherwise BdsMisc.c would do a reset to make it 125% to avoid s4 resume issues.
///
GLOBAL_REMOVE_IF_UNREFERENCED EFI_MEMORY_TYPE_INFORMATION mDefaultMemoryTypeInformation[] = {
  { EfiACPIReclaimMemory,   FixedPcdGet32 (PcdPlatformEfiAcpiReclaimMemorySize) },  // ASL
  { EfiACPIMemoryNVS,       FixedPcdGet32 (PcdPlatformEfiAcpiNvsMemorySize) },      // ACPI NVS (including S3 related)
  { EfiReservedMemoryType,  FixedPcdGet32 (PcdPlatformEfiReservedMemorySize) },     // BIOS Reserved (including S3 related)
  { EfiRuntimeServicesData, FixedPcdGet32 (PcdPlatformEfiRtDataMemorySize) },       // Runtime Service Data
  { EfiRuntimeServicesCode, FixedPcdGet32 (PcdPlatformEfiRtCodeMemorySize) },       // Runtime Service Code
  { EfiMaxMemoryType, 0 }
};

VOID
BuildMemoryTypeInformation (
  VOID
  )
{
  EFI_STATUS                      Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices;
  UINTN                           DataSize;
  EFI_MEMORY_TYPE_INFORMATION     MemoryData[EfiMaxMemoryType + 1];

  //
  // Locate system configuration variable
  //
  Status = PeiServicesLocatePpi(
             &gEfiPeiReadOnlyVariable2PpiGuid, // GUID
             0,                                // INSTANCE
             NULL,                             // EFI_PEI_PPI_DESCRIPTOR
             (VOID **) &VariableServices       // PPI
             );
  ASSERT_EFI_ERROR(Status);

  DataSize = sizeof (MemoryData);
  Status = VariableServices->GetVariable (
                               VariableServices,
                               EFI_MEMORY_TYPE_INFORMATION_VARIABLE_NAME,
                               &gEfiMemoryTypeInformationGuid,
                               NULL,
                               &DataSize,
                               &MemoryData
                               );
  if (EFI_ERROR(Status)) {
    DataSize = sizeof (mDefaultMemoryTypeInformation);
    CopyMem(MemoryData, mDefaultMemoryTypeInformation, DataSize);
  }

  ///
  /// Build the GUID'd HOB for DXE
  ///
  BuildGuidDataHob (
    &gEfiMemoryTypeInformationGuid,
    MemoryData,
    DataSize
    );
}

EFI_STATUS
EFIAPI
GetPlatformMemorySize (
  IN      EFI_PEI_SERVICES                       **PeiServices,
  IN      PEI_PLATFORM_MEMORY_SIZE_PPI           *This,
  IN OUT  UINT64                                 *MemorySize
  )
{
  EFI_STATUS                      Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *Variable;
  UINTN                           DataSize;
  EFI_MEMORY_TYPE_INFORMATION     MemoryData[EfiMaxMemoryType + 1];
  UINTN                           Index;
  EFI_BOOT_MODE                   BootMode;
  UINTN                           IndexNumber;

#define PEI_MIN_MEMORY_SIZE             (EFI_PHYSICAL_ADDRESS) ((320 * 0x100000))

  *MemorySize = PEI_MIN_MEMORY_SIZE;
  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID **)&Variable
             );

  ASSERT_EFI_ERROR (Status);

  Status = PeiServicesGetBootMode (&BootMode);
  ASSERT_EFI_ERROR (Status);

  DataSize = sizeof (MemoryData);

  Status = Variable->GetVariable (
                      Variable,
                      EFI_MEMORY_TYPE_INFORMATION_VARIABLE_NAME,
                      &gEfiMemoryTypeInformationGuid,
                      NULL,
                      &DataSize,
                      &MemoryData
                      );
  IndexNumber = sizeof (mDefaultMemoryTypeInformation) / sizeof (EFI_MEMORY_TYPE_INFORMATION);

  //
  // Accumulate maximum amount of memory needed
  //

  DEBUG((EFI_D_ERROR, "PEI_MIN_MEMORY_SIZE:%dKB \n", DivU64x32(*MemorySize,1024)));
  DEBUG((EFI_D_ERROR, "IndexNumber:%d MemoryDataNumber%d \n", IndexNumber,DataSize/ sizeof (EFI_MEMORY_TYPE_INFORMATION)));
  if (EFI_ERROR (Status)) {
    //
    // Start with minimum memory
    //
    for (Index = 0; Index < IndexNumber; Index++) {
      DEBUG((EFI_D_ERROR, "Index[%d].Type = %d .NumberOfPages=0x%x\n", Index,mDefaultMemoryTypeInformation[Index].Type,mDefaultMemoryTypeInformation[Index].NumberOfPages));
      *MemorySize += mDefaultMemoryTypeInformation[Index].NumberOfPages * EFI_PAGE_SIZE;
    }
    DEBUG((EFI_D_ERROR, "No memory type,  Total platform memory:%dKB \n", DivU64x32(*MemorySize,1024)));
  } else {
    //
    // Start with at least 0x200 pages of memory for the DXE Core and the DXE Stack
    //
    for (Index = 0; Index < IndexNumber; Index++) {
      DEBUG((EFI_D_ERROR, "Index[%d].Type = %d .NumberOfPages=0x%x\n", Index,MemoryData[Index].Type,MemoryData[Index].NumberOfPages));
      *MemorySize += MemoryData[Index].NumberOfPages * EFI_PAGE_SIZE;

    }
    DEBUG((EFI_D_ERROR, "has memory type,  Total platform memory:%dKB \n", DivU64x32(*MemorySize,1024)));
  }

  return EFI_SUCCESS;
}

/**

  This function checks the memory range in PEI.

  @param PeiServices     Pointer to PEI Services.
  @param This            Pei memory test PPI pointer.
  @param BeginAddress    Beginning of the memory address to be checked.
  @param MemoryLength    Bytes of memory range to be checked.
  @param Operation       Type of memory check operation to be performed.
  @param ErrorAddress    Return the address of the error memory address.

  @retval EFI_SUCCESS         The operation completed successfully.
  @retval EFI_DEVICE_ERROR    Memory test failed. It's not safe to use this range of memory.

**/
EFI_STATUS
EFIAPI
BaseMemoryTest (
  IN  EFI_PEI_SERVICES                   **PeiServices,
  IN  PEI_BASE_MEMORY_TEST_PPI           *This,
  IN  EFI_PHYSICAL_ADDRESS               BeginAddress,
  IN  UINT64                             MemoryLength,
  IN  PEI_MEMORY_TEST_OP                 Operation,
  OUT EFI_PHYSICAL_ADDRESS               *ErrorAddress
  )
{
  UINT32                TestPattern;
  UINT32                SpanSize;
  EFI_PHYSICAL_ADDRESS  TempAddress;

#define MEMORY_TEST_PATTERN     0x5A5A5A5A
#define MEMORY_TEST_COVER_SPAN  0x40000

  TestPattern = MEMORY_TEST_PATTERN;
  SpanSize    = 0;

  //
  // Make sure we don't try and test anything above the max physical address range
  //
  ASSERT (BeginAddress + MemoryLength < MAX_ADDRESS);

  switch (Operation) {
  case Extensive:
    SpanSize = 0x4;
    break;

  case Sparse:
  case Quick:
    SpanSize = MEMORY_TEST_COVER_SPAN;
    break;

  case Ignore:
    goto Done;
    break;
  }
  //
  // Write the test pattern into memory range
  //
  TempAddress = BeginAddress;
  while (TempAddress < BeginAddress + MemoryLength) {
    (*(UINT32 *) (UINTN) TempAddress) = TestPattern;
    TempAddress += SpanSize;
  }
  //
  // Read pattern from memory and compare it
  //
  TempAddress = BeginAddress;
  while (TempAddress < BeginAddress + MemoryLength) {
    if ((*(UINT32 *) (UINTN) TempAddress) != TestPattern) {
      *ErrorAddress = TempAddress;
      return EFI_DEVICE_ERROR;
    }

    TempAddress += SpanSize;
  }

Done:

  return EFI_SUCCESS;
}

/**
  Set Cache Mtrr.
**/
VOID
SetCacheMtrr (
  VOID
  )
{
  EFI_STATUS                  Status;
  EFI_PEI_HOB_POINTERS        Hob;
  MTRR_SETTINGS               MtrrSetting;
  UINT64                      MemoryBase;
  UINT64                      MemoryLength;
  UINT64                      LowMemoryLength;
  UINT64                      HighMemoryLength;
  EFI_BOOT_MODE               BootMode;
  EFI_RESOURCE_ATTRIBUTE_TYPE ResourceAttribute;
  UINT64                      CacheMemoryLength;

  ///
  /// Reset all MTRR setting.
  ///
  ZeroMem(&MtrrSetting, sizeof(MTRR_SETTINGS));

  ///
  /// Cache the Flash area as WP to boost performance
  ///
  Status = MtrrSetMemoryAttributeInMtrrSettings (
                &MtrrSetting,
                (UINTN) PcdGet32 (PcdFlashAreaBaseAddress),
                (UINTN) PcdGet32 (PcdFlashAreaSize),
                CacheWriteProtected
                );
  ASSERT_EFI_ERROR (Status);

  ///
  /// Update MTRR setting from MTRR buffer for Flash Region to be WP to boost performance
  ///
  MtrrSetAllMtrrs (&MtrrSetting);

  ///
  /// Set low to 1 MB. Since 1MB cacheability will always be set
  /// until override by CSM.
  /// Initialize high memory to 0.
  ///
  LowMemoryLength   = 0x100000;
  HighMemoryLength  = 0;
  ResourceAttribute = (
                       EFI_RESOURCE_ATTRIBUTE_PRESENT |
                       EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
                       EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE |
                       EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE |
                       EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
                       EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE
                       );

  Status = PeiServicesGetBootMode (&BootMode);
  ASSERT_EFI_ERROR (Status);

  if (BootMode != BOOT_ON_S3_RESUME) {
    ResourceAttribute |= EFI_RESOURCE_ATTRIBUTE_TESTED;
  }

  Status = PeiServicesGetHobList ((VOID **) &Hob.Raw);
  while (!END_OF_HOB_LIST (Hob)) {
    if (Hob.Header->HobType == EFI_HOB_TYPE_RESOURCE_DESCRIPTOR) {
      if ((Hob.ResourceDescriptor->ResourceType == EFI_RESOURCE_SYSTEM_MEMORY) ||
          ((Hob.ResourceDescriptor->ResourceType == EFI_RESOURCE_MEMORY_RESERVED) &&
           (Hob.ResourceDescriptor->ResourceAttribute == ResourceAttribute))
         ) {
        if (Hob.ResourceDescriptor->PhysicalStart >= 0x100000000ULL) {
          HighMemoryLength += Hob.ResourceDescriptor->ResourceLength;
        } else if (Hob.ResourceDescriptor->PhysicalStart >= 0x100000) {
          LowMemoryLength += Hob.ResourceDescriptor->ResourceLength;
        }
      }
    }

    Hob.Raw = GET_NEXT_HOB (Hob);
  }

  DEBUG ((DEBUG_INFO, "Memory Length (Below 4GB) = %lx.\n", LowMemoryLength));
  DEBUG ((DEBUG_INFO, "Memory Length (Above 4GB) = %lx.\n", HighMemoryLength));

  ///
  /// Assume size of main memory is multiple of 256MB
  ///
  MemoryLength = (LowMemoryLength + 0xFFFFFFF) & 0xF0000000;
  MemoryBase = 0;

  CacheMemoryLength = MemoryLength;
  ///
  /// Programming MTRRs to avoid override SPI region with UC when MAX TOLUD Length >= 3.5GB
  ///
  if (MemoryLength > 0xDC000000) {
     CacheMemoryLength = 0xC0000000;
     Status = MtrrSetMemoryAttributeInMtrrSettings (
                &MtrrSetting,
                MemoryBase,
                CacheMemoryLength,
                CacheWriteBack
                );
     ASSERT_EFI_ERROR (Status);

     MemoryBase = 0xC0000000;
     CacheMemoryLength = MemoryLength - 0xC0000000;
     if (MemoryLength > 0xE0000000) {
        CacheMemoryLength = 0x20000000;
        Status = MtrrSetMemoryAttributeInMtrrSettings (
                 &MtrrSetting,
                 MemoryBase,
                 CacheMemoryLength,
                 CacheWriteBack
                 );
        ASSERT_EFI_ERROR (Status);

        MemoryBase = 0xE0000000;
        CacheMemoryLength = MemoryLength - 0xE0000000;
     }
  }

  Status = MtrrSetMemoryAttributeInMtrrSettings (
                &MtrrSetting,
                MemoryBase,
                CacheMemoryLength,
                CacheWriteBack
                );
  ASSERT_EFI_ERROR (Status);

  if (LowMemoryLength != MemoryLength) {
     MemoryBase = LowMemoryLength;
     MemoryLength -= LowMemoryLength;
     Status = MtrrSetMemoryAttributeInMtrrSettings (
                   &MtrrSetting,
                   MemoryBase,
                   MemoryLength,
                   CacheUncacheable
                   );
      ASSERT_EFI_ERROR (Status);
  }

  ///
  /// VGA-MMIO - 0xA0000 to 0xC0000 to be UC
  ///
  Status = MtrrSetMemoryAttributeInMtrrSettings (
                &MtrrSetting,
                0xA0000,
                0x20000,
                CacheUncacheable
                );
  ASSERT_EFI_ERROR (Status);

  ///
  /// Update MTRR setting from MTRR buffer
  ///
  MtrrSetAllMtrrs (&MtrrSetting);

  return ;
}

VOID
ReportCpuHob (
  VOID
  )
{
  UINT8                         PhysicalAddressBits;
  UINT32                        RegEax;

  AsmCpuid (0x80000000, &RegEax, NULL, NULL, NULL);
  if (RegEax >= 0x80000008) {
    AsmCpuid (0x80000008, &RegEax, NULL, NULL, NULL);
    PhysicalAddressBits = (UINT8) RegEax;
  } else {
    PhysicalAddressBits = 36;
  }

  ///
  /// Create a CPU hand-off information
  ///
  BuildCpuHob (PhysicalAddressBits, 16);
}

/**
  Install Firmware Volume Hob's once there is main memory

  @param[in]  PeiServices       General purpose services available to every PEIM.
  @param[in]  NotifyDescriptor  Notify that this module published.
  @param[in]  Ppi               PPI that was installed.

  @retval     EFI_SUCCESS       The function completed successfully.
**/
EFI_STATUS
EFIAPI
MemoryDiscoveredPpiNotifyCallback (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_STATUS                    Status;
  EFI_BOOT_MODE                 BootMode;

  Status = BoardInitAfterMemoryInit ();
  ASSERT_EFI_ERROR (Status);

  Status = PeiServicesGetBootMode (&BootMode);
  ASSERT_EFI_ERROR (Status);

  SetCacheMtrr ();

  ReportCpuHob ();

  TestPointMemoryDiscoveredMtrrFunctional ();

  TestPointMemoryDiscoveredMemoryResourceFunctional ();

  ///
  /// If S3 resume, then we are done
  ///
  if (BootMode == BOOT_ON_S3_RESUME) {
    return EFI_SUCCESS;
  }

  TestPointMemoryDiscoveredDmaProtectionEnabled ();

  if (PcdGetBool (PcdStopAfterMemInit)) {
    CpuDeadLoop ();
  }

  return Status;
}


/**
  This function handles PlatformInit task after PeiReadOnlyVariable2 PPI produced

  @param[in]  PeiServices  Pointer to PEI Services Table.

  @retval     EFI_SUCCESS  The function completes successfully
  @retval     others
**/
EFI_STATUS
EFIAPI
PlatformInitPreMem (
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                        Status;
  EFI_BOOT_MODE                     BootMode;

  //
  // Start board detection
  //
  BoardDetect ();

  BoardDebugInit ();

  TestPointDebugInitDone ();

  if (PcdGetBool (PcdStopAfterDebugInit)) {
    CpuDeadLoop ();
  }

  BootMode = BoardBootModeDetect ();
  Status = PeiServicesSetBootMode (BootMode);
  ASSERT_EFI_ERROR (Status);
  if (BootMode == BOOT_IN_RECOVERY_MODE) {
    Status = PeiServicesInstallPpi (&mPpiListRecoveryBootMode);
  }
  ///
  /// Signal possible dependent modules that there has been a
  /// final boot mode determination, it is used to build BIST
  /// Hob for Dxe use.
  ///
  Status = PeiServicesInstallPpi (&mPpiBootMode);
  ASSERT_EFI_ERROR (Status);

  BuildMemoryTypeInformation ();

  if (!PcdGetBool(PcdFspWrapperBootMode)) {
    Status = PeiServicesInstallPpi (mMemPpiList);
    ASSERT_EFI_ERROR (Status);
  }

  Status = BoardInitBeforeMemoryInit ();
  ASSERT_EFI_ERROR (Status);

  return Status;
}


/**
  Platform Init before memory PEI module entry point

  @param[in]  FileHandle           Not used.
  @param[in]  PeiServices          General purpose services available to every PEIM.

  @retval     EFI_SUCCESS          The function completes successfully
  @retval     EFI_OUT_OF_RESOURCES Insufficient resources to create database
**/
EFI_STATUS
EFIAPI
PlatformInitPreMemEntryPoint (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS Status;

  Status = PlatformInitPreMem (PeiServices);

  ///
  /// After code reorangized, memorycallback will run because the PPI is already
  /// installed when code run to here, it is supposed that the InstallEfiMemory is
  /// done before.
  ///
  Status = PeiServicesNotifyPpi (&mMemDiscoveredNotifyList);

  return Status;
}
