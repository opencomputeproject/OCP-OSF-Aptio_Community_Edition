/* Copyright 2021-2024 Advanced Micro Devices, Inc. All rights reserved. */
/**
 * @file  FchIsaReg.h
 * @brief FCH ISA sub-controller registers
 *
 *
 *
 */

#pragma once

#define ACPI_MMIO_BASE  0xFED80000ul
#define IOMUX_BASE      0xD00 // BYTE
#define SPI_BASE        0xFEC10000ul

#define FCH_LPC_BUS   0
#define FCH_LPC_DEV   0x14
#define FCH_LPC_FUNC  3
#define LPC_BUS_DEV_FUN ((FCH_LPC_BUS << 8) + (FCH_LPC_DEV << 3) + FCH_LPC_FUNC)

/* *********************************************
 * IOMUX control Registers  FCH::IOMUX::
 * *********************************************/

#define FCH_IOMUX_REG15 0x15
#define FCH_IOMUX_REG16 0x16
#define FCH_IOMUX_REG4A 0x4A
#define FCH_IOMUX_REG4B 0x4B
#define FCH_IOMUX_REG56 0x56
#define FCH_IOMUX_REG57 0x57
#define FCH_IOMUX_REG58 0x58
#define FCH_IOMUX_REG68 0x68
#define FCH_IOMUX_REG69 0x69
#define FCH_IOMUX_REG6A 0x6A
#define FCH_IOMUX_REG6B 0x6B
#define FCH_IOMUX_REG6C 0x6C
#define FCH_IOMUX_REG6D 0x6D
#define FCH_IOMUX_REG71 0x71
#define FCH_IOMUX_REG72 0x72

/* *********************************************
 * LPC sub-controller  FCH::LPCPCICFG::
 * *********************************************/

//HS only:#define FCH_LPC_REG2C 0x2C // Subsystem ID & Subsystem Vendor ID - Wo/Ro

/** IO Port/Mem Decode
 *  Reg(FCH::LPCPCICFG::IO_PORT_DECODE_ENABLE) NDA
 */
#define FCH_LPC_REG44         0x44
  //define *legacy IO*      BIT_32(0) //Bits 31:0  ///@todo disposition Internal

/**
 * Reg(FCH::LPCPCICFG::ROMADDRESSRANGE_2_START_ADDRESS) NDA
 */
#define FCH_LPC_REG6C         0x6C
  //define rom_start_address_2  BIT_32(0) // Bits 7:0 ///@todo disposition NDA

/** Misc control
 *  Reg(FCH::LPCPCICFG::MISCELLANEOUS_CONTROL_BITS) NDA
 */
#define FCH_LPC_REG78         0x78
  //define *Reserved*     BIT_32(1) ///@todo disposition Internal
  //define no_hog         BIT_32(0) ///@todo disposition Internal

/** Host Control
 *  Reg(FCH::LPCPCICFG::HOSTCONTROL) NDA
 */
#define FCH_LPC_REGBB         0xBB
  //define t_start_fix    BIT_32(3) ///@todo disposition Internal
  //define *Reserved*     BIT_32(2) ///@todo disposition NDA reserved

/** PCI Control
 *  Reg(FCH::LPCPCICFG::PCI_CONTROL) NDA
 */
#define FCH_LPC_REG40         0x40
  //define legacy_dma_enable  BIT_32(2) ///@todo disposition Internal


/** IO port Decode
 *  Reg((FCH::LPCPCICFG::IO_MEM_PORT_DECODE_ENABLE) NDA
 */
#define FCH_LPC_REG48     0x48
  //define wide_generic_io_port_enable BIT_32(2) ///@todo disposition Internal
  //define alternate_super_io_configuration_port_enable BIT_32(1) ///@todo disposition Internal
  //define super_io_configuration_port_enable BIT_32(0) ///@todo disposition Internal

/** SPI base address
 *  Reg(FCH::LPCPCICFG::SPI_BASE_ADDR) NDA
 */
#define FCH_LPC_REGA0     0xA0 //used, NDA ///@todo disposition NDA
  //define spi_espi_baseaddr  BIT_32(8) //Bits 31:8 ///@todo disposition NDA

/** EC Port address
 *  Reg(FCH::LPCPCICFG::EC_PORTADDRESS) NDA
 */
#define FCH_LPC_REGA4     0xA4
  //define *reserved*         BIT_32(0) //Bits 15:0 ///@todo disposition NDA

#define FCH_LPC_REGB8     0x0B8

/** EC Control
 * Reg(FCH::LPCPCICFG::ECCONTROL) NDA
 */
#define FCH_LPC_REGBA     0xBA
  //define *reserved*         BIT_32(0) //Bits 7:0  ///@todo disposition Internal



/** Clock Pin Control
 * Reg(FCH::LPCPCICFG::CLKPINCNTRL) NDA
 */
#define FCH_LPC_REGD0 0xD0
  //define lclk1en            BIT_32(6) //INT @todo INT Reg Disposition
  //define lclk0en            BIT_32(5) //INT @todo INT Reg Disposition



/* *********************************************
 * SPI sub-controller FCH::LPCHOSTSPIREG::
 * *********************************************/
#define FCH_SPI_MMIO_REG00          0x00  ///@todo Reserved reg in SN
/** SPI control 1
 *  Reg(FCH::LPCHOSTSPIREG::SPI_CNTRL1_REGISTER) Pub
 */
#define FCH_SPI_MMIO_REG0C          0x0C
  //define waitcount          BIT_32(16) ///@todo disposition INT Bits 21:16

#define FCH_SPI_MMIO_REG1D                               0x1D 

/** SPI100 Enable
 * Reg(FCH::LPCHOSTSPIREG::SPI100ENABLE_REGISTER) Pub
 */
#define FCH_SPI_MMIO_REG20                               0x20
  //define normspeed           BIT_32(28)  //Pub  Bits 31:28
  #define FCH_SPI_NORMAL_SPEED_BIT_OFFSET    28
  //define altspeednew         BIT_32(20)  //Pub  Bits 23:20
  #define FCH_SPI_WRITE_SPEED_BIT_OFFSET     20
  //define tpmspeed            BIT_32(16)  //Pub  Bits 19:16
  #define FCH_SPI_TPM_SPEED_BIT_OFFSET       16
  //define usespi100           BIT_32(0)   //Pub

#define FCH_SPI_MMIO_REG22                               0x22

/** SPI Host Prefetch
 * Reg(FCH::LPCHOSTSPIREG::SPI100_HOST_PREFETCH_CONFIG_REGISTER) Pub
 */
#define FCH_SPI_MMIO_REG2C                               0x2C
  //define rd4dw_en_host        BIT_32(15) //Pub //DMA burst enable
  //define hostbursten          BIT_32(14) //INT

//TODO AMI OVERRIDE --- defines used in AmdSpiHc driver

#define FCH_SPI_MMIO_REG45_CMDCODE                       0x45
#define FCH_SPI_MMIO_REG47_CMDTRIGGER                    0x47
#define FCH_SPI_MMIO_REG48_TXBYTECOUNT                   0x48
#define FCH_SPI_MMIO_REG4B_RXBYTECOUNT                   0x4B
#define FCH_SPI_MMIO_REG4C_SPISTATUS                     0x4C
  #define FCH_SPI_BUSY                                     0x80000000l

#define FCH_SPI_MMIO_REG80_FIFO                          0x80
  #define FCH_SPI_FIFO_PTR_CRL                             0x00100000l

//HS only: #define FCH_SPI_MMIO_REG1C  0x1C
//#define FCH_SPI_MMIO_REG22                               0x22
//#define FCH_SPI_MMIO_REG32_SPI100_DUMMY_CYCLE_CONFIG     0x32
//#define FCH_SPI_MMIO_REG58_ADDR32_CTRL2                  0x58
//#define FCH_SPI_MODE_QUAL_114                            0x3
//#define FCH_SPI_MODE_QUAL_122                            0x4
//#define FCH_SPI_MODE_QUAL_144                            0x5
//#define FCH_SPI_BUSY                                     0x80000000l
//#define FCH_SPI_EXEC_OPCODE                              0x00010000l
//#define FCH_SPI_MMIO_REG1F_X05_TX_BYTE_COUNT             0x05
//#define FCH_SPI_MMIO_REG1F_X06_RX_BYTE_COUNT             0x06
//#define FCH_SPI_MMIO_REG1E                               0x1E
//#define FCH_SPI_MMIO_REG1F                               0x1F
//#define FCH_SPI_RETRY_TIMES                              0x3
//#define FCH_SPI_MMIO_REG80_FIFO                          0x80
//#define FCH_SPI_MMIO_REG50_ADDR32_CTRL0                  0x50
//#define FCH_SPI_DEVICE_COMPLETION_STATUS                 0x1
//#define FCH_SPI_DEVICE_WRITE_ENABLED                     0x2
//#define FCH_SPI_ROM_ADDR_32                              0x1
//#define FCH_SPI_MANUFACTURER_ID_JEDEC                    0x20
//#define FCH_SPI_DUMMY_CYCLE_COUNT                        0x8A

/* *********************************************
 * FCH AOAC sub-controller  FCH::AOAC::
 * *********************************************/
//#define FCH_AOAC_LPC 0x04
