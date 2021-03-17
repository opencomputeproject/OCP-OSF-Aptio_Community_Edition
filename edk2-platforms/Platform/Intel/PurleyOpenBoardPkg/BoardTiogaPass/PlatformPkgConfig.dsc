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

# ***********************************************************************
# *                                                                     *
# *   Copyright (c) 1985 - 2021, American Megatrends International LLC. *
# *                                                                     *
# *      All rights reserved.                                           *
# *                                                                     *
# *      This program and the accompanying materials are licensed and   *
# *      made available under the terms and conditions of the BSD       *
# *      License that accompanies this distribution.  The full text of  *
# *      the license may be found at:                                   *
# *      http://opensource.org/licenses/bsd-license.php.                *
# *                                                                     *
# *      THIS PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN        *
# *      "AS IS" BASIS, WITHOUT WARRANTIES OR REPRESENTATIONS OF        *
# *      ANY KIND, EITHER EXPRESS OR IMPLIED.                           *
# *                                                                     *
# ***********************************************************************

#
# TRUE is ENABLE. FALSE is DISABLE.
#

[PcdsFixedAtBuild]
  gMinPlatformPkgTokenSpaceGuid.PcdBootStage|4

[PcdsFeatureFlag]
  gMinPlatformPkgTokenSpaceGuid.PcdStopAfterDebugInit|FALSE
  gMinPlatformPkgTokenSpaceGuid.PcdStopAfterMemInit|FALSE
  gMinPlatformPkgTokenSpaceGuid.PcdBootToShellOnly|FALSE
  gMinPlatformPkgTokenSpaceGuid.PcdUefiSecureBootEnable|FALSE
  gMinPlatformPkgTokenSpaceGuid.PcdTpm2Enable|FALSE

!if gMinPlatformPkgTokenSpaceGuid.PcdBootStage >= 1
  gMinPlatformPkgTokenSpaceGuid.PcdStopAfterDebugInit|TRUE
!endif

!if gMinPlatformPkgTokenSpaceGuid.PcdBootStage >= 2
  gMinPlatformPkgTokenSpaceGuid.PcdStopAfterDebugInit|FALSE
  gMinPlatformPkgTokenSpaceGuid.PcdStopAfterMemInit|TRUE
!endif

!if gMinPlatformPkgTokenSpaceGuid.PcdBootStage >= 3
  gMinPlatformPkgTokenSpaceGuid.PcdStopAfterMemInit|FALSE
  gMinPlatformPkgTokenSpaceGuid.PcdBootToShellOnly|TRUE
!endif

!if gMinPlatformPkgTokenSpaceGuid.PcdBootStage >= 4
  gMinPlatformPkgTokenSpaceGuid.PcdBootToShellOnly|FALSE
!endif

!if gMinPlatformPkgTokenSpaceGuid.PcdBootStage >= 5
  gMinPlatformPkgTokenSpaceGuid.PcdUefiSecureBootEnable|TRUE
  gMinPlatformPkgTokenSpaceGuid.PcdTpm2Enable|TRUE
!endif
  
  !if $(TARGET) == DEBUG
    gMinPlatformPkgTokenSpaceGuid.PcdSmiHandlerProfileEnable|TRUE
  !else
    gMinPlatformPkgTokenSpaceGuid.PcdSmiHandlerProfileEnable|FALSE
  !endif

  gMinPlatformPkgTokenSpaceGuid.PcdPerformanceEnable|TRUE

  gAdvancedFeaturePkgTokenSpaceGuid.PcdNetworkEnable|TRUE
  gAdvancedFeaturePkgTokenSpaceGuid.PcdSmbiosEnable|TRUE
  gAdvancedFeaturePkgTokenSpaceGuid.PcdIpmiEnable|TRUE

  gPlatformTokenSpaceGuid.PcdFastBoot|FALSE
!if gPlatformTokenSpaceGuid.PcdFastBoot == TRUE
  gAdvancedFeaturePkgTokenSpaceGuid.PcdIpmiEnable|FALSE
  gPlatformTokenSpaceGuid.PcdUpdateConsoleInBds|FALSE
!endif