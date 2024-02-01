/*****************************************************************************
 *
 * Copyright (C) 2020-2024 Advanced Micro Devices, Inc. All rights reserved.
 *
 *****************************************************************************/

/**
  SMM Access Driver
  
**/

#include <SmmAccess2Dxe.h>

EFI_SMRAM_DESCRIPTOR      *mSmramMap;
EFI_MP_SERVICES_PROTOCOL  *mMpServices = NULL;
UINTN                     NumberOfRegions = 0;
BOOLEAN                   mSetSmmLock;

STATIC EFI_SMM_ACCESS2_PROTOCOL mSmmAccess2 = {
  SmmAccess2Open,
  SmmAccess2Close,
  SmmAccess2Lock,
  SmmAccess2GetCapabilities,
  FALSE,
  FALSE
};

/**
 *  Helper routine to open SMRAM on cores
 *
 *  @param[in]     Unused
 *
 */
VOID
EFIAPI
OpenSmramOnCore (
  IN       VOID *Unused
 )
{
  // Disable protection in ASeg and TSeg
  AsmMsrAnd64 (SMMMASK_ADDRESS, ~((UINT64) BIT1 | (UINT64) BIT0));

  // Enable FixMtrrModEn
  AsmMsrOr64 (MSR_SYS_CFG, LShiftU64(1, 19));

  // Enable Rd/Wr DRAM in ASeg
  AsmMsrOr64 (MSR_IA32_MTRR_FIX16K_A0000, 0x1818181818181818);

  // Disable FixMtrrModEn
  AsmMsrAnd64 (MSR_SYS_CFG, ~(LShiftU64 (1, 19)));
}

/**
 *  Helper routine to close SMRAM on cores
 *
 *  @param[in]     Unused
 *
 */
VOID
EFIAPI
CloseSmramOnCore (
  IN       VOID *Unused
 )
{
  // Disable protection in ASeg and TSeg
  AsmMsrAnd64 (SMMMASK_ADDRESS, ~((UINT64) BIT1 | (UINT64) BIT0));

  // Enable FixMtrrModEn
  AsmMsrOr64 (MSR_SYS_CFG, LShiftU64 (1, 19));

  // Disable Rd/Wr DRAM in ASeg
  AsmMsrAnd64 (MSR_IA32_MTRR_FIX16K_A0000, 0xE7E7E7E7E7E7E7E7);

  // Disable FixMtrrModEn
  AsmMsrAnd64 (MSR_SYS_CFG, ~(LShiftU64 (1, 19)));
}

/**
 *  Helper routine to lock SMRAM on cores
 *
 *  @param[in]     Unused
 *
 */
VOID
EFIAPI
LockSmramOnCore (
  IN       VOID *Unused
 )
{
  AsmWbinvd ();

  AsmMsrOr64 (SMMMASK_ADDRESS, (BIT1 | BIT0));

  if (mSetSmmLock) {
    AsmMsrOr64 (MSR_HWCR, SMMLOCK|SMMBASELOCK);
  }
}

/**
 *  Opens the SMRAM area to be accessible by a boot-service driver
 *
 *  @param[in]     This                  The EFI_SMM_ACCESS2_PROTOCOL instance
 *
 *  @retval        EFI_SUCCESS           The operation was successful
 *  @retval        EFI_DEVICE_ERROR      SMRAM cannot be opened, perhaps because it is locked
 *  @retval        EFI_UNSUPPORTED       The system does not support opening and closing of SMRAM
 */
EFI_STATUS
EFIAPI
SmmAccess2Open (
  IN       EFI_SMM_ACCESS2_PROTOCOL *This
  )
{

  UINTN      Index;

  DEBUG ((DEBUG_INFO, "SmmAccess2Open: OPEN SMM\n"));

  if ((mSmmAccess2.LockState) || (mMpServices == NULL)) {
    return EFI_DEVICE_ERROR;
  }

  for (Index = 0; Index < NumberOfRegions; Index++) {
    if (mSmramMap[Index].RegionState & EFI_SMRAM_LOCKED) {
      DEBUG ((DEBUG_INFO, "SmmAccess2Open: RegionState %d is locked\n", Index));
      continue;
    }
    mSmramMap[Index].RegionState &= ~(EFI_SMRAM_CLOSED | EFI_ALLOCATED);
    mSmramMap[Index].RegionState |= EFI_SMRAM_OPEN;
  }

  mMpServices->StartupAllAPs (
               mMpServices,
               OpenSmramOnCore,
               PcdGetBool (PcdAmdStartupAllAPsSingleThread),
               NULL,
               0,
               NULL,
               NULL
               );
  OpenSmramOnCore (NULL);

  mSmmAccess2.OpenState = TRUE;

  return EFI_SUCCESS;
}

/**
 *  Inhibits access to the SMRAM.
 *
 *  @param[in]     This                  The EFI_SMM_ACCESS2_PROTOCOL instance
 *
 *  @retval        EFI_SUCCESS           The operation was successful
 *  @retval        EFI_DEVICE_ERROR      SMRAM cannot be closed
 *  @retval        EFI_UNSUPPORTED       The system does not support opening and closing of SMRAM
 */
EFI_STATUS
EFIAPI
SmmAccess2Close (
  IN       EFI_SMM_ACCESS2_PROTOCOL *This
  )
{

  UINTN Index;

  DEBUG ((DEBUG_INFO, "SmmAccess2Close: CLOSE SMM\n"));

  if (mSmmAccess2.LockState || (mMpServices == NULL)) {
    return EFI_DEVICE_ERROR;
  }

  for (Index = 0; Index < NumberOfRegions; Index++) {
    if (mSmramMap[Index].RegionState & (EFI_SMRAM_LOCKED | EFI_SMRAM_CLOSED)) {
      DEBUG ((DEBUG_INFO, "SmmAccess2Close: RegionState %d is locked\n", Index));
      continue;
    }
    mSmramMap[Index].RegionState &= ~EFI_SMRAM_OPEN;
    mSmramMap[Index].RegionState |= (EFI_SMRAM_CLOSED | EFI_ALLOCATED);
  }

  mMpServices->StartupAllAPs (
                      mMpServices,
                      CloseSmramOnCore,
                      PcdGetBool (PcdAmdStartupAllAPsSingleThread),
                      NULL,
                      0,
                      NULL,
                      NULL
                      );
  CloseSmramOnCore (NULL);

  mSmmAccess2.OpenState = FALSE;

  return EFI_SUCCESS;
}

/**
 *  Inhibits access to the SMRAM
 *
 *  @param[in]     This                  The EFI_SMM_ACCESS2_PROTOCOL instance
 *
 *  @retval        EFI_SUCCESS           The device was successfully locked
 *  @retval        EFI_UNSUPPORTED       The system does not support locking of SMRAM
 */
EFI_STATUS
EFIAPI
SmmAccess2Lock (
  IN       EFI_SMM_ACCESS2_PROTOCOL *This
  )
{
  UINTN                 Index;
  UINT32                *SMIx98;
  UINT64                FchAddress;

  DEBUG ((DEBUG_INFO, "SmmAccess2Lock: LOCKING SMM\n"));

  if (mSmmAccess2.OpenState || (mMpServices == NULL)) {
    return EFI_DEVICE_ERROR;
  }

  for (Index = 0; Index < NumberOfRegions; Index++) {
    mSmramMap[Index].RegionState |= EFI_SMRAM_LOCKED;
  }

  // Check if already locked
  if (mSmmAccess2.LockState) {
    DEBUG ((DEBUG_INFO, "SmmAccess2Lock: Already Locked.\n"));
    return EFI_SUCCESS;
  }

  /// @todo Use FCH library to enable/disable SMI
  // Disable SMI generation
  FchAddress = FCH_MMIO_ADDRESS + SMI_REGISTER_SPACE + SMI_TRIG0_OFFSET;
  SMIx98 = (UINT32 *) FchAddress;
  *SMIx98 |= SMIENB;

  //AcquirePspSmiRegMutexV2 ();
  mMpServices->StartupAllAPs (
                      mMpServices,
                      LockSmramOnCore,
                      PcdGetBool (PcdAmdStartupAllAPsSingleThread),
                      NULL,
                      0,
                      NULL,
                      NULL
                      );
  LockSmramOnCore (NULL);
  //ReleasePspSmiRegMutexV2 ();

  mSmmAccess2.LockState = TRUE;

  // Re-enable SMI generation
  *SMIx98 &= ~SMIENB;

  return EFI_SUCCESS;
}

/**
 *  Queries the memory controller for the regions that will support SMRAM
 *
 *  @param[in]       This                The EFI_SMM_ACCESS2_PROTOCOL instance
 *  @param[in, out]  SmramMapSize        A pointer to the size, in bytes, of the SmramMemoryMap
 *                                       buffer. On input, this value is the size of the buffer
 *                                       that is allocated by the caller. On output, it is the
 *                                       size of the buffer that was returned if the buffer was
 *                                       large enough, or, if the buffer was too small, the size
 *                                       of the buffer that is needed to contain the map
 *  @param[in, out]  SmramMap            A pointer to the buffer in which firmware places the
 *                                       current memory map. The map is an array of EFI_SMRAM_DESCRIPTORs
 *
 *  @retval        EFI_SUCCESS           The operation was successful
 *  @retval        EFI_BUFFER_TOO_SMALL  SmramMapSize buffer is too small
 */
EFI_STATUS
EFIAPI
SmmAccess2GetCapabilities (
  IN CONST EFI_SMM_ACCESS2_PROTOCOL *This,
  IN OUT   UINTN                    *SmramMapSize,
  IN OUT   EFI_SMRAM_DESCRIPTOR     *SmramMap
  )
{
  UINTN Size;
  EFI_STATUS Status;

  Size = NumberOfRegions * sizeof (EFI_SMRAM_DESCRIPTOR);
  Status = EFI_SUCCESS;

  if ((*SmramMapSize >= Size) && (SmramMap != NULL)) {
    CopyMem (SmramMap, mSmramMap, Size);
  } else {
    DEBUG ((DEBUG_ERROR, "SmmAccess2GetCapabilities: SmramMapSize is too small!\n"));
    Status = EFI_BUFFER_TOO_SMALL;
  }

  *SmramMapSize = Size;

  return Status;
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Helper routine to program TsegBase and TsegMask
 *
 *  @param[in]     Buffer                     Tseg configuration data
 *
 */
VOID
EFIAPI
SetupSmmRegs (
  IN       VOID  *Buffer
  )
{
  SMM_TSEG_CONFIG  *TsegConfig;


  TsegConfig = Buffer;

  // Setup TSegBase and TSegMask
  AsmWriteMsr64 (SMMADDR_ADDRESS, TsegConfig->TsegBase);
  AsmWriteMsr64 (SMMMASK_ADDRESS, TsegConfig->TsegMask);
}


/*++

Routine Description:

  Installs an SMM Access2 Protocol.

Arguments:

  ImageHandle  -  Handle for the image of this driver.
  SystemTable  -  Pointer to the EFI System Table.

Returns:

  EFI_SUCCESS     -  Protocol successfully started and installed.
  EFI_UNSUPPORTED -  Protocol can't be started.
  EFI_NOT_FOUND   -  Protocol not found.
--*/

EFI_STATUS
EFIAPI
SmmAccess2DriverEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  UINTN                             Index;
  UINT64                            TotalSize;
  EFI_STATUS                        Status;
  EFI_HOB_GUID_TYPE                 *GuidHob;
  SMM_TSEG_CONFIG                   TsegConfig;
  EFI_SMRAM_HOB_DESCRIPTOR_BLOCK    *SmramDescriptorBlock;
  EFI_HANDLE                        SmmAccessHandle = NULL;
  
  Status = gBS->LocateProtocol (
                  &gEfiMpServiceProtocolGuid,
                  NULL,
                  (VOID **) &mMpServices
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "SmmAccess2DriverEntryPoint: Locate MP Service Status:[%r]\n", Status));
    return EFI_NOT_FOUND;
  }

  GuidHob = GetFirstGuidHob (&gEfiSmmPeiSmramMemoryReserveGuid);
  if (GuidHob == NULL) {
    DEBUG ((DEBUG_ERROR, "SmmAccess2DriverEntryPoint: Get PEI SMRAM Memory Hob failed\n"));
    return EFI_NOT_FOUND;
  }

  SmramDescriptorBlock = (EFI_SMRAM_HOB_DESCRIPTOR_BLOCK *) GET_GUID_HOB_DATA (GuidHob);

  mSmramMap = AllocateZeroPool ((SmramDescriptorBlock->NumberOfSmmReservedRegions) * sizeof (EFI_SMRAM_DESCRIPTOR));

  NumberOfRegions = SmramDescriptorBlock->NumberOfSmmReservedRegions;

  TotalSize = 0;
  for (Index = 0; Index < NumberOfRegions; Index++) {
    mSmramMap[Index].PhysicalStart = SmramDescriptorBlock->Descriptor[Index].PhysicalStart;
    mSmramMap[Index].CpuStart      = SmramDescriptorBlock->Descriptor[Index].CpuStart;
    mSmramMap[Index].PhysicalSize  = SmramDescriptorBlock->Descriptor[Index].PhysicalSize;
    mSmramMap[Index].RegionState   = SmramDescriptorBlock->Descriptor[Index].RegionState;
    TotalSize += mSmramMap[Index].PhysicalSize;
  }

  TsegConfig.TsegBase = mSmramMap[0].PhysicalStart;
  TsegConfig.TsegMask = ~(TotalSize - 1) | 0x6600;

  mMpServices->StartupAllAPs (
                      mMpServices,
                      SetupSmmRegs,
                      FALSE,
                      NULL,
                      0,
                      (VOID *) &TsegConfig,
                      NULL
                      );
  SetupSmmRegs (&TsegConfig);

  mSetSmmLock = PcdGetBool (PcdAmdSmmLock);

  Status = gBS->InstallMultipleProtocolInterfaces (
                                      &SmmAccessHandle,
                                      &gEfiSmmAccess2ProtocolGuid,
                                      &mSmmAccess2,
                                      NULL
                                      );
  DEBUG ((DEBUG_INFO, "SmmAccess2DriverEntryPoint: Install SMM Access Protocol Status:[%r]\n", Status));
  return Status;
}
