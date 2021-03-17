/** @file
  Prototype of the SiConfigBlockLib library.

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SI_CONFIG_BLOCK_LIB_H_
#define _SI_CONFIG_BLOCK_LIB_H_


typedef
VOID
(*LOAD_DEFAULT_FUNCTION) (
  IN VOID   *ConfigBlockPointer
  );

typedef struct {
  EFI_GUID               *Guid;
  UINT16                 Size;
  UINT8                  Revision;
  LOAD_DEFAULT_FUNCTION  LoadDefault;
} COMPONENT_BLOCK_ENTRY;

/**
  GetComponentConfigBlockTotalSize get config block table total size.

  @param[in] ComponentBlocks    Component blocks array
  @param[in] TotalBlockCount    Number of blocks

  @retval                       Size of config block table
**/
UINT16
EFIAPI
GetComponentConfigBlockTotalSize (
  IN COMPONENT_BLOCK_ENTRY *ComponentBlocks,
  IN UINT16                TotalBlockCount
  );

/**
  AddComponentConfigBlocks add all config blocks.

  @param[in] ConfigBlockTableAddress    The pointer to add config blocks
  @param[in] ComponentBlocks            Config blocks array
  @param[in] TotalBlockCount            Number of blocks

  @retval EFI_SUCCESS                   The policy default is initialized.
  @retval EFI_OUT_OF_RESOURCES          Insufficient resources to create buffer
**/
EFI_STATUS
EFIAPI
AddComponentConfigBlocks (
  IN VOID                  *ConfigBlockTableAddress,
  IN COMPONENT_BLOCK_ENTRY *ComponentBlocks,
  IN UINT16                TotalBlockCount
  );
#endif // _SI_CONFIG_BLOCK_LIB_H_
