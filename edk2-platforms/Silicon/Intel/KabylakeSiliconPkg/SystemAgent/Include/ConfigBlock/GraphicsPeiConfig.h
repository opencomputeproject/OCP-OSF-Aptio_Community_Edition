/** @file
  Policy definition for Internal Graphics Config Block (PostMem)

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _GRAPHICS_PEI_CONFIG_H_
#define _GRAPHICS_PEI_CONFIG_H_
#pragma pack(push, 1)

#define GRAPHICS_PEI_CONFIG_REVISION 1

/**
  This configuration block is to configure IGD related variables used in PostMem PEI.
  If Intel Gfx Device is not supported, all policies can be ignored.
  <b>Revision 1</b>:
  - Initial version.
**/
typedef struct {
  CONFIG_BLOCK_HEADER   Header;                   ///< Offset 0-27 Config Block Header
  UINT32                PeiGraphicsPeimInit: 1;   ///< Offset 28:6 :This policy is used to enable/disable Intel Gfx PEIM.<b>0- Disable</b>, 1- Enable
  UINT32                RsvdBits0          : 31;  ///< Offser 28:16 :Reserved for future use
  VOID*                 LogoPtr;                  ///< Offset 32 Address of Logo to be displayed in PEI
  UINT32                LogoSize;                 ///< Offset 36 Logo Size
  VOID*                 GraphicsConfigPtr;        ///< Offset 40 Address of the Graphics Configuration Table
} GRAPHICS_PEI_CONFIG;
#pragma pack(pop)

#endif // _GRAPHICS_PEI_CONFIG_H_
