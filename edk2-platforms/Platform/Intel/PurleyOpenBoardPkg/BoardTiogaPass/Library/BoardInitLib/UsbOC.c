/** @file

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

//***********************************************************************
//*                                                                     *
//*   Copyright (c) 1985 - 2021, American Megatrends International LLC. *
//*                                                                     *
//*      All rights reserved.                                           *
//*                                                                     *
//*      This program and the accompanying materials are licensed and   *
//*      made available under the terms and conditions of the BSD       *
//*      License that accompanies this distribution.  The full text of  *
//*      the license may be found at:                                   *
//*      http://opensource.org/licenses/bsd-license.php.                *
//*                                                                     *
//*      THIS PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN        *
//*      "AS IS" BASIS, WITHOUT WARRANTIES OR REPRESENTATIONS OF        *
//*      ANY KIND, EITHER EXPRESS OR IMPLIED.                           *
//*                                                                     *
//***********************************************************************

#include <PiPei.h>

#include <Library/PcdLib.h>
#include <PchLimits.h>
#include <PchPolicyCommon.h>

PCH_USB_OVERCURRENT_PIN Usb20OverCurrentMappings[PCH_MAX_USB2_PORTS] = {
                          PchUsbOverCurrentPin0,
                          PchUsbOverCurrentPinSkip,
                          PchUsbOverCurrentPinSkip,
                          PchUsbOverCurrentPinSkip,
                          PchUsbOverCurrentPinSkip,
                          PchUsbOverCurrentPinSkip,
                          PchUsbOverCurrentPinSkip,
                          PchUsbOverCurrentPinSkip,
                          PchUsbOverCurrentPinSkip,
                          PchUsbOverCurrentPinSkip,
                          PchUsbOverCurrentPinSkip,
                          PchUsbOverCurrentPinSkip,
                          PchUsbOverCurrentPinSkip,
                          PchUsbOverCurrentPinSkip,
                          PchUsbOverCurrentPinSkip,
                          PchUsbOverCurrentPinSkip
                       };

PCH_USB_OVERCURRENT_PIN Usb30OverCurrentMappings[PCH_MAX_USB3_PORTS] = {
                          PchUsbOverCurrentPin0,
                          PchUsbOverCurrentPinSkip,
                          PchUsbOverCurrentPinSkip,
                          PchUsbOverCurrentPinSkip,
                          PchUsbOverCurrentPinSkip,
                          PchUsbOverCurrentPinSkip,
                          PchUsbOverCurrentPinSkip,
                          PchUsbOverCurrentPinSkip,
                          PchUsbOverCurrentPinSkip,
                          PchUsbOverCurrentPinSkip
                       };

