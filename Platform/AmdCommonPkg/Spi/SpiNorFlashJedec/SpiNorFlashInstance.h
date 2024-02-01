/*****************************************************************************
 *
 * Copyright (C) 2018-2024 Advanced Micro Devices, Inc. All rights reserved.
 *
 *****************************************************************************/

#ifndef SPI_NOR_FLASH_INSTANCE_H_
#define SPI_NOR_FLASH_INSTANCE_H_

#include <PiDxe.h>
#include <Protocol/SpiNorFlash.h>
#include <Protocol/SpiIo.h>
#include <Spi/SpiNorFlashJedec.h>

#define SPI_NOR_FLASH_SIGNATURE SIGNATURE_32 ('s', 'n', 'f', 'm')

typedef struct {
  UINTN Signature;
  EFI_HANDLE Handle;
  EFI_SPI_NOR_FLASH_PROTOCOL Protocol;
  EFI_SPI_IO_PROTOCOL *SpiIo;
  UINT32 SfdpBasicFlashByteCount;
  SFDB_BASIC_FLASH_PARAMETER *SfdpBasicFlash;
  UINT8 *SpiTransactionWriteBuffer;
  UINT32 SpiTransactionWriteBufferIndex;
} SPI_NOR_FLASH_INSTANCE;

#define SPI_NOR_FLASH_FROM_THIS(a) \
  CR (a, SPI_NOR_FLASH_INSTANCE, Protocol, \
      SPI_NOR_FLASH_SIGNATURE)

#endif // SPI_NOR_FLASH_INSTANCE_H_
