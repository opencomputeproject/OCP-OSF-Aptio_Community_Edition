/*****************************************************************************
 *
 * Copyright (C) 2018-2023 Advanced Micro Devices, Inc. All rights reserved.
 *
 *****************************************************************************/

#include <Base.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/TimerLib.h>
#include <Protocol/SpiConfiguration.h>
#include <Protocol/SpiIo.h>
#include <Spi/SpiNorFlashJedec.h>
#include "SpiNorFlashInstance.h"

/**
  Fill Write Buffer with Opcode, Address, Dummy Bytes, and Data

  @param[in]    Opcode      - Opcode for transaction
  @param[in]    Address     - SPI Offset Start Address
  @param[in]    WriteBytes  - Number of bytes to write to SPI device
  @param[in]    WriteBuffer - Buffer containing bytes to write to SPI device

  @retval       Size of Data in Buffer
**/
UINT32
FillWriteBuffer (
  IN      SPI_NOR_FLASH_INSTANCE *Instance,
  IN      UINT8     Opcode,
  IN      UINT32    DummyBytes,
  IN      UINT8     AddressBytesSupported,
  IN      BOOLEAN   UseAddress,
  IN      UINT32    Address,
  IN      UINT32    WriteBytes,
  IN      UINT8     *WriteBuffer
  )
{
  UINT32 AddressSize;
  UINT32 BigEndianAddress;
  UINT32 Index;

  // Copy Opcode into Write Buffer
  Instance->SpiTransactionWriteBuffer[0] = Opcode;
  Index = 1;
  if (UseAddress == TRUE) {
    if (AddressBytesSupported == SPI_ADDR_3BYTE_ONLY) {
      AddressSize = 3;
    } else if (AddressBytesSupported == SPI_ADDR_4BYTE_ONLY) {
      AddressSize = 4;
    // EPYC processor will always have SPI HC and SPI part configured for
    // 4-byte addressing if the SPI part is > 16MB.
    } else if (Instance->Protocol.FlashSize <= SIZE_16MB) {
      AddressSize = 3;
    } else {  // SPI part is > 16MB use 4-byte addressing.
      AddressSize = 4;
    }
    BigEndianAddress = SwapBytes32 ((UINT32)Address);
    BigEndianAddress >>= ((sizeof (UINT32) - AddressSize) * 8);
    CopyMem (&Instance->SpiTransactionWriteBuffer[Index],
        &BigEndianAddress, AddressSize);
    Index += AddressSize;
  }
  // Fill DummyBytes
  if (DummyBytes != 0) {
    SetMem (&Instance->SpiTransactionWriteBuffer[Index],
        DummyBytes, 0);
    Index += DummyBytes;
  }
  // Fill Data
  if (WriteBytes > 0) {
    CopyMem (&Instance->SpiTransactionWriteBuffer[Index],
        WriteBuffer, WriteBytes);
    Index += WriteBytes;
  }
  return Index;
}

/**
  Internal Read the flash status register.

  This routine reads the flash part status register.

  @param[in]  Instance       SPI_NOR_FLASH_INSTANCE
                             structure.
  @param[in]  LengthInBytes  Number of status bytes to read.
  @param[out] FlashStatus    Pointer to a buffer to receive the flash status.

  @retval EFI_SUCCESS  The status register was read successfully.

**/
EFI_STATUS
EFIAPI
InternalReadStatus (
  IN                SPI_NOR_FLASH_INSTANCE *Instance,
  IN  UINT32        LengthInBytes,
  OUT UINT8         *FlashStatus
  )
{
  EFI_STATUS Status;
  UINT32 TransactionBufferLength;
  // Read Status register
  TransactionBufferLength = FillWriteBuffer (
                              Instance,
                              SPI_FLASH_RDSR,
                              SPI_FLASH_RDSR_DUMMY,
                              SPI_FLASH_RDSR_ADDR_BYTES,
                              FALSE,
                              0,
                              0,
                              NULL
                              );
  Status = Instance->SpiIo->Transaction (
              Instance->SpiIo,
              SPI_TRANSACTION_WRITE_THEN_READ,
              FALSE,
              0,
              1,
              8,
              TransactionBufferLength,
              Instance->SpiTransactionWriteBuffer,
              1,
              FlashStatus
              );
  ASSERT_EFI_ERROR (Status);
  return Status;
}

/**
  Set Write Enable Latch

  @param[in]  Instance  - SPI NOR instance with all protocols, etc.

  @retval EFI_SUCCESS           SPI Write Enable succeeded
  @retval EFI_DEVICE_ERROR      SPI Flash part did not respond properly
**/
EFI_STATUS
EFIAPI
SetWel (
  IN      SPI_NOR_FLASH_INSTANCE *Instance
  )
{
  EFI_STATUS Status;
  UINT32 TransactionBufferLength;

  TransactionBufferLength = FillWriteBuffer (
                              Instance,
                              SPI_FLASH_WREN,
                              SPI_FLASH_WREN_DUMMY,
                              SPI_FLASH_WREN_ADDR_BYTES,
                              FALSE,
                              0,
                              0,
                              NULL
                              );
  Status = Instance->SpiIo->Transaction (
              Instance->SpiIo,
              SPI_TRANSACTION_WRITE_ONLY,
              FALSE,
              0,
              1,
              8,
              TransactionBufferLength,
              Instance->SpiTransactionWriteBuffer,
              0,
              NULL
              );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

/**
  Check for not device write in progress

  @param[in]  Instance  - SPI NOR instance with all protocols, etc.

  @retval EFI_SUCCESS           Device does not have a write in progress
  @retval EFI_DEVICE_ERROR      SPI Flash part did not respond properly
**/
EFI_STATUS
EFIAPI
WaitNotWip (
  IN      SPI_NOR_FLASH_INSTANCE *Instance
  )
{
  EFI_STATUS Status;
  UINT8 DeviceStatus;
  UINTN RetryCount;
  UINTN DelayMicroseconds;

  DelayMicroseconds = FixedPcdGet32 (PcdAmdSpiDelayMicroseconds);
  RetryCount = FixedPcdGet32 (PcdAmdSpiNorRetryCount);
  if (RetryCount == 0) {
    RetryCount = 1;
  }
  do {
    Status = InternalReadStatus (Instance, 1, &DeviceStatus);
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR (Status)
        || (DeviceStatus & SPI_FLASH_SR_WIP) == SPI_FLASH_SR_NOT_WIP) {
      break;
    }
    MicroSecondDelay (DelayMicroseconds);
    RetryCount--;
  } while (RetryCount > 0);
  if (RetryCount == 0) {
    DEBUG((DEBUG_ERROR, "SpiNorFlash:%a: Timeout error\n", __FUNCTION__));
    Status = EFI_DEVICE_ERROR;
  }
  return Status;
}

/**
  Check for write enable latch set and not device write in progress

  @param[in]  Instance  - SPI NOR instance with all protocols, etc.

  @retval EFI_SUCCESS           Device does not have a write in progress and
                                write enable latch is set
  @retval EFI_DEVICE_ERROR      SPI Flash part did not respond properly
**/
EFI_STATUS
EFIAPI
WaitWelNotWip (
  IN      SPI_NOR_FLASH_INSTANCE *Instance
  )
{
  EFI_STATUS Status;
  UINT8 DeviceStatus;
  UINTN RetryCount;
  UINTN DelayMicroseconds;

  DelayMicroseconds = FixedPcdGet32 (PcdAmdSpiDelayMicroseconds);
  RetryCount = FixedPcdGet32 (PcdAmdSpiNorRetryCount);
  if (RetryCount == 0) {
    RetryCount = 1;
  }
  do {
    Status = InternalReadStatus (Instance, 1, &DeviceStatus);
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR (Status)
        || (DeviceStatus & (SPI_FLASH_SR_WIP | SPI_FLASH_SR_WEL))
            == SPI_FLASH_SR_WEL) {
      break;
    }
    MicroSecondDelay (DelayMicroseconds);
    RetryCount--;
  } while (RetryCount > 0);
  if (RetryCount == 0) {
    DEBUG((DEBUG_ERROR, "SpiNorFlash:%a: Timeout error\n", __FUNCTION__));
    Status = EFI_DEVICE_ERROR;
  }
  return Status;
}

/**
  Check for not write enable latch set and not device write in progress

  @param[in]  Instance  - SPI NOR instance with all protocols, etc.

  @retval EFI_SUCCESS           Device does not have a write in progress and
                                write enable latch is not set
  @retval EFI_DEVICE_ERROR      SPI Flash part did not respond properly
**/
EFI_STATUS
EFIAPI
WaitNotWelNotWip (
  IN      SPI_NOR_FLASH_INSTANCE *Instance
  )
{
  EFI_STATUS Status;
  UINT8 DeviceStatus;
  UINTN RetryCount;
  UINTN DelayMicroseconds;

  DelayMicroseconds = FixedPcdGet32 (PcdAmdSpiDelayMicroseconds);
  RetryCount = FixedPcdGet32 (PcdAmdSpiNorRetryCount);
  if (RetryCount == 0) {
    RetryCount = 1;
  }
  do {
    Status = InternalReadStatus (Instance, 1, &DeviceStatus);
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR (Status)
        || (DeviceStatus & (SPI_FLASH_SR_WIP | SPI_FLASH_SR_WEL))
            == SPI_FLASH_SR_NOT_WIP) {
      break;
    }
    MicroSecondDelay (DelayMicroseconds);
    RetryCount--;
  } while (RetryCount > 0);
  if (RetryCount == 0) {
    DEBUG((DEBUG_ERROR, "SpiNorFlash:%a: Timeout error\n", __FUNCTION__));
    Status = EFI_DEVICE_ERROR;
  }
  return Status;
}

/**
  Read the 3 byte manufacture and device ID from the SPI flash.

  This routine must be called at or below TPL_NOTIFY.
  This routine reads the 3 byte manufacture and device ID from the flash part
  filling the buffer provided.

  @param[in]  This    Pointer to an EFI_SPI_NOR_FLASH_PROTOCOL data structure.
  @param[out] Buffer  Pointer to a 3 byte buffer to receive the manufacture and
                      device ID.



  @retval EFI_SUCCESS            The manufacture and device ID was read
                                 successfully.
  @retval EFI_INVALID_PARAMETER  Buffer is NULL
  @retval EFI_DEVICE_ERROR       Invalid data received from SPI flash part.

**/
EFI_STATUS
EFIAPI
GetFlashId (
  IN  CONST EFI_SPI_NOR_FLASH_PROTOCOL  *This,
  OUT UINT8                             *Buffer
  )
{
  EFI_STATUS Status;
  SPI_NOR_FLASH_INSTANCE *Instance;
  UINT32 TransactionBufferLength;

  if (Buffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Instance = SPI_NOR_FLASH_FROM_THIS (This);

  // Check not WIP
  Status = WaitNotWip (Instance);

  if (!EFI_ERROR (Status)) {
    TransactionBufferLength = FillWriteBuffer (
                                Instance,
                                SPI_FLASH_RDID,
                                SPI_FLASH_RDID_DUMMY,
                                SPI_FLASH_RDID_ADDR_BYTES,
                                FALSE,
                                0,
                                0,
                                NULL
                                );
    Status = Instance->SpiIo->Transaction (
                Instance->SpiIo,
                SPI_TRANSACTION_WRITE_THEN_READ,
                FALSE,
                0,
                1,
                8,
                TransactionBufferLength,
                Instance->SpiTransactionWriteBuffer,
                3,
                Buffer
                );
    ASSERT_EFI_ERROR (Status);
  }

  return Status;
}

/**
  Read data from the SPI flash at not fast speed

  This routine must be called at or below TPL_NOTIFY.
  This routine reads data from the SPI part in the buffer provided.

  @param[in]  This           Pointer to an EFI_SPI_NOR_FLASH_PROTOCOL data
                             structure.
  @param[in]  FlashAddress   Address in the flash to start reading
  @param[in]  LengthInBytes  Read length in bytes
  @param[out] Buffer         Address of a buffer to receive the data

  @retval EFI_SUCCESS            The data was read successfully.
  @retval EFI_INVALID_PARAMETER  Buffer is NULL, or
                                 FlashAddress >= This->FlashSize, or
                                 LengthInBytes > This->FlashSize - FlashAddress

**/
EFI_STATUS
EFIAPI
LfReadData (
  IN  CONST EFI_SPI_NOR_FLASH_PROTOCOL  *This,
  IN  UINT32                            FlashAddress,
  IN  UINT32                            LengthInBytes,
  OUT UINT8                             *Buffer
  )
{
  EFI_STATUS Status;
  SPI_NOR_FLASH_INSTANCE *Instance;
  UINT32  ByteCounter;
  UINT32 CurrentAddress;
  UINT8 *CurrentBuffer;
  UINT32 Length;
  UINT32 TransactionBufferLength;
  UINT32 MaximumTransferBytes;


  Status = EFI_DEVICE_ERROR;
  if (Buffer == NULL ||
      FlashAddress >= This->FlashSize ||
      LengthInBytes > This->FlashSize - FlashAddress) {
    return EFI_INVALID_PARAMETER;
  }

  Instance = SPI_NOR_FLASH_FROM_THIS (This);
  MaximumTransferBytes = Instance->SpiIo->MaximumTransferBytes;

  CurrentBuffer = Buffer;
  Length = 0;
  for (ByteCounter = 0; ByteCounter < LengthInBytes;) {
    CurrentAddress = FlashAddress + ByteCounter;
    CurrentBuffer = Buffer + ByteCounter;
    Length = LengthInBytes - ByteCounter;
    // Length must be MaximumTransferBytes or less
    if (Length > MaximumTransferBytes) {
      Length = MaximumTransferBytes;
    }

    // Check not WIP
    Status = WaitNotWip (Instance);
    if (EFI_ERROR (Status)) {
      break;
    }

    TransactionBufferLength = FillWriteBuffer (
                                Instance,
                                SPI_FLASH_READ,
                                SPI_FLASH_READ_DUMMY,
                                SPI_FLASH_READ_ADDR_BYTES,
                                TRUE,
                                CurrentAddress,
                                0,
                                NULL
                                );
    Status = Instance->SpiIo->Transaction (
                Instance->SpiIo,
                SPI_TRANSACTION_WRITE_THEN_READ,
                FALSE,
                0,
                1,
                8,
                TransactionBufferLength,
                Instance->SpiTransactionWriteBuffer,
                Length,
                CurrentBuffer
                );
    ASSERT_EFI_ERROR (Status);
    ByteCounter += Length;
  }
  return Status;
}

/**
  Read data from the SPI flash.

  This routine must be called at or below TPL_NOTIFY.
  This routine reads data from the SPI part in the buffer provided.

  @param[in]  This           Pointer to an EFI_SPI_NOR_FLASH_PROTOCOL data
                             structure.
  @param[in]  FlashAddress   Address in the flash to start reading
  @param[in]  LengthInBytes  Read length in bytes
  @param[out] Buffer         Address of a buffer to receive the data

  @retval EFI_SUCCESS            The data was read successfully.
  @retval EFI_INVALID_PARAMETER  Buffer is NULL, or
                                 FlashAddress >= This->FlashSize, or
                                 LengthInBytes > This->FlashSize - FlashAddress

**/
EFI_STATUS
EFIAPI
ReadData (
  IN  CONST EFI_SPI_NOR_FLASH_PROTOCOL  *This,
  IN  UINT32                            FlashAddress,
  IN  UINT32                            LengthInBytes,
  OUT UINT8                             *Buffer
  )
{
  EFI_STATUS Status;
  SPI_NOR_FLASH_INSTANCE *Instance;
  UINT32  ByteCounter;
  UINT32 CurrentAddress;
  UINT8 *CurrentBuffer;
  UINT32 Length;
  UINT32 TransactionBufferLength;
  UINT32 MaximumTransferBytes;


  Status = EFI_DEVICE_ERROR;
  if (Buffer == NULL ||
      FlashAddress >= This->FlashSize ||
      LengthInBytes > This->FlashSize - FlashAddress) {
    return EFI_INVALID_PARAMETER;
  }

  Instance = SPI_NOR_FLASH_FROM_THIS (This);
  MaximumTransferBytes = Instance->SpiIo->MaximumTransferBytes;

  CurrentBuffer = Buffer;
  Length = 0;
  for (ByteCounter = 0; ByteCounter < LengthInBytes;) {
    CurrentAddress = FlashAddress + ByteCounter;
    CurrentBuffer = Buffer + ByteCounter;
    Length = LengthInBytes - ByteCounter;
    // Length must be MaximumTransferBytes or less
    if (Length > MaximumTransferBytes) {
      Length = MaximumTransferBytes;
    }

    // Check not WIP
    Status = WaitNotWip (Instance);
    if (EFI_ERROR (Status)) {
      break;
    }

    TransactionBufferLength = FillWriteBuffer (
                                Instance,
                                SPI_FLASH_FAST_READ,
                                SPI_FLASH_FAST_READ_DUMMY,
                                SPI_FLASH_FAST_READ_ADDR_BYTES,
                                TRUE,
                                CurrentAddress,
                                0,
                                NULL
                                );
    Status = Instance->SpiIo->Transaction (
                Instance->SpiIo,
                SPI_TRANSACTION_WRITE_THEN_READ,
                FALSE,
                0,
                1,
                8,
                TransactionBufferLength,
                Instance->SpiTransactionWriteBuffer,
                Length,
                CurrentBuffer
                );
    ASSERT_EFI_ERROR (Status);
    ByteCounter += Length;
  }
  return Status;
}


/**
  Read the flash status register.

  This routine must be called at or below TPL_NOTIFY.
  This routine reads the flash part status register.

  @param[in]  This           Pointer to an EFI_SPI_NOR_FLASH_PROTOCOL data
                             structure.
  @param[in]  LengthInBytes  Number of status bytes to read.
  @param[out] FlashStatus    Pointer to a buffer to receive the flash status.

  @retval EFI_SUCCESS  The status register was read successfully.

**/
EFI_STATUS
EFIAPI
ReadStatus (
  IN  CONST EFI_SPI_NOR_FLASH_PROTOCOL  *This,
  IN  UINT32                            LengthInBytes,
  OUT UINT8                             *FlashStatus
  )
{
  EFI_STATUS Status;
  SPI_NOR_FLASH_INSTANCE *Instance;

  if (LengthInBytes != 1) {
    return EFI_INVALID_PARAMETER;
  }

  Instance = SPI_NOR_FLASH_FROM_THIS (This);

  Status = InternalReadStatus (Instance, LengthInBytes, FlashStatus);

  return Status;
}

/**
  Write the flash status register.

  This routine must be called at or below TPL_N OTIFY.
  This routine writes the flash part status register.

  @param[in] This           Pointer to an EFI_SPI_NOR_FLASH_PROTOCOL data
                            structure.
  @param[in] LengthInBytes  Number of status bytes to write.
  @param[in] FlashStatus    Pointer to a buffer containing the new status.

  @retval EFI_SUCCESS           The status write was successful.
  @retval EFI_OUT_OF_RESOURCES  Failed to allocate the write buffer.

**/
EFI_STATUS
EFIAPI
WriteStatus (
  IN CONST EFI_SPI_NOR_FLASH_PROTOCOL  *This,
  IN UINT32                            LengthInBytes,
  IN UINT8                             *FlashStatus
  )
{
  EFI_STATUS Status;
  SPI_NOR_FLASH_INSTANCE *Instance;
  UINT32 TransactionBufferLength;

  if (LengthInBytes != 1) {
    return EFI_INVALID_PARAMETER;
  }

  Instance = SPI_NOR_FLASH_FROM_THIS (This);

  // Check not WIP
  Status = WaitNotWip (Instance);

  // Set Write Enable
  if (!EFI_ERROR (Status)) {
    Status = SetWel (Instance);
    ASSERT_EFI_ERROR (Status);

    // Check not WIP & WEL enabled
    if (!EFI_ERROR (Status)) {
      Status = WaitWelNotWip (Instance);

      // Write the Status Register
      if (!EFI_ERROR (Status)) {
        TransactionBufferLength = FillWriteBuffer (
                                    Instance,
                                    SPI_FLASH_WRSR,
                                    SPI_FLASH_WRSR_DUMMY,
                                    SPI_FLASH_WRSR_ADDR_BYTES,
                                    FALSE,
                                    0,
                                    0,
                                    NULL
                                    );
        Status = Instance->SpiIo->Transaction (
                    Instance->SpiIo,
                    SPI_TRANSACTION_WRITE_ONLY,
                    FALSE,
                    0,
                    1,
                    8,
                    TransactionBufferLength,
                    Instance->SpiTransactionWriteBuffer,
                    0,
                    NULL
                    );
        ASSERT_EFI_ERROR (Status);
      }
    }
  }

  return Status;
}

/**
  Write data to the SPI flash.

  This routine must be called at or below TPL_NOTIFY.
  This routine breaks up the write operation as necessary to write the data to
  the SPI part.

  @param[in] This           Pointer to an EFI_SPI_NOR_FLASH_PROTOCOL data
                            structure.
  @param[in] FlashAddress   Address in the flash to start writing
  @param[in] LengthInBytes  Write length in bytes
  @param[in] Buffer         Address of a buffer containing the data

  @retval EFI_SUCCESS            The data was written successfully.
  @retval EFI_INVALID_PARAMETER  Buffer is NULL, or
                                 FlashAddress >= This->FlashSize, or
                                 LengthInBytes > This->FlashSize - FlashAddress
  @retval EFI_OUT_OF_RESOURCES   Insufficient memory to copy buffer.

**/
EFI_STATUS
EFIAPI
WriteData (
  IN CONST EFI_SPI_NOR_FLASH_PROTOCOL  *This,
  IN UINT32                            FlashAddress,
  IN UINT32                            LengthInBytes,
  IN UINT8                             *Buffer
  )
{
  EFI_STATUS  Status;
  SPI_NOR_FLASH_INSTANCE *Instance;
  UINT32 ByteCounter;
  UINT32 CurrentAddress;
  UINT32 Length;
  UINT32 BytesUntilBoundary;
  UINT8 *CurrentBuffer;
  UINT32 TransactionBufferLength;
  UINT32 MaximumTransferBytes;
  UINT32 SpiFlashPageSize;

  Status = EFI_DEVICE_ERROR;
  if (Buffer == NULL ||
      LengthInBytes == 0 ||
      FlashAddress >= This->FlashSize ||
      LengthInBytes > This->FlashSize - FlashAddress) {
    return EFI_INVALID_PARAMETER;
  }

  Instance = SPI_NOR_FLASH_FROM_THIS (This);
  MaximumTransferBytes = Instance->SpiIo->MaximumTransferBytes;
  if (Instance->SfdpBasicFlashByteCount >= 11 * 4) { // JESD216C spec DWORD 11
    SpiFlashPageSize = 1 << Instance->SfdpBasicFlash->PageSize;
  } else {
    SpiFlashPageSize = 256;
  }

  CurrentBuffer = Buffer;
  Length = 0;
  for (ByteCounter = 0; ByteCounter < LengthInBytes;) {
    CurrentAddress = FlashAddress + ByteCounter;
    CurrentBuffer = Buffer + ByteCounter;
    Length = LengthInBytes - ByteCounter;
    // Length must be MaximumTransferBytes or less
    if (Length > MaximumTransferBytes) {
      Length = MaximumTransferBytes;
    }
    // Cannot cross SpiFlashPageSize boundary
    BytesUntilBoundary = SpiFlashPageSize
                         - (CurrentAddress % SpiFlashPageSize);
    if (BytesUntilBoundary != 0 && Length > BytesUntilBoundary) {
      Length = BytesUntilBoundary;
    }
    // Check not WIP
    Status = WaitNotWip (Instance);
    if (EFI_ERROR (Status)) {
      break;
    }

    //Set Write Enable
    Status = SetWel (Instance);
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR (Status)) {
      break;
    }

    // Check not WIP & WEL enabled
    Status = WaitWelNotWip (Instance);
    if (EFI_ERROR (Status)) {
      break;
    }

    //  Write Data
    TransactionBufferLength = FillWriteBuffer (
                                Instance,
                                SPI_FLASH_PP,
                                SPI_FLASH_PP_DUMMY,
                                SPI_FLASH_PP_ADDR_BYTES,
                                TRUE,
                                CurrentAddress,
                                Length,
                                CurrentBuffer
                                );
    Status = Instance->SpiIo->Transaction (
                Instance->SpiIo,
                SPI_TRANSACTION_WRITE_ONLY,
                FALSE,
                0,
                1,
                8,
                TransactionBufferLength,
                Instance->SpiTransactionWriteBuffer,
                0,
                NULL
                );
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR (Status)) {
      break;
    }

    // Check not WIP & not WEL
    Status = WaitNotWelNotWip (Instance);
    if (EFI_ERROR (Status)) {
      break;
    }

    ByteCounter += Length;
  }
  return Status;
}

/**
  Efficiently erases one or more 4KiB regions in the SPI flash.

  This routine must be called at or below TPL_NOTIFY.
  This routine uses a combination of 4 KiB and larger blocks to erase the
  specified area.

  @param[in] This          Pointer to an EFI_SPI_NOR_FLASH_PROTOCOL data
                           structure.
  @param[in] FlashAddress  Address within a 4 KiB block to start erasing
  @param[in] BlockCount    Number of 4 KiB blocks to erase

  @retval EFI_SUCCESS            The erase was completed successfully.
  @retval EFI_INVALID_PARAMETER  FlashAddress >= This->FlashSize, or
                                 BlockCount * 4 KiB
                                   > This->FlashSize - FlashAddress

**/
EFI_STATUS
EFIAPI
Erase (
  IN CONST EFI_SPI_NOR_FLASH_PROTOCOL  *This,
  IN UINT32                            FlashAddress,
  IN UINT32                            BlockCount
  )
{
  EFI_STATUS  Status;
  SPI_NOR_FLASH_INSTANCE *Instance;
  UINT8 Opcode;
  UINT32 Dummy;
  UINT8 AddrBytes;
  UINT32 ByteCounter;
  UINT32 EraseLength;
  UINT32 CurrentAddress;
  UINT32 Length;
  UINT32 TransactionBufferLength;

  Status = EFI_DEVICE_ERROR;
  Instance = SPI_NOR_FLASH_FROM_THIS (This);
  EraseLength = BlockCount * SIZE_4KB;
  // Align start Address to 4KB
  CurrentAddress = FlashAddress & ~(SIZE_4KB - 1);
  if (BlockCount == 0 ||
      CurrentAddress >= This->FlashSize ||
      EraseLength > This->FlashSize - CurrentAddress) {
    return EFI_INVALID_PARAMETER;
  }

  Length = 0;
  for (ByteCounter = 0; ByteCounter < EraseLength;) {
    CurrentAddress = FlashAddress + ByteCounter;
    Length = EraseLength - ByteCounter;

    // Calculate largest erase size for this pass
    if (Length == This->FlashSize) {
      Opcode = SPI_FLASH_CE;
      Dummy = SPI_FLASH_CE_DUMMY;
      AddrBytes = SPI_FLASH_CE_ADDR_BYTES;
    } else if ((CurrentAddress % SIZE_64KB) == 0 &&
               Length >= SIZE_64KB) {
      Opcode = SPI_FLASH_BE;
      Dummy = SPI_FLASH_BE_DUMMY;
      AddrBytes = SPI_FLASH_BE_ADDR_BYTES;
      Length = SIZE_64KB;
    } else if ((CurrentAddress % SIZE_32KB) == 0 &&
               Length >= SIZE_32KB) {
      Opcode = SPI_FLASH_BE32K;
      Dummy = SPI_FLASH_BE32K_DUMMY;
      AddrBytes = SPI_FLASH_BE32K_ADDR_BYTES;
      Length = SIZE_32KB;
    } else {
      Opcode = SPI_FLASH_SE;
      Dummy = SPI_FLASH_SE_DUMMY;
      AddrBytes = SPI_FLASH_SE_ADDR_BYTES;
      Length = SIZE_4KB;
    }

    // Check not WIP
    Status = WaitNotWip (Instance);
    if (EFI_ERROR (Status)) {
      break;
    }

    // Set Write Enable
    Status = SetWel (Instance);
    if (EFI_ERROR (Status)) {
      break;
    }

    // Check not WIP & WEL enabled
    Status = WaitWelNotWip (Instance);
    if (EFI_ERROR (Status)) {
      break;
    }

    // Erase Block
    TransactionBufferLength = FillWriteBuffer (
                                Instance,
                                Opcode,
                                Dummy,
                                AddrBytes,
                                TRUE,
                                CurrentAddress,
                                0,
                                NULL
                                );
    Status = Instance->SpiIo->Transaction (
                Instance->SpiIo,
                SPI_TRANSACTION_WRITE_ONLY,
                FALSE,
                0,
                1,
                8,
                TransactionBufferLength,
                Instance->SpiTransactionWriteBuffer,
                0,
                NULL
                );
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR (Status)) {
      break;
    }

    // Check not WIP & not WEL
    Status = WaitNotWelNotWip (Instance);
    if (EFI_ERROR (Status)) {
      break;
    }

    ByteCounter += Length;
  }
  return Status;
}
