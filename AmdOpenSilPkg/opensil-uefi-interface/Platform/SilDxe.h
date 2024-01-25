/**
 * @file  SilDxe.h
 * @brief AMD openSIL FW initialization library prototypes
 *
 */
/* Copyright 2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 */
#include "Sil-api.h"
#pragma once

typedef EFI_STATUS (*SIL_FWINIT_FUNCTION) ();

EFI_STATUS
SilFwDataInit (
  EFI_SYSTEM_TABLE    *SystemTable,
  SIL_FWINIT_FUNCTION FwInitFunction
  );

EFI_STATUS
TranslateStatus (
  SIL_STATUS SilStatus
);
