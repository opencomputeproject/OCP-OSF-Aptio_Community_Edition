/** @file
  USB Remote Network Driver Interface Spec. Driver Binding

  Copyright (c) 2021, American Megatrends International LLC.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <UsbRndisDriver.h>

extern EFI_COMPONENT_NAME2_PROTOCOL gUsbRndisComponentName2;

EFI_DRIVER_BINDING_PROTOCOL gUsbRndisDriverBinding = {
    UsbRndisDriverSupported,
    UsbRndisDriverStart,
    UsbRndisDriverStop,
    FixedPcdGet32 (PcdRndisDriverVersion),
    NULL,
    NULL
};


/**
  Check if this interface is USB Rndis SubType

  @param  UsbIo  A pointer to the EFI_USB_IO_PROTOCOL instance.

  @retval TRUE   USB Rndis SubType.
  @retval FALSE  Not USB Rndis SubType.
**/

BOOLEAN
IsSupportedDevice (
    IN EFI_USB_IO_PROTOCOL *UsbIo
)
{
    EFI_STATUS                      Status;
    EFI_USB_INTERFACE_DESCRIPTOR    InterfaceDescriptor;

    Status = UsbIo->UsbGetInterfaceDescriptor (
                                            UsbIo,
                                            &InterfaceDescriptor );
    if (EFI_ERROR(Status)) {
        return FALSE;
    }

    // Check for RNDIS device and CDC-DATA.
    if (((InterfaceDescriptor.InterfaceClass == USB_CDC_CONTROL_INTERFACE_BASE_CLASS) &&
         (InterfaceDescriptor.InterfaceSubClass == USB_CDC_CONTROL_INTERFACE_SUB_CLASS) &&
         (InterfaceDescriptor.InterfaceProtocol == USB_CDC_CONTROL_INTERFACE_PROTOCOL)) ||
        ((InterfaceDescriptor.InterfaceClass == USB_CDC_DATA_INTERFACE_BASE_CLASS) &&
         (InterfaceDescriptor.InterfaceSubClass == USB_CDC_DATA_INTERFACE_SUB_CLASS) &&
         (InterfaceDescriptor.InterfaceProtocol == USB_CDC_DATA_INTERFACE_PROTOCOL)) ||
        ((InterfaceDescriptor.InterfaceClass == USB_RNDIS_INTERFACE_BASE_CLASS) &&
         (InterfaceDescriptor.InterfaceSubClass == USB_RNDIS_INTERFACE_SUB_CLASS) &&
         (InterfaceDescriptor.InterfaceProtocol == USB_RNDIS_INTERFACE_PROTOCOL))) {
        return TRUE;
    }

    return FALSE;
}

/**
  Check if the USB Rndis and USB CDC Data is the same

  @param  UsbRndisDataPath    A pointer to RNDIS EFI_DEVICE_PATH_PROTOCOL instance.
  @param  UsbCdcDataPath      A pointer to CDC EFI_DEVICE_PATH_PROTOCOL instance.

  @retval EFI_SUCCESS               Is the same device.
  @retval EFI_UNSUPPORTED           Is not the same device.
**/

EFI_STATUS
IsSameDevice (
    EFI_DEVICE_PATH_PROTOCOL    *UsbRndisDataPath,
    EFI_DEVICE_PATH_PROTOCOL    *UsbCdcDataPath
)
{
    while (TRUE) {
        if (IsDevicePathEnd (NextDevicePathNode (UsbRndisDataPath))) {
            if (((USB_DEVICE_PATH*)UsbRndisDataPath)->ParentPortNumber ==
                ((USB_DEVICE_PATH*)UsbCdcDataPath)->ParentPortNumber) {
                return EFI_SUCCESS;
            } else {
                break;
            }
        } else {
            if (CompareMem (UsbCdcDataPath, UsbRndisDataPath, sizeof(EFI_DEVICE_PATH_PROTOCOL)) != 0) {
                break;
            }
            UsbRndisDataPath = NextDevicePathNode(UsbRndisDataPath);
            UsbCdcDataPath   = NextDevicePathNode(UsbCdcDataPath);
        }
    }

    return EFI_UNSUPPORTED;
}

/**
    Check if the USB CDC Data(UsbIo) installed and return USB CDC Data Handle.

    @param  UsbIo        A pointer to the EFI_USB_IO_PROTOCOL instance.

    @retval TRUE         USB CDC Data(UsbIo) installed.
    @retval FALSE        USB CDC Data(UsbIo) did not installed.
**/
BOOLEAN
IsUsbCdcData (
    IN EFI_USB_IO_PROTOCOL  *UsbIo
)
{
    EFI_STATUS                      Status;
    EFI_USB_INTERFACE_DESCRIPTOR    InterfaceDescriptor;

    Status = UsbIo->UsbGetInterfaceDescriptor (
                                            UsbIo,
                                            &InterfaceDescriptor );
    if (EFI_ERROR (Status)) {
        return FALSE;
    }

    // Check for CDC-DATA
    if ((InterfaceDescriptor.InterfaceClass == USB_CDC_DATA_INTERFACE_BASE_CLASS) &&
        (InterfaceDescriptor.InterfaceSubClass == USB_CDC_DATA_INTERFACE_SUB_CLASS) &&
        (InterfaceDescriptor.InterfaceProtocol == USB_CDC_DATA_INTERFACE_PROTOCOL)) {
        return TRUE;
    }

    return FALSE;
}

/**
    Check if the USB Rndis(UsbIo) installed

    @param  UsbIo        A pointer to the EFI_USB_IO_PROTOCOL instance.

    @retval TRUE         USB Rndis(UsbIo) installed.
    @retval FALSE        USB Rndis(UsbIo) did not installed.
**/

BOOLEAN
IsUsbRndis (
    IN EFI_USB_IO_PROTOCOL  *UsbIo
)
{
    EFI_STATUS                      Status;
    EFI_USB_INTERFACE_DESCRIPTOR    InterfaceDescriptor;

    Status = UsbIo->UsbGetInterfaceDescriptor (
                                        UsbIo,
                                        &InterfaceDescriptor );
    if (EFI_ERROR (Status)) {
        return FALSE;
    }

    // Check for RNDIS device.
    if ((InterfaceDescriptor.InterfaceClass == USB_CDC_CONTROL_INTERFACE_BASE_CLASS) &&
        (InterfaceDescriptor.InterfaceSubClass == USB_CDC_CONTROL_INTERFACE_SUB_CLASS) &&
        (InterfaceDescriptor.InterfaceProtocol == USB_CDC_CONTROL_INTERFACE_PROTOCOL)) {
        return TRUE;
    }

    return FALSE;
}

/**
    Control comes here when a CDC device is found.Check if a RNDIS interface is already found for this device or not.
    For one device two USBIO will be installed each for CDC and RNDIS interface.

    @param  UsbCdcDataPath    A pointer to the EFI_DEVICE_PATH_PROTOCOL instance.
    @param  UsbRndisDevice    A pointer to the USB_RNDIS_DEVICE

    @retval EFI_STATUS
**/
EFI_STATUS
UpdateRndisDevice (
    IN EFI_DEVICE_PATH_PROTOCOL        *UsbCdcDataPath,
    USB_RNDIS_DEVICE                   **UsbRndisDevice
)
{
    EFI_STATUS                      Status;
    UINTN                           Index;
    UINTN                           HandleCount;
    EFI_HANDLE                      *HandleBuffer;
    EFI_DEVICE_PATH_PROTOCOL        *UsbRndisDataPath;
    USB_ETHERNET_PROTOCOL           *UsbEthDevice;

    Status = gBS->LocateHandleBuffer(
                                ByProtocol,
                                &gUsbEthProtocolGuid,
                                NULL,
                                &HandleCount,
                                &HandleBuffer );
    if (EFI_ERROR(Status)) {
        return Status;
    }

    for (Index = 0; Index < HandleCount; Index++) {
        Status = gBS->HandleProtocol (
                            HandleBuffer[Index],
                            &gUsbEthProtocolGuid,
                            (VOID**)&UsbEthDevice );
        if (EFI_ERROR (Status)) {
            continue;
        }

        *UsbRndisDevice = USB_RNDIS_DEVICE_FROM_THIS (UsbEthDevice);

        Status = gBS->HandleProtocol (
                            HandleBuffer[Index],
                            &gEfiDevicePathProtocolGuid,
                            (VOID**)&UsbRndisDataPath );
        if (EFI_ERROR (Status)) {
            continue;
        }

        Status = IsSameDevice (
                        UsbRndisDataPath,
                        UsbCdcDataPath );
        if (!EFI_ERROR (Status)) {
            FreePool (HandleBuffer);
            return EFI_SUCCESS;
        }
    } // End of For loop

    FreePool (HandleBuffer);
    return EFI_NOT_FOUND;
}

/**
    For the given Rndis Device, find a matching CDC device already exists or not. If found update the handle
    and UsbIO protocol.

    @param  UsbRndisDevice        A pointer to the EFI_DEVICE_PATH_PROTOCOL instance.

    @retval None
**/
VOID
FindMatchingCdcData (
    USB_RNDIS_DEVICE  *UsbRndisDevice
)
{
    EFI_STATUS                  Status;
    UINTN                       Index;
    UINTN                       HandleCount;
    EFI_HANDLE                  *HandleBuffer;
    EFI_USB_IO_PROTOCOL         *UsbIo;
    EFI_DEVICE_PATH_PROTOCOL    *UsbRndisDataPath;
    EFI_DEVICE_PATH_PROTOCOL    *UsbCdcDataPath;

    // Find the parent RNDIS and update the UsbIo for the CDC device
    Status = gBS->HandleProtocol (
                        UsbRndisDevice->UsbRndisHandle,
                        &gEfiDevicePathProtocolGuid,
                        (VOID **)&UsbRndisDataPath );
    if (EFI_ERROR (Status)) {
        return;
    }

    Status = gBS->LocateHandleBuffer (
                            ByProtocol,
                            &gEfiUsbIoProtocolGuid,
                            NULL,
                            &HandleCount,
                            &HandleBuffer );
    if (EFI_ERROR (Status)) {
        return;
    }

    for (Index = 0; Index < HandleCount; Index++) {
        Status = gBS->HandleProtocol (
                        HandleBuffer[Index],
                        &gEfiUsbIoProtocolGuid,
                        (VOID**)&UsbIo );
        if (EFI_ERROR (Status)) {
            continue;
        }

        if (!IsUsbCdcData (UsbIo)) {
            continue;
        }

        Status = gBS->HandleProtocol (
                        HandleBuffer[Index],
                        &gEfiDevicePathProtocolGuid,
                        (VOID**)&UsbCdcDataPath );
        if (EFI_ERROR (Status)) {
            continue;
        }

        Status = IsSameDevice (
                        UsbRndisDataPath,
                        UsbCdcDataPath );
        if (!EFI_ERROR (Status)) {
            UsbRndisDevice->UsbIoCdcData     = UsbIo;
            UsbRndisDevice->UsbCdcDataHandle = HandleBuffer[Index];
            GetEndpoint (UsbRndisDevice->UsbIoCdcData, UsbRndisDevice);
            break;
        }
    } //End of For loop

    FreePool (HandleBuffer);
}

/**
  For the given UsbIo CdcData, find a matching Rndis device already exists or not.

  @param  CdcHandle
  @param  CdcUsbIo
  @param  RndisHandle

  @retval EFI_STATUS
**/
EFI_STATUS
FindMatchingRndisDev (
    EFI_HANDLE              CdcHandle,
    EFI_USB_IO_PROTOCOL     **CdcUsbIo,
    EFI_HANDLE              *RndisHandle
)
{
    EFI_STATUS                  Status;
    UINTN                       Index;
    UINTN                       HandleCount;
    EFI_HANDLE                  *HandleBuffer;
    EFI_USB_IO_PROTOCOL         *UsbIo;
    EFI_DEVICE_PATH_PROTOCOL    *UsbRndisDataPath;
    EFI_DEVICE_PATH_PROTOCOL    *UsbCdcDataPath;

    //Find the parent RNDIS and update the UsbIo for the CDC device
    Status = gBS->HandleProtocol (
                            CdcHandle,
                            &gEfiDevicePathProtocolGuid,
                            (VOID**)&UsbCdcDataPath );
    if (EFI_ERROR (Status)) {
        return Status;
    }

    Status = gBS->LocateHandleBuffer (
                            ByProtocol,
                            &gEfiUsbIoProtocolGuid,
                            NULL,
                            &HandleCount,
                            &HandleBuffer );
    if (EFI_ERROR (Status)) {
        return Status;
    }

    for (Index = 0; Index < HandleCount; Index++) {
        Status = gBS->HandleProtocol (
                        HandleBuffer[Index],
                        &gEfiUsbIoProtocolGuid,
                        (VOID**)&UsbIo );
        if (EFI_ERROR (Status)) {
            continue;
        }

        if (!IsUsbRndis (UsbIo)) {
            continue;
        }

        Status = gBS->HandleProtocol (
                        HandleBuffer[Index],
                        &gEfiDevicePathProtocolGuid,
                        (VOID **)&UsbRndisDataPath );
        if (EFI_ERROR (Status)) {
            continue;
        }

        Status = IsSameDevice (
                        UsbRndisDataPath,
                        UsbCdcDataPath );
        if (!EFI_ERROR (Status)) {
            *CdcUsbIo    = UsbIo;
            *RndisHandle = HandleBuffer[Index];
            FreePool (HandleBuffer);
            return Status;
        }
    } //End of For loop

    FreePool (HandleBuffer);
    return EFI_NOT_FOUND;
}

/**
  USB Rndis Driver Binding Support.

  @param  This                Protocol instance pointer.
  @param  ControllerHandle    Handle of device to test.
  @param  RemainingDevicePath Optional parameter use to pick a specific child
                              device to start.

  @retval EFI_SUCCESS         This driver supports this device.
  @retval EFI_ALREADY_STARTED This driver is already running on this device.
  @retval other               This driver does not support this device.

**/
EFI_STATUS
EFIAPI
UsbRndisDriverSupported (
    IN EFI_DRIVER_BINDING_PROTOCOL    *This,
    IN EFI_HANDLE                     ControllerHandle,
    IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
)
{
    EFI_STATUS              Status;
    EFI_USB_IO_PROTOCOL     *UsbIo;

    Status = gBS->OpenProtocol (
                    ControllerHandle,
                    &gEfiUsbIoProtocolGuid,
                    (VOID**)&UsbIo,
                    This->DriverBindingHandle,
                    ControllerHandle,
                    EFI_OPEN_PROTOCOL_BY_DRIVER );
    if (EFI_ERROR (Status)) {
        return Status;
    }

    Status = IsSupportedDevice (UsbIo) ? EFI_SUCCESS : EFI_UNSUPPORTED;

    gBS->CloseProtocol (
            ControllerHandle,
            &gEfiUsbIoProtocolGuid,
            This->DriverBindingHandle,
            ControllerHandle );
    return Status;
}

/**
  USB Rndis Driver Binding Start.

  @param  This                 Protocol instance pointer.
  @param  ControllerHandle     Handle of device to bind driver to.
  @param  RemainingDevicePath  Optional parameter use to pick a specific child
                               device to start.

  @retval EFI_SUCCESS          This driver is added to ControllerHandle
  @retval EFI_DEVICE_ERROR     This driver could not be started due to a device error
  @retval other                This driver does not support this device;
**/
EFI_STATUS
EFIAPI
UsbRndisDriverStart (
    IN EFI_DRIVER_BINDING_PROTOCOL    *This,
    IN EFI_HANDLE                     ControllerHandle,
    IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
)
{
    EFI_STATUS                      Status;
    USB_RNDIS_DEVICE                *UsbRndisDevice;
    EFI_DEVICE_PATH_PROTOCOL        *UsbEthPath;
    EFI_USB_IO_PROTOCOL             *UsbIo;
    EFI_USB_INTERFACE_DESCRIPTOR    Interface;
    EFI_HANDLE                      RndisHandle = ControllerHandle;

    Status = gBS->OpenProtocol (
                        ControllerHandle,
                        &gEfiUsbIoProtocolGuid,
                        (VOID**)&UsbIo,
                        This->DriverBindingHandle,
                        ControllerHandle,
                        EFI_OPEN_PROTOCOL_BY_DRIVER );
    if (EFI_ERROR(Status)) {
        return Status;
    }

    Status = gBS->OpenProtocol (
                    ControllerHandle,
                    &gEfiDevicePathProtocolGuid,
                    (VOID**)&UsbEthPath,
                    This->DriverBindingHandle,
                    ControllerHandle,
                    EFI_OPEN_PROTOCOL_BY_DRIVER );
    if (EFI_ERROR (Status)) {
        gBS->CloseProtocol (
                    ControllerHandle,
                    &gEfiUsbIoProtocolGuid,
                    This->DriverBindingHandle,
                    ControllerHandle );
        return Status;
    }

    gBS->CloseProtocol (
                ControllerHandle,
                &gEfiDevicePathProtocolGuid,
                This->DriverBindingHandle,
                ControllerHandle );

    // Controls come here for RNDIS and CDC. If it is CDC, check whether RNDIS is present on the same controller or not.
    if (IsUsbCdcData (UsbIo)) {

        DEBUG((DEBUG_VERBOSE, "Rndis CDCData interface found\n"));

        //Find the parent RNDIS and update the UsbIo for the CDC device
        Status = UpdateRndisDevice (
                            UsbEthPath,
                            &UsbRndisDevice );
        if (!EFI_ERROR (Status)) {
            DEBUG((DEBUG_VERBOSE, "Rndis Matching interface found\n"));
            UsbRndisDevice->UsbIoCdcData     = UsbIo;
            UsbRndisDevice->UsbCdcDataHandle = ControllerHandle;
            GetEndpoint (
                    UsbRndisDevice->UsbIoCdcData,
                    UsbRndisDevice );
            return Status;
        } else {
            // Check if RnDis exist
            Status = FindMatchingRndisDev (
                                ControllerHandle,
                                &UsbIo,
                                &RndisHandle );
            if (EFI_ERROR(Status)) {
                gBS->CloseProtocol(
                            ControllerHandle,
                            &gEfiUsbIoProtocolGuid,
                            This->DriverBindingHandle,
                            ControllerHandle );
                return Status;
            }
        }
    }

    UsbRndisDevice = AllocateZeroPool (sizeof(USB_RNDIS_DEVICE));
    if (UsbRndisDevice == NULL) {
        Status = EFI_OUT_OF_RESOURCES;
        goto Error;
    }

    Status = LoadAllDescriptor (
                            UsbIo,
                            &UsbRndisDevice->Config );
    if (EFI_ERROR (Status)) {
        goto Error;
    }

    Status = UsbIo->UsbGetInterfaceDescriptor (
                                            UsbIo,
                                            &Interface );
    if (EFI_ERROR(Status)) {
        goto Error;
    }

    UsbRndisDevice->Signature                          = USB_RNDIS_SIGNATURE;
    UsbRndisDevice->NumOfInterface                     = Interface.InterfaceNumber;
    UsbRndisDevice->UsbRndisHandle                     = RndisHandle;
    UsbRndisDevice->UsbCdcDataHandle                   = 0;
    UsbRndisDevice->UsbIo                              = UsbIo;
    UsbRndisDevice->UsbEth.UsbEthReceive               = RndisUndiReceive;
    UsbRndisDevice->UsbEth.UsbEthTransmit              = RndisUndiTransmit;
    UsbRndisDevice->UsbEth.UsbEthInterrupt             = UsbRndisInterrupt;
    UsbRndisDevice->UsbEth.UsbEthMacAddress            = GetUsbEthMacAddress;
    UsbRndisDevice->UsbEth.UsbEthMaxBulkSize           = UsbEthBulkSize;
    UsbRndisDevice->UsbEth.UsbHeaderFuncDescriptor     = GetUsbHeaderFunDescriptor;
    UsbRndisDevice->UsbEth.UsbUnionFuncDescriptor      = GetUsbUnionFunDescriptor;
    UsbRndisDevice->UsbEth.UsbEthFuncDescriptor        = GetUsbRndisFunDescriptor;
    UsbRndisDevice->UsbEth.SetUsbEthMcastFilter        = SetUsbRndisMcastFilter;
    UsbRndisDevice->UsbEth.SetUsbEthPowerPatternFilter = SetUsbRndisPowerFilter;
    UsbRndisDevice->UsbEth.GetUsbEthPoewrPatternFilter = GetUsbRndisPowerFilter;
    UsbRndisDevice->UsbEth.SetUsbEthPacketFilter       = SetUsbRndisPacketFilter;
    UsbRndisDevice->UsbEth.GetUsbEthStatistic          = GetRndisStatistic;


    UsbRndisDevice->UsbEth.UsbEthUndi.UsbEthUndiGetState          = RndisDummyReturn;
    UsbRndisDevice->UsbEth.UsbEthUndi.UsbEthUndiStart             = RndisUndiStart;
    UsbRndisDevice->UsbEth.UsbEthUndi.UsbEthUndiStop              = RndisUndiStop;
    UsbRndisDevice->UsbEth.UsbEthUndi.UsbEthUndiGetInitInfo       = RndisUndiGetInitInfo;
    UsbRndisDevice->UsbEth.UsbEthUndi.UsbEthUndiGetConfigInfo     = RndisUndiGetConfigInfo;
    UsbRndisDevice->UsbEth.UsbEthUndi.UsbEthUndiInitialize        = RndisUndiInitialize;
    UsbRndisDevice->UsbEth.UsbEthUndi.UsbEthUndiReset             = RndisUndiReset;
    UsbRndisDevice->UsbEth.UsbEthUndi.UsbEthUndiShutdown          = RndisUndiShutdown;
    UsbRndisDevice->UsbEth.UsbEthUndi.UsbEthUndiInterruptEnable   = RndisDummyReturn;
    UsbRndisDevice->UsbEth.UsbEthUndi.UsbEthUndiReceiveFilter     = RndisUndiReceiveFilter;
    UsbRndisDevice->UsbEth.UsbEthUndi.UsbEthUndiStationAddress    = RndisDummyReturn;
    UsbRndisDevice->UsbEth.UsbEthUndi.UsbEthUndiStatistics        = NULL;
    UsbRndisDevice->UsbEth.UsbEthUndi.UsbEthUndiMcastIp2Mac       = RndisDummyReturn;
    UsbRndisDevice->UsbEth.UsbEthUndi.UsbEthUndiNvData            = RndisDummyReturn;
    UsbRndisDevice->UsbEth.UsbEthUndi.UsbEthUndiGetStatus         = RndisUndiGetStatus;
    UsbRndisDevice->UsbEth.UsbEthUndi.UsbEthUndiFillHeader        = RndisDummyReturn;
    UsbRndisDevice->UsbEth.UsbEthUndi.UsbEthUndiTransmit          = NULL;
    UsbRndisDevice->UsbEth.UsbEthUndi.UsbEthUndiReceive           = NULL;

    UsbRndisDevice->MaxTransferSize       = FixedPcdGet32 (PcdMaxTransferSize);
    UsbRndisDevice->MaxPacketsPerTransfer = 1;
    UsbRndisDevice->PacketAlignmentFactor = 0;

    InitializeListHead (&UsbRndisDevice->ReceivePacketList);

    // This is a RNDIS interface. See whether CDC-DATA interface has already been connected or not
    FindMatchingCdcData (UsbRndisDevice);

    if (UsbRndisDevice->UsbIoCdcData != NULL) {
        Status = gBS->InstallProtocolInterface (
                        &ControllerHandle,
                        &gUsbEthProtocolGuid,
                        EFI_NATIVE_INTERFACE,
                        &(UsbRndisDevice->UsbEth) );
        if (EFI_ERROR (Status)) {
            goto Error;
        }

        GetEndpoint (UsbRndisDevice->UsbIo, UsbRndisDevice);
        return EFI_SUCCESS;
    }

Error:
    gBS->CloseProtocol (
            ControllerHandle,
            &gEfiUsbIoProtocolGuid,
            This->DriverBindingHandle,
            ControllerHandle );

    if (UsbRndisDevice != NULL) {
        if (UsbRndisDevice->Config != NULL) {
            FreePool (UsbRndisDevice->Config);
        }

        FreePool (UsbRndisDevice);
    }

    return Status;
}

/**
  CheckandStopRndisDevice

  @param  This                 Protocol instance pointer.
  @param  ControllerHandle     Handle of device to bind driver to.

  @retval EFI_SUCCESS          This driver is added to ControllerHandle
  @retval EFI_DEVICE_ERROR     This driver could not be started due to a device error
  @retval other                This driver does not support this device
**/
EFI_STATUS
CheckandStopRndisDevice (
    IN  EFI_DRIVER_BINDING_PROTOCOL    *This,
    IN  EFI_HANDLE                     ControllerHandle
)
{
    EFI_STATUS             Status;
    EFI_USB_IO_PROTOCOL    *UsbIo;

    Status = gBS->OpenProtocol (
                        ControllerHandle,
                        &gEfiUsbIoProtocolGuid,
                        (VOID**)&UsbIo,
                        This->DriverBindingHandle,
                        ControllerHandle,
                        EFI_OPEN_PROTOCOL_GET_PROTOCOL );
    if (EFI_ERROR(Status)) {
        return Status;
    }

    if (IsUsbRndis (UsbIo)) {
        Status = gBS->CloseProtocol (
                            ControllerHandle,
                            &gEfiUsbIoProtocolGuid,
                            This->DriverBindingHandle,
                            ControllerHandle );
        return Status;
    }

    return EFI_UNSUPPORTED;
}

/**
    USB Rndis Driver Binding Stop.

    @param  This              Protocol instance pointer.
    @param  ControllerHandle  Handle of device to stop driver on
    @param  NumberOfChildren  Number of Handles in ChildHandleBuffer. If number of
                            children is zero stop the entire bus driver.
    @param  ChildHandleBuffer List of Child Handles to Stop.

    @retval EFI_SUCCESS       This driver is removed ControllerHandle
    @retval other             This driver was not removed from this device
**/
EFI_STATUS
EFIAPI
UsbRndisDriverStop (
    IN  EFI_DRIVER_BINDING_PROTOCOL    *This,
    IN  EFI_HANDLE                     ControllerHandle,
    IN  UINTN                          NumberOfChildren,
    IN  EFI_HANDLE                     *ChildHandleBuffer
)
{
    EFI_STATUS                 Status;
    USB_RNDIS_DEVICE           *UsbRndisDevice;
    USB_ETHERNET_PROTOCOL      *UsbEthProtocol;

    Status = gBS->OpenProtocol (
                        ControllerHandle,
                        &gUsbEthProtocolGuid,
                        (VOID**)&UsbEthProtocol,
                        This->DriverBindingHandle,
                        ControllerHandle,
                        EFI_OPEN_PROTOCOL_GET_PROTOCOL );
    if (EFI_ERROR (Status)) {
        Status = CheckandStopRndisDevice (This, ControllerHandle);
        return Status;
    }

    UsbRndisDevice = USB_RNDIS_DEVICE_FROM_THIS(UsbEthProtocol);

    gBS->CloseProtocol (
                UsbRndisDevice->UsbCdcDataHandle,
                &gEfiUsbIoProtocolGuid,
                This->DriverBindingHandle,
                UsbRndisDevice->UsbCdcDataHandle );

    Status = gBS->UninstallProtocolInterface (
                    ControllerHandle,
                    &gUsbEthProtocolGuid,
                    UsbEthProtocol );
    if (EFI_ERROR (Status)) {
        return Status;
    }

    Status = gBS->CloseProtocol (
                    ControllerHandle,
                    &gEfiUsbIoProtocolGuid,
                    This->DriverBindingHandle,
                    ControllerHandle );

    FreePool (UsbRndisDevice->Config);
    FreePool (UsbRndisDevice);
    return Status;
}

/**
    USB Rndis Entry Point.

    @param  ImageHandle
    @param  SystemTable

    @retval EFI_SUCCESS
**/
EFI_STATUS
EFIAPI
UsbRndisEntry (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable
)
{
    gUsbRndisDriverBinding.DriverBindingHandle = ImageHandle;
    gUsbRndisDriverBinding.ImageHandle         = ImageHandle;

    return gBS->InstallMultipleProtocolInterfaces (
                    &gUsbRndisDriverBinding.DriverBindingHandle,
                    &gEfiDriverBindingProtocolGuid,
                    &gUsbRndisDriverBinding,
                    &gEfiComponentName2ProtocolGuid,
                    &gUsbRndisComponentName2,
                    NULL );
}

