/******************************************************************************
* Copyright (C) 2017-2023 Advanced Micro Devices, Inc. All rights reserved.
*
*
***************************************************************************/

#include <PiSmm.h>
#include <Library/SmmCpuFeaturesLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PcdLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Register/Cpuid.h>
#include <Library/SmmServicesTableLib.h>
#include <Register/SmramSaveStateMap.h>
#include <Register/AMDSmramSaveStateMap.h>


// EFER register LMA bit
#define LMA BIT10

// Machine Specific Registers (MSRs)
#define SMMADDR_ADDRESS       0xC0010112ul
#define SMMMASK_ADDRESS       0xC0010113ul
#define EFER_ADDRESS          0XC0000080ul

// Set default value to assume SMRR is not supported
BOOLEAN  mSmrrSupported = TRUE;

// Set default value to assume MSR_SMM_FEATURE_CONTROL is not supported
BOOLEAN  mSmmFeatureControlSupported = FALSE;

// The mode of the CPU at the time an SMI occurs
extern UINT8  mSmmSaveStateRegisterLma;

// Macro used to simplify the lookup table entries of type CPU_SMM_SAVE_STATE_LOOKUP_ENTRY
#define SMM_CPU_OFFSET(Field) OFFSET_OF (AMD_SMRAM_SAVE_STATE_MAP, Field)

// Macro used to simplify the lookup table entries of type CPU_SMM_SAVE_STATE_REGISTER_RANGE
#define SMM_REGISTER_RANGE(Start, End) { Start, End, End - Start + 1 }

// Structure used to describe a range of registers
typedef struct {
  EFI_SMM_SAVE_STATE_REGISTER  Start;
  EFI_SMM_SAVE_STATE_REGISTER  End;
  UINTN                        Length;
} CPU_SMM_SAVE_STATE_REGISTER_RANGE;

// Structure used to build a lookup table to retrieve the widths and offsets
// associated with each supported EFI_SMM_SAVE_STATE_REGISTER value

#define SMM_SAVE_STATE_REGISTER_SMMREVID_INDEX        1
#define SMM_SAVE_STATE_REGISTER_MAX_INDEX             2

typedef struct {
  UINT8   Width32;
  UINT8   Width64;
  UINT16  Offset32;
  UINT16  Offset64Lo;
  UINT16  Offset64Hi;
  BOOLEAN Writeable;
} CPU_SMM_SAVE_STATE_LOOKUP_ENTRY;


// Table used by GetRegisterIndex() to convert an EFI_SMM_SAVE_STATE_REGISTER
// value to an index into a table of type CPU_SMM_SAVE_STATE_LOOKUP_ENTRY
static CONST CPU_SMM_SAVE_STATE_REGISTER_RANGE mSmmCpuRegisterRanges[] = {
  SMM_REGISTER_RANGE (EFI_SMM_SAVE_STATE_REGISTER_GDTBASE, EFI_SMM_SAVE_STATE_REGISTER_LDTINFO),
  SMM_REGISTER_RANGE (EFI_SMM_SAVE_STATE_REGISTER_ES,      EFI_SMM_SAVE_STATE_REGISTER_RIP),
  SMM_REGISTER_RANGE (EFI_SMM_SAVE_STATE_REGISTER_RFLAGS,  EFI_SMM_SAVE_STATE_REGISTER_CR4),
  { (EFI_SMM_SAVE_STATE_REGISTER)0, (EFI_SMM_SAVE_STATE_REGISTER)0, 0 }
};

// Lookup table used to retrieve the widths and offsets associated with each
// supported EFI_SMM_SAVE_STATE_REGISTER value
static CONST CPU_SMM_SAVE_STATE_LOOKUP_ENTRY mSmmCpuWidthOffset[] = {
  {0, 0, 0, 0, FALSE},                                                                                                     //  Reserved

  //
  // Internally defined CPU Save State Registers. Not defined in PI SMM CPU Protocol.
  //
  {4, 4, SMM_CPU_OFFSET (x86.SMMRevId)  , SMM_CPU_OFFSET (x64.SMMRevId)  , 0                                 , FALSE}, // SMM_SAVE_STATE_REGISTER_SMMREVID_INDEX  = 1

  //
  // CPU Save State registers defined in PI SMM CPU Protocol.
  //
  {4, 8, SMM_CPU_OFFSET (x86.GDTBase)   , SMM_CPU_OFFSET (x64._GDTRBaseLoDword) , SMM_CPU_OFFSET (x64._GDTRBaseHiDword), FALSE},  //  EFI_SMM_SAVE_STATE_REGISTER_GDTBASE  = 4
  {0, 8, 0                              , SMM_CPU_OFFSET (x64._IDTRBaseLoDword) , SMM_CPU_OFFSET (x64._IDTRBaseLoDword), FALSE},  //  EFI_SMM_SAVE_STATE_REGISTER_IDTBASE  = 5
  {0, 8, 0                              , SMM_CPU_OFFSET (x64._LDTRBaseLoDword) , SMM_CPU_OFFSET (x64._LDTRBaseLoDword), FALSE},  //  EFI_SMM_SAVE_STATE_REGISTER_LDTBASE  = 6
  {0, 2, 0                              , SMM_CPU_OFFSET (x64._GDTRLimit)        , 0                                   , FALSE},  //  EFI_SMM_SAVE_STATE_REGISTER_GDTLIMIT = 7
  {0, 2, 0                              , SMM_CPU_OFFSET (x64._IDTRLimit)        , 0                                   , FALSE},  //  EFI_SMM_SAVE_STATE_REGISTER_IDTLIMIT = 8
  {0, 4, 0                              , SMM_CPU_OFFSET (x64._LDTRLimit)        , 0                                   , FALSE},  //  EFI_SMM_SAVE_STATE_REGISTER_LDTLIMIT = 9
  {0, 0, 0                            , 0                                        , 0                                   , FALSE},  //  EFI_SMM_SAVE_STATE_REGISTER_LDTINFO  = 10
  {4, 2, SMM_CPU_OFFSET (x86._ES)       , SMM_CPU_OFFSET (x64._ES)               , 0                                   , FALSE},  //  EFI_SMM_SAVE_STATE_REGISTER_ES       = 20
  {4, 2, SMM_CPU_OFFSET (x86._CS)       , SMM_CPU_OFFSET (x64._CS)               , 0                                   , FALSE},  //  EFI_SMM_SAVE_STATE_REGISTER_CS       = 21
  {4, 2, SMM_CPU_OFFSET (x86._SS)       , SMM_CPU_OFFSET (x64._SS)               , 0                                   , FALSE},  //  EFI_SMM_SAVE_STATE_REGISTER_SS       = 22
  {4, 2, SMM_CPU_OFFSET (x86._DS)       , SMM_CPU_OFFSET (x64._DS)               , 0                                   , FALSE},  //  EFI_SMM_SAVE_STATE_REGISTER_DS       = 23
  {4, 2, SMM_CPU_OFFSET (x86._FS)       , SMM_CPU_OFFSET (x64._FS)               , 0                                   , FALSE},  //  EFI_SMM_SAVE_STATE_REGISTER_FS       = 24
  {4, 2, SMM_CPU_OFFSET (x86._GS)       , SMM_CPU_OFFSET (x64._GS)               , 0                                   , FALSE},  //  EFI_SMM_SAVE_STATE_REGISTER_GS       = 25
  {0, 2, 0                              , SMM_CPU_OFFSET (x64._LDTR)             , 0                                   , FALSE},  //  EFI_SMM_SAVE_STATE_REGISTER_LDTR_SEL = 26
  {0, 2, 0                              , SMM_CPU_OFFSET (x64._TR)               , 0                                   , FALSE},  //  EFI_SMM_SAVE_STATE_REGISTER_TR_SEL   = 27
  {4, 8, SMM_CPU_OFFSET (x86._DR7)      , SMM_CPU_OFFSET (x64._DR7)              , SMM_CPU_OFFSET (x64._DR7)         + 4, FALSE},   //  EFI_SMM_SAVE_STATE_REGISTER_DR7      = 28
  {4, 8, SMM_CPU_OFFSET (x86._DR6)      , SMM_CPU_OFFSET (x64._DR6)              , SMM_CPU_OFFSET (x64._DR6)         + 4, FALSE},   //  EFI_SMM_SAVE_STATE_REGISTER_DR6      = 29
  {0, 8, 0                              , SMM_CPU_OFFSET (x64._R8)               , SMM_CPU_OFFSET (x64._R8)          + 4, TRUE},   //  EFI_SMM_SAVE_STATE_REGISTER_R8       = 30
  {0, 8, 0                              , SMM_CPU_OFFSET (x64._R9)               , SMM_CPU_OFFSET (x64._R9)          + 4, TRUE},   //  EFI_SMM_SAVE_STATE_REGISTER_R9       = 31
  {0, 8, 0                              , SMM_CPU_OFFSET (x64._R10)              , SMM_CPU_OFFSET (x64._R10)         + 4, TRUE},   //  EFI_SMM_SAVE_STATE_REGISTER_R10      = 32
  {0, 8, 0                              , SMM_CPU_OFFSET (x64._R11)              , SMM_CPU_OFFSET (x64._R11)         + 4, TRUE},   //  EFI_SMM_SAVE_STATE_REGISTER_R11      = 33
  {0, 8, 0                              , SMM_CPU_OFFSET (x64._R12)              , SMM_CPU_OFFSET (x64._R12)         + 4, TRUE},   //  EFI_SMM_SAVE_STATE_REGISTER_R12      = 34
  {0, 8, 0                              , SMM_CPU_OFFSET (x64._R13)              , SMM_CPU_OFFSET (x64._R13)         + 4, TRUE},   //  EFI_SMM_SAVE_STATE_REGISTER_R13      = 35
  {0, 8, 0                              , SMM_CPU_OFFSET (x64._R14)              , SMM_CPU_OFFSET (x64._R14)         + 4, TRUE},   //  EFI_SMM_SAVE_STATE_REGISTER_R14      = 36
  {0, 8, 0                              , SMM_CPU_OFFSET (x64._R15)              , SMM_CPU_OFFSET (x64._R15)         + 4, TRUE},   //  EFI_SMM_SAVE_STATE_REGISTER_R15      = 37
  {4, 8, SMM_CPU_OFFSET (x86._EAX)      , SMM_CPU_OFFSET (x64._RAX)              , SMM_CPU_OFFSET (x64._RAX)         + 4, TRUE},   //  EFI_SMM_SAVE_STATE_REGISTER_RAX      = 38
  {4, 8, SMM_CPU_OFFSET (x86._EBX)      , SMM_CPU_OFFSET (x64._RBX)              , SMM_CPU_OFFSET (x64._RBX)         + 4, TRUE},   //  EFI_SMM_SAVE_STATE_REGISTER_RBX      = 39
  {4, 8, SMM_CPU_OFFSET (x86._ECX)      , SMM_CPU_OFFSET (x64._RCX)              , SMM_CPU_OFFSET (x64._RCX)         + 4, TRUE},   //  EFI_SMM_SAVE_STATE_REGISTER_RBX      = 39
  {4, 8, SMM_CPU_OFFSET (x86._EDX)      , SMM_CPU_OFFSET (x64._RDX)              , SMM_CPU_OFFSET (x64._RDX)         + 4, TRUE},   //  EFI_SMM_SAVE_STATE_REGISTER_RDX      = 41
  {4, 8, SMM_CPU_OFFSET (x86._ESP)      , SMM_CPU_OFFSET (x64._RSP)              , SMM_CPU_OFFSET (x64._RSP)         + 4, TRUE},   //  EFI_SMM_SAVE_STATE_REGISTER_RSP      = 42
  {4, 8, SMM_CPU_OFFSET (x86._EBP)      , SMM_CPU_OFFSET (x64._RBP)              , SMM_CPU_OFFSET (x64._RBP)         + 4, TRUE},   //  EFI_SMM_SAVE_STATE_REGISTER_RBP      = 43
  {4, 8, SMM_CPU_OFFSET (x86._ESI)      , SMM_CPU_OFFSET (x64._RSI)              , SMM_CPU_OFFSET (x64._RSI)         + 4, TRUE},   //  EFI_SMM_SAVE_STATE_REGISTER_RSI      = 44
  {4, 8, SMM_CPU_OFFSET (x86._EDI)      , SMM_CPU_OFFSET (x64._RDI)              , SMM_CPU_OFFSET (x64._RDI)         + 4, TRUE},   //  EFI_SMM_SAVE_STATE_REGISTER_RDI      = 45
  {4, 8, SMM_CPU_OFFSET (x86._EIP)      , SMM_CPU_OFFSET (x64._RIP)              , SMM_CPU_OFFSET (x64._RIP)         + 4, TRUE},   //  EFI_SMM_SAVE_STATE_REGISTER_RIP      = 46

  {4, 8, SMM_CPU_OFFSET (x86._EFLAGS)   , SMM_CPU_OFFSET (x64._RFLAGS)           , SMM_CPU_OFFSET (x64._RFLAGS)      + 4, TRUE},   //  EFI_SMM_SAVE_STATE_REGISTER_RFLAGS   = 51
  {4, 8, SMM_CPU_OFFSET (x86._CR0)      , SMM_CPU_OFFSET (x64._CR0)              , SMM_CPU_OFFSET (x64._CR0)         + 4, FALSE},   //  EFI_SMM_SAVE_STATE_REGISTER_CR0      = 52
  {4, 8, SMM_CPU_OFFSET (x86._CR3)      , SMM_CPU_OFFSET (x64._CR3)              , SMM_CPU_OFFSET (x64._CR3)         + 4, FALSE},   //  EFI_SMM_SAVE_STATE_REGISTER_CR3      = 53
  {0, 8, 0                              , SMM_CPU_OFFSET (x64._CR4)              , SMM_CPU_OFFSET (x64._CR4)         + 4, FALSE},   //  EFI_SMM_SAVE_STATE_REGISTER_CR4      = 54
  {0, 0, 0, 0, 0}
};


/**
  Read information from the CPU save state.

  @param  Register  Specifies the CPU register to read form the save state.

  @retval 0   Register is not valid
  @retval >0  Index into mSmmCpuWidthOffset[] associated with Register

**/
static UINTN
GetRegisterIndex (
  IN EFI_SMM_SAVE_STATE_REGISTER  Register
  )
{
  UINTN  Index;
  UINTN  Offset;

  for (Index = 0, Offset = SMM_SAVE_STATE_REGISTER_MAX_INDEX; mSmmCpuRegisterRanges[Index].Length != 0; Index++) {
    if (Register >= mSmmCpuRegisterRanges[Index].Start && Register <= mSmmCpuRegisterRanges[Index].End) {
      return Register - mSmmCpuRegisterRanges[Index].Start + Offset;
    }
    Offset += mSmmCpuRegisterRanges[Index].Length;
  }
  return 0;
}

/**
  Read a CPU Save State register on the target processor.

  This function abstracts the differences that whether the CPU Save State register is in the
  IA32 CPU Save State Map or X64 CPU Save State Map.

  This function supports reading a CPU Save State register in SMBase relocation handler.

  @param[in]  CpuIndex       Specifies the zero-based index of the CPU save state.
  @param[in]  RegisterIndex  Index into mSmmCpuWidthOffset[] look up table.
  @param[in]  Width          The number of bytes to read from the CPU save state.
  @param[out] Buffer         Upon return, this holds the CPU register value read from the save state.

  @retval EFI_SUCCESS           The register was read from Save State.
  @retval EFI_NOT_FOUND         The register is not defined for the Save State of Processor.
  @retval EFI_INVALID_PARAMTER  This or Buffer is NULL.

**/
static EFI_STATUS
ReadSaveStateRegisterByIndex (
  IN UINTN   CpuIndex,
  IN UINTN   RegisterIndex,
  IN UINTN   Width,
  OUT VOID   *Buffer
  )
{
  AMD_SMRAM_SAVE_STATE_MAP  *CpuSaveState;
  //UINT32                    SmmRevId;


  if (RegisterIndex == 0) {
    return EFI_NOT_FOUND;
  }

  CpuSaveState = gSmst->CpuSaveState[CpuIndex];
  //SmmRevId = CpuSaveState->x86.SMMRevId;


  if (mSmmSaveStateRegisterLma == EFI_SMM_SAVE_STATE_REGISTER_LMA_32BIT) {
    //
    // If 32-bit mode width is zero, then the specified register can not be accessed
    //
    if (mSmmCpuWidthOffset[RegisterIndex].Width32 == 0) {
      return EFI_NOT_FOUND;
    }

    //
    // If Width is bigger than the 32-bit mode width, then the specified register can not be accessed
    //
    if (Width > mSmmCpuWidthOffset[RegisterIndex].Width32) {
      return EFI_INVALID_PARAMETER;
    }

    //
    // Write return buffer
    //
    ASSERT(CpuSaveState != NULL);
    CopyMem(Buffer, (UINT8 *)CpuSaveState + mSmmCpuWidthOffset[RegisterIndex].Offset32, Width);
  } else {
    //
    // If 64-bit mode width is zero, then the specified register can not be accessed
    //
    if (mSmmCpuWidthOffset[RegisterIndex].Width64 == 0) {
      return EFI_NOT_FOUND;
    }

    //
    // If Width is bigger than the 64-bit mode width, then the specified register can not be accessed
    //
    if (Width > mSmmCpuWidthOffset[RegisterIndex].Width64) {
      return EFI_INVALID_PARAMETER;
    }

    //
    // Write lower 32-bits of return buffer
    //
    CopyMem(Buffer, (UINT8 *)CpuSaveState + mSmmCpuWidthOffset[RegisterIndex].Offset64Lo, MIN(4, Width));
    if (Width >= 4) {
      //
      // Write upper 32-bits of return buffer
      //
      CopyMem((UINT8 *)Buffer + 4, (UINT8 *)CpuSaveState + mSmmCpuWidthOffset[RegisterIndex].Offset64Hi, Width - 4);
    }
  }
  return EFI_SUCCESS;
}


/**
  Read an SMM Save State register on the target processor.  If this function
  returns EFI_UNSUPPORTED, then the caller is responsible for reading the
  SMM Save Sate register.

  @param[in]  CpuIndex  The index of the CPU to read the SMM Save State.  The
                        value must be between 0 and the NumberOfCpus field in
                        the System Management System Table (SMST).
  @param[in]  Register  The SMM Save State register to read.
  @param[in]  Width     The number of bytes to read from the CPU save state.
  @param[out] Buffer    Upon return, this holds the CPU register value read
                        from the save state.

  @retval EFI_SUCCESS           The register was read from Save State.
  @retval EFI_INVALID_PARAMTER  Buffer is NULL.
  @retval EFI_UNSUPPORTED       This function does not support reading Register.

**/
EFI_STATUS
EFIAPI
SmmCpuFeaturesReadSaveStateRegister (
  IN  UINTN                        CpuIndex,
  IN  EFI_SMM_SAVE_STATE_REGISTER  Register,
  IN  UINTN                        Width,
  OUT VOID                         *Buffer
  )
{
  UINT32                      SmmRevId;
  EFI_SMM_SAVE_STATE_IO_INFO  *IoInfo;
  AMD_SMRAM_SAVE_STATE_MAP    *CpuSaveState;
  UINT8                       DataWidth;

  // Read CPU State
  CpuSaveState = (AMD_SMRAM_SAVE_STATE_MAP*)gSmst->CpuSaveState[CpuIndex];


  // Check for special EFI_SMM_SAVE_STATE_REGISTER_LMA
  if (Register == EFI_SMM_SAVE_STATE_REGISTER_LMA) {

    // Only byte access is supported for this register
    if (Width != 1) {
      return EFI_INVALID_PARAMETER;
    }

    *(UINT8 *)Buffer = mSmmSaveStateRegisterLma;

    return EFI_SUCCESS;
  }


  // Check for special EFI_SMM_SAVE_STATE_REGISTER_IO

  if (Register == EFI_SMM_SAVE_STATE_REGISTER_IO) {
    //
    // Get SMM Revision ID
    //
    ReadSaveStateRegisterByIndex (CpuIndex, SMM_SAVE_STATE_REGISTER_SMMREVID_INDEX, sizeof(SmmRevId), &SmmRevId);

    //
    // See if the CPU supports the IOMisc register in the save state
    //
    if (SmmRevId < AMD_SMM_MIN_REV_ID_x64) {
      return EFI_NOT_FOUND;
    }

    // Check if IO Restart Dword [IO Trap] is valid or not using bit 1.
    if (!(CpuSaveState->x64.IO_DWord & 0x02u)) {
      return EFI_NOT_FOUND;
    }

    // Zero the IoInfo structure that will be returned in Buffer
    IoInfo = (EFI_SMM_SAVE_STATE_IO_INFO *)Buffer;
    ZeroMem (IoInfo, sizeof(EFI_SMM_SAVE_STATE_IO_INFO));

    IoInfo->IoPort = (UINT16) (CpuSaveState->x64.IO_DWord >> 16u);

    if (CpuSaveState->x64.IO_DWord & 0x10u) {
      IoInfo->IoWidth = EFI_SMM_SAVE_STATE_IO_WIDTH_UINT8;
      DataWidth = 0x01u;
    }
    else if (CpuSaveState->x64.IO_DWord & 0x20u) {
      IoInfo->IoWidth = EFI_SMM_SAVE_STATE_IO_WIDTH_UINT16;
      DataWidth = 0x02u;
    }
    else {
      IoInfo->IoWidth = EFI_SMM_SAVE_STATE_IO_WIDTH_UINT32;
      DataWidth = 0x04u;
    }

    if (CpuSaveState->x64.IO_DWord & 0x01u) {
      IoInfo->IoType = EFI_SMM_SAVE_STATE_IO_TYPE_INPUT;
    }
    else {
      IoInfo->IoType = EFI_SMM_SAVE_STATE_IO_TYPE_OUTPUT;
    }

    if (IoInfo->IoType == EFI_SMM_SAVE_STATE_IO_TYPE_INPUT || IoInfo->IoType == EFI_SMM_SAVE_STATE_IO_TYPE_OUTPUT) {
      SmmCpuFeaturesReadSaveStateRegister (CpuIndex, EFI_SMM_SAVE_STATE_REGISTER_RAX, DataWidth, &IoInfo->IoData);
    }

    return EFI_SUCCESS;
  }

  // Convert Register to a register lookup table index
  return ReadSaveStateRegisterByIndex (CpuIndex, GetRegisterIndex (Register), Width, Buffer);
}


/**
  Writes an SMM Save State register on the target processor.  If this function
  returns EFI_UNSUPPORTED, then the caller is responsible for writing the
  SMM Save Sate register.

  @param[in] CpuIndex  The index of the CPU to write the SMM Save State.  The
                       value must be between 0 and the NumberOfCpus field in
                       the System Management System Table (SMST).
  @param[in] Register  The SMM Save State register to write.
  @param[in] Width     The number of bytes to write to the CPU save state.
  @param[in] Buffer    Upon entry, this holds the new CPU register value.

  @retval EFI_SUCCESS           The register was written to Save State.
  @retval EFI_INVALID_PARAMTER  Buffer is NULL.
  @retval EFI_UNSUPPORTED       This function does not support writing Register.
**/
EFI_STATUS
EFIAPI
SmmCpuFeaturesWriteSaveStateRegister (
  IN UINTN                        CpuIndex,
  IN EFI_SMM_SAVE_STATE_REGISTER  Register,
  IN UINTN                        Width,
  IN CONST VOID                   *Buffer
  )
{
  UINTN                 RegisterIndex;
  AMD_SMRAM_SAVE_STATE_MAP  *CpuSaveState;

  //
  // Writes to EFI_SMM_SAVE_STATE_REGISTER_LMA are ignored
  //
  if (Register == EFI_SMM_SAVE_STATE_REGISTER_LMA) {
    return EFI_SUCCESS;
  }

  //
  // Writes to EFI_SMM_SAVE_STATE_REGISTER_IO are not supported
  //
  if (Register == EFI_SMM_SAVE_STATE_REGISTER_IO) {
    return EFI_NOT_FOUND;
  }

  //
  // Convert Register to a register lookup table index
  //
  RegisterIndex = GetRegisterIndex (Register);
  if (RegisterIndex == 0) {
    return EFI_NOT_FOUND;
  }

  CpuSaveState = gSmst->CpuSaveState[CpuIndex];

  //
  // Do not write non-writable SaveState, because it will cause exception.
  //
  if (!mSmmCpuWidthOffset[RegisterIndex].Writeable) {
    return EFI_UNSUPPORTED;
  }

  //
  // Check CPU mode
  //
  if (mSmmSaveStateRegisterLma == EFI_SMM_SAVE_STATE_REGISTER_LMA_32BIT) {
    //
    // If 32-bit mode width is zero, then the specified register can not be accessed
    //
    if (mSmmCpuWidthOffset[RegisterIndex].Width32 == 0) {
      return EFI_NOT_FOUND;
    }

    //
    // If Width is bigger than the 32-bit mode width, then the specified register can not be accessed
    //
    if (Width > mSmmCpuWidthOffset[RegisterIndex].Width32) {
      return EFI_INVALID_PARAMETER;
    }
    //
    // Write SMM State register
    //
    ASSERT (CpuSaveState != NULL);
    CopyMem((UINT8 *)CpuSaveState + mSmmCpuWidthOffset[RegisterIndex].Offset32, Buffer, Width);
  } else {
    //
    // If 64-bit mode width is zero, then the specified register can not be accessed
    //
    if (mSmmCpuWidthOffset[RegisterIndex].Width64 == 0) {
      return EFI_NOT_FOUND;
    }

    //
    // If Width is bigger than the 64-bit mode width, then the specified register can not be accessed
    //
    if (Width > mSmmCpuWidthOffset[RegisterIndex].Width64) {
      return EFI_INVALID_PARAMETER;
    }

    //
    // Write lower 32-bits of SMM State register
    //
    CopyMem((UINT8 *)CpuSaveState + mSmmCpuWidthOffset[RegisterIndex].Offset64Lo, Buffer, MIN (4, Width));
    if (Width >= 4) {
      //
      // Write upper 32-bits of SMM State register
      //
      CopyMem((UINT8 *)CpuSaveState + mSmmCpuWidthOffset[RegisterIndex].Offset64Hi, (UINT8 *)Buffer + 4, Width - 4);
    }
  }
  return EFI_SUCCESS;
}


/**
  The constructor function

  @param[in]  ImageHandle  The firmware allocated handle for the EFI image.
  @param[in]  SystemTable  A pointer to the EFI System Table.

  @retval EFI_SUCCESS      The constructor always returns EFI_SUCCESS.

**/
EFI_STATUS
EFIAPI
SmmCpuFeaturesLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  UINT32   LMAValue;
  LMAValue = (UINT32)AsmReadMsr64 (EFER_ADDRESS) & LMA;

  mSmmSaveStateRegisterLma = EFI_SMM_SAVE_STATE_REGISTER_LMA_32BIT;
  if (LMAValue)
  {
    mSmmSaveStateRegisterLma = EFI_SMM_SAVE_STATE_REGISTER_LMA_64BIT;
  }


  return EFI_SUCCESS;
}

/**
  Called during the very first SMI into System Management Mode to initialize
  CPU features, including SMBASE, for the currently executing CPU.  Since this
  is the first SMI, the SMRAM Save State Map is at the default address of
  AMD_SMM_DEFAULT_SMBASE + SMRAM_SAVE_STATE_MAP_OFFSET.  The currently executing
  CPU is specified by CpuIndex and CpuIndex can be used to access information
  about the currently executing CPU in the ProcessorInfo array and the
  HotPlugCpuData data structure.

  @param[in] CpuIndex        The index of the CPU to initialize.  The value
                             must be between 0 and the NumberOfCpus field in
                             the System Management System Table (SMST).
  @param[in] IsMonarch       TRUE if the CpuIndex is the index of the CPU that
                             was elected as monarch during System Management
                             Mode initialization.
                             FALSE if the CpuIndex is not the index of the CPU
                             that was elected as monarch during System
                             Management Mode initialization.
  @param[in] ProcessorInfo   Pointer to an array of EFI_PROCESSOR_INFORMATION
                             structures.  ProcessorInfo[CpuIndex] contains the
                             information for the currently executing CPU.
  @param[in] CpuHotPlugData  Pointer to the CPU_HOT_PLUG_DATA structure that
                             contains the ApidId and SmBase arrays.
**/
VOID
EFIAPI
SmmCpuFeaturesInitializeProcessor (
  IN UINTN                      CpuIndex,
  IN BOOLEAN                    IsMonarch,
  IN EFI_PROCESSOR_INFORMATION  *ProcessorInfo,
  IN CPU_HOT_PLUG_DATA          *CpuHotPlugData
  )
{
  UINT32   LMAValue;
  AMD_SMRAM_SAVE_STATE_MAP  *CpuState;
  //
  // Configure SMBASE.
  //
  CpuState = (AMD_SMRAM_SAVE_STATE_MAP *)(UINTN)(AMD_SMM_DEFAULT_SMBASE + SMRAM_SAVE_STATE_MAP_OFFSET);
  CpuState->x64.SMBASE = (UINT32)CpuHotPlugData->SmBase[CpuIndex];

  // Re-initialize the value of mSmmSaveStateRegisterLma flag which might have been changed in PiCpuSmmDxeSmm Driver
  // Entry point, to make sure correct value on AMD platform is assigned to be used by SmmCpuFeaturesLib.
  LMAValue = (UINT32)AsmReadMsr64 (EFER_ADDRESS) & LMA;
  mSmmSaveStateRegisterLma = EFI_SMM_SAVE_STATE_REGISTER_LMA_32BIT;
  if (LMAValue)
  {
    mSmmSaveStateRegisterLma = EFI_SMM_SAVE_STATE_REGISTER_LMA_64BIT;
  }

  //
  // If SMRR is supported, then program SMRR base/mask MSRs.
  // The EFI_MSR_SMRR_PHYS_MASK_VALID bit is not set until the first normal SMI.
  // The code that initializes SMM environment is running in normal mode
  // from SMRAM region.  If SMRR is enabled here, then the SMRAM region
  // is protected and the normal mode code execution will fail.
  //
  if (mSmrrSupported) {
    //
    // SMRR size cannot be less than 4-KBytes
    // SMRR size must be of length 2^n
    // SMRR base alignment cannot be less than SMRR length
    //
    if ((CpuHotPlugData->SmrrSize < SIZE_4KB) ||
        (CpuHotPlugData->SmrrSize != GetPowerOfTwo32 (CpuHotPlugData->SmrrSize)) ||
        ((CpuHotPlugData->SmrrBase & ~(CpuHotPlugData->SmrrSize - 1)) != CpuHotPlugData->SmrrBase)) {
      //
      // Print message and halt if CPU is Monarch
      //
      if (IsMonarch) {
        DEBUG ((EFI_D_ERROR, "SMM Base/Size does not meet alignment/size requirement!\n"));
        CpuDeadLoop ();
      }
    } else {
      AsmWriteMsr64 (SMMADDR_ADDRESS, CpuHotPlugData->SmrrBase);
      AsmWriteMsr64 (SMMMASK_ADDRESS, ((~(UINT64)(CpuHotPlugData->SmrrSize - 1)) | 0x6600));
    }
  }
}

/**
  This function updates the SMRAM save state on the currently executing CPU
  to resume execution at a specific address after an RSM instruction.  This
  function must evaluate the SMRAM save state to determine the execution mode
  the RSM instruction resumes and update the resume execution address with
  either NewInstructionPointer32 or NewInstructionPoint.  The auto HALT restart
  flag in the SMRAM save state must always be cleared.  This function returns
  the value of the instruction pointer from the SMRAM save state that was
  replaced.  If this function returns 0, then the SMRAM save state was not
  modified.

  This function is called during the very first SMI on each CPU after
  SmmCpuFeaturesInitializeProcessor() to set a flag in normal execution mode
  to signal that the SMBASE of each CPU has been updated before the default
  SMBASE address is used for the first SMI to the next CPU.

  @param[in] CpuIndex                 The index of the CPU to hook.  The value
                                      must be between 0 and the NumberOfCpus
                                      field in the System Management System Table
                                      (SMST).
  @param[in] CpuState                 Pointer to SMRAM Save State Map for the
                                      currently executing CPU.
  @param[in] NewInstructionPointer32  Instruction pointer to use if resuming to
                                      32-bit execution mode from 64-bit SMM.
  @param[in] NewInstructionPointer    Instruction pointer to use if resuming to
                                      same execution mode as SMM.

  @retval 0    This function did modify the SMRAM save state.
  @retval > 0  The original instruction pointer value from the SMRAM save state
               before it was replaced.
**/
UINT64
EFIAPI
SmmCpuFeaturesHookReturnFromSmm (
  IN UINTN                 CpuIndex,
  IN SMRAM_SAVE_STATE_MAP  *CpuState,
  IN UINT64                NewInstructionPointer32,
  IN UINT64                NewInstructionPointer
  )
{
  UINT64                OriginalInstructionPointer;
  AMD_SMRAM_SAVE_STATE_MAP  *AmdCpuState;

  AmdCpuState = (AMD_SMRAM_SAVE_STATE_MAP*)CpuState;

  if (mSmmSaveStateRegisterLma == EFI_SMM_SAVE_STATE_REGISTER_LMA_32BIT) {
    OriginalInstructionPointer = (UINT64)AmdCpuState->x86._EIP;
    AmdCpuState->x86._EIP = (UINT32)NewInstructionPointer;
    //
    // Clear the auto HALT restart flag so the RSM instruction returns
    // program control to the instruction following the HLT instruction.
    //
    if ((AmdCpuState->x86.AutoHALTRestart & BIT0) != 0) {
      AmdCpuState->x86.AutoHALTRestart &= ~BIT0;
    }
  } else {
    OriginalInstructionPointer = AmdCpuState->x64._RIP;
    if ((AmdCpuState->x64.EFER & LMA) == 0) {
      AmdCpuState->x64._RIP = (UINT32)NewInstructionPointer32;
    } else {
      AmdCpuState->x64._RIP = (UINT32)NewInstructionPointer;
    }
    //
    // Clear the auto HALT restart flag so the RSM instruction returns
    // program control to the instruction following the HLT instruction.
    //
    if ((AmdCpuState->x64.AutoHALTRestart & BIT0) != 0) {
      AmdCpuState->x64.AutoHALTRestart &= ~BIT0;
    }
  }
  return OriginalInstructionPointer;
}

/**
  Hook point in normal execution mode that allows the one CPU that was elected
  as monarch during System Management Mode initialization to perform additional
  initialization actions immediately after all of the CPUs have processed their
  first SMI and called SmmCpuFeaturesInitializeProcessor() relocating SMBASE
  into a buffer in SMRAM and called SmmCpuFeaturesHookReturnFromSmm().
**/
VOID
EFIAPI
SmmCpuFeaturesSmmRelocationComplete (
  VOID
  )
{
}

/**
  Return the size, in bytes, of a custom SMI Handler in bytes.  If 0 is
  returned, then a custom SMI handler is not provided by this library,
  and the default SMI handler must be used.

  @retval 0    Use the default SMI handler.
  @retval > 0  Use the SMI handler installed by SmmCpuFeaturesInstallSmiHandler()
               The caller is required to allocate enough SMRAM for each CPU to
               support the size of the custom SMI handler.
**/
UINTN
EFIAPI
SmmCpuFeaturesGetSmiHandlerSize (
  VOID
  )
{
  return 0;
}

/**
  Install a custom SMI handler for the CPU specified by CpuIndex.  This function
  is only called if SmmCpuFeaturesGetSmiHandlerSize() returns a size is greater
  than zero and is called by the CPU that was elected as monarch during System
  Management Mode initialization.

  @param[in] CpuIndex   The index of the CPU to install the custom SMI handler.
                        The value must be between 0 and the NumberOfCpus field
                        in the System Management System Table (SMST).
  @param[in] SmBase     The SMBASE address for the CPU specified by CpuIndex.
  @param[in] SmiStack   The stack to use when an SMI is processed by the
                        the CPU specified by CpuIndex.
  @param[in] StackSize  The size, in bytes, if the stack used when an SMI is
                        processed by the CPU specified by CpuIndex.
  @param[in] GdtBase    The base address of the GDT to use when an SMI is
                        processed by the CPU specified by CpuIndex.
  @param[in] GdtSize    The size, in bytes, of the GDT used when an SMI is
                        processed by the CPU specified by CpuIndex.
  @param[in] IdtBase    The base address of the IDT to use when an SMI is
                        processed by the CPU specified by CpuIndex.
  @param[in] IdtSize    The size, in bytes, of the IDT used when an SMI is
                        processed by the CPU specified by CpuIndex.
  @param[in] Cr3        The base address of the page tables to use when an SMI
                        is processed by the CPU specified by CpuIndex.
**/
VOID
EFIAPI
SmmCpuFeaturesInstallSmiHandler (
  IN UINTN   CpuIndex,
  IN UINT32  SmBase,
  IN VOID    *SmiStack,
  IN UINTN   StackSize,
  IN UINTN   GdtBase,
  IN UINTN   GdtSize,
  IN UINTN   IdtBase,
  IN UINTN   IdtSize,
  IN UINT32  Cr3
  )
{
}

/**
  Determines if MTRR registers must be configured to set SMRAM cache-ability
  when executing in System Management Mode.

  @retval TRUE   MTRR registers must be configured to set SMRAM cache-ability.
  @retval FALSE  MTRR registers do not need to be configured to set SMRAM
                 cache-ability.
**/
BOOLEAN
EFIAPI
SmmCpuFeaturesNeedConfigureMtrrs (
  VOID
  )
{
  return FALSE;
}

/**
  Disable SMRR register if SMRR is supported and SmmCpuFeaturesNeedConfigureMtrrs()
  returns TRUE.
**/
VOID
EFIAPI
SmmCpuFeaturesDisableSmrr (
  VOID
  )
{

}

/**
  Enable SMRR register if SMRR is supported and SmmCpuFeaturesNeedConfigureMtrrs()
  returns TRUE.
**/
VOID
EFIAPI
SmmCpuFeaturesReenableSmrr (
  VOID
  )
{

}

/**
  Processor specific hook point each time a CPU enters System Management Mode.

  @param[in] CpuIndex  The index of the CPU that has entered SMM.  The value
                       must be between 0 and the NumberOfCpus field in the
                       System Management System Table (SMST).
**/
VOID
EFIAPI
SmmCpuFeaturesRendezvousEntry (
  IN UINTN  CpuIndex
  )
{
}

/**
  Processor specific hook point each time a CPU exits System Management Mode.

  @param[in] CpuIndex  The index of the CPU that is exiting SMM.  The value must
                       be between 0 and the NumberOfCpus field in the System
                       Management System Table (SMST).
**/
VOID
EFIAPI
SmmCpuFeaturesRendezvousExit (
  IN UINTN  CpuIndex
  )
{

}

/**
  Check to see if an SMM register is supported by a specified CPU.

  @param[in] CpuIndex  The index of the CPU to check for SMM register support.
                       The value must be between 0 and the NumberOfCpus field
                       in the System Management System Table (SMST).
  @param[in] RegName   Identifies the SMM register to check for support.

  @retval TRUE   The SMM register specified by RegName is supported by the CPU
                 specified by CpuIndex.
  @retval FALSE  The SMM register specified by RegName is not supported by the
                 CPU specified by CpuIndex.
**/
BOOLEAN
EFIAPI
SmmCpuFeaturesIsSmmRegisterSupported (
  IN UINTN         CpuIndex,
  IN SMM_REG_NAME  RegName
  )
{
  if (mSmmFeatureControlSupported && RegName == SmmRegFeatureControl) {
    return TRUE;
  }
  return FALSE;
}

/**
  Returns the current value of the SMM register for the specified CPU.
  If the SMM register is not supported, then 0 is returned.

  @param[in] CpuIndex  The index of the CPU to read the SMM register.  The
                       value must be between 0 and the NumberOfCpus field in
                       the System Management System Table (SMST).
  @param[in] RegName   Identifies the SMM register to read.

  @return  The value of the SMM register specified by RegName from the CPU
           specified by CpuIndex.
**/
UINT64
EFIAPI
SmmCpuFeaturesGetSmmRegister (
  IN UINTN         CpuIndex,
  IN SMM_REG_NAME  RegName
  )
{
  return 0;
}

/**
  Sets the value of an SMM register on a specified CPU.
  If the SMM register is not supported, then no action is performed.

  @param[in] CpuIndex  The index of the CPU to write the SMM register.  The
                       value must be between 0 and the NumberOfCpus field in
                       the System Management System Table (SMST).
  @param[in] RegName   Identifies the SMM register to write.
                       registers are read-only.
  @param[in] Value     The value to write to the SMM register.
**/
VOID
EFIAPI
SmmCpuFeaturesSetSmmRegister (
  IN UINTN         CpuIndex,
  IN SMM_REG_NAME  RegName,
  IN UINT64        Value
  )
{

}

/**
  This function is hook point called after the gEfiSmmReadyToLockProtocolGuid
  notification is completely processed.
**/
VOID
EFIAPI
SmmCpuFeaturesCompleteSmmReadyToLock (
  VOID
  )
{
}

/**
  This API provides a method for a CPU to allocate a specific region for storing page tables.

  This API can be called more once to allocate memory for page tables.

  Allocates the number of 4KB pages of type EfiRuntimeServicesData and returns a pointer to the
  allocated buffer.  The buffer returned is aligned on a 4KB boundary.  If Pages is 0, then NULL
  is returned.  If there is not enough memory remaining to satisfy the request, then NULL is
  returned.

  This function can also return NULL if there is no preference on where the page tables are allocated in SMRAM.

  @param  Pages                 The number of 4 KB pages to allocate.

  @return A pointer to the allocated buffer for page tables.
  @retval NULL      Fail to allocate a specific region for storing page tables,
                    Or there is no preference on where the page tables are allocated in SMRAM.

**/
VOID *
EFIAPI
SmmCpuFeaturesAllocatePageTableMemory (
  IN UINTN           Pages
  )
{
  return NULL;
}

