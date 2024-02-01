/*****************************************************************************
 *
 * Copyright (C) 2018-2024 Advanced Micro Devices, Inc. All rights reserved.
 *
 *****************************************************************************/

#ifndef AMD_SPI_HC_INSTANCE_H_
#define AMD_SPI_HC_INSTANCE_H_

#include <Library/AmdPspRomArmorLib.h>
#include <Library/DevicePathLib.h>
#include <Uefi/UefiBaseType.h>
#include <Protocol/SpiHc.h>
#include <Protocol/SpiConfiguration.h>
#include <Protocol/HiiConfigAccess.h>
#include "AmdSpiHc.h"
#include "AmdSpiHcNvData.h"

#define SPI_HOST_CONTROLLER_SIGNATURE SIGNATURE_32 ('s', 'h', 'c', 'd')

typedef struct {
  UINTN                           Signature;
  EFI_HANDLE                      Handle;
  EFI_EVENT                       Event;
  BOOLEAN                         PspMailboxSpiMode;
  SPI_COMMUNICATION_BUFFER        SpiCommunicationBuffer;
  VOID                            *Registration;
  EFI_PHYSICAL_ADDRESS            HcAddress;
  EFI_SPI_HC_PROTOCOL             Protocol;

  // Produced protocol
  EFI_HII_HANDLE                  HiiHandle;
  EFI_HII_CONFIG_ACCESS_PROTOCOL  ConfigAccess;
} SPI_HOST_CONTROLLER_INSTANCE;

#define SPI_HOST_CONTROLLER_FROM_THIS(a) \
  CR (a, SPI_HOST_CONTROLLER_INSTANCE, Protocol, \
      SPI_HOST_CONTROLLER_SIGNATURE)

#define SPI_HOST_CONTROLLER_FROM_CONFIG_ACCESS(a) \
  CR (a, SPI_HOST_CONTROLLER_INSTANCE, ConfigAccess, \
      SPI_HOST_CONTROLLER_SIGNATURE)

#pragma pack(1)

///
/// HII specific Vendor Device Path definition.
///
typedef struct {
  VENDOR_DEVICE_PATH             VendorDevicePath;
  EFI_DEVICE_PATH_PROTOCOL       End;
} HII_VENDOR_DEVICE_PATH;

#pragma pack()

extern UINT8  AmdSpiHcFormBin[];
extern UINT8  AmdSpiHcProtocolDxeStrings[];

EFI_STATUS
EFIAPI
AmdSpiHcHiiInstallForms (
  SPI_HOST_CONTROLLER_INSTANCE  *Instance
  );

#endif // AMD_SPI_HC_SMM_PROTOCOL_H_
