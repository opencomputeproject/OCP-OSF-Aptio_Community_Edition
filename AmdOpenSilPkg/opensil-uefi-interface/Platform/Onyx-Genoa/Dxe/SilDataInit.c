/**
 * @file  SilDataInit.c
 * @brief DXE driver created to execute Onyx SI OpenSIL call.
 *
 */
/*
 * Copyright 2021-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 */
#include <Uefi.h>

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Sil-api.h>
#include <SilDxe.h>
#include <CcxClass-api.h>
#include <xSIM-api.h>
#include <xPRF-api.h>
#include <Library/BaseLib.h>
#include <Uefi/UefiSpec.h>
#include <Uefi/UefiBaseType.h>
#include <PiDxe.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/BaseMemoryLib.h>
#include <xPrfServicesDxe.h>
#include <Include/xPrfServicesDxeProtocol.h>
#include <Include/Pi/PiHob.h>
#include <Include/Library/HobLib.h>

/*
 * Guids produced/consumed by openSIL DXE driver
 */
extern EFI_GUID gPeiOpenSilDataHobGuid;
extern EFI_GUID gEfiPciIoProtocolGuid;

EFI_STATUS CcxDxeInit (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable);

/**
 * SilPciIoNotification
 *
 * @brief Event handler to be launched after PCI I/O protocol
 * @details This is the openSil 'POST PCI Enumeration' time point (TP2)
 *
 * @return  void
 */
VOID
EFIAPI
SilPciIoNotification (
  IN       EFI_EVENT        Event,
  IN       VOID             *Context
  )
{

  DEBUG ((DEBUG_INFO, "OpenSIL PCI IO callback\n"));
  // Pass control to openSIL for timepoint 2
  InitializeSiTp2 ();

  /// @todo : Revisit if we need reset capability from TP3
  // SilHandleReset (PeiServices, SilStatus);

  gBS->CloseEvent(Event);
}

/**
 * SilReadyToBootCallback
 *
 * @brief Event handler to be launched after ready to boot
 * @details This is the openSil 'Pre-OS Boot' time point (TP3)
 *
 * @return  void
 *
 */
VOID
EFIAPI
SilReadyToBootCallback (
  IN       EFI_EVENT        Event,
  IN       VOID             *Context
  )
{

  DEBUG ((DEBUG_INFO, "OpenSIL ready to boot callback\n"));
  // Pass control to openSIL for timepoint 2
  InitializeSiTp3 ();

  /// @todo : Revisit if we need reset capability from TP3
  // SilHandleReset (PeiServices, SilStatus);

  gBS->CloseEvent(Event);
}

/**
 * OnyxlIpBlocksInit
 *
 * @brief Initialize IP blocks with the host FW specific data: PCDs, Setup variables, etc.
 */
EFI_STATUS OnyxIpBlocksInit (VOID)
{
  EFI_STATUS Status;
// place Onyx IP data init code that depends on Host FW (PCDs, Setup questions, etc.)

  Status = EFI_SUCCESS;

  return Status;
}

/**
 * @brief Onyx openSIL FW DXE driver entry point
 *
 * @param ImageHandle   Image handle of DXE driver
 * @param Systemtable   Pointer to UEFI system table
 *
 * @return EFI_SUCCESS
 */
EFI_STATUS
EFIAPI
SilDxeEntryPoint (
  IN       EFI_HANDLE           ImageHandle,
  IN       EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS         Status;
  EFI_EVENT          PciIoEvent;
  EFI_EVENT          BootEvent;
  VOID               *Registration;

  Status = EFI_SUCCESS;
  DEBUG ((DEBUG_INFO, "OpenSIL DXE Driver execution\n"));

  Status = SilFwDataInit (SystemTable, OnyxIpBlocksInit);
  ASSERT_EFI_ERROR (Status);

  Status = CcxDxeInit (ImageHandle, SystemTable);
  ASSERT_EFI_ERROR (Status);

  // Create PciIo notification to call SillPciIoCallback
  Status = gBS->CreateEvent (
              EVT_NOTIFY_SIGNAL,
              TPL_NOTIFY,
              (EFI_EVENT_NOTIFY) SilPciIoNotification,
              NULL,
              &PciIoEvent
              );
  ASSERT_EFI_ERROR (Status);

  Status = gBS->RegisterProtocolNotify (
              &gEfiPciIoProtocolGuid,
              PciIoEvent,
              &Registration
              );
  ASSERT_EFI_ERROR (Status);

  // Create ready to boot callback to call SilReadyToBootCallback
  Status = EfiCreateEventReadyToBootEx (
              TPL_CALLBACK,
              (EFI_EVENT_NOTIFY) SilReadyToBootCallback,
              NULL,
              &BootEvent
              );
  ASSERT_EFI_ERROR (Status);

  // Install xPRF services protocol to provide xPRF interface to UEFI
  Status = xPrfServicesProtocolInstall ();

  return Status;
}
