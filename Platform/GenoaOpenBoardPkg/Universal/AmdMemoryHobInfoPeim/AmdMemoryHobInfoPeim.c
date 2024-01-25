/*****************************************************************************
 *
 * Copyright (C) 2020-2023 Advanced Micro Devices, Inc. All rights reserved.
 *
 *****************************************************************************/

#include <PiPei.h>
#include <xPRF-api.h>
#include <ApobCmn.h>
#include <Include/xPrfServicesPpi.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Guid/AmdMemoryInfoHob.h>
#include <Ppi/AmdMemoryInfoHobPpi.h>

#define MAX_NUMBER_OF_EXTENDED_MEMORY_DESCRIPTOR 30
#define MAX_SIZEOF_AMD_MEMORY_INFO_HOB_BUFFER (sizeof(AMD_MEMORY_INFO_HOB) + \
                              (MAX_NUMBER_OF_EXTENDED_MEMORY_DESCRIPTOR * sizeof(AMD_MEMORY_RANGE_DESCRIPTOR)))

#define AMD_MEM_PPI_MAX_SOCKETS_SUPPORTED FixedPcdGet8(PcdAmdMemMaxSocketSupportedV2)
#define AMD_MEM_PPI_MAX_DIES_PER_SOCKET FixedPcdGet8(PcdAmdMemMaxDiePerSocketV2)
#define AMD_MEM_PPI_MAX_CHANNELS_PER_DIE ABL_APOB_MAX_CHANNELS_PER_SOCKET
#define AMD_MEM_PPI_MAX_DIMMS_PER_CHANNEL FixedPcdGet8(PcdAmdMemMaxDimmPerChannelV2)

//
// PPI Initialization
//
STATIC AMD_MEMORY_INFO_HOB_PPI mAmdMemoryHobInfoAvailblePpi = {
    AMD_MEMORY_INFO_HOB_PPI_REV_0400};

STATIC EFI_PEI_PPI_DESCRIPTOR mAmdMemoryHobInfoAvailblePpiList =
    {
        (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
        &gAmdMemoryInfoHobPpiGuid,
        &mAmdMemoryHobInfoAvailblePpi};

EFI_STATUS
BuildMemoryHobInfo(
    IN AMD_OPENSIL_XPRF_SERVICES_PPI *OpenSilXprfServicePpi)
/*++

Routine Description:

  This function build HOB info from SIL interface parameters

Arguments:

  OpenSilXprfServicePpi -     Pointer SIL PPI services

Returns:
  EFI_STATUS - Status code
--*/
{
  EFI_STATUS Status;
  UINT8 MemInfoHobBuffer[MAX_SIZEOF_AMD_MEMORY_INFO_HOB_BUFFER];
  AMD_MEMORY_INFO_HOB *MemInfoHob;
  AMD_MEMORY_RANGE_DESCRIPTOR *MemRangeDesc;
  AMD_MEMORY_SUMMARY MemInitTable;
  UINT64 TopOfMemAddress;
  UINT32 NumOfHoles;
  MEMORY_HOLE_DESCRIPTOR *MemoryHoleDescPtr = NULL;
  UINT64 CurrentBase;
  UINT32 MemRangeIndex;
  UINT32 Index;
  UINTN SizeOfMemInfoHob;

  MemInitTable.MaxSocketSupported = AMD_MEM_PPI_MAX_SOCKETS_SUPPORTED;
  MemInitTable.MaxDiePerSocket = AMD_MEM_PPI_MAX_DIES_PER_SOCKET;
  MemInitTable.MaxChannelPerDie = AMD_MEM_PPI_MAX_CHANNELS_PER_DIE;
  MemInitTable.MaxDimmPerChannel = AMD_MEM_PPI_MAX_DIMMS_PER_CHANNEL;

  Status = OpenSilXprfServicePpi->SilGetMemInitInfo(&MemInitTable);

  if (EFI_ERROR(Status))
  {
    DEBUG((DEBUG_ERROR, "OpenSilXprfServicePpi->SilGetMemInitInfo : %r\n", Status));
    return Status;
  }

  Status = OpenSilXprfServicePpi->SilGetSystemMemoryMap(
      &NumOfHoles,
      &TopOfMemAddress,
      (VOID **)&MemoryHoleDescPtr);
  DEBUG((DEBUG_INFO,
    "OpenSilXprfServicePpi->SilGetSystemMemoryMap : %r TopOfMemAddress : %x, NumOfHoles : %x MemoryHoleDescPtr : %x\n",
     Status, TopOfMemAddress, NumOfHoles, MemoryHoleDescPtr));

  if (EFI_ERROR(Status) || (MemoryHoleDescPtr == NULL))
  {
    return EFI_NOT_FOUND;
  }

  MemInfoHob = (AMD_MEMORY_INFO_HOB *)MemInfoHobBuffer;
  MemRangeDesc = &MemInfoHob->Ranges[0];

  MemInfoHob->Version = AMD_MEMORY_INFO_HOB_VERISION;
  MemRangeIndex = 0;
  CurrentBase = 0;

  for (Index = 0; Index < NumOfHoles; Index++)
  {
    switch (MemoryHoleDescPtr->Type)
    {
    case MMIO:
      MemRangeDesc[MemRangeIndex].Size = (MemoryHoleDescPtr->Base - CurrentBase);
      if (0 != MemRangeDesc[MemRangeIndex].Size)
      {
        MemRangeDesc[MemRangeIndex].Attribute = AMD_MEMORY_ATTRIBUTE_AVAILABLE;
        MemRangeDesc[MemRangeIndex].Base = CurrentBase;
        CurrentBase += MemRangeDesc[MemRangeIndex].Size;

        DEBUG((DEBUG_INFO, "    MemRangeIndex: 0x%x\n", MemRangeIndex));
        DEBUG((DEBUG_INFO, "    Base Hi: 0x%08x\n", RShiftU64(MemRangeDesc[MemRangeIndex].Base, 32)));
        DEBUG((DEBUG_INFO, "    Base Lo: 0x%08x\n", ((MemRangeDesc[MemRangeIndex].Base) & 0xFFFFFFFF)));
        DEBUG((DEBUG_INFO, "    Size Hi: 0x%08x\n", RShiftU64(MemRangeDesc[MemRangeIndex].Size, 32)));
        DEBUG((DEBUG_INFO, "    Size Lo: 0x%08x\n", ((MemRangeDesc[MemRangeIndex].Size) & 0xFFFFFFFF)));
        DEBUG((DEBUG_INFO, "    Attribute: 0x%08x\n", MemRangeDesc[MemRangeIndex].Attribute));
        DEBUG((DEBUG_INFO, "    CurrentBase Hi: 0x%08x\n", RShiftU64(CurrentBase, 32)));
        DEBUG((DEBUG_INFO, "    CurrentBase Lo: 0x%08x\n", (CurrentBase & 0xFFFFFFFF)));
        DEBUG((DEBUG_INFO, "\n"));

        MemRangeIndex += 1;
      }

      MemRangeDesc[MemRangeIndex].Size = MemoryHoleDescPtr->Size;
      ASSERT(0 != MemRangeDesc[MemRangeIndex].Size);
      MemRangeDesc[MemRangeIndex].Attribute = AMD_MEMORY_ATTRIBUTE_MMIO;
      MemRangeDesc[MemRangeIndex].Base = MemoryHoleDescPtr->Base;
      CurrentBase += MemoryHoleDescPtr->Size;
      DEBUG((DEBUG_INFO, "    MemRangeIndex: 0x%x\n", MemRangeIndex));
      DEBUG((DEBUG_INFO, "    Base Hi: 0x%08x\n", RShiftU64(MemRangeDesc[MemRangeIndex].Base, 32)));
      DEBUG((DEBUG_INFO, "    Base Lo: 0x%08x\n", ((MemRangeDesc[MemRangeIndex].Base) & 0xFFFFFFFF)));
      DEBUG((DEBUG_INFO, "    Size Hi: 0x%08x\n", RShiftU64(MemRangeDesc[MemRangeIndex].Size, 32)));
      DEBUG((DEBUG_INFO, "    Size Lo: 0x%08x\n", ((MemRangeDesc[MemRangeIndex].Size) & 0xFFFFFFFF)));
      DEBUG((DEBUG_INFO, "    Attribute: 0x%08x\n", MemRangeDesc[MemRangeIndex].Attribute));
      DEBUG((DEBUG_INFO, "    CurrentBase Hi: 0x%08x\n", RShiftU64(CurrentBase, 32)));
      DEBUG((DEBUG_INFO, "    CurrentBase Lo: 0x%08x\n", (CurrentBase & 0xFFFFFFFF)));
      DEBUG((DEBUG_INFO, "\n"));
      break;

    case Reserved1TbRemap:
      MemRangeDesc[MemRangeIndex].Size = (MemoryHoleDescPtr->Base - CurrentBase);
      if (0 != MemRangeDesc[MemRangeIndex].Size)
      {
        MemRangeDesc[MemRangeIndex].Attribute = AMD_MEMORY_ATTRIBUTE_AVAILABLE;
        MemRangeDesc[MemRangeIndex].Base = CurrentBase;
        CurrentBase += MemRangeDesc[MemRangeIndex].Size;

        DEBUG((DEBUG_INFO, "    MemRangeIndex: 0x%x\n", MemRangeIndex));
        DEBUG((DEBUG_INFO, "    Base Hi: 0x%08x\n", RShiftU64(MemRangeDesc[MemRangeIndex].Base, 32)));
        DEBUG((DEBUG_INFO, "    Base Lo: 0x%08x\n", ((MemRangeDesc[MemRangeIndex].Base) & 0xFFFFFFFF)));
        DEBUG((DEBUG_INFO, "    Size Hi: 0x%08x\n", RShiftU64(MemRangeDesc[MemRangeIndex].Size, 32)));
        DEBUG((DEBUG_INFO, "    Size Lo: 0x%08x\n", ((MemRangeDesc[MemRangeIndex].Size) & 0xFFFFFFFF)));
        DEBUG((DEBUG_INFO, "    Attribute: 0x%08x\n", MemRangeDesc[MemRangeIndex].Attribute));
        DEBUG((DEBUG_INFO, "    CurrentBase Hi: 0x%08x\n", RShiftU64(CurrentBase, 32)));
        DEBUG((DEBUG_INFO, "    CurrentBase Lo: 0x%08x\n", (CurrentBase & 0xFFFFFFFF)));
        DEBUG((DEBUG_INFO, "\n"));

        MemRangeIndex += 1;
      }

      MemRangeDesc[MemRangeIndex].Size = MemoryHoleDescPtr->Size;
      ASSERT(0 != MemRangeDesc[MemRangeIndex].Size);
      MemRangeDesc[MemRangeIndex].Attribute = AMD_MEMORY_ATTRIBUTE_MMIO_RESERVED;
      MemRangeDesc[MemRangeIndex].Base = MemoryHoleDescPtr->Base;
      CurrentBase += MemoryHoleDescPtr->Size;
      DEBUG((DEBUG_INFO, "    MemRangeIndex: 0x%x\n", MemRangeIndex));
      DEBUG((DEBUG_INFO, "    Base Hi: 0x%08x\n", RShiftU64(MemRangeDesc[MemRangeIndex].Base, 32)));
      DEBUG((DEBUG_INFO, "    Base Lo: 0x%08x\n", ((MemRangeDesc[MemRangeIndex].Base) & 0xFFFFFFFF)));
      DEBUG((DEBUG_INFO, "    Size Hi: 0x%08x\n", RShiftU64(MemRangeDesc[MemRangeIndex].Size, 32)));
      DEBUG((DEBUG_INFO, "    Size Lo: 0x%08x\n", ((MemRangeDesc[MemRangeIndex].Size) & 0xFFFFFFFF)));
      DEBUG((DEBUG_INFO, "    Attribute: 0x%08x\n", MemRangeDesc[MemRangeIndex].Attribute));
      DEBUG((DEBUG_INFO, "    CurrentBase Hi: 0x%08x\n", RShiftU64(CurrentBase, 32)));
      DEBUG((DEBUG_INFO, "    CurrentBase Lo: 0x%08x\n", (CurrentBase & 0xFFFFFFFF)));
      DEBUG((DEBUG_INFO, "\n"));
      break;

    case UMA:
      MemRangeDesc[MemRangeIndex].Size = (MemoryHoleDescPtr->Base - CurrentBase);
      if (0 != MemRangeDesc[MemRangeIndex].Size)
      {
        MemRangeDesc[MemRangeIndex].Attribute = AMD_MEMORY_ATTRIBUTE_AVAILABLE;
        MemRangeDesc[MemRangeIndex].Base = CurrentBase;
        CurrentBase += MemRangeDesc[MemRangeIndex].Size;

        DEBUG((DEBUG_INFO, "    MemRangeIndex: 0x%x\n", MemRangeIndex));
        DEBUG((DEBUG_INFO, "    Base Hi: 0x%08x\n", RShiftU64(MemRangeDesc[MemRangeIndex].Base, 32)));
        DEBUG((DEBUG_INFO, "    Base Lo: 0x%08x\n", ((MemRangeDesc[MemRangeIndex].Base) & 0xFFFFFFFF)));
        DEBUG((DEBUG_INFO, "    Size Hi: 0x%08x\n", RShiftU64(MemRangeDesc[MemRangeIndex].Size, 32)));
        DEBUG((DEBUG_INFO, "    Size Lo: 0x%08x\n", ((MemRangeDesc[MemRangeIndex].Size) & 0xFFFFFFFF)));
        DEBUG((DEBUG_INFO, "    Attribute: 0x%08x\n", MemRangeDesc[MemRangeIndex].Attribute));
        DEBUG((DEBUG_INFO, "    CurrentBase Hi: 0x%08x\n", RShiftU64(CurrentBase, 32)));
        DEBUG((DEBUG_INFO, "    CurrentBase Lo: 0x%08x\n", (CurrentBase & 0xFFFFFFFF)));
        DEBUG((DEBUG_INFO, "\n"));

        MemRangeIndex += 1;
      }

      MemRangeDesc[MemRangeIndex].Size = MemoryHoleDescPtr->Size;
      ASSERT(0 != MemRangeDesc[MemRangeIndex].Size);
      MemRangeDesc[MemRangeIndex].Attribute = AMD_MEMORY_ATTRIBUTE_UMA;
      MemRangeDesc[MemRangeIndex].Base = MemoryHoleDescPtr->Base;
      CurrentBase += MemoryHoleDescPtr->Size;
      DEBUG((DEBUG_INFO, "    MemRangeIndex: 0x%x\n", MemRangeIndex));
      DEBUG((DEBUG_INFO, "    Base Hi: 0x%08x\n", RShiftU64(MemRangeDesc[MemRangeIndex].Base, 32)));
      DEBUG((DEBUG_INFO, "    Base Lo: 0x%08x\n", ((MemRangeDesc[MemRangeIndex].Base) & 0xFFFFFFFF)));
      DEBUG((DEBUG_INFO, "    Size Hi: 0x%08x\n", RShiftU64(MemRangeDesc[MemRangeIndex].Size, 32)));
      DEBUG((DEBUG_INFO, "    Size Lo: 0x%08x\n", ((MemRangeDesc[MemRangeIndex].Size) & 0xFFFFFFFF)));
      DEBUG((DEBUG_INFO, "    Attribute: 0x%08x\n", MemRangeDesc[MemRangeIndex].Attribute));
      DEBUG((DEBUG_INFO, "    CurrentBase Hi: 0x%08x\n", RShiftU64(CurrentBase, 32)));
      DEBUG((DEBUG_INFO, "    CurrentBase Lo: 0x%08x\n", (CurrentBase & 0xFFFFFFFF)));
      DEBUG((DEBUG_INFO, "\n"));
      break;

    default:
      if (PcdGetBool(PcdCfgIommuSupport) && (CurrentBase <= 0xFD00000000) && (MemoryHoleDescPtr->Base >= 0x10000000000))
      {
        MemRangeDesc[MemRangeIndex].Size = (0xFD00000000 - CurrentBase);
        if (0 != MemRangeDesc[MemRangeIndex].Size)
        {
          MemRangeDesc[MemRangeIndex].Attribute = AMD_MEMORY_ATTRIBUTE_AVAILABLE;
          MemRangeDesc[MemRangeIndex].Base = CurrentBase;
          CurrentBase += MemRangeDesc[MemRangeIndex].Size;

          DEBUG((DEBUG_INFO, "    MemRangeIndex: 0x%x\n", MemRangeIndex));
          DEBUG((DEBUG_INFO, "    Base Hi: 0x%08x\n", RShiftU64(MemRangeDesc[MemRangeIndex].Base, 32)));
          DEBUG((DEBUG_INFO, "    Base Lo: 0x%08x\n", ((MemRangeDesc[MemRangeIndex].Base) & 0xFFFFFFFF)));
          DEBUG((DEBUG_INFO, "    Size Hi: 0x%08x\n", RShiftU64(MemRangeDesc[MemRangeIndex].Size, 32)));
          DEBUG((DEBUG_INFO, "    Size Lo: 0x%08x\n", ((MemRangeDesc[MemRangeIndex].Size) & 0xFFFFFFFF)));
          DEBUG((DEBUG_INFO, "    Attribute: 0x%08x\n", MemRangeDesc[MemRangeIndex].Attribute));
          DEBUG((DEBUG_INFO, "    CurrentBase Hi: 0x%08x\n", RShiftU64(CurrentBase, 32)));
          DEBUG((DEBUG_INFO, "    CurrentBase Lo: 0x%08x\n", (CurrentBase & 0xFFFFFFFF)));
          DEBUG((DEBUG_INFO, "\n"));

          MemRangeIndex += 1;
        }

        MemRangeDesc[MemRangeIndex].Size = (0x10000000000 - CurrentBase);
        if (0 != MemRangeDesc[MemRangeIndex].Size)
        {
          MemRangeDesc[MemRangeIndex].Attribute = AMD_MEMORY_ATTRIBUTE_RESERVED;
          MemRangeDesc[MemRangeIndex].Base = CurrentBase;
          CurrentBase += MemRangeDesc[MemRangeIndex].Size;

          DEBUG((DEBUG_INFO, "    MemRangeIndex: 0x%x\n", MemRangeIndex));
          DEBUG((DEBUG_INFO, "    Base Hi: 0x%08x\n", RShiftU64(MemRangeDesc[MemRangeIndex].Base, 32)));
          DEBUG((DEBUG_INFO, "    Base Lo: 0x%08x\n", ((MemRangeDesc[MemRangeIndex].Base) & 0xFFFFFFFF)));
          DEBUG((DEBUG_INFO, "    Size Hi: 0x%08x\n", RShiftU64(MemRangeDesc[MemRangeIndex].Size, 32)));
          DEBUG((DEBUG_INFO, "    Size Lo: 0x%08x\n", ((MemRangeDesc[MemRangeIndex].Size) & 0xFFFFFFFF)));
          DEBUG((DEBUG_INFO, "    Attribute: 0x%08x\n", MemRangeDesc[MemRangeIndex].Attribute));
          DEBUG((DEBUG_INFO, "    CurrentBase Hi: 0x%08x\n", RShiftU64(CurrentBase, 32)));
          DEBUG((DEBUG_INFO, "    CurrentBase Lo: 0x%08x\n", (CurrentBase & 0xFFFFFFFF)));
          DEBUG((DEBUG_INFO, "\n"));

          MemRangeIndex += 1;
        }
      }

      MemRangeDesc[MemRangeIndex].Size = (MemoryHoleDescPtr->Base - CurrentBase);
      if (0 != MemRangeDesc[MemRangeIndex].Size)
      {
        MemRangeDesc[MemRangeIndex].Attribute = AMD_MEMORY_ATTRIBUTE_AVAILABLE;
        MemRangeDesc[MemRangeIndex].Base = CurrentBase;
        CurrentBase += MemRangeDesc[MemRangeIndex].Size;

        DEBUG((DEBUG_INFO, "    MemRangeIndex: 0x%x\n", MemRangeIndex));
        DEBUG((DEBUG_INFO, "    Base Hi: 0x%08x\n", RShiftU64(MemRangeDesc[MemRangeIndex].Base, 32)));
        DEBUG((DEBUG_INFO, "    Base Lo: 0x%08x\n", ((MemRangeDesc[MemRangeIndex].Base) & 0xFFFFFFFF)));
        DEBUG((DEBUG_INFO, "    Size Hi: 0x%08x\n", RShiftU64(MemRangeDesc[MemRangeIndex].Size, 32)));
        DEBUG((DEBUG_INFO, "    Size Lo: 0x%08x\n", ((MemRangeDesc[MemRangeIndex].Size) & 0xFFFFFFFF)));
        DEBUG((DEBUG_INFO, "    Attribute: 0x%08x\n", MemRangeDesc[MemRangeIndex].Attribute));
        DEBUG((DEBUG_INFO, "    CurrentBase Hi: 0x%08x\n", RShiftU64(CurrentBase, 32)));
        DEBUG((DEBUG_INFO, "    CurrentBase Lo: 0x%08x\n", (CurrentBase & 0xFFFFFFFF)));
        DEBUG((DEBUG_INFO, "\n"));

        MemRangeIndex += 1;
      }

      MemRangeDesc[MemRangeIndex].Size = MemoryHoleDescPtr->Size;
      ASSERT(0 != MemRangeDesc[MemRangeIndex].Size);
      MemRangeDesc[MemRangeIndex].Attribute = AMD_MEMORY_ATTRIBUTE_RESERVED;
      MemRangeDesc[MemRangeIndex].Base = MemoryHoleDescPtr->Base;
      CurrentBase += MemoryHoleDescPtr->Size;
      DEBUG((DEBUG_INFO, "    MemRangeIndex: 0x%x\n", MemRangeIndex));
      DEBUG((DEBUG_INFO, "    Base Hi: 0x%08x\n", RShiftU64(MemRangeDesc[MemRangeIndex].Base, 32)));
      DEBUG((DEBUG_INFO, "    Base Lo: 0x%08x\n", ((MemRangeDesc[MemRangeIndex].Base) & 0xFFFFFFFF)));
      DEBUG((DEBUG_INFO, "    Size Hi: 0x%08x\n", RShiftU64(MemRangeDesc[MemRangeIndex].Size, 32)));
      DEBUG((DEBUG_INFO, "    Size Lo: 0x%08x\n", ((MemRangeDesc[MemRangeIndex].Size) & 0xFFFFFFFF)));
      DEBUG((DEBUG_INFO, "    Attribute: 0x%08x\n", MemRangeDesc[MemRangeIndex].Attribute));
      DEBUG((DEBUG_INFO, "    CurrentBase Hi: 0x%08x\n", RShiftU64(CurrentBase, 32)));
      DEBUG((DEBUG_INFO, "    CurrentBase Lo: 0x%08x\n", (CurrentBase & 0xFFFFFFFF)));
      DEBUG((DEBUG_INFO, "\n"));
      break;
    }
    MemRangeIndex++;
    MemoryHoleDescPtr++;
  }

  if (CurrentBase < TopOfMemAddress)
  {
    //
    // MemRangeIndex will be incremented in the previous loop hence dont need to increment here
    //
    MemRangeDesc[MemRangeIndex].Attribute = AMD_MEMORY_ATTRIBUTE_AVAILABLE;
    MemRangeDesc[MemRangeIndex].Base = CurrentBase;
    MemRangeDesc[MemRangeIndex].Size = TopOfMemAddress - CurrentBase;

    DEBUG((DEBUG_INFO, "    MemRangeIndex: 0x%x\n", MemRangeIndex));
    DEBUG((DEBUG_INFO, "    Base Hi: 0x%08x\n", RShiftU64(MemRangeDesc[MemRangeIndex].Base, 32)));
    DEBUG((DEBUG_INFO, "    Base Lo: 0x%08x\n", ((MemRangeDesc[MemRangeIndex].Base) & 0xFFFFFFFF)));
    DEBUG((DEBUG_INFO, "    Size Hi: 0x%08x\n", RShiftU64(MemRangeDesc[MemRangeIndex].Size, 32)));
    DEBUG((DEBUG_INFO, "    Size Lo: 0x%08x\n", ((MemRangeDesc[MemRangeIndex].Size) & 0xFFFFFFFF)));
    DEBUG((DEBUG_INFO, "    Attribute: 0x%04x\n", MemRangeDesc[MemRangeIndex].Attribute));
    DEBUG((DEBUG_INFO, "\n"));
  }
  else
  {
    // Since no additional descriptor required to be updated, decrease the
    // incremented number by 1 to ensure we report correct number of descriptor
    MemRangeIndex--;
  }

  MemInfoHob->NumberOfDescriptor = MemRangeIndex + 1;
  SizeOfMemInfoHob = sizeof(AMD_MEMORY_INFO_HOB) + (MemInfoHob->NumberOfDescriptor - 1) * sizeof(AMD_MEMORY_RANGE_DESCRIPTOR);
  DEBUG((DEBUG_INFO, "    NumberOfDescriptor: 0x%x\n", MemInfoHob->NumberOfDescriptor));
  DEBUG((DEBUG_INFO, "    SizeOfMemInfoHob: 0x%x\n", SizeOfMemInfoHob));

  //
  // Update Voltage Information.
  //
  MemInfoHob->AmdMemoryVddioValid = TRUE;
  MemInfoHob->AmdMemoryVddio = MemInitTable.AmdMemoryVddIo;
  MemInfoHob->AmdMemoryVddpVddrValid = MemInitTable.AmdMemoryVddpVddr.IsValid;
  MemInfoHob->AmdMemoryVddpVddr = MemInitTable.AmdMemoryVddpVddr.Voltage;
  MemInfoHob->AmdMemoryFrequencyValid = TRUE;
  MemInfoHob->AmdMemoryFrequency = MemInitTable.AmdMemoryFrequency;
  MemInfoHob->AmdMemoryDdrMaxRate = MemInitTable.DdrMaxRate;

  if (BuildGuidDataHob(&gAmdMemoryInfoHobGuid, &MemInfoHobBuffer, SizeOfMemInfoHob) == NULL)
  {
    DEBUG((DEBUG_ERROR, "BuildHobInfo: Failed to build gAmdMemoryInfoHobGuid Hob!\n"));
    Status = EFI_NOT_FOUND;
  }

  return Status;
}

EFI_STATUS
EFIAPI
InitializeAmdMemoryInfoHobPeim(
    IN EFI_PEI_FILE_HANDLE FileHandle,
    IN CONST EFI_PEI_SERVICES **PeiServices)
/*++

Routine Description:

  Initialization Entry Point for AmdMemoryHobInfo PEIM

Arguments:
  FileHandle -  FileHandle
  PeiServices - PeiServices

Returns:
  EFI_STATUS  - Status code
                EFI_SUCCESS

--*/
{
  EFI_STATUS Status;
  AMD_OPENSIL_XPRF_SERVICES_PPI *OpenSilXprfServicePpi;

  Status = (*PeiServices)->LocatePpi(PeiServices, &gOpenSilxPrfServicePpiGuid, 0, NULL, (VOID **)&OpenSilXprfServicePpi);
  if (EFI_ERROR(Status))
  {
    DEBUG((DEBUG_ERROR, "OpenSilXprfServicePpi Not Found!!!\n"));
    return Status;
  }

  //
  // Build Memory Info Hob
  //
  Status = BuildMemoryHobInfo(OpenSilXprfServicePpi);

  if (!EFI_ERROR(Status))
  {
    Status = (**PeiServices).InstallPpi(PeiServices, &mAmdMemoryHobInfoAvailblePpiList);
    DEBUG((DEBUG_INFO, "AMD MemoryHobInfo PPI installed : %r\n", Status));
  }

  return Status;
}
