/** @file

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _MULTI_BOARD_INIT_SUPPORT_LIB_H_
#define _MULTI_BOARD_INIT_SUPPORT_LIB_H_

#include <Library/BoardInitLib.h>

typedef
EFI_STATUS
(EFIAPI *BOARD_DETECT) (
  VOID
  );

typedef
EFI_STATUS
(EFIAPI *BOARD_INIT) (
  VOID
  );

typedef
EFI_BOOT_MODE
(EFIAPI *BOARD_BOOT_MODE_DETECT) (
  VOID
  );

typedef struct {
  BOARD_DETECT  BoardDetect;
} BOARD_DETECT_FUNC;

typedef struct {
  BOARD_INIT              BoardDebugInit;
  BOARD_BOOT_MODE_DETECT  BoardBootModeDetect;
  BOARD_INIT              BoardInitBeforeMemoryInit;
  BOARD_INIT              BoardInitAfterMemoryInit;
  BOARD_INIT              BoardInitBeforeTempRamExit;
  BOARD_INIT              BoardInitAfterTempRamExit;
} BOARD_PRE_MEM_INIT_FUNC;

typedef struct {
  BOARD_INIT              BoardInitBeforeSiliconInit;
  BOARD_INIT              BoardInitAfterSiliconInit;
} BOARD_POST_MEM_INIT_FUNC;

typedef struct {
  BOARD_INIT              BoardInitAfterPciEnumeration;
  BOARD_INIT              BoardInitReadyToBoot;
  BOARD_INIT              BoardInitEndOfFirmware;
} BOARD_NOTIFICATION_INIT_FUNC;

EFI_STATUS
EFIAPI
RegisterBoardDetect (
  IN BOARD_DETECT_FUNC  *BoardDetect
  );

EFI_STATUS
EFIAPI
RegisterBoardPreMemInit (
  IN BOARD_PRE_MEM_INIT_FUNC  *BoardPreMemInit
  );

EFI_STATUS
EFIAPI
RegisterBoardPostMemInit (
  IN BOARD_POST_MEM_INIT_FUNC  *BoardPostMemInit
  );

EFI_STATUS
EFIAPI
RegisterBoardNotificationInit (
  IN BOARD_NOTIFICATION_INIT_FUNC  *BoardNotificationInit
  );

#endif
