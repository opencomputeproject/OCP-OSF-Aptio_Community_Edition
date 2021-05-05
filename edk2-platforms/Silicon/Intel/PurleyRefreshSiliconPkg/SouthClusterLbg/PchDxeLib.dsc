### @file
#
# Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
#
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
###

[LibraryClasses.X64.DXE_RUNTIME_DRIVER]
  ResetSystemLib|$(PCH_PKG)/Library/DxeRuntimeResetSystemLib/DxeRuntimeResetSystemLib.inf

[LibraryClasses.X64.DXE_SMM_DRIVER]
  SpiFlashCommonLib|$(PCH_PKG)/Library/SmmSpiFlashCommonLib/SmmSpiFlashCommonLib.inf
