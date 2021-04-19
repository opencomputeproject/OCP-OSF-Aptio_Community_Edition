/** @file

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/PeiServicesLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/HobLib.h>
#include <Library/Tpm2CommandLib.h>
#include <Library/Tpm2DeviceLib.h>
#include <Library/RngLib.h>

#include <Ppi/EndOfPeiPhase.h>

#define MAX_NEW_AUTHORIZATION_SIZE        SHA512_DIGEST_SIZE

/**
  Generate high-quality entropy source through RDRAND.

  @param[in]   Length        Size of the buffer, in bytes, to fill with.
  @param[out]  Entropy       Pointer to the buffer to store the entropy data.

  @retval EFI_SUCCESS        Entropy generation succeeded.
  @retval EFI_NOT_READY      Failed to request random data.

**/
EFI_STATUS
EFIAPI
RdRandGenerateEntropy (
  IN UINTN         Length,
  OUT UINT8        *Entropy
  )
{
  EFI_STATUS  Status;
  UINTN       BlockCount;
  UINT64      Seed[2];
  UINT8       *Ptr;

  Status = EFI_NOT_READY;
  BlockCount = Length / 64;
  Ptr = (UINT8 *)Entropy;

  //
  // Generate high-quality seed for DRBG Entropy
  //
  while (BlockCount > 0) {
    Status = GetRandomNumber128(Seed);
    if (EFI_ERROR(Status)) {
      return Status;
    }
    CopyMem(Ptr, Seed, 64);

    BlockCount--;
    Ptr = Ptr + 64;
  }

  //
  // Populate the remained data as request.
  //
  Status = GetRandomNumber128(Seed);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  CopyMem(Ptr, Seed, (Length % 64));

  return Status;
}

/**
  Set PlatformAuth to random value.
**/
VOID
RandomizePlatformAuth (
  VOID
  )
{
  EFI_STATUS                        Status;
  UINT16                            AuthSize;
  TPML_PCR_SELECTION                Pcrs;
  UINT32                            Index;
  UINT8                             *Rand;
  UINTN                             RandSize;
  TPM2B_AUTH                        NewPlatformAuth;

  //
  // Send Tpm2HierarchyChange Auth with random value to avoid PlatformAuth being null
  //
  ZeroMem(&Pcrs, sizeof(TPML_PCR_SELECTION));
  AuthSize = MAX_NEW_AUTHORIZATION_SIZE;

  Status = Tpm2GetCapabilityPcrs(&Pcrs);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Tpm2GetCapabilityPcrs fail!\n"));
  } else {
    for (Index = 0; Index < Pcrs.count; Index++) {
      switch (Pcrs.pcrSelections[Index].hash) {
      case TPM_ALG_SHA1:
        AuthSize = SHA1_DIGEST_SIZE;
        break;
      case TPM_ALG_SHA256:
        AuthSize = SHA256_DIGEST_SIZE;
        break;
      case TPM_ALG_SHA384:
        AuthSize = SHA384_DIGEST_SIZE;
        break;
      case TPM_ALG_SHA512:
        AuthSize = SHA512_DIGEST_SIZE;
        break;
      case TPM_ALG_SM3_256:
        AuthSize = SM3_256_DIGEST_SIZE;
        break;
      }
    }
  }

  ZeroMem(NewPlatformAuth.buffer, AuthSize);
  NewPlatformAuth.size = AuthSize;

  //
  // Allocate one buffer to store random data.
  //
  RandSize = MAX_NEW_AUTHORIZATION_SIZE;
  Rand = AllocatePool(RandSize);

  RdRandGenerateEntropy(RandSize, Rand);
  CopyMem(NewPlatformAuth.buffer, Rand, AuthSize);

  FreePool(Rand);

  //
  // Send Tpm2HierarchyChangeAuth command with the new Auth value
  //
  Status = Tpm2HierarchyChangeAuth(TPM_RH_PLATFORM, NULL, &NewPlatformAuth);
  DEBUG((DEBUG_INFO, "Tpm2HierarchyChangeAuth Result: - %r\n", Status));
  ZeroMem(NewPlatformAuth.buffer, AuthSize);
  ZeroMem(Rand, RandSize);
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
  VOID *TcgEventLog;

  //
  // Try to get TcgEventLog in S3 to see if S3 error is reported.
  //
  TcgEventLog = GetFirstGuidHob(&gTcgEventEntryHobGuid);
  if (TcgEventLog == NULL) {
    TcgEventLog = GetFirstGuidHob(&gTcgEvent2EntryHobGuid);
  }

  if (TcgEventLog == NULL) {
    //
    // no S3 error reported
    //
    return EFI_SUCCESS;
  }

  //
  // If there is S3 error on TPM_SU_STATE and success on TPM_SU_CLEAR,
  // Send Tpm2HierarchyChange Auth with random value to avoid PlatformAuth being null
  //
  RandomizePlatformAuth();

  return EFI_SUCCESS;
}

static EFI_PEI_NOTIFY_DESCRIPTOR  mEndOfPeiNotifyList = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiEndOfPeiSignalPpiGuid,
  (EFI_PEIM_NOTIFY_ENTRY_POINT)PlatformInitEndOfPei
};

/**
  Main entry

  @param[in]  FileHandle              Handle of the file being invoked.
  @param[in]  PeiServices             Pointer to PEI Services table.

  @retval EFI_SUCCESS Install function successfully. 

**/
EFI_STATUS
EFIAPI
Tcg2PlatformPeiEntryPoint (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS               Status;
  EFI_BOOT_MODE            BootMode;

  Status = PeiServicesGetBootMode (&BootMode);
  ASSERT_EFI_ERROR(Status);

  if (BootMode != BOOT_ON_S3_RESUME) {
    return EFI_SUCCESS;
  }

  //
  // Performing PlatformInitEndOfPei after EndOfPei PPI produced
  //
  Status = PeiServicesNotifyPpi (&mEndOfPeiNotifyList);

  return Status;
}
