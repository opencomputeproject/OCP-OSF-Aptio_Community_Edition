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
#ifndef _PCH_POLICY_COMMON_H_
#define _PCH_POLICY_COMMON_H_

#include <ConfigBlock.h>

#include "PchLimits.h"
#include "ConfigBlock/PchGeneralConfig.h"
#include "ConfigBlock/PcieRpConfig.h"
#include "ConfigBlock/SataConfig.h"
#include "ConfigBlock/IoApicConfig.h"
#include "ConfigBlock/Cio2Config.h"
#include "ConfigBlock/DmiConfig.h"
#include "ConfigBlock/FlashProtectionConfig.h"
#include "ConfigBlock/HdAudioConfig.h"
#include "ConfigBlock/InterruptConfig.h"
#include "ConfigBlock/IshConfig.h"
#include "ConfigBlock/LanConfig.h"
#include "ConfigBlock/LockDownConfig.h"
#include "ConfigBlock/P2sbConfig.h"
#include "ConfigBlock/PmConfig.h"
#include "ConfigBlock/Port61Config.h"
#include "ConfigBlock/ScsConfig.h"
#include "ConfigBlock/SerialIoConfig.h"
#include "ConfigBlock/SerialIrqConfig.h"
#include "ConfigBlock/SpiConfig.h"
#include "ConfigBlock/ThermalConfig.h"
#include "ConfigBlock/UsbConfig.h"
#include "ConfigBlock/EspiConfig.h"

#ifndef FORCE_ENABLE
#define FORCE_ENABLE  1
#endif
#ifndef FORCE_DISABLE
#define FORCE_DISABLE 2
#endif
#ifndef PLATFORM_POR
#define PLATFORM_POR  0
#endif


#endif // _PCH_POLICY_COMMON_H_
