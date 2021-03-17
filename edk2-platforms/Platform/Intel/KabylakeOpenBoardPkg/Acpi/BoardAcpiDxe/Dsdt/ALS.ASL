/** @file
  ACPI DSDT table

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

Device(ALSD) 
{
  Name(_HID,"ACPI0008")

  Method(_STA,0)
  {
    If(LEqual(ALSE,2))
    {
      Return(0x000B)    // ALS Enabled.  Don't show it in UI.
    }

    Return(0x0000)      // ALS Disabled.  Hide it.
  }

  Method(_ALI)
  {
    Return (Or(ShiftLeft(LHIH,8),LLOW))
  }

  Name(_ALR, Package()
  {
    Package() {70,  0},
    Package() {73,  10},
    Package() {85,  80},
    Package() {100, 300},
    Package() {150, 1000}
  })
  
}
