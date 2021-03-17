/** @file
  IPMI Command - NetFnTransport.

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
IpmiSolActivating (
  IN  IPMI_SOL_ACTIVATING_REQUEST  *SolActivatingRequest,
  OUT UINT8                        *CompletionCode
  )
{
  EFI_STATUS                   Status;
  UINT32                       DataSize;

  DataSize = sizeof(*CompletionCode);
  Status = IpmiSubmitCommand (
             IPMI_NETFN_TRANSPORT,
             IPMI_TRANSPORT_SOL_ACTIVATING,
             (VOID *)SolActivatingRequest,
             sizeof(*SolActivatingRequest),
             (VOID *)CompletionCode,
             &DataSize
             );
  return Status;
}

EFI_STATUS
EFIAPI
IpmiSetSolConfigurationParameters (
  IN  IPMI_SET_SOL_CONFIGURATION_PARAMETERS_REQUEST  *SetConfigurationParametersRequest,
  IN  UINT32                                         SetConfigurationParametersRequestSize,
  OUT UINT8                                          *CompletionCode
  )
{
  EFI_STATUS                   Status;
  UINT32                       DataSize;

  DataSize = sizeof(*CompletionCode);
  Status = IpmiSubmitCommand (
             IPMI_NETFN_TRANSPORT,
             IPMI_TRANSPORT_SET_SOL_CONFIG_PARAM,
             (VOID *)SetConfigurationParametersRequest,
             SetConfigurationParametersRequestSize,
             (VOID *)CompletionCode,
             &DataSize
             );
  return Status;
}

EFI_STATUS
EFIAPI
IpmiGetSolConfigurationParameters (
  IN  IPMI_GET_SOL_CONFIGURATION_PARAMETERS_REQUEST  *GetConfigurationParametersRequest,
  OUT IPMI_GET_SOL_CONFIGURATION_PARAMETERS_RESPONSE *GetConfigurationParametersResponse,
  IN OUT UINT32                                      *GetConfigurationParametersResponseSize
  )
{
  EFI_STATUS                   Status;

  Status = IpmiSubmitCommand (
             IPMI_NETFN_TRANSPORT,
             IPMI_TRANSPORT_GET_SOL_CONFIG_PARAM,
             (VOID *)GetConfigurationParametersRequest,
             sizeof(*GetConfigurationParametersRequest),
             (VOID *)GetConfigurationParametersResponse,
             GetConfigurationParametersResponseSize
             );
  return Status;
}
