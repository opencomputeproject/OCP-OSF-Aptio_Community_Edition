/** @file

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

//
// This defines the maximum number of sockets supported by some modules.
// It is generally better to use a dynamic solution.
// This is also defined by build tools for some special build
// environments used in validation that do not support EDK II build
// and thus can't use PCD.
//

#ifndef MAX_SOCKET
#define MAX_SOCKET (FixedPcdGet32 (PcdMaxCpuSocketCount))
#endif

