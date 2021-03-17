/** @file

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _PCH_LBG_HSIO_BX_H_
#define _PCH_LBG_HSIO_BX_H_

#define PCH_LBG_HSIO_VER_BX   0x2f

extern UINT8                      PchLbgChipsetInitTable_Bx[2844];
extern PCH_SBI_HSIO_TABLE_STRUCT  *PchLbgHsio_Bx_Ptr;
extern UINT16                     PchLbgHsio_Bx_Size;

#endif //_PCH_LBG_HSIO_BX_H_