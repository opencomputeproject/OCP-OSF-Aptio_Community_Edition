/** @file

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _MrcCommonTypes_h_
#define _MrcCommonTypes_h_

#include "DataTypes.h"

#define INT32_MIN                       (0x80000000)
#ifndef INT32_MAX  //INT32_MAX->Already defined
#define INT32_MAX                       (0x7FFFFFFF)
#endif
#define INT16_MIN                       (0x8000)
#define INT16_MAX                       (0x7FFF)

#endif // _MrcCommonTypes_h_
