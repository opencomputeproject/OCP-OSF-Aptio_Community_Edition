/**

  Copyright (c) 2023, American Megatrends International LLC.
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

/* $NoKeywords:$ */
/**
 * @file
 *
 * Base Fabric MMIO map manager Lib implementation for DFX
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Fabric
 * @e \$Revision$   @e \$Date$
 *
 */
#ifndef _FABRIC_RESOURCE_MANAGER_LIB_H_
#define _FABRIC_RESOURCE_MANAGER_LIB_H_

#include <PiDxe.h>
#include <SilCommon.h>
#include <RcMgr/DfX/RcManager4-api.h>
#include <Apob.h>
#include <DF/DfIp2Ip.h>
#include <DF/Common/BaseFabricTopologyCmn.h>
#include <DF/DfX/DfXBaseFabricTopology.h>

/*---------------------------------------------------------------------------------------
 *                        F U N C T I O N    P R O T O T Y P E
 *---------------------------------------------------------------------------------------
 */
/*---------------------------------------------------------------------------------------*/
/**
 * FabricGetAvailableResource
 *
 * Get available DF resource (MMIO/IO) for each Rb
 *
 * @param[in, out]    ResourceForEachRb     Avaiable DF resource (MMIO/IO) for each Rb
 *
 * @retval            EFI_SUCCESS           Success to get available resource
 *                    EFI_ABORTED           Cannot get information of MMIO or IO
 */
EFI_STATUS
FabricGetAvailableResource (
  IN  DFX_FABRIC_RESOURCE_FOR_EACH_RB    *ResourceForEachRb
  );

/**
 * This service retrieves information about the overall system with respect to data fabric.
 *
 * @param[out] NumberOfInstalledProcessors    Pointer to the total number of populated
 *                                            processor sockets in the system.
 * @param[out] TotalNumberOfDie               Pointer to the total number of die in the system.
 * @param[out] TotalNumberOfRootBridges       Pointer to the total number of root PCI bridges in
 *                                            the system.
 * @param[out] SystemFchRootBridgeLocation    System primary FCH location.
 * @param[out] SystemSmuRootBridgeLocation    System primary SMU location.
 *
 * @retval EFI_SUCCESS                        The system topology information was successfully retrieved.
 * @retval EFI_INVALID_PARAMETER              All output parameter pointers are NULL.
 *
 **/
EFI_STATUS
EFIAPI
FabricGetSystemInfo (
  OUT   UINT32                                 *NumberOfInstalledProcessors,
  OUT   UINT32                                 *TotalNumberOfDie,
  OUT   UINT32                                 *TotalNumberOfRootBridges,
  OUT   ROOT_BRIDGE_LOCATION                   *SystemFchRootBridgeLocation,
  OUT   ROOT_BRIDGE_LOCATION                   *SystemSmuRootBridgeLocation
  );
  
#endif


