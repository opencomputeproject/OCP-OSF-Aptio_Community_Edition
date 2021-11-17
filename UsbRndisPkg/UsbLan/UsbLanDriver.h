/** @file
  USB Lan Driver header file

  Copyright (c) 2021, American Megatrends International LLC.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _USB_LAN_DRIVER_H_
#define _USB_LAN_DRIVER_H_

#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/DevicePathLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiUsbLib.h>
#include <Protocol/UsbIo.h>
#include <Protocol/NetworkInterfaceIdentifier.h>
#include <Protocol/UsbEthernetProtocol.h>

#define USB_LAN_DRIVER_VERSION   1
#define USB_LAN_POLLING_INTERVAL 0x10
#define RX_BUFFER_COUNT 32
#define TX_BUFFER_COUNT 32
#define MEMORY_REQUIRE 0

#define UNDI_DEV_SIGNATURE   SIGNATURE_32('u','n','d','i')
#define UNDI_DEV_FROM_THIS(a) CR(a, NIC_DEVICE, NiiProtocol, UNDI_DEV_SIGNATURE)
#define UNDI_DEV_FROM_NIC(a) CR(a, NIC_DEVICE, NicInfo, UNDI_DEV_SIGNATURE)

#pragma pack(1)
typedef struct {
    UINT8 DestAddr[PXE_HWADDR_LEN_ETHER];
    UINT8 SrcAddr[PXE_HWADDR_LEN_ETHER];
    UINT16 Protocol;
} EthernetHeader;
#pragma pack()

typedef struct {
    UINTN                                     Signature;
    EFI_NETWORK_INTERFACE_IDENTIFIER_PROTOCOL NiiProtocol;
    EFI_HANDLE                                DeviceHandle;
    EFI_DEVICE_PATH_PROTOCOL                  *BaseDevPath;
    EFI_DEVICE_PATH_PROTOCOL                  *DevPath;
    NIC_DATA                                  NicInfo;
} NIC_DEVICE;

typedef VOID (*API_FUNC)(PXE_CDB*, NIC_DATA*);

extern PXE_SW_UNDI  *gPxe;
extern NIC_DEVICE   *gLanDeviceList[MAX_USBLAN_INTERFACE];
extern EFI_COMPONENT_NAME2_PROTOCOL   gUsbLanComponentName2;

EFI_STATUS EFIAPI UsbLanSupported(EFI_DRIVER_BINDING_PROTOCOL*, EFI_HANDLE, EFI_DEVICE_PATH_PROTOCOL*);
EFI_STATUS EFIAPI UsbLanDriverStart(EFI_DRIVER_BINDING_PROTOCOL*, EFI_HANDLE, EFI_DEVICE_PATH_PROTOCOL*);
EFI_STATUS EFIAPI UsbLanDriverStop(EFI_DRIVER_BINDING_PROTOCOL*, EFI_HANDLE, UINTN, EFI_HANDLE*);

VOID PxeStructInit(PXE_SW_UNDI*);
VOID UpdateNicNum(NIC_DATA*, PXE_SW_UNDI*);
VOID UndiApiEntry(UINT64);
UINTN MapIt(NIC_DATA*, UINT64, UINT32, UINT32, UINT64);
VOID UnMapIt(NIC_DATA*, UINT64, UINT32, UINT32, UINT64);
VOID UndiGetState(PXE_CDB*, NIC_DATA*);
VOID UndiStart(PXE_CDB*, NIC_DATA*);
VOID UndiStop(PXE_CDB*, NIC_DATA*);
VOID UndiGetInitInfo(PXE_CDB*, NIC_DATA*);
VOID UndiGetConfigInfo(PXE_CDB*, NIC_DATA*);
VOID UndiInitialize(PXE_CDB*, NIC_DATA*);
VOID UndiReset(PXE_CDB*, NIC_DATA*);
VOID UndiShutdown(PXE_CDB*, NIC_DATA*);
VOID UndiInterruptEnable(PXE_CDB*, NIC_DATA*);
VOID UndiReceiveFilter(PXE_CDB*, NIC_DATA*);
VOID UndiStationAddress(PXE_CDB*, NIC_DATA*);
VOID UndiStatistics(PXE_CDB*, NIC_DATA*);
VOID UndiMcastIp2Mac(PXE_CDB*, NIC_DATA*);
VOID UndiNvData(PXE_CDB*, NIC_DATA*);
VOID UndiGetStatus(PXE_CDB*, NIC_DATA*);
VOID UndiFillHeader(PXE_CDB*, NIC_DATA*);
VOID UndiTransmit(PXE_CDB*, NIC_DATA*);
VOID UndiReceive(PXE_CDB*, NIC_DATA*);
UINT16 Initialize(PXE_CDB*, NIC_DATA*);
UINT16 Transmit(PXE_CDB*, NIC_DATA*, UINT64, UINT16);
UINT16 Receive(PXE_CDB*, NIC_DATA*, UINT64, UINT64);
UINT16 Setfilter(NIC_DATA*, UINT16, UINT64, UINT32);
UINT16 Statistics(NIC_DATA*, UINT64, UINT16);

#endif
