## @file
#  Component description file for the SkyLake SiPkg DXE drivers.
#
# Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
#
# This program and the accompanying materials are licensed and made available under
# the terms and conditions of the BSD License which accompanies this distribution.
# The full text of the license may be found at
# http://opensource.org/licenses/bsd-license.php
#
# THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
# WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
##


#
# Common
#

#
# Pch
#
  $(PLATFORM_SI_PACKAGE)/Pch/PchInit/Dxe/PchInitDxe.inf
  $(PLATFORM_SI_PACKAGE)/Pch/SmmControl/RuntimeDxe/SmmControl.inf

  $(PLATFORM_SI_PACKAGE)/Pch/Spi/Smm/PchSpiSmm.inf



  $(PLATFORM_SI_PACKAGE)/Pch/PchSmiDispatcher/Smm/PchSmiDispatcher.inf
  $(PLATFORM_SI_PACKAGE)/Pch/PchInit/Smm/PchInitSmm.inf

#
# SystemAgent
#
  $(PLATFORM_SI_PACKAGE)/SystemAgent/SmmAccess/Dxe/SmmAccess.inf
  $(PLATFORM_SI_PACKAGE)/SystemAgent/SaInit/Dxe/SaInitDxe.inf {
    <LibraryClasses>
!if $(TARGET) == DEBUG
      DebugLib|MdePkg/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
!endif
  }


!if gSiPkgTokenSpaceGuid.PcdAcpiEnable == TRUE
  $(PLATFORM_SI_PACKAGE)/SystemAgent/AcpiTables/SaAcpiTables.inf
  $(PLATFORM_SI_PACKAGE)/SystemAgent/AcpiTables/SaSsdt/SaSsdt.inf
!endif

#
# Cpu
#




#
# Me
#






  $(PLATFORM_SI_PACKAGE)/Hsti/Dxe/HstiSiliconDxe.inf {
    <LibraryClasses>
      HstiLib|MdePkg/Library/DxeHstiLib/DxeHstiLib.inf
!if $(TARGET) == DEBUG
      DebugLib|MdePkg/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
!endif
  }
