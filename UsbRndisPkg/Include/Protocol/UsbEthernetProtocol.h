/** @file
  Definitions for USB Ethernet Protocol

  Copyright (c) 2021, American Megatrends International LLC.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _USB_ETHERNET_PROTOCOL_H
#define _USB_ETHERNET_PROTOCOL_H

#define USB_ETHERNET_PROTOCOL_GUID \
    {0x8d8969cc, 0xfeb0, 0x4303, {0xb2, 0x1a, 0x1f, 0x11, 0x6f, 0x38, 0x56, 0x43}}

typedef struct _USB_ETHERNET_PROTOCOL USB_ETHERNET_PROTOCOL;

#define USB_CDC_CLASS           0x02
#define USB_CDC_ACM_SUBCLASS    0x02
#define USB_CDC_ECM_SUBCLASS    0x06
#define USB_CDC_NCM_SUBCLASS    0x0D
#define USB_CDC_DATA_CLASS      0x0A
#define USB_CDC_DATA_SUBCLASS   0x00
#define USB_NO_CLASS_PROTOCOL   0x00
#define USB_NCM_NTB_PROTOCOL    0x01
#define USB_VENDOR_PROTOCOL     0xFF

// Type Values for the DescriptorType Field
#define CS_INTERFACE    0x24
#define CS_ENDPOINT     0x25

// Descriptor SubType in Functional Descriptors
#define HEADER_FUN_DESCRIPTOR   0x00
#define UNION_FUN_DESCRIPTOR    0x06
#define ETHERNET_FUN_DESCRIPTOR 0x0F

#define MAX_USBLAN_INTERFACE    0x10

// USB Header functional Descriptor
typedef struct {
    UINT8   FunctionLength;
    UINT8   DescriptorType;
    UINT8   DescriptorSubtype;
    UINT16  BcdCdc;
} USB_HEADER_FUN_DESCRIPTOR;

// USB Union Functional Descriptor
typedef struct {
    UINT8   FunctionLength;
    UINT8   DescriptorType;
    UINT8   DescriptorSubtype;
    UINT8   MasterInterface;
    UINT8   SlaveInterface;
} USB_UNION_FUN_DESCRIPTOR;

// USB Ethernet Functional Descriptor
typedef struct {
    UINT8   FunctionLength;
    UINT8   DescriptorType;
    UINT8   DescriptorSubtype;
    UINT8   MacAddress;
    UINT32  EthernetStatistics;
    UINT16  MaxSegmentSize;
    UINT16  NumberMcFilters;
    UINT8   NumberPowerFilters;
} USB_ETHERNET_FUN_DESCRIPTOR;

typedef struct {
    UINT32  UsBitRate;
    UINT32  DsBitRate;
} USB_CONNECT_SPEED_CHANGE;

// Request Type Codes for USB Ethernet
#define USB_ETHERNET_GET_REQ_TYPE   0xA1
#define USB_ETHRTNET_SET_REQ_TYPE   0x21

// Class-Specific Request Codes for Ethernet subclass
// USB ECM 1.2 specification, Section 6.2
#define SET_ETH_MULTICAST_FILTERS_REQ                   0x40
#define SET_ETH_POWER_MANAGEMENT_PATTERN_FILTER_REQ     0x41
#define GET_ETH_POWER_MANAGEMENT_PATTERN_FILTER_REQ     0x42
#define SET_ETH_PACKET_FILTER_REQ                       0x43
#define GET_ETH_STATISTIC_REQ                           0x44

// USB ECM command request length
#define USB_ETH_POWER_FILTER_LENGTH  2  // Section 6.2.3
#define USB_ETH_PACKET_FILTER_LENGTH 0  // Section 6.2.4
#define USB_ETH_STATISTIC            4  // Section 6.2.5

// USB Ethernet Packet Filter Bitmap
// USB ECM 1.2 specification, Section 6.2.4
#define USB_ETH_PACKET_TYPE_PROMISCUOUS     BIT0
#define USB_ETH_PACKET_TYPE_ALL_MULTICAST   BIT1
#define USB_ETH_PACKET_TYPE_DIRECTED        BIT2
#define USB_ETH_PACKET_TYPE_BROADCAST       BIT3
#define USB_ETH_PACKET_TYPE_MULTICAST       BIT4

// USB Ethernet Statistics Feature Selector Codes
// USB ECM 1.2 specification, Section 6.2.5
#define USB_ETH_XMIT_OK                 0x01
#define USB_ETH_RCV_OK                  0x02
#define USB_ETH_XMIT_ERROR              0x03
#define USB_ETH_RCV_ERROR               0x04
#define USB_ETH_RCV_NO_BUFFER           0x05
#define USB_ETH_DIRECTED_BYTES_XMIT     0x06
#define USB_ETH_DIRECTED_FRAMES_XMIT    0x07
#define USB_ETH_MULTICAST_BYTES_XMIT    0x08
#define USB_ETH_MULTICAST_FRAMES_XMIT   0x09
#define USB_ETH_BROADCAST_BYTES_XMIT    0x0A
#define USB_ETH_BROADCAST_FRAMES_XMIT   0x0B
#define USB_ETH_DIRECTED_BYTES_RCV      0x0C
#define USB_ETH_DIRECTED_FRAMES_RCV     0x0D
#define USB_ETH_MULTICAST_BYTES_RCV     0x0E
#define USB_ETH_MULTICAST_FRAMES_RCV    0x0F
#define USB_ETH_BROADCAST_BYTES_RCV     0x10
#define USB_ETH_BROADCAST_FRAMES_RCV    0x11
#define USB_ETH_RCV_CRC_ERROR           0x12
#define USB_ETH_TRANSMIT_QUEUE_LENGTH   0x13
#define USB_ETH_RCV_ERROR_ALIGNMENT     0x14
#define USB_ETH_XMIT_ONE_COLLISION      0x15
#define USB_ETH_XMIT_MORE_COLLISIONS    0x16
#define USB_ETH_XMIT_DEFERRED           0x17
#define USB_ETH_XMIT_MAX_COLLISIONS     0x18
#define USB_ETH_RCV_OVERRUN             0x19
#define USB_ETH_XMIT_UNDERRUN           0x1A
#define USB_ETH_XMIT_HEARTBEAT_FAILURE  0x1B
#define USB_ETH_XMIT_TIMES_CRS_LOST     0x1C
#define USB_ETH_XMIT_LATE_COLLISIONS    0x1D

// NIC Information
typedef struct {
    UINT32                  Signature;
    USB_ETHERNET_PROTOCOL   *UsbEth;
    UINT16                  InterrupOpFlag;
    UINT64                  MappedAddr;
    PXE_MAC_ADDR            McastList[MAX_MCAST_ADDRESS_CNT];
    UINT8                   McastCount;
    UINT64                  MediaHeader[MAX_XMIT_BUFFERS];
    UINT8                   TxBufferCount;
    UINT16                  State;
    BOOLEAN                 CanTransmit;
    BOOLEAN                 CanReceive;
    UINT16                  ReceiveStatus;
    UINT8                   RxFilter;
    UINT32                  RxFrame;
    UINT32                  TxFrame;
    UINT16                  NetworkConnect;
    UINT8                   CableDetect;
    UINT16                  MaxSegmentSize;
    EFI_MAC_ADDRESS         MacAddr;
    PXE_CPB_START_31        PxeStart;
    PXE_CPB_INITIALIZE      PxeInit;
    UINT8                   PermNodeAddress[PXE_MAC_LENGTH];
    UINT8                   CurrentNodeAddress[PXE_MAC_LENGTH];
    UINT8                   BroadcastNodeAddress[PXE_MAC_LENGTH];
    EFI_USB_DEVICE_REQUEST  Request;
} NIC_DATA;

#define NIC_DATA_SIGNATURE  SIGNATURE_32('n', 'i', 'c', 'd')
#define NIC_DATA_FROM_USB_ETHERNET_PROTOCOL(a) CR (a, NIC_DATA, UsbEth, NIC_DATA_SIGNATURE)

/**
  This command is used to determine the operational state of the UNDI.

  @param  Cdb  A pointer to the command descriptor block.
  @param  Nic  A pointer to the Network interface controller data.

  @retval EFI_SUCCESS           The request executed successfully.
  @retval EFI_TIMEOUT           A timeout occurred executing the request.
  @retval EFI_DEVICE_ERROR      The request failed due to a device error.
  @retval EFI_INVALID_PARAMETER One of the parameters has an invalid value.
  @retval EFI_UNSUPPORTED       Not supported.
**/
typedef
EFI_STATUS
(EFIAPI *USB_ETHERNET_UNDI_GET_STATE)(
    IN  PXE_CDB     *Cdb,
    IN  NIC_DATA    *Nic
);

/**
  This command is used to change the UNDI operational state from stopped to started.

  @param  Cdb  A pointer to the command descriptor block.
  @param  Nic  A pointer to the Network interface controller data.

  @retval EFI_SUCCESS           The request executed successfully.
  @retval EFI_TIMEOUT           A timeout occurred executing the request.
  @retval EFI_DEVICE_ERROR      The request failed due to a device error.
  @retval EFI_INVALID_PARAMETER One of the parameters has an invalid value.
  @retval EFI_UNSUPPORTED       Not supported.
**/
typedef
EFI_STATUS
(EFIAPI *USB_ETHERNET_UNDI_START)(
    IN  PXE_CDB     *Cdb,
    IN  NIC_DATA    *Nic
);

/**
  This command is used to change the UNDI operational state from started to stopped.

  @param  Cdb  A pointer to the command descriptor block.
  @param  Nic  A pointer to the Network interface controller data.

  @retval EFI_SUCCESS           The request executed successfully.
  @retval EFI_TIMEOUT           A timeout occurred executing the request.
  @retval EFI_DEVICE_ERROR      The request failed due to a device error.
  @retval EFI_INVALID_PARAMETER One of the parameters has an invalid value.
  @retval EFI_UNSUPPORTED       Not supported.
**/
typedef
EFI_STATUS
(EFIAPI *USB_ETHERNET_UNDI_STOP)(
    IN  PXE_CDB     *Cdb,
    IN  NIC_DATA    *Nic
);

/**
  This command is used to retrieve initialization information that is
  needed by drivers and applications to initialized UNDI.

  @param  Cdb  A pointer to the command descriptor block.
  @param  Nic  A pointer to the Network interface controller data.

  @retval EFI_SUCCESS           The request executed successfully.
  @retval EFI_TIMEOUT           A timeout occurred executing the request.
  @retval EFI_DEVICE_ERROR      The request failed due to a device error.
  @retval EFI_INVALID_PARAMETER One of the parameters has an invalid value.
  @retval EFI_UNSUPPORTED       Not supported.
**/
typedef
EFI_STATUS
(EFIAPI *USB_ETHERNET_UNDI_GET_INIT_INFO)(
    IN  PXE_CDB     *Cdb,
    IN  NIC_DATA    *Nic
);

/**
  This command is used to retrieve configuration information about
  the NIC being controlled by the UNDI.

  @param  Cdb  A pointer to the command descriptor block.
  @param  Nic  A pointer to the Network interface controller data.

  @retval EFI_SUCCESS           The request executed successfully.
  @retval EFI_TIMEOUT           A timeout occurred executing the request.
  @retval EFI_DEVICE_ERROR      The request failed due to a device error.
  @retval EFI_INVALID_PARAMETER One of the parameters has an invalid value.
  @retval EFI_UNSUPPORTED       Not supported.
**/
typedef
EFI_STATUS
(EFIAPI *USB_ETHERNET_UNDI_GET_CONFIG_INFO)(
    IN  PXE_CDB     *Cdb,
    IN  NIC_DATA    *Nic
);

/**
  This command resets the network adapter and initializes UNDI using
  the parameters supplied in the CPB.

  @param  Cdb  A pointer to the command descriptor block.
  @param  Nic  A pointer to the Network interface controller data.

  @retval EFI_SUCCESS           The request executed successfully.
  @retval EFI_TIMEOUT           A timeout occurred executing the request.
  @retval EFI_DEVICE_ERROR      The request failed due to a device error.
  @retval EFI_INVALID_PARAMETER One of the parameters has an invalid value.
  @retval EFI_UNSUPPORTED       Not supported.
**/
typedef
EFI_STATUS
(EFIAPI *USB_ETHERNET_UNDI_INITIALIZE)(
    IN  PXE_CDB     *Cdb,
    IN  NIC_DATA    *Nic
);

/**
  This command resets the network adapter and reinitializes the UNDI
  with the same parameters provided in the Initialize command.

  @param  Cdb  A pointer to the command descriptor block.
  @param  Nic  A pointer to the Network interface controller data.

  @retval EFI_SUCCESS           The request executed successfully.
  @retval EFI_TIMEOUT           A timeout occurred executing the request.
  @retval EFI_DEVICE_ERROR      The request failed due to a device error.
  @retval EFI_INVALID_PARAMETER One of the parameters has an invalid value.
  @retval EFI_UNSUPPORTED       Not supported.
**/
typedef
EFI_STATUS
(EFIAPI *USB_ETHERNET_UNDI_RESET)(
    IN  PXE_CDB     *Cdb,
    IN  NIC_DATA    *Nic
);

/**
  The Shutdown command resets the network adapter and leaves it in a
  safe state for another driver to initialize.

  @param  Cdb  A pointer to the command descriptor block.
  @param  Nic  A pointer to the Network interface controller data.

  @retval EFI_SUCCESS           The request executed successfully.
  @retval EFI_TIMEOUT           A timeout occurred executing the request.
  @retval EFI_DEVICE_ERROR      The request failed due to a device error.
  @retval EFI_INVALID_PARAMETER One of the parameters has an invalid value.
  @retval EFI_UNSUPPORTED       Not supported.
**/
typedef
EFI_STATUS
(EFIAPI *USB_ETHERNET_UNDI_SHUTDOWN)(
    IN  PXE_CDB     *Cdb,
    IN  NIC_DATA    *Nic
);

/**
  The Interrupt Enables command can be used to read and/or change
  the current external interrupt enable settings.

  @param  Cdb  A pointer to the command descriptor block.
  @param  Nic  A pointer to the Network interface controller data.

  @retval EFI_SUCCESS           The request executed successfully.
  @retval EFI_TIMEOUT           A timeout occurred executing the request.
  @retval EFI_DEVICE_ERROR      The request failed due to a device error.
  @retval EFI_INVALID_PARAMETER One of the parameters has an invalid value.
  @retval EFI_UNSUPPORTED       Not supported.
**/
typedef
EFI_STATUS
(EFIAPI *USB_ETHERNET_UNDI_INTERRUPT_ENABLE)(
    IN  PXE_CDB     *Cdb,
    IN  NIC_DATA    *Nic
);

/**
  This command is used to read and change receive filters and,
  if supported, read and change the multicast MAC address filter list.

  @param  Cdb  A pointer to the command descriptor block.
  @param  Nic  A pointer to the Network interface controller data.

  @retval EFI_SUCCESS           The request executed successfully.
  @retval EFI_TIMEOUT           A timeout occurred executing the request.
  @retval EFI_DEVICE_ERROR      The request failed due to a device error.
  @retval EFI_INVALID_PARAMETER One of the parameters has an invalid value.
  @retval EFI_UNSUPPORTED       Not supported.
**/
typedef
EFI_STATUS
(EFIAPI *USB_ETHERNET_UNDI_RECEIVE_FILTER)(
    IN  PXE_CDB     *Cdb,
    IN  NIC_DATA    *Nic
);

/**
  This command is used to get current station and broadcast MAC addresses
  and, if supported, to change the current station MAC address.

  @param  Cdb  A pointer to the command descriptor block.
  @param  Nic  A pointer to the Network interface controller data.

  @retval EFI_SUCCESS           The request executed successfully.
  @retval EFI_TIMEOUT           A timeout occurred executing the request.
  @retval EFI_DEVICE_ERROR      The request failed due to a device error.
  @retval EFI_INVALID_PARAMETER One of the parameters has an invalid value.
  @retval EFI_UNSUPPORTED       Not supported.
**/
typedef
EFI_STATUS
(EFIAPI *USB_ETHERNET_UNDI_STATION_ADDRESS)(
    IN  PXE_CDB     *Cdb,
    IN  NIC_DATA    *Nic
);

/**
  This command is used to read and clear the NIC traffic statistics.

  @param  Cdb  A pointer to the command descriptor block.
  @param  Nic  A pointer to the Network interface controller data.

  @retval EFI_SUCCESS           The request executed successfully.
  @retval EFI_TIMEOUT           A timeout occurred executing the request.
  @retval EFI_DEVICE_ERROR      The request failed due to a device error.
  @retval EFI_INVALID_PARAMETER One of the parameters has an invalid value.
  @retval EFI_UNSUPPORTED       Not supported.
**/
typedef
EFI_STATUS
(EFIAPI *USB_ETHERNET_UNDI_STATISTICS)(
    IN  PXE_CDB     *Cdb,
    IN  NIC_DATA    *Nic
);

/**
  Translate a multicast IPv4 or IPv6 address to a multicast MAC address.

  @param  Cdb  A pointer to the command descriptor block.
  @param  Nic  A pointer to the Network interface controller data.

  @retval EFI_SUCCESS           The request executed successfully.
  @retval EFI_TIMEOUT           A timeout occurred executing the request.
  @retval EFI_DEVICE_ERROR      The request failed due to a device error.
  @retval EFI_INVALID_PARAMETER One of the parameters has an invalid value.
  @retval EFI_UNSUPPORTED       Not supported.
**/
typedef
EFI_STATUS
(EFIAPI *USB_ETHERNET_UNDI_MCAST_IPTOMAC)(
    IN  PXE_CDB     *Cdb,
    IN  NIC_DATA    *Nic
);

/**
  This command is used to read and write (if supported by NIC H/W)
  nonvolatile storage on the NIC.

  @param  Cdb  A pointer to the command descriptor block.
  @param  Nic  A pointer to the Network interface controller data.

  @retval EFI_SUCCESS           The request executed successfully.
  @retval EFI_TIMEOUT           A timeout occurred executing the request.
  @retval EFI_DEVICE_ERROR      The request failed due to a device error.
  @retval EFI_INVALID_PARAMETER One of the parameters has an invalid value.
  @retval EFI_UNSUPPORTED       Not supported.
**/
typedef
EFI_STATUS
(EFIAPI *USB_ETHERNET_UNDI_NV_DATA)(
    IN  PXE_CDB     *Cdb,
    IN  NIC_DATA    *Nic
);

/**
  This command returns the current interrupt status and/or the
  transmitted buffer addresses and the current media status.

  @param  Cdb  A pointer to the command descriptor block.
  @param  Nic  A pointer to the Network interface controller data.

  @retval EFI_SUCCESS           The request executed successfully.
  @retval EFI_TIMEOUT           A timeout occurred executing the request.
  @retval EFI_DEVICE_ERROR      The request failed due to a device error.
  @retval EFI_INVALID_PARAMETER One of the parameters has an invalid value.
  @retval EFI_UNSUPPORTED       Not supported.
**/
typedef
EFI_STATUS
(EFIAPI *USB_ETHERNET_UNDI_GET_STATUS)(
    IN  PXE_CDB     *Cdb,
    IN  NIC_DATA    *Nic
);

/**
  This command is used to fill the media header(s) in transmit packet(s).

  @param  Cdb  A pointer to the command descriptor block.
  @param  Nic  A pointer to the Network interface controller data.

  @retval EFI_SUCCESS           The request executed successfully.
  @retval EFI_TIMEOUT           A timeout occurred executing the request.
  @retval EFI_DEVICE_ERROR      The request failed due to a device error.
  @retval EFI_INVALID_PARAMETER One of the parameters has an invalid value.
  @retval EFI_UNSUPPORTED       Not supported.
**/
typedef
EFI_STATUS
(EFIAPI *USB_ETHERNET_UNDI_FILL_HEADER)(
    IN  PXE_CDB     *Cdb,
    IN  NIC_DATA    *Nic
);

/**
  The Transmit command is used to place a packet into the transmit queue.

  @param  Cdb  A pointer to the command descriptor block.
  @param  Nic  A pointer to the Network interface controller data.

  @retval EFI_SUCCESS           The request executed successfully.
  @retval EFI_TIMEOUT           A timeout occurred executing the request.
  @retval EFI_DEVICE_ERROR      The request failed due to a device error.
  @retval EFI_INVALID_PARAMETER One of the parameters has an invalid value.
  @retval EFI_UNSUPPORTED       Not supported.
**/
typedef
EFI_STATUS
(EFIAPI *USB_ETHERNET_UNDI_TRANSMIT)(
    IN  PXE_CDB     *Cdb,
    IN  NIC_DATA    *Nic
);

/**
  When the network adapter has received a frame, this command is used
  to copy the frame into driver/application storage.

  @param  Cdb  A pointer to the command descriptor block.
  @param  Nic  A pointer to the Network interface controller data.

  @retval EFI_SUCCESS           The request executed successfully.
  @retval EFI_TIMEOUT           A timeout occurred executing the request.
  @retval EFI_DEVICE_ERROR      The request failed due to a device error.
  @retval EFI_INVALID_PARAMETER One of the parameters has an invalid value.
  @retval EFI_UNSUPPORTED       Not supported.
**/
typedef
EFI_STATUS
(EFIAPI *USB_ETHERNET_UNDI_RECEIVE)(
    IN  PXE_CDB     *Cdb,
    IN  NIC_DATA    *Nic
);

/**
  This command resets the network adapter and initializes UNDI using
  the parameters supplied in the CPB.

  @param  Cdb  A pointer to the command descriptor block.
  @param  Nic  A pointer to the Network interface controller data.

  @retval EFI_SUCCESS           The request executed successfully.
  @retval EFI_TIMEOUT           A timeout occurred executing the request.
  @retval EFI_DEVICE_ERROR      The request failed due to a device error.
  @retval EFI_INVALID_PARAMETER One of the parameters has an invalid value.
  @retval EFI_UNSUPPORTED       Not supported.
**/
typedef
EFI_STATUS
(EFIAPI *USB_ETHERNET_INITIALIZE)(
    IN  PXE_CDB     *Cdb,
    IN  NIC_DATA    *Nic
);

/**
  This command is used to read and clear the NIC traffic statistics.

  @param  Cdb  A pointer to the command descriptor block.
  @param  Nic  A pointer to the Network interface controller data.

  @retval EFI_SUCCESS           The request executed successfully.
  @retval EFI_TIMEOUT           A timeout occurred executing the request.
  @retval EFI_DEVICE_ERROR      The request failed due to a device error.
  @retval EFI_INVALID_PARAMETER One of the parameters has an invalid value.
  @retval EFI_UNSUPPORTED       Not supported.
**/
typedef
EFI_STATUS
(EFIAPI *USB_ETHERNET_STATISTICE)(
    IN  NIC_DATA    *Nic,
    IN  UINT64      DbAddr,
    IN  UINT16      DbSize
);

/**
  This function is used to manage a USB device with the bulk transfer pipe. Endpoint is Bulk in.

  @param  Cdb           A pointer to the command descriptor block.
  @param  This          A pointer to the USB_ETHERNET_PROTOCOL instance.
  @param  Packet        A pointer to the buffer of data that will be transmitted to USB
                        device or received from USB device.
  @param  PacketLength  A pointer to the PacketLength

  @retval EFI_SUCCESS           The bulk transfer has been successfully executed.
  @retval EFI_DEVICE_ERROR      The transfer failed. The transfer status is returned in status.
  @retval EFI_INVALID_PARAMETE  One or more parameters are invalid.
  @retval EFI_OUT_OF_RESOURCES  The request could not be submitted due to a lack of resources.
  @retval EFI_TIMEOUT           The control transfer fails due to timeout.
**/
typedef
EFI_STATUS
(EFIAPI *USB_ETHERNET_RECEIVE)(
    IN PXE_CDB                      *Cdb,
    IN USB_ETHERNET_PROTOCOL        *This,
    OUT VOID                        *Packet,
    OUT UINTN                       *PacketLength
);

/**
  This function is used to manage a USB device with the bulk transfer pipe. Endpoint is Bulk out.

  @param  Cdb           A pointer to the command descriptor block.
  @param  This          A pointer to the USB_ETHERNET_PROTOCOL instance.
  @param  Packet        A pointer to the buffer of data that will be transmitted to USB
                        device or received from USB device.
  @param  PacketLength  A pointer to the PacketLength

  @retval EFI_SUCCESS           The bulk transfer has been successfully executed.
  @retval EFI_DEVICE_ERROR      The transfer failed. The transfer status is returned in status.
  @retval EFI_INVALID_PARAMETE  One or more parameters are invalid.
  @retval EFI_OUT_OF_RESOURCES  The request could not be submitted due to a lack of resources.
  @retval EFI_TIMEOUT           The control transfer fails due to timeout.
**/
typedef
EFI_STATUS
(EFIAPI *USB_ETHERNET_TRANSMIT)(
    IN PXE_CDB                      *Cdb,
    IN USB_ETHERNET_PROTOCOL        *This,
    IN VOID                         *Packet,
    IN UINTN                        *PacketLength
);


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
typedef
EFI_STATUS
(EFIAPI *USB_ETHERNET_INTERRUPT)(
    IN USB_ETHERNET_PROTOCOL    *This,
    IN BOOLEAN                  IsNewTransfer,
    IN UINTN                    PollingInterval,
    IN EFI_USB_DEVICE_REQUEST   *Request
);


/**
  Retrieves the USB Ethernet Mac Address.

  @param  This          A pointer to the USB_ETHERNET_PROTOCOL instance.
  @param  MacAddress    A pointer to the caller allocated USB Ethernet Mac Address.

  @retval EFI_SUCCESS           The USB Header Functional descriptor was retrieved successfully.
  @retval EFI_INVALID_PARAMETER UsbHeaderFunDescriptor is NULL.
  @retval EFI_NOT_FOUND         The USB Header Functional descriptor was not found.
**/
typedef
EFI_STATUS
(EFIAPI *USB_GET_ETH_MAC_ADDRESS)(
    IN  USB_ETHERNET_PROTOCOL   *This,
    OUT EFI_MAC_ADDRESS         *MacAddress
);

/**
  Retrieves the USB Ethernet Bulk transfer data size.

  @param  This          A pointer to the USB_ETHERNET_PROTOCOL instance.
  @param  BulkSize      A pointer to the Bulk transfer data size.

  @retval EFI_SUCCESS           The USB Header Functional descriptor was retrieved successfully.
  @retval EFI_INVALID_PARAMETER UsbHeaderFunDescriptor is NULL.
  @retval EFI_NOT_FOUND         The USB Header Functional descriptor was not found.
**/
typedef
EFI_STATUS
(EFIAPI *USB_ETH_MAX_BULK_SIZE)(
    IN  USB_ETHERNET_PROTOCOL       *This,
    OUT UINTN                       *BulkSize
);

/**
  Retrieves the USB Header functional Descriptor.

  @param  This                   A pointer to the USB_ETHERNET_PROTOCOL instance.
  @param  UsbHeaderFunDescriptor A pointer to the caller allocated USB Header Functional Descriptor.

  @retval EFI_SUCCESS           The USB Header Functional descriptor was retrieved successfully.
  @retval EFI_INVALID_PARAMETER UsbHeaderFunDescriptor is NULL.
  @retval EFI_NOT_FOUND         The USB Header Functional descriptor was not found.
**/
typedef
EFI_STATUS
(EFIAPI *USB_HEADER_FUNC_DESCRIPTOR)(
    IN USB_ETHERNET_PROTOCOL        *This,
    OUT USB_HEADER_FUN_DESCRIPTOR   *UsbHeaderFunDescriptor
);

/**
  Retrieves the USB Union functional Descriptor.

  @param  This                   A pointer to the USB_ETHERNET_PROTOCOL instance.
  @param  UsbUnionFunDescriptor  A pointer to the caller allocated USB Union Functional Descriptor.

  @retval EFI_SUCCESS           The USB Union Functional descriptor was retrieved successfully.
  @retval EFI_INVALID_PARAMETER UsbUnionFunDescriptor is NULL.
  @retval EFI_NOT_FOUND         The USB Union Functional descriptor was not found.
**/
typedef
EFI_STATUS
(EFIAPI *USB_UNION_FUNC_DESCRIPTOR)(
    IN USB_ETHERNET_PROTOCOL        *This,
    OUT USB_UNION_FUN_DESCRIPTOR    *UsbUnionFunDescriptor
);

/**
  Retrieves the USB Ethernet functional Descriptor.

  @param  This                   A pointer to the USB_ETHERNET_PROTOCOL instance.
  @param  UsbEthFunDescriptor    A pointer to the caller allocated USB Ethernet Functional Descriptor.

  @retval EFI_SUCCESS           The USB Ethernet Functional descriptor was retrieved successfully.
  @retval EFI_INVALID_PARAMETER UsbEthFunDescriptor is NULL.
  @retval EFI_NOT_FOUND         The USB Ethernet Functional descriptor was not found.
**/
typedef
EFI_STATUS
(EFIAPI *USB_ETHERNET_FUNC_DESCRIPTOR)(
    IN USB_ETHERNET_PROTOCOL        *This,
    OUT USB_ETHERNET_FUN_DESCRIPTOR *UsbEthFunDescriptor
);

/**
  This request sets the Ethernet device multicast filters as specified in the
  sequential list of 48 bit Ethernet multicast addresses.

  @param  This                   A pointer to the USB_ETHERNET_PROTOCOL instance.
  @param  Value                  Number of filters.
  @param  McastAddr              A pointer to the value of the multicast addresses.

  @retval EFI_SUCCESS           The request executed successfully.
  @retval EFI_TIMEOUT           A timeout occurred executing the request.
  @retval EFI_DEVICE_ERROR      The request failed due to a device error.
  @retval EFI_INVALID_PARAMETER One of the parameters has an invalid value.
  @retval EFI_UNSUPPORTED       Not supported.
**/
typedef
EFI_STATUS
(EFIAPI *USB_ETHERNET_SET_ETH_MULTICAST_FILTERS)(
    IN USB_ETHERNET_PROTOCOL    *This,
    IN UINT16                   Value,
    IN VOID                     *McastAddr
);

/**
  This request sets up the specified Ethernet power management pattern filter as
  described in the data structure.

  @param  This                   A pointer to the USB_ETHERNET_PROTOCOL instance.
  @param  Value                  Number of filters.
  @param  Length                 Size of structure.
  @param  PatternFilter          A pointer to the power management pattern filter structure.

  @retval EFI_SUCCESS           The request executed successfully.
  @retval EFI_TIMEOUT           A timeout occurred executing the request.
  @retval EFI_DEVICE_ERROR      The request failed due to a device error.
  @retval EFI_INVALID_PARAMETER One of the parameters has an invalid value.
  @retval EFI_UNSUPPORTED       Not supported.
**/
typedef
EFI_STATUS
(EFIAPI *USB_ETHERNET_SET_ETH_POWER_MANAGE_PATTERN_FILTER)(
    IN USB_ETHERNET_PROTOCOL    *This,
    IN UINT16                   Value,
    IN UINT16                   Length,
    IN VOID                     *PatternFilter
);

/**
  This request retrieves the status of the specified Ethernet power management
  pattern filter from the device.

  @param  This                   A pointer to the USB_ETHERNET_PROTOCOL instance.
  @param  Value                  Number of filters.
  @param  PatternFilter          A pointer to the pattern active boolean.

  @retval EFI_SUCCESS           The request executed successfully.
  @retval EFI_TIMEOUT           A timeout occurred executing the request.
  @retval EFI_DEVICE_ERROR      The request failed due to a device error.
  @retval EFI_INVALID_PARAMETER One of the parameters has an invalid value.
  @retval EFI_UNSUPPORTED       Not supported.
**/
typedef
EFI_STATUS
(EFIAPI *USB_ETHERNET_GET_ETH_POWER_MANAGE_PATTERN_FILTER)(
    IN USB_ETHERNET_PROTOCOL    *This,
    IN UINT16                   Value,
    IN BOOLEAN                  *PatternActive
);

/**
  This request is used to configure device Ethernet packet filter settings.

  @param  This                   A pointer to the USB_ETHERNET_PROTOCOL instance.
  @param  Value                  Packet Filter Bitmap.

  @retval EFI_SUCCESS           The request executed successfully.
  @retval EFI_TIMEOUT           A timeout occurred executing the request.
  @retval EFI_DEVICE_ERROR      The request failed due to a device error.
  @retval EFI_INVALID_PARAMETER One of the parameters has an invalid value.
  @retval EFI_UNSUPPORTED       Not supported.
**/
typedef
EFI_STATUS
(EFIAPI *USB_ETHERNET_SET_ETH_PACKET_FILTER)(
    IN USB_ETHERNET_PROTOCOL    *This,
    IN UINT16                   Value
);

/**
  This request is used to retrieve a statistic based on the feature selector.

  @param  This                   A pointer to the USB_ETHERNET_PROTOCOL instance.
  @param  Value                  Packet Filter Bitmap.
  @param  Statistic              A pointer to the 32 bit unsigned integer.

  @retval EFI_SUCCESS           The request executed successfully.
  @retval EFI_TIMEOUT           A timeout occurred executing the request.
  @retval EFI_DEVICE_ERROR      The request failed due to a device error.
  @retval EFI_INVALID_PARAMETER One of the parameters has an invalid value.
  @retval EFI_UNSUPPORTED       Not supported.
**/
typedef
EFI_STATUS
(EFIAPI *USB_ETHERNET_GET_ETH_STATISTIC)(
    IN USB_ETHERNET_PROTOCOL    *This,
    IN UINT16                   Value,
    IN VOID                     *Statistic
);

typedef struct {
    USB_ETHERNET_UNDI_GET_STATE         UsbEthUndiGetState;
    USB_ETHERNET_UNDI_START             UsbEthUndiStart;
    USB_ETHERNET_UNDI_STOP              UsbEthUndiStop;
    USB_ETHERNET_UNDI_GET_INIT_INFO     UsbEthUndiGetInitInfo;
    USB_ETHERNET_UNDI_GET_CONFIG_INFO   UsbEthUndiGetConfigInfo;
    USB_ETHERNET_UNDI_INITIALIZE        UsbEthUndiInitialize;
    USB_ETHERNET_UNDI_RESET             UsbEthUndiReset;
    USB_ETHERNET_UNDI_SHUTDOWN          UsbEthUndiShutdown;
    USB_ETHERNET_UNDI_INTERRUPT_ENABLE  UsbEthUndiInterruptEnable;
    USB_ETHERNET_UNDI_RECEIVE_FILTER    UsbEthUndiReceiveFilter;
    USB_ETHERNET_UNDI_STATION_ADDRESS   UsbEthUndiStationAddress;
    USB_ETHERNET_UNDI_STATISTICS        UsbEthUndiStatistics;
    USB_ETHERNET_UNDI_MCAST_IPTOMAC     UsbEthUndiMcastIp2Mac;
    USB_ETHERNET_UNDI_NV_DATA           UsbEthUndiNvData;
    USB_ETHERNET_UNDI_GET_STATUS        UsbEthUndiGetStatus;
    USB_ETHERNET_UNDI_FILL_HEADER       UsbEthUndiFillHeader;
    USB_ETHERNET_UNDI_TRANSMIT          UsbEthUndiTransmit;
    USB_ETHERNET_UNDI_RECEIVE           UsbEthUndiReceive;
} USB_ETHERNET_UNDI;

// The USB_ETHERNET_PROTOCOL provides some basic USB Ethernet device relevant
// descriptor and specific requests.
struct _USB_ETHERNET_PROTOCOL{
    USB_ETHERNET_UNDI                                UsbEthUndi;
    // call undi child function use
    USB_ETHERNET_INITIALIZE                          UsbEthInitialize;
    USB_ETHERNET_STATISTICE                          UsbEthStatistice;
    USB_ETHERNET_RECEIVE                             UsbEthReceive;
    USB_ETHERNET_TRANSMIT                            UsbEthTransmit;

    USB_ETHERNET_INTERRUPT                           UsbEthInterrupt;
    USB_GET_ETH_MAC_ADDRESS                          UsbEthMacAddress;
    USB_ETH_MAX_BULK_SIZE                            UsbEthMaxBulkSize;
    USB_HEADER_FUNC_DESCRIPTOR                       UsbHeaderFuncDescriptor;
    USB_UNION_FUNC_DESCRIPTOR                        UsbUnionFuncDescriptor;
    USB_ETHERNET_FUNC_DESCRIPTOR                     UsbEthFuncDescriptor;
    USB_ETHERNET_SET_ETH_MULTICAST_FILTERS           SetUsbEthMcastFilter;
    USB_ETHERNET_SET_ETH_POWER_MANAGE_PATTERN_FILTER SetUsbEthPowerPatternFilter;
    USB_ETHERNET_GET_ETH_POWER_MANAGE_PATTERN_FILTER GetUsbEthPoewrPatternFilter;
    USB_ETHERNET_SET_ETH_PACKET_FILTER               SetUsbEthPacketFilter;
    USB_ETHERNET_GET_ETH_STATISTIC                   GetUsbEthStatistic;
};

#endif
