/** @file
  CPU Policy structure definition which will contain several config blocks during runtime.

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#ifndef _CPU_POLICY_COMMON_H_
#define _CPU_POLICY_COMMON_H_

#include <ConfigBlock.h>
#include <ConfigBlock/CpuOverclockingConfig.h>
#include <ConfigBlock/CpuConfig.h>
#include <ConfigBlock/CpuPidTestConfig.h>
#include <ConfigBlock/CpuPowerMgmtBasicConfig.h>
#include <ConfigBlock/CpuPowerMgmtCustomConfig.h>
#include <ConfigBlock/CpuPowerMgmtPsysConfig.h>
#include <ConfigBlock/CpuPowerMgmtTestConfig.h>
#include <ConfigBlock/CpuPowerMgmtVrConfig.h>
#include <ConfigBlock/CpuTestConfig.h>
#include <ConfigBlock/CpuConfigLibPreMemConfig.h>

#endif // _CPU_POLICY_COMMON_H_
