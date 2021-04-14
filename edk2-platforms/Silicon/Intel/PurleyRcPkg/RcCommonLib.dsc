### @file
#
# Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
#
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
###

[LibraryClasses.common]
  PcieAddrLib|$(RC_PKG)/Library/PcieAddressLib/PcieAddressLib.inf
  SiliconAccessLib|$(RC_PKG)/Library/UsraAccessLib/UsraAccessLib.inf
  CsrToPcieLib|$(RC_PKG)/Library/CsrToPcieLibNull/BaseCsrToPcieLibNull.inf
  PcieAddrLib|$(RC_PKG)/Library/PcieAddressLib/PcieAddressLib.inf
  MmPciLib|$(RC_PKG)/Library/MmPciBaseLib/MmPciBaseLib.inf
