/** @file
  USB Lan Driver Binding

  Copyright (c) 2021, American Megatrends International LLC.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "UsbLanDriver.h"


PXE_SW_UNDI  *gPxe = NULL;
NIC_DEVICE   *gLanDeviceList[MAX_USBLAN_INTERFACE];

EFI_GUID gUsbEthProtocolGuid = USB_ETHERNET_PROTOCOL_GUID;

EFI_DRIVER_BINDING_PROTOCOL  gUsbLanDriverBinding = {
    UsbLanSupported,
    UsbLanDriverStart,
    UsbLanDriverStop,
    USB_LAN_DRIVER_VERSION,
    NULL,
    NULL
};

/**
    Create Mac Device Path

    @param  Dev       A pointer to the EFI_DEVICE_PATH_PROTOCOL instance.
    @param  BaseDev   A pointer to the EFI_DEVICE_PATH_PROTOCOL instance.
    @param  Nic       A pointer to the Ethernet NIC Network Adapter.

    @retval EFI_SUCCESS   Mac device path create successful.
**/
EFI_STATUS
CreateMacDevicePath (
    IN OUT  EFI_DEVICE_PATH_PROTOCOL **Dev,
    IN      EFI_DEVICE_PATH_PROTOCOL *BaseDev,
    IN      NIC_DATA                 *Nic
)
{
    MAC_ADDR_DEVICE_PATH      MacAddrNode;
    EFI_DEVICE_PATH_PROTOCOL  *EndNode;
    UINT8                     *DevicePath;
    UINT16                    TotalLength;
    UINT16                    BaseLength;

    ZeroMem (&MacAddrNode, sizeof(MAC_ADDR_DEVICE_PATH));
    CopyMem (&MacAddrNode.MacAddress, &Nic->MacAddr, sizeof(EFI_MAC_ADDRESS));

    MacAddrNode.Header.Type       = MESSAGING_DEVICE_PATH;
    MacAddrNode.Header.SubType    = MSG_MAC_ADDR_DP;
    MacAddrNode.Header.Length[0]  = (UINT8)sizeof(MacAddrNode);
    MacAddrNode.Header.Length[1]  = 0;

    EndNode = BaseDev;

    while (!IsDevicePathEnd (EndNode)) {
        EndNode = NextDevicePathNode (EndNode);
    }

    BaseLength  = (UINT16)((UINTN)(EndNode) - (UINTN)(BaseDev));
    TotalLength = (UINT16)(BaseLength + sizeof (MacAddrNode) + sizeof (EFI_DEVICE_PATH_PROTOCOL));

    DevicePath = AllocatePool (TotalLength);
    if (DevicePath == NULL) {
        return EFI_OUT_OF_RESOURCES;
    }

    *Dev = (EFI_DEVICE_PATH_PROTOCOL*)DevicePath;
    CopyMem (DevicePath, (CHAR8*)BaseDev, BaseLength);
    DevicePath += BaseLength;
    CopyMem (DevicePath, (CHAR8*)&MacAddrNode, sizeof(MacAddrNode));
    DevicePath += sizeof(MacAddrNode);
    CopyMem (DevicePath, (CHAR8*)EndNode, sizeof(EFI_DEVICE_PATH_PROTOCOL));

    return EFI_SUCCESS;
}

/**
    USB Lan Driver Binding Support.

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
UsbLanSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
)
{
    EFI_STATUS              Status;
    USB_ETHERNET_PROTOCOL   *UsbEth;

    Status = gBS->OpenProtocol (
                        ControllerHandle,
                        &gUsbEthProtocolGuid,
                        (VOID**)&UsbEth,
                        This->DriverBindingHandle,
                        ControllerHandle,
                        EFI_OPEN_PROTOCOL_BY_DRIVER );
    if (EFI_ERROR (Status)) {
        return Status;
    }

    gBS->CloseProtocol (
                ControllerHandle,
                &gUsbEthProtocolGuid,
                This->DriverBindingHandle,
                ControllerHandle );
    return Status;
}

/**
    USB Lan Driver Binding Start.

    @param  This                 Protocol instance pointer.
    @param  ControllerHandle     Handle of device to bind driver to.
    @param  RemainingDevicePath  Optional parameter use to pick a specific child
                                 device to start.

    @retval EFI_SUCCESS          This driver is added to ControllerHandle
    @retval EFI_DEVICE_ERROR     This driver could not be started due to a device error
    @retval other                This driver does not support this device
**/
EFI_STATUS
EFIAPI
UsbLanDriverStart (
    IN EFI_DRIVER_BINDING_PROTOCOL    *This,
    IN EFI_HANDLE                     ControllerHandle,
    IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
)
{
    EFI_STATUS                  Status;
    EFI_DEVICE_PATH_PROTOCOL    *UsbEthPath;
    USB_ETHERNET_PROTOCOL       *UsbEth;
    EFI_MAC_ADDRESS             MacAddress;
    UINTN                       BulkDataSize;
    NIC_DEVICE                  *NicDevice;
    UINT8                       *TmpPxePointer = NULL;

    Status = gBS->OpenProtocol (
                        ControllerHandle,
                        &gUsbEthProtocolGuid,
                        (VOID **)&UsbEth,
                        This->DriverBindingHandle,
                        ControllerHandle,
                        EFI_OPEN_PROTOCOL_BY_DRIVER );
    if (EFI_ERROR (Status)) {
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
                    &gUsbEthProtocolGuid,
                    This->DriverBindingHandle,
                    ControllerHandle );
        return Status;
    }

    ZeroMem (&MacAddress, sizeof(EFI_MAC_ADDRESS));

    Status = UsbEth->UsbEthMacAddress (
                                UsbEth,
                                &MacAddress );
    if (!EFI_ERROR (Status)) {
        Status = UsbEth->UsbEthMaxBulkSize (
                                    UsbEth,
                                    &BulkDataSize );
    }
    if (EFI_ERROR (Status)) {
        gBS->CloseProtocol (
                    ControllerHandle,
                    &gEfiDevicePathProtocolGuid,
                    This->DriverBindingHandle,
                    ControllerHandle );
        gBS->CloseProtocol (
                    ControllerHandle,
                    &gUsbEthProtocolGuid,
                    This->DriverBindingHandle,
                    ControllerHandle );
        return Status;
    }

    NicDevice = AllocateZeroPool (sizeof(NIC_DEVICE));
    if (NicDevice == NULL) {
        gBS->CloseProtocol (
                    ControllerHandle,
                    &gEfiDevicePathProtocolGuid,
                    This->DriverBindingHandle,
                    ControllerHandle );
        gBS->CloseProtocol (
                    ControllerHandle,
                    &gUsbEthProtocolGuid,
                    This->DriverBindingHandle,
                    ControllerHandle );
        return EFI_UNSUPPORTED;
    }

    // for alignment adjustment
    if (gPxe == NULL) {
        TmpPxePointer = AllocateZeroPool (sizeof(PXE_SW_UNDI) + 16);
        if (TmpPxePointer == NULL) {
            FreePool (NicDevice);
            gBS->CloseProtocol (
                    ControllerHandle,
                    &gEfiDevicePathProtocolGuid,
                    This->DriverBindingHandle,
                    ControllerHandle );
            gBS->CloseProtocol (
                    ControllerHandle,
                    &gUsbEthProtocolGuid,
                    This->DriverBindingHandle,
                    ControllerHandle );
            return EFI_UNSUPPORTED;
        }

        // check for paragraph alignment here
        if(((UINTN)TmpPxePointer & 0x0F) != 0) {
            gPxe = (PXE_SW_UNDI*)(TmpPxePointer + 8);
        } else {
            gPxe = (PXE_SW_UNDI*)TmpPxePointer;
        }
        PxeStructInit (gPxe);
    }

    NicDevice->NiiProtocol.Id    = (UINT64)(UINTN)(gPxe);
    NicDevice->NiiProtocol.IfNum = gPxe->IFcnt | gPxe->IFcntExt << 8;

    UpdateNicNum (&NicDevice->NicInfo, gPxe);

    NicDevice->NicInfo.Signature       = NIC_DATA_SIGNATURE;

    NicDevice->NicInfo.UsbEth          = UsbEth;
    NicDevice->NicInfo.MaxSegmentSize  = (UINT16)BulkDataSize;
    CopyMem (
        &NicDevice->NicInfo.MacAddr,
        &MacAddress,
        sizeof(MacAddress));

    NicDevice->NicInfo.TxBufferCount = 0;

    gLanDeviceList[NicDevice->NiiProtocol.IfNum] = NicDevice;

    Status = CreateMacDevicePath (
                        &NicDevice->DevPath,
                        UsbEthPath,
                        &NicDevice->NicInfo );
    if (EFI_ERROR (Status)) {
        UpdateNicNum (NULL, gPxe);
        if (TmpPxePointer != NULL) {
            FreePool (TmpPxePointer);
        }
    }

    NicDevice->Signature                  = UNDI_DEV_SIGNATURE;
    NicDevice->NiiProtocol.Revision       = EFI_NETWORK_INTERFACE_IDENTIFIER_PROTOCOL_REVISION;
    NicDevice->NiiProtocol.Type           = EfiNetworkInterfaceUndi;
    NicDevice->NiiProtocol.MajorVer       = PXE_ROMID_MAJORVER;
    NicDevice->NiiProtocol.MinorVer       = PXE_ROMID_MINORVER;
    NicDevice->NiiProtocol.ImageSize      = 0;
    NicDevice->NiiProtocol.ImageAddr      = 0;
    NicDevice->NiiProtocol.Ipv6Supported  = TRUE;

    NicDevice->NiiProtocol.StringId[0]    = 'U';
    NicDevice->NiiProtocol.StringId[1]    = 'N';
    NicDevice->NiiProtocol.StringId[2]    = 'D';
    NicDevice->NiiProtocol.StringId[3]    = 'I';
    NicDevice->DeviceHandle               = NULL;

    //Initial cable connect value to 0(disconnect)
    NicDevice->NicInfo.Request.Value = 0;

    Status = UsbEth->UsbEthInterrupt (
                                UsbEth,
                                TRUE,
                                USB_LAN_POLLING_INTERVAL,
                                &NicDevice->NicInfo.Request );
    ASSERT_EFI_ERROR(Status);

    Status = gBS->InstallMultipleProtocolInterfaces (
                                    &NicDevice->DeviceHandle,
                                    &gEfiNetworkInterfaceIdentifierProtocolGuid_31,
                                    &NicDevice->NiiProtocol,
                                    &gEfiDevicePathProtocolGuid,
                                    NicDevice->DevPath,
                                    NULL );
    if (!EFI_ERROR (Status)) {
        Status = gBS->OpenProtocol(ControllerHandle,
                            &gUsbEthProtocolGuid,
                            (VOID **)&UsbEth,
                            This->DriverBindingHandle,
                            NicDevice->DeviceHandle,
                            EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER );
    } else {
        gLanDeviceList[NicDevice->NiiProtocol.IfNum] = NULL;
        gBS->CloseProtocol (
                    ControllerHandle,
                    &gEfiDevicePathProtocolGuid,
                    This->DriverBindingHandle,
                    ControllerHandle );
        gBS->CloseProtocol (
                    ControllerHandle,
                    &gUsbEthProtocolGuid,
                    This->DriverBindingHandle,
                    ControllerHandle );
        FreePool (NicDevice->DevPath);
        FreePool (NicDevice);
    }

    return Status;
}

/**
    USB Lan Driver Binding Stop.

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
UsbLanDriverStop (
    IN  EFI_DRIVER_BINDING_PROTOCOL    *This,
    IN  EFI_HANDLE                     Controller,
    IN  UINTN                          NumberOfChildren,
    IN  EFI_HANDLE                     *ChildHandleBuffer
)
{
    EFI_STATUS                                Status;
    BOOLEAN                                   AllChildrenStopped;
    UINTN                                     Index;
    USB_ETHERNET_PROTOCOL                     *UsbEth;
    NIC_DEVICE                                *NicDevice;
    EFI_NETWORK_INTERFACE_IDENTIFIER_PROTOCOL *NiiProtocol;

    if (NumberOfChildren == 0) {
        Status = gBS->OpenProtocol (
                            Controller,
                            &gEfiNetworkInterfaceIdentifierProtocolGuid_31,
                            (VOID **)&NiiProtocol,
                            This->DriverBindingHandle,
                            Controller,
                            EFI_OPEN_PROTOCOL_GET_PROTOCOL );

        if (EFI_ERROR(Status)) {
            gBS->CloseProtocol (
                            Controller,
                            &gEfiDevicePathProtocolGuid,
                            This->DriverBindingHandle,
                            Controller );
            gBS->CloseProtocol (
                            Controller,
                            &gUsbEthProtocolGuid,
                            This->DriverBindingHandle,
                            Controller );
            return EFI_SUCCESS;
        }

        NicDevice = UNDI_DEV_FROM_THIS (NiiProtocol);
        Status = gBS->UninstallMultipleProtocolInterfaces (
                                                  Controller,
                                                  &gEfiNetworkInterfaceIdentifierProtocolGuid_31,
                                                  &NicDevice->NiiProtocol,
                                                  &gEfiDevicePathProtocolGuid,
                                                  NicDevice->DevPath,
                                                  NULL );
        if (EFI_ERROR (Status)) {
            return Status;
        }

        FreePool (NicDevice->DevPath);
        FreePool (NicDevice);

        gBS->CloseProtocol (
                        Controller,
                        &gEfiDevicePathProtocolGuid,
                        This->DriverBindingHandle,
                        Controller );
        gBS->CloseProtocol (
                        Controller,
                        &gUsbEthProtocolGuid,
                        This->DriverBindingHandle,
                        Controller );
        return EFI_SUCCESS;
    }

    AllChildrenStopped = TRUE;

    for (Index = 0; Index < NumberOfChildren; Index++) {
        Status = gBS->OpenProtocol (
                            ChildHandleBuffer[Index],
                            &gEfiNetworkInterfaceIdentifierProtocolGuid_31,
                            (VOID **)&NiiProtocol,
                            This->DriverBindingHandle,
                            Controller,
                            EFI_OPEN_PROTOCOL_GET_PROTOCOL );
        if (EFI_ERROR(Status)) {
            AllChildrenStopped = FALSE;
            continue;
        }

        NicDevice = UNDI_DEV_FROM_THIS (NiiProtocol);

        gBS->CloseProtocol (
                        Controller,
                        &gUsbEthProtocolGuid,
                        This->DriverBindingHandle,
                        ChildHandleBuffer[Index] );

        Status = gBS->UninstallMultipleProtocolInterfaces (
                                        ChildHandleBuffer[Index],
                                        &gEfiNetworkInterfaceIdentifierProtocolGuid_31,
                                        &NicDevice->NiiProtocol,
                                        &gEfiDevicePathProtocolGuid,
                                        NicDevice->DevPath,
                                        NULL );
        if (EFI_ERROR(Status)) {
            Status = gBS->OpenProtocol (
                                Controller,
                                &gUsbEthProtocolGuid,
                                (VOID**)&UsbEth,
                                This->DriverBindingHandle,
                                ChildHandleBuffer[Index],
                                EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER );
        } else {
            FreePool (NicDevice->DevPath);
            FreePool (NicDevice);
        }
    }

    if (!AllChildrenStopped) {
        return EFI_DEVICE_ERROR;
    }

    return Status;
}

/**
    USB Lan Driver Entry.

    @param  ImageHandle    The firmware allocated handle for the EFI image.
    @param  SystemTable    A pointer to the EFI System Table.

    @retval EFI_SUCCESS       This driver install success
    @retval other             This driver install fail
**/
EFI_STATUS
EFIAPI
UsbLanInit (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable
)
{
    EFI_STATUS    Status;

    gUsbLanDriverBinding.DriverBindingHandle = ImageHandle;
    gUsbLanDriverBinding.ImageHandle         = ImageHandle;

    Status = gBS->InstallMultipleProtocolInterfaces (
                                &gUsbLanDriverBinding.DriverBindingHandle,
                                &gEfiDriverBindingProtocolGuid,
                                &gUsbLanDriverBinding,
                                &gEfiComponentName2ProtocolGuid,
                                &gUsbLanComponentName2,
                                NULL );
    return Status;
}
