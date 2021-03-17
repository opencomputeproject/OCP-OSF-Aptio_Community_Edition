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

[LibraryClasses.common.DXE_DRIVER]
  MmPciLib|$(RC_PKG)/Library/DxeMmPciBaseLib/DxeMmPciBaseLib.inf

[LibraryClasses.common.DXE_RUNTIME_DRIVER]
  MmPciLib|$(RC_PKG)/Library/DxeMmPciBaseLib/DxeMmPciBaseLib.inf

[LibraryClasses.common.DXE_SMM_DRIVER]
  MmPciLib|$(RC_PKG)/Library/DxeMmPciBaseLib/SmmMmPciBaseLib.inf

[LibraryClasses.X64.DXE_SMM_DRIVER]
  MmPciLib|$(RC_PKG)/Library/DxeMmPciBaseLib/SmmMmPciBaseLib.inf

[LibraryClasses.X64.UEFI_APPLICATION]
