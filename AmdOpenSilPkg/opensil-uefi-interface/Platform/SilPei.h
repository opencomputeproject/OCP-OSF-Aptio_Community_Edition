/**
 * @file  SilPei.h
 * @brief AMD openSIL FW initialization library prototypes
 *
 */
/* Copyright 2021-2024 Advanced Micro Devices, Inc. All rights reserved.
 *
 */

#pragma once


typedef EFI_STATUS (*SIL_FWINIT_FUNCTION) ();

EFI_STATUS SilFwDataInit (CONST EFI_PEI_SERVICES **PeiServices, SIL_FWINIT_FUNCTION FwInitFunction);

EFI_STATUS
TranslateStatus (
  SIL_STATUS SilStatus
);

