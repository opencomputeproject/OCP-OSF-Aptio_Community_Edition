/**
 * @file  xPrfServicesDxe.c
 * @brief xPRF services protocol functions.
 *
 */
/*
 * Copyright 2021-2024 Advanced Micro Devices, Inc. All rights reserved.
 *
 */

#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Uefi/UefiBaseType.h>
#include <Uefi/UefiSpec.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <Sil-api.h>
#include <xPRF-api.h>
#include "xPrfServicesDxe.h"
#include <Include/xPrfServicesDxeProtocol.h>
#include <SilDxe.h>

extern EFI_GUID gOpenSilxPrfProtocolGuid;

/*
 * openSIL xPRF services protocol
 */
AMD_OPENSIL_XPRF_SERVICES_PROTOCOL   mOpenSilXprfProtocol = {
// RAS xPRF Services Start
  SilCollectDimmMap,
  SilGetMaxCpus,
  SilCollectCpuMap,
  SilCollectCoreMcaInfo,
  SilCollectMcaErrorInfo,
  SilMcaErrorAddrTranslate,
  SilTranslateSysAddrToCS,
  SilCoreMcaIpIdInstanceIdInit,
  SilProgramCoreMcaConfigUmc,
// RAS xPRF Services End
  SilGetThreadsPerCore,
  SilGetPStatePower,
  SilCreateSratApicEntry,
  SilPStateGatherData,
  SilGetCratHsaProcInfo,
  SilCratCacheEntry
};

/*
 * xPRF services protocol wrapper functions that call into xPRF.  These wrappers
 * are populated into the xPRF services protocol.
 *
 * These wrapper functions should translate any openSIL output to UEFI output.
 */
/**
 * SilCollectDimmMap
 *
 * @brief   xPRF protocol wrapper function for openSIL xPrfSilCollectDimmMap
 *
 * @details This wrapper function calls xPRF and translates the output if
 *          necessary.
 *
 * @param   DimmMap     Input buffer for the Dimm Map.  The Host is responsible
 *                      for ensuring the buffer size matches with openSIL and
 *                      that the input and output structures match.
 *
 * @return  EFI_STATUS  The status from openSIL translated into UEFI format.
 *
 */
EFI_STATUS
SilCollectDimmMap (
  SIL_ADDR_DATA *DimmMap
)
{
  SIL_STATUS SilStatus;

  DEBUG ((DEBUG_INFO, "Calling xPRF Service xPrfCollectDimmMap\n"));
  SilStatus = xPrfCollectDimmMap (DimmMap);

  return TranslateStatus (SilStatus);
}

/**
 * SilGetMaxCpus
 *
 * @brief   xPRF protocol wrapper function for openSIL xPrfGetMaxCpus
 *
 * @details This wrapper function calls xPRF and translates the output if
 *          necessary.
 *
 * @return  UINT32 The maximum possible CPUs in the system
 */
UINT32
SilGetMaxCpus (VOID)
{
  // Call to xPRF service
  return xPrfGetMaxCpus ();
}

/**
 * SilCollectCpuMap
 *
 * @brief   xPRF protocol wrapper function for openSIL GetMaxCpus
 *
 * @details This wrapper function calls xPRF and translates the output if
 *          necessary.
 * @param   RasCpuMap   On input, the pointer to the CPU map structure.  It is
 *                      the responsibility of the host to allocate sufficient
 *                      memory for this structure.
 *                      On output, the buffer is filled with instances of
 *                      SIL_CPU_INFO (defined in RasClass-api.h) for each CPU.
 * @param   CpuMapSize  The size of the buffer RasCpuMap.
 * @param   TotalCpus   Return the total number of CPUs.
 *
 * @return  EFI_STATUS  The status from openSIL translated into UEFI format.
 */
EFI_STATUS
SilCollectCpuMap (
  SIL_CPU_INFO  *RasCpuMap,
  UINT32        CpuMapSize,
  UINT32        *TotalCpus
  )
{
  SIL_STATUS SilStatus;

  DEBUG ((DEBUG_INFO, "Calling xPRF Service xPrfCollectCpuMap\n"));
  SilStatus = xPrfCollectCpuMap (RasCpuMap, CpuMapSize, TotalCpus);

  // Translate the SIL_STATUS and return it
  return TranslateStatus (SilStatus);
}

/**
 * SilCollectCoreMcaInfo
 *
 * @brief   xPRF protocol wrapper function for openSIL xPrfCollectCoreMcaInfo
 *
 * @details This wrapper function calls xPRF and translates the output if
 *          necessary.
 *
 * @note    ***This function should be executed on the BSP and all APs.
 *
 * @param   Buffer  An input buffer the points the the Cpu MCA info of the
 *                  currently executing processor.
 *
 */
VOID
SilCollectCoreMcaInfo (
  VOID    *Buffer
  )
{
  xPrfCollectCoreMcaInfo (Buffer);
}


/**
 * SilCollectMcaErrorInfo
 *
 * @brief   xPRF protocol wrapper function for openSIL xPrfCollectMcaErrorInfo
 *
 * @details This wrapper function calls xPRF and translates the output if
 *          necessary.
 *
 * @note    ***This function should be executed on the BSP and all APs.
 *
 * @param   Buffer  An input buffer the points the the MCA Error info of the
 *                  currently executing processor.
 *
 */
VOID
SilCollectMcaErrorInfo (
  VOID    *Buffer
  )
{
  xPrfCollectMcaErrorInfo (Buffer);
}

/**
 * SilMcaErrorAddrTranslate
 *
 * @brief   xPRF protocol wrapper function for openSIL xPrfMcaErrorAddrTranslate
 *
 * @details This wrapper function calls xPRF and translates the output if
 *          necessary.
 *
 *          Host is responsible for ensuring the input parameters match what
 *          is expected by openSIL (defined in RasClass-api.h).
 *
 * @param   SystemMemoryAddress Pointer to return the calculated system address
 * @param   NormalizedAddress   UMC memory address Information passed in from
 *                              Host.
 * @param   DimmInfo            Point to a buffer to populate translated
 *                              normalized address data. Host is responsible
 *                              for ensuring the buffer size is sufficient to
 *                              contain SIL_DIMM_INFO (defined in
 *                              RasClass-api.h).
 * @param   AddrData            Dimm information map used in address translation
 *
 * @return  EFI_STATUS
 * @retval  The return status from xPrfMcaErrorAddrTranslate converted to
 *          UEFI status format.
 */
EFI_STATUS
SilMcaErrorAddrTranslate (
  uint64_t                *SystemMemoryAddress,
  SIL_NORMALIZED_ADDRESS  *NormalizedAddress,
  SIL_DIMM_INFO           *DimmInfo,
  SIL_ADDR_DATA           *AddrData
  )
{
  SIL_STATUS  SilStatus;

  DEBUG ((DEBUG_INFO, "Calling xPRF Service xPrfMcaErrorAddrTranslate\n"));
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
 * @brief   xPRF protocol wrapper function for openSIL xPrfTranslateSysAddrToCS
 *
 * @details This wrapper function calls xPRF and translates the output if
 *          necessary.
 *
 *          Host is responsible for ensuring the input parameters match what
 *          is expected by openSIL (defined in RasClass-api.h).
 *
 * @param   NormalizedAddress   UMC memory address Information passed in from
 *                              Host.
 * @param   SystemMemoryAddress Pointer to return the calculated system address
 * @param   DimmInfo            Point to a buffer to populate translated
 *                              normalized address data. Host is responsible
 *                              for ensuring the buffer size is sufficient to
 *                              contain SIL_DIMM_INFO (defined in
 *                              RasClass-api.h).
 * @param   AddrData            Dimm information map used in address translation
 *
 * @return  EFI_STATUS
 * @retval  The return status from xPrfTranslateSysAddrToCS converted to
 *          UEFI status format.
 */
EFI_STATUS
SilTranslateSysAddrToCS (
  uint64_t                *SystemMemoryAddress,
  SIL_NORMALIZED_ADDRESS  *NormalizedAddress,
  SIL_DIMM_INFO           *DimmInfo,
  SIL_ADDR_DATA           *AddrData
  )
{
  SIL_STATUS  SilStatus;

  DEBUG ((DEBUG_INFO, "Calling xPRF Service xPrfTranslateSysAddrToCS\n"));
  SilStatus = xPrfTranslateSysAddrToCS (
    SystemMemoryAddress,
    NormalizedAddress,
    DimmInfo,
    AddrData
    );

  // Translate the SIL_STATUS and return it
  return TranslateStatus (SilStatus);
}

/**
 * SilCoreMcaIpIdInstanceIdInit
 *
 * @brief Program the Core MCA_IPID MSR Instance ID values for the CPU specified in RasCpuInfo.
 *
 * @param   RasCpuInfo  The CPU info structure for the core to program (RasClass-api.h)
 *
 * @note    ***This function should be executed on the BSP and all APs.
 *
 * @return  SIL_STATUS
 * @retval  SilPass     If the function completed normally
 * @retval  SilNotFound If RAS API was not found in the API list
 */
EFI_STATUS
SilCoreMcaIpIdInstanceIdInit (
  SIL_CPU_INFO  *RasCpuInfo
)
{
  SIL_STATUS  SilStatus;
  DEBUG ((DEBUG_INFO, "Calling xPRF Service xPrfMcaIpIdInstanceIdInit\n"));
  SilStatus = xPrfMcaIpIdInstanceIdInit (RasCpuInfo);

  // Translate the SIL_STATUS and return it
  return TranslateStatus (SilStatus);
}

/**
 * SilProgramCoreMcaConfigUmc
 *
 * @brief   This function will enable/disable FRU text in MCA based on the input flag for the currently executing
 *          processor.
 *
 * @param   Buffer      Input from host FW containing a flag to enable/disable FRU text in MCA.
 *
 * @note    ***This function should be executed on the BSP and all APs.
 *
 * @return  SIL_STATUS
 * @retval  SilPass     If the function completed normally
 * @retval  SilNotFound If RAS API was not found in the API list
 */
VOID
SilProgramCoreMcaConfigUmc (
  VOID  *Buffer
)
{
  SIL_STATUS  SilStatus;
  bool        EnableFruText;
  EFI_STATUS  Status;

  DEBUG ((DEBUG_INFO, "Calling xPRF Service xPrfProgramCoreMcaConfigUmc\n"));
  EnableFruText = (bool *)Buffer;

  SilStatus = xPrfProgramCoreMcaConfigUmc (EnableFruText);
  Status = TranslateStatus (SilStatus);
  if (Status != EFI_SUCCESS) {
    DEBUG ((DEBUG_ERROR, "Enabling FRU text in MCA Failed! Status = %r\n", Status));
  }
}

/**
 * SilGetThreadsPerCore
 *
 * @brief   xPRF protocol wrapper function for openSIL xPrfGetThreadsPerCore
 * @return  UINT8 ThreadsPerCore Value
 */
UINT8
SilGetThreadsPerCore (VOID)
{
  UINT8 ThreadsPerCoreValue;

  ThreadsPerCoreValue = 0;

  ThreadsPerCoreValue = (UINT8)(xPrfGetThreadsPerCore());
  DEBUG ((DEBUG_INFO, "opensil-uefi-interface xPrf ThreadsPerCore Value %d\n",ThreadsPerCoreValue));

  return ThreadsPerCoreValue;
}

/**
 * SilGetPStatePower
 *
 * @brief   xPRF protocol wrapper function for openSIL xPrfGetPStatePower
 *
 * @details This wrapper function calls xPRF and translates the output if
 *          necessary.
 *
 * @return  UINT32 Power in mW of the specified PState
 */
UINT32
SilGetPStatePower (VOID)
{
  UINT32 PowerInmW;

  PowerInmW      = 0;

  PowerInmW = (UINT32)(xPrfGetPStatePower());
  DEBUG ((DEBUG_INFO, "opensil-uefi-interface xPrf PStatePower Value %d\n",PowerInmW));

  return PowerInmW;
}

/**
 * SilCreateSratApicEntry
 *
 * @brief create SRAT Local APIC structure
 *
 * @param   SratApic      Input buffer for the SRAT Local APIC structure.  The Host is responsible
 *                        for ensuring the buffer size is sufficient to contain
 *                        SIL_SRAT_APIC or SIL_SRAT_x2APIC structure.
 *                        On output, the buffer is populated with SIL_SRAT_APIC or SIL_SRAT_x2APIC.
 * @param   SratApicSize  The size of the SratApicSize input buffer from the Host. This
 *                        is used by openSIL to ensure the input buffer size is
 *                        sufficient to contain SIL_SRAT_APIC or SIL_SRAT_x2APIC.
 *
 * @param   ApicModeValue APIC operation modes. Options are:
 *                        ApicMode              - Small systems (< 128 cores).
 *                        x2ApicMode            - Large systems.
 *                        ApicCompatibilityMode - Allow the code to choose depending on the quantity
 *                        of cores present. X2 mode is preferred.
 *
 * @param  SratTableLength SilCreateSratApicEntry xPrf Service update Total SRAT Table Length into
 *                        "SratTableLength" variable.
 *
 * @return  EFI_STATUS
 *
 * @retval  SUCCESS       The SRAT APIC Entry was successfully populated.
 * @retval  FAILURE       The SRAT APIC Entry input buffer size was not sufficient.
 */
EFI_STATUS
SilCreateSratApicEntry (
  VOID    *SratApic,
  UINT32  SratApicSize,
  UINT8   ApicModeValue,
  UINT32  *SratTableLength
)
{
  SIL_STATUS SilStatus;

  DEBUG ((DEBUG_INFO, "opensil-uefi-interface xPrfCreateSratApicEntry triggered \n"));
  SilStatus = xPrfCreateSratApicEntry ((UINT8 *)SratApic, SratApicSize,ApicModeValue,(UINT32 *)SratTableLength);

  return TranslateStatus (SilStatus);
}

/**
 * SilPStateGatherData
 *
 * @brief   This xPrf Service Provide the  PState information.
 *
 * @details This function will collect all PState information from the MSRs and fill up the
 *          Ouput Buffer.
 *          PState Informations are  : Physical socket number
 *                                     Logical core number in this socket
 *						               Pstate enable status
 *						               Core Frequency in MHz
 *						               Power in MilliWatts
 *						               Software P-state number
 *
 * @param   PStateData      Input buffer for the PState System Info structure.  The Host is responsible
 *                          for ensuring the buffer size is sufficient to contain
 *                          PSTATE_SYS_INFO  structure.
 *                          On output, the buffer is populated with PSTATE_SYS_INFO.
 * @param   PStateDataSize  The size of the PStateDataSize input buffer from the Host. This
 *                          is used by openSIL to ensure the input buffer size is
 *                          sufficient to contain PSTATE_SYS_INFO .
 *
 * @return  EFI_STATUS
 *
 * @retval  SUCCESS         The PState System Info was successfully populated.
 * @retval  FAILURE         The PState System Info input buffer size was not sufficient.
 */
EFI_STATUS
SilPStateGatherData (
  VOID    *PStateData,
  UINT32  PStateDataSize
  )
{

  SIL_STATUS SilStatus;
  DEBUG ((DEBUG_INFO, "opensil-uefi-interface xPrfPStateGatherData triggered \n"));
  SilStatus = xPrfPStateGatherData ((PSTATE_SYS_INFO *)PStateData, PStateDataSize);

  return TranslateStatus (SilStatus);
}

/**
 * SilGetCratHsaProcInfo
 *
 * @brief   This xPrf Service Provide the CRAT information about the HSA.
 *
 * @details This function will collect CRAT information about the HSA and fill up the
 *          Ouput Buffer.
 *          CRAT Informations are  : proximity node
 *                                  logical processor included in this HSA proximity domain
 *                                  count of execution units present in the APU node.
 *
 * @param  CratHsaProcData      Input buffer for the CRAT Info structure about the HSA. The
 *                              Host is responsible for ensuring the buffer size is
 *                              sufficient to contain SIL_CRAT_HSA_PROC_INFO  structure.
 *                              On output, the buffer is populated with SIL_CRAT_HSA_PROC_INFO.
 * @param  CratHsaProcDataSize  The size of the CratHsaProcDataSize input buffer from the Host.
 *                              This is used by openSIL to ensure the input buffer size is
 *                              sufficient to contain SIL_CRAT_HSA_PROC_INFO .
 *
 * @return  EFI_STATUS
 *
 * @retval  SUCCESS         The PState System Info was successfully populated.
 * @retval  FAILURE         The PState System Info input buffer size was not sufficient.
 */
EFI_STATUS
SilGetCratHsaProcInfo (
  VOID    *CratHsaProcData,
  UINT32  CratHsaProcDataSize
  )
{

  SIL_STATUS SilStatus;
  DEBUG ((DEBUG_INFO, "opensil-uefi-interface SilGetCratHsaProcInfo triggered \n"));
  SilStatus = xPrfGetCratHsaProcInfo ((UINT8 *)CratHsaProcData, CratHsaProcDataSize);

  return TranslateStatus (SilStatus);
}

/**
 * SilCratCacheEntry
 *
 * @brief   This xPrf Service Provide the CRAT Cache information .
 *
 * @details This function will collect CRAT Cache information and fill up the
 *          Ouput Buffer.
 *          CRAT  Cache Informations are  : Low value of a logical processor
 *                                                    which includes this component
 *
 * @param  CratCacheEntry       Input buffer for the CRAT Cache Info structure. The
 *                              Host is responsible for ensuring the buffer size is
 *                              sufficient to contain SIL_CRAT_CACHE  structure.
 *                              On output, the buffer is populated with SIL_CRAT_CACHE.
 * @param  CratCacheEntrySize   The size of the CratCacheEntrySize input buffer from the Host.
 *                              This is used by openSIL to ensure the input buffer size is
 *                              sufficient to contain SIL_CRAT_CACHE .
 *
 * @return  EFI_STATUS
 *
 * @retval  SUCCESS         The PState System Info was successfully populated.
 * @retval  FAILURE         The PState System Info input buffer size was not sufficient.
 */
EFI_STATUS
SilCratCacheEntry (
  VOID    *CratCacheEntry,
  UINT32  CratCacheEntrySize
  )
{

  SIL_STATUS SilStatus;
  DEBUG ((DEBUG_INFO, "opensil-uefi-interface SilCratCacheEntry triggered \n"));
  SilStatus = xPrfCratCacheEntry ((UINT8 *)CratCacheEntry, CratCacheEntrySize);

  return TranslateStatus (SilStatus);
}

/**
 * xPrfServicesProtocolInstall
 *
 * @brief   Installed xPrfServicesProtocol for DXE
 *
 * @todo    Install xPrfServicesProtocol for SMM as well.
 *
 * @return  EFI_STATUS
 */
EFI_STATUS
xPrfServicesProtocolInstall (VOID)
{
  EFI_HANDLE        Handle;
  EFI_STATUS        Status;

  DEBUG ((DEBUG_INFO, "Installing xPrfServicesProtocol\n"));

  /*
   * Install openSIL xPRF services protocol.  Also needs SMM protocol for RAS.
   */
  Handle = NULL;
  Status = gBS->InstallProtocolInterface (
              &Handle,
              &gOpenSilxPrfProtocolGuid,
              EFI_NATIVE_INTERFACE,
              &mOpenSilXprfProtocol
              );
  ASSERT_EFI_ERROR (Status);

  return Status;
}
