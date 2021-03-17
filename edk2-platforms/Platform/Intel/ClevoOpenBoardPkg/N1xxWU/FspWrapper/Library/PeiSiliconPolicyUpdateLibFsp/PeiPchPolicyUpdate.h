/** @file

Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _PEI_PCH_POLICY_UPDATE_H_
#define _PEI_PCH_POLICY_UPDATE_H_

//
// External include files do NOT need to be explicitly specified in real EDKII
// environment
//
#include <PiPei.h>

#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <Ppi/SiPolicy.h>
#include <Library/MmPciLib.h>
#include <Library/ConfigBlockLib.h>

#include <FspEas.h>
#include <FspmUpd.h>
#include <FspsUpd.h>

#endif
