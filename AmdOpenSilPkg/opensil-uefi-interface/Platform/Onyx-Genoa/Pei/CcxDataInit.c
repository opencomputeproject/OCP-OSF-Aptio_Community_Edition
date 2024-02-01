/* Copyright 2021-2024 Advanced Micro Devices, Inc. All rights reserved. */
/**
 * @file  CcxDataInit.c
 * @brief Initialize Ccx data prior to openSIL execution..
 *
 */

#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Sil-api.h>
#include <CcxClass-api.h>
#include <Library/PspDirectoryLib.h>
#include <CcxMicrocodePatch.h>
#include <Library/PeiServicesLib.h>
#include <Library/BaseMemoryLib.h>

extern EFI_GUID gPeiOpenSilCcxDownCoreDataGuid;

/**
 * SetCcxData
 *
 * @brief Set the data in Ccx init IP block
 * @details
 *      Locate the Ccx - the resource initialization IP block
 * @return EFI_SUCCESS or EFI_DEVICE_ERROR
 */
EFI_STATUS
SetCcxData (
  void
  )
{
  CCXCLASS_DATA_BLK *CcxData;
  uint64_t  UcodeEntryAddress;
  uint32_t  UcodeEntrySize;
  bool      UcodeBIOSEntryInfoStatus;
  bool      MicrocodeAddFound;
  uint8_t   InstanceNumber;
  MPB               *MicroCode;

  UcodeEntryAddress        = 0;
  UcodeEntrySize           = 0;
  UcodeBIOSEntryInfoStatus = FALSE;
  InstanceNumber           = 0;
  MicrocodeAddFound        = FALSE;

  CcxData = (CCXCLASS_DATA_BLK *)SilFindStructure (SilId_CcxClass,  0);
  DEBUG ((DEBUG_ERROR, "SIL Ccx memory block is found at: 0x%x \n", CcxData));
  if (CcxData == NULL) {
    return EFI_NOT_FOUND; // Could not find the IP input block
  }

  CcxData->CcxInputBlock.AmdApicMode                    = PcdGet8(PcdAmdApicMode);
  CcxData->CcxInputBlock.AmdDownCoreMode                = PcdGet8(PcdAmdDownCoreMode);
  CcxData->CcxInputBlock.AmdCcdMode                     = PcdGet8(PcdAmdCcdMode);
  CcxData->CcxInputBlock.AmdSmtMode                     = PcdGet8(PcdAmdSmtMode);
  CcxData->CcxInputBlock.AmdCoreDisCcd[0]               = PcdGet16(PcdAmdCoreDisCcd0);
  CcxData->CcxInputBlock.AmdCoreDisCcd[1]               = PcdGet16(PcdAmdCoreDisCcd1);
  CcxData->CcxInputBlock.AmdCoreDisCcd[2]               = PcdGet16(PcdAmdCoreDisCcd2);
  CcxData->CcxInputBlock.AmdCoreDisCcd[3]               = PcdGet16(PcdAmdCoreDisCcd3);
  CcxData->CcxInputBlock.AmdCoreDisCcd[4]               = PcdGet16(PcdAmdCoreDisCcd4);
  CcxData->CcxInputBlock.AmdCoreDisCcd[5]               = PcdGet16(PcdAmdCoreDisCcd5);
  CcxData->CcxInputBlock.AmdCoreDisCcd[6]               = PcdGet16(PcdAmdCoreDisCcd6);
  CcxData->CcxInputBlock.AmdCoreDisCcd[7]               = PcdGet16(PcdAmdCoreDisCcd7);
  CcxData->CcxInputBlock.AmdIbrsEn                      = PcdGetBool(PcdAmdIbrsEn);
  CcxData->CcxInputBlock.AmdBranchSampling              = PcdGetBool(PcdAmdBranchSampling);
  CcxData->CcxInputBlock.AmdSnpMemCover                 = PcdGet8(PcdAmdSnpMemCover);
  CcxData->CcxInputBlock.AmdVmplEnable                  = PcdGetBool(PcdAmdVmplEnable);
  CcxData->CcxInputBlock.AmdSnpMemSize                  = PcdGet32 (PcdAmdSnpMemSize);
  CcxData->CcxInputBlock.AmdCoreDisCcd[8]               = PcdGet16(PcdAmdCoreDisCcd8);
  CcxData->CcxInputBlock.AmdCoreDisCcd[9]               = PcdGet16(PcdAmdCoreDisCcd9);
  CcxData->CcxInputBlock.AmdCoreDisCcd[10]              = PcdGet16(PcdAmdCoreDisCcd10);
  CcxData->CcxInputBlock.AmdCoreDisCcd[11]              = PcdGet16(PcdAmdCoreDisCcd11);
  CcxData->CcxInputBlock.AmdGameMode                    = PcdGetBool(PcdAmdGameMode);
  CcxData->CcxInputBlock.AmdCStateMode                  = PcdGet8(PcdAmdCStateMode);
  CcxData->CcxInputBlock.AmdCc6Ctrl                     = PcdGet8(PcdAmdCc6Ctrl);
  CcxData->CcxInputBlock.AmdCStateIoBaseAddress         = PcdGet16(PcdAmdCStateIoBaseAddress);
  CcxData->CcxInputBlock.AmdCpbMode                     = PcdGet8(PcdAmdCpbMode);
  CcxData->CcxInputBlock.EnSpecStFill                   = PcdGetBool(PcdAmdEnSpecStFill);
  CcxData->CcxInputBlock.EnableSvmAVIC                  = PcdGetBool(PcdAmdEnableSvmAVIC);
  CcxData->CcxInputBlock.EnableSvmX2AVIC                = PcdGetBool(PcdAmdEnableSvmX2AVIC);
  CcxData->CcxInputBlock.MonMwaitDis                    = PcdGet8(PcdAmdMonMwaitDis);
  CcxData->CcxInputBlock.AmdFixedMtrr250                = PcdGet64(PcdAmdFixedMtrr250);
  CcxData->CcxInputBlock.AmdFixedMtrr258                = PcdGet64(PcdAmdFixedMtrr258);
  CcxData->CcxInputBlock.AmdFixedMtrr259                = PcdGet64(PcdAmdFixedMtrr259);
  CcxData->CcxInputBlock.AmdFixedMtrr268                = PcdGet64(PcdAmdFixedMtrr268);
  CcxData->CcxInputBlock.AmdFixedMtrr269                = PcdGet64(PcdAmdFixedMtrr269);
  CcxData->CcxInputBlock.AmdFixedMtrr26A                = PcdGet64(PcdAmdFixedMtrr26A);
  CcxData->CcxInputBlock.AmdFixedMtrr26B                = PcdGet64(PcdAmdFixedMtrr26B);
  CcxData->CcxInputBlock.AmdFixedMtrr26C                = PcdGet64(PcdAmdFixedMtrr26C);
  CcxData->CcxInputBlock.AmdFixedMtrr26D                = PcdGet64(PcdAmdFixedMtrr26D);
  CcxData->CcxInputBlock.AmdFixedMtrr26E                = PcdGet64(PcdAmdFixedMtrr26E);
  CcxData->CcxInputBlock.AmdFixedMtrr26F                = PcdGet64(PcdAmdFixedMtrr26F);
  CcxData->CcxInputBlock.DisableWcSpecConfig            = PcdGet8(PcdAmdDisableWcSpecConfig);
  CcxData->CcxInputBlock.AmdPstatePolicy                = PcdGet8(PcdAmdAgesaPstatePolicy);
  CcxData->CcxInputBlock.AmdSplitRmpTable               = PcdGet8(PcdAmdSplitRmpTable);
  CcxData->CcxInputBlock.AmdReserved                    = 0xFF;
  CcxData->CcxInputBlock.AmdCpuPauseDelay               = PcdGet8(PcdAmdCpuPauseDelay);

  DEBUG ((DEBUG_INFO, "SIL Processor ID : 0x%x \n", CcxData->CcxOutputBlock.ProcessorId));


  DEBUG ((DEBUG_INFO, "SIL Processor ID : 0x%x \n", CcxData->CcxOutputBlock.ProcessorId));
  // this loop is used find the exact Microcode of the running Processor chip
  // from Microcode block on the BSP
  while (MicrocodeAddFound == FALSE)
  {
    //Read the patch id from the Microcode block on the BSP
    UcodeBIOSEntryInfoStatus = BIOSEntryInfo(UCODE_PATCH, InstanceNumber, NULL, &UcodeEntryAddress,
                                              &UcodeEntrySize, NULL);
    InstanceNumber ++;
    if (UcodeBIOSEntryInfoStatus == TRUE) {
      MicroCode = (MPB *)(size_t)UcodeEntryAddress;
      // compare with running Microprocessor patch ID.
	  if(MicroCode->MPB_REVISION.ProcessorRevisionID == CcxData->CcxOutputBlock.ProcessorId)
	  {
        //Update Valid Micrcode Info of the running Microprocessor
		MicrocodeAddFound = TRUE;
        CcxData->CcxInputBlock.UcodePatchEntryInfo.UcodePatchEntryAddress = UcodeEntryAddress;
        CcxData->CcxInputBlock.UcodePatchEntryInfo.UcodePatchEntrySize    = UcodeEntrySize;
        DEBUG ((DEBUG_INFO, "UcodePatch EntryAddress=%x\n",
                    CcxData->CcxInputBlock.UcodePatchEntryInfo.UcodePatchEntryAddress));
        DEBUG ((DEBUG_INFO, "UcodePatch EntrySize=%x\n",
                    CcxData->CcxInputBlock.UcodePatchEntryInfo.UcodePatchEntrySize));
	  } else {
        DEBUG ((DEBUG_INFO, "Microcode Info =%x,%x\n",
                    UcodeEntryAddress,MicroCode->MPB_REVISION));
	  }
    } else {
      MicrocodeAddFound = TRUE;
      DEBUG ((DEBUG_ERROR, "Fail to get the UcodePatch BiosEntry status =%x\n",UcodeBIOSEntryInfoStatus));
    }
  }
  return EFI_SUCCESS;
}

/**
 * UpdateEnCoreLimitInHob
 *
 * @brief Update Core Limit value into Host Specified Memory location using HOB Method
 *
 * @param EnCoreLimit Ccx Core Count Value
 *
 * @return EFI_SUCCESS or EFI_NOT_FOUND
 *       EFI_SUCCESS   : Host able to create Guid type memory block using HOB 
 *       EFI_NOT_FOUND : Hob create unsuccessful
 */
static EFI_STATUS
UpdateEnCoreLimitInHob (
  uint32_t EnCoreLimit
  )
{
  EFI_HOB_GUID_TYPE *Hob;
  EFI_STATUS Status;
	
  Status = PeiServicesCreateHob (
  EFI_HOB_TYPE_GUID_EXTENSION,
  (uint16_t)(sizeof(EFI_HOB_GUID_TYPE) + sizeof(uint32_t)),
  (void **)&Hob);
  ASSERT_EFI_ERROR (Status);
  
  CopyMem(&Hob->Name, &gPeiOpenSilCcxDownCoreDataGuid, sizeof(EFI_GUID));
  Hob++;
  CopyMem (Hob, (uint32_t *)&EnCoreLimit, sizeof(uint32_t));

  return Status;
}

/**
 * CcxDataBackToHostFW
 *
 * @brief Send Updated Ccx IP blocks Data to host FW
 * @return EFI_SUCCESS or EFI_NOT_FOUND
 *       EFI_SUCCESS   : Received valid address within the Host allocated memory block 
 *                       and succesfully update the PCD.
 *       EFI_NOT_FOUND : Indicates the requested block was not found
 */
EFI_STATUS
CcxDataBackToHostFW (
  void
  )
{
  EFI_STATUS Status;
  CCXCLASS_DATA_BLK *CcxDataHostFw;

  CcxDataHostFw = (CCXCLASS_DATA_BLK *)SilFindStructure (SilId_CcxClass,  0);
  DEBUG ((DEBUG_ERROR, "CcxDataBackToHostFW SIL Ccx memory block at: 0x%x \n", CcxDataHostFw));
  if (CcxDataHostFw == NULL) {
    return EFI_NOT_FOUND; // Could not find the IP input block
  }

  Status = PcdSet8S(PcdAmdDownCoreMode,CcxDataHostFw->CcxInputBlock.AmdDownCoreMode);
  DEBUG ((DEBUG_ERROR, "PcdAmdDownCoreMode update: %d\n", Status));
  Status = PcdSet8S(PcdAmdApicMode,CcxDataHostFw->CcxInputBlock.AmdApicMode);
  DEBUG ((DEBUG_ERROR, "PcdAmdApicMode update: %d\n", Status));
  Status = PcdSetBoolS(PcdAmdAcpiS3Support,CcxDataHostFw->CcxOutputBlock.AmdAcpiS3Support);
  DEBUG ((DEBUG_ERROR, "PcdAmdAcpiS3Support update: %d\n", Status));
  Status = UpdateEnCoreLimitInHob(CcxDataHostFw->CcxOutputBlock.AmdCcxCoreCount);
  DEBUG ((DEBUG_ERROR, "UpdateEnCoreLimitInHob Status: %d\n", Status));
/* move out
  //
  // Always Initialize PSP BAR to support RdRand Instruction
  //
  DEBUG ((DEBUG_INFO,"\tPsp BAR init\n"));
  PspBarInitEarlyV2 ();

  //Enable RdRand at PEI phase
  Status = EnableRdInstructionEarly ();
*/

  return EFI_SUCCESS;
}
