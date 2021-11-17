/** @file
  USB Lan Driver Component Name definitions

  Copyright (c) 2021, American Megatrends International LLC.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "UsbLanDriver.h"

extern EFI_DRIVER_BINDING_PROTOCOL  gUsbLanDriverBinding;
extern EFI_GUID gUsbEthProtocolGuid;

GLOBAL_REMOVE_IF_UNREFERENCED
EFI_UNICODE_STRING_TABLE gUsbLanDriverNameTable[] = {
  {
    "eng;en",
    L"USB LAN Driver"
  },
  {
    NULL,
    NULL
  }
};

GLOBAL_REMOVE_IF_UNREFERENCED
EFI_UNICODE_STRING_TABLE  *gUsbLanControllerNameTable = NULL;

EFI_STATUS
EFIAPI
UsbLanComponentNameGetDriverName (
    IN  EFI_COMPONENT_NAME_PROTOCOL  *This,
    IN  CHAR8                        *Language,
    OUT CHAR16                       **DriverName
);

EFI_STATUS
EFIAPI
UsbLanComponentNameGetControllerName (
    IN EFI_COMPONENT_NAME_PROTOCOL     *This,
    IN EFI_HANDLE                      Controller,
    IN EFI_HANDLE                      ChildHandle        OPTIONAL,
    IN CHAR8                           *Language,
    OUT CHAR16                         **ControllerName
);

GLOBAL_REMOVE_IF_UNREFERENCED
EFI_COMPONENT_NAME_PROTOCOL  gUsbLanComponentName = {
    UsbLanComponentNameGetDriverName,
    UsbLanComponentNameGetControllerName,
    "eng"
};

GLOBAL_REMOVE_IF_UNREFERENCED EFI_COMPONENT_NAME2_PROTOCOL gUsbLanComponentName2 = {
    (EFI_COMPONENT_NAME2_GET_DRIVER_NAME)UsbLanComponentNameGetDriverName,
    (EFI_COMPONENT_NAME2_GET_CONTROLLER_NAME)UsbLanComponentNameGetControllerName,
    "en"
};

/**
    Retrieves a Unicode string that is the user readable name of
    the EFI Driver.


    @param
        This       - A pointer to the EFI_COMPONENT_NAME_PROTOCOL instance.
        Language   - A pointer to a three character ISO 639-2 language identifier.
        This is the language of the driver name that that the caller
        is requesting, and it must match one of the languages specified
        in SupportedLanguages.  The number of languages supported by a
        driver is up to the driver writer.
        DriverName - A pointer to the Unicode string to return.  This Unicode string
        is the name of the driver specified by This in the language
        specified by Language.


    @retval EFI_SUCCES The Unicode string for the Driver specified by This
        and the language specified by Language was returned
        in DriverName.
    @retval EFI_INVALID_PARAMETER Language is NULL.
    @retval EFI_INVALID_PARAMETER DriverName is NULL.
    @retval EFI_UNSUPPORTED The driver specified by This does not support the
        language specified by Language.
**/
EFI_STATUS
EFIAPI
UsbLanComponentNameGetDriverName (
    IN  EFI_COMPONENT_NAME_PROTOCOL  *This,
    IN  CHAR8                        *Language,
    OUT CHAR16                       **DriverName
)
{
    return LookupUnicodeString2 (
                    Language,
                    This->SupportedLanguages,
                    gUsbLanDriverNameTable,
                    DriverName,
                    (BOOLEAN) (This == &gUsbLanComponentName) );
}

/**
    Retrieves a Unicode string that is the user readable name of
    the controller that is being managed by an EFI Driver.

    @param
        This             - A pointer to the EFI_COMPONENT_NAME_PROTOCOL instance.
        ControllerHandle - The handle of a controller that the driver specified by
        This is managing.  This handle specifies the controller
        whose name is to be returned.
        ChildHandle      - The handle of the child controller to retrieve the name
        of.  This is an optional parameter that may be NULL.  It
        will be NULL for device drivers.  It will also be NULL
        for a bus drivers that wish to retrieve the name of the
        bus controller.  It will not be NULL for a bus driver
        that wishes to retrieve the name of a child controller.
        Language         - A pointer to a three character ISO 639-2 language
        identifier.  This is the language of the controller name
        that that the caller is requesting, and it must match one
        of the languages specified in SupportedLanguages.  The
        number of languages supported by a driver is up to the
        driver writer.
        ControllerName   - A pointer to the Unicode string to return.  This Unicode
        string is the name of the controller specified by
        ControllerHandle and ChildHandle in the language
        specified by Language from the point of view of the
        driver specified by This.


    @retval EFI_SUCCESS The Unicode string for the user readable name in the
        language specified by Language for the driver
        specified by This was returned in DriverName.
    @retval EFI_INVALID_PARAMETER ControllerHandle is not a valid EFI_HANDLE.
    @retval EFI_INVALID_PARAMETER ChildHandle is not NULL and it is not a valid
        EFI_HANDLE.
    @retval EFI_INVALID_PARAMETER Language is NULL.
    @retval EFI_INVALID_PARAMETER ControllerName is NULL.
    @retval EFI_UNSUPPORTED The driver specified by This is not currently
        managing the controller specified by
        ControllerHandle and ChildHandle.
    @retval EFI_UNSUPPORTED The driver specified by This does not support the
        language specified by Language.

**/
EFI_STATUS
EFIAPI
UsbLanComponentNameGetControllerName (
    IN EFI_COMPONENT_NAME_PROTOCOL     *This,
    IN EFI_HANDLE                      Controller,
    IN EFI_HANDLE                      ChildHandle        OPTIONAL,
    IN CHAR8                           *Language,
    OUT CHAR16                         **ControllerName
)
{
    EFI_STATUS                Status;
    CHAR16                    *HandleName;
    EFI_USB_IO_PROTOCOL       *UsbIo = NULL;
    EFI_USB_DEVICE_DESCRIPTOR DevDesc;

    if ((Language == NULL) || (ControllerName == NULL)) {
        return EFI_INVALID_PARAMETER;
    }

    // Make sure this driver is currently managing ControllerHandle
    Status = EfiTestManagedDevice (
                            Controller,
                            gUsbLanDriverBinding.DriverBindingHandle,
                            &gUsbEthProtocolGuid );
    if (EFI_ERROR (Status)) {
        return Status;
    }

    // Make sure this driver produced ChildHandle
    Status = EfiTestChildHandle (
                        Controller,
                        ChildHandle,
                        &gUsbEthProtocolGuid );
    if (EFI_ERROR (Status)) {
        return Status;
    }

    Status = gBS->HandleProtocol (
                            Controller,
                            &gEfiUsbIoProtocolGuid,
                            (VOID**)&UsbIo);
    if (EFI_ERROR(Status)) {
        return Status;
    }

    Status = UsbIo->UsbGetDeviceDescriptor (UsbIo, &DevDesc);
    if (EFI_ERROR(Status)) {
        return Status;
    }

    Status = UsbIo->UsbGetStringDescriptor (
                                    UsbIo,
                                    0x409,
                                    DevDesc.StrManufacturer,
                                    &HandleName );
    if (EFI_ERROR(Status)) {
        return Status;
    }

    *ControllerName = HandleName;

    if (gUsbLanControllerNameTable != NULL) {
        FreeUnicodeStringTable (gUsbLanControllerNameTable);
        gUsbLanControllerNameTable = NULL;
    }

    Status = AddUnicodeString2 (
                        "eng",
                        gUsbLanComponentName.SupportedLanguages,
                        &gUsbLanControllerNameTable,
                        HandleName,
                        TRUE );
    if (EFI_ERROR(Status)) {
        return Status;
    }

    Status = AddUnicodeString2 (
                        "en",
                        gUsbLanComponentName2.SupportedLanguages,
                        &gUsbLanControllerNameTable,
                        HandleName,
                        FALSE );
    if (EFI_ERROR(Status)) {
        return Status;
    }

    return LookupUnicodeString2 (
                    Language,
                    This->SupportedLanguages,
                    gUsbLanControllerNameTable,
                    ControllerName,
                    (BOOLEAN)(This == &gUsbLanComponentName) );
}
