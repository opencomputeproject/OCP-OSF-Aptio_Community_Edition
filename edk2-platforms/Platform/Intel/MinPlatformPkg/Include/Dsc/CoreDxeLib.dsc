## @file
#  Platform description.
#
# Copyright (c) 2017 - 2018, Intel Corporation. All rights reserved.<BR>
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
  # Generic EDKII Lib
  #
  
  #
  # DXE phase common
  #
[LibraryClasses.common.DXE_CORE,LibraryClasses.common.DXE_SMM_DRIVER,LibraryClasses.common.SMM_CORE,LibraryClasses.common.DXE_DRIVER,LibraryClasses.common.DXE_RUNTIME_DRIVER,LibraryClasses.common.UEFI_DRIVER,LibraryClasses.common.UEFI_APPLICATION]
  HobLib|MdePkg/Library/DxeHobLib/DxeHobLib.inf
  PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  ReportStatusCodeLib|MdeModulePkg/Library/DxeReportStatusCodeLib/DxeReportStatusCodeLib.inf
  ExtractGuidedSectionLib|MdePkg/Library/DxeExtractGuidedSectionLib/DxeExtractGuidedSectionLib.inf

  HstiLib|MdePkg/Library/DxeHstiLib/DxeHstiLib.inf
  
  LockBoxLib|MdeModulePkg/Library/SmmLockBoxLib/SmmLockBoxDxeLib.inf

  CpuExceptionHandlerLib|UefiCpuPkg/Library/CpuExceptionHandlerLib/DxeCpuExceptionHandlerLib.inf

  MpInitLib|UefiCpuPkg/Library/MpInitLib/DxeMpInitLib.inf
  FrameBufferBltLib|MdeModulePkg/Library/FrameBufferBltLib/FrameBufferBltLib.inf
  TimerLib|PcAtChipsetPkg/Library/AcpiTimerLib/DxeAcpiTimerLib.inf

!if gMinPlatformPkgTokenSpaceGuid.PcdPerformanceEnable == TRUE
  PerformanceLib|MdeModulePkg/Library/DxePerformanceLib/DxePerformanceLib.inf
!endif
  TimerLib|PcAtChipsetPkg/Library/AcpiTimerLib/DxeAcpiTimerLib.inf
  TpmMeasurementLib|SecurityPkg/Library/DxeTpmMeasurementLib/DxeTpmMeasurementLib.inf

  BaseCryptLib|CryptoPkg/Library/BaseCryptLib/BaseCryptLib.inf

  HttpLib|NetworkPkg/Library/DxeHttpLib/DxeHttpLib.inf

  Tpm2DeviceLib|SecurityPkg/Library/Tpm2DeviceLibRouter/Tpm2DeviceLibRouterDxe.inf
  HashLib|SecurityPkg/Library/HashLibBaseCryptoRouter/HashLibBaseCryptoRouterDxe.inf
  Tcg2PhysicalPresenceLib|SecurityPkg/Library/DxeTcg2PhysicalPresenceLib/DxeTcg2PhysicalPresenceLib.inf
  VmgExitLib|UefiCpuPkg/Library/VmgExitLibNull/VmgExitLibNull.inf

[LibraryClasses.common.DXE_CORE]
  HobLib|MdePkg/Library/DxeCoreHobLib/DxeCoreHobLib.inf
  MemoryAllocationLib|MdeModulePkg/Library/DxeCoreMemoryAllocationLib/DxeCoreMemoryAllocationLib.inf
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf

!if gMinPlatformPkgTokenSpaceGuid.PcdPerformanceEnable == TRUE
  PerformanceLib|MdeModulePkg/Library/DxeCorePerformanceLib/DxeCorePerformanceLib.inf
  TimerLib|PcAtChipsetPkg/Library/AcpiTimerLib/DxeAcpiTimerLib.inf
!endif

!if $(TARGET) == DEBUG
  DebugLib|MdePkg/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
!endif

[LibraryClasses.common.DXE_DRIVER]
  Tpm2DeviceLib|SecurityPkg/Library/Tpm2DeviceLibTcg2/Tpm2DeviceLibTcg2.inf
  PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf

[LibraryClasses.common.UEFI_DRIVER]
  PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
#  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf

[LibraryClasses.common.DXE_SMM_DRIVER]
  PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
  SmmServicesTableLib|MdePkg/Library/SmmServicesTableLib/SmmServicesTableLib.inf
  MmServicesTableLib|MdePkg/Library/MmServicesTableLib/MmServicesTableLib.inf
  ReportStatusCodeLib|MdeModulePkg/Library/SmmReportStatusCodeLib/SmmReportStatusCodeLib.inf
  MemoryAllocationLib|MdePkg/Library/SmmMemoryAllocationLib/SmmMemoryAllocationLib.inf
  LockBoxLib|MdeModulePkg/Library/SmmLockBoxLib/SmmLockBoxSmmLib.inf
  SmmMemLib|MdePkg/Library/SmmMemLib/SmmMemLib.inf
  SmmPeriodicSmiLib|MdePkg/Library/SmmPeriodicSmiLib/SmmPeriodicSmiLib.inf

!if gMinPlatformPkgTokenSpaceGuid.PcdPerformanceEnable == TRUE
  PerformanceLib|MdeModulePkg/Library/SmmPerformanceLib/SmmPerformanceLib.inf
  TimerLib|PcAtChipsetPkg/Library/AcpiTimerLib/DxeAcpiTimerLib.inf
!endif

  SmmCpuPlatformHookLib|UefiCpuPkg/Library/SmmCpuPlatformHookLibNull/SmmCpuPlatformHookLibNull.inf
  SmmCpuFeaturesLib|UefiCpuPkg/Library/SmmCpuFeaturesLib/SmmCpuFeaturesLib.inf

  CpuExceptionHandlerLib|UefiCpuPkg/Library/CpuExceptionHandlerLib/SmmCpuExceptionHandlerLib.inf
  Tcg2PhysicalPresenceLib|SecurityPkg/Library/SmmTcg2PhysicalPresenceLib/SmmTcg2PhysicalPresenceLib.inf
  BaseCryptLib|CryptoPkg/Library/BaseCryptLib/SmmCryptLib.inf

[LibraryClasses.common.SMM_CORE]
  PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
  HobLib|MdePkg/Library/DxeHobLib/DxeHobLib.inf
  MemoryAllocationLib|MdeModulePkg/Library/PiSmmCoreMemoryAllocationLib/PiSmmCoreMemoryAllocationLib.inf
  SmmServicesTableLib|MdeModulePkg/Library/PiSmmCoreSmmServicesTableLib/PiSmmCoreSmmServicesTableLib.inf
  ReportStatusCodeLib|MdeModulePkg/Library/SmmReportStatusCodeLib/SmmReportStatusCodeLib.inf
  SmmMemLib|MdePkg/Library/SmmMemLib/SmmMemLib.inf

!if gMinPlatformPkgTokenSpaceGuid.PcdPerformanceEnable == TRUE
  PerformanceLib|MdeModulePkg/Library/SmmCorePerformanceLib/SmmCorePerformanceLib.inf
  TimerLib|PcAtChipsetPkg/Library/AcpiTimerLib/DxeAcpiTimerLib.inf
!endif

!if $(TARGET) == DEBUG
  DebugLib|MdePkg/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
!endif

[LibraryClasses.common.DXE_RUNTIME_DRIVER]
  PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
  ReportStatusCodeLib|MdeModulePkg/Library/RuntimeDxeReportStatusCodeLib/RuntimeDxeReportStatusCodeLib.inf
!if gMinPlatformPkgTokenSpaceGuid.PcdPerformanceEnable == TRUE
  PerformanceLib|MdeModulePkg/Library/DxePerformanceLib/DxePerformanceLib.inf
  TimerLib|PcAtChipsetPkg/Library/AcpiTimerLib/DxeAcpiTimerLib.inf
!endif
  BaseCryptLib|CryptoPkg/Library/BaseCryptLib/RuntimeCryptLib.inf
  VariablePolicyLib|MdeModulePkg/Library/VariablePolicyLib/VariablePolicyLibRuntimeDxe.inf

[LibraryClasses.common.UEFI_DRIVER]
  Tpm2DeviceLib|SecurityPkg/Library/Tpm2DeviceLibTcg2/Tpm2DeviceLibTcg2.inf

[LibraryClasses.common.UEFI_APPLICATION]
  PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
  #PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  ShellLib|ShellPkg/Library/UefiShellLib/UefiShellLib.inf
  FileHandleLib|MdePkg/Library/UefiFileHandleLib/UefiFileHandleLib.inf
  SortLib|MdeModulePkg/Library/UefiSortLib/UefiSortLib.inf

!if gMinPlatformPkgTokenSpaceGuid.PcdPerformanceEnable == TRUE
  PerformanceLib|MdeModulePkg/Library/DxePerformanceLib/DxePerformanceLib.inf
  TimerLib|PcAtChipsetPkg/Library/AcpiTimerLib/DxeAcpiTimerLib.inf
!endif
