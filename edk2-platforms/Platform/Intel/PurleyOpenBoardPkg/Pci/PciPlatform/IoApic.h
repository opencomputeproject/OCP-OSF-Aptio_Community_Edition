/** @file

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _IOAPIC_H_
#define _IOAPIC_H_

#define EFI_IO_APIC_INDEX_OFFSET          0x00
#define EFI_IO_APIC_DATA_OFFSET           0x10
#define EFI_IO_APIC_IRQ_ASSERTION_OFFSET  0x20
#define EFI_IO_APIC_EOI_OFFSET            0x40

#define EFI_IO_APIC_ID_REGISTER           0x0
#define EFI_IO_APIC_ID_BITSHIFT           24
#define EFI_IO_APIC_VER_REGISTER          0x1
#define EFI_IO_APIC_BOOT_CONFIG_REGISTER  0x3
#define EFI_IO_APIC_FSB_INT_DELIVERY      0x1

#endif
