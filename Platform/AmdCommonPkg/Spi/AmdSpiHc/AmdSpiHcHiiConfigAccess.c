/*****************************************************************************
 *
 * Copyright (C) 2018-2023 Advanced Micro Devices, Inc. All rights reserved.
 *
 *****************************************************************************/

#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DevicePathLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/HiiLib.h>
#include <Library/BaseLib.h>
#include <Protocol/HiiConfigAccess.h>
#include <Library/UefiDriverEntryPoint.h>
#include "AmdSpiHcNvData.h"
#include "AmdSpiHcInstance.h"

HII_VENDOR_DEVICE_PATH  mHiiVendorDevicePath = {
  {
    {
      HARDWARE_DEVICE_PATH,
      HW_VENDOR_DP,
      {
        (UINT8) (sizeof (VENDOR_DEVICE_PATH)),
        (UINT8) ((sizeof (VENDOR_DEVICE_PATH)) >> 8)
      }
    },
    AMD_SPI_HC_FORMSET_GUID
  },
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    {
      (UINT8) (END_DEVICE_PATH_LENGTH),
      (UINT8) ((END_DEVICE_PATH_LENGTH) >> 8)
    }
  }
};

/**
  This function allows a caller to extract the current configuration for one
  or more named elements from the target driver.

  @param[in]   This              Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param[in]   Request           A null-terminated Unicode string in
                                 <ConfigRequest> format.
  @param[out]  Progress          On return, points to a character in the Request
                                 string. Points to the string's null terminator if
                                 request was successful. Points to the most recent
                                 '&' before the first failing name/value pair (or
                                 the beginning of the string if the failure is in
                                 the first name/value pair) if the request was not
                                 successful.
  @param[out]  Results           A null-terminated Unicode string in
                                 <ConfigAltResp> format which has all values filled
                                 in for the names in the Request string. String to
                                 be allocated by the called function.

  @retval EFI_SUCCESS            The Results is filled with the requested values.
  @retval EFI_OUT_OF_RESOURCES   Not enough memory to store the results.
  @retval EFI_INVALID_PARAMETER  Request is illegal syntax, or unknown name.
  @retval EFI_NOT_FOUND          Routing data doesn't match any storage in this
                                 driver.
**/
EFI_STATUS
EFIAPI
AmdSpiExtractConfig (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
  IN CONST EFI_STRING Request,
  OUT EFI_STRING *Progress,
  OUT EFI_STRING *Results
  )
{
  return EFI_NOT_FOUND;
}

/**
  This function processes the results of changes in configuration.

  @param[in]  This               Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param[in]  Configuration      A null-terminated Unicode string in <ConfigResp>
                                 format.
  @param[out] Progress           A pointer to a string filled in with the offset of
                                 the most recent '&' before the first failing
                                 name/value pair (or the beginning of the string if
                                 the failure is in the first name/value pair) or
                                 the terminating NULL if all was successful.

  @retval EFI_SUCCESS            The Results is processed successfully.
  @retval EFI_INVALID_PARAMETER  Configuration is NULL.
  @retval EFI_NOT_FOUND          Routing data doesn't match any storage in this
                                 driver.
**/
EFI_STATUS
EFIAPI
AmdSpiHcRouteConfig (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
  IN CONST EFI_STRING Configuration,
  OUT EFI_STRING *Progress
  )
{
  return EFI_NOT_FOUND;
}

/**
  This function processes the results of changes in configuration.

  @param[in]  This               Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param[in]  Action             Specifies the type of action taken by the browser.
  @param[in]  QuestionId         A unique value which is sent to the original
                                 exporting driver so that it can identify the type
                                 of data to expect.
  @param[in]  Type               The type of value for the question.
  @param[in]  Value              A pointer to the data being sent to the original
                                 exporting driver.
  @param[out] ActionRequest      On return, points to the action requested by the
                                 callback function.

  @retval EFI_SUCCESS            The callback successfully handled the action.
  @retval EFI_OUT_OF_RESOURCES   Not enough storage is available to hold the
                                 variable and its data.
  @retval EFI_DEVICE_ERROR       The variable could not be saved.
  @retval EFI_UNSUPPORTED        The specified Action is not supported by the
                                 callback.
**/
EFI_STATUS
EFIAPI
AmdSpiHcCallback (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
  IN EFI_BROWSER_ACTION Action,
  IN EFI_QUESTION_ID QuestionId,
  IN UINT8 Type,
  IN OUT EFI_IFR_TYPE_VALUE *Value,
  OUT EFI_BROWSER_ACTION_REQUEST *ActionRequest
  )
{
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
AmdSpiHcHiiUninstallForms (
  SPI_HOST_CONTROLLER_INSTANCE  *Private
  )
{
  EFI_STATUS  Status;

  if (Private->HiiHandle != NULL) {
    HiiRemovePackages (Private->HiiHandle);
  }

  if (Private->Handle != NULL) {
    Status = gBS->UninstallMultipleProtocolInterfaces (
                    &Private->Handle,
                    &gEfiDevicePathProtocolGuid,
                    &mHiiVendorDevicePath,
                    &gEfiHiiConfigAccessProtocolGuid,
                    &Private->ConfigAccess,
                    NULL
                    );
    ASSERT_EFI_ERROR (Status);
  }

  Private->ConfigAccess.ExtractConfig = AmdSpiExtractConfig;
  Private->ConfigAccess.RouteConfig = AmdSpiHcRouteConfig;
  Private->ConfigAccess.Callback = AmdSpiHcCallback;

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
AmdSpiHcHiiInstallForms (
  SPI_HOST_CONTROLLER_INSTANCE  *Private
  )
{
  EFI_STATUS  Status;
  UINTN                 VariableSize;
  AMD_SPI_HC_NV_DATA    AmdSpiHcNvData;
  EFI_STRING            ConfigRequestHdr;
  BOOLEAN               DefaultSuccess;

  Private->ConfigAccess.ExtractConfig = AmdSpiExtractConfig;
  Private->ConfigAccess.RouteConfig = AmdSpiHcRouteConfig;
  Private->ConfigAccess.Callback = AmdSpiHcCallback;

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Private->Handle,
                  &gEfiDevicePathProtocolGuid,
                  &mHiiVendorDevicePath,
                  &gEfiHiiConfigAccessProtocolGuid,
                  &Private->ConfigAccess,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Publish our HII data
  //
  Private->HiiHandle = HiiAddPackages (
                        &mAmdSpiHcFormSetGuid,
                        Private->Handle,
                        AmdSpiHcProtocolDxeStrings,
                        AmdSpiHcFormBin,
                        NULL
                        );
  if (Private->HiiHandle == NULL) {
    AmdSpiHcHiiUninstallForms (Private);
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Initialize efi variable storage
  //
  ZeroMem (&AmdSpiHcNvData, sizeof (AMD_SPI_HC_NV_DATA));

  ConfigRequestHdr = HiiConstructConfigHdr (
                        &mAmdSpiHcFormSetGuid,
                        mAmdSpiHcNvDataVar,
                        Private->Handle
                        );
  ASSERT (ConfigRequestHdr != NULL);

  // Validate the UEFI Variable
  VariableSize = sizeof (AMD_SPI_HC_NV_DATA);
  Status = gRT->GetVariable (
                  mAmdSpiHcNvDataVar,
                  &mAmdSpiHcFormSetGuid,
                  NULL,
                  &VariableSize,
                  &AmdSpiHcNvData
                  );
  if (!EFI_ERROR (Status)) {
    //
    // EFI variable does exist, validate Current Setting
    //
    DefaultSuccess = HiiValidateSettings (ConfigRequestHdr);
    if (!DefaultSuccess) {
      AmdSpiHcHiiUninstallForms (Private);
      return EFI_INVALID_PARAMETER;
    }
  } else {
    //
    // Variable must exist for HII to interact with.
    // Store zero data to EFI variable Storage.
    //
    Status = gRT->SetVariable(
                    mAmdSpiHcNvDataVar,
                    &mAmdSpiHcFormSetGuid,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                    sizeof (AMD_SPI_HC_NV_DATA),
                    &AmdSpiHcNvData
                    );
    if (EFI_ERROR (Status)) {
      AmdSpiHcHiiUninstallForms (Private);
      return Status;
    }
    //
    // Blank UEFI variable now exists, now build and have HII store the correct
    // default values from the IFR.
    //
    DefaultSuccess = HiiSetToDefaults (
                       ConfigRequestHdr,
                       EFI_HII_DEFAULT_CLASS_STANDARD
                       );
    if (!DefaultSuccess) {
      AmdSpiHcHiiUninstallForms (Private);
      return EFI_INVALID_PARAMETER;
    }
  }
  FreePool (ConfigRequestHdr);

  return EFI_SUCCESS;
}
