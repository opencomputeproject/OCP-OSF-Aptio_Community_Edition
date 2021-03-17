/** @file

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef  _mem_platform_h
#define  _mem_platform_h

#include "DataTypes.h"

#ifdef   SERIAL_DBG_MSG
#define  MRC_TRACE  1
#endif


//
// Compatible BIOS Data Structure
//
#define BDAT_SUPPORT    0  //Memory Data Schema 4 and RMT Schema 5 of BDAT 4.0

//
// QR support
//
#define  QR_DIMM_SUPPORT 1

//
// Define to enable DIMM margin checking
//
#define  MARGIN_CHECK   1

//
// Define to enable SODIMM module support
//
#define  SODIMM_SUPPORT    1

//
// Define to enable ME UMA support
//
//#define ME_SUPPORT_FLAG   1

//
// Define to enable XMP
//
#define XMP_SUPPORT     1

// Define to enable DEBUG for NVMCTLR (LATE CMD CLK)
//#define DEBUG_LATECMDCLK      1

// Define to enable MRS Stacking
//#define MRS_STACKING    1

//
// Define to max ppr
//
#define MAX_PPR_ADDR_ENTRIES           20

//
//-------------------------------------
// DVP Platform-specific defines
//-------------------------------------
//
#ifdef   DVP_PLATFORM
#endif   // DVP_PLATFORM

//
//-------------------------------------
// CRB Platform-specific defines
//-------------------------------------
//
#ifdef   CRB_PLATFORM
#endif   // CRB_PLATFORM

#ifndef MAX_HA
#define MAX_HA              2                   // Number of Home Agents / IMCs
#endif

//SKX_TODO: I have removed NonPOR elements, I will delete this line before submit

#endif   // _mem_platform_h
