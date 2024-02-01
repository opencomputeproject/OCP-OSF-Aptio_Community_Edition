/*****************************************************************************
 *
 * Copyright (C) 2020-2024 Advanced Micro Devices, Inc. All rights reserved.
 *
 *****************************************************************************/

/**
 * 
 @file  AmdPspDxe.c
 @brief DXE driver to initialize PSP bar and enable relevant CPU features (rdseed) on all CPU cores.

**/

/**
 * 
 * Note: At the time of this driver development, openSIL resource manager IP API is not available in DXE.
 * Until RC MGR IP is added to TP2 IP list, use direct calls to the resource manager.
 * 
 **/

#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <MsrReg.h>
#include <RcMgr/FabricResourceManager.h>
#include <Pci.h>
#include <CommonLib/SmnAccess.h>
#include <Protocol/MpService.h>
#include <SMU/SmuIp2Ip.h>
#include <DF/DfIp2Ip.h>
#include <CoreTopologyService.h>
#include <SilHob.h>
#include <Include/Pi/PiHob.h>
#include <Include/Library/HobLib.h>

#define CCP_BAR_SIZE            0x100000ul
#define PSP_BAR_SIZE            0x100000ul

#define NB_SMN_INDEX_2_PCI_ADDR (MAKE_SBDFO(0, 0, 0, 0, 0xB8))
#define NB_SMN_DATA_2_PCI_ADDR (MAKE_SBDFO(0, 0, 0, 0, 0xBC))

#define IOHC0NBMSIC_SMN_BASE    0x13B10000ul

///< Define the offsets of HSP_BASE_ADDR in NB MISC block
#define NBMSIC_CCP_BASE_ADDR_LO_OFFSET   0x2D8ul
#define NBMSIC_CCP_BASE_ADDR_HI_OFFSET   0x2DCul
#define NBMSIC_PSP_BASE_ADDR_LO_OFFSET   0x2E0ul
#define NBMSIC_PSP_BASE_ADDR_HI_OFFSET   0x2E4ul

#define PSP_BASE_MSR          0xC00110A2
#define CPUID_FEATURES_MSR    0xC0011004
  
VOID        PspBarInitEarlyV2 (VOID);
EFI_STATUS  EnableRdInstructionEarly (VOID);
BOOLEAN     X86RdSeed64(UINT64 *);

/**
 *
 *  Get PSP SMN base address and root bridge number. Input parameters are the pointers to the caller's data
 *
 *  @param[in, out]   SmnBase       SMN Base Pointer
 *  @param[in, out]   RbNumber      Root Bridge number
 *
 **/
VOID
EFIAPI
GetPspIOHCxNbMiscSmnBase(
    IN OUT UINT32 *SmnBase,
    IN OUT UINT8 *RbNumber)
{
  *SmnBase = IOHC0NBMSIC_SMN_BASE;
  if (RbNumber != NULL) {
    *RbNumber = 0;
  }
}

 /**
  *
  * @brief  Initialize CCP Bar on a given die
  * 
  * Allocate MMIO region for CCP and program its bar register
  *
  * @param[in]      PspAddr                 Value written to Psp Bar address
  * @param[in]      UINT32 SocketNum        CPU socket
  * @param[in]      UINT32 RbNum            Root bridge
  * @param[in]      UINT32 SmuRegInstanceId SMU register instance
  * @param[in, out] UINT64 *CcpMmioBase     CCP base address
  *
  */
VOID
PspCcpBarInitOnDie (
  IN        UINT32 SocketNum,
  IN        UINT32 RbNum,
  IN        UINT32 SmuRegInstanceId,
  IN OUT    UINT64 *CcpMmioBase
  )
{
  SIL_STATUS                           SilStatus;
  UINT32                               Value32;
  UINT64                               Length;
  UINT8                                RbNumber;
  FABRIC_TARGET                        MmioTarget;
  FABRIC_MMIO_ATTRIBUTE                Attributes;
  UINT32                               SmnBase;
  UINT64                               PspMmioBase;
  //RCMGR_IP2IP_API                      *RcMgrIp2Ip;
  SMU_IP2IP_API                        *SmuApi;

//  if (SilGetIp2IpApi(SilId_RcManager, (void **)(&RcMgrIp2Ip)) != SilPass) {
//    DEBUG ((DEBUG_ERROR, "PspCcpBarInitOnDie: MMIO allocator API is not found.\n"));
//    return;
//  }

  if (SilGetIp2IpApi (SilId_SmuClass, (void **)&SmuApi)) {
    DEBUG ((DEBUG_ERROR, "PspCcpBarInitOnDie: SMU API is not found.\n"));
    return;
  }

  RbNumber = 0;
  ZeroMem (&MmioTarget, sizeof (FABRIC_TARGET));

  GetPspIOHCxNbMiscSmnBase (&SmnBase, NULL);

  //Allocate MMIO Region from MMIO manager
  Length = CCP_BAR_SIZE;
  MmioTarget.TgtType = TARGET_RB;
  MmioTarget.SocketNum = (UINT8) SocketNum;
  MmioTarget.RbNum = RbNumber;
  Attributes.ReadEnable = 1;
  Attributes.WriteEnable = 1;
  Attributes.NonPosted = 0;
  Attributes.MmioType = NON_PCI_DEVICE_BELOW_4G;

//  SilStatus = RcMgrIp2Ip->FabricReserveMmio (CcpMmioBase, &Length, ALIGN_1M, MmioTarget, &Attributes);
  SilStatus = FabricReserveMmio (CcpMmioBase, &Length, ALIGN_1M, MmioTarget, &Attributes);
  if (SilStatus != SilPass)
  {
    DEBUG ((DEBUG_ERROR, "CcpBarInitOnDie: Allocate MMIO Fail\n"));
    ASSERT (FALSE);
    return;
  }
  DEBUG ((DEBUG_INFO, "CcpBarInitOnDie: CCP MMIO base @0x%llx\n", *CcpMmioBase));

  //Set CCP BASE Address in NBMISC, and enable lock the MMIO
  Value32 = (UINT32) (*CcpMmioBase | (BIT0 + BIT8));
  SmuApi->SmuRegisterWrite (SmuRegInstanceId, SmnBase + NBMSIC_CCP_BASE_ADDR_LO_OFFSET, &Value32);
  Value32 = (UINT32) RShiftU64 (*CcpMmioBase, 32);
  SmuApi->SmuRegisterWrite (SmuRegInstanceId, SmnBase + NBMSIC_CCP_BASE_ADDR_HI_OFFSET, &Value32);

  //Allocate MMIO Region from MMIO manager
  Length = PSP_BAR_SIZE;
  MmioTarget.TgtType = TARGET_RB;
  MmioTarget.SocketNum = (UINT8) SocketNum;
  MmioTarget.RbNum = RbNumber;
  Attributes.ReadEnable = 1;
  Attributes.WriteEnable = 1;
  Attributes.NonPosted = 0;
  Attributes.MmioType = NON_PCI_DEVICE_BELOW_4G;

  //SilStatus = RcMgrIp2Ip->FabricReserveMmio (&PspMmioBase, &Length, ALIGN_1M, MmioTarget, &Attributes);
  SilStatus = FabricReserveMmio (&PspMmioBase, &Length, ALIGN_1M, MmioTarget, &Attributes);
  if (SilStatus != SilPass)
  {
    DEBUG ((DEBUG_ERROR, "PspBarInitOnDie: Allocate MMIO Fail\n"));
    ASSERT(FALSE);
    return;
  }
  DEBUG((DEBUG_INFO, "PspBarInitOnDie: PSP MMIO base @0x%llx\n", PspMmioBase));

  //Set PSP BASE Address in NBMISC, and enable lock the MMIO
  Value32 = (UINT32) (PspMmioBase | (BIT0 + BIT8));
  SmuApi->SmuRegisterWrite (SmuRegInstanceId, SmnBase + NBMSIC_PSP_BASE_ADDR_LO_OFFSET, &Value32);
  Value32 = (UINT32) RShiftU64 (PspMmioBase, 32);
  SmuApi->SmuRegisterWrite (SmuRegInstanceId, SmnBase + NBMSIC_PSP_BASE_ADDR_HI_OFFSET, &Value32);
}

 /**
  *
  * Programs the Psp Address register to the desired value.
  *
  * @param[in]      PspAddr                Value written to Psp Bar address
  *
  */
VOID
UpdatePspAddr (
  IN       UINT64 PspAddr
  )
{
  AsmWriteMsr64 (PSP_BASE_MSR, PspAddr);
  PspAddr = AsmReadMsr64 (PSP_BASE_MSR);
}

/**
 * @brief Set the Psp Addr Msr
 *
 * @param Buffer PSP MMIO Address
 * @return VOID
 */
VOID
EFIAPI
SetPspAddrMsrTask (
  IN  VOID  *Buffer
  )
{
  UINT64 Tmp64;
  //Set PSP BAR Address
  Tmp64 = 0;
  Tmp64 = *((UINT64 *)Buffer);

  UpdatePspAddr (Tmp64);
}

/**
 * @brief Get MMIO for CCP on socket 0
 *
 * @return EFI_STATUS
 */
EFI_STATUS
GetCcpMmioBase (
  UINT64                 *CcpMmioBase
  )
{
  UINT32  MmioBaseLo;
  UINT32  MmioBaseHi;
  UINT32  SmnBase;

  GetPspIOHCxNbMiscSmnBase (&SmnBase, NULL);

  MmioBaseLo = xUSLSmnRead (0, 0, SmnBase + NBMSIC_CCP_BASE_ADDR_LO_OFFSET);
  MmioBaseLo &= 0xFFF00000;
  if (MmioBaseLo == 0) {
    DEBUG ((DEBUG_ERROR, "CCP MMIO not initialized\n"));
    //Not initialed
    return EFI_NOT_READY;
  }
  MmioBaseHi = xUSLSmnRead (0, 0, SmnBase + NBMSIC_CCP_BASE_ADDR_HI_OFFSET);

  *CcpMmioBase = ((UINT64) MmioBaseHi << 32) + MmioBaseLo;
  return EFI_SUCCESS;
}

/**
 * @brief Enable RDRAND RDSEED Instructions on APs
 *
 * @return EFI_STATUS
 */
EFI_STATUS
EnableRdInstruction (
  VOID
  )
{
  EFI_STATUS  Status;
  EFI_MP_SERVICES_PROTOCOL  *MpServices;
  UINT64                    CcpMmioBase;
  UINT32                    SmuRegInstanceId;
  UINT32                    LogicalCoreId;
  UINT32                    CcpBarInitFlag;
  UINT32                    Socket;
  UINT32                    Die;
  UINT32                    Ccd;
  UINT32                    Ccx;
  UINT32                    Core;
  UINT32                    Thread;
  UINT32                    NumberOfSockets;
  UINT32                    NumberOfDies;
  UINT32                    NumberOfCcds;
  UINT32                    NumberOfComplexes;
  UINT32                    NumberOfCores;
  UINT32                    NumberOfThreads;
  SIL_STATUS                SilStatus;
  DF_IP2IP_API*             DfApi;

  //
  // Always Initialize PSP BAR to support RdRand Instruction
  //
  DEBUG ((DEBUG_INFO,"\tPsp BAR init\n"));
  PspBarInitEarlyV2 ();

  Status = EnableRdInstructionEarly ();

  SilStatus = SilGetIp2IpApi (SilId_DfClass, (void **)&DfApi);
  if (SilStatus != SilPass) {
    DEBUG ((DEBUG_ERROR, "EnableRdInstruction: DF API not found!\n"));
    return EFI_NOT_FOUND;
  }

  Status = gBS->LocateProtocol (&gEfiMpServiceProtocolGuid, NULL, (VOID **)&MpServices);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "EnableRdInstruction: Locate MP Protocol fail\n"));
    return EFI_UNSUPPORTED;
  }

  DfApi->DfGetSystemInfo (&NumberOfSockets, NULL, NULL, NULL, NULL);

  SmuRegInstanceId = 0;
  LogicalCoreId = 0;
  CcpBarInitFlag = 0;

  for (Socket = 0; Socket < NumberOfSockets; Socket++) {
    DfApi->DfGetProcessorInfo (Socket, &NumberOfDies, NULL);

    for (Die = 0; Die < NumberOfDies; Die++) {
      SilStatus = GetCoreTopologyOnDie (
        Socket,
        Die,
        &NumberOfCcds,
        &NumberOfComplexes,
        &NumberOfCores,
        &NumberOfThreads
        );
      if (SilStatus != SilPass) {
        DEBUG ((DEBUG_ERROR, "EnableRdInstruction ERROR: can not get core topology for skt %d die %d\n", Socket, Die));
        return EFI_DEVICE_ERROR;
      }

      //PSP BAR already initialized on main die, skip it
      if ((Socket == 0) && (Die == 0)) {
        CcpMmioBase = 0;
        if (EFI_ERROR (GetCcpMmioBase (&CcpMmioBase))) {
          DEBUG ((DEBUG_ERROR, "GetCcpMmioBase get fail\n"));
          return EFI_DEVICE_ERROR;
        }
      } else {
        CcpMmioBase = 0;
        if (CcpBarInitFlag != ((Socket << 16) + Die)) {
          DEBUG ((DEBUG_INFO, "Setup PSP BAR on socket-0x%x die-0x%x\n", Socket, Die));
          PspCcpBarInitOnDie (Socket, Die, SmuRegInstanceId, &CcpMmioBase);
          CcpBarInitFlag = (Socket << 16) + Die;
        }
      }
      SmuRegInstanceId ++;
      for (Ccd = 0; Ccd < NumberOfCcds; Ccd++) {
        for (Ccx = 0; Ccx < NumberOfComplexes; Ccx++) {
          for (Core = 0; Core < NumberOfCores; Core++) {
            for (Thread = 0; Thread < NumberOfThreads; Thread++) {
              if (LogicalCoreId != 0) {
                DEBUG ((DEBUG_INFO, "Set PSPADDR MSR 0x%x for core 0x%x [%x-%x-%x-%x-%x-%x] \n",
                  CcpMmioBase, LogicalCoreId, Socket, Die, Ccd, Ccx, Core, Thread));
                Status = MpServices->StartupThisAP (
                                      MpServices,                                    // EFI_MP_SERVICES_PROTOCOL *this
                                      (EFI_AP_PROCEDURE) SetPspAddrMsrTask,          // EFI_AP_PROCEDURE
                                      LogicalCoreId,
                                      NULL,                                          // EFI_EVENT WaitEvent OPTIONAL
                                      0,                                             // UINTN Timeout (Unsupported)
                                      &CcpMmioBase,                                  // VOID *ProcArguments OPTIONAL
                                      NULL                                           // Failed CPUList OPTIONAL (unsupported)
                                      );
              }
              LogicalCoreId ++;
            }
          }
        }
      }
    }
  }

  return EFI_SUCCESS;
}

/**
 * @brief Return the PspMMIO MMIO location
 *
 * @param[in, out] PspMmioBase Pointer to Psp MMIO address
 *
 * @retval BOOLEAN  FALSE: Error, TRUE Success
 */
BOOLEAN
GetPspMmioBase(
    IN OUT UINT32 *PspMmioBase)
{
  UINT32 Value32;
  UINT32 PciAddress;
  UINT32 SmnBase;

  *PspMmioBase = 0;

  GetPspIOHCxNbMiscSmnBase (&SmnBase, NULL);

  PciAddress = NB_SMN_INDEX_2_PCI_ADDR;
  Value32 = SmnBase + NBMSIC_PSP_BASE_ADDR_LO_OFFSET;
  xUSLPciWrite32 (PciAddress, Value32);
  PciAddress = NB_SMN_DATA_2_PCI_ADDR;
  Value32 = xUSLPciRead32 (PciAddress);
  // Mask out the lower bits
  Value32 &= 0xFFF00000;

  if (Value32 == 0)
  {
    return (FALSE);
  }

  *PspMmioBase = Value32;
  return (TRUE);
}

/**
 * @brief Psp Bar initialize
 */
VOID PspBarInitEarlyV2 (VOID)
{
  UINT32        Value32;
  UINT64        PspMmioBase;
  UINT32        PciAddress;
  UINT64        Length;
  FABRIC_TARGET MmioTarget;
  SIL_STATUS    SilStatus;
  FABRIC_MMIO_ATTRIBUTE Attributes;
  UINT32        SmnBase;
  UINT8         RbNumber;
  UINT64        ApicBar;
//  RCMGR_IP2IP_API *RcMgrIp2Ip;

  SmnBase = 0;
  RbNumber = 0;

  // is this BSP core?
  ApicBar = AsmReadMsr64 (MSR_APIC_BAR);
  DEBUG((DEBUG_INFO, "APIC Bar: 0x%lx\n", ApicBar));
  if ((ApicBar & BIT8) == 0) {
    return; // not BSP
  }

  // Check if PSP BAR has been assigned, if not do the PSP BAR initialation
  if (GetPspMmioBase (&Value32) == TRUE) {
    DEBUG((DEBUG_INFO, "PspBarInitEarlyV2: PSP BAR has already been initialized.\n"));
    return;
  }
  DEBUG((DEBUG_INFO, "GetPspMmioBase: 0x%x\n", Value32));

//  if (SilGetIp2IpApi(SilId_RcManager, (void **)(&RcMgrIp2Ip)) != SilPass)
//  {
//    DEBUG((DEBUG_ERROR, "PspBarInitEarlyV2: MMIO allocator API is not found.\n"));
//    return;
//  }
//  DEBUG((DEBUG_INFO, "RcMgrIp2Ip: 0x%x\n", RcMgrIp2Ip));

  GetPspIOHCxNbMiscSmnBase (&SmnBase, &RbNumber);

  // Allocate MMIO Region from MMIO manager
  Length = PSP_BAR_SIZE;
  MmioTarget.TgtType = TARGET_RB;
  MmioTarget.SocketNum = 0;
  MmioTarget.RbNum = RbNumber;
  Attributes.ReadEnable = 1;
  Attributes.WriteEnable = 1;
  Attributes.NonPosted = 0;
  Attributes.MmioType = NON_PCI_DEVICE_BELOW_4G;
  PspMmioBase = 0;

  //SilStatus = RcMgrIp2Ip->FabricReserveMmio (&PspMmioBase, &Length, ALIGN_1M, MmioTarget, &Attributes);
  SilStatus = FabricReserveMmio (&PspMmioBase, &Length, ALIGN_1M, MmioTarget, &Attributes);
  if (SilStatus != SilPass)
  {
    DEBUG ((DEBUG_ERROR, "PspBarInitEarlyV2: Allocate MMIO Fail\n"));
    ASSERT (FALSE);
    return;
  }
  DEBUG ((DEBUG_INFO, "PspBarInitEarlyV2: PSP MMIO base @0x%llx\n", PspMmioBase));

  // Set PSP BASE Address in NBMISC, and enable lock the MMIO
  PciAddress = NB_SMN_INDEX_2_PCI_ADDR;
  Value32 = SmnBase + NBMSIC_PSP_BASE_ADDR_LO_OFFSET;
  xUSLPciWrite32(PciAddress, Value32);
  PciAddress = NB_SMN_DATA_2_PCI_ADDR;
  Value32 = (UINT32)(PspMmioBase | (BIT0 + BIT8));
  xUSLPciWrite32 (PciAddress, Value32);

  PciAddress = NB_SMN_INDEX_2_PCI_ADDR;
  Value32 = SmnBase + NBMSIC_PSP_BASE_ADDR_HI_OFFSET;
  xUSLPciWrite32(PciAddress, Value32);
  PciAddress = NB_SMN_DATA_2_PCI_ADDR;
  Value32 = (UINT32)RShiftU64 (PspMmioBase, 32);
  xUSLPciWrite32 (PciAddress, Value32);
}

/**
 * @brief Enable MMIO for CCP
 *
 * @return EFI_STATUS
 */
EFI_STATUS
CcpBarInit(
    UINT64 *CcpMmioBase)
{
  UINT32                Value32;
  UINT64                MmioBase;
  UINT64                Length;
  FABRIC_TARGET         MmioTarget;
  SIL_STATUS            SilStatus;
  FABRIC_MMIO_ATTRIBUTE Attributes;
  UINT32                SmnBase;
//  RCMGR_IP2IP_API *RcMgrIp2Ip;

//  if (SilGetIp2IpApi(SilId_RcManager, (void **)(&RcMgrIp2Ip)) != SilPass)
//  {
//    DEBUG((DEBUG_ERROR, "CcpBarInit: MMIO allocator API is not found.\n"));
//    return EFI_NOT_FOUND;
//  }

  ZeroMem (&MmioTarget, sizeof(FABRIC_TARGET));

  GetPspIOHCxNbMiscSmnBase (&SmnBase, NULL);

  // Allocate MMIO Region from MMIO manager
  Length = CCP_BAR_SIZE;
  MmioTarget.TgtType = TARGET_RB;
  MmioTarget.SocketNum = 0;
  MmioTarget.RbNum = 0;
  Attributes.ReadEnable = 1;
  Attributes.WriteEnable = 1;
  Attributes.NonPosted = 0;
  Attributes.MmioType = NON_PCI_DEVICE_BELOW_4G;
  MmioBase = 0;
  //SilStatus = RcMgrIp2Ip->FabricReserveMmio (&MmioBase, &Length, ALIGN_1M, MmioTarget, &Attributes);
  SilStatus = FabricReserveMmio (&MmioBase, &Length, ALIGN_1M, MmioTarget, &Attributes);

  if (SilStatus != SilPass)
  {
    DEBUG ((DEBUG_ERROR, "CcpBarInit: Allocate MMIO Fail\n"));
    ASSERT (FALSE);
    return EFI_NOT_FOUND;
  }
  DEBUG ((DEBUG_INFO, "CcpBarInit: MMIO base @0x%llx\n", MmioBase));

  Value32 = (UINT32)(MmioBase | (BIT0 + BIT8)); // Enabled and Lock
  xUSLSmnWrite (0, 0, SmnBase + NBMSIC_CCP_BASE_ADDR_LO_OFFSET, Value32);
  Value32 = (UINT32)RShiftU64 (MmioBase, 32);
  xUSLSmnWrite (0, 0, SmnBase + NBMSIC_CCP_BASE_ADDR_HI_OFFSET, Value32);
  *CcpMmioBase = MmioBase;

  return EFI_SUCCESS;
}

/**
 * @brief Dump RDRAND, RDSEED related information
 *
 * @return VOID
 */
VOID DumpRdInstructionInfo()
{
  UINT32  Value32;
  UINT64  Value64;
  UINT64  RdValue1;
  UINT64  RdValue2;
  BOOLEAN RdStatus1;

  Value32 = 0;
  Value64 = 0;
  RdStatus1 = FALSE;
  // Print RdInstruction capability from CPUID
  //  CPUID_Fn00000001_ECX [Feature Identifiers] (Core::X86::Cpuid::FeatureIdEcx)
  //  BIT30 | RDRAND. Read-only. Reset: Fixed,1. RDRAND instruction support
  AsmCpuid(0x00000001, NULL, NULL, &Value32, NULL);
  DEBUG((DEBUG_INFO, "CPUID_Fn00000001_ECX_RDRAND %a\n", (Value32 & BIT30) ? "Supported" : "Unsupported"));
  // CPUID_Fn00000007_EBX_x00 [Structured Extended Feature Identifiers]
  // BIT18 | RDSEED. Read-only. Reset: Fixed,1. RDSEED is present.
  AsmCpuidEx(0x00000007, 0, NULL, &Value32, NULL, NULL);
  DEBUG((DEBUG_INFO, "CPUID_Fn00000007_EBX_RDSEED %a\n", (Value32 & BIT18) ? "Supported" : "Unsupported"));

  RdValue1 = 0xFFFFFFFFFFFFFFFFul;
  RdValue2 = 0xFFFFFFFFFFFFFFFFul;
  RdStatus1 = AsmRdRand64(&RdValue1);

  DEBUG((DEBUG_INFO, "RDRAND verify: RdValue1:0x%x RdValue2:0x%x\n", RdValue1, RdValue2));
  if (((RdStatus1 & RdStatus1) == FALSE) || (RdValue1 == 0xFFFFFFFFFFFFFFFFul) || (RdValue1 == RdValue2))
  {
    DEBUG((DEBUG_INFO, "%a\n", "FAIL"));
  }
  else
  {
    DEBUG((DEBUG_INFO, "%a\n", "PASS"));
  }

  RdValue1 = 0xFFFFFFFFul;
  RdValue2 = 0xFFFFFFFFul;
  RdStatus1 = X86RdSeed64(&RdValue1);
  DEBUG((DEBUG_INFO, "RDSEED verify: RdValue1:0x%llx RdValue2:0x%llx\n", RdValue1, RdValue2));
  if (((RdStatus1 & RdStatus1) == FALSE) || (RdValue1 == 0xFFFFFFFFFFFFFFFFul) || (RdValue1 == RdValue2))
  {
    DEBUG((DEBUG_INFO, "%a\n", "FAIL"));
  }
  else
  {
    DEBUG((DEBUG_INFO, "%a\n", "PASS"));
  }
}

#define HW_DEFAULT_VALUE_PSP_BAR_MSR 0

/**
 * @brief Enable RdRand and RdSeed Instructions on BSP
 *
 * @return EFI_STATUS
 */
EFI_STATUS
EnableRdInstructionEarly(
    VOID)
{
  UINT64 CcpMmioBase;
  volatile UINT64 PspMsrValue;
  EFI_STATUS Status;

  Status = CcpBarInit(&CcpMmioBase);
  if (EFI_ERROR(Status))
  {
    return Status;
  }
  PspMsrValue = 0;
  // PSP BAR MSR can not be set twice, otherwise, it will cause CPU exception
  PspMsrValue = AsmReadMsr64(PSP_BASE_MSR);
  if (PspMsrValue == HW_DEFAULT_VALUE_PSP_BAR_MSR)
  {
    // Setup PSP BAR MSR for main die
    DEBUG((DEBUG_INFO, "Set PspAddrMsr for BSP 0x%lx\n", CcpMmioBase));
    AsmWriteMsr64(PSP_BASE_MSR, CcpMmioBase);
  }
  DumpRdInstructionInfo();

  return EFI_SUCCESS;
}

/**
 * @brief Onyx PSP Init DXE driver entry point
 *
 * @param ImageHandle   Image handle of DXE driver
 * @param Systemtable   Pointer to UEFI system table
 *
 * @return EFI_SUCCESS
 */
EFI_STATUS
EFIAPI
PspDxeInit (
  IN       EFI_HANDLE           ImageHandle,
  IN       EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_HOB_GUID_TYPE  *Hob;
  UINT64             DataPointer;
  SIL_DATA_HOB       *SilDataHob;
  EFI_STATUS         Status;

  // the following code is required to initialize API pointers for openSIL IPs
  Hob = (EFI_HOB_GUID_TYPE *)GetFirstGuidHob(&gPeiOpenSilDataHobGuid);
  Hob++;
  SilDataHob = (SIL_DATA_HOB *)Hob;
  DataPointer = (UINT64)(SilDataHob->SilDataPointer);
  DEBUG ((DEBUG_INFO, "openSIL Data Block Location @0x%lx\n", DataPointer));

  Status = xSimAssignMemoryTp2 ((VOID *)DataPointer, 0);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) return Status;

  EnableRdInstruction ();

  return EFI_SUCCESS;
}
