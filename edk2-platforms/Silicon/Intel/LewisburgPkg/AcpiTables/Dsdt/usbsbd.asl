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
// _DSM : Device Specific Method supporting USB Sideband Deferring function
//
// Arg0: UUID Unique function identifier
// Arg1: Integer Revision Level
// Arg2: Integer Function Index
// Arg3: Package Parameters
//
Method (_DSM, 4, Serialized, 0, UnknownObj, {BuffObj, IntObj, IntObj, PkgObj})
{

  If (LEqual(Arg0, ToUUID ("A5FC708F-8775-4BA6-BD0C-BA90A1EC72F8")))
  {
    //
    // Switch by function index
    //
    Switch (ToInteger(Arg2))
    {
      //
      // Standard query - A bitmask of functions supported
      // Supports function 0-2
      //
      Case (0)
      {
        if (LEqual(Arg1, 1)){ // test Arg1 for the revision 
          Return (Buffer () {0x07})
        } else {
          Return (Buffer () {0})
        }
      }
      //
      // USB Sideband Deferring Support
      //   0: USB Sideband Deferring not supported on this device
      //   1: USB Sideband Deferring supported
      //
      Case (1)
      {
        if (LEqual(SDGV,0xFF)){ // check for valid GPE vector
          Return (0)
        } else {
          Return (1)
        }
      }
      //
      // GPE Vector
      //  Return the bit offset within the GPE block of the GPIO (HOST_ALERT) driven by this device
      //
      Case (2)
      {
        Return (SDGV)
      }
    }
  }

  Return (0)
}
