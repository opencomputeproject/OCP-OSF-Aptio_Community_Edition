/*****************************************************************************
 *
 * Copyright (C) 2021-2024 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _Cf9_RESET_H_
#define _Cf9_RESET_H_

#include <Uefi.h>

#include <Library/IoLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/Reset.h>
#include <Sil-api.h>
#include <FCH/Common/FchCore/FchHwAcpi/FchHwAcpi.h>

//
// Reset control register values
//
#define FULLRESET       0x0E
#define HARDRESET       0x06
#define SOFTRESET       0x04
#define FULLSTARTSTATE  0x0A
#define HARDSTARTSTATE  0x02
#define SOFTSTARTSTATE  0x0

#define SUS_S3          0x0C00U     // S3
#define SUS_S5          0x1400U     // S5
#define SLP_TYPE        0x1C00U     // MASK
#define SLP_EN          0x2000U     // BIT13

#endif
