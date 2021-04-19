/** @file
  Platform specific TPM2 component.

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiDxe.h>

#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/Tpm2CommandLib.h>
#include <Library/RngLib.h>
#include <Library/UefiLib.h>
#include <Protocol/DxeSmmReadyToLock.h>

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
  This is the Event call back function to notify the Library the system is entering
  run time phase.

  @param  Event   Pointer to this event
  @param  Context Event hanlder private data
 **/
VOID
EFIAPI
ReadyToLockEventCallBack (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS   Status;
  VOID         *Interface;

  //
  // Try to locate it because EfiCreateProtocolNotifyEvent will trigger it once when registration.
  // Just return if it is not found.
  //
  Status = gBS->LocateProtocol (
                  &gEfiDxeSmmReadyToLockProtocolGuid,
                  NULL,
                  &Interface
                  );
  if (EFI_ERROR (Status)) {
    return ;
  }

  //
  // Send Tpm2HierarchyChange Auth with random value to avoid PlatformAuth being null
  //
  RandomizePlatformAuth();

  gBS->CloseEvent (Event);
}

/**
  The driver's entry point.

  @param[in] ImageHandle  The firmware allocated handle for the EFI image.
  @param[in] SystemTable  A pointer to the EFI System Table.

  @retval EFI_SUCCESS     The entry point is executed successfully.
  @retval other           Some error occurs when executing this entry point.
**/
EFI_STATUS
EFIAPI
Tcg2PlatformDxeEntryPoint (
  IN    EFI_HANDLE                  ImageHandle,
  IN    EFI_SYSTEM_TABLE            *SystemTable
  )
{
  VOID                      *Registration;
  EFI_EVENT                 Event;

  Event = EfiCreateProtocolNotifyEvent  (
            &gEfiDxeSmmReadyToLockProtocolGuid,
            TPL_CALLBACK,
            ReadyToLockEventCallBack,
            NULL,
            &Registration
            );
  ASSERT (Event != NULL);

  return EFI_SUCCESS;
}
