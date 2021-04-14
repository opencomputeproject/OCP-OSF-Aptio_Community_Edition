## @file
#  Clevo N1xxWU board configuration.
#
# Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
#
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

[PcdsFixedAtBuild]
  #
  # Please select BootStage here.
  # Stage 1 - enable debug (system deadloop after debug init)
  # Stage 2 - mem init (system deadloop after mem init)
  # Stage 3 - boot to shell only
  # Stage 4 - boot to OS
  # Stage 5 - boot to OS with security boot enabled
  #
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

  gBoardModuleTokenSpaceGuid.PcdTbtEnable|FALSE
  #
  # More fine granularity control below:
  #
  gBoardModuleTokenSpaceGuid.PcdMultiBoardSupport|TRUE

#
# TRUE is ENABLE. FALSE is DISABLE.
#

#
# BIOS build switches configuration
#
  gSiPkgTokenSpaceGuid.PcdOptimizeCompilerEnable|TRUE

# CPU
  gSiPkgTokenSpaceGuid.PcdSourceDebugEnable|FALSE
  gSiPkgTokenSpaceGuid.PcdTxtEnable|TRUE  #Set to FALSE for GCC Build @todo Convert TXT ASM to NASM
  gSiPkgTokenSpaceGuid.PcdBiosGuardEnable|TRUE

# SA
  gSiPkgTokenSpaceGuid.PcdIgdEnable|TRUE
  gSiPkgTokenSpaceGuid.PcdPegEnable|TRUE
  gSiPkgTokenSpaceGuid.PcdSgEnable|TRUE
  gSiPkgTokenSpaceGuid.PcdSaDmiEnable|TRUE
  gSiPkgTokenSpaceGuid.PcdSkycamEnable|TRUE
  gSiPkgTokenSpaceGuid.PcdGmmEnable|TRUE
  gSiPkgTokenSpaceGuid.PcdSaOcEnable|TRUE
  gSiPkgTokenSpaceGuid.PcdVtdEnable|TRUE
  gSiPkgTokenSpaceGuid.PcdPeiDisplayEnable|TRUE

# ME
  gSiPkgTokenSpaceGuid.PcdAmtEnable|TRUE
  gSiPkgTokenSpaceGuid.PcdAtaEnable|TRUE
  gSiPkgTokenSpaceGuid.PcdPttEnable|TRUE
  gSiPkgTokenSpaceGuid.PcdJhiEnable|TRUE

  gSiPkgTokenSpaceGuid.PcdAcpiEnable|TRUE
  gSiPkgTokenSpaceGuid.PcdBdatEnable|TRUE
  gSiPkgTokenSpaceGuid.PcdBootGuardEnable|TRUE
  gSiPkgTokenSpaceGuid.PcdIntegratedTouchEnable|TRUE
  gSiPkgTokenSpaceGuid.PcdCpuPowerOnConfigEnable|TRUE
  gSiPkgTokenSpaceGuid.PcdSiCsmEnable|FALSE
  gSiPkgTokenSpaceGuid.PcdEvLoaderEnable|FALSE
  gSiPkgTokenSpaceGuid.PcdTraceHubEnable|TRUE
  gSiPkgTokenSpaceGuid.PcdOverclockEnable|TRUE
  gSiPkgTokenSpaceGuid.PcdPpmEnable|TRUE
  gSiPkgTokenSpaceGuid.PcdS3Enable|TRUE
  gSiPkgTokenSpaceGuid.PcdSerialGpioEnable|TRUE
  gSiPkgTokenSpaceGuid.PcdSmbiosEnable|TRUE
  gSiPkgTokenSpaceGuid.PcdSmmVariableEnable|TRUE
  gSiPkgTokenSpaceGuid.PcdSoftwareGuardEnable|TRUE
  gSiPkgTokenSpaceGuid.PcdSsaFlagEnable|FALSE
  gSiPkgTokenSpaceGuid.PcdOcWdtEnable|TRUE
  gSiPkgTokenSpaceGuid.PcdSiCatalogDebugEnable|FALSE

#
# Override some PCDs for specific build requirements.
#
  #
  # Disable USB debug message when Source Level Debug is enabled
  # because they cannot be enabled at the same time.
  #

    gSiPkgTokenSpaceGuid.PcdPttEnable|FALSE
    gSiPkgTokenSpaceGuid.PcdTxtEnable|FALSE
    gSiPkgTokenSpaceGuid.PcdTxtEnable|FALSE

  !if $(TARGET) == DEBUG
    gSiPkgTokenSpaceGuid.PcdOptimizeCompilerEnable|TRUE
  !else
    gSiPkgTokenSpaceGuid.PcdOptimizeCompilerEnable|TRUE
  !endif

  !if $(TARGET) == DEBUG
    gMinPlatformPkgTokenSpaceGuid.PcdSmiHandlerProfileEnable|TRUE
  !else
    gMinPlatformPkgTokenSpaceGuid.PcdSmiHandlerProfileEnable|FALSE
  !endif

    gMinPlatformPkgTokenSpaceGuid.PcdPerformanceEnable|FALSE

