/**
 * @file  SilSmm.h
 * @brief AMD openSIL FW initialization library prototypes
 *
 */
/* Copyright 2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 */

#pragma once

EFI_STATUS
SilFwDataInit (
  EFI_SYSTEM_TABLE    *SystemTable
  );

EFI_STATUS
TranslateStatus (
  SIL_STATUS SilStatus
);
