#;*****************************************************************************
#;
#; Copyright (C) 2023 Advanced Micro Devices, Inc. All rights reserved.
#;
#;******************************************************************************

################################################################################
#
# Defines Section - statements that will be processed to create a Makefile.
#
################################################################################
[Defines]
  PLATFORM_GUID                  = 9E09C37D-36C3-4D9E-ACF1-5807A6CA0C08
  PLATFORM_VERSION               = 0.1
  DSC_SPECIFICATION              = 0x00010005

  ##
  # SIL_PLATFORM_NAME would be the board name such as Onyx
  # SOC_SKU would be the SOC such as Genoa
  # SIL_BOARD_DIRECTORY would be such as Onyx-Genoa
  ##
  SIL_BOARD_DIRECTORY            = $(SIL_PLATFORM_NAME)-$(SOC_SKU)
  SUPPORTED_ARCHITECTURES        = IA32|X64
  BUILD_TARGETS                  = DEBUG|RELEASE|NOOPT
  SKUID_IDENTIFIER               = DEFAULT

[Packages]
  AmdOpenSilPkg/opensil-uefi-interface/AmdOpenSilPkg.dec

################################################################################
#
# Library Class section - list of all Library Classes needed by this Package.
#
################################################################################

[LibraryClasses]
  BaseMemoryLib|MdePkg/Library/BaseMemoryLib/BaseMemoryLib.inf
  FabricResourceInitLib|AgesaModulePkg/Library/FabricResourceManagerGenoaLib/FabricResourceInit3Lib.inf
  SilEfiLib|AmdOpenSilPkg/opensil-uefi-interface/SilToUefi/SilEfiPI72.inf
  libAMDxPRF|AmdOpenSilPkg/opensil-uefi-interface/libAMDxPRF.inf
  libAMDxSIM|AmdOpenSilPkg/opensil-uefi-interface/libF19M10xSIM.inf
  libAMDxUSL|AmdOpenSilPkg/opensil-uefi-interface/libAMDxUSL.inf

[LibraryClasses.common.PEIM]
  PeiServicesLib|MdePkg/Library/PeiServicesLib/PeiServicesLib.inf
  PcdLib|MdePkg/Library/PeiPcdLib/PeiPcdLib.inf
  SilPeiInit|AmdOpenSilPkg/opensil-uefi-interface/Platform/SilPei.inf

[LibraryClasses.common.DXE_DRIVER]
  PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
  SilDxeInit|AmdOpenSilPkg/opensil-uefi-interface/Platform/SilDxe.inf

################################################################################
#
# Pcd Section - list of all EDK II PCD Entries defined by this Package
#
################################################################################
[PcdsFixedAtBuild]


###################################################################################################
#
# Components Section - list of the modules and components that will be processed by compilation
#                      tools and the EDK II tools to generate PE32/PE32+/Coff image files.
#
# Note: The EDK II DSC file is not used to specify how compiled binary images get placed
#       into firmware volume images. This section is just a list of modules to compile from
#       source into UEFI-compliant binaries.
#       It is the FDF file that contains information on combining binary files into firmware
#       volume images, whose concept is beyond UEFI and is described in PI specification.
#       Binary modules do not need to be listed in this section, as they should be
#       specified in the FDF file. For example: Shell binary (Shell_Full.efi), FAT binary (Fat.efi),
#       Logo (Logo.bmp), and etc.
#       There may also be modules listed in this section that are not required in the FDF file,
#       When a module listed here is excluded from FDF file, then UEFI-compliant binary will be
#       generated for it, but the binary will not be put into any firmware volume.
#
###################################################################################################
[Components]

[Components.IA32]
  AmdOpenSilPkg/opensil-uefi-interface/Platform/SilPei.inf
  AmdOpenSilPkg/opensil-uefi-interface/Platform/$(SIL_BOARD_DIRECTORY)/Pei/Sil$(SIL_PLATFORM_NAME)Pei.inf

[Components.X64]
  AmdOpenSilPkg/opensil-uefi-interface/Platform/SilDxe.inf
  AmdOpenSilPkg/opensil-uefi-interface/Platform/$(SIL_BOARD_DIRECTORY)/Dxe/Sil$(SIL_PLATFORM_NAME)Dxe.inf

[BuildOptions]
  GCC:*_*_*_CC_FLAGS     = -D OPENSIL
  INTEL:*_*_*_CC_FLAGS   = /D OPENSIL
  MSFT:*_*_*_CC_FLAGS    = /D OPENSIL