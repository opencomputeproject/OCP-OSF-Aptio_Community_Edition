### @file
#
# Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
#
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
###

[LibraryClasses.common.DXE_DRIVER]
  MmPciLib|$(RC_PKG)/Library/DxeMmPciBaseLib/DxeMmPciBaseLib.inf

[LibraryClasses.common.DXE_RUNTIME_DRIVER]
  MmPciLib|$(RC_PKG)/Library/DxeMmPciBaseLib/DxeMmPciBaseLib.inf

[LibraryClasses.common.DXE_SMM_DRIVER]
  MmPciLib|$(RC_PKG)/Library/DxeMmPciBaseLib/SmmMmPciBaseLib.inf

[LibraryClasses.X64.DXE_SMM_DRIVER]
  MmPciLib|$(RC_PKG)/Library/DxeMmPciBaseLib/SmmMmPciBaseLib.inf

[LibraryClasses.X64.UEFI_APPLICATION]
