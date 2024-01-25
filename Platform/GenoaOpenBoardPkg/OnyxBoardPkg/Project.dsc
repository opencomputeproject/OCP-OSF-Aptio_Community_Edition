#;*****************************************************************************
#; Copyright (C) 2019-2023 Advanced Micro Devices, Inc. All rights reserved.
#;
#;******************************************************************************

# *****************************************************************************
# Defines passed into build
# RELEASE_DATE
# FIRMWARE_REVISION_NUM
# FIRMWARE_VERSION_STR
# PLATFORM_CRB
# AMD_PROCESSOR
# CBS_INCLUDE
# INTERNAL_IDS
# SIMNOW_SUPPORT
# EMULATION
# *****************************************************************************

[Defines]
  PROCESSOR_NAME                 = Genoa
  PROCESSOR_PATH                 = $(PROCESSOR_NAME)OpenBoardPkg
  BOARD_NAME                     = Onyx
  PLATFORM_NAME                  = $(BOARD_NAME)BoardPkg
  PLATFORM_GUID                  = 0b5350f0-7076-11eb-9439-0242ac130002
  PLATFORM_VERSION               = 0.01
  DSC_SPECIFICATION              = 0x00010005
  OUTPUT_DIRECTORY               = Build/$(PLATFORM_NAME)_$(PROCESSOR_NAME)
!ifdef $(INTERNAL_IDS)
  OUTPUT_DIRECTORY               = $(OUTPUT_DIRECTORY)_INTERNAL
!else
  OUTPUT_DIRECTORY               = $(OUTPUT_DIRECTORY)_EXTERNAL
!endif
  SUPPORTED_ARCHITECTURES        = IA32|X64
  BUILD_TARGETS                  = DEBUG|RELEASE|NOOPT
  SKUID_IDENTIFIER               = DEFAULT
  FLASH_DEFINITION               = $(PLATFORM_NAME)/Project.fdf

  DEFINE  PEI_ARCH               = IA32
  DEFINE  DXE_ARCH               = X64
  PREBUILD = "python Platform/PlatformTools/support/prepostbuild_launcher.py prebuild"
  POSTBUILD = "python Platform/PlatformTools/support/prepostbuild_launcher.py postbuild"

  #
  # Platform On/Off features are defined here
  #
  DEFINE SOURCE_DEBUG_ENABLE   = FALSE
  DEFINE DEBUG_DISPATCH_ENABLE = FALSE
  DEFINE DISABLE_SMT           = FALSE

  #
  # Unit Test Enable/Disable features are defined here
  #
  DEFINE UNIT_TEST_AMD_AML_ENABLE   = FALSE

  # Non-runtime UEFI output
  DEFINE LOGGING_ENABLE        = TRUE
  # SMM and Dxe runtime debug message control
  DEFINE RUNTIME_LOGGING_ENABLE = TRUE

  DEFINE USE_1GB_PAGE_TABLES   = TRUE
  # EDK2 components are starting to use PLATFORMX64_ENABLE in their include
  # DSC/FDF files
  DEFINE PLATFORMX64_ENABLE    = TRUE

  # Disable IPV6 and HttpBoot
  DEFINE NETWORK_IP6_ENABLE       = FALSE
  DEFINE NETWORK_HTTP_ENABLE      = FALSE
  DEFINE NETWORK_HTTP_BOOT_ENABLE = FALSE

  # SERIAL_PORT Options:
  #   NONE
  #   FCH_MMIO    UART0, MMIO
  #   FCH_IO      UART0, 0x3F8
  #   BMC_SOL     UART1, MMIO
  #   BMC_SOL_IO  UART1, 0x2F8
  #   BMC_ESPI    eSPI0, 0x3F8
  #     Note - BMC_ESPI requires the following settings:
  #            1. APCB_TOKEN_UID_FCH_CONSOLE_OUT_ENABLE_VALUE = 1
  #            2. APCB_TOKEN_UID_FCH_CONSOLE_OUT_SERIAL_PORT_VALUE = 0
  #            3. BMC configured to use eSPI virtual UART 0x3F8
  DEFINE SERIAL_PORT            = "BMC_SOL_IO"

  #
  # Simnow Options
  #
  DEFINE SIMNOW_PORT80_DEBUG   = FALSE

  DEFINE USB_SUPPORT           = TRUE
  DEFINE SATA_SUPPORT          = TRUE
  DEFINE SLINK_SUPPORT         = FALSE

  #
  # Set Serial Port Base Address
  #
  !if $(SERIAL_PORT) == "BMC_SOL"
    DEFINE SERIAL_REGISTER_BASE = 0xFEDCA000
  !elseif $(SERIAL_PORT) == "BMC_SOL_IO"
    DEFINE SERIAL_REGISTER_BASE = 0x3F8
  !elseif $(SERIAL_PORT) == "FCH_MMIO"
    DEFINE SERIAL_REGISTER_BASE = 0xFEDC9000
  !elseif $(SERIAL_PORT) == "FCH_IO"
    DEFINE SERIAL_REGISTER_BASE = 0x3F8
  !elseif $(SERIAL_PORT) == "BMC_ESPI"
    DEFINE SERIAL_REGISTER_BASE = 0x3F8
  !else
    DEFINE SERIAL_REGISTER_BASE = 0xFEDC9000
  !endif

#-----------------------------------------------------------
#  End of [Defines] section
#-----------------------------------------------------------

  !include $(PLATFORM_NAME)/Include/Dsc/Smbios.dsc

[Packages]
  MinPlatformPkg/MinPlatformPkg.dec
  Network/NetworkFeaturePkg/NetworkFeaturePkg.dec

[PcdsFixedAtBuild]
  ######################################
  # Key Boot Stage
  ######################################
  #
  # Please select the Boot Stage here.
  # Stage 1 - enable debug (system deadloop after debug init)
  # Stage 2 - mem init (system deadloop after mem init)
  # Stage 3 - boot to shell only
  # Stage 4 - boot to OS
  # Stage 5 - boot to OS with security boot enabled
  # Stage 6 - boot with advanced features enabled
  #
  gMinPlatformPkgTokenSpaceGuid.PcdBootStage|4

[PcdsFeatureFlag]
  ######################################
  # Platform Configuration
  ######################################
  gMinPlatformPkgTokenSpaceGuid.PcdStopAfterDebugInit|FALSE
  gMinPlatformPkgTokenSpaceGuid.PcdStopAfterMemInit|FALSE
  gMinPlatformPkgTokenSpaceGuid.PcdSmiHandlerProfileEnable|FALSE
  gMinPlatformPkgTokenSpaceGuid.PcdUefiSecureBootEnable|FALSE
  gMinPlatformPkgTokenSpaceGuid.PcdPerformanceEnable|FALSE
  gMinPlatformPkgTokenSpaceGuid.PcdTpm2Enable|FALSE
  gMinPlatformPkgTokenSpaceGuid.PcdBootToShellOnly|FALSE
  gMinPlatformPkgTokenSpaceGuid.PcdSerialTerminalEnable|TRUE
  gEfiMdeModulePkgTokenSpaceGuid.PcdPciBusHotplugDeviceSupport|FALSE
  gNetworkFeaturePkgTokenSpaceGuid.PcdNetworkFeatureEnable|FALSE

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
    # gMinPlatformPkgTokenSpaceGuid.PcdUefiSecureBootEnable|TRUE
    gMinPlatformPkgTokenSpaceGuid.PcdTpm2Enable|TRUE
    gNetworkFeaturePkgTokenSpaceGuid.PcdNetworkFeatureEnable|TRUE
    gEfiMdeModulePkgTokenSpaceGuid.PcdPciBusHotplugDeviceSupport|TRUE
  !endif

#
# Network Advanced Features
#
!if gNetworkFeaturePkgTokenSpaceGuid.PcdNetworkFeatureEnable == TRUE
  !include Network/NetworkFeaturePkg/Include/NetworkFeature.dsc
!endif

[PcdsFeatureFlag]
  gEfiMdeModulePkgTokenSpaceGuid.PcdPciDegradeResourceForOptionRom|FALSE
  gEfiMdeModulePkgTokenSpaceGuid.PcdDxeIplSwitchToLongMode|TRUE
  gEfiMdePkgTokenSpaceGuid.PcdDriverDiagnostics2Disable|TRUE
  gEfiMdePkgTokenSpaceGuid.PcdComponentName2Disable|TRUE
  gEfiMdeModulePkgTokenSpaceGuid.PcdConOutGopSupport|TRUE
  gEfiMdeModulePkgTokenSpaceGuid.PcdConOutUgaSupport|FALSE
  gEfiMdeModulePkgTokenSpaceGuid.PcdFirmwarePerformanceDataTableS3Support|FALSE

  #
  # SMM
  #
  !if gMinPlatformPkgTokenSpaceGuid.PcdBootToShellOnly == FALSE
    gUefiCpuPkgTokenSpaceGuid.PcdCpuSmmStackGuard|FALSE
    gUefiCpuPkgTokenSpaceGuid.PcdCpuSmmEnableBspElection|FALSE
    gUefiCpuPkgTokenSpaceGuid.PcdCpuSmmFeatureControlMsrLock|FALSE
  !else
    gAmdCommonPkgTokenSpaceGuid.PcdAmdSpiWriteDisable|FALSE
  !endif

  # Uefi Cpu Package
  gUefiCpuPkgTokenSpaceGuid.PcdCpuSmmBlockStartupThisAp|TRUE

  #ACPI
  gEfiMdeModulePkgTokenSpaceGuid.PcdInstallAcpiSdtProtocol|TRUE

[PcdsFixedAtBuild.IA32]
  #
  # Temporary DRAM space for SEC->PEI transition (256KB)
  # AMD_ENABLE_UEFI_STACK (Flat32.asm) divides: 1/2 Heap + 1/2 Stack
  #
  gAmdCommonPkgTokenSpaceGuid.PcdTempRamBase|0x00100000
  gAmdCommonPkgTokenSpaceGuid.PcdTempRamSize|0x00100000

  #
  # Temporary stack for PEI. 0 means half of PcdTempRamSize.
  #  Set by SecStartup() in UefiCpuPkg/SecCore/SecMain.c,
  #  and aligned with AMD_ENABLE_UEFI_STACK.
  gUefiCpuPkgTokenSpaceGuid.PcdPeiTemporaryRamStackSize|0
  gEfiMdeModulePkgTokenSpaceGuid.PcdPeiCoreMaxPeiStackSize|0x80000

[PcdsFixedAtBuild]
  ## Indicate the TPM2 ACPI table revision. Rev 4 has been defined since TCG ACPI Specification Rev 00.37.<BR><BR>
  # To support configuring from setup page, this PCD can be DynamicHii type and map to a setup option.<BR>
  # For example, map to TCG2_VERSION.Tpm2AcpiTableRev to be configured by Tcg2ConfigDxe driver.<BR>
  # gEfiSecurityPkgTokenSpaceGuid.PcdTpm2AcpiTableRev|L"TCG2_VERSION"|gTcg2ConfigFormSetGuid|0x8|3|NV,BS<BR>
  # @Prompt Revision of TPM2 ACPI table.
  gEfiSecurityPkgTokenSpaceGuid.PcdTpm2AcpiTableRev|4

  ## This PCD defines initial setting of TCG2 Persistent Firmware Management Flags
  # PCD can be configured for different settings in different scenarios
  # This PCD follows UEFI TCG2 library definition bit of the BIOS TPM/Storage Management Flags<BR>
  #    BIT0  -  Reserved <BR>
  #    BIT1  -  TCG2_BIOS_TPM_MANAGEMENT_FLAG_PP_REQUIRED_FOR_CLEAR <BR>
  #    BIT2  -  Reserved <BR>
  #    BIT3  -  TCG2_LIB_PP_FLAG_RESET_TRACK <BR>
  #    BIT4  -  TCG2_BIOS_TPM_MANAGEMENT_FLAG_PP_REQUIRED_FOR_TURN_ON <BR>
  #    BIT5  -  TCG2_BIOS_TPM_MANAGEMENT_FLAG_PP_REQUIRED_FOR_TURN_OFF <BR>
  #    BIT6  -  TCG2_BIOS_TPM_MANAGEMENT_FLAG_PP_REQUIRED_FOR_CHANGE_EPS <BR>
  #    BIT7  -  TCG2_BIOS_TPM_MANAGEMENT_FLAG_PP_REQUIRED_FOR_CHANGE_PCRS <BR>
  #    BIT16 -  TCG2_BIOS_STORAGE_MANAGEMENT_FLAG_PP_REQUIRED_FOR_ENABLE_BLOCK_SID <BR>
  #    BIT17 -  TCG2_BIOS_STORAGE_MANAGEMENT_FLAG_PP_REQUIRED_FOR_DISABLE_BLOCK_SID <BR>
  #    BIT18 -  TCG2_BIOS_STORAGE_MANAGEMENT_FLAG_ENABLE_BLOCK_SID <BR>
  # @Prompt Initial setting of TCG2 Persistent Firmware Management Flags
  gEfiSecurityPkgTokenSpaceGuid.PcdTcg2PhysicalPresenceFlags|0x700E0
  gEfiMdeModulePkgTokenSpaceGuid.PcdVpdBaseAddress|0x0

  gEfiMdeModulePkgTokenSpaceGuid.PcdSrIovSupport|FALSE
  gEfiMdeModulePkgTokenSpaceGuid.PcdAriSupport|FALSE

  # Default Value of PlatformLangCodes Variable.
  gEfiMdePkgTokenSpaceGuid.PcdUefiVariableDefaultPlatformLangCodes|"en-US"

  ## The mask is used to control ReportStatusCodeLib behavior.
  #  BIT0 - Enable Progress Code.
  #  BIT1 - Enable Error Code.
  #  BIT2 - Enable Debug Code.
  !ifdef $(INTERNAL_IDS)
    gEfiMdePkgTokenSpaceGuid.PcdReportStatusCodePropertyMask|0x07
  !else
    gEfiMdePkgTokenSpaceGuid.PcdReportStatusCodePropertyMask|0x00
  !endif
  #
  # Debug Masks
  #
  # //
  # // Declare bits for PcdDebugPropertyMask
  # //
  # DEBUG_PROPERTY_DEBUG_ASSERT_ENABLED       0x01
  # DEBUG_PROPERTY_DEBUG_PRINT_ENABLED        0x02
  # DEBUG_PROPERTY_DEBUG_CODE_ENABLED         0x04
  # DEBUG_PROPERTY_CLEAR_MEMORY_ENABLED       0x08
  # DEBUG_PROPERTY_ASSERT_BREAKPOINT_ENABLED  0x10
  # DEBUG_PROPERTY_ASSERT_DEADLOOP_ENABLED    0x20

  # //
  # // Declare bits for PcdFixedDebugPrintErrorLevel and the ErrorLevel parameter of DebugPrint()
  # //
  # DEBUG_INIT      0x00000001  // Initialization
  # DEBUG_WARN      0x00000002  // Warnings
  # DEBUG_LOAD      0x00000004  // Load events
  # DEBUG_FS        0x00000008  // EFI File system
  # DEBUG_POOL      0x00000010  // Alloc & Free's
  # DEBUG_PAGE      0x00000020  // Alloc & Free's
  # DEBUG_INFO      0x00000040  // Informational debug messages
  # DEBUG_DISPATCH  0x00000080  // PEI/DXE/SMM Dispatchers
  # DEBUG_VARIABLE  0x00000100  // Variable
  # DEBUG_BM        0x00000400  // Boot Manager
  # DEBUG_BLKIO     0x00001000  // BlkIo Driver
  # DEBUG_NET       0x00004000  // SNI Driver
  # DEBUG_UNDI      0x00010000  // UNDI Driver
  # DEBUG_LOADFILE  0x00020000  // UNDI Driver
  # DEBUG_EVENT     0x00080000  // Event messages
  # DEBUG_GCD       0x00100000  // Global Coherency Database changes
  # DEBUG_CACHE     0x00200000  // Memory range cachability changes
  # DEBUG_VERBOSE   0x00400000  // Detailed debug messages that may significantly impact boot performance
  # DEBUG_ERROR     0x80000000  // Error

  gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x27
  !if $(DEBUG_DISPATCH_ENABLE)
    gEfiMdePkgTokenSpaceGuid.PcdFixedDebugPrintErrorLevel|0x800000CF
  !else
    gEfiMdePkgTokenSpaceGuid.PcdFixedDebugPrintErrorLevel|0x8000004F
  !endif

  #
  # Specifies the initial value for Register_D in RTC.
  # Reason for change:
  #  PcRtc.c wants to see register D bit 7 (VRT) high almost immediately after writing the below value,
  #  which clears it with the default UEFI value of zero. The AMD FCH updates this bit only once per 4-1020ms (1020ms default).
  #  This causes function RtcWaitToUpdate to return an error. Preset VRT to 1 to avoid this.
  #
  gPcAtChipsetPkgTokenSpaceGuid.PcdInitialValueRtcRegisterD|0x80


  !if gMinPlatformPkgTokenSpaceGuid.PcdPerformanceEnable == TRUE
    gEfiMdePkgTokenSpaceGuid.PcdPerformanceLibraryPropertyMask|0x1
    gEfiMdeModulePkgTokenSpaceGuid.PcdMaxPeiPerformanceLogEntries|80
  !else
    gEfiMdePkgTokenSpaceGuid.PcdPerformanceLibraryPropertyMask|0x00
  !endif

  #
  #SMBIOS
  #
  gAmdBoardPkgTokenSpaceGuid.PcdAmdSmbiosT16MaximumCapacity|0x80000000
  gAmdBoardPkgTokenSpaceGuid.PcdAmdSmbiosSocketDesignationSocket0|"P0"

  #
  # PCIe Config-space MMIO (1MB per bus, 256MB)
  #
  gEfiMdePkgTokenSpaceGuid.PcdPciExpressBaseAddress|0xE0000000
  gAmdCommonPkgTokenSpaceGuid.PcdPciExpressBaseAddressLow|0xE0000000
  gAmdCommonPkgTokenSpaceGuid.PcdPciExpressBaseAddressHi|0x00000000
  gAmdCommonPkgTokenSpaceGuid.PcdMmioCfgBusRange|0x00000008
  gEfiMdePkgTokenSpaceGuid.PcdPciExpressBaseSize|0x10000000
  # PCIe Config-space end address (PcdPciExpressBaseAddress + PcdPciExpressBaseSize)
  gAmdCommonPkgTokenSpaceGuid.PcdPciExpressLimitAddress|0xEFFFFFFF
  gEfiMdeModulePkgTokenSpaceGuid.PcdPcieResizableBarSupport|TRUE

  #
  # FCH UART HW Initialization
  # 0: Initialize FCH UART 0 HW
  # 1: Initialize FCH UART 1 HW
  #
  # If in Legacy IO mode, Need UART to conform to industry standard COM settings
  # COM1: I/O port 0x3F8, IRQ 4
  # COM2: I/O port 0x2F8, IRQ 3
  # COM3: I/O port 0x3E8, IRQ 4
  # COM4: I/O port 0x2E8, IRQ 3
  !if ($(SERIAL_PORT) == "FCH_MMIO") OR ($(SERIAL_PORT) == "FCH_IO")
    gPlatformTokenSpaceGuid.PcdFchUartPort|0
    !if ($(SERIAL_PORT) == "FCH_IO") # Set IRQs for UART0 -> COMx
      # UART0 is COM1/3 IRQ4
      !if $(SERIAL_REGISTER_BASE) == 0x3F8 OR $(SERIAL_REGISTER_BASE) == 0x3E8
        gAmdBoardPkgTokenSpaceGuid.PcdFchUart0Irq|0x04
        gAmdBoardPkgTokenSpaceGuid.PcdFchUart1Irq|0x03
        gAmdBoardPkgTokenSpaceGuid.PcdFchUart2Irq|0x04
        gAmdBoardPkgTokenSpaceGuid.PcdFchUart3Irq|0x03
      # UART0 is COM2/4 IRQ3
      !elseif $(SERIAL_REGISTER_BASE) == 0x2F8 OR $(SERIAL_REGISTER_BASE) == 0x2E8
        gAmdBoardPkgTokenSpaceGuid.PcdFchUart0Irq|0x03
        gAmdBoardPkgTokenSpaceGuid.PcdFchUart1Irq|0x04
        gAmdBoardPkgTokenSpaceGuid.PcdFchUart2Irq|0x03
        gAmdBoardPkgTokenSpaceGuid.PcdFchUart3Irq|0x04
      !endif
    !endif
  !elseif ($(SERIAL_PORT) == "BMC_SOL") OR ($(SERIAL_PORT) == "BMC_SOL_IO")
    gPlatformTokenSpaceGuid.PcdFchUartPort|1
    !if ($(SERIAL_PORT) == "BMC_SOL_IO") # Set IRQs for UART1 -> COMx
      # UART1 is COM1/3 IRQ4
      !if $(SERIAL_REGISTER_BASE) == 0x3F8 OR $(SERIAL_REGISTER_BASE) == 0x3E8
        gAmdBoardPkgTokenSpaceGuid.PcdFchUart0Irq|0x03
        gAmdBoardPkgTokenSpaceGuid.PcdFchUart1Irq|0x04
        gAmdBoardPkgTokenSpaceGuid.PcdFchUart2Irq|0x03
        gAmdBoardPkgTokenSpaceGuid.PcdFchUart3Irq|0x04
      # UART1 is COM2/4 IRQ3
      !elseif $(SERIAL_REGISTER_BASE) == 0x2F8 OR $(SERIAL_REGISTER_BASE) == 0x2E8
        gAmdBoardPkgTokenSpaceGuid.PcdFchUart0Irq|0x04
        gAmdBoardPkgTokenSpaceGuid.PcdFchUart1Irq|0x03
        gAmdBoardPkgTokenSpaceGuid.PcdFchUart2Irq|0x04
        gAmdBoardPkgTokenSpaceGuid.PcdFchUart3Irq|0x03
      !endif
    !endif
  !endif

  #
  # SerialPortLib (16650 UART)
  #
  ## Advertise that our UART is not PCI-based
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialPciDeviceInfo|{0xFF}
  ## Base address of 16550 serial port registers in MMIO or I/O space.
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialRegisterBase|$(SERIAL_REGISTER_BASE)
  #
  # Set UART parameters SERIAL_REGISTER_BASE (If it's MMIO, it must >= 0x10000)
  !if $(SERIAL_REGISTER_BASE) >= 0x10000
    ## Indicates the 16550 serial port registers are in MMIO space, or in I/O space.
    #   TRUE  - 16550 serial port registers are in MMIO space.
    #   FALSE - 16550 serial port registers are in I/O space.
    gEfiMdeModulePkgTokenSpaceGuid.PcdSerialUseMmio|TRUE

    ## The number of bytes between registers in serial device.
    gEfiMdeModulePkgTokenSpaceGuid.PcdSerialRegisterStride|4

    ## UART clock frequency is for the baud rate configuration.
    gEfiMdeModulePkgTokenSpaceGuid.PcdSerialClockRate|48000000
  !else
    ## Indicates the 16550 serial port registers are in MMIO space, or in I/O space.
    #   TRUE  - 16550 serial port registers are in MMIO space.
    #   FALSE - 16550 serial port registers are in I/O space.
    gEfiMdeModulePkgTokenSpaceGuid.PcdSerialUseMmio|FALSE

    ## The number of bytes between registers in serial device.
    gEfiMdeModulePkgTokenSpaceGuid.PcdSerialRegisterStride|1

    ## UART clock frequency is for the baud rate configuration.
    gEfiMdeModulePkgTokenSpaceGuid.PcdSerialClockRate|1843200
  !endif

  ## Indicates if the 16550 serial port hardware flow control will be enabled. Default is FALSE.
  #   TRUE  - 16550 serial port hardware flow control will be enabled.
  #   FALSE - 16550 serial port hardware flow control will be disabled.
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialUseHardwareFlowControl|FALSE

  ## Indicates if the 16550 serial Tx operations will be blocked if DSR is not asserted (no cable). Default is FALSE.
  #  This PCD is ignored if PcdSerialUseHardwareFlowControl is FALSE.
  #   TRUE  - 16550 serial Tx operations will be blocked if DSR is not asserted.
  #   FALSE - 16550 serial Tx operations will not be blocked if DSR is not asserted.
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialDetectCable|FALSE

  ## Line Control Register (LCR) for the 16550 serial port. This encodes data bits, parity, and stop bits.
  #    BIT1..BIT0 - Data bits.  00b = 5 bits, 01b = 6 bits, 10b = 7 bits, 11b = 8 bits.
  #    BIT2       - Stop Bits.  0 = 1 stop bit.  1 = 1.5 stop bits if 5 data bits selected, otherwise 2 stop bits.
  #    BIT5..BIT3 - Parity.  xx0b = No Parity, 001b = Odd Parity, 011b = Even Parity, 101b = Mark Parity, 111b=Stick Parity.
  #    BIT7..BIT6 - Reserved.  Must be 0.
  #
  #  Default is No Parity, 8 Data Bits, 1 Stop Bit.
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialLineControl|0x03

  ## FIFO Control Register (FCR) for the 16550 serial port.
  #    BIT0       - FIFO Enable.  0 = Disable FIFOs.  1 = Enable FIFOs.
  #    BIT1       - Clear receive FIFO.  1 = Clear FIFO.
  #    BIT2       - Clear transmit FIFO.  1 = Clear FIFO.
  #    BIT4..BIT3 - Reserved.  Must be 0.
  #    BIT5       - Enable 64-byte FIFO.  0 = Disable 64-byte FIFO.  1 = Enable 64-byte FIFO.
  #    BIT7..BIT6 - Reserved.  Must be 0.<BR>
  #
  #  Default is to enable and clear all FIFOs.
  !if ($(SERIAL_PORT) == "FCH_IO") OR ($(SERIAL_PORT) == "BMC_SOL_IO")
    gEfiMdeModulePkgTokenSpaceGuid.PcdSerialFifoControl|0x00
  !else
    gEfiMdeModulePkgTokenSpaceGuid.PcdSerialFifoControl|0x07
  !endif

  ## Baud rate for the 16550 serial port.  Default is 115200 baud.
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialBaudRate|115200

  #
  # SerialDxe (uses SerialPortLib)
  #
  ## Indicates the default baud rate of UART.
  # @ValidList | 115200, 57600, 38400, 19200, 9600, 7200, 4800, 3600, 2400, 2000, 1800, 1200, 600, 300, 150, 134, 110, 75, 50
  gEfiMdePkgTokenSpaceGuid.PcdUartDefaultBaudRate|115200

  ## Indicates the number of efficient data bit in UART transaction.
  # @ValidRange | 5 - 8
  gEfiMdePkgTokenSpaceGuid.PcdUartDefaultDataBits|8

  ## Indicates the setting of data parity in UART transaction.
  # 0 - Default Parity.
  # 1 - No Parity.
  # 2 - Even Parity.
  # 3 - Odd Parity.
  # 4 - Mark Parity.
  # 5 - Space Parity.
  # @ValidRange | 0 - 5
  gEfiMdePkgTokenSpaceGuid.PcdUartDefaultParity|1

  ## Indicates the setting of stop bit in UART transaction.
  # 0 - Default Stop Bits.
  # 1 - One Stop Bit.
  # 2 - One Five Stop Bits.
  # 3 - Two Stop Bits.
  # @ValidRange | 0 - 3
  gEfiMdePkgTokenSpaceGuid.PcdUartDefaultStopBits|1

  ## Indicates the receive FIFO depth of UART controller.
  gEfiMdePkgTokenSpaceGuid.PcdUartDefaultReceiveFifoDepth|64

  #
  # TerminalDxe
  #
  ## Indicates the usable type of terminal.
  #  0 - PCANSI
  #  1 - VT100
  #  2 - VT100+
  #  3 - UTF8
  #  4 - TTYTERM, NOT defined in UEFI SPEC
  # @ValidRange | 0 - 4
  gEfiMdePkgTokenSpaceGuid.PcdDefaultTerminalType|0

  #
  # Boot
  #
  # PCDs to set the default size of the different UEFI memory types to promote
  # contiguous UEFI memory allocation.  These values are used by
  # AmdCommon/Pei/PlatformInitPei/MemoryInitPei.c to reserve
  # default chunks for each memory type when gEfiMemoryTypeInformationGuid
  # variable is not set.  These values can be updated to prevent reboot because
  # MdeModulePkg/Library/UefiBootManagerLib/BmMisc.c:
  # BmSetMemoryTypeInformationVariable() sets gEfiMemoryTypeInformationGuid at
  # the end of post to reserve more memory.  Serial output from this code will
  # display sizes required, which can then be updated in these PCDs.
  # Memory Type 09
  gMinPlatformPkgTokenSpaceGuid.PcdPlatformEfiAcpiReclaimMemorySize        | 0x200
  # Memory Type 0A
  gMinPlatformPkgTokenSpaceGuid.PcdPlatformEfiAcpiNvsMemorySize            | 0x100
  # Memory Type 00
  gMinPlatformPkgTokenSpaceGuid.PcdPlatformEfiReservedMemorySize           | 0x1000
  # Memory Type 06
  gMinPlatformPkgTokenSpaceGuid.PcdPlatformEfiRtDataMemorySize             | 0x400
  # Memory Type 05
  gMinPlatformPkgTokenSpaceGuid.PcdPlatformEfiRtCodeMemorySize             | 0x100
  gEfiMdePkgTokenSpaceGuid.PcdPlatformBootTimeOut|10
  gEfiMdeModulePkgTokenSpaceGuid.PcdConInConnectOnDemand|FALSE
  gEfiMdeModulePkgTokenSpaceGuid.PcdResetOnMemoryTypeInformationChange|TRUE
  # 462CAA21-7614-4503-836E-8AB6F4662331 (UiApp FILE_GUID)
  gEfiMdeModulePkgTokenSpaceGuid.PcdBootManagerMenuFile|{ GUID("462CAA21-7614-4503-836E-8AB6F4662331") }
!if $(USE_1GB_PAGE_TABLES)
  gEfiMdeModulePkgTokenSpaceGuid.PcdUse1GPageTable|TRUE
!endif

  #
  # ACPI
  #
  gAmdBoardPkgTokenSpaceGuid.PcdFchOemEnableAcpiSwSmi|0xA0
  gAmdBoardPkgTokenSpaceGuid.PcdFchOemDisableAcpiSwSmi|0xA1
  gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiDefaultOemId|"AMDINC"
  gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiDefaultOemTableId|0x2020202058594E4F # "ONYX    "
  gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiDefaultOemRevision|0x00000001
  gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiDefaultCreatorId|0x20444D41
  gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiDefaultCreatorRevision|0x00000001
  gMinPlatformPkgTokenSpaceGuid.PcdFadtPreferredPmProfile|4
  gMinPlatformPkgTokenSpaceGuid.PcdFadtIaPcBootArch|0x0000
  gMinPlatformPkgTokenSpaceGuid.PcdFadtFlags|0x0002052D
  gPcAtChipsetPkgTokenSpaceGuid.PcdHpetBaseAddress|0xFED00000
  gAmdBoardPkgTokenSpaceGuid.PcdAmdAcpiCpuSsdtProcessorScopeInSb|TRUE
  gMinPlatformPkgTokenSpaceGuid.PcdIoApicId|0x80   # gAmdBoardPkgTokenSpaceGuid.PcdCfgFchIoapicId
  gMinPlatformPkgTokenSpaceGuid.PcdPcIoApicCount|4
  gMinPlatformPkgTokenSpaceGuid.PcdPcIoApicIdBase|0xF1
  gMinPlatformPkgTokenSpaceGuid.PcdPcIoApicAddressBase|0xFEC00000
  gMinPlatformPkgTokenSpaceGuid.PcdLocalApicAddress|0xFEE00000
  gMinPlatformPkgTokenSpaceGuid.PcdAcpiEnableSwSmi|gAmdBoardPkgTokenSpaceGuid.PcdFchOemEnableAcpiSwSmi
  gMinPlatformPkgTokenSpaceGuid.PcdAcpiDisableSwSmi|gAmdBoardPkgTokenSpaceGuid.PcdFchOemDisableAcpiSwSmi

  # Max Cpu constraints
  gMinPlatformPkgTokenSpaceGuid.PcdMaxCpuThreadCount|2
  gMinPlatformPkgTokenSpaceGuid.PcdMaxCpuCoreCount|128
  gMinPlatformPkgTokenSpaceGuid.PcdMaxCpuSocketCount|1

  #
  # EFI NV Storage
  #
  gEfiMdeModulePkgTokenSpaceGuid.PcdMaxVariableSize|0x3000

  gAmdBoardPkgTokenSpaceGuid.PcdCfgIommuMMIOAddressReservedEnable|TRUE
  gAmdBoardPkgTokenSpaceGuid.PcdCfgIoApicMMIOAddressReservedEnable|TRUE
  gAmdBoardPkgTokenSpaceGuid.PcdCfgIoApicIdPreDefineEn|TRUE   #### Makes PEI assign IOAPIC IDs
  gAmdBoardPkgTokenSpaceGuid.PcdCfgIoApicIdBase|0xF1
  gAmdBoardPkgTokenSpaceGuid.PcdCompliantEdkIIAcpiSdtProtocol|TRUE
  gAmdBoardPkgTokenSpaceGuid.PcdAmdFchCfgAcpiPm1EvtBlkAddr|0x800
  gAmdBoardPkgTokenSpaceGuid.PcdAmdFchCfgAcpiPm1CntBlkAddr|0x804
  gAmdBoardPkgTokenSpaceGuid.PcdAmdFchCfgAcpiPmTmrBlkAddr|0x808
  gAmdBoardPkgTokenSpaceGuid.PcdAmdFchCfgCpuControlBlkAddr|0x810
  gAmdBoardPkgTokenSpaceGuid.PcdAmdFchCfgAcpiGpe0BlkAddr|0x820
  gAmdBoardPkgTokenSpaceGuid.PcdFchOemBeforePciRestoreSwSmi|0xB3
  gAmdBoardPkgTokenSpaceGuid.PcdFchOemAfterPciRestoreSwSmi|0xB4
  gAmdBoardPkgTokenSpaceGuid.PcdFchOemSpiUnlockSwSmi|0xB7
  gAmdBoardPkgTokenSpaceGuid.PcdFchOemSpiLockSwSmi|0xB8
  gAmdBoardPkgTokenSpaceGuid.PcdAmdNumberOfPhysicalSocket|gMinPlatformPkgTokenSpaceGuid.PcdMaxCpuSocketCount

  #
  # Memory
  #
  gAmdBoardPkgTokenSpaceGuid.PcdAmdMemMaxDimmPerChannelV2|1

  # Change MTRR defaults for 0xE0000-0xFFFFF
  gAmdBoardPkgTokenSpaceGuid.PcdAmdFixedMtrr26C|0xFFFFFFFFFFFFFFFF
  gAmdBoardPkgTokenSpaceGuid.PcdAmdFixedMtrr26D|0xFFFFFFFFFFFFFFFF
  gAmdBoardPkgTokenSpaceGuid.PcdAmdFixedMtrr26E|0xFFFFFFFFFFFFFFFF
  gAmdBoardPkgTokenSpaceGuid.PcdAmdFixedMtrr26F|0xFFFFFFFFFFFFFFFF

  # Disable S3 support
  gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiS3Enable|FALSE

  ## Toggle for whether the VariablePolicy engine should allow disabling.
  # The engine is enabled at power-on, but the interface allows the platform to
  # disable enforcement for servicing flexibility. If this PCD is disabled, it will block the ability to
  # disable the enforcement and VariablePolicy enforcement will always be ON.
  #   TRUE - VariablePolicy can be disabled by request through the interface (until interface is locked)
  #   FALSE - VariablePolicy interface will not accept requests to disable and is ALWAYS ON
  # @Prompt Allow VariablePolicy enforcement to be disabled.
  gEfiMdeModulePkgTokenSpaceGuid.PcdAllowVariablePolicyEnforcementDisable|TRUE

  #
  # FALSE: The board is not a FSP wrapper (FSP binary not used)
  # TRUE:  The board is a FSP wrapper (FSP binary is used)
  #
  gMinPlatformPkgTokenSpaceGuid.PcdFspWrapperBootMode|FALSE

  # TRUE  - 5-Level Paging will be enabled.
  # FALSE - 5-Level Paging will not be enabled.
  gEfiMdeModulePkgTokenSpaceGuid.PcdUse5LevelPageTable|FALSE

  # Specifies stack size in bytes for each processor in SMM.
  gUefiCpuPkgTokenSpaceGuid.PcdCpuSmmStackSize|0x4000
  gAmdBoardPkgTokenSpaceGuid.PcdAmdAcpiTableHeaderOemId|gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiDefaultOemId
  gAmdBoardPkgTokenSpaceGuid.PcdAmdMemMaxSocketSupportedV2|1

  # Secureboot
  !if gMinPlatformPkgTokenSpaceGuid.PcdUefiSecureBootEnable == TRUE
    gEfiSecurityPkgTokenSpaceGuid.PcdUserPhysicalPresence|TRUE
  !endif

[PcdsDynamicDefault.common]
  gEfiMdeModulePkgTokenSpaceGuid.PcdStatusCodeUseSerial|TRUE
  gEfiMdeModulePkgTokenSpaceGuid.PcdStatusCodeUseMemory|FALSE

  #
  # Set video resolution.
  #
  gEfiMdeModulePkgTokenSpaceGuid.PcdVideoHorizontalResolution|1024
  gEfiMdeModulePkgTokenSpaceGuid.PcdVideoVerticalResolution|768

  #
  # Account for 1P configuration (256 threads per socket)
  #
  gUefiCpuPkgTokenSpaceGuid.PcdCpuMaxLogicalProcessorNumber|256

  #
  # Set MMIO Above4GB at the 1TB boundary
  #
  gAmdBoardPkgTokenSpaceGuid.PcdAmdMmioAbove4GLimit|0x7FBFFFFFFFF

  #
  # The base address of temporary page table for accessing PCIE MMIO base address above 4G in PEI phase.
  #
  gAmdBoardPkgTokenSpaceGuid.PcdAmdPeiTempPageTableBaseAddress|0x60000000

  # IO Resource padding in bytes, default 4KB, override to 0.
  gAmdCommonPkgTokenSpaceGuid.PcdPciHotPlugResourcePadIO|0x00

  #
  # Flash NV Storage
  #
  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageVariableBase64|0
  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwWorkingBase64|0
  gEfiMdeModulePkgTokenSpaceGuid.PcdFlashNvStorageFtwSpareBase64|0

  gAmdBoardPkgTokenSpaceGuid.PcdCfgGnbIoapicId|gAmdBoardPkgTokenSpaceGuid.PcdCfgIoApicIdBase

  gAmdBoardPkgTokenSpaceGuid.PcdLegacyFree|FALSE
  gAmdBoardPkgTokenSpaceGuid.PcdHpetEnable|TRUE
  gAmdBoardPkgTokenSpaceGuid.PcdHpetMsiDis|FALSE
  gAmdBoardPkgTokenSpaceGuid.PcdNativePcieSupport|TRUE
  gAmdBoardPkgTokenSpaceGuid.PcdNoneSioKbcSupport|TRUE
  gAmdBoardPkgTokenSpaceGuid.PcdCfgFchIoapicId|gMinPlatformPkgTokenSpaceGuid.PcdIoApicId

  # FchRTDeviceEnableMap
  # < BIT4 - LPC : PcdLpcEnable
  # < BIT5 - I2C0 : FchRTDeviceEnableMap[BIT5]
  # < BIT6 - I2C1 : FchRTDeviceEnableMap[BIT6]
  # < BIT7 - I2C2 : FchRTDeviceEnableMap[BIT7]
  # < BIT8 - I2C3 : FchRTDeviceEnableMap[BIT8]
  # < BIT9 - I2C4 : FchRTDeviceEnableMap[BIT9]
  # < BIT10 - I2C5 : FchRTDeviceEnableMap[BIT10]
  # < BIT11 - UART0 : FchRTDeviceEnableMap[BIT11]
  # < BIT12 - UART1 : FchRTDeviceEnableMap[BIT12]
  # < BIT16 - UART2 : FchRTDeviceEnableMap[BIT13]
  # < BIT18 - SD : PcdEmmcEnable and PcdEmmcType < 5
  # < BIT26 - UART3 : FchRTDeviceEnableMap[BIT26]
  # < BIT27 - eSPI : PcdEspiEnable - read-only.
  # < BIT28 - eMMC : PcdEmmcEnable - read-only.
  !if ($(SERIAL_PORT) == "FCH_MMIO") OR ($(SERIAL_PORT) == "FCH_IO")
    gAmdBoardPkgTokenSpaceGuid.FchRTDeviceEnableMap|0x00000800
  !elseif ($(SERIAL_PORT) == "BMC_SOL") OR ($(SERIAL_PORT) == "BMC_SOL_IO")
    gAmdBoardPkgTokenSpaceGuid.FchRTDeviceEnableMap|0x00001000
  !endif
  # FchUartLegacyEnable
  # 0-disable, 1- 0x2E8/2EF, 2 - 0x2F8/2FF, 3 - 0x3E8/3EF, 4 - 0x3F8/3FF
  !if $(SERIAL_PORT) == "FCH_IO"
    !if $(SERIAL_REGISTER_BASE) == 0x2E8
      gAmdBoardPkgTokenSpaceGuid.FchUart0LegacyEnable|0x01
    !elseif $(SERIAL_REGISTER_BASE) == 0x2F8
      gAmdBoardPkgTokenSpaceGuid.FchUart0LegacyEnable|0x02
    !elseif $(SERIAL_REGISTER_BASE) == 0x3E8
      gAmdBoardPkgTokenSpaceGuid.FchUart0LegacyEnable|0x03
    !elseif $(SERIAL_REGISTER_BASE) == 0x3F8
      gAmdBoardPkgTokenSpaceGuid.FchUart0LegacyEnable|0x04
    !endif
  !elseif  $(SERIAL_PORT) == "BMC_SOL_IO"
    !if $(SERIAL_REGISTER_BASE) == 0x2E8
      gAmdBoardPkgTokenSpaceGuid.FchUart1LegacyEnable|0x01
    !elseif $(SERIAL_REGISTER_BASE) == 0x2F8
      gAmdBoardPkgTokenSpaceGuid.FchUart1LegacyEnable|0x02
    !elseif $(SERIAL_REGISTER_BASE) == 0x3E8
      gAmdBoardPkgTokenSpaceGuid.FchUart1LegacyEnable|0x03
    !elseif $(SERIAL_REGISTER_BASE) == 0x3F8
      gAmdBoardPkgTokenSpaceGuid.FchUart1LegacyEnable|0x04
    !endif
  !endif

  !if $(SLINK_SUPPORT)
    gAmdBoardPkgTokenSpaceGuid.PcdCcixEnable|TRUE
  !endif

  #
  # Platform CCIX (Slink)
  #
  # PcdInterleavePortsOnCcixController:
  # 1) Requires equal memory capacity per port (Endpoint)
  # 2) Requires power-of-two connected ports (2 or 4)
  #
  # PcdInterleavePairOfCcixControllers:
  # 1) Implies PcdInterleavePortsOnCcixController is TRUE
  # 2) Requires same number of connected ports per controller (1, 2 or 4)
  # 3) Requires separate pair of Coherent Slaves assigned to each controller
  # 4) Valid config NBIO configs: NBIO0+NBIO1, NBIO0+NBIO3, NBIO2+NBIO1, NBIO2+NBIO3
  #
  #

  gAmdBoardPkgTokenSpaceGuid.PcdEarlyBmcLinkTraining|TRUE
  gAmdBoardPkgTokenSpaceGuid.PcdEarlyBmcLinkSocket|0
  gAmdBoardPkgTokenSpaceGuid.PcdEarlyBmcLinkDie|0
  gAmdBoardPkgTokenSpaceGuid.PcdEarlyBmcLinkLaneNum|134

  gAmdBoardPkgTokenSpaceGuid.PcdXhciOcPolarityCfgLow|TRUE
  gAmdBoardPkgTokenSpaceGuid.PcdXhciUsb31OcPinSelect|0xFFFF1010
  gAmdBoardPkgTokenSpaceGuid.PcdXhciUsb20OcPinSelect|0xFFFFFFFFFFFF1010

  !if $(USB_SUPPORT)
    ### USB 3.0 controller0
    gAmdBoardPkgTokenSpaceGuid.PcdXhci0Enable|TRUE
    ### USB 3.0 controller1
    gAmdBoardPkgTokenSpaceGuid.PcdXhci1Enable|TRUE
    ### USB3.0 controller0 on MCM-1
    gAmdBoardPkgTokenSpaceGuid.PcdXhci2Enable|FALSE
    ### USB3.0 controller1 on MCM-1
    gAmdBoardPkgTokenSpaceGuid.PcdXhci3Enable|FALSE

    gAmdBoardPkgTokenSpaceGuid.PcdXhciSsid|0x00000000
    gAmdBoardPkgTokenSpaceGuid.PcdXhciECCDedErrRptEn|FALSE
  !else
    ### USB 3.0 controller0
    gAmdBoardPkgTokenSpaceGuid.PcdXhci0Enable|FALSE
    ### USB 3.0 controller1
    gAmdBoardPkgTokenSpaceGuid.PcdXhci1Enable|FALSE
    ### USB3.0 controller0 on MCM-1
    gAmdBoardPkgTokenSpaceGuid.PcdXhci2Enable|FALSE
    ### USB3.0 controller1 on MCM-1
    gAmdBoardPkgTokenSpaceGuid.PcdXhci3Enable|FALSE
  !endif

!if $(SATA_SUPPORT)
  ### @brief FCH-SATA enables
  ### @details Select whether or not the FCH Sata controller is active.
  ### @li  TRUE  - This option is active.
  ### @li  FALSE - This option is turned off.
  gAmdBoardPkgTokenSpaceGuid.PcdSataEnable|TRUE

  gAmdBoardPkgTokenSpaceGuid.PcdSataStaggeredSpinup|TRUE

  ### @brief Sata Port ESP Enables
  ### @details Die1, Die2, etc. Port 0-7 ESP
  gAmdBoardPkgTokenSpaceGuid.PcdSataMultiDiePortESP|0x00000000FFFFFFFF
  #gAmdBoardPkgTokenSpaceGuid.PcdSataMultiDiePortESP|0x000000000000FFFF
!else
  ### @brief FCH-SATA enables
  ### @details Select whether or not the FCH Sata controller is active.
  ### @li  TRUE  - This option is active.
  ### @li  FALSE - This option is turned off.
  gAmdBoardPkgTokenSpaceGuid.PcdSataEnable|FALSE
!endif

  #
  # Set Package Power Tracking Limit
  #
  gAmdBoardPkgTokenSpaceGuid.PcdCfgPlatformPPT|400

  #
  # Firmware Revision
  #
  gEfiMdeModulePkgTokenSpaceGuid.PcdFirmwareVendor|L"AMD"
  gEfiMdeModulePkgTokenSpaceGuid.PcdFirmwareRevision|$(FIRMWARE_REVISION_NUM)
  gEfiMdeModulePkgTokenSpaceGuid.PcdFirmwareVersionString|L"$(FIRMWARE_VERSION_STR)"
  gEfiMdeModulePkgTokenSpaceGuid.PcdFirmwareReleaseDateString|L"$(RELEASE_DATE)"

  !if $(DISABLE_SMT)
    gAmdBoardPkgTokenSpaceGuid.PcdAmdSmtMode|0x0
  !endif

  gAmdBoardPkgTokenSpaceGuid.PcdNvdimmEnable|FALSE

  gAmdBoardPkgTokenSpaceGuid.PcdAmdApicMode|0x02
  # MinPlatformPkg
  gMinPlatformPkgTokenSpaceGuid.PcdPcIoApicEnable|0x0F
  gMinPlatformPkgTokenSpaceGuid.PcdAcpiGpe0BlockAddress|gAmdBoardPkgTokenSpaceGuid.PcdAmdFchCfgAcpiGpe0BlkAddr
  gMinPlatformPkgTokenSpaceGuid.PcdAcpiGpe1BlockAddress|0
  gMinPlatformPkgTokenSpaceGuid.PcdAcpiPm1AControlBlockAddress|gAmdBoardPkgTokenSpaceGuid.PcdAmdFchCfgAcpiPm1CntBlkAddr
  gMinPlatformPkgTokenSpaceGuid.PcdAcpiPm1AEventBlockAddress|gAmdBoardPkgTokenSpaceGuid.PcdAmdFchCfgAcpiPm1EvtBlkAddr
  gMinPlatformPkgTokenSpaceGuid.PcdAcpiPm1BControlBlockAddress|0
  gMinPlatformPkgTokenSpaceGuid.PcdAcpiPm1BEventBlockAddress|0
  gMinPlatformPkgTokenSpaceGuid.PcdAcpiPm2ControlBlockAddress|0
  gMinPlatformPkgTokenSpaceGuid.PcdAcpiPmTimerBlockAddress|gAmdBoardPkgTokenSpaceGuid.PcdAmdFchCfgAcpiPmTmrBlkAddr

[PcdsPatchableInModule]
    gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|gEfiMdePkgTokenSpaceGuid.PcdFixedDebugPrintErrorLevel

[PcdsDynamicHii.X64.DEFAULT]
!if gMinPlatformPkgTokenSpaceGuid.PcdTpm2Enable == TRUE
  gEfiSecurityPkgTokenSpaceGuid.PcdTcgPhysicalPresenceInterfaceVer|L"TCG2_VERSION"|gTcg2ConfigFormSetGuid|0x0|"1.3"|NV,BS
  gEfiSecurityPkgTokenSpaceGuid.PcdTpm2AcpiTableRev|L"TCG2_VERSION"|gTcg2ConfigFormSetGuid|0x8|4|NV,BS
!endif

[PcdsDynamicDefault]
  gAmdBoardPkgTokenSpaceGuid.PcdLegacyFree|TRUE
  gAmdBoardPkgTokenSpaceGuid.PcdAmdMemPostPackageRepair|TRUE
  gAmdBoardPkgTokenSpaceGuid.PcdAmdCxlProtocolErrorReporting|1
  gAmdBoardPkgTokenSpaceGuid.PcdAmdCxlComponentErrorReporting|1
  gAmdBoardPkgTokenSpaceGuid.PcdEgressPoisonSeverityLo|0
  gAmdBoardPkgTokenSpaceGuid.PcdEgressPoisonSeverityHi|0
  gAmdBoardPkgTokenSpaceGuid.PcdXhciECCDedErrRptEn|TRUE
  !ifdef $(INTERNAL_IDS)
    gAmdBoardPkgTokenSpaceGuid.PcdAmdCcxSingleBitErrLogging|TRUE
  !endif
  gAmdBoardPkgTokenSpaceGuid.PcdAmdS3LibTableSize|0x100000
  gAmdBoardPkgTokenSpaceGuid.PcdAmdPspAntiRollbackLateSplFuse|TRUE
  gAmdBoardPkgTokenSpaceGuid.PcdUsbSspOemConfigurationTable|{0x0D, 0x02, 0x50, 0x00, 0x03, 0x03, 0x03, 0x03, 0x00, 0x01, 0x06, 0x03, 0x01, 0x03, 0x03, 0x03, 0x03, 0x00, 0x01, 0x06, 0x03, 0x01, 0x03, 0x03, 0x03, 0x03, 0x00, 0x01, 0x06, 0x03, 0x01, 0x03, 0x03, 0x03, 0x03, 0x00, 0x01, 0x06, 0x03, 0x01, 0x00, 0xFF, 0x03, 0x03, 0x03, 0x03, 0x00, 0x01, 0x06, 0x03, 0x01,0x03, 0x03, 0x03, 0x03, 0x00, 0x01, 0x06, 0x03, 0x01,0x03, 0x03, 0x03, 0x03, 0x00, 0x01, 0x06, 0x03, 0x01,0x03, 0x03, 0x03, 0x03, 0x00, 0x01, 0x06, 0x03, 0x01, 0x00, 0x00}

  gAmdBoardPkgTokenSpaceGuid.PcdCfgIommuSupport|TRUE
  !if $(SIMNOW_SUPPORT) == TRUE
    # DMA Remap not working in UEFI SimNow
    gAmdBoardPkgTokenSpaceGuid.PcdIvInfoDmaReMap|FALSE
  !else
    gAmdBoardPkgTokenSpaceGuid.PcdIvInfoDmaReMap|FALSE
  !endif

  # Enable CState
  gAmdBoardPkgTokenSpaceGuid.PcdAmdCStateMode|1
  gAmdBoardPkgTokenSpaceGuid.PcdAmdCStateIoBaseAddress|0x813

  gAmdBoardPkgTokenSpaceGuid.PcdAmdFchI2c0SdaHold|0x35
  gAmdBoardPkgTokenSpaceGuid.PcdAmdFchI2c1SdaHold|0x35
  gAmdBoardPkgTokenSpaceGuid.PcdAmdFchI2c2SdaHold|0x35
  gAmdBoardPkgTokenSpaceGuid.PcdAmdFchI2c3SdaHold|0x35
  gAmdBoardPkgTokenSpaceGuid.PcdAmdFchI2c4SdaHold|0x35
  gAmdBoardPkgTokenSpaceGuid.PcdAmdFchI2c5SdaHold|0x35

  gAmdBoardPkgTokenSpaceGuid.PcdResetMode|0x07

  # According to ACPI 6.4 porting guide
  gAmdBoardPkgTokenSpaceGuid.PcdAmdFchCfgSmiCmdPortAddr|0xB2

!if gMinPlatformPkgTokenSpaceGuid.PcdTpm2Enable == TRUE
  ## This PCD indicates the initialization policy for TPM 1.2.<BR><BR>
  #  If 0, no initialization needed - most likely used for chipset SRTM solution, in which TPM is already initialized.<BR>
  #  If 1, initialization needed.<BR>
  # @Prompt TPM 1.2 device initialization policy.
  # @ValidRange 0x80000001 | 0x00 - 0x1
  gEfiSecurityPkgTokenSpaceGuid.PcdTpmInitializationPolicy|1

  ## Guid name to identify TPM instance.<BR><BR>
  #  TPM_DEVICE_INTERFACE_NONE means disable.<BR>
  #  TPM_DEVICE_INTERFACE_TPM12 means TPM 1.2 DTPM.<BR>
  #  TPM_DEVICE_INTERFACE_DTPM2 means TPM 2.0 DTPM.<BR>
  #  Other GUID value means other TPM 2.0 device.<BR>
  # @Prompt TPM device type identifier
  gEfiSecurityPkgTokenSpaceGuid.PcdTpmInstanceGuid|{GUID({ 0x286bf25a, 0xc2c3, 0x408c, { 0xb3, 0xb4, 0x25, 0xe6, 0x75, 0x8b, 0x73, 0x17 } })}

  ## This PCD is to control which device is the potential trusted console input device.<BR><BR>
  # For example:<BR>
  # USB Short Form: UsbHID(0xFFFF,0xFFFF,0x1,0x1)<BR>
  #   //Header                    VendorId    ProductId   Class SubClass Protocol<BR>
  #     {0x03, 0x0F, 0x0B, 0x00,  0xFF, 0xFF, 0xFF, 0xFF, 0x03, 0x01,    0x01,<BR>
  #   //Header<BR>
  #      0x7F, 0xFF, 0x04, 0x00}<BR>
  gMinPlatformPkgTokenSpaceGuid.PcdTrustedConsoleInputDevicePath|{DEVICE_PATH("VenHw(D3987D4B-971A-435F-8CAF-4967EB627241)/Uart(115200,8,N,1)/VenMsg(E0C14753-F9BE-11D2-9A0C-0090273FC14D),UsbClass(0xFFFF,0xFFFF,0x03,0x01,0x01)")}

  ## This PCD is to control which device is the potential trusted console output device.<BR><BR>
  # For example:<BR>
  # Integrated Graphic: PciRoot(0x0)/Pci(0x2,0x0)<BR>
  #   //Header                    HID                     UID<BR>
  #     {0x02, 0x01, 0x0C, 0x00,  0xd0, 0x41, 0x03, 0x0A, 0x00, 0x00, 0x00, 0x00,<BR>
  #   //Header                    Func  Dev<BR>
  #      0x01, 0x01, 0x06, 0x00,  0x00, 0x02,
  #   //Header<BR>
  #      0x7F, 0xFF, 0x04, 0x00}<BR>
  gMinPlatformPkgTokenSpaceGuid.PcdTrustedConsoleOutputDevicePath|{DEVICE_PATH("VenHw(D3987D4B-971A-435F-8CAF-4967EB627241)/Uart(115200,8,N,1)/VenMsg(E0C14753-F9BE-11D2-9A0C-0090273FC14D),PcieRoot(0xB)/Pci(0x05,0x02)/Pci(0x00,0x00)/Pci(0x00,0x00)/Pci(0x00,0x00)/AcpiAdr(0x80010100)")}
  ## This PCD indicates current active TPM interface type.
  #  According to TCG PTP spec 1.3, there are 3 types defined in TPM2_PTP_INTERFACE_TYPE.<BR>
  #  0x00 - FIFO interface as defined in TIS 1.3 is active.<BR>
  #  0x01 - FIFO interface as defined in PTP for TPM 2.0 is active.<BR>
  #  0x02 - CRB interface is active.<BR>
  #  0xFF - Contains no current active TPM interface type.<BR>
  #
  # @Prompt current active TPM interface type.
  gEfiSecurityPkgTokenSpaceGuid.PcdActiveTpmInterfaceType|0x00
!endif

[PcdsDynamicExDefault]
  #MCTP
  gAmdBoardPkgTokenSpaceGuid.PcdAmdMCTPEnable|TRUE

#######################################
# Library Includes
#######################################
!include MinPlatformPkg/Include/Dsc/CoreCommonLib.dsc
!include MinPlatformPkg/Include/Dsc/CorePeiLib.dsc
!include MinPlatformPkg/Include/Dsc/CoreDxeLib.dsc

[LibraryClasses.common]
  #####################################
  # Platform Package
  #####################################
  ReportFvLib|AmdCommonPkg/Library/PeiReportFvLib/PeiReportFvLib.inf
  TestPointCheckLib|MinPlatformPkg/Test/Library/TestPointCheckLibNull/TestPointCheckLibNull.inf
  PlatformBootManagerLib|MinPlatformPkg/Bds/Library/DxePlatformBootManagerLib/DxePlatformBootManagerLib.inf
  LocalApicLib|AmdCommonPkg/Library/BaseXApicX2ApicLib/BaseXApicX2ApicLib.inf
  PciSegmentLib|MdePkg/Library/PciSegmentLibSegmentInfo/BasePciSegmentLibSegmentInfo.inf
  FabricResourceManagerLib|Platform/GenoaOpenBoardPkg/Universal/Library/FabricResourceManagerLib/FabricResourceManagerLib.inf
  PcieResourcesLib|Platform/GenoaOpenBoardPkg/Universal/Library/PcieResourcesLib/PcieResourcesLib.inf
  PciSegmentInfoLib|Platform/GenoaOpenBoardPkg/Universal/Library/PciSegmentInfoLibSimple/PciSegmentInfoLibSimple.inf
  BoardAcpiEnableLib|Platform/GenoaOpenBoardPkg/Fch/BoardAcpiLib/SmmBoardAcpiEnableLib.inf
  PspDirectoryLib|Platform/AmdCommonPkg/Library/PspDirectoryLib/PspDirectoryLib.inf
  
[LibraryClasses]
  #
  # Base
  #
  !if $(LOGGING_ENABLE)
    DebugLib|MdePkg/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
  !else
    DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
  !endif
  !if $(SOURCE_DEBUG_ENABLE)
    DebugCommunicationLib|SourceLevelDebugPkg/Library/DebugCommunicationLibSerialPort/DebugCommunicationLibSerialPort.inf
    PeCoffExtraActionLib|SourceLevelDebugPkg/Library/PeCoffExtraActionLibDebug/PeCoffExtraActionLibDebug.inf
  !else
    PeCoffExtraActionLib|MdePkg/Library/BasePeCoffExtraActionLibNull/BasePeCoffExtraActionLibNull.inf
  !endif
  ## PciCf8Lib|MdePkg/Library/BasePciCf8Lib/BasePciCf8Lib.inf
  ## PciLib|MdePkg/Library/BasePciLibCf8/BasePciLibCf8.inf
  PciLib|MdePkg/Library/BasePciLibPciExpress/BasePciLibPciExpress.inf
  
  #
  # Generic Modules
  #
  SmbusLib|MdePkg/Library/DxeSmbusLib/DxeSmbusLib.inf

!if $(CAPSULE_ENABLE)
  CapsuleLib|MdeModulePkg/Library/DxeCapsuleLibFmp/DxeCapsuleLib.inf
!else
  CapsuleLib|MdeModulePkg/Library/DxeCapsuleLibNull/DxeCapsuleLibNull.inf
!endif

  #
  # Platform-specific
  #
  PlatformSecLib|AmdCommonPkg/Library/PlatformSecLib/PlatformSecLib.inf
!if $(SERIAL_PORT) == "NONE"
  SerialPortLib|MdePkg/Library/BaseSerialPortLibNull/BaseSerialPortLibNull.inf
!else
  SerialPortLib|MdeModulePkg/Library/BaseSerialPortLib16550/BaseSerialPortLib16550.inf
  !if $(SERIAL_PORT) == "BMC_ESPI"
    PlatformHookLib|MdeModulePkg/Library/BasePlatformHookLibNull/BasePlatformHookLibNull.inf
  !else
    PlatformHookLib|AmdCommonPkg/Library/PlatformHookLib/BasePlatformHookLibAmdFchUart/BasePlatformHookLibAmdFchUart.inf
  !endif
!endif
  TimerLib|AmdCommonPkg/Library/TscTimerLib/DxeTscTimerLib.inf
  
  AcpiSdtParserLib|AmdCommonPkg/Library/DxeAcpiSdtParserLib/DxeAcpiSdtParserLib.inf
  AslUpdateLib|MinPlatformPkg/Acpi/Library/DxeAslUpdateLib/DxeAslUpdateLib.inf
  BoardAcpiTableLib|MinPlatformPkg/Acpi/Library/BoardAcpiTableLibNull/BoardAcpiTableLibNull.inf

 
  #
  # AMD AML
  #
  AmlLib|AmdCommonPkg/Library/DxeAmdAmlLib/AmlLib.inf

!if $(UNIT_TEST_AMD_AML_ENABLE)
  UnitTestLib|UnitTestFrameworkPkg/Library/UnitTestLib/UnitTestLib.inf
  UnitTestPersistenceLib|UnitTestFrameworkPkg/Library/UnitTestPersistenceLibNull/UnitTestPersistenceLibNull.inf
  UnitTestResultReportLib|UnitTestFrameworkPkg/Library/UnitTestResultReportLib/UnitTestResultReportLibDebugLib.inf
!endif
  BootLogoLib|MdeModulePkg/Library/BootLogoLib/BootLogoLib.inf

[LibraryClasses.common.SEC]
  #
  # SEC specific phase
  #
  TimerLib|AmdCommonPkg/Library/TscTimerLib/BaseTscTimerLib.inf

[LibraryClasses.IA32.PEIM,LibraryClasses.IA32.PEI_CORE]
  #
  # PEI phase common
  #
  TimerLib|AmdCommonPkg/Library/TscTimerLib/PeiTscTimerLib.inf
  ResetSystemLib|MdeModulePkg/Library/PeiResetSystemLib/PeiResetSystemLib.inf

[LibraryClasses.common.PEIM]
  SetCacheMtrrLib|AmdCommonPkg/Library/SetCacheMtrrLib/SetCacheMtrrLib.inf
  ReportCpuHobLib|AmdCommonPkg/Library/ReportCpuHobLib/ReportCpuHobLib.inf

  libAMDxPRF|AmdOpenSilPkg/opensil-uefi-interface/libAMDxPRF.inf
  libAMDxSIM|AmdOpenSilPkg/opensil-uefi-interface/libF19M10xSIM.inf
  libAMDxUSL|AmdOpenSilPkg/opensil-uefi-interface/libAMDxUSL.inf
  SilEfiLib|AmdOpenSilPkg/opensil-uefi-interface/SilToUefi/SilEfiPI72.inf
  SilPeiInit|AmdOpenSilPkg/opensil-uefi-interface/Platform/SilPei.inf


[LibraryClasses.IA32.SEC]
  #####################################
  # Platform Package
  #####################################
  SetCacheMtrrLib|MinPlatformPkg/Library/SetCacheMtrrLib/SetCacheMtrrLibNull.inf
  ReportCpuHobLib|AmdCommonPkg/Library/ReportCpuHobLib/ReportCpuHobLib.inf

[LibraryClasses.common.SEC, LibraryClasses.common.PEIM, LibraryClasses.common.PEI_CORE]
  PciLib|MdePkg/Library/PeiPciLibPciCfg2/PeiPciLibPciCfg2.inf
  PciSegmentLib|MdePkg/Library/PeiPciSegmentLibPciCfg2/PeiPciSegmentLibPciCfg2.inf

[LibraryClasses.Common.DXE_DRIVER]
  SmbiosMiscLib|$(PROCESSOR_PATH)/$(PLATFORM_NAME)/Library/SmbiosMiscLib/SmbiosMiscLib.inf
  libAMDxPRF|AmdOpenSilPkg/opensil-uefi-interface/libAMDxPRF.inf
  libAMDxSIM|AmdOpenSilPkg/opensil-uefi-interface/libF19M10xSIM.inf
  libAMDxUSL|AmdOpenSilPkg/opensil-uefi-interface/libAMDxUSL.inf
  SilEfiLib|AmdOpenSilPkg/opensil-uefi-interface/SilToUefi/SilEfiPI72.inf
  SilDxeInit|AmdOpenSilPkg/opensil-uefi-interface/Platform/SilDxe.inf


[LibraryClasses.Common.DXE_CORE, LibraryClasses.Common.DXE_DRIVER, LibraryClasses.Common.DXE_SMM_DRIVER]
  TimerLib|AmdCommonPkg/Library/TscTimerLib/DxeTscTimerLib.inf
  BoardBdsHookLib|AmdBoardPkg/Library/BoardBdsHookLib/BoardBdsHookLib.inf
  BoardBootManagerLib|BoardModulePkg/Library/BoardBootManagerLib/BoardBootManagerLib.inf
  PciHostBridgeLib|Platform/GenoaOpenBoardPkg/Universal/Library/PciHostBridgeLib/PciHostBridgeLib.inf
  libAMDxSIM|AmdOpenSilPkg/opensil-uefi-interface/libF19M10xSIM.inf
  libAMDxUSL|AmdOpenSilPkg/opensil-uefi-interface/libAMDxUSL.inf
  SilEfiLib|AmdOpenSilPkg/opensil-uefi-interface/SilToUefi/SilEfiPI72.inf

[LibraryClasses.Common.DXE_SMM_DRIVER]
!if $(RUNTIME_LOGGING_ENABLE)
  DebugLib|MdePkg/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
!else
  DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
!endif

[LibraryClasses.Common.SMM_CORE]
SmmCorePlatformHookLib|AmdCommonPkg/Library/SmmCorePlatformHookLib/SmmCorePlatformHookLib.inf
!if $(RUNTIME_LOGGING_ENABLE)
  DebugLib|MdePkg/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
!else
  DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
!endif

[LibraryClasses.Common.DXE_RUNTIME_DRIVER, LibraryClasses.Common.UEFI_DRIVER]
  TimerLib|AmdCommonPkg/Library/TscTimerLib/DxeTscTimerLib.inf
  ResetSystemLib|Platform/AmdCommonPkg/Library/DxeRuntimeResetSystemLib/DxeRuntimeResetSystemLib.inf
  libAMDxSIM|AmdOpenSilPkg/opensil-uefi-interface/libF19M10xSIM.inf
  libAMDxUSL|AmdOpenSilPkg/opensil-uefi-interface/libAMDxUSL.inf
  SilEfiLib|AmdOpenSilPkg/opensil-uefi-interface/SilToUefi/SilEfiPI72.inf

[LibraryClasses.Common.DXE_RUNTIME_DRIVER]
  PciSegmentLib|MdePkg/Library/PciSegmentLibSegmentInfo/DxeRuntimePciSegmentLibSegmentInfo.inf

!if $(CAPSULE_ENABLE)
  CapsuleLib|MdeModulePkg/Library/DxeCapsuleLibFmp/DxeRuntimeCapsuleLib.inf
!endif

!if $(RUNTIME_LOGGING_ENABLE)
  DebugLib|MdePkg/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
!else
  DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
!endif

[LibraryClasses.Common.UEFI_DRIVER,LibraryClasses.Common.UEFI_APPLICATION]
  UefiShellBcfgCommandLib|ShellPkg/Library/UefiShellBcfgCommandLib/UefiShellBcfgCommandLib.inf

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

[Components.IA32]
  !include MinPlatformPkg/Include/Dsc/CorePeiInclude.dsc

  #
  # SEC Core
  #
  UefiCpuPkg/SecCore/SecCore.inf {
    <LibraryClasses>
      SecBoardInitLib|MinPlatformPkg/PlatformInit/Library/SecBoardInitLibNull/SecBoardInitLibNull.inf
  }
  #
  # PEIM
  #
  MdeModulePkg/Universal/StatusCodeHandler/Pei/StatusCodeHandlerPei.inf {
    <LibraryClasses>
    !if $(LOGGING_ENABLE)
      DebugLib|MdePkg/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
    !else
      DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
    !endif
  }
  MdeModulePkg/Universal/CapsulePei/CapsulePei.inf

  #######################################
  # Platform Package
  #######################################
  MinPlatformPkg/PlatformInit/ReportFv/ReportFvPei.inf
  MinPlatformPkg/PlatformInit/PlatformInitPei/PlatformInitPreMem.inf {
    <LibraryClasses>
      BoardInitLib|AmdBoardPkg/Library/BoardInitLib/PeiBoardInitPreMemLib.inf
  }
  MinPlatformPkg/PlatformInit/PlatformInitPei/PlatformInitPostMem.inf {
    <LibraryClasses>
      BoardInitLib|MinPlatformPkg/PlatformInit/Library/BoardInitLibNull/BoardInitLibNull.inf
  }
  
  Platform/GenoaOpenBoardPkg/Universal/AmdMemoryHobInfoPeim/AmdMemoryHobInfoPeim.inf
  
  # NVMe
  # MdeModulePkg/Bus/Pci/NvmExpressPei/NvmExpressPei.inf

  # Security
  !if gMinPlatformPkgTokenSpaceGuid.PcdTpm2Enable == TRUE
    MinPlatformPkg/Tcg/Tcg2PlatformPei/Tcg2PlatformPei.inf
  !endif

  # OPENSIL
  AmdOpenSilPkg/opensil-uefi-interface/Platform/Onyx-Genoa/Pei/SilOnyxPei.inf {
    <LibraryClasses>
      SilDataInitLib|Platform/GenoaOpenBoardPkg/OnyxBoardPkg/Library/PlatformSilDataInitLib/PlatformSilDataInitLib.inf
  }

[Components.X64]
  !include MinPlatformPkg/Include/Dsc/CoreDxeInclude.dsc
  #
  # DXE Core
  #
!if $(SOURCE_DEBUG_ENABLE)
  SourceLevelDebugPkg/DebugAgentDxe/DebugAgentDxe.inf {
    <LibraryClasses>
      DebugAgentLib|SourceLevelDebugPkg/Library/DebugAgent/DxeDebugAgentLib.inf
  }
!endif

  #
  # EDK Core modules
  #
  UefiCpuPkg/CpuDxe/CpuDxe.inf
  MdeModulePkg/Universal/SectionExtractionDxe/SectionExtractionDxe.inf

  Platform/GenoaOpenBoardPkg/Fch/SmmAccess2Dxe/SmmAccess2Dxe.inf
  Platform/GenoaOpenBoardPkg/Fch/SmmControlDxe/SmmControl.inf
  Platform/GenoaOpenBoardPkg/Fch/SmmDispatcher/SmmDispatcher.inf
  edk2-platforms/Platform/Intel/MinPlatformPkg/Acpi/AcpiSmm/AcpiSmm.inf

  #
  # SPI
  #
  AmdCommonPkg/Spi/AmdSpiHc/AmdSpiHcDxe.inf
  AmdCommonPkg/Spi/BoardSpiConfig/BoardSpiConfigDxe.inf
  AmdCommonPkg/Spi/BoardSpiBus/BoardSpiBusDxe.inf
  !if gMinPlatformPkgTokenSpaceGuid.PcdBootToShellOnly == FALSE
    AmdCommonPkg/Spi/BoardSpiConfig/BoardSpiConfigSmm.inf
    AmdCommonPkg/Spi/BoardSpiBus/BoardSpiBusSmm.inf
    AmdCommonPkg/Spi/AmdSpiHc/AmdSpiHcSmm.inf
    AmdCommonPkg/Spi/SpiNorFlashJedec/SpiNorFlashSmm.inf
    AmdCommonPkg/Spi/AmdSpiFvb/AmdSpiFvbSmm.inf
  !else
    AmdCommonPkg/Spi/SpiNorFlashJedec/SpiNorFlashDxe.inf
    AmdCommonPkg/Spi/AmdSpiFvb/AmdSpiFvbDxe.inf
  !endif

  #
  # Platform
  #
  MinPlatformPkg/PlatformInit/PlatformInitDxe/PlatformInitDxe.inf {
  <LibraryClasses>
    BoardInitLib|AmdBoardPkg/Library/DxeBoardInitLib/DxeBoardInitLib.inf
  }

  #
  # PCI
  #
  MdeModulePkg/Bus/Pci/PciHostBridgeDxe/PciHostBridgeDxe.inf
  MdeModulePkg/Bus/Pci/PciBusDxe/PciBusDxe.inf

  #
  # USB
  #
!if $(USB_SUPPORT)
  MdeModulePkg/Bus/Usb/UsbMouseDxe/UsbMouseDxe.inf
!endif

  #
  # NVME
  #
  MdeModulePkg/Bus/Pci/NvmExpressDxe/NvmExpressDxe.inf

  #
  # SATA
  #
!if $(SATA_SUPPORT)
  MdeModulePkg/Bus/Pci/SataControllerDxe/SataControllerDxe.inf
  MdeModulePkg/Bus/Ata/AtaAtapiPassThru/AtaAtapiPassThru.inf
  MdeModulePkg/Bus/Ata/AtaBusDxe/AtaBusDxe.inf
  MdeModulePkg/Bus/Scsi/ScsiBusDxe/ScsiBusDxe.inf
  MdeModulePkg/Bus/Scsi/ScsiDiskDxe/ScsiDiskDxe.inf
!endif

  #
  # SMM
  #
!if gMinPlatformPkgTokenSpaceGuid.PcdBootToShellOnly == FALSE
  UefiCpuPkg/PiSmmCpuDxeSmm/PiSmmCpuDxeSmm.inf {
    <LibraryClasses>
      SmmCpuFeaturesLib|AmdCommonPkg/Library/SmmCpuFeaturesLib/SmmCpuFeaturesLib.inf
      SmmCpuPlatformHookLib|UefiCpuPkg/Library/SmmCpuPlatformHookLibNull/SmmCpuPlatformHookLibNull.inf

    !if $(LOGGING_ENABLE)
      DebugLib|MdePkg/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
    !else
      DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
    !endif

    !if $(SOURCE_DEBUG_ENABLE)
      DebugAgentLib|SourceLevelDebugPkg/Library/DebugAgent/SmmDebugAgentLib.inf
    !endif
    <PcdsPatchableInModule>
      #
      # Disable DEBUG_CACHE because SMI entry/exit may change MTRRs
      #
      gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x801000C7
  }

  MdeModulePkg/Universal/FaultTolerantWriteDxe/FaultTolerantWriteDxe.inf
  MdeModulePkg/Universal/FaultTolerantWriteDxe/FaultTolerantWriteSmmDxe.inf
  MdeModulePkg/Universal/Variable/RuntimeDxe/VariableRuntimeDxe.inf
!else
  MdeModulePkg/Universal/FaultTolerantWriteDxe/FaultTolerantWriteDxe.inf
  MdeModulePkg/Universal/Variable/RuntimeDxe/VariableRuntimeDxe.inf
!endif

  #
  # ACPI
  #
  !if gMinPlatformPkgTokenSpaceGuid.PcdBootToShellOnly == FALSE
    AmdCommonPkg/Acpi/AcpiTables/AcpiPlatform.inf
    AmdBoardPkg/Acpi/BoardAcpiDxe/BoardAcpiDxe.inf
    AmdCommonPkg/Acpi/AcpiCommon/AcpiCommon.inf
    AmdCommonPkg/Acpi/SpcrAcpiDxe/SpcrAcpiDxe.inf {
      <LibraryClasses>
        SpcrDeviceLib|AmdCommonPkg/Library/SpcrDeviceLib/SpcrDeviceLib.inf
  }
  !endif

  #------------------------------------------------------------------------------
  # Driver to enable BMC VGA device. Obtain the proper GOP driver for the 2600
  # chip.
  #
  # Copy the "uefi_2600.efi" driver:
  # From (ZIP file): UEFI/x64/
  # To (workspace):
  # <workspace>/CrbSupportPkg/BmcGopDxe/X64/
  #------------------------------------------------------------------------------
  MdeModulePkg/Universal/Console/GraphicsConsoleDxe/GraphicsConsoleDxe.inf
  CrbSupportPkg/BmcGopDxe/BmcGopDxe.inf

  #
  # File System Modules
  #
!if gMinPlatformPkgTokenSpaceGuid.PcdPerformanceEnable == TRUE
  MdeModulePkg/Universal/FvSimpleFileSystemDxe/FvSimpleFileSystemDxe.inf
!endif

  #
  # EFI Shell
  #
  ShellPkg/Application/Shell/Shell.inf {
    <LibraryClasses>
      ShellCommandLib|ShellPkg/Library/UefiShellCommandLib/UefiShellCommandLib.inf
      NULL|ShellPkg/Library/UefiShellLevel1CommandsLib/UefiShellLevel1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellLevel2CommandsLib/UefiShellLevel2CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellLevel3CommandsLib/UefiShellLevel3CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellDriver1CommandsLib/UefiShellDriver1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellDebug1CommandsLib/UefiShellDebug1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellInstall1CommandsLib/UefiShellInstall1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellNetwork1CommandsLib/UefiShellNetwork1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellAcpiViewCommandLib/UefiShellAcpiViewCommandLib.inf
      ## NULL|ShellPkg/Library/UefiShellTftpCommandLib/UefiShellTftpCommandLib.inf
      BcfgCommandLib|ShellPkg/Library/UefiShellBcfgCommandLib/UefiShellBcfgCommandLib.inf
      FileHandleLib|MdePkg/Library/UefiFileHandleLib/UefiFileHandleLib.inf
      HandleParsingLib|ShellPkg/Library/UefiHandleParsingLib/UefiHandleParsingLib.inf
      ShellLib|ShellPkg/Library/UefiShellLib/UefiShellLib.inf

    <PcdsFixedAtBuild>
      gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0xFF
      gEfiShellPkgTokenSpaceGuid.PcdShellLibAutoInitialize|FALSE
      gEfiMdePkgTokenSpaceGuid.PcdUefiLibMaxPrintBufferSize|16000
  }

  BoardModulePkg/BoardBdsHookDxe/BoardBdsHookDxe.inf

  # LOGO
  AmdCommonPkg/Logo/LogoDxe/LogoDxe.inf

  # Security
  !if gMinPlatformPkgTokenSpaceGuid.PcdTpm2Enable == TRUE
    MinPlatformPkg/Tcg/Tcg2PlatformDxe/Tcg2PlatformDxe.inf {
      <LibraryClasses>
        TpmPlatformHierarchyLib|MinPlatformPkg/Tcg/Library/PeiDxeTpmPlatformHierarchyLib/PeiDxeTpmPlatformHierarchyLib.inf
    }
    UefiCpuPkg/MicrocodeMeasurementDxe/MicrocodeMeasurementDxe.inf
    MdeModulePkg/Universal/SmbiosMeasurementDxe/SmbiosMeasurementDxe.inf
  !endif

  !if gMinPlatformPkgTokenSpaceGuid.PcdUefiSecureBootEnable == TRUE
    AmdCommonPkg/SecureBoot/SecureBootDefaultKeysInit/SecureBootDefaultKeysInit.inf
  !endif

  Platform/GenoaOpenBoardPkg/Psp/PspInitDxe.inf

  # OPENSIL
  AmdOpenSilPkg/opensil-uefi-interface/Platform/Onyx-Genoa/Dxe/SilOnyxDxe.inf

[BuildOptions]
    GCC:*_*_*_CC_FLAGS     = -D DISABLE_NEW_DEPRECATED_INTERFACES
    INTEL:*_*_*_CC_FLAGS   = /D DISABLE_NEW_DEPRECATED_INTERFACES
    MSFT:*_*_*_CC_FLAGS    = /D DISABLE_NEW_DEPRECATED_INTERFACES

  GCC:*_*_*_CC_FLAGS     = -D USE_EDKII_HEADER_FILE

  # Turn off DEBUG messages for Release Builds
  GCC:RELEASE_*_*_CC_FLAGS     = -D MDEPKG_NDEBUG
  INTEL:RELEASE_*_*_CC_FLAGS   = /D MDEPKG_NDEBUG
  MSFT:RELEASE_*_*_CC_FLAGS    = /D MDEPKG_NDEBUG

  !ifdef $(INTERNAL_IDS)
    GCC:*_*_*_CC_FLAGS     = -D INTERNAL_IDS
    INTEL:*_*_*_CC_FLAGS   = /D INTERNAL_IDS
    MSFT:*_*_*_CC_FLAGS    = /D INTERNAL_IDS
    MSFT:*_*_*_VFRPP_FLAGS = /D INTERNAL_IDS
    MSFT:*_*_*_ASLCC_FLAGS = /D INTERNAL_IDS
    MSFT:*_*_*_ASLPP_FLAGS = /D INTERNAL_IDS
    MSFT:*_*_*_PP_FLAGS    = /D INTERNAL_IDS
    MSFT:*_*_*_APP_FLAGS   = /D INTERNAL_IDS
  !endif

  # openSIL flag
    GCC:*_*_*_CC_FLAGS     = -D OPENSIL
    INTEL:*_*_*_CC_FLAGS   = /D OPENSIL
    MSFT:*_*_*_CC_FLAGS    = /D OPENSIL

[BuildOptions.common.EDKII.DXE_RUNTIME_DRIVER, BuildOptions.common.EDKII.DXE_SMM_DRIVER, BuildOptions.common.EDKII.SMM_CORE]
  #Force modules to 4K alignment
  MSFT:*_*_*_DLINK_FLAGS = /ALIGN:4096
  GCC:*_*_*_DLINK_FLAGS = -z common-page-size=0x1000

[BuildOptions.common.EDKII.DXE_DRIVER, BuildOptions.common.EDKII.DXE_CORE, BuildOptions.common.EDKII.UEFI_DRIVER]
  #Force modules to 4K alignment
  MSFT:*_*_*_DLINK_FLAGS = /ALIGN:4096
  GCC:*_*_*_DLINK_FLAGS = -z common-page-size=0x1000
