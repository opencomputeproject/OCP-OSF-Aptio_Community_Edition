/** @file
  USB RNDIS header file

  Copyright (c) 2021, American Megatrends International LLC.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _USB_RNDIS_DRIVER_H_
#define _USB_RNDIS_DRIVER_H_

#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/DevicePathLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiUsbLib.h>
#include <Protocol/UsbIo.h>
#include <Protocol/UsbEthernetProtocol.h>

#define USB_CDC_CONTROL_INTERFACE_BASE_CLASS 0x02
#define USB_CDC_CONTROL_INTERFACE_SUB_CLASS  0x02
#define USB_CDC_CONTROL_INTERFACE_PROTOCOL   0xFF

#define USB_CDC_DATA_INTERFACE_BASE_CLASS    0x0A
#define USB_CDC_DATA_INTERFACE_SUB_CLASS     0x00
#define USB_CDC_DATA_INTERFACE_PROTOCOL      0x00

#define USB_RNDIS_INTERFACE_BASE_CLASS       0xEF
#define USB_RNDIS_INTERFACE_SUB_CLASS        0x04
#define USB_RNDIS_INTERFACE_PROTOCOL         0x01


typedef struct _REMOTE_NDIS_MSG_HEADER REMOTE_NDIS_MSG_HEADER;

typedef struct {
    UINT32                      Signature;
    USB_ETHERNET_PROTOCOL       UsbEth;
    EFI_HANDLE                  UsbCdcDataHandle;
    EFI_HANDLE                  UsbRndisHandle;
    EFI_USB_IO_PROTOCOL         *UsbIo;
    EFI_USB_IO_PROTOCOL         *UsbIoCdcData;
    EFI_USB_CONFIG_DESCRIPTOR   *Config;
    UINT8                       NumOfInterface;
    UINT8                       BulkInEndpoint;
    UINT8                       BulkOutEndpoint;
    UINT8                       InterrupEndpoint;
    EFI_MAC_ADDRESS             MacAddress;
    UINT32			            RequestId;
    UINT32                      Medium;
    UINT32                      MaxPacketsPerTransfer;
    UINT32                      MaxTransferSize;
    UINT32                      PacketAlignmentFactor;
    LIST_ENTRY                  ReceivePacketList;
} USB_RNDIS_DEVICE;


#define USB_RNDIS_SIGNATURE  SIGNATURE_32('r', 'n', 'd', 's')
#define USB_RNDIS_DEVICE_FROM_THIS(a) \
                CR (a, USB_RNDIS_DEVICE, UsbEth, USB_RNDIS_SIGNATURE)

EFI_STATUS
EFIAPI
UsbRndisDriverSupported (
    IN EFI_DRIVER_BINDING_PROTOCOL    *This,
    IN EFI_HANDLE                     ControllerHandle,
    IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
);

EFI_STATUS
EFIAPI
UsbRndisDriverStart (
    IN EFI_DRIVER_BINDING_PROTOCOL    *This,
    IN EFI_HANDLE                     ControllerHandle,
    IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
);

EFI_STATUS
EFIAPI
UsbRndisDriverStop (
    IN  EFI_DRIVER_BINDING_PROTOCOL    *This,
    IN  EFI_HANDLE                     ControllerHandle,
    IN  UINTN                          NumberOfChildren,
    IN  EFI_HANDLE                     *ChildHandleBuffer
);

EFI_STATUS
LoadAllDescriptor (
    IN EFI_USB_IO_PROTOCOL             *UsbIo,
    IN OUT EFI_USB_CONFIG_DESCRIPTOR   **ConfigDesc
);

BOOLEAN
NextDescriptor (
    IN EFI_USB_CONFIG_DESCRIPTOR *Desc,
    IN OUT UINTN                 *Offset
);

EFI_STATUS
GetFunctionalDescriptor (
    IN  EFI_USB_CONFIG_DESCRIPTOR   *Config,
    IN  UINT8                       FunDescriptorType,
    OUT VOID                        *DataBuffer
);

VOID
GetEndpoint (
    IN  EFI_USB_IO_PROTOCOL *UsbIo,
    OUT USB_RNDIS_DEVICE    *UsbRndisDevice
);

EFI_STATUS
EFIAPI
UsbRndisInterrupt (
    IN USB_ETHERNET_PROTOCOL        *This,
    IN BOOLEAN                      IsNewTransfer,
    IN UINTN                        PollingInterval,
    IN EFI_USB_DEVICE_REQUEST       *Request
);

EFI_STATUS
EFIAPI
InterruptCallback (
    IN  VOID        *Data,
    IN  UINTN       DataLength,
    IN  VOID        *Context,
    IN  UINT32      Status
);

EFI_STATUS
EFIAPI
GetUsbEthMacAddress (
    IN  USB_ETHERNET_PROTOCOL   *This,
    OUT EFI_MAC_ADDRESS         *MacAddress
);

EFI_STATUS
EFIAPI
UsbEthBulkSize (
    IN  USB_ETHERNET_PROTOCOL   *This,
    OUT UINTN                   *BulkSize
);

EFI_STATUS
EFIAPI
RndisDummyReturn (
    IN PXE_CDB   *Cdb,
    IN NIC_DATA  *Nic
);

EFI_STATUS
EFIAPI
RndisUndiStart (
    IN PXE_CDB   *Cdb,
    IN NIC_DATA  *Nic
);

EFI_STATUS
EFIAPI
RndisUndiStop (
    IN PXE_CDB   *Cdb,
    IN NIC_DATA  *Nic
);

EFI_STATUS
EFIAPI
RndisUndiGetInitInfo (
    IN PXE_CDB   *Cdb,
    IN NIC_DATA  *Nic
);

EFI_STATUS
EFIAPI
RndisUndiGetConfigInfo (
    IN PXE_CDB   *Cdb,
    IN NIC_DATA  *Nic
);

EFI_STATUS
EFIAPI
RndisUndiInitialize (
    IN PXE_CDB   *Cdb,
    IN NIC_DATA  *Nic
);

EFI_STATUS
EFIAPI
RndisUndiTransmit (
    IN PXE_CDB                 *Cdb,
    IN USB_ETHERNET_PROTOCOL   *UsbEthDriver,
    VOID                       *BulkOutData,
    UINTN                      *DataLengh
);

EFI_STATUS
EFIAPI
RndisUndiReceive (
    IN  PXE_CDB                    *Cdb,
    IN  USB_ETHERNET_PROTOCOL      *UsbEthDriver,
    OUT VOID                       *BulkInData,
    OUT UINTN                      *DataLength
);

EFI_STATUS
EFIAPI
RndisUndiReset (
    IN PXE_CDB   *Cdb,
    IN NIC_DATA  *Nic
);

EFI_STATUS
EFIAPI
RndisUndiShutdown (
    IN PXE_CDB   *Cdb,
    IN NIC_DATA  *Nic
);

EFI_STATUS
EFIAPI
RndisUndiReceiveFilter (
    IN PXE_CDB   *Cdb,
    IN NIC_DATA  *Nic
);

EFI_STATUS
EFIAPI
RndisUndiGetStatus (
    IN PXE_CDB   *Cdb,
    IN NIC_DATA  *Nic
);

EFI_STATUS
EFIAPI
GetUsbHeaderFunDescriptor (
    IN  USB_ETHERNET_PROTOCOL       *This,
    OUT USB_HEADER_FUN_DESCRIPTOR   *UsbHeaderFunDescriptor
);

EFI_STATUS
EFIAPI
GetUsbUnionFunDescriptor (
    IN  USB_ETHERNET_PROTOCOL       *This,
    OUT USB_UNION_FUN_DESCRIPTOR    *UsbUnionFunDescriptor
);

EFI_STATUS
EFIAPI
GetUsbRndisFunDescriptor (
    IN  USB_ETHERNET_PROTOCOL       *This,
    OUT USB_ETHERNET_FUN_DESCRIPTOR *UsbEthFunDescriptor
);

EFI_STATUS
EFIAPI
SetUsbRndisMcastFilter (
    IN USB_ETHERNET_PROTOCOL        *This,
    IN UINT16                       Value,
    IN VOID                         *McastAddr
);

EFI_STATUS
EFIAPI
SetUsbRndisPowerFilter (
    IN USB_ETHERNET_PROTOCOL        *This,
    IN UINT16                       Value,
    IN UINT16                       Length,
    IN VOID                         *PatternFilter
);

EFI_STATUS
EFIAPI
GetUsbRndisPowerFilter (
    IN USB_ETHERNET_PROTOCOL    *This,
    IN UINT16                   Value,
    IN BOOLEAN                  *PatternActive
);

EFI_STATUS
EFIAPI
SetUsbRndisPacketFilter (
    IN USB_ETHERNET_PROTOCOL    *This,
    IN UINT16                   Value
);

EFI_STATUS
EFIAPI
GetRndisStatistic (
    IN USB_ETHERNET_PROTOCOL    *This,
    IN UINT16                   Value,
    IN VOID                     *Statistic
);

EFI_STATUS
RndisControlMsg (
    IN  USB_RNDIS_DEVICE            *UsbRndisDevice,
    IN  REMOTE_NDIS_MSG_HEADER		*RndisMsg,
    OUT REMOTE_NDIS_MSG_HEADER      *RndisMsgResponse
);

EFI_STATUS
RndisTransmitDataMsg (
    IN  USB_RNDIS_DEVICE            *UsbRndisDevice,
    IN  REMOTE_NDIS_MSG_HEADER      *RndisMsg,
    UINTN                           *TransferLength
);


EFI_STATUS
RndisReceiveDataMsg (
    IN  USB_RNDIS_DEVICE            *UsbRndisDevice,
    IN  REMOTE_NDIS_MSG_HEADER      *RndisMsg,
    UINTN                           *TransferLength
);

VOID
PrintRndisMsg (
    REMOTE_NDIS_MSG_HEADER      *RndisMsg
);

#define RNDIS_PACKET_MSG                    0x00000001
#define RNDIS_INITIALIZE_MSG                0x00000002
#define RNDIS_INITIALIZE_CMPLT              0x80000002
#define RNDIS_HLT_MSG                       0x00000003
#define RNDIS_QUERY_MSG                     0x00000004
#define RNDIS_QUERY_CMPLT                   0x80000004
#define RNDIS_SET_MSG                       0x00000005
#define RNDIS_SET_CMPLT                     0x80000005
#define RNDIS_RESET_MSG                     0x00000006
#define RNDIS_RESET_CMPLT                   0x80000006
#define RNDIS_INDICATE_STATUS_MSG           0x00000007
#define RNDIS_KEEPALIVE_MSG                 0x00000008
#define RNDIS_KEEPALIVE_CMPLT               0x80000008

#define RNDIS_STATUS_SUCCESS                0x00000000
#define RNDIS_STATUS_FAILURE                0xC0000001
#define RNDIS_STATUS_INVALID_DATA           0xC0010015
#define RNDIS_STATUS_NOT_SUPPORTED          0xC00000BB
#define RNDIS_STATUS_MEDIA_CONNECT          0x4001000B
#define RNDIS_STATUS_MEDIA_DISCONNECT       0x4001000C

#define SEND_ENCAPSULATED_COMMAND           0x00000000
#define GET_ENCAPSULATED_RESPONSE           0x00000001

//
// Ndis Packet Filter Bits (OID_GEN_CURRENT_PACKET_FILTER).
//
#define NDIS_PACKET_TYPE_DIRECTED           0x0001
#define NDIS_PACKET_TYPE_MULTICAST          0x0002
#define NDIS_PACKET_TYPE_ALL_MULTICAST      0x0004
#define NDIS_PACKET_TYPE_BROADCAST          0x0008
#define NDIS_PACKET_TYPE_SOURCE_ROUTING     0x0010
#define NDIS_PACKET_TYPE_PROMISCUOUS        0x0020
#define NDIS_PACKET_TYPE_SMT                0x0040
#define NDIS_PACKET_TYPE_ALL_LOCAL          0x0080
#define NDIS_PACKET_TYPE_MAC_FRAME          0x8000
#define NDIS_PACKET_TYPE_FUNCTIONAL         0x4000
#define NDIS_PACKET_TYPE_ALL_FUNCTIONAL     0x2000
#define NDIS_PACKET_TYPE_GROUP              0x1000

#pragma pack(1)

typedef struct _REMOTE_NDIS_MSG_HEADER{
    UINT32      MessageType;
    UINT32      MessageLength;
} REMOTE_NDIS_MSG_HEADER;

typedef struct {
    UINT32      MessageType;
    UINT32      MessageLength;
    UINT32      RequestID;
    UINT32      MajorVersion;
    UINT32      MinorVersion;
    UINT32      MaxTransferSize;
} REMOTE_NDIS_INITIALIZE_MSG;

typedef struct {
    UINT32      MessageType;
    UINT32      MessageLength;
    UINT32      RequestID;
} REMOTE_NDIS_HALT_MSG;

typedef struct {
    UINT32      MessageType;
    UINT32      MessageLength;
    UINT32      RequestID;
    UINT32      Oid;
    UINT32      InformationBufferLength;
    UINT32      InformationBufferOffset;
    UINT32      Reserved;
} REMOTE_NDIS_QUERY_MSG;

typedef struct {
    UINT32      MessageType;
    UINT32      MessageLength;
    UINT32      RequestID;
    UINT32      Oid;
    UINT32      InformationBufferLength;
    UINT32      InformationBufferOffset;
    UINT32      Reserved;
} REMOTE_NDIS_SET_MSG;

typedef struct {
    UINT32      MessageType;
    UINT32      MessageLength;
    UINT32      Reserved;
} REMOTE_NDIS_RESET_MSG;

typedef struct {
    UINT32      MessageType;
    UINT32      MessageLength;
    UINT32      Status;
    UINT32      StatusBufferLength;
    UINT32      StatusBufferOffset;
} REMOTE_NDIS_INDICATE_STATUS_MSG;

typedef struct {
    UINT32      DiagStatus;
    UINT32      ErrorOffset;
} RNDIS_DIAGNOSTIC_INFO;

typedef struct {
    UINT32      MessageType;
    UINT32      MessageLength;
    UINT32      RequestID;
} REMOTE_NDIS_KEEPALIVE_MSG;

typedef struct {
    UINT32      MessageType;
    UINT32      MessageLength;
    UINT32      RequestID;
    UINT32      Status;
    UINT32      MajorVersion;
    UINT32      MinorVersion;
    UINT32      DeviceFlags;
    UINT32      Medium;
    UINT32      MaxPacketsPerTransfer;
    UINT32      MaxTransferSize;
    UINT32      PacketAlignmentFactor;
    UINT64      Reserved;
} REMOTE_NDIS_INITIALIZE_CMPLT;

typedef struct {
    UINT32      MessageType;
    UINT32      MessageLength;
    UINT32      RequestID;
    UINT32      Status;
    UINT32      InformationBufferLength;
    UINT32      InformationBufferOffset;
} REMOTE_NDIS_QUERY_CMPLT;

typedef struct {
    UINT32      MessageType;
    UINT32      MessageLength;
    UINT32      RequestID;
    UINT32      Status;
} REMOTE_NDIS_SET_CMPLT;

typedef struct {
    UINT32      MessageType;
    UINT32      MessageLength;
    UINT32      Status;
    UINT32      AddressingReset;
} REMOTE_NDIS_RESET_CMPLT;

typedef struct {
    UINT32      MessageType;
    UINT32      MessageLength;
    UINT32      RequestID;
    UINT32      Status;
} REMOTE_NDIS_KEEPALIVE_CMPLT;

typedef struct {
    UINT32      MessageType;
    UINT32      MessageLength;
    UINT32      DataOffset;
    UINT32      DataLength;
    UINT32      OutOfBandDataOffset;
    UINT32      OutOfBandDataLength;
    UINT32      NumOutOfBandDataElements;
    UINT32      PerPacketInfoOffset;
    UINT32      PerPacketInfoLength;
    UINT32      Reserved1;
    UINT32      Reserved2;
} REMOTE_NDIS_PACKET_MSG;

typedef struct {
    UINT32      Size;
    UINT32      Type;
    UINT32      ClassInformationOffset;
} OUT_OF_BAND_DATA_RECORD;

typedef struct {
    UINT32      Size;
    UINT32      Type;
    UINT32      ClassInformationOffset;
} PER_PACKET_INFO_DATA_RECORD;

typedef struct {
    LIST_ENTRY  PacketList;
    UINT8       *OrgBuffer;
    UINTN       RemainingLength;
    //UINT8       PacketStart;    // Variable size data to follow
}PACKET_LIST;

#pragma pack()

#endif


