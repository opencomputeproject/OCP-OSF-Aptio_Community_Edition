# @file
#  Component description file for the SkyLake SiPkg DXE libraries.
#
# Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
#
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

#
# Silicon Init Dxe Library
#

#
# Common
#
!if gSiPkgTokenSpaceGuid.PcdAcpiEnable == TRUE
 AslUpdateLib|$(PLATFORM_SI_PACKAGE)/Library/DxeAslUpdateLib/DxeAslUpdateLib.inf
!else
 AslUpdateLib|$(PLATFORM_SI_PACKAGE)/Library/DxeAslUpdateLibNull/DxeAslUpdateLibNull.inf
!endif

#
# Cpu
#
 CpuCommonLib|$(PLATFORM_SI_PACKAGE)/Cpu/LibraryPrivate/PeiDxeSmmCpuCommonLib/PeiDxeSmmCpuCommonLib.inf

#
# Pch
#
 PchHdaLib|$(PLATFORM_SI_PACKAGE)/Pch/LibraryPrivate/DxePchHdaLib/DxePchHdaLib.inf
 ResetSystemLib|$(PLATFORM_SI_PACKAGE)/Pch/Library/DxeResetSystemLib/DxeResetSystemLib.inf

#
# Me
#

#
# SystemAgent
#
 DxeSaPolicyLib|$(PLATFORM_SI_PACKAGE)/SystemAgent/Library/DxeSaPolicyLib/DxeSaPolicyLib.inf
