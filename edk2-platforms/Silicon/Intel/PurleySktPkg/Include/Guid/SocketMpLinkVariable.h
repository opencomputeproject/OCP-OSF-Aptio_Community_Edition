/** @file

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef   __SOCKET_MP_LINK_CONFIG_DATA_H__
#define   __SOCKET_MP_LINK_CONFIG_DATA_H__

#include <UncoreCommonIncludes.h>
#include "SocketConfiguration.h"

extern EFI_GUID gEfiSocketMpLinkVariableGuid;
#define SOCKET_MP_LINK_CONFIGURATION_NAME L"SocketMpLinkConfig"

#pragma pack(1)
typedef struct {
  // SKXTODO: rename to Kti when removing HSX code
  UINT8  QpiSetupNvVariableStartTag;  // This must be the very first one of the whole KTI Setup NV variable!

  //
  // Used by the PciHostBridge DXE driver, these variables don't need to be exposed through setup options
  // The variables are used as a communication vehicle from the PciHostBridge DXE driver to an OEM hook
  // which updates the KTI resource map
  //
  //
  //  KTI host structure inputs
  //
  UINT8  BusRatio[MAX_SOCKET];
  UINT8  LegacyVgaSoc;         // Socket that claims the legacy VGA range; valid values are 0-3; 0 is default.
  UINT8  LegacyVgaStack;       // Stack that claims the legacy VGA range; valid values are 0-3; 0 is default.
  UINT8  MmioP2pDis;           // 1 - Disable; 0 - Enable
  UINT8  DebugPrintLevel;      // Bit 0 - Fatal, Bit1 - Warning, Bit2 - Info Summary; Bit 3 - Info detailed. 1 - Enable; 0 - Disable
  UINT8  DegradePrecedence;    // Use DEGRADE_PRECEDENCE definition; TOPOLOGY_PRECEDENCE is default

  //
  // Phy/Link Layer Options
  //
  UINT8  QpiLinkSpeedMode;         // Link speed mode selection; 0 - Slow Speed; 1- Full Speed
  UINT8  QpiLinkSpeed;             // One of SPEED_REC_96GT, SPEED_REC_104GT, MAX_KTI_LINK_SPEED (default), FREQ_PER_LINK
  UINT8  KtiLinkL0pEn;             // 0 - Disable, 1 - Enable, 2- Auto (default)
  UINT8  KtiLinkL1En;              // 0 - Disable, 1 - Enable, 2- Auto (default)
  UINT8  KtiFailoverEn;            // 0 - Disable, 1 - Enable, 2- Auto (default)
  UINT8  KtiLbEn;                  // 0 - Disable(default), 1 - Enable
  UINT8  KtiCrcMode;               // 0 - 8 bit CRC 1 - 16 bit CRC Mode
  UINT8  QpiCpuSktHotPlugEn;       // 0 - Disable (default), 1 - Enable
  UINT8  KtiCpuSktHotPlugTopology; // 0 - 4S Topology (default), 1 - 8S Topology
  UINT8  KtiSkuMismatchCheck;      // 0 - No, 1 - Yes (default)
  UINT8  KtiLinkVnaOverride;       // 0x100 - per link, 0xff - max (default), 0x00 - min
  UINT8  SncEn;                    // 0 - Disable (default), 1 - Enable
  UINT8  IoDcMode;                 // 0 - Disable IODC,  1 - AUTO (default), 2 - IODC_EN_REM_INVITOM_PUSH, 3 - IODC_EN_REM_INVITOM_ALLOCFLOW
                                   // 4 - IODC_EN_REM_INVITOM_ALLOC_NONALLOC, 5 - IODC_EN_REM_INVITOM_AND_WCILF
  UINT8  DirectoryModeEn;          // 0 - Disable; 1 - Enable (default)
  UINT8  XptPrefetchEn;            // XPT Prefetch :  1 - Enable (Default); 0 - Disable
  UINT8  KtiPrefetchEn;            // KTI Prefetch :  1 - Enable (Default); 0 - Disable
  UINT8  RdCurForXptPrefetchEn;    // RdCur for XPT Prefetch :  0 - Disable, 1 - Enable, 2- Auto (default)
  UINT8  IrqThreshold;             // KTI IRQ Threshold setting
  UINT8  TscSyncEn;                // TSC Sync Enable: 0 - Disable; 1 - Enable; 2 - AUTO (default)
  UINT8  StaleAtoSOptEn;           // HA A to S directory optimization
  UINT8  LLCDeadLineAlloc;         // Never fill dead lines in LLC: 1 - Enable, 0 - Disable

  //
  // KTI DFX variables
  //
  UINT8  DfxSystemWideParmStart;       // This must be the first DFX variable
  UINT8  DfxHaltLinkFailReset;         // 2 - Auto; 1 - Enable; 0 - Disable
  UINT8  DfxKtiMaxInitAbort;           // 0 - Disable; 1 - Enable; 2 - AUTO (default)
  UINT8  DfxLlcShareDrdCrd;            // Enable migration from SF to LLC and to leave shared lines in the LLC for Drd and Crd: 1 - Enable; 1 - Disable; 2 - Auto
  UINT8  DfxBiasFwdMode;               // 0 - Mode 0 (Fwd only when Hom != Req); 1 - Mode 1 (Fwd when Hom != Req & Hom != Local); 2 - Mode 2 (Disable Bias Fwd)
  UINT8  DfxSnoopFanoutEn;             // snoop fanout enable 0: disable 1: enable 2 - Auto (default) (EX only)
  UINT8  DfxHitMeEn;                   // CHA HitME$ Enable: 1 - Enable; 0 - Disable; 2 - Auto (Default)
  UINT8  DfxFrcfwdinv;                 // Enable alias all conflict flows to FwdInvItoE behaviour: 1 - Enable; 0 - Disable; 2 - Auto (default)
  UINT8  DfxDbpEnable;                 // Dbp Enable : 1 - Enable; 0 - Disable; 2 - Auto (default)
  UINT8  DfxOsbEn;                     // OSB Enable: 1 - Enable; 0 - Disable; 2 - Auto (default)
  UINT8  DfxHitMeRfoDirsEn;            // Enable HitME DIR=S RFO optimization: 1 - Enable; 0 - Disable; 2 - Auto (default)
  UINT8  DfxGateOsbIodcAllocEn;        // When OSB indicates that there aren't enough snoop credits don't allocate IODC entry: 1 - Enable; 0 - Disable; 2 - Auto (default)
  UINT8  DfxDualLinksInterleavingMode; // In 2S 2KTI can: 2 - Auto - do nothing (default); 1 - use legacy CHA interleaving (disable SNC, turn off XOR interleave); 0 - disable D2C
  UINT8  DfxSystemDegradeMode;         // 0 - Degrade to 1S; 1 - Degarde to supported topology (default); 2 - Leave the topology as is.
  UINT8  DfxVn1En;                     // VN1 enable 0: disable 1: enable 2 - Auto (default) (EX only)
  UINT8  DfxD2cEn;                     // Direct To Core enable: 1 - Enable; 0 - Disable; 2 - Auto (default)
  UINT8  DfxD2kEn;                     // Direct To Kti enable: 1 - Enable; 0 - Disable; 2 - Auto (default)
  UINT8  DfxSystemWideParmEnd;         // This must be the last DFX variable


#define  CSICPUPRTVARIABLE(x)       x##KtiPortDisable;x##KtiLinkSpeed;x##KtiLinkVnaOverride;

  UINT8 KtiCpuPerPortStartTag;
  CSICPUPRTVARIABLE(UINT8 Cpu0P0)
  CSICPUPRTVARIABLE(UINT8 Cpu0P1)
  CSICPUPRTVARIABLE(UINT8 Cpu0P2)
#if MAX_SOCKET > 1
  CSICPUPRTVARIABLE(UINT8 Cpu1P0)
  CSICPUPRTVARIABLE(UINT8 Cpu1P1)
  CSICPUPRTVARIABLE(UINT8 Cpu1P2)
#endif
#if MAX_SOCKET > 2
  CSICPUPRTVARIABLE(UINT8 Cpu2P0)
  CSICPUPRTVARIABLE(UINT8 Cpu2P1)
  CSICPUPRTVARIABLE(UINT8 Cpu2P2)
#endif
#if MAX_SOCKET > 3
  CSICPUPRTVARIABLE(UINT8 Cpu3P0)
  CSICPUPRTVARIABLE(UINT8 Cpu3P1)
  CSICPUPRTVARIABLE(UINT8 Cpu3P2)
#endif
#if (MAX_SOCKET > 4)
  CSICPUPRTVARIABLE(UINT8 Cpu4P0)
  CSICPUPRTVARIABLE(UINT8 Cpu4P1)
  CSICPUPRTVARIABLE(UINT8 Cpu4P2)
#endif
#if (MAX_SOCKET > 5)
  CSICPUPRTVARIABLE(UINT8 Cpu5P0)
  CSICPUPRTVARIABLE(UINT8 Cpu5P1)
  CSICPUPRTVARIABLE(UINT8 Cpu5P2)
#endif
#if (MAX_SOCKET > 6)
  CSICPUPRTVARIABLE(UINT8 Cpu6P0)
  CSICPUPRTVARIABLE(UINT8 Cpu6P1)
  CSICPUPRTVARIABLE(UINT8 Cpu6P2)
#endif
#if (MAX_SOCKET > 7)
  CSICPUPRTVARIABLE(UINT8 Cpu7P0)
  CSICPUPRTVARIABLE(UINT8 Cpu7P1)
  CSICPUPRTVARIABLE(UINT8 Cpu7P2)
#endif

#define CSICPUPRTDFXVARIABLE(x)    x##DfxCrcMode;x##DfxL0pEnable;x##DfxL1Enable;x##DfxKtiFailoverEn;

  UINT8 DfxKtiCpuPerPortStartTag;
  CSICPUPRTDFXVARIABLE(UINT8 Cpu0P0)
  CSICPUPRTDFXVARIABLE(UINT8 Cpu0P1)
  CSICPUPRTDFXVARIABLE(UINT8 Cpu0P2)
#if MAX_SOCKET > 1
  CSICPUPRTDFXVARIABLE(UINT8 Cpu1P0)
  CSICPUPRTDFXVARIABLE(UINT8 Cpu1P1)
  CSICPUPRTDFXVARIABLE(UINT8 Cpu1P2)
#endif
#if MAX_SOCKET > 2
  CSICPUPRTDFXVARIABLE(UINT8 Cpu2P0)
  CSICPUPRTDFXVARIABLE(UINT8 Cpu2P1)
  CSICPUPRTDFXVARIABLE(UINT8 Cpu2P2)
#endif
#if MAX_SOCKET > 3
  CSICPUPRTDFXVARIABLE(UINT8 Cpu3P0)
  CSICPUPRTDFXVARIABLE(UINT8 Cpu3P1)
  CSICPUPRTDFXVARIABLE(UINT8 Cpu3P2)
#endif
#if MAX_SOCKET > 4
  CSICPUPRTDFXVARIABLE(UINT8 Cpu4P0)
  CSICPUPRTDFXVARIABLE(UINT8 Cpu4P1)
  CSICPUPRTDFXVARIABLE(UINT8 Cpu4P2)
#endif
#if MAX_SOCKET > 5
  CSICPUPRTDFXVARIABLE(UINT8 Cpu5P0)
  CSICPUPRTDFXVARIABLE(UINT8 Cpu5P1)
  CSICPUPRTDFXVARIABLE(UINT8 Cpu5P2)
#endif
#if MAX_SOCKET > 6
  CSICPUPRTDFXVARIABLE(UINT8 Cpu6P0)
  CSICPUPRTDFXVARIABLE(UINT8 Cpu6P1)
  CSICPUPRTDFXVARIABLE(UINT8 Cpu6P2)
#endif
#if MAX_SOCKET > 7
  CSICPUPRTDFXVARIABLE(UINT8 Cpu7P0)
  CSICPUPRTDFXVARIABLE(UINT8 Cpu7P1)
  CSICPUPRTDFXVARIABLE(UINT8 Cpu7P2)
#endif

  UINT8  QpiSetupNvVariableEndTag;  // This must be the last one of the whole KTI Setup NV variable
} SOCKET_MP_LINK_CONFIGURATION;

#pragma pack()

#endif // __SOCKET_MP_LINK_CONFIG_DATA_H__

