/**
  Copyright (c) 2023, American Megatrends International LLC.
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

/**
 * @file  NbioDataInit.c
 * @brief Initialize NBIO data prior to openSIL execution.
 *
 */

#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Sil-api.h>
#include <RcMgr/DfX/RcManager4-api.h>

/**
 * FabricResourceInit
 *
 * Initialize DF resource registers for each RootBridge.
 *
 */
EFI_STATUS
PlatformFabricDataInit (
  IN OUT    DFX_RCMGR_INPUT_BLK  *DFRcmgrInputBlock
  )
{

  DFRcmgrInputBlock->SetRcBasedOnNv = FALSE;
  DFRcmgrInputBlock->SocketNumber = 1;
  DFRcmgrInputBlock->RbsPerSocket = 4;
  DFRcmgrInputBlock->McptEnable = TRUE;
  DFRcmgrInputBlock->PciExpressBaseAddress = PcdGet64 (PcdPciExpressBaseAddress);
  DFRcmgrInputBlock->BottomMmioReservedForPrimaryRb = PcdGet32 (PcdAmdBottomMmioReservedForPrimaryRb);
  DFRcmgrInputBlock->MmioSizePerRbForNonPciDevice = PcdGet32 (PcdAmdMmioSizePerRbForNonPciDevice);
  DFRcmgrInputBlock->MmioAbove4GLimit = PcdGet64 (PcdAmdMmioAbove4GLimit);
  DFRcmgrInputBlock->Above4GMmioSizePerRbForNonPciDevice = PcdGet32 (PcdAmdAbove4GMmioSizePerRbForNonPciDevice);

  DEBUG ((DEBUG_ERROR, "\n RC MGR PCDs\n"));
  DEBUG ((DEBUG_ERROR, "DFRcmgrInputBlock->PciExpressBaseAddress             = %lx\n", PcdGet64 (PcdPciExpressBaseAddress)));
  DEBUG ((DEBUG_ERROR, "DFRcmgrInputBlock->BottomMmioReservedForPrimaryRb    = %x\n", PcdGet32 (PcdAmdBottomMmioReservedForPrimaryRb)));
  DEBUG ((DEBUG_ERROR, "DFRcmgrInputBlock->MmioSizePerRbForNonPciDevice      = %x\n", PcdGet32 (PcdAmdMmioSizePerRbForNonPciDevice)));
  DEBUG ((DEBUG_ERROR, "DFRcmgrInputBlock->MmioAbove4GLimit                  = %lx\n", PcdGet64 (PcdAmdMmioAbove4GLimit)));
  DEBUG ((DEBUG_ERROR, "DFRcmgrInputBlock->Above4GMmioSizePerRbForNonPciDevice = %x\n", PcdGet32 (PcdAmdAbove4GMmioSizePerRbForNonPciDevice)));

  return EFI_SUCCESS;
}
