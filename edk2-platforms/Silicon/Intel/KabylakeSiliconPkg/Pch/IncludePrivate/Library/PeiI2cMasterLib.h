/** @file
  PEI I2C Master module

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _PEI_I2C_MASTER_LIB_H_
#define _PEI_I2C_MASTER_LIB_H_

#include <Ppi/I2cMaster.h>
#include <Library/PchSerialIoLib.h>

/*
  Installs I2cMaster PPIs for each I2c controller.

  @param[in] Controller          - SerialIo controller number

  @retval EFI_INVALID_PARAMETER  - wrong Controller number
  @retval EFI_ALREADY_STARTED    - I2cMaster Ppi was already installed on given controller
  @retval EFI_SUCCESS            - I2cMaster Ppi succesfully installed
  @retval any other return value - internal error of InstallPpi function
*/
EFI_STATUS
InstallI2cMasterPpi (
  PCH_SERIAL_IO_CONTROLLER Controller
  );

/*
  Finds I2cMasterPpi instance for a specified controller

  @param[in] Controller - SerialIo controller number

  @retval NULL          - couldn't locate I2cMaster Ppi for given controller
  @retval not-NULL      - pointer to I2cMaster Ppi
*/
EFI_PEI_I2C_MASTER_PPI*
LocateI2cMasterPpi (
  PCH_SERIAL_IO_CONTROLLER Controller
  );

#endif
