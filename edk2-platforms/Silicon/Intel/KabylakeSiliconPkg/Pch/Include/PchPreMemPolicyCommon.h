/** @file
  PCH configuration based on PCH policy

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#ifndef _PCH_PREMEM_POLICY_COMMON_H_
#define _PCH_PREMEM_POLICY_COMMON_H_

#include <ConfigBlock.h>

#include "PchLimits.h"
#include "ConfigBlock/PchGeneralConfig.h"
#include "ConfigBlock/DciConfig.h"
#include "ConfigBlock/WatchDogConfig.h"
#include "ConfigBlock/TraceHubConfig.h"
#include "ConfigBlock/HpetConfig.h"
#include "ConfigBlock/SmbusConfig.h"
#include "ConfigBlock/LpcConfig.h"
#include "ConfigBlock/HsioPcieConfig.h"
#include "ConfigBlock/HsioSataConfig.h"
#include "ConfigBlock/HsioConfig.h"

#pragma pack (push,1)

#ifndef FORCE_ENABLE
#define FORCE_ENABLE  1
#endif
#ifndef FORCE_DISABLE
#define FORCE_DISABLE 2
#endif
#ifndef PLATFORM_POR
#define PLATFORM_POR  0
#endif

/**
  PCH Policy revision number
  Any backwards compatible changes to this structure will result in an update in the revision number
**/
#define PCH_PREMEM_POLICY_REVISION  1

/**
  PCH Policy PPI\n
  All PCH config block change history will be listed here\n\n

  - <b>Revision 1</b>:
    - Initial version.\n
**/
typedef struct _PCH_PREMEM_POLICY {
  CONFIG_BLOCK_TABLE_HEADER      TableHeader;
/*
  Individual Config Block Structures are added here in memory as part of AddConfigBlock()
*/
} PCH_PREMEM_POLICY;

#pragma pack (pop)

#endif // _PCH_PREMEM_POLICY_COMMON_H_
