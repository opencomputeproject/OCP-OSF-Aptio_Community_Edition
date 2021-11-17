/** @file
  USB Ethernet descriptor and specific requests implement.

  Copyright (c) 2021, American Megatrends International LLC.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <UsbRndisDriver.h>

/**
    Load All of device descriptor.

    @param  UsbIo                 A pointer to the EFI_USB_IO_PROTOCOL instance.
    @param  ConfigDesc            A pointer to the configuration descriptor.

    @retval EFI_SUCCESS           The request executed successfully.
    @retval EFI_OUT_OF_RESOURCES  The request could not be completed because the
                                  buffer specified by DescriptorLength and Descriptor
                                  is not large enough to hold the result of the request.
    @retval EFI_TIMEOUT           A timeout occurred executing the request.
    @retval EFI_DEVICE_ERROR      The request failed due to a device error. The transfer
                                  status is returned in Status.
**/
EFI_STATUS
LoadAllDescriptor (
    IN EFI_USB_IO_PROTOCOL             *UsbIo,
    IN OUT EFI_USB_CONFIG_DESCRIPTOR   **ConfigDesc
)
{
    EFI_STATUS                  Status;
    UINT32                      TransStatus;
    EFI_USB_CONFIG_DESCRIPTOR   Tmp;

    Status = UsbIo->UsbGetConfigDescriptor (UsbIo, &Tmp);
    if (EFI_ERROR(Status)) {
        return Status;
    }

    *ConfigDesc = AllocatePool (Tmp.TotalLength);
    if (*ConfigDesc == NULL) {
        return EFI_OUT_OF_RESOURCES;
    }

    Status = UsbGetDescriptor (
                    UsbIo,
                    (USB_DESC_TYPE_CONFIG << 8) | (Tmp.ConfigurationValue - 1),  // zero based
                    0,
                    Tmp.TotalLength,
                    *ConfigDesc,
                    &TransStatus );
    return Status;
}

/**
    Returns pointer to the next descriptor for the pack of USB descriptors
    located in continues memory segment

    @param  Desc   A pointer to the CONFIG_DESCRIPTOR instance.
    @param  Offset A pointer to the sum of descriptor length.

    @retval TRUE   The request executed successfully.
    @retval FALSE  No next descriptor.
**/
BOOLEAN
NextDescriptor (
    IN EFI_USB_CONFIG_DESCRIPTOR *Desc,
    IN OUT UINTN                 *Offset
)
{
    EFI_USB_CONFIG_DESCRIPTOR *TempDesc;

    if ((Desc == NULL) || (Offset == NULL) || (*Offset >= Desc->TotalLength)) {
        return FALSE;
    }

    TempDesc = (EFI_USB_CONFIG_DESCRIPTOR*)((UINT8*)Desc + *Offset);
    if (TempDesc->Length == 0) {
        return FALSE;
    }

    *Offset += TempDesc->Length;
    if (*Offset >= Desc->TotalLength) {
        return FALSE;
    }

    return TRUE;
}

/**
    Read Function descriptor

    @param  Config             A pointer to all of configuration.
    @param  FunDescriptorType  USB CDC class descriptor SubType.
    @param  DataBuffer         A pointer to the Data of corresponding to device capability.

    @retval EFI_SUCCESS        The device capability descriptor was retrieved
                                successfully.
    @retval EFI_UNSUPPORTED    No supported.
    @retval EFI_NOT_FOUND      The device capability descriptor was not found.
**/

EFI_STATUS
GetFunctionalDescriptor (
    IN  EFI_USB_CONFIG_DESCRIPTOR   *Config,
    IN  UINT8                       FunDescriptorType,
    OUT VOID                        *DataBuffer
)
{
    EFI_STATUS                      Status = EFI_NOT_FOUND;
    UINTN                           Offset;
    EFI_USB_INTERFACE_DESCRIPTOR    *Interface;

    if (Config == NULL) {
        return EFI_INVALID_PARAMETER;
    }

    for (Offset = 0; NextDescriptor(Config, &Offset); ) {

        Interface = (EFI_USB_INTERFACE_DESCRIPTOR*)((UINT8*)Config + Offset);
        if (Interface->DescriptorType != CS_INTERFACE) {
            continue;
        }

        if (((USB_HEADER_FUN_DESCRIPTOR*)Interface)->DescriptorSubtype != FunDescriptorType) {
            continue;
        }

        switch (FunDescriptorType) {
            case HEADER_FUN_DESCRIPTOR:
                CopyMem (
                    DataBuffer,
                    (USB_HEADER_FUN_DESCRIPTOR*)Interface,
                    sizeof(USB_HEADER_FUN_DESCRIPTOR));
                return EFI_SUCCESS;

            case UNION_FUN_DESCRIPTOR:
                CopyMem (
                    DataBuffer,
                    (USB_UNION_FUN_DESCRIPTOR*)Interface,
                    ((USB_UNION_FUN_DESCRIPTOR*)Interface)->FunctionLength);
                return EFI_SUCCESS;

            case ETHERNET_FUN_DESCRIPTOR:
                CopyMem (
                    DataBuffer,
                    (USB_ETHERNET_FUN_DESCRIPTOR*)Interface,
                    sizeof(USB_ETHERNET_FUN_DESCRIPTOR));
                return EFI_SUCCESS;

            default:
                Status = EFI_UNSUPPORTED;
                break;
        }
    }

    return Status;
}

/**
    Get USB Ethernet IO endpoint and USB CDC data IO endpoint.

    @param  UsbIo           A pointer to the EFI_USB_IO_PROTOCOL instance.
    @param  UsbRndisDevice  A pointer to the USB_RNDIS_DEVICE.

    @retval None
**/
VOID
GetEndpoint (
    IN  EFI_USB_IO_PROTOCOL *UsbIo,
    OUT USB_RNDIS_DEVICE    *UsbRndisDevice
)
{
    EFI_STATUS                    Status;
    UINT8                         Index;
    UINT32                        Result;
    EFI_USB_INTERFACE_DESCRIPTOR  Interface;
    EFI_USB_ENDPOINT_DESCRIPTOR   Endpoint;

    Status = UsbIo->UsbGetInterfaceDescriptor (
                                            UsbIo,
                                            &Interface);
    if (EFI_ERROR (Status)) {
        return;
    }

    if (Interface.NumEndpoints == 0 ) {

        Status = UsbSetInterface (UsbIo, 1, 0, &Result);
        if (EFI_ERROR (Status)) {
            return;
        }

        Status = UsbIo->UsbGetInterfaceDescriptor (
                                                UsbIo,
                                                &Interface );
        if (EFI_ERROR (Status)) {
            return;
        }
    }

    for (Index = 0; Index < Interface.NumEndpoints; Index++) {

        Status = UsbIo->UsbGetEndpointDescriptor (
                                            UsbIo,
                                            Index,
                                            &Endpoint );
        if (EFI_ERROR (Status)) {
            continue;
        }

        switch ((Endpoint.Attributes & (BIT0 | BIT1))) {
            case USB_ENDPOINT_BULK:
                if (Endpoint.EndpointAddress & BIT7) {
                    UsbRndisDevice->BulkInEndpoint = Endpoint.EndpointAddress;
                } else {
                    UsbRndisDevice->BulkOutEndpoint = Endpoint.EndpointAddress;
                }
                break;

            case USB_ENDPOINT_INTERRUPT:
                UsbRndisDevice->InterrupEndpoint = Endpoint.EndpointAddress;
                break;
        }
    }
}

/**
    Async USB transfer callback routine.

    @param  Data                  Data received or sent via the USB Asynchronous Transfer, if the
                                transfer completed successfully.
    @param  DataLength            The length of Data received or sent via the Asynchronous
                                Transfer, if transfer successfully completes.
    @param  Context               Data passed from UsbAsyncInterruptTransfer() request.
    @param  Status                Indicates the result of the asynchronous transfer.

    @retval EFI_SUCCESS           The asynchronous USB transfer request has been successfully executed.
    @retval EFI_DEVICE_ERROR      The asynchronous USB transfer request failed.
**/
EFI_STATUS
EFIAPI
InterruptCallback (
    IN  VOID        *Data,
    IN  UINTN       DataLength,
    IN  VOID        *Context,
    IN  UINT32      Status
)
{
    if ((Data == NULL) || (Context == NULL)) {
        return EFI_INVALID_PARAMETER;
    }

    if (((EFI_USB_DEVICE_REQUEST*)Data)->Request == 0) {
        CopyMem (
            Context,
            Data,
            sizeof(EFI_USB_DEVICE_REQUEST));
    }

    return EFI_SUCCESS;
}

/**
    This function is used to manage a USB device with an interrupt transfer pipe.

    @param  This                  A pointer to the USB_ETHERNET_PROTOCOL instance.
    @param  IsNewTransfer         If TRUE, a new transfer will be submitted to USB controller. If
                                FALSE, the interrupt transfer is deleted from the device's interrupt
                                transfer queue.
    @param  PollingInterval       Indicates the periodic rate, in milliseconds, that the transfer is to be
                                executed.This parameter is required when IsNewTransfer is TRUE. The
                                value must be between 1 to 255, otherwise EFI_INVALID_PARAMETER is returned.
                                The units are in milliseconds.
    @param  DataLength            A pointer to the DataLength
    @param  Context               Data passed to the InterruptCallback function. This is an optional
                                parameter and may be NULL.

    @retval EFI_SUCCESS           The asynchronous USB transfer request transfer has been successfully executed.
    @retval EFI_DEVICE_ERROR      The asynchronous USB transfer request failed.
**/
EFI_STATUS
EFIAPI
UsbRndisInterrupt (
    IN USB_ETHERNET_PROTOCOL        *This,
    IN BOOLEAN                      IsNewTransfer,
    IN UINTN                        PollingInterval,
    IN EFI_USB_DEVICE_REQUEST       *Request
)
{
    EFI_STATUS                      Status;
    USB_RNDIS_DEVICE                *UsbRndisDevice;
    UINTN                           DataLength = 0;

    if (This == NULL) {
        return EFI_INVALID_PARAMETER;
    }

    UsbRndisDevice = USB_RNDIS_DEVICE_FROM_THIS(This);

    if (IsNewTransfer) {

        DataLength = sizeof(EFI_USB_DEVICE_REQUEST) + sizeof(USB_CONNECT_SPEED_CHANGE);

        Status = UsbRndisDevice->UsbIo->UsbAsyncInterruptTransfer (
                                    UsbRndisDevice->UsbIo,
                                    UsbRndisDevice->InterrupEndpoint,
                                    IsNewTransfer,
                                    PollingInterval,
                                    DataLength,
                                    InterruptCallback,
                                    Request );
        if (Status == EFI_INVALID_PARAMETER){
            // Because of Stacked AsyncInterrupt request are not supported
            Status = UsbRndisDevice->UsbIo->UsbAsyncInterruptTransfer (
                                    UsbRndisDevice->UsbIo,
                                    UsbRndisDevice->InterrupEndpoint,
                                    0,
                                    0,
                                    0,
                                    NULL,
                                    NULL );
        }
    } else {
        Status = UsbRndisDevice->UsbIo->UsbAsyncInterruptTransfer (
                            UsbRndisDevice->UsbIo,
                            UsbRndisDevice->InterrupEndpoint,
                            IsNewTransfer,
                            0,
                            0,
                            NULL,
                            NULL );
    }

    return Status;
}

/**
    Retrieves the USB Ethernet Mac Address.

    @param  This          A pointer to the USB_ETHERNET_PROTOCOL instance.
    @param  MacAddress    A pointer to the caller allocated USB Ethernet Mac Address.

    @retval EFI_SUCCESS           The USB Header Functional descriptor was retrieved successfully.
    @retval EFI_INVALID_PARAMETER UsbHeaderFunDescriptor is NULL.
    @retval EFI_NOT_FOUND         The USB Header Functional descriptor was not found.
**/
EFI_STATUS
EFIAPI
GetUsbEthMacAddress (
    IN  USB_ETHERNET_PROTOCOL   *This,
    OUT EFI_MAC_ADDRESS         *MacAddress
)
{
    EFI_STATUS                      Status;
    USB_RNDIS_DEVICE                *UsbRndisDevice;
    USB_ETHERNET_FUN_DESCRIPTOR     UsbEthDescriptor;
    CHAR16                          *Data;
    CHAR16                          Tmp;
    UINT8                           Index;
    UINT8                           High;
    UINT8                           Low;

    if ((This == NULL) || (MacAddress == NULL)) {
        return EFI_INVALID_PARAMETER;
    }

    UsbRndisDevice = USB_RNDIS_DEVICE_FROM_THIS(This);

    Status = This->UsbEthFuncDescriptor (
                                    This,
                                    &UsbEthDescriptor );
    if (EFI_ERROR(Status)) {
        return Status;
    }

    Status = UsbRndisDevice->UsbIo->UsbGetStringDescriptor (
                            UsbRndisDevice->UsbIo,
                            0x409, // English-US Language ID
                            UsbEthDescriptor.MacAddress,
                            &Data );
    if (EFI_ERROR(Status)) {
        return Status;
    }

    for (Index = 0; Index < PXE_HWADDR_LEN_ETHER; Index++) {
        Tmp  = *Data++;
        High = (UINT8)StrHexToUintn (&Tmp);
        Tmp  = *Data++;
        Low  = (UINT8)StrHexToUintn (&Tmp);
        MacAddress->Addr[Index] = (High << 4) | Low;
    }

    return Status;
}

/**
    Retrieves the USB Ethernet Bulk transfer data size.

    @param  This          A pointer to the USB_ETHERNET_PROTOCOL instance.
    @param  BulkSize      A pointer to the Bulk transfer data size.

    @retval EFI_SUCCESS           The USB Header Functional descriptor was retrieved successfully.
    @retval EFI_INVALID_PARAMETER UsbHeaderFunDescriptor is NULL.
    @retval EFI_NOT_FOUND         The USB Header Functional descriptor was not found.
**/
EFI_STATUS
EFIAPI
UsbEthBulkSize (
    IN  USB_ETHERNET_PROTOCOL   *This,
    OUT UINTN                   *BulkSize
)
{
    EFI_STATUS                      Status;
    USB_ETHERNET_FUN_DESCRIPTOR     UsbEthFunDescriptor;

    if ((This == NULL) || (BulkSize == NULL)) {
        return EFI_INVALID_PARAMETER;
    }

    Status = This->UsbEthFuncDescriptor (
                                    This,
                                    &UsbEthFunDescriptor );
    if (EFI_ERROR(Status)) {
        return Status;
    }

    *BulkSize = (UINTN)UsbEthFunDescriptor.MaxSegmentSize;
    return  Status;
}

/**
    Retrieves the USB Header functional Descriptor.

    @param  This                   A pointer to the USB_ETHERNET_PROTOCOL instance.
    @param  UsbHeaderFunDescriptor A pointer to the caller allocated USB Header Functional Descriptor.

    @retval EFI_SUCCESS           The USB Header Functional descriptor was retrieved successfully.
    @retval EFI_INVALID_PARAMETER UsbHeaderFunDescriptor is NULL.
    @retval EFI_NOT_FOUND         The USB Header Functional descriptor was not found.
**/
EFI_STATUS
EFIAPI
GetUsbHeaderFunDescriptor (
    IN  USB_ETHERNET_PROTOCOL       *This,
    OUT USB_HEADER_FUN_DESCRIPTOR   *UsbHeaderFunDescriptor
)
{
    EFI_STATUS                      Status;
    USB_RNDIS_DEVICE                *UsbRndisDevice;

    if ((This == NULL) || (UsbHeaderFunDescriptor == NULL)) {
        return EFI_INVALID_PARAMETER;
    }

    UsbRndisDevice = USB_RNDIS_DEVICE_FROM_THIS (This);

    Status = GetFunctionalDescriptor (
                    UsbRndisDevice->Config,
                    HEADER_FUN_DESCRIPTOR,
                    UsbHeaderFunDescriptor );
    return Status;
}

/**
    Retrieves the USB Union functional Descriptor.

    @param  This                   A pointer to the USB_ETHERNET_PROTOCOL instance.
    @param  UsbUnionFunDescriptor  A pointer to the caller allocated USB Union Functional Descriptor.

    @retval EFI_SUCCESS            The USB Union Functional descriptor was retrieved successfully.
    @retval EFI_INVALID_PARAMETER  UsbUnionFunDescriptor is NULL.
    @retval EFI_NOT_FOUND          The USB Union Functional descriptor was not found.
**/
EFI_STATUS
EFIAPI
GetUsbUnionFunDescriptor (
    IN  USB_ETHERNET_PROTOCOL       *This,
    OUT USB_UNION_FUN_DESCRIPTOR    *UsbUnionFunDescriptor
)
{
    EFI_STATUS           Status;
    USB_RNDIS_DEVICE     *UsbRndisDevice;

    if ((This == NULL) || (UsbUnionFunDescriptor == NULL)) {
        return EFI_INVALID_PARAMETER;
    }

    UsbRndisDevice = USB_RNDIS_DEVICE_FROM_THIS(This);

    Status = GetFunctionalDescriptor (
                        UsbRndisDevice->Config,
                        UNION_FUN_DESCRIPTOR,
                        UsbUnionFunDescriptor );
    return Status;
}

/**
    Retrieves the USB Ethernet functional Descriptor.

    @param  This                   A pointer to the USB_ETHERNET_PROTOCOL instance.
    @param  UsbEthFunDescriptor    A pointer to the caller allocated USB Ethernet Functional Descriptor.

    @retval EFI_SUCCESS            The USB Ethernet Functional descriptor was retrieved successfully.
    @retval EFI_INVALID_PARAMETER  UsbEthFunDescriptor is NULL.
    @retval EFI_NOT_FOUND          The USB Ethernet Functional descriptor was not found.
**/
EFI_STATUS
EFIAPI
GetUsbRndisFunDescriptor (
    IN  USB_ETHERNET_PROTOCOL       *This,
    OUT USB_ETHERNET_FUN_DESCRIPTOR *UsbEthFunDescriptor
)
{
    EFI_STATUS                      Status;
    USB_RNDIS_DEVICE                *UsbRndisDevice;

    if ((This == NULL) || (UsbEthFunDescriptor == NULL)) {
        return EFI_INVALID_PARAMETER;
    }

    UsbRndisDevice = USB_RNDIS_DEVICE_FROM_THIS (This);

    Status = GetFunctionalDescriptor (
                        UsbRndisDevice->Config,
                        ETHERNET_FUN_DESCRIPTOR,
                        UsbEthFunDescriptor );
    return Status;
}

/**
    This request sets the Ethernet device multicast filters as specified in the
    sequential list of 48 bit Ethernet multicast addresses.

    @param  This                   A pointer to the USB_ETHERNET_PROTOCOL instance.
    @param  Value                  Number of filters.
    @param  McastAddr              A pointer to the value of the multicast addresses.

    @retval EFI_SUCCESS            The request executed successfully.
    @retval EFI_TIMEOUT            A timeout occurred executing the request.
    @retval EFI_DEVICE_ERROR       The request failed due to a device error.
    @retval EFI_INVALID_PARAMETER  One of the parameters has an invalid value.
    @retval EFI_UNSUPPORTED        Not supported.
**/

EFI_STATUS
EFIAPI
SetUsbRndisMcastFilter (
    IN USB_ETHERNET_PROTOCOL        *This,
    IN UINT16                       Value,
    IN VOID                         *McastAddr
)
{
    EFI_STATUS                      Status;
    EFI_USB_DEVICE_REQUEST          Request;
    UINT32                          TransStatus;
    USB_ETHERNET_FUN_DESCRIPTOR     UsbEthFunDescriptor;
    USB_RNDIS_DEVICE                *UsbRndisDevice;

    if (This == NULL) {
        return EFI_INVALID_PARAMETER;
    }

    UsbRndisDevice = USB_RNDIS_DEVICE_FROM_THIS (This);

    Status = This->UsbEthFuncDescriptor (
                                    This,
                                    &UsbEthFunDescriptor );
    if (EFI_ERROR(Status)) {
        return Status;
    }

    if ((UsbEthFunDescriptor.NumberMcFilters << 1) == 0) {
        return EFI_UNSUPPORTED;
    }

    Request.RequestType = USB_ETHRTNET_SET_REQ_TYPE;
    Request.Request     = SET_ETH_MULTICAST_FILTERS_REQ;
    Request.Value       = Value;
    Request.Index       = UsbRndisDevice->NumOfInterface;
    Request.Length      = Value * 6;

    return UsbRndisDevice->UsbIo->UsbControlTransfer (
                    UsbRndisDevice->UsbIo,
                    &Request,
                    EfiUsbDataOut,
                    FixedPcdGet32 (PcdUsbEthernetTransferTimeOut),
                    McastAddr,
                    Request.Length,
                    &TransStatus );
}

/**
    This request sets up the specified Ethernet power management pattern filter as
    described in the data structure.

    @param  This                   A pointer to the USB_ETHERNET_PROTOCOL instance.
    @param  Value                  Number of filters.
    @param  Length                 Size of structure.
    @param  PatternFilter          A pointer to the power management pattern filter structure.

    @retval EFI_SUCCESS            The request executed successfully.
    @retval EFI_TIMEOUT            A timeout occurred executing the request.
    @retval EFI_DEVICE_ERROR       The request failed due to a device error.
    @retval EFI_INVALID_PARAMETER  One of the parameters has an invalid value.
    @retval EFI_UNSUPPORTED        Not supported.
**/
EFI_STATUS
EFIAPI
SetUsbRndisPowerFilter (
    IN USB_ETHERNET_PROTOCOL        *This,
    IN UINT16                       Value,
    IN UINT16                       Length,
    IN VOID                         *PatternFilter
)
{
    EFI_USB_DEVICE_REQUEST          Request;
    UINT32                          TransStatus;
    USB_RNDIS_DEVICE                *UsbRndisDevice;

    if (This == NULL) {
        return EFI_INVALID_PARAMETER;
    }

    UsbRndisDevice = USB_RNDIS_DEVICE_FROM_THIS (This);

    Request.RequestType = USB_ETHRTNET_SET_REQ_TYPE;
    Request.Request     = SET_ETH_POWER_MANAGEMENT_PATTERN_FILTER_REQ;
    Request.Value       = Value;
    Request.Index       = UsbRndisDevice->NumOfInterface;
    Request.Length      = Length;

    return UsbRndisDevice->UsbIo->UsbControlTransfer (
                            UsbRndisDevice->UsbIo,
                            &Request,
                            EfiUsbDataOut,
                            FixedPcdGet32 (PcdUsbEthernetTransferTimeOut),
                            PatternFilter,
                            Length,
                            &TransStatus );
}

/**
    This request retrieves the status of the specified Ethernet power management
    pattern filter from the device.

    @param  This                   A pointer to the USB_ETHERNET_PROTOCOL instance.
    @param  Value                  Number of filters.
    @param  PatternFilter          A pointer to the pattern active boolean.

    @retval EFI_SUCCESS            The request executed successfully.
    @retval EFI_TIMEOUT            A timeout occurred executing the request.
    @retval EFI_DEVICE_ERROR       The request failed due to a device error.
    @retval EFI_INVALID_PARAMETER  One of the parameters has an invalid value.
    @retval EFI_UNSUPPORTED        Not supported.
**/
EFI_STATUS
EFIAPI
GetUsbRndisPowerFilter (
    IN USB_ETHERNET_PROTOCOL    *This,
    IN UINT16                   Value,
    IN BOOLEAN                  *PatternActive
)
{
    EFI_USB_DEVICE_REQUEST          Request;
    UINT32                          TransStatus;
    USB_RNDIS_DEVICE                *UsbRndisDevice;

    if (This == NULL) {
        return EFI_INVALID_PARAMETER;
    }

    UsbRndisDevice = USB_RNDIS_DEVICE_FROM_THIS(This);

    Request.RequestType = USB_ETHERNET_GET_REQ_TYPE;
    Request.Request     = GET_ETH_POWER_MANAGEMENT_PATTERN_FILTER_REQ;
    Request.Value       = Value;
    Request.Index       = UsbRndisDevice->NumOfInterface;
    Request.Length      = USB_ETH_POWER_FILTER_LENGTH;

    return UsbRndisDevice->UsbIo->UsbControlTransfer (
                            UsbRndisDevice->UsbIo,
                            &Request,
                            EfiUsbDataIn,
                            FixedPcdGet32 (PcdUsbEthernetTransferTimeOut),
                            PatternActive,
                            USB_ETH_POWER_FILTER_LENGTH,
                            &TransStatus );
}

/**
    Updates Filter settings on the device

    @param  Cdb
    @param  Nic

    @retval EFI_STATUS
**/
EFI_STATUS
EFIAPI
RndisUndiReceiveFilter (
    IN  PXE_CDB     *Cdb,
    IN  NIC_DATA    *Nic
)
{
    UINT8                        *McastList;
    UINT8                        Count = 0;
    UINT8                        Index1;
    UINT8                        Index2;
    UINT64                       CpbAddr;
    UINT32                       CpbSize;
    UINT16                       SetFilter;
    PXE_CPB_RECEIVE_FILTERS      *Cpb;
    USB_ETHERNET_FUN_DESCRIPTOR  UsbEthFunDescriptor;

    if ((Cdb == NULL) || (Nic == NULL)) {
        return EFI_INVALID_PARAMETER;
    }

    CpbAddr   = Cdb->CPBaddr;
    CpbSize   = Cdb->CPBsize;
    SetFilter = (UINT16)(Cdb->OpFlags & 0x1F);
    Cpb       = (PXE_CPB_RECEIVE_FILTERS*)(UINTN)CpbAddr;

    Nic->RxFilter = (UINT8)SetFilter;

    if ((SetFilter & PXE_OPFLAGS_RECEIVE_FILTER_FILTERED_MULTICAST) != 0 || (Cpb != NULL)) {
        if (Cpb != NULL) {
            Nic->McastCount = (UINT8)(CpbSize / PXE_MAC_LENGTH);
            CopyMem (
                &Nic->McastList,
                Cpb,
                Nic->McastCount );
        }

        if (Nic->CanReceive) {
            Nic->CanReceive = FALSE;
        }

        Nic->UsbEth->UsbEthFuncDescriptor (
                                    Nic->UsbEth,
                                    &UsbEthFunDescriptor );

        if ((UsbEthFunDescriptor.NumberMcFilters << 1) == 0) {
            Nic->RxFilter |= PXE_OPFLAGS_RECEIVE_FILTER_ALL_MULTICAST;
            Nic->UsbEth->SetUsbEthPacketFilter (
                                        Nic->UsbEth,
                                        Nic->RxFilter);
        } else {
            McastList = AllocatePool (Nic->McastCount * 6);
            if (McastList == NULL) {
                return PXE_STATCODE_INVALID_PARAMETER;
            }

            for (Index1 = 0; Index1 < Nic->McastCount; Index1++) {
                for (Index2 = 0; Index2 < 6; Index2++) {
                    McastList[Count++] = Cpb->MCastList[Index1][Index2];
                }
            }

            Nic->RxFilter |= PXE_OPFLAGS_RECEIVE_FILTER_FILTERED_MULTICAST;

            Nic->UsbEth->SetUsbEthMcastFilter (
                                    Nic->UsbEth,
                                    Nic->McastCount,
                                    McastList );

            Nic->UsbEth->SetUsbEthPacketFilter (
                                    Nic->UsbEth,
                                    Nic->RxFilter);
            FreePool (McastList);
        }
    }

    return EFI_SUCCESS;
}

/**
    This request is used to configure device Ethernet packet filter settings.

    @param  This                  A pointer to the USB_ETHERNET_PROTOCOL instance.
    @param  Value                 Packet Filter Bitmap.

    @retval EFI_SUCCESS           The request executed successfully.
    @retval EFI_TIMEOUT           A timeout occurred executing the request.
    @retval EFI_DEVICE_ERROR      The request failed due to a device error.
    @retval EFI_INVALID_PARAMETER One of the parameters has an invalid value.
    @retval EFI_UNSUPPORTED       Not supported.
**/

EFI_STATUS
EFIAPI
SetUsbRndisPacketFilter (
    IN USB_ETHERNET_PROTOCOL    *This,
    IN UINT16                   Value
)
{
    return EFI_SUCCESS;
}

/**
    This request is used to retrieve a statistic based on the feature selector.

    @param  This                  A pointer to the USB_ETHERNET_PROTOCOL instance.
    @param  Value                 Packet Filter Bitmap.
    @param  Statistic             A pointer to the 32 bit unsigned integer.

    @retval EFI_SUCCESS           The request executed successfully.
    @retval EFI_TIMEOUT           A timeout occurred executing the request.
    @retval EFI_DEVICE_ERROR      The request failed due to a device error.
    @retval EFI_INVALID_PARAMETER One of the parameters has an invalid value.
    @retval EFI_UNSUPPORTED       Not supported.
**/
EFI_STATUS
EFIAPI
GetRndisStatistic (
    IN USB_ETHERNET_PROTOCOL    *This,
    IN UINT16                   Value,
    IN VOID                     *Statistic
)
{
    return EFI_SUCCESS;
}

/**
    This function is called when UndiStart is invoked

    @param  Cdb                   A pointer to the PXE_CDB.
    @param  Nic                   A pointer to the Nic.

    @retval EFI_STATUS
**/
EFI_STATUS
EFIAPI
RndisUndiStart (
    IN  PXE_CDB     *Cdb,
    IN  NIC_DATA    *Nic
)
{
    EFI_STATUS  Status;

    if ((Cdb == NULL) || (Nic == NULL)) {
        return EFI_INVALID_PARAMETER;
    }

    DEBUG((DEBUG_VERBOSE, "RndisUndiStart Nic %lx Cdb %lx Nic State %x\n", Nic, Cdb, Nic->State));

    // Issue Rndis Reset and bring the device to RNDIS_BUS_INITIALIZED state
    Status = RndisUndiReset (Cdb, Nic);
    if (EFI_ERROR (Status)) {
        RndisUndiReset (Cdb, Nic);
    }

    Status = RndisUndiInitialize (Cdb, Nic);
    if (EFI_ERROR (Status)) {
        RndisUndiInitialize (Cdb, Nic);
    }

    RndisUndiShutdown (Cdb, Nic);
	return EFI_SUCCESS;
}

/**
  This function is called when Undistop is invoked

  @param  Cdb         A pointer to the PXE_CDB.
  @param  Nic         A pointer to the Nic.

  @retval EFI_STATUS
**/
EFI_STATUS
EFIAPI
RndisUndiStop (
    IN  PXE_CDB     *Cdb,
    IN  NIC_DATA    *Nic
)
{
    return EFI_SUCCESS;
}

/**
  This function is called when UndiGetInitInfo is invoked

  @param  Cdb        A pointer to the PXE_CDB.
  @param  Nic        A pointer to the Nic.

  @retval EFI_STATUS
**/
EFI_STATUS
EFIAPI
RndisUndiGetInitInfo (
    IN  PXE_CDB     *Cdb,
    IN  NIC_DATA    *Nic
)
{
    USB_ETHERNET_PROTOCOL       *UsbEthDevice;
    USB_RNDIS_DEVICE            *UsbRndisDevice;
    PXE_DB_GET_INIT_INFO        *Db;

    if ((Cdb == NULL) || (Nic == NULL)) {
        return EFI_INVALID_PARAMETER;
    }

    UsbEthDevice    = Nic->UsbEth;
    UsbRndisDevice  = USB_RNDIS_DEVICE_FROM_THIS (UsbEthDevice);

    Db = (PXE_DB_GET_INIT_INFO*)(UINTN)Cdb->DBaddr;

    Db->FrameDataLen = UsbRndisDevice->MaxTransferSize - sizeof (REMOTE_NDIS_PACKET_MSG) - PXE_MAC_HEADER_LEN_ETHER;

    // Limit Max MTU size to 1500 bytes as RNDIS spec.
    if (Db->FrameDataLen > PXE_MAX_TXRX_UNIT_ETHER) {
        Db->FrameDataLen = PXE_MAX_TXRX_UNIT_ETHER;
    }

    return EFI_SUCCESS;
}

/**
    This function is called when RndisUndiGetConfigInfo is invoked

    @param  Cdb                   A pointer to the PXE_CDB.
    @param  Nic                   A pointer to the Nic.

    @retval EFI_STATUS
**/
EFI_STATUS
EFIAPI
RndisUndiGetConfigInfo (
    IN  PXE_CDB     *Cdb,
    IN  NIC_DATA    *Nic
)
{
    return EFI_SUCCESS;
}

/**
    This function is called when UndiInitialize is invoked

    @param  Cdb                   A pointer to the PXE_CDB.
    @param  Nic                   A pointer to the Nic.

    @retval EFI_STATUS
**/
EFI_STATUS
EFIAPI
RndisUndiInitialize (
    IN  PXE_CDB     *Cdb,
    IN  NIC_DATA    *Nic
)
{
    USB_ETHERNET_PROTOCOL           *UsbEthDriver;
    USB_RNDIS_DEVICE                *UsbRndisDevice;
    REMOTE_NDIS_INITIALIZE_MSG		RndisInitMsg;
    REMOTE_NDIS_INITIALIZE_CMPLT    RndisInitMsgCmplt;
    EFI_STATUS                      Status;

    if ((Cdb == NULL) || (Nic == NULL)) {
        return EFI_INVALID_PARAMETER;
    }

    UsbEthDriver   = Nic->UsbEth;
    UsbRndisDevice = USB_RNDIS_DEVICE_FROM_THIS (UsbEthDriver);

    ZeroMem (&RndisInitMsg, sizeof (REMOTE_NDIS_INITIALIZE_MSG));
    ZeroMem (&RndisInitMsgCmplt, sizeof (REMOTE_NDIS_INITIALIZE_CMPLT));

    RndisInitMsg.MessageType     = RNDIS_INITIALIZE_MSG;
    RndisInitMsg.MessageLength   = sizeof (REMOTE_NDIS_INITIALIZE_MSG);
    RndisInitMsg.RequestID       = UsbRndisDevice->RequestId;
    RndisInitMsg.MajorVersion    = FixedPcdGet32 (PcdRndisMajorVersion);
    RndisInitMsg.MinorVersion    = FixedPcdGet32 (PcdRndisMinorVersion);
    RndisInitMsg.MaxTransferSize = FixedPcdGet32 (PcdMaxTransferSize);

    RndisInitMsgCmplt.MessageType   = RNDIS_INITIALIZE_CMPLT;
    RndisInitMsgCmplt.MessageLength = sizeof (REMOTE_NDIS_INITIALIZE_CMPLT);

    Status = RndisControlMsg (
                    UsbRndisDevice,
                    (REMOTE_NDIS_MSG_HEADER*)&RndisInitMsg,
                    (REMOTE_NDIS_MSG_HEADER*)&RndisInitMsgCmplt );

    UsbRndisDevice->RequestId++;

    if (EFI_ERROR(Status) || (RndisInitMsgCmplt.Status & BIT31)) {
        return Status;
    }

    // Only Wired Medium is supported
    if (RndisInitMsgCmplt.Medium) {
        return EFI_UNSUPPORTED;
    }

    UsbRndisDevice->Medium                = RndisInitMsgCmplt.Medium;
    UsbRndisDevice->MaxPacketsPerTransfer = RndisInitMsgCmplt.MaxPacketsPerTransfer;
    UsbRndisDevice->MaxTransferSize       = RndisInitMsgCmplt.MaxTransferSize;
    UsbRndisDevice->PacketAlignmentFactor = RndisInitMsgCmplt.PacketAlignmentFactor;

    DEBUG((DEBUG_VERBOSE, "Medium : %x \n", RndisInitMsgCmplt.Medium));
    DEBUG((DEBUG_VERBOSE, "MaxPacketsPerTransfer : %x \n", RndisInitMsgCmplt.MaxPacketsPerTransfer));
    DEBUG((DEBUG_VERBOSE, "MaxTransferSize : %x\n", RndisInitMsgCmplt.MaxTransferSize));
    DEBUG((DEBUG_VERBOSE, "PacketAlignmentFactor : %x\n", RndisInitMsgCmplt.PacketAlignmentFactor));

    return Status;
}

/**
    This function is called when UndiReset is invoked

    @param  Cdb                   A pointer to the PXE_CDB.
    @param  Nic                   A pointer to the Nic.

    @retval EFI_STATUS
**/
EFI_STATUS
EFIAPI
RndisUndiReset (
    IN  PXE_CDB     *Cdb,
    IN  NIC_DATA    *Nic
)
{
    USB_ETHERNET_PROTOCOL       *UsbEthDriver;
    USB_RNDIS_DEVICE            *UsbRndisDevice;
    REMOTE_NDIS_RESET_MSG       RndisResetMsg;
    REMOTE_NDIS_RESET_CMPLT     RndisResetCmplt;
    EFI_STATUS                  Status;

    if ((Cdb == NULL) || (Nic == NULL)) {
        return EFI_INVALID_PARAMETER;
    }

    UsbEthDriver   = Nic->UsbEth;
    UsbRndisDevice = USB_RNDIS_DEVICE_FROM_THIS(UsbEthDriver);

    ZeroMem (&RndisResetMsg, sizeof (REMOTE_NDIS_RESET_MSG));
    ZeroMem (&RndisResetCmplt, sizeof (REMOTE_NDIS_RESET_CMPLT));

    RndisResetMsg.MessageType   = RNDIS_RESET_MSG;
    RndisResetMsg.MessageLength = sizeof(REMOTE_NDIS_RESET_MSG);

    RndisResetCmplt.MessageType   = RNDIS_RESET_CMPLT;
    RndisResetCmplt.MessageLength = sizeof (REMOTE_NDIS_RESET_CMPLT);

    Status = RndisControlMsg (
                        UsbRndisDevice,
                        (REMOTE_NDIS_MSG_HEADER*)&RndisResetMsg,
                        (REMOTE_NDIS_MSG_HEADER*)&RndisResetCmplt );

    UsbRndisDevice->RequestId = 1;        // Let's start with 1

    if (EFI_ERROR(Status) || (RndisResetCmplt.Status & 0x80000000)) {
        return EFI_DEVICE_ERROR;
    }

    return EFI_SUCCESS;
}

/**
    This function is called when UndiShutdown is invoked

    @param  Cdb                   A pointer to the PXE_CDB.
    @param  Nic                   A pointer to the Nic.

    @retval EFI_STATUS
**/
EFI_STATUS
EFIAPI
RndisUndiShutdown (
    IN  PXE_CDB     *Cdb,
    IN  NIC_DATA    *Nic
)
{
    USB_ETHERNET_PROTOCOL       *UsbEthDriver;
    USB_RNDIS_DEVICE            *UsbRndisDevice;
    REMOTE_NDIS_HALT_MSG        RndisHltMsg;
    EFI_STATUS                  Status;

    if ((Cdb == NULL) || (Nic == NULL)) {
        return EFI_INVALID_PARAMETER;
    }

    UsbEthDriver   = Nic->UsbEth;
    UsbRndisDevice = USB_RNDIS_DEVICE_FROM_THIS(UsbEthDriver);

    ZeroMem (&RndisHltMsg, sizeof (REMOTE_NDIS_HALT_MSG));

    RndisHltMsg.MessageType   = RNDIS_HLT_MSG;
    RndisHltMsg.MessageLength = sizeof (REMOTE_NDIS_HALT_MSG);

    Status = RndisControlMsg (
                        UsbRndisDevice,
                        (REMOTE_NDIS_MSG_HEADER*)&RndisHltMsg,
                        NULL );

    UsbRndisDevice->RequestId = 1;
    return Status;
}

/**
    Update the Media connection.

    @param  Cdb                   A pointer to the PXE_CDB.
    @param  Nic                   A pointer to the Nic.

    @retval EFI_STATUS
**/
EFI_STATUS
EFIAPI
RndisUndiGetStatus (
    IN  PXE_CDB     *Cdb,
    IN  NIC_DATA    *Nic
)
{
    if ((Cdb == NULL) || (Nic == NULL)) {
        return EFI_INVALID_PARAMETER;
    }

    Cdb->StatFlags &= ~(PXE_STATFLAGS_GET_STATUS_NO_MEDIA);
    return EFI_SUCCESS;
}

/**
    Transmit the data after appending RNDIS header

    @param  Cdb
    @param  UsbEthDriver
    @param  BulkOutData
    @param  DataLength

    @retval EFI_STATUS
**/
EFI_STATUS
EFIAPI
RndisUndiTransmit (
    IN PXE_CDB                     *Cdb,
    IN USB_ETHERNET_PROTOCOL       *UsbEthDriver,
    VOID                           *BulkOutData,
    UINTN                          *DataLength
)
{

    EFI_STATUS                  Status;
    USB_RNDIS_DEVICE            *UsbRndisDevice;
    REMOTE_NDIS_PACKET_MSG      *RndisPacketMsg;
    UINTN                       TransferLength;

    if ((Cdb == NULL) || (UsbEthDriver == NULL) ||
        (BulkOutData == NULL) || (DataLength == NULL)) {
        return EFI_INVALID_PARAMETER;
    }

    UsbRndisDevice = USB_RNDIS_DEVICE_FROM_THIS(UsbEthDriver);

    RndisPacketMsg = AllocateZeroPool(sizeof(REMOTE_NDIS_PACKET_MSG) + *DataLength);
    if (RndisPacketMsg == NULL) {
        return EFI_OUT_OF_RESOURCES;
    }

    RndisPacketMsg->MessageType   = RNDIS_PACKET_MSG;
    RndisPacketMsg->MessageLength = sizeof(REMOTE_NDIS_PACKET_MSG) + (UINT32)*DataLength;
    RndisPacketMsg->DataOffset    = sizeof(REMOTE_NDIS_PACKET_MSG) - 8;
    RndisPacketMsg->DataLength    = (UINT32)*DataLength;

    CopyMem (
        ((UINT8*)RndisPacketMsg) + sizeof(REMOTE_NDIS_PACKET_MSG),
        BulkOutData,
        *DataLength);

    TransferLength = RndisPacketMsg->MessageLength;

    Status = RndisTransmitDataMsg (
                        UsbRndisDevice,
                        (REMOTE_NDIS_MSG_HEADER*)RndisPacketMsg,
                        &TransferLength);
    FreePool(RndisPacketMsg);
    return Status;
}

/**
    Receives and removes RNDIS header and returns the raw data

    @param  Cdb                   A pointer to the PXE_CDB.
    @param  UsbEthDriver
    @param  BulkInData
    @param  DataLength

    @retval EFI_STATUS
**/
EFI_STATUS
EFIAPI
RndisUndiReceive (
    IN PXE_CDB                      *Cdb,
    IN USB_ETHERNET_PROTOCOL        *This,
    OUT VOID                        *Packet,
    OUT UINTN                       *PacketLength
)
{
    EFI_STATUS                  Status;
    USB_RNDIS_DEVICE            *UsbRndisDevice;
    REMOTE_NDIS_PACKET_MSG      *RndisPacketMsg;
    UINTN                       TransferLength;
    VOID                        *Buffer;
    PACKET_LIST                 *HeadPacket = NULL;
    PACKET_LIST                 *PacketList;
    UINT32					    ReceivedBytes = 0;

    if ((Cdb == NULL) || (This == NULL) ||
        (Packet == NULL) || (PacketLength == NULL)) {
        return EFI_INVALID_PARAMETER;
    }

    UsbRndisDevice = USB_RNDIS_DEVICE_FROM_THIS(This);

    // Check if there is any outstanding packet to receive
    // The buffer allocated has a linked List followed by the packet.
    do {

        Buffer = AllocatePool(sizeof(PACKET_LIST) + sizeof(REMOTE_NDIS_PACKET_MSG) + UsbRndisDevice->MaxTransferSize);
        if (Buffer == NULL) {
            return EFI_OUT_OF_RESOURCES;
        }

        PacketList = (PACKET_LIST*)Buffer;
        // Save the original address for freeing it up
        PacketList->OrgBuffer       = Buffer;
        PacketList->RemainingLength = UsbRndisDevice->MaxTransferSize;

        RndisPacketMsg = (REMOTE_NDIS_PACKET_MSG*)((UINT8*)Buffer + sizeof(PACKET_LIST));
        ZeroMem (RndisPacketMsg, sizeof (REMOTE_NDIS_PACKET_MSG));
        TransferLength = UsbRndisDevice->MaxTransferSize;

        Status = RndisReceiveDataMsg (
                            UsbRndisDevice,
                            (REMOTE_NDIS_MSG_HEADER*)RndisPacketMsg,
                            &TransferLength );
        if (EFI_ERROR(Status) || (TransferLength == 0)){
            FreePool(Buffer);
            break;
        }

        // Handle Multiple packets
        if ((RndisPacketMsg->MessageType == RNDIS_PACKET_MSG) &&
            (RndisPacketMsg->DataOffset == sizeof(REMOTE_NDIS_PACKET_MSG) - 8)){
            //Insert Packet
            InsertTailList(&UsbRndisDevice->ReceivePacketList, Buffer);
        }

    } while (TRUE);

    // Check if they linked list has any received buffer. If yes report it.
    if (IsListEmpty (&UsbRndisDevice->ReceivePacketList)) {
        return EFI_NOT_FOUND;
    }

    HeadPacket = (PACKET_LIST*)GetFirstNode (&UsbRndisDevice->ReceivePacketList);

    RndisPacketMsg = (REMOTE_NDIS_PACKET_MSG*)((UINT8*)HeadPacket + sizeof (PACKET_LIST));

    PrintRndisMsg ((REMOTE_NDIS_MSG_HEADER*)RndisPacketMsg);

    if ((RndisPacketMsg->MessageType == RNDIS_PACKET_MSG) &&
        (RndisPacketMsg->DataOffset == sizeof(REMOTE_NDIS_PACKET_MSG) - 8)){

        if (*PacketLength >= RndisPacketMsg->DataLength) {
            CopyMem (
                Packet,
                (UINT8 *)RndisPacketMsg + RndisPacketMsg->DataOffset + 8,
                RndisPacketMsg->DataLength );

            ReceivedBytes += RndisPacketMsg->DataLength;
            //Packet         = ((UINT8*)Packet) + RndisPacketMsg->DataLength;

            ((PACKET_LIST*)HeadPacket)->RemainingLength -= RndisPacketMsg->DataLength;
        } else {
            DEBUG((DEBUG_ERROR, "RndisUndiReceive:Buffer too small %x\n", RndisPacketMsg->DataLength));
            *PacketLength = RndisPacketMsg->DataLength;
            return EFI_BUFFER_TOO_SMALL;
        }

        // check if there this is a multi-packet message. If so update the pointer so that next Receive call will return that data.
        RndisPacketMsg = (REMOTE_NDIS_PACKET_MSG*)((UINT8*)RndisPacketMsg + RndisPacketMsg->DataLength);
        PacketList     = (PACKET_LIST*)((UINT8 *)RndisPacketMsg - sizeof(PACKET_LIST));

        if ((HeadPacket->RemainingLength > sizeof(REMOTE_NDIS_PACKET_MSG)) &&
            (RndisPacketMsg->MessageType == RNDIS_PACKET_MSG) &&
            (RndisPacketMsg->DataOffset == sizeof(REMOTE_NDIS_PACKET_MSG) - 8)){

            // Multi-Packet msg is found. Since the first packet is consumed, update the linked list to point this new packet.
            PacketList->OrgBuffer       = HeadPacket->OrgBuffer;
            PacketList->RemainingLength = HeadPacket->RemainingLength;

            RemoveEntryList (&HeadPacket->PacketList);
            InsertHeadList (&UsbRndisDevice->ReceivePacketList, &PacketList->PacketList);
        } else {
            RemoveEntryList (&HeadPacket->PacketList);
            FreePool ((PACKET_LIST*)HeadPacket->OrgBuffer);
        }
    } else {
        // Packet doesn't contain valid header
        DEBUG((DEBUG_ERROR, "RndisUndiReceive:Invalid RNDIS Packet received\n"));
        RemoveEntryList (&HeadPacket->PacketList);
        FreePool ((PACKET_LIST*)HeadPacket->OrgBuffer);
    }

    if (ReceivedBytes != 0) {
    	*PacketLength = ReceivedBytes;
    }
    return EFI_SUCCESS;
}


/**
    This is a dummy function which just returns. Unimplimented USB_ETHERNET_PROTOCOL functions
    point to this function.

    @param  Cdb                   A pointer to the PXE_CDB.
    @param  Nic                   A pointer to the Nic.

    @retval EFI_STATUS
**/
EFI_STATUS
EFIAPI
RndisDummyReturn (
    IN  PXE_CDB     *Cdb,
    IN  NIC_DATA    *Nic
)
{
	return EFI_SUCCESS;
}

/**
    This function send the RNDIS command through the device's control endpoint

    @param  UsbRndisDevice
    @param  RndisMsg
    @param  RndisMsgResponse

    @retval EFI_STATUS
**/
EFI_STATUS
RndisControlMsg (
    IN  USB_RNDIS_DEVICE            *UsbRndisDevice,
    IN  REMOTE_NDIS_MSG_HEADER		*RndisMsg,
    OUT REMOTE_NDIS_MSG_HEADER      *RndisMsgResponse
)
{
    EFI_USB_IO_PROTOCOL             *UsbIo = UsbRndisDevice->UsbIo;
	EFI_USB_DEVICE_REQUEST          DevReq;
	UINT32                          UsbStatus;
	EFI_STATUS                      Status;
	UINT32                          SaveResponseType = 0;
	UINT32                          SaveResponseLength = 0;
    UINT32                          Index;
    REMOTE_NDIS_INITIALIZE_CMPLT    *RndisInitCmplt = (REMOTE_NDIS_INITIALIZE_CMPLT *)RndisMsgResponse;

    if (RndisMsgResponse) {
        SaveResponseType = RndisMsgResponse->MessageType;
        SaveResponseLength = RndisMsgResponse->MessageLength;
    }

	ZeroMem (&DevReq, sizeof (EFI_USB_DEVICE_REQUEST));

	DevReq.RequestType  = USB_REQ_TYPE_CLASS | USB_TARGET_INTERFACE;
	DevReq.Request      = SEND_ENCAPSULATED_COMMAND;
	DevReq.Value        = 0;
	DevReq.Index        = 0;
	DevReq.Length       = (UINT16)RndisMsg->MessageLength;

	PrintRndisMsg (RndisMsg);

	Status = UsbIo->UsbControlTransfer (
                        UsbIo,
                        &DevReq,
                        EfiUsbDataOut,
                        FixedPcdGet32 (PcdUsbEthernetTransferTimeOut),
                        RndisMsg,
                        RndisMsg->MessageLength,
                        &UsbStatus );

	DEBUG((DEBUG_VERBOSE, "RndisControlMsg: UsbStatus : %x Status : %r RndisMsgResponse : %lx\n",
                                        UsbStatus, Status, RndisMsgResponse));

    // Error or no response expected
	if ((EFI_ERROR(Status)) || (RndisMsgResponse == NULL)) {
	    return Status;
	}

	for (Index = 0; Index < (FixedPcdGet32 (PcdRndisControlTimeOut) / 100); Index++){

	    ZeroMem (&DevReq, sizeof (EFI_USB_DEVICE_REQUEST));

	    DevReq.RequestType  = USB_ENDPOINT_DIR_IN | USB_REQ_TYPE_CLASS | USB_TARGET_INTERFACE;
	    DevReq.Request      = GET_ENCAPSULATED_RESPONSE;
	    DevReq.Value        = 0;
	    DevReq.Index        = 0;
	    DevReq.Length       = (UINT16)RndisMsgResponse->MessageLength;

	    Status = UsbIo->UsbControlTransfer (
                            UsbIo,
                            &DevReq,
                            EfiUsbDataIn,
                            FixedPcdGet32 (PcdUsbEthernetTransferTimeOut),
                            RndisMsgResponse,
                            RndisMsgResponse->MessageLength,
                            &UsbStatus );

	    DEBUG ((DEBUG_VERBOSE, "RndisControlMsg Response: UsbStatus : %x Status : %r \n", UsbStatus, Status));

	    PrintRndisMsg (RndisMsgResponse);

	    if (!EFI_ERROR (Status)) {
            if ((RndisInitCmplt->RequestID != ((REMOTE_NDIS_INITIALIZE_CMPLT *)RndisMsg)->RequestID) ||
                (RndisInitCmplt->MessageType != SaveResponseType)){
                DEBUG((DEBUG_ERROR, "Retry the response\n"));
                continue;
            }
	        return Status;
	    }

	    RndisMsgResponse->MessageType   = SaveResponseType;
	    RndisMsgResponse->MessageLength = SaveResponseLength;

        gBS->Stall(100000); // 100msec
	}

	DEBUG((DEBUG_ERROR, "RndisControlMsg: TimeOut\n"));
	return EFI_TIMEOUT;
}

/**
    This function send the RNDIS command through the device's Data endpoint

    @param  UsbRndisDevice
    @param  RndisMsg
    @param  TransferLength

    @retval EFI_STATUS
**/
EFI_STATUS
RndisTransmitDataMsg (
    IN  USB_RNDIS_DEVICE            *UsbRndisDevice,
    IN  REMOTE_NDIS_MSG_HEADER		*RndisMsg,
    UINTN                           *TransferLength
)
{

    EFI_STATUS                      Status;
    UINT32                          UsbStatus;

    if (UsbRndisDevice->BulkInEndpoint == 0) {
        GetEndpoint (UsbRndisDevice->UsbIoCdcData, UsbRndisDevice);
    }

    PrintRndisMsg(RndisMsg);

    Status = UsbRndisDevice->UsbIoCdcData->UsbBulkTransfer (
                        UsbRndisDevice->UsbIoCdcData,
                        UsbRndisDevice->BulkOutEndpoint,
                        RndisMsg,
                        TransferLength,
                        FixedPcdGet64 (PcdUsbTxEthernetBulkTimeOut),
                        &UsbStatus );
    return Status;
}

/**
    This function send the RNDIS command through the device's Data endpoint

    @param  UsbRndisDevice
    @param  RndisMsg
    @param  TransferLength

    @retval EFI_STATUS
**/
EFI_STATUS
RndisReceiveDataMsg (
    IN  USB_RNDIS_DEVICE            *UsbRndisDevice,
    IN  REMOTE_NDIS_MSG_HEADER      *RndisMsg,
    UINTN                           *TransferLength
)
{
    EFI_STATUS    Status;
    UINT32        UsbStatus = 0;

    if (UsbRndisDevice->BulkInEndpoint == 0){
        GetEndpoint (UsbRndisDevice->UsbIoCdcData, UsbRndisDevice);
    }

    Status = UsbRndisDevice->UsbIoCdcData->UsbBulkTransfer (
                                            UsbRndisDevice->UsbIoCdcData,
                                            UsbRndisDevice->BulkInEndpoint,
                                            RndisMsg,
                                            TransferLength,
                                            FixedPcdGet64 (PcdUsbRxEthernetBulkTimeOut),
                                            &UsbStatus );
    PrintRndisMsg (RndisMsg);
    return Status;
}

/**
    Prints RNDIS Header and Data

    @param  RndisMsg

    @retval None
**/
VOID
PrintRndisMsg (
    REMOTE_NDIS_MSG_HEADER      *RndisMsg
)
{
    switch (RndisMsg->MessageType) {

        case RNDIS_PACKET_MSG:
            DEBUG ((DEBUG_VERBOSE, "RNDIS_PACKET_MSG:\n"));
            break;

        case RNDIS_INITIALIZE_MSG:
            DEBUG ((DEBUG_VERBOSE, "RNDIS_INITIALIZE_MSG:\n"));
            break;

        case RNDIS_INITIALIZE_CMPLT:
            DEBUG ((DEBUG_VERBOSE, "RNDIS_INITIALIZE_CMPLT:\n"));
            break;

        case RNDIS_HLT_MSG:
            DEBUG ((DEBUG_VERBOSE, "RNDIS_HLT_MSG:\n"));
            break;

        case RNDIS_QUERY_MSG:
            DEBUG ((DEBUG_VERBOSE, "RNDIS_QUERY_MSG:\n"));
            break;

        case RNDIS_QUERY_CMPLT:
            DEBUG ((DEBUG_VERBOSE, "RNDIS_QUERY_CMPLT:\n"));
            break;

        case RNDIS_SET_MSG:
            DEBUG ((DEBUG_VERBOSE, "RNDIS_SET_MSG:\n"));
            break;

        case RNDIS_SET_CMPLT:
            DEBUG ((DEBUG_VERBOSE, "RNDIS_SET_CMPLT:\n"));
            break;

        case RNDIS_RESET_MSG:
            DEBUG ((DEBUG_VERBOSE, "RNDIS_RESET_MSG:\n"));
            break;

        case RNDIS_RESET_CMPLT:
            DEBUG ((DEBUG_VERBOSE, "RNDIS_RESET_CMPLT:\n"));
            break;

        case RNDIS_INDICATE_STATUS_MSG:
            DEBUG ((DEBUG_VERBOSE, "RNDIS_INDICATE_STATUS_MSG:\n"));
            break;

        case RNDIS_KEEPALIVE_MSG:
            DEBUG ((DEBUG_VERBOSE, "RNDIS_KEEPALIVE_MSG:\n"));
            break;

        case RNDIS_KEEPALIVE_CMPLT:
            DEBUG ((DEBUG_VERBOSE, "RNDIS_KEEPALIVE_CMPLT:\n"));
    }
}
