/** @file

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/BoardInitLib.h>
#include <Library/MultiBoardInitSupportLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>

EFI_STATUS
EFIAPI
RegisterBoardNotificationInit (
  IN BOARD_NOTIFICATION_INIT_FUNC  *BoardNotificationInit
  )
{
  EFI_HANDLE  Handle;
  EFI_STATUS  Status;

  Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gBoardNotificationInitGuid,
                  EFI_NATIVE_INTERFACE,
                  BoardNotificationInit
                  );
  ASSERT_EFI_ERROR(Status);

  return EFI_SUCCESS;
}
