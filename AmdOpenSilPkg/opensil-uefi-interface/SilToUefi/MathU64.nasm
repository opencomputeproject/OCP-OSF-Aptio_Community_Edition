;
; @file         MathU64.nasm
; @brief        Intrinsic functions for 64-bit data calculations in 32-bit CPU mode.
;
; Copyright 2021-2023 Advanced Micro Devices, Inc. All rights reserved.
;

    global __allmul
    global __aulldiv
    global __aullrem
    global __udivdi3
    global __umoddi3

    section .text



;***
;__allmul - long multiply routine
;
;Purpose:
;       Does a long multiply (same for signed/unsigned)
;       Parameters are not changed.
;
;Entry:
;       Parameters are passed on the stack:
;               1st pushed: multiplier (QWORD)
;               2nd pushed: multiplicand (QWORD)
;
;Exit:
;       EDX:EAX - product of multiplier and multiplicand
;       NOTE: parameters are removed from the stack
;
;Uses:
;       ECX
;
;*******************************************************************************
__allmul:
;
;       AHI, BHI : upper 32 bits of A and B
;       ALO, BLO : lower 32 bits of A and B
;
;             ALO * BLO
;       ALO * BHI
; +     BLO * AHI
; ---------------------
        mov     eax, [esp + 8]
        mov     ecx, [esp + 16]
        or      ecx, eax         ;test for both hiwords zero.
        mov     ecx, [esp + 12]
        jnz     .hard     ;both are zero, just mult ALO and BLO

        mov     eax,[esp + 4]
        mul     ecx

        ret     16              ; callee restores the stack

.hard:
        push    ebx

; esp has been altered

        mul     ecx             ;eax has AHI, ecx has BLO, so AHI * BLO
        mov     ebx, eax        ;save result

        mov     eax, [esp + 8]
        mul     dword [esp + 20]      ;ALO * BHI
        add     ebx, eax        ;ebx = ((ALO * BHI) + (AHI * BLO))

        mov     eax, [esp + 8]  ;ecx = BLO
        mul     ecx             ;so edx:eax = ALO*BLO
        add     edx,ebx         ;now edx has all the LO*HI stuff

        pop     ebx

        ret     16              ; callee restores the stack

;------------------------------------------------------------------------------
; Divides a 64-bit unsigned value by another 64-bit unsigned value and returns
; the 64-bit result and the 64-bit remainder.
;
;  On entry:
;      [ESP]    : Return Address
;      [ESP+4]  : QWORD. Dividend
;      [ESP+12] : QWORD. Divisor
;  On exit:
;      EDX:EAX contains the quotient (dividend/divisor)
;      EBX:ECX contains the remainder (divided % divisor)
;
;  NOTE: this routine removes parameters from the stack.
;------------------------------------------------------------------------------
_aulldvrm:
    mov     ecx, [esp + 16]             ; ecx <- divisor[32..63]
    test    ecx, ecx
    jnz     .DivRemU64x64               ; call _@DivRemU64x64 if Divisor > 2^32
    jmp     .DivRemU64x32               ; call _@DivRemU64x32 if Divisor < 2^32

.DivRemU64x64:
    push    esi
    push    edi
;   [ESP+12] : QWORD. Dividend
;   [ESP+20] : QWORD. Divisor

    mov     edx, dword [esp + 16]
    mov     eax, dword [esp + 12]       ; edx:eax <- dividend
    mov     edi, edx
    mov     esi, eax                    ; edi:esi <- dividend
    mov     ebx, dword [esp + 20]       ; ecx:ebx <- divisor (ecx has been initialized before jumping to _@DivRemU64x64)
.NextBit:
    shr     edx, 1
    rcr     eax, 1
    shrd    ebx, ecx, 1
    shr     ecx, 1
    jnz     .NextBit
    div     ebx
    mov     ebx, eax                    ; ebx <- quotient
    mov     ecx, [esp + 24]             ; ecx <- high dword of divisor
    mul     dword [esp + 20]            ; edx:eax <- quotient * divisor[0..31]
    imul    ecx, ebx                    ; ecx <- quotient * divisor[32..63]
    add     edx, ecx                    ; edx <- (quotient * divisor)[32..63]
    jc      .TooLarge                   ; product > 2^64
    cmp     edi, edx                    ; compare high 32 bits
    ja      .Correct
    jb      .TooLarge                   ; product > dividend
    cmp     esi, eax
    jae     .Correct                    ; product <= dividend
.TooLarge:
    dec     ebx                         ; adjust quotient by -1
    sub     eax, dword [esp + 20]
    sbb     edx, dword [esp + 24]       ; edx:eax <- (quotient - 1) * divisor
.Correct:
    sub     esi, eax
    sbb     edi, edx                    ; edi:esi <- remainder
    mov     eax, ebx                    ; eax <- quotient
    xor     edx, edx                    ; quotient is 32 bits long
    mov     ebx, edi                    ; ebx <- Remainder[32..63]
    mov     ecx, esi                    ; ecx <- Remainder[0..31]

    pop     edi
    pop     esi
    ret     16                          ; remove parameters from the stack

.DivRemU64x32:
    mov     ecx, [esp + 12]         ; ecx <- divisor
    mov     eax, [esp + 8]          ; eax <- dividend[32..63]
    xor     edx, edx
    div     ecx                     ; eax <- quotient[32..63], edx <- remainder
    push    eax
    mov     eax, [esp + 8]          ; eax <- dividend[0..31]
    div     ecx                     ; eax <- quotient[0..31]
    xor     ebx, ebx                ; ebx <- Remainder[32..63] (always zero)
    mov     ecx, edx                ; ecx <- Remainder[0..31]
    pop     edx                     ; edx <- quotient[32..63]
    ret     16                      ; remove parameters from the stack


;------------------------------------------------------------------------------
; Divides a 64-bit unsigned value by another 64-bit unsigned value and returns
; the 64-bit result.
;
;  On entry:
;      [ESP]    : Return Address
;      [ESP+4]  : QWORD. Dividend
;      [ESP+12] : QWORD. Divisor
;  On exit:
;      EDX:EAX contains the quotient (dividend/divisor)
;
;  NOTES:
;   this routine destroys ECX
;   this routine removes parameters from the stack
;------------------------------------------------------------------------------
__aulldiv:
    mov    [esp-4], ebx ; push ebx

    ; Copy function parameters to the top of the stack
    mov    eax, [esp+4] ; DividentLo
    mov    [esp-20], eax
    mov    eax, [esp+8] ; DividentHi
    mov    [esp-16], eax
    mov    eax, [esp+12] ; DivisorLo
    mov    [esp-12], eax
    mov    eax, [esp+16] ; DivisorHi
    mov    [esp-8], eax

    sub    esp, 20 ; Adjust stack sizeof(Divisor) + sizeof(Dividend) + sizeof(EBX) = 20

    call   _aulldvrm
    pop    ebx
    ret    16


;------------------------------------------------------------------------------
; Divides a 64-bit unsigned value by another 64-bit unsigned value and returns
; the 64-bit remainder.
;
;  On entry:
;      [ESP]    : Return Address
;      [ESP+4]  : QWORD. Dividend
;      [ESP+12] : QWORD. Divisor
;  On exit:
;      EDX:EAX contains the remainder (divided % divisor)
;
;  NOTES:
;   this routine destroys ECX
;   this routine removes parameters from the stack
;------------------------------------------------------------------------------
__aullrem:
    mov    [esp-4], ebx ; push ebx

    ; Copy function parameters to the top of the stack
    mov    eax, [esp+4] ; DividentLo
    mov    [esp-20], eax
    mov    eax, [esp+8] ; DividentHi
    mov    [esp-16], eax
    mov    eax, [esp+12] ; DivisorLo
    mov    [esp-12], eax
    mov    eax, [esp+16] ; DivisorHi
    mov    [esp-8], eax

    sub    esp, 20 ; Adjust stack sizeof(Divisor) + sizeof(Dividend) + sizeof(EBX) = 20

    call   _aulldvrm
    mov    edx, ebx
    mov    eax, ecx
    pop    ebx
    ret    16

;------------------------------------------------------------------------------
; Divides a 64-bit unsigned value by another 64-bit unsigned value and returns
; the 64-bit result.
;
;  On entry:
;      [ESP]    : Return Address
;      [ESP+4]  : QWORD. Dividend
;      [ESP+12] : QWORD. Divisor
;  On exit:
;      EDX:EAX contains the quotient (dividend/divisor)
;
;  NOTES:
;   this routine does not remove parameters from the stack
;------------------------------------------------------------------------------
__udivdi3:
    mov    [esp-4], ebx ; push ebx
    mov    [esp-8], ecx ; push ecx

    ; Copy function parameters to the top of the stack
    mov    eax, [esp+4] ; DividentLo
    mov    [esp-24], eax
    mov    eax, [esp+8] ; DividentHi
    mov    [esp-20], eax
    mov    eax, [esp+12] ; DivisorLo
    mov    [esp-16], eax
    mov    eax, [esp+16] ; DivisorHi
    mov    [esp-12], eax

    sub    esp, 24 ; Adjust stack sizeof(Divisor) + sizeof(Dividend) + sizeof(EBX) + sizeof(ECX)= 24

    call   _aulldvrm

    pop    ecx
    pop    ebx
    ret

;------------------------------------------------------------------------------
; Divides a 64-bit unsigned value by another 64-bit unsigned value and returns
; the 64-bit remainder.
;
;  On entry:
;      [ESP]    : Return Address
;      [ESP+4]  : QWORD. Dividend
;      [ESP+12] : QWORD. Divisor
;  On exit:
;      EDX:EAX contains the remainder (divided % divisor)
;
;  NOTES:
;   this routine does not remove parameters from the stack
;------------------------------------------------------------------------------
__umoddi3:
    mov    [esp-4], ebx ; push ebx
    mov    [esp-8], ecx ; push ecx

    ; Copy function parameters to the top of the stack
    mov    eax, [esp+4] ; DividentLo
    mov    [esp-24], eax
    mov    eax, [esp+8] ; DividentHi
    mov    [esp-20], eax
    mov    eax, [esp+12] ; DivisorLo
    mov    [esp-16], eax
    mov    eax, [esp+16] ; DivisorHi
    mov    [esp-12], eax

    sub    esp, 24 ; Adjust stack sizeof(Divisor) + sizeof(Dividend) + sizeof(EBX) +sizeof(ECX) = 24

    call   _aulldvrm
    mov    edx, ebx
    mov    eax, ecx

    pop    ecx
    pop    ebx
    ret
