/** @file
  Header file for SMM Control PEI Library.

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#ifndef _PCH_SMM_CONTROL_LIB_H_
#define _PCH_SMM_CONTROL_LIB_H_

/**
  This function install PEI SMM Control PPI

  @retval EFI_STATUS  Results of the installation of the SMM Control PPI
**/
EFI_STATUS
EFIAPI
PchSmmControlInit (
  VOID
  );

#endif
