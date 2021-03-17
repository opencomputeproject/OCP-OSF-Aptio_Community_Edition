/** @file

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SIO_REG_H_
#define _SIO_REG_H_

typedef struct {
  UINT8 Index;
  UINT8 Value;
} SIO_INDEX_DATA;

#define REG_LOGICAL_DEVICE        0x07
#define ACTIVATE                  0x30

#define BASE_ADDRESS_HIGH0        0x60
#define BASE_ADDRESS_LOW0         0x61
#define INTERRUPT_TYPE            0x71

#define SIO_INDEX_PORT  0x2E
#define SIO_DATA_PORT   0x2F

#define SIO_UART1       0x02
#define SIO_SMI         0x0D
#define SIO_MAILBOX     0x0E

#define SIO_UNLOCK      0xA5
#define SIO_LOCK        0xAA

#define EXIST BIT4

#endif
