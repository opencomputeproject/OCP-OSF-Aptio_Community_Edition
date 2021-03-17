## @file
#  Component description file for the SkyLake SiPkg PEI libraries.
#
# Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
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
# Silicon Init Pei Library
#
 SiPolicyLib|$(PLATFORM_SI_PACKAGE)/Library/PeiSiPolicyLib/PeiSiPolicyLib.inf
 SiConfigBlockLib|$(PLATFORM_SI_PACKAGE)/Library/BaseSiConfigBlockLib/BaseSiConfigBlockLib.inf

#
# Pch
#
 PchPolicyLib|$(PLATFORM_SI_PACKAGE)/Pch/Library/PeiPchPolicyLib/PeiPchPolicyLib.inf
!if gSiPkgTokenSpaceGuid.PcdOcWdtEnable == TRUE
 OcWdtLib|$(PLATFORM_SI_PACKAGE)/Pch/Library/PeiOcWdtLib/PeiOcWdtLib.inf
!else
 OcWdtLib|$(PLATFORM_SI_PACKAGE)/Pch/Library/PeiOcWdtLibNull/PeiOcWdtLibNull.inf
!endif
 ResetSystemLib|$(PLATFORM_SI_PACKAGE)/Pch/Library/PeiResetSystemLib/PeiResetSystemLib.inf
 PchResetLib|$(PLATFORM_SI_PACKAGE)/Pch/Library/PeiPchResetLib/PeiPchResetLib.inf
 SpiLib|$(PLATFORM_SI_PACKAGE)/Pch/Library/PeiSpiLib/PeiSpiLib.inf

#
# Cpu
#
 CpuCommonLib|$(PLATFORM_SI_PACKAGE)/Cpu/LibraryPrivate/PeiDxeSmmCpuCommonLib/PeiDxeSmmCpuCommonLib.inf

#
# Me
#
 PeiMePolicyLib|$(PLATFORM_SI_PACKAGE)/Me/Library/PeiMePolicyLib/PeiMePolicyLib.inf

#
# SA
#

  PeiSaPolicyLib|$(PLATFORM_SI_PACKAGE)/SystemAgent/Library/PeiSaPolicyLib/PeiSaPolicyLib.inf
#
# Cpu
#
 CpuPolicyLib|$(PLATFORM_SI_PACKAGE)/Cpu/Library/PeiCpuPolicyLib/PeiCpuPolicyLib.inf
 CpuPolicyLibPreMem|$(PLATFORM_SI_PACKAGE)/Cpu/Library/PeiCpuPolicyLibPreMem/PeiCpuPolicyLibPreMem.inf

