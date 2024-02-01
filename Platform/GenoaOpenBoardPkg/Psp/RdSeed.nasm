;*****************************************************************************
;
; Copyright (C) 2008-2024 Advanced Micro Devices, Inc. All rights reserved.
;
;******************************************************************************

    DEFAULT REL
    SECTION .text

global ASM_PFX(X86RdSeed64)
ASM_PFX(X86RdSeed64):
    ; rdseed   eax
    ; Hardware modifies the CF flag to indicate whether the value returned in the destination register is
    ; valid. If CF = 1, the value is valid. If CF = 0, the value is invalid. Software must test the state of the CF
    ; flag prior to using the value returned in the destination register to determine if the value is valid.
    db     0xf, 0xc7, 0xf8
    jc     rn32_ok
    xor    rax, rax     ;Error return FALSE
    ret
rn32_ok:
    mov    [rcx], eax
    mov    rax,  1      ;return TRUE
    ret
