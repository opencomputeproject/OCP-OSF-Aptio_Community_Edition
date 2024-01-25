/**
 * @file  xPrfServicesSmm.c
 * @brief xPRF services protocol functions for SMM.
 */
/**
 * Copyright 2021-2023 Advanced Micro Devices, Inc. All rights reserved.
 *
 */
#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Sil-api.h>
#include <xPRF-api.h>
#include <SilSmm.h>
#include "xPrfServicesSmm.h"


extern EFI_GUID gOpenSilxPrfSmmProtocolGuid;

/*
 * openSIL xPRF services protocol
 */
AMD_OPENSIL_XPRF_SMM_SERVICES_PROTOCOL   mOpenSilXprfSmmProtocol = {
  .McaErrorAddrTranslate        = SilMcaErrorAddrTranslate,
  .TranslateSysAddrToCS         = SilTranslateSysAddrToCS,
  .GetLocalSmiStatus            = SilGetLocalSmiStatus,
  .CollectMcaErrorInfo          = SilCollectMcaErrorInfo,
  .TranslateSysAddrToDpa        = SilTranslateSysAddrToDpa,
  .UpdateDimmFruTextToMca       = SilUpdateDimmFruTextToMca,
  .SetIpMcaCtlMask              = SilSetIpMcaCtlMask
};

/**
 * SilMcaErrorAddrTranslate
 *
 * @brief   Translate Unified Memory Controller (UMC) local address into
 *          specific memory DIMM information and system address.
 *
 * @param   SystemMemoryAddress Pointer to return the calculated system address
 * @param   NormalizedAddress   UMC memory address Information passed in from
 *                              Host.
 * @param   DimmInfo            Point to a buffer to populate translated
 *                              normalized address data. Host is responsible
 *                              for ensuring the buffer size is sufficient to
 *                              contain SIL_DIMM_INFO (defined in
 *                              RasClass-api.h).
 * @param   AddrData            Dimm information map, created by Host call to
 *                              xPrfCollectDimmMap, used in address translation
 *
 * @return  EFI_STATUS
 *
 */
EFI_STATUS
SilMcaErrorAddrTranslate (
  UINT64                  *SystemMemoryAddress,
  SIL_NORMALIZED_ADDRESS  *NormalizedAddress,
  SIL_DIMM_INFO           *DimmInfo,
  SIL_ADDR_DATA           *AddrData
  )
{
  SIL_STATUS  SilStatus;

  SilStatus = xPrfMcaErrorAddrTranslate (
    SystemMemoryAddress,
    NormalizedAddress,
    DimmInfo,
    AddrData
    );

  // Translate the SIL_STATUS and return it
  return TranslateStatus (SilStatus);
}

/**
 * SilTranslateSysAddrToCS
 *
 * @brief Translate system address into specific memory DIMM information and
 *        normalized address information
 *
 * @param  SystemMemoryAddress  System Address
 * @param  NormalizedAddress    UMC memory address Information
 * @param  DimmInfo             DIMM information
 * @param  AddrData             Dimm information map used in address translation
 *
 * @return EFI_STATUS
 *
 */
EFI_STATUS
SilTranslateSysAddrToCS (
  UINT64                  *SystemMemoryAddress,
  SIL_NORMALIZED_ADDRESS  *NormalizedAddress,
  SIL_DIMM_INFO           *DimmInfo,
  SIL_ADDR_DATA           *AddrData
  )
{
  SIL_STATUS  SilStatus;

  SilStatus = xPrfTranslateSysAddrToCS (
    SystemMemoryAddress,
    NormalizedAddress,
    DimmInfo,
    AddrData
    );
  return TranslateStatus (SilStatus);
}

/**
 * SilGetLocalSmiStatus
 *
 * @brief   Get the Local Smi Status from the SMM save state area.
 *
 * @details The top 512 bytes (FE00h to FFFFh) of SMRAM memory space are the
 *          default SMM state-save area. When an SMI occurs, the processor saves
 *          its state in the 512-byte SMRAM state-save area during the control
 *          transfer into SMM.
 *
 * @note  ***This function is expected to be executed on a specific processor
 *        by the Host in a multi-processor environment.
 *
 * @param  LocalSmiStatusData Pointer to buffer to return LocalSmiStatus.
 *
 */
VOID
SilGetLocalSmiStatus (
  VOID *LocalSmiStatusData
  )
{
  XPRF_GET_LOCAL_SMI_STATUS_DATA  *TempData;
  TempData = (XPRF_GET_LOCAL_SMI_STATUS_DATA  *)LocalSmiStatusData;
  TempData->LocalSmiStatus = xPrfGetLocalSmiStatus ();
}

/**
 * SilCollectMcaErrorInfo
 *
 * @brief   Function for performing a search for MCA errors through all banks on
 *          a specific thread.
 *
 * @details It is the responsibility of the Host to ensure the input buffer is
 *          sufficient to contain the SIL_RAS_MCA_ERROR_INFO_V2 (defined in
 *          RasClass-api.h).
 *
 * @note    ***This function is executed on all processors by the Host in a
 *          multi-processor environment.
 *
 * @param   SilRasMcaErrorInfo Input pointer to a buffer that will be populated
 *                             with SIL_RAS_MCA_ERROR_INFO_V2.
 */
VOID
SilCollectMcaErrorInfo (
  VOID *SilRasMcaErrorInfo
  )
{
  xPrfCollectMcaErrorInfo (SilRasMcaErrorInfo);
}

/**
 * SilTranslateSysAddrToDpa
 *
 * @brief   System Address Translate to DIMM Physical Address (DPA)
 *
 * @details Translate system address into specific memory DIMM information and
 *          normalized address information
 *
 * @param SystemMemoryAddress System Address input
 * @param Dpa                 DIMM Physical Address to output
 * @param AddrData            Dimm information map input used in address
 *                            translation
 *
 * @return EFI_STATUS
 *
 * @retval EFI_NOT_FOUND      RAS API was not found in the API list
 * @retval EFI_SUCCESS        Address translated successfully
 */
EFI_STATUS
SilTranslateSysAddrToDpa (
  UINT64        *SystemMemoryAddress,
  UINT64        *Dpa,
  SIL_ADDR_DATA *AddrData
  )
{
  SIL_STATUS  SilStatus;

  SilStatus = xPrfTranslateSysAddrToDpa (
    SystemMemoryAddress,
    Dpa,
    AddrData
    );
  return TranslateStatus (SilStatus);
}

/**
 * SilUpdateDimmFruTextToMca
 *
 * @brief   This function provides the service to add Field Replaceable Unit
 *          (FRU) text to UMC MCA bank
 *
 * @param   DimmFruTextTable Input of the Dimm Fru Text table.  Host is expected
 *                           to provide the table in a format compatible with
 *                           SIL_DIMM_FRUTEXT_TABLE (defined in RasClass-api.h)
 *
 * @return EFI_STATUS
 *
 */
EFI_STATUS
SilUpdateDimmFruTextToMca (
  SIL_DIMM_FRUTEXT_TABLE   *DimmFruTextTable
  )
{
  SIL_STATUS  SilStatus;

  SilStatus = xPrfUpdateDimmFruTextToMca (DimmFruTextTable);
  return TranslateStatus (SilStatus);
}

/**
 * SilSetIpMcaCtlMask
 *
 * @brief   Set the MCA_CTL_MASK for a given IP based on platform configuration.
 *
 * @details Set the IP MCA_CTL_MASK based on an input buffer provided by the host
 *          firmware. The buffer contains a SIL_MCA_CTL_MASK_DATA with the following
 *          data:
 *            - HardwareId   Hardware ID input to match with MCA Bank MCA IP ID.
 *            - McaType      MCA Type input to match with MCA Bank MCA IP ID.
 *            - IpRasPolicy  Input containing platform policy information that is
 *                           used to configure MCA Control mask settings. The host
 *                           is responsible for populating the appropriate IP structure
 *                           with valid policy information.
 *
 * @param Buffer  A pointer to an input buffer from the host firmware containing a SIL_MCA_CTL_MASK_DATA
 *                structure.
 *
 */
void
SilSetIpMcaCtlMask (
  void  *Buffer
  )
{
  SIL_STATUS            Status;
  SIL_MCA_CTL_MASK_DATA *McaCtlMaskData;

  McaCtlMaskData = (SIL_MCA_CTL_MASK_DATA *)Buffer;
  Status = xPrfSetIpMcaCtlMask (McaCtlMaskData->HardwareId,
                                McaCtlMaskData->McaType,
                                McaCtlMaskData->IpRasPolicy
                                );
}

/**
 * xPrfServicesSmmProtocolInstall
 *
 * @brief   Installed xPrfServicesProtocol for DXE
 *
 * @todo    Install xPrfServicesProtocol for SMM as well.
 *
 * @return  EFI_STATUS
 */
EFI_STATUS
xPrfServicesSmmProtocolInstall (VOID)
{
  EFI_HANDLE        Handle;
  EFI_STATUS        Status;

  DEBUG ((DEBUG_INFO, "Installing xPrfServicesSmmProtocol\n"));

  /*
   * Install openSIL xPRF services protocol.  Also needs SMM protocol for RAS.
   */
  Handle = NULL;
  Status = gSmst->SmmInstallProtocolInterface (
                &Handle,
                &gOpenSilxPrfSmmProtocolGuid,
                EFI_NATIVE_INTERFACE,
                &mOpenSilXprfSmmProtocol
            );
  ASSERT_EFI_ERROR (Status);

  return Status;
}