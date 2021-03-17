# @file
#  Component description file for the SkyLake SiPkg DXE libraries.
#
# Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
#
# This program and the accompanying materials are licensed and made available under
# the terms and conditions of the BSD License which accompanies this distribution.
# The full text of the license may be found at
# http://opensource.org/licenses/bsd-license.php
#
# THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
# WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
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
