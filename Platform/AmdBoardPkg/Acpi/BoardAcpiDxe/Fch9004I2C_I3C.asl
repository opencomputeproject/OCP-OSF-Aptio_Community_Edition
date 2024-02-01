/*****************************************************************************
 *
 * Copyright (C) 2021-2024 Advanced Micro Devices, Inc. All rights reserved.
 *
 *****************************************************************************/

// Name (TSOS, 0x75)

// If(CondRefOf(\_OSI))
// {
//   If(\_OSI("Windows 2009"))
//   {
//     Store(0x50, TSOS)
//   }
//   If(\_OSI("Windows 2015"))
//   {
//     Store(0x70, TSOS)
//   }
// }

Scope(\_SB) {

OperationRegion(ECMC, SystemIo, 0x72, 0x02)
Field(ECMC, AnyAcc, NoLock, Preserve)
{
  ECMI, 8,
  ECMD, 8,
}
IndexField(ECMI, ECMD, ByteAcc, NoLock, Preserve) {
  Offset (0x08),
  FRTB, 32,
}
OperationRegion(FRTP, SystemMemory, FRTB, 0x100)
Field(FRTP, AnyAcc, NoLock, Preserve)
{
  PEBA, 64,
  Offset (0x08),
  , 5,
  IC0E, 1,   //I2C0, 5
  IC1E, 1,   //I2C1, 6
  IC2E, 1,   //I2C2, 7
  IC3E, 1,   //I2C3, 8
  IC4E, 1,   //I2C3, 9
  IC5E, 1,   //I2C3, 10
  , 1,   //UART0, 11
  , 1,   //UART1, 12
  I31E, 1,   //I3C1   13
  I32E, 1,   //I3C2   14
  I33E, 1,   //I3C3 , 15
  , 1,   //UART2, 16
  , 1,
  , 2,   //18-19, EMMC Driver type, 0:AMD eMMC Driver (AMDI0040) 1:MS SD Driver (PNP0D40) 2:0:MS eMMC Driver (AMDI0040)
  , 1,   //UART4, 20
  I30E, 1,   //I3C0, 21
  , 1,
  XHCE, 1,   //XCHI, 23
  SD_E, 1,   //SD,   24
  , 2,
  , 1,   //ESPI  27
  Offset (0x0C),
  PCEF, 1,   // Post Code Enable Flag
  , 4,
  IC0D, 1,   //I2C0, 5
  IC1D, 1,
  IC2D, 1,
  IC3D, 1,   //I2C3, 8
  IC4D, 1,   //I2C3, 9
  IC5D, 1,   //I2C3, 10
  , 1,   //UART0, 11
  , 1,   //UART1, 12
  , 1,   //UART1, 13
  , 1,   //UART1, 14
  , 1,   //SATA, 15
  , 2,
  , 1,   //EHCI, 18
  , 4,
  , 1,   //XCHI, 23
  , 1,   //SD,   24
  , 6,
  , 1,   //S0I3 flag, 31
  Offset (0x10),
  , 16,
  , 32,
  , 16,
  , 32,
  , 8, //SataDevSlpPort0S5Pin
  , 8, //SataDevSlpPort1S5Pin
  , 1, //Carrizo Serials
  Offset (0x24),
  I20Q, 8,    // I2c0Irq
  I21Q, 8,    // I2c1Irq
  I22Q, 8,    // I2c2Irq
  I23Q, 8,    // I2c3Irq
  I24Q, 8,    // I2c4Irq
  I25Q, 8,    // I2c5Irq
}


  Method(SRAD,2, Serialized)  //SoftResetAoacDevice, Arg0:Device ID, Arg1:reset period in micro seconds
  {
    ShiftLeft(Arg0, 1, Local0)
    Add (Local0, 0xfed81e40, Local0)
    OperationRegion( ADCR, SystemMemory, Local0, 0x02)
    Field( ADCR, ByteAcc, NoLock, Preserve) { //AoacD3ControlRegister
      ADTD, 2,
      ADPS, 1,
      ADPD, 1,
      ADSO, 1,
      ADSC, 1,
      ADSR, 1,
      ADIS, 1,
      ADDS, 3,
    }
    store (one, ADIS)       // IsSwControl = 1
    store (zero, ADSR)      // SwRstB = 0
    stall (Arg1)
    store (one, ADSR)       // SwRstB = 1
    store (zero, ADIS)      // IsSwControl = 0
    stall (Arg1)
  }

  Device(I2CA) {
    Name(_HID,"AMDI0010")            // Hardware Device ID
    Name(_UID,0x0)
    Method(_CRS, 0, Serialized) {
      Name(BUF0, ResourceTemplate(){
        IRQ(Edge, ActiveHigh, Exclusive) {10}
        Memory32Fixed(ReadWrite, 0xFEDC2000, 0x1000)
      })
      // Create pointers to the specific byte
      CreateWordField (BUF0, 0x01, IRQW)
      //Modify the IRQ
      ShiftLeft (One, And (I20Q, 0x0F), IRQW)
      Return(BUF0) // return the result
    }// end _CRS method

    Method(_STA, 0, NotSerialized) {
        if (LEqual(IC0E, one)) {
            Return (0x0F)
        } Else {
          Return (0x00)
        }
    }
    Method(RSET,0) { SRAD (5, 200)}
  } // End Device I2CA

  Device(I2CB)
  {
    Name(_HID,"AMDI0010")            // Hardware Device ID
    Name(_UID,0x1)
    Method(_CRS, 0, Serialized) {
      Name(BUF0, ResourceTemplate(){
        IRQ(Edge, ActiveHigh, Exclusive) {11}
        Memory32Fixed(ReadWrite, 0xFEDC3000, 0x1000)
      })
      // Create pointers to the specific byte
      CreateWordField (BUF0, 0x01, IRQW)
      //Modify the IRQ
      ShiftLeft (One, And (I21Q, 0x0F), IRQW)
      Return(BUF0) // return the result
    }// end _CRS method

    Method(_STA, 0, NotSerialized) {
        if (LEqual(IC1E, one)) {
            Return (0x0F)
        } Else {
          Return (0x00)
        }
    }
    Method(RSET,0) { SRAD (6, 200)}
  } // End Device I2CB

  Device(I2CC) {
    Name(_HID,"AMDI0010")            // Hardware Device ID
    Name(_UID,0x2)
    Method(_CRS, 0, Serialized) {
      Name(BUF0, ResourceTemplate(){
        IRQ(Edge, ActiveHigh, Exclusive) {4}
        Memory32Fixed(ReadWrite, 0xFEDC4000, 0x1000)
      })
      // Create pointers to the specific byte
      CreateWordField (BUF0, 0x01, IRQW)
      //Modify the IRQ
      ShiftLeft (One, And (I22Q, 0x0F), IRQW)
      Return(BUF0) // return the result
    }// end _CRS method

    Method(_STA, 0, NotSerialized) {
        if (LEqual(IC2E, one)) {
            Return (0x0F)
        } Else {
          Return (0x00)
        }
    }
    Method(RSET,0) { SRAD (7, 200)}
  } // End Device I2CC

  Device(I2CD) {
    Name(_HID,"AMDI0010")            // Hardware Device ID
    Name(_UID,0x3)
    Method(_CRS, 0, Serialized) {
      Name(BUF0, ResourceTemplate(){
        IRQ(Edge, ActiveHigh, Exclusive) {6}
        Memory32Fixed(ReadWrite, 0xFEDC5000, 0x1000)
      })
      // Create pointers to the specific byte
      CreateWordField (BUF0, 0x01, IRQW)
      //Modify the IRQ
      ShiftLeft (One, And (I23Q, 0x0F), IRQW)
      Return(BUF0) // return the result
    }// end _CRS method

    Method(_STA, 0, NotSerialized) {
        if (LEqual(IC3E, one)) {
            Return (0x0F)
        } Else {
          Return (0x00)
        }
    }
    Method(RSET,0) { SRAD (8, 200)}
  } // End Device I2CD

  Device(I2CE) {
    Name(_HID,"AMDI0010")            // Hardware Device ID
    Name(_UID,0x4)
    Method(_CRS, 0, Serialized) {
      Name(BUF0, ResourceTemplate(){
        IRQ(Edge, ActiveHigh, Exclusive) {14}
        Memory32Fixed(ReadWrite, 0xFEDC6000, 0x1000)
      })
      // Create pointers to the specific byte
      CreateWordField (BUF0, 0x01, IRQW)
      //Modify the IRQ
      ShiftLeft (One, And (I24Q, 0x0F), IRQW)
      Return(BUF0) // return the result
    }// end _CRS method

    Method(_STA, 0, NotSerialized) {
        if (LEqual(IC4E, one)) {
            Return (0x0F)
        } Else {
          Return (0x00)
        }
    }
    Method(RSET,0) { SRAD (9, 200)}
  } // End Device I2CE

  Device(I2CF) {
    Name(_HID,"AMDI0010")            // Hardware Device ID
    Name(_UID,0x5)
    Method(_CRS, 0, Serialized) {
      Name(BUF0, ResourceTemplate(){
        IRQ(Edge, ActiveHigh, Exclusive) {15}
        Memory32Fixed(ReadWrite, 0xFEDCB000, 0x1000)
      })
      // Create pointers to the specific byte
      CreateWordField (BUF0, 0x01, IRQW)
      //Modify the IRQ
      ShiftLeft (One, And (I25Q, 0x0F), IRQW)
      Return(BUF0) // return the result
    }// end _CRS method

    Method(_STA, 0, NotSerialized) {
        if (LEqual(IC5E, one)) {
            Return (0x0F)
        } Else {
          Return (0x00)
        }
    }
    Method(RSET,0) { SRAD (10, 200)}
  } // End Device I2CF

  Name(I3ID,"AMDI0015")             // AMD I3C Driver ID
  Name(I2ID,"AMDI0016")

  Device(I3CA) {
    Method(_HID, 0, Serialized) {
        Return (I3ID)
    }
    Name(_UID,0x0)
    Method(_CRS, 0, Serialized) {
      Name(BUF0, ResourceTemplate(){
        IRQ(Edge, ActiveHigh, Exclusive) {10}
      Memory32Fixed(ReadWrite, 0xFEDD2000, 0x1000)
      })
      // Create pointers to the specific byte
      CreateWordField (BUF0, 0x01, IRQW)
      //Modify the IRQ
      ShiftLeft (One, And (I20Q, 0x0F), IRQW)
      Return(BUF0) // return the result
    }// end _CRS method

    Method(_STA, 0, NotSerialized) {
        if (LEqual(I30E, one)) {
            Return (0x0F)
        } Else {
          Return (0x00)
        }
    }
    Method(RSET,0) { SRAD (21, 200)}

  } // End Device I3CA

  Device(I3CB) {
    Method(_HID, 0, Serialized) {
        Return (I3ID)
    }
    Name(_UID,0x1)
    Method(_CRS, 0, Serialized) {
      Name(BUF0, ResourceTemplate(){
        IRQ(Edge, ActiveHigh, Exclusive) {11}
      Memory32Fixed(ReadWrite, 0xFEDD3000, 0x1000)
      })
      // Create pointers to the specific byte
      CreateWordField (BUF0, 0x01, IRQW)
      //Modify the IRQ
      ShiftLeft (One, And (I21Q, 0x0F), IRQW)
      Return(BUF0) // return the result
    }// end _CRS method

    Method(_STA, 0, NotSerialized) {
        if (LEqual(I31E, one)) {
            Return (0x0F)
        } Else {
          Return (0x00)
        }
    }

    Method(RSET,0) { SRAD (13, 200)}

  } // End Device I3CB

  Device(I3CC) {
    Method(_HID, 0, Serialized) {
        Return (I3ID)
    }
    Name(_UID,0x2)
    Method(_CRS, 0, Serialized) {
      Name(BUF0, ResourceTemplate(){
        IRQ(Edge, ActiveHigh, Exclusive) {4}
      Memory32Fixed(ReadWrite, 0xFEDD4000, 0x1000)
      })
      // Create pointers to the specific byte
      CreateWordField (BUF0, 0x01, IRQW)
      //Modify the IRQ
      ShiftLeft (One, And (I22Q, 0x0F), IRQW)
      Return(BUF0) // return the result
    }// end _CRS method

    Method(_STA, 0, NotSerialized) {
        if (LEqual(I32E, one)) {
            Return (0x0F)
        } Else {
          Return (0x00)
        }
    }
    Method(RSET,0) { SRAD (14, 200)}
  } // End Device I3CC

  Device(I3CD) {
    Method(_HID, 0, Serialized) {
       Return (I3ID)
    }
    Name(_UID,0x3)
    Method(_CRS, 0, Serialized) {
      Name(BUF0, ResourceTemplate(){
        IRQ(Edge, ActiveHigh, Exclusive) {6}
      Memory32Fixed(ReadWrite, 0xFEDD6000, 0x1000)
      })
      // Create pointers to the specific byte
      CreateWordField (BUF0, 0x01, IRQW)
      //Modify the IRQ
      ShiftLeft (One, And (I22Q, 0x0F), IRQW)
      Return(BUF0) // return the result
    }// end _CRS method

    Method(_STA, 0, NotSerialized) {
        if (LEqual(I33E, one)) {
            Return (0x0F)
        } Else {
          Return (0x00)
        }
    }
    Method(RSET,0) { SRAD (15, 200)}
  } // End Device I3CD

} // Scope SB

