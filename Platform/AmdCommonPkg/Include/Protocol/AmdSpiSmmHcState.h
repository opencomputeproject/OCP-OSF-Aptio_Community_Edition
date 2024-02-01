/******************************************************************************
 * Copyright (C) 2018-2024 Advanced Micro Devices, Inc. All rights reserved.
 *
 *
 ***************************************************************************/

#ifndef __AMD_SMM_SPI_HC_STATE_PROTOCOL_H__
#define __AMD_SMM_SPI_HC_STATE_PROTOCOL_H__

typedef struct _SMM_EFI_SPI_HC_STATE_PROTOCOL SMM_EFI_SPI_HC_STATE_PROTOCOL;

/**
  Save/Restore the state of the SPI Host Controller

  This routine is called at TPL_NOTIFY.
  Use a chipset specific method to save the state of the SPI Host controller so
  it can be used without disturbing other transactions.

  @param[in] This           Pointer to an EFI_SPI_HC_PROTOCOL structure.

  @retval EFI_SUCCESS            The State was saved successfully
  @retval DEVICE_ERROR           A Device error occurred while saving state

**/
typedef EFI_STATUS
(EFIAPI *SMM_SPI_HC_STATE) (
  IN CONST SMM_EFI_SPI_HC_STATE_PROTOCOL  *This
  );

/**
  Lock/Unlock the SPI Host Controller

  This routine is called at TPL_NOTIFY.
  Use a chipset specific method to restore the state of the SPI Host controller
  so it can be used without disturbing other transactions.

  @param[in] This           Pointer to an EFI_SPI_HC_PROTOCOL structure.

  @retval EFI_SUCCESS      The clock was set up successfully
  @retval EFI_UNSUPPORTED  The SPI controller was not able to support the
                           frequency requested by ClockHz

**/
typedef EFI_STATUS
(EFIAPI *SMM_SPI_HC_LOCK_UNLOCK) (
  IN CONST SMM_EFI_SPI_HC_STATE_PROTOCOL  *This
  );

/**
  Block/Unblock SPI opcode

  This routine is called at TPL_NOTIFY.
  Use a chipset specific method to restore the state of the SPI Host controller
  so it can be used without disturbing other transactions.

  @param[in] This           Pointer to an EFI_SPI_HC_PROTOCOL structure.

  @retval EFI_SUCCESS      The clock was set up successfully
  @retval EFI_UNSUPPORTED  The SPI controller was not able to support the
                           frequency requested by ClockHz

**/
typedef EFI_STATUS
(EFIAPI *SMM_SPI_HC_BLOCK_UNBLOCK_OPCODE) (
  IN CONST SMM_EFI_SPI_HC_STATE_PROTOCOL  *This,
  IN UINT8 Opcode
  );

/**
  Block/Unblock any SPI opcodes

  This routine is called at TPL_NOTIFY.
  Use a chipset specific method to restore the state of the SPI Host controller
  so it can be used without disturbing other transactions.

  @param[in] This           Pointer to an EFI_SPI_HC_PROTOCOL structure.

  @retval EFI_SUCCESS      The clock was set up successfully
  @retval EFI_UNSUPPORTED  The SPI controller was not able to support the
                           frequency requested by ClockHz

**/
typedef EFI_STATUS
(EFIAPI *SMM_SPI_HC_BLOCK_UNBLOCK_ALL_OPCODES) (
  IN CONST SMM_EFI_SPI_HC_STATE_PROTOCOL  *This
  );

///
/// Support a SPI data transaction between the SPI controller and a SPI chip.
///
struct _SMM_EFI_SPI_HC_STATE_PROTOCOL {
  ///
  /// Save/Restore SPI host controller state.
  ///
  SMM_SPI_HC_STATE SaveState;
  SMM_SPI_HC_STATE RestoreState;

  /// Lock/Unlock SPI host controller registers
  SMM_SPI_HC_LOCK_UNLOCK Lock;
  SMM_SPI_HC_LOCK_UNLOCK Unlock;

  /// Block/Unblock SPI Opcode
  SMM_SPI_HC_BLOCK_UNBLOCK_OPCODE BlockOpcode;
  SMM_SPI_HC_BLOCK_UNBLOCK_OPCODE UnblockOpcode;
  SMM_SPI_HC_BLOCK_UNBLOCK_ALL_OPCODES UnblockAllOpcodes;
};

extern EFI_GUID gAmdSpiHcStateProtocolGuid;

#endif // __AMD_SMM_SPI_HC_STATE_PROTOCOL_H__
