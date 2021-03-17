/** @file

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Uefi.h>
#include <Library/TestPointCheckLib.h>
#include <Library/TestPointLib.h>
#include <Library/DebugLib.h>

/**
  This function dump raw data.

  @param  Data  raw data
  @param  Size  raw data size
**/
VOID
InternalDumpData (
  IN UINT8  *Data,
  IN UINTN  Size
  )
{
  UINTN  Index;
  for (Index = 0; Index < Size; Index++) {
    DEBUG ((DEBUG_INFO, "%02x", (UINTN)Data[Index]));
    if ((Index + 1) != Size) {
      DEBUG ((DEBUG_INFO, " "));
    }
  }
}

/**

  This function dump raw data with colume format.

  @param  Data  raw data
  @param  Size  raw data size

**/
VOID
InternalDumpHex (
  IN UINT8  *Data,
  IN UINTN  Size
  )
{
  UINTN   Index;
  UINTN   Count;
  UINTN   Left;

#define COLUME_SIZE  (16 * 2)

  Count = Size / COLUME_SIZE;
  Left  = Size % COLUME_SIZE;
  for (Index = 0; Index < Count; Index++) {
    DEBUG ((EFI_D_INFO, "%04x: ", Index * COLUME_SIZE));
    InternalDumpData (Data + Index * COLUME_SIZE, COLUME_SIZE);
    DEBUG ((EFI_D_INFO, "\n"));
  }

  if (Left != 0) {
    DEBUG ((EFI_D_INFO, "%04x: ", Index * COLUME_SIZE));
    InternalDumpData (Data + Index * COLUME_SIZE, Left);
    DEBUG ((EFI_D_INFO, "\n"));
  }
}
