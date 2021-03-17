/** @file

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

//***********************************************************************
//*                                                                     *
//*   Copyright (c) 1985 - 2021, American Megatrends International LLC. *
//*                                                                     *
//*      All rights reserved.                                           *
//*                                                                     *
//*      This program and the accompanying materials are licensed and   *
//*      made available under the terms and conditions of the BSD       *
//*      License that accompanies this distribution.  The full text of  *
//*      the license may be found at:                                   *
//*      http://opensource.org/licenses/bsd-license.php.                *
//*                                                                     *
//*      THIS PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN        *
//*      "AS IS" BASIS, WITHOUT WARRANTIES OR REPRESENTATIONS OF        *
//*      ANY KIND, EITHER EXPRESS OR IMPLIED.                           *
//*                                                                     *
//***********************************************************************

#ifndef MINIBIOS_BUILD
#include <PiPei.h>
#include <Platform.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#endif

#include <SysHost.h>

#define SPEED_REC_96GT     0
#define SPEED_REC_104GT    1
#define ADAPTIVE_CTLE 0x3f

#pragma pack(1)

ALL_LANES_EPARAM_LINK_INFO  KtiTiogaPassAllLanesEparamTable[] = {
  //
  // SocketID, Freq, Link, TXEQL, CTLEPEAK
  //

  //
  // Socket 0
  //
  {0x0, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK0), 0x2A34353F, ADAPTIVE_CTLE},
  {0x0, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK1), 0x2B33373F, ADAPTIVE_CTLE},

  //
  // Socket 1
  //
  {0x1, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK0), 0x2D3A323F, ADAPTIVE_CTLE},
  {0x1, (1 << SPEED_REC_96GT) | (1 << SPEED_REC_104GT), (1 << KTI_LINK1), 0x2A32373F, ADAPTIVE_CTLE}
};

#pragma pack()

UINT32 KtiTiogaPassAllLanesEparamTableSize = sizeof(KtiTiogaPassAllLanesEparamTable);
