/** @file

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

Scope(\_SB.PCI0) {
  //
  // Integrated Sensor Hub (PCI Mode) - Device 19, Function 0
  //
  Device(ISHD) {
    Name(_ADR, 0x00130000)

    Method(_DSM, 0x4, NotSerialized, 0, UnknownObj, {BuffObj, IntObj, IntObj, PkgObj}) {
      if(PCIC(Arg0)) { return(PCID(Arg0,Arg1,Arg2,Arg3)) }
      //Fix warning: not all control paths return a value
      Return(0)
    } // End _DSM
  } // Device(ISHD)
}