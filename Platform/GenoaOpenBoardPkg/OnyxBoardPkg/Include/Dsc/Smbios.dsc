#;*****************************************************************************
#; Copyright (C) 2021-2024 Advanced Micro Devices, Inc. All rights reserved.
#;
#;******************************************************************************
#
## @file
# Smbios Platform description.
#
##

[PcdsFixedAtBuild]
  #****************************************************************************
  # COMMON SMBIOS
  #****************************************************************************
  #
  # IPMI Interface Type
  #
  # 0 - Unknown
  # 1 - KCS
  # 2 - SMIC
  # 3 - BT
  # 4 - SSIF
  gAmdCommonPkgTokenSpaceGuid.PcdIpmiInterfaceType|1

  # SMBIOS Type 4 Processor Information
  gAmdBoardPkgTokenSpaceGuid.PcdAmdSmbiosSerialNumberSocket0|"Default String"
  gAmdBoardPkgTokenSpaceGuid.PcdAmdSmbiosAssetTagSocket0|"Default String"
  gAmdBoardPkgTokenSpaceGuid.PcdAmdSmbiosPartNumberSocket0|"Default String"

[PcdsDynamicDefault]
  #****************************************************************************
  # BASIC SMBIOS
  #****************************************************************************
  #
  # SMBIOS Type 0 BIOS Information Strings
  gSmbiosFeaturePkgTokenSpaceGuid.PcdSmbiosType0StringBiosReleaseDate|"$(RELEASE_DATE)"
  gSmbiosFeaturePkgTokenSpaceGuid.PcdSmbiosType0StringBiosVersion|"$(FIRMWARE_VERSION_STR)"
  gSmbiosFeaturePkgTokenSpaceGuid.PcdSmbiosType0BiosInformation.SystemBiosMajorRelease|0xFF
  gSmbiosFeaturePkgTokenSpaceGuid.PcdSmbiosType0BiosInformation.SystemBiosMinorRelease|0xFF
  gSmbiosFeaturePkgTokenSpaceGuid.PcdSmbiosType0BiosInformation.EmbeddedControllerFirmwareMajorRelease|0xFF
  gSmbiosFeaturePkgTokenSpaceGuid.PcdSmbiosType0BiosInformation.EmbeddedControllerFirmwareMinorRelease|0xFF
  gSmbiosFeaturePkgTokenSpaceGuid.PcdSmbiosType0BiosInformation.ExtendedBiosSize.Size|32
  gSmbiosFeaturePkgTokenSpaceGuid.PcdSmbiosType0BiosInformation.ExtendedBiosSize.Unit|0x00
  gSmbiosFeaturePkgTokenSpaceGuid.PcdSmbiosType0BiosInformation.BiosCharacteristics.PlugAndPlayIsSupported|0
  gSmbiosFeaturePkgTokenSpaceGuid.PcdSmbiosType0BiosInformation.BiosCharacteristics.EDDSpecificationIsSupported|0
  gSmbiosFeaturePkgTokenSpaceGuid.PcdSmbiosType0BiosInformation.BiosCharacteristics.Floppy525_12IsSupported|0
  gSmbiosFeaturePkgTokenSpaceGuid.PcdSmbiosType0BiosInformation.BiosCharacteristics.Floppy35_720IsSupported|0
  gSmbiosFeaturePkgTokenSpaceGuid.PcdSmbiosType0BiosInformation.BiosCharacteristics.Floppy35_288IsSupported|0
  gSmbiosFeaturePkgTokenSpaceGuid.PcdSmbiosType0BiosInformation.BiosCharacteristics.PrintScreenIsSupported|0
  gSmbiosFeaturePkgTokenSpaceGuid.PcdSmbiosType0BiosInformation.BiosCharacteristics.Keyboard8042IsSupported|0
  gSmbiosFeaturePkgTokenSpaceGuid.PcdSmbiosType0BiosInformation.BiosCharacteristics.SerialIsSupported|0
  gSmbiosFeaturePkgTokenSpaceGuid.PcdSmbiosType0BiosInformation.BiosCharacteristics.PrinterIsSupported|0
  gSmbiosFeaturePkgTokenSpaceGuid.PcdSmbiosType0BiosInformation.BiosCharacteristics.CgaMonoIsSupported|0
  gSmbiosFeaturePkgTokenSpaceGuid.PcdSmbiosType0BiosInformation.BIOSCharacteristicsExtensionBytes[0]|0x01
  gSmbiosFeaturePkgTokenSpaceGuid.PcdSmbiosType0BiosInformation.BIOSCharacteristicsExtensionBytes[1]|0x0C
  gSmbiosFeaturePkgTokenSpaceGuid.PcdSmbiosType0StringVendor|"AMD Corporation"

  # SMBIOS Type 1 System Information Strings
  gSmbiosFeaturePkgTokenSpaceGuid.PcdSmbiosType1StringFamily|$(PROCESSOR_NAME)
  gSmbiosFeaturePkgTokenSpaceGuid.PcdSmbiosType1StringProductName|$(BOARD_NAME)
  gSmbiosFeaturePkgTokenSpaceGuid.PcdSmbiosType1SystemInformation.Uuid|{GUID("069F7A75-1155-455F-81E9-2D778481D7EF")}
  gSmbiosFeaturePkgTokenSpaceGuid.PcdSmbiosType1StringManufacturer|"AMD Corporation"
  gSmbiosFeaturePkgTokenSpaceGuid.PcdSmbiosType1StringSerialNumber|"Default String"
  gSmbiosFeaturePkgTokenSpaceGuid.PcdSmbiosType1StringVersion|"Default String"

  # SMBIOS Type 2 Baseboard Information Strings
  gSmbiosFeaturePkgTokenSpaceGuid.PcdSmbiosType2StringManufacturer|"AMD Corporation"
  gSmbiosFeaturePkgTokenSpaceGuid.PcdSmbiosType2StringVersion|"Default String"
  gSmbiosFeaturePkgTokenSpaceGuid.PcdSmbiosType2StringAssetTag|"Default String"
  gSmbiosFeaturePkgTokenSpaceGuid.PcdSmbiosType2StringLocationInChassis|"Default String"
  gSmbiosFeaturePkgTokenSpaceGuid.PcdSmbiosType2StringSerialNumber|"Default String"

  # SMBIOS Type 3 System Enclosure Information Strings
  gSmbiosFeaturePkgTokenSpaceGuid.PcdSmbiosType3StringManufacturer|"AMD Corporation"
  gSmbiosFeaturePkgTokenSpaceGuid.PcdSmbiosType3StringVersion|"Default String"
  gSmbiosFeaturePkgTokenSpaceGuid.PcdSmbiosType3StringAssetTag|"Default String"
  gSmbiosFeaturePkgTokenSpaceGuid.PcdSmbiosType3StringSKUNumber|"Default String"
  gSmbiosFeaturePkgTokenSpaceGuid.PcdSmbiosType3StringSerialNumber|"Default String"

  # SMBIOS Type 11 OEM Strings
  gAmdCommonPkgTokenSpaceGuid.PcdType11OemStringsCount|1
  gAmdCommonPkgTokenSpaceGuid.PcdType11OemStrings|{"Default String"}

  # SMBIOS Type 12 System Configuration Options
  gAmdCommonPkgTokenSpaceGuid.PcdType12SystemCfgOptionsCount|1
  gAmdCommonPkgTokenSpaceGuid.PcdType12SystemCfgOptions|{"Default String"}

[Components.X64]
  MdeModulePkg/Universal/SmbiosDxe/SmbiosDxe.inf
  SystemInformation/SmbiosFeaturePkg/SmbiosBasicDxe/SmbiosBasicDxe.inf
  AmdCommonPkg/Smbios/SmbiosCommonDxe/SmbiosCommonDxe.inf {
    <LibraryClasses>
      PciSegmentLib|MdePkg/Library/PciSegmentLibSegmentInfo/BasePciSegmentLibSegmentInfo.inf
  }
