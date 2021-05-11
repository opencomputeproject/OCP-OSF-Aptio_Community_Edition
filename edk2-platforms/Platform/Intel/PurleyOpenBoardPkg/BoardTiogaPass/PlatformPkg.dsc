### @file
#
# Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
#
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
###

# ***********************************************************************
# *                                                                     *
# *   Copyright (c) 1985 - 2021, American Megatrends International LLC. *
# *                                                                     *
# *      All rights reserved.                                           *
# *                                                                     *
# *      SPDX-License-Identifier: BSD-2-Clause-Patent                   *
# *                                                                     *
# ***********************************************************************

################################################################################
#
# Defines Section - statements that will be processed to create a Makefile.
#
################################################################################
[Defines]
  DEFINE BOARD_NAME            = BoardTiogaPass
  DEFINE BOARD_PKG             = PurleyOpenBoardPkg
  DEFINE SILICON_BIN_PKG       = PurleySiliconBinPkg
  DEFINE RC_PKG                = PurleyRefreshSiliconPkg
  DEFINE SKT_PKG               = PurleyRefreshSiliconPkg
  DEFINE PCH_PKG               = PurleyRefreshSiliconPkg/SouthClusterLbg
  DEFINE DXE_ARCH              = X64
  DEFINE PEI_ARCH              = IA32

  PLATFORM_NAME                       = Purley
  PLATFORM_GUID                       = D7EAF54D-C9B9-4075-89F0-71943DBCFA61
  PLATFORM_VERSION                    = 0.1
  DSC_SPECIFICATION                   = 0x00010005
  OUTPUT_DIRECTORY                    = Build/$(BOARD_PKG)/$(BOARD_NAME)
  SUPPORTED_ARCHITECTURES             = IA32|X64
  BUILD_TARGETS                       = DEBUG|RELEASE
  SKUID_IDENTIFIER                    = DEFAULT

  #
  # Set the global variables
  #
  EDK_GLOBAL UEFI_PREFIX =
  EDK_GLOBAL PI_PERFIX   =

  EDK_GLOBAL BOARD_PKG         = $(BOARD_PKG)
  EDK_GLOBAL SILICON_BIN_PKG   = $(SILICON_BIN_PKG)
  EDK_GLOBAL SKT_PKG           = $(SKT_PKG)
  EDK_GLOBAL PCH_PKG           = $(PCH_PKG)

  FLASH_DEFINITION                    = $(BOARD_PKG)/$(BOARD_NAME)/PlatformPkg.fdf

  FIX_LOAD_TOP_MEMORY_ADDRESS         = 0


################################################################################
#
# SKU Identification section - list of all SKU IDs supported by this
#                              Platform.
#
################################################################################
[SkuIds]
  0|DEFAULT              # The entry: 0|DEFAULT is reserved and always required.

[DefaultStores]
  0|STANDARD             # UEFI Standard default  0|STANDARD is reserved.
  1|MANUFACTURING        # UEFI Manufacturing default 1|MANUFACTURING is reserved.

################################################################################
#
# Library Class section - list of all Library Classes needed by this Platform.
#
################################################################################

[PcdsFeatureFlag]
  #
  # Platform On/Off features are defined here
  #
  !include $(BOARD_PKG)/$(BOARD_NAME)/PlatformPkgConfig.dsc

!include MinPlatformPkg/Include/Dsc/CoreCommonLib.dsc

!include $(RC_PKG)/RcCommonLib.dsc
!include $(SKT_PKG)/SktCommonLib.dsc
!include $(PCH_PKG)/PchCommonLib.dsc

[LibraryClasses.common]
!if gPlatformTokenSpaceGuid.PcdFastBoot == FALSE
  PlatformBootManagerLib|MinPlatformPkg/Bds/Library/DxePlatformBootManagerLib/DxePlatformBootManagerLib.inf
!else
  PlatformBootManagerLib|$(BOARD_PKG)/Override/Platform/Intel/MinPlatformPkg/Bds/Library/DxePlatformBootManagerLib/DxePlatformBootManagerLib.inf
!endif

  ReportFvLib|MinPlatformPkg/PlatformInit/Library/PeiReportFvLib/PeiReportFvLib.inf
  TestPointCheckLib|MinPlatformPkg/Test/Library/TestPointCheckLibNull/TestPointCheckLibNull.inf

  CompressLib|MinPlatformPkg/Library/CompressLib/CompressLib.inf

  PciSegmentInfoLib|MinPlatformPkg/Pci/Library/PciSegmentInfoLibSimple/PciSegmentInfoLibSimple.inf
  AslUpdateLib|MinPlatformPkg/Acpi/Library/DxeAslUpdateLib/DxeAslUpdateLib.inf

  #
  # Board
  #
  SiliconPolicyInitLib|$(BOARD_PKG)/Policy/Library/SiliconPolicyInitLib/SiliconPolicyInitLib.inf
  SiliconPolicyUpdateLib|$(BOARD_PKG)/Policy/Library/SiliconPolicyUpdateLib/SiliconPolicyUpdateLib.inf
  PlatformHookLib|$(BOARD_PKG)/$(BOARD_NAME)/Library/BasePlatformHookLib/BasePlatformHookLib.inf
  BoardInitLib|MinPlatformPkg/PlatformInit/Library/BoardInitLibNull/BoardInitLibNull.inf


!if gAdvancedFeaturePkgTokenSpaceGuid.PcdIpmiEnable == TRUE
  IpmiLib|$(BOARD_PKG)/Features/Ipmi/Library/IpmiLibKcs/IpmiLibKcs.inf
  IpmiCommandLib|AdvancedFeaturePkg/Ipmi/Library/IpmiCommandLib/IpmiCommandLib.inf
  IpmiPlatformHookLib|$(BOARD_PKG)/Features/Ipmi/Library/IpmiPlatformHookLib/IpmiPlatformHookLib.inf
!endif

  OrderedCollectionLib|MdePkg/Library/BaseOrderedCollectionRedBlackTreeLib/BaseOrderedCollectionRedBlackTreeLib.inf
## --------------------------------------------------------------
## End of section, don't put library after CpPcCommonFeature.dsc
## --------------------------------------------------------------

!include MinPlatformPkg/Include/Dsc/CorePeiLib.dsc
!include $(RC_PKG)/RcPeiLib.dsc
!include $(SKT_PKG)/SktPeiLib.dsc
!include $(PCH_PKG)/PchPeiLib.dsc

[LibraryClasses.IA32]
!if $(TARGET) == DEBUG
TestPointCheckLib|MinPlatformPkg/Test/Library/TestPointCheckLibNull/TestPointCheckLibNull.inf
!endif
TestPointLib|MinPlatformPkg/Test/Library/TestPointCheckLibNull/TestPointCheckLibNull.inf

!include MinPlatformPkg/Include/Dsc/CoreDxeLib.dsc
!include $(RC_PKG)/RcDxeLib.dsc
!include $(SKT_PKG)/SktDxeLib.dsc
!include $(PCH_PKG)/PchDxeLib.dsc

[LibraryClasses.X64]
  BoardAcpiTableLib|$(BOARD_PKG)/$(BOARD_NAME)/Library/BoardAcpiLib/DxeBoardAcpiTableLib.inf
!if $(TARGET) == DEBUG
TestPointCheckLib|MinPlatformPkg/Test/Library/TestPointCheckLibNull/TestPointCheckLibNull.inf
!endif
  TestPointLib|MinPlatformPkg/Test/Library/TestPointLib/DxeTestPointLib.inf

[LibraryClasses.X64.DXE_SMM_DRIVER]
!if $(TARGET) == DEBUG
  TestPointCheckLib|MinPlatformPkg/Test/Library/TestPointCheckLib/SmmTestPointCheckLib.inf
!endif
  TestPointLib|MinPlatformPkg/Test/Library/TestPointLib/SmmTestPointLib.inf

!include $(BOARD_PKG)/$(BOARD_NAME)/PlatformPkgPcd.dsc

[Components.IA32]

!include MinPlatformPkg/Include/Dsc/CorePeiInclude.dsc
!include AdvancedFeaturePkg/Include/Dsc/CoreAdvancedPeiInclude.dsc

  $(SILICON_BIN_PKG)/FvTempMemorySilicon/$(TARGET)/FvTempMemorySilicon.inf
  $(SILICON_BIN_PKG)/FvPreMemorySilicon/$(TARGET)/FvPreMemorySilicon.inf
  $(SILICON_BIN_PKG)/FvPostMemorySilicon/$(TARGET)/FvPostMemorySilicon.inf

!if gAdvancedFeaturePkgTokenSpaceGuid.PcdIpmiEnable == TRUE
  AdvancedFeaturePkg/Ipmi/IpmiInit/PeiIpmiInit.inf
  AdvancedFeaturePkg/Ipmi/Frb/FrbPei.inf
!endif

  $(BOARD_PKG)/Policy/SystemBoard/SystemBoardPei.inf

  MinPlatformPkg/PlatformInit/ReportFv/ReportFvPei.inf
  MinPlatformPkg/PlatformInit/PlatformInitPei/PlatformInitPreMem.inf {
    <LibraryClasses>
      BoardInitLib|$(BOARD_PKG)/$(BOARD_NAME)/Library/BoardInitLib/PeiBoardInitPreMemLib.inf
  }
  MinPlatformPkg/PlatformInit/PlatformInitPei/PlatformInitPostMem.inf {
    <LibraryClasses>
      BoardInitLib|$(BOARD_PKG)/$(BOARD_NAME)/Library/BoardInitLib/PeiBoardInitPostMemLib.inf
  }
  MinPlatformPkg/PlatformInit/SiliconPolicyPei/SiliconPolicyPeiPreMem.inf
  MinPlatformPkg/PlatformInit/SiliconPolicyPei/SiliconPolicyPeiPostMem.inf

[Components.X64]

#TiogaPass Override START : Added Board Specific changes in core drivers
 #!include MinPlatformPkg/Include/Dsc/CoreDxeInclude.dsc
 !include CoreDxeInclude.dsc 
#TiogaPass Override END

!include AdvancedFeaturePkg/Include/Dsc/CoreAdvancedDxeInclude.dsc

  $(SILICON_BIN_PKG)/FvLateSilicon/$(TARGET)/FvLateSilicon.inf

!if gAdvancedFeaturePkgTokenSpaceGuid.PcdIpmiEnable == TRUE
  AdvancedFeaturePkg/Ipmi/IpmiInit/DxeIpmiInit.inf
  AdvancedFeaturePkg/Ipmi/Frb/FrbDxe.inf
  AdvancedFeaturePkg/Ipmi/OsWdt/OsWdt.inf
  AdvancedFeaturePkg/Ipmi/SolStatus/SolStatus.inf
  AdvancedFeaturePkg/Ipmi/IpmiFru/IpmiFru.inf
  AdvancedFeaturePkg/Ipmi/BmcElog/BmcElog.inf
  AdvancedFeaturePkg/Ipmi/BmcAcpi/BmcAcpi.inf
!endif

  $(SILICON_BIN_PKG)/Microcode/Microcode.inf

  $(BOARD_PKG)/Policy/IioUdsDataDxe/IioUdsDataDxe.inf
  $(BOARD_PKG)/Policy/PlatformCpuPolicy/PlatformCpuPolicy.inf
  $(BOARD_PKG)/Pci/PciPlatform/PciPlatform.inf
  $(BOARD_PKG)/Acpi/BoardAcpiDxe/BoardAcpiDxe.inf {
    <BuildOptions>
        MSFT:*_*_*_ASLCC_FLAGS = /D PURLEY_FLAG /D PCH_SPT
        GCC:*_*_*_ASLCC_FLAGS  = -D PURLEY_FLAG -D PCH_SPT
  }

# This is for prebuild only. No need to include in final FDF.
  $(BOARD_PKG)/Acpi/BoardAcpiDxe/Dsdt.inf {
    <BuildOptions>
        MSFT:*_*_*_ASLCC_FLAGS = /D PURLEY_FLAG /D PCH_SPT
        GCC:*_*_*_ASLCC_FLAGS  = -D PURLEY_FLAG -D PCH_SPT
  }

  MinPlatformPkg/Flash/SpiFvbService/SpiFvbServiceSmm.inf
  MinPlatformPkg/Acpi/AcpiTables/AcpiPlatform.inf
  MinPlatformPkg/Acpi/AcpiSmm/AcpiSmm.inf {
    <LibraryClasses>
      BoardAcpiEnableLib|$(BOARD_PKG)/$(BOARD_NAME)/Library/BoardAcpiLib/SmmBoardAcpiEnableLib.inf
  }

  MinPlatformPkg/PlatformInit/PlatformInitDxe/PlatformInitDxe.inf
  MinPlatformPkg/PlatformInit/PlatformInitSmm/PlatformInitSmm.inf

!if gAdvancedFeaturePkgTokenSpaceGuid.PcdSmbiosEnable == TRUE
  AdvancedFeaturePkg/Smbios/SmbiosBasicDxe/SmbiosBasicDxe.inf
!endif

  PurleyOpenBoardPkg/Policy/S3NvramSave/S3NvramSave.inf

  #
  # Shell
  #
  ShellPkg/Application/Shell/Shell.inf {
   <PcdsFixedAtBuild>
     gEfiShellPkgTokenSpaceGuid.PcdShellLibAutoInitialize|FALSE
   <LibraryClasses>
     NULL|ShellPkg/Library/UefiShellLevel2CommandsLib/UefiShellLevel2CommandsLib.inf
     NULL|ShellPkg/Library/UefiShellLevel1CommandsLib/UefiShellLevel1CommandsLib.inf
     NULL|ShellPkg/Library/UefiShellLevel3CommandsLib/UefiShellLevel3CommandsLib.inf
     NULL|ShellPkg/Library/UefiShellDriver1CommandsLib/UefiShellDriver1CommandsLib.inf
     NULL|ShellPkg/Library/UefiShellInstall1CommandsLib/UefiShellInstall1CommandsLib.inf
     NULL|ShellPkg/Library/UefiShellDebug1CommandsLib/UefiShellDebug1CommandsLib.inf
     NULL|ShellPkg/Library/UefiShellNetwork1CommandsLib/UefiShellNetwork1CommandsLib.inf
     NULL|ShellPkg/Library/UefiShellNetwork2CommandsLib/UefiShellNetwork2CommandsLib.inf
     ShellCommandLib|ShellPkg/Library/UefiShellCommandLib/UefiShellCommandLib.inf
     HandleParsingLib|ShellPkg/Library/UefiHandleParsingLib/UefiHandleParsingLib.inf
     BcfgCommandLib|ShellPkg/Library/UefiShellBcfgCommandLib/UefiShellBcfgCommandLib.inf
     ShellCEntryLib|ShellPkg/Library/UefiShellCEntryLib/UefiShellCEntryLib.inf
     ShellLib|ShellPkg/Library/UefiShellLib/UefiShellLib.inf
  }

  MdeModulePkg/Bus/Pci/NvmExpressDxe/NvmExpressDxe.inf

!include $(BOARD_PKG)/$(BOARD_NAME)/PlatformPkgBuildOption.dsc
