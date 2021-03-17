/** @file
  Trace Hub policy

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#ifndef _TRACEHUB_CONFIG_H_
#define _TRACEHUB_CONFIG_H_

#define TRACEHUB_PREMEM_CONFIG_REVISION 1
extern EFI_GUID gTraceHubPreMemConfigGuid;

#pragma pack (push,1)

///
/// The PCH_TRACE_HUB_CONFIG block describes TraceHub settings for PCH.
///
typedef struct {
  CONFIG_BLOCK_HEADER   Header;          ///< Config Block Header
  UINT32  EnableMode         :  2;       ///< 0 = Disable <b> 2 = Host Debugger enabled </b>
  UINT32  RsvdBits0          : 30;       ///< Reserved bits
  UINT32  MemReg0Size;                   ///< Default is <b>0 (none)</b>.
  UINT32  MemReg1Size;                   ///< Default is <b>0 (none)</b>.
} PCH_TRACE_HUB_PREMEM_CONFIG;

#pragma pack (pop)

#endif // _TRACEHUB_CONFIG_H_
