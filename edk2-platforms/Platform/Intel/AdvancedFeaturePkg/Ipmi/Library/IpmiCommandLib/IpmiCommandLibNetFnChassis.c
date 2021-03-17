/** @file
  IPMI Command - NetFnChassis.

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiPei.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/IpmiLib.h>

#include <IndustryStandard/Ipmi.h>


EFI_STATUS
EFIAPI
IpmiGetChassisCapabilities (
  OUT IPMI_GET_CHASSIS_CAPABILITIES_RESPONSE  *GetChassisCapabilitiesResponse
  )
{
  EFI_STATUS                   Status;
  UINT32                       DataSize;

  DataSize = sizeof(*GetChassisCapabilitiesResponse);
  Status = IpmiSubmitCommand (
             IPMI_NETFN_CHASSIS,
             IPMI_CHASSIS_GET_CAPABILITIES,
             NULL,
             0,
             (VOID *)GetChassisCapabilitiesResponse,
             &DataSize
             );
  return Status;
}

EFI_STATUS
EFIAPI
IpmiGetChassisStatus (
  OUT IPMI_GET_CHASSIS_STATUS_RESPONSE  *GetChassisStatusResponse
  )
{
  EFI_STATUS                   Status;
  UINT32                       DataSize;

  DataSize = sizeof(*GetChassisStatusResponse);
  Status = IpmiSubmitCommand (
             IPMI_NETFN_CHASSIS,
             IPMI_CHASSIS_GET_STATUS,
             NULL,
             0,
             (VOID *)GetChassisStatusResponse,
             &DataSize
             );
  return Status;
}

EFI_STATUS
EFIAPI
IpmiChassisControl (
  IN IPMI_CHASSIS_CONTROL_REQUEST  *ChassisControlRequest,
  OUT UINT8                        *CompletionCode
  )
{
  EFI_STATUS                   Status;
  UINT32                       DataSize;

  DataSize = sizeof(*CompletionCode);
  Status = IpmiSubmitCommand (
             IPMI_NETFN_CHASSIS,
             IPMI_CHASSIS_CONTROL,
             (VOID *)ChassisControlRequest,
             sizeof(*ChassisControlRequest),
             (VOID *)CompletionCode,
             &DataSize
             );
  return Status;
}

EFI_STATUS
EFIAPI
IpmiSetPowerRestorePolicy (
  IN  IPMI_SET_POWER_RESTORE_POLICY_REQUEST  *ChassisControlRequest,
  OUT IPMI_SET_POWER_RESTORE_POLICY_RESPONSE *ChassisControlResponse
  )
{
  EFI_STATUS                   Status;
  UINT32                       DataSize;

  DataSize = sizeof(*ChassisControlResponse);
  Status = IpmiSubmitCommand (
             IPMI_NETFN_CHASSIS,
             IPMI_CHASSIS_SET_POWER_RESTORE_POLICY,
             (VOID *)ChassisControlRequest,
             sizeof(*ChassisControlRequest),
             (VOID *)ChassisControlResponse,
             &DataSize
             );
  return Status;
}
