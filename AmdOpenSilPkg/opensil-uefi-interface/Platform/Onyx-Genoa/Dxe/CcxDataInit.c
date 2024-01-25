/**
 * @file  CcxDataInit.c
 * @brief The Host will locate a structure within the memory block 
 *        that was assigned by an IP block
 *        Allocates the Secure Nested Paging (SNP) for Reverse Map Table (RMP)
 *
 */
/**
 * Copyright 2021-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 */

#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Sil-api.h>
#include <CcxClass-api.h>
#include <xPRF-api.h>
#include <Uefi/UefiSpec.h>
#include <Uefi/UefiBaseType.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/MpService.h>
#include <Library/BaseLib.h>
#include <MsrReg.h>

#pragma pack (push, 1)

/// AP MSR sync up
typedef struct {
  IN  UINT32 MsrAddr;     ///< MSR address
  IN  UINT64 MsrData;     ///< MSR Settings
  IN  UINT64 MsrMask;     ///< MSR mask
} AP_MSR_SYNC;

#pragma pack (pop)


VOID EFIAPI CcxZen4InitWithMpServices (EFI_EVENT, VOID*);
VOID EFIAPI CcxZen4SyncRmpMsrs (VOID*);

AP_MSR_SYNC          RmpMsrSyncTable[] =
{
  { 0xC0010132, 0x0000000000000000, 0xFFFFFFFFFFFFFFFF  },
  { 0xC0010133, 0x0000000000000000, 0xFFFFFFFFFFFFFFFF  },
  { MSR_SYS_CFG, 0x0000000000000000, 0xFFFFFFFFFFFFFFFF  },
};

/**
 * CcxAllocateMemoryToRMPTable
 *
 * @brief   Allocate Secure Nested Paging for Reverse Map Table
 *
 */
void
CcxAllocateMemoryToRMPTable (
 void
 )
{

  EFI_PHYSICAL_ADDRESS    RmpTableBase;
  EFI_STATUS              Status;
  EFI_PHYSICAL_ADDRESS    AlignedTableAddress ;
  CCXCLASS_DATA_BLK       *CcxData;

  RmpTableBase        = 0;
  AlignedTableAddress = 0;

  CcxData = (CCXCLASS_DATA_BLK *)SilFindStructure (SilId_CcxClass,  0);
  if (CcxData == NULL) {
    DEBUG ((DEBUG_ERROR, "OpenSIL DXE Driver Could not found the block memory\n"));
    return; // Could not find the IP input block
  }

  if((CcxData->CcxOutputBlock.AmdIsSnpSupported) &&
     (CcxData->CcxInputBlock.AmdSnpMemCover != 0)) {

    RmpTableBase = (EFI_PHYSICAL_ADDRESS)CcxData->CcxOutputBlock.AmdRmpTableBase;
	// find enough memory for RMP table to fit on MB boundary
    Status = gBS->AllocatePages (AllocateMaxAddress,
                                 EfiReservedMemoryType,
                                 (UINTN)(EFI_SIZE_TO_PAGES (CcxData->CcxOutputBlock.AmdRmpTableSize + SIZE_1MB)),
                                 &RmpTableBase
                                );
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "[ERROR] Failed to allocate RMP Table.\n"));
      return;
    }

    // align on MB boundary
    AlignedTableAddress = (RmpTableBase + SIZE_1MB - 1) & ~(SIZE_1MB - 1);

    // free pages before reallocating on MB boundary
    gBS->FreePages (RmpTableBase, (UINTN)(EFI_SIZE_TO_PAGES (CcxData->CcxOutputBlock.AmdRmpTableSize + SIZE_1MB)));

    // reserve memory for RMP Table on MB boundary
    Status = gBS->AllocatePages (AllocateAddress,
                                 EfiReservedMemoryType,
                                 (UINTN)(EFI_SIZE_TO_PAGES (CcxData->CcxOutputBlock.AmdRmpTableSize)),
                                 &AlignedTableAddress
                                );

    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "[ERROR] Try to allocate RMP Table at 0x%x but failed.\n",AlignedTableAddress));
      return;
    }

    // zero out RMP table
    gBS->SetMem ((VOID *) AlignedTableAddress,
                 (UINTN)(CcxData->CcxOutputBlock.AmdRmpTableSize),
                 0
                );

    xPrfSetSnpRmp(AlignedTableAddress,CcxData->CcxOutputBlock.AmdRmpTableSize);
  }
  else {
    DEBUG ((DEBUG_ERROR, "AmdIsSnpSupported not supported\n"));
  }
}


EFI_STATUS
CcxDxeInit (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  UINT64                                   ApicBar;

  DEBUG ((DEBUG_INFO, "CCX DXE Init.\n"));

  // is this BSP core?
  ApicBar = AsmReadMsr64 (MSR_APIC_BAR);
  if ((ApicBar & BIT8) == 0) return EFI_SUCCESS;  // not BSP

  DEBUG ((DEBUG_INFO, "Is BSP.\n"));

  CcxAllocateMemoryToRMPTable ();
  CcxZen4InitWithMpServices (NULL, NULL);

  return EFI_SUCCESS;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  CcxZen4InitWithMpServices
 *
 *  @param[in] Event        The event that invoked this routine
 *  @param[in] Context      Unused
 *
 *  Description:
 *    This routine runs necessary routines across all APs
 *
 */
VOID
EFIAPI
CcxZen4InitWithMpServices (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  EFI_STATUS                Status;
  EFI_MP_SERVICES_PROTOCOL  *MpServices;
  UINTN                     i;

  Status = gBS->LocateProtocol (&gEfiMpServiceProtocolGuid, NULL, (VOID **)&MpServices);
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "ERROR: MP services protocol is not located.\n"));
    return;
  }

  DEBUG ((DEBUG_INFO, "  CcxZen4InitWithMpServices Entry\n"));

  for (i = 0; i < (sizeof (RmpMsrSyncTable) / sizeof (RmpMsrSyncTable[0])); i++) {
    RmpMsrSyncTable[i].MsrData = AsmReadMsr64 (RmpMsrSyncTable[i].MsrAddr);
  }
  MpServices->StartupAllAPs (
      MpServices,
      CcxZen4SyncRmpMsrs,
      FALSE,
      NULL,
      0,
      NULL,
      NULL
  );
}


/* -----------------------------------------------------------------------------*/
/**
 *
 *  CcxZen4SyncRmpMsrs
 *
 *  Description:
 *    This routine synchronizes the MSRs in RmpMsrSyncTable across all APs
 *
 */
VOID
EFIAPI
CcxZen4SyncRmpMsrs (
  IN       VOID  *Void
  )
{
  UINTN  i;

  for (i = 0; i < (sizeof (RmpMsrSyncTable) / sizeof (RmpMsrSyncTable[0])); i++) {
    AsmMsrAndThenOr64 (
        RmpMsrSyncTable[i].MsrAddr,
        ~(RmpMsrSyncTable[i].MsrMask),
        (RmpMsrSyncTable[i].MsrData & RmpMsrSyncTable[i].MsrMask)
        );
  }
}

