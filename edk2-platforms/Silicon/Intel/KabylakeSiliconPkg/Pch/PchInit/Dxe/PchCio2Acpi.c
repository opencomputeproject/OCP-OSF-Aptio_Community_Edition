/** @file
  Initializes PCH CIO2 device ACPI data.

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PchInit.h>

/**
  Update ASL definitions for CIO2 device.

  @retval EFI_SUCCESS         The function completed successfully
**/
EFI_STATUS
UpdateCio2AcpiData (
  VOID
  )
{
  UINT32 Index;
  PCH_STEPPING    PchStep;

  DEBUG ((DEBUG_INFO, "UpdateCio2AcpiData() Start\n"));
  PchStep   = PchStepping ();

  //if CIO2 is enabled as ACPI device then update its ACPI data
  if (PchStep >= PchLpC0) {
    mPchNvsAreaProtocol.Area->Cio2EnabledAsAcpiDevice = 0;
    DEBUG ((DEBUG_INFO, "UpdateCio2AcpiData() Cio2 has not been enabled as ACPI device\n"));
  } else {
    mPchNvsAreaProtocol.Area->Cio2EnabledAsAcpiDevice = 0;
    if (mPchConfigHob->Cio2.DeviceEnable == 1) {
      mPchNvsAreaProtocol.Area->Cio2EnabledAsAcpiDevice = 1;
      for (Index = 0; Index < mPchConfigHob->Interrupt.NumOfDevIntConfig; Index++) {
        if ((mPchConfigHob->Interrupt.DevIntConfig[Index].Device == PCI_DEVICE_NUMBER_PCH_CIO2) &&
            (mPchConfigHob->Interrupt.DevIntConfig[Index].Function == PCI_FUNCTION_NUMBER_PCH_CIO2)) {
          mPchNvsAreaProtocol.Area->Cio2IrqNumber = mPchConfigHob->Interrupt.DevIntConfig[Index].Irq;
          DEBUG ((DEBUG_INFO, "UpdateCio2AcpiData() Cio2 has been enabled as ACPI device. Irq number = 0x%x\n", mPchConfigHob->Interrupt.DevIntConfig[Index].Irq));
          break;
        }
      }
    }
  }

  DEBUG ((DEBUG_INFO, "UpdateCio2AcpiData() End\n"));

  return EFI_SUCCESS;
}


