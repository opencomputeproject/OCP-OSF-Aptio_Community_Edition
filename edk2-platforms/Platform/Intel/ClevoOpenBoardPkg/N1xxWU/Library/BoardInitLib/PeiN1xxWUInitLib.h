/** @file

Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _PEI_N1_XX_WU_BOARD_INIT_LIB_H_
#define _PEI_N1_XX_WU_BOARD_INIT_LIB_H_

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/PcdLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/GpioLib.h>
#include <Ppi/SiPolicy.h>
#include <PchHsioPtssTables.h>
#include <IoExpander.h>

#include <N1xxWUId.h>

extern const UINT8 mDqByteMapSklRvp3[2][6][2];
extern const UINT8 mDqsMapCpu2DramSklRvp3[2][8];
extern const UINT8 mSkylakeRvp3Spd110[];
extern const UINT16 mSkylakeRvp3Spd110Size;
extern HSIO_PTSS_TABLES PchLpHsioPtss_Bx_N1xxWU[];
extern UINT16 PchLpHsioPtss_Bx_N1xxWU_Size;
extern HSIO_PTSS_TABLES PchLpHsioPtss_Cx_N1xxWU[];
extern UINT16 PchLpHsioPtss_Cx_N1xxWU_Size;

extern HDAUDIO_VERB_TABLE HdaVerbTableAlc286Rvp3;
extern GPIO_INIT_CONFIG mGpioTableN1xxWUUcmcDevice[];
extern UINT16 mGpioTableN1xxWUUcmcDeviceSize;

extern IO_EXPANDER_GPIO_CONFIG mGpioTableIoExpander[];
extern UINT16 mGpioTableIoExpanderSize;
extern GPIO_INIT_CONFIG mGpioTableN1xxWUTouchpanel;
extern GPIO_INIT_CONFIG mGpioTableN1xxWU[];
extern UINT16 mGpioTableN1xxWUSize;

#endif // _PEI_N1_XX_WU_BOARD_INIT_LIB_H_
