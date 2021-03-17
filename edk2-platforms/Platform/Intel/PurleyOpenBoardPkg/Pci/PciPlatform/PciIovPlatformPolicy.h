/** @file

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef PCI_IOV_PLATFORM_POLICY_H_
#define PCI_IOV_PLATFORM_POLICY_H_

/**

    The GetSystemLowestPageSize() function retrieves the system lowest page size.
    
    @param This                 - Pointer to the EFI_PCI_IOV_PLATFORM_PROTOCOL instance.
    @param SystemLowestPageSize - The system lowest page size. (This system supports a
                                  page size of 2^(n+12) if bit n is set.)
    
    @retval EFI_SUCCESS           - The function completed successfully.
    @retval EFI_INVALID_PARAMETER - SystemLowestPageSize is NULL.
    
**/
EFI_STATUS
EFIAPI
GetSystemLowestPageSize (
  IN  EFI_PCI_IOV_PLATFORM_PROTOCOL           *This,
  OUT UINT32                                  *SystemLowestPageSize
)
;


/**

    The GetPlatformPolicy() function retrieves the platform policy regarding PCI IOV.
    
    @param This         - Pointer to the EFI_PCI_IOV_PLATFORM_PROTOCOL instance.
    @param PciIovPolicy - The platform policy for PCI IOV configuration.
    
    @retval EFI_SUCCESS           - The function completed successfully.
    @retval EFI_INVALID_PARAMETER - PciPolicy is NULL.
    
**/
EFI_STATUS
EFIAPI 
GetIovPlatformPolicy (
  IN  EFI_PCI_IOV_PLATFORM_PROTOCOL           *This,
  OUT EFI_PCI_IOV_PLATFORM_POLICY             *PciIovPolicy
)
;

#endif
