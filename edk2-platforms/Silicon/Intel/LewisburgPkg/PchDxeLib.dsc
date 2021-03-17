### @file
#
# Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
#
# This program and the accompanying materials are licensed and made available under
# the terms and conditions of the BSD License which accompanies this distribution.
# The full text of the license may be found at
# http://opensource.org/licenses/bsd-license.php
#
# THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
# WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
###

[LibraryClasses.X64.DXE_RUNTIME_DRIVER]
  ResetSystemLib|$(PCH_PKG)/Library/DxeRuntimeResetSystemLib/DxeRuntimeResetSystemLib.inf

[LibraryClasses.X64.DXE_SMM_DRIVER]
  SpiFlashCommonLib|$(PCH_PKG)/Library/SmmSpiFlashCommonLib/SmmSpiFlashCommonLib.inf
