/**
 * @file  SilDataInit.c
 * @brief DXE driver created to execute Onyx SI OpenSIL call.
 *
 */
/*
 * Copyright 2021-2024 Advanced Micro Devices, Inc. All rights reserved.
 *
 */
#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Sil-api.h>
#include <SilSmm.h>
#include "xPrfServicesSmm.h"

/**
 * SilSmmEntryPoint
 *
 * @brief openSIL FW SMM driver entry point
 *
 * @param ImageHandle   Image handle of DXE driver
 * @param Systemtable   Pointer to UEFI system table
 *
 * @return EFI_SUCCESS
 */
EFI_STATUS
EFIAPI
SilSmmEntryPoint (
  IN       EFI_HANDLE           ImageHandle,
  IN       EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS Status;

  Status = SilFwDataInit (SystemTable);
  ASSERT_EFI_ERROR (Status);

  Status = xPrfServicesSmmProtocolInstall ();

  return Status;
}