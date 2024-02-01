;
; @file         MathU64.nasm
; @brief        Intrinsic functions for 64-bit data shifting in 32-bit CPU mode.
;
; Copyright 2021-2024 Advanced Micro Devices, Inc. All rights reserved.
;


    global __aullshr
    global __allshl

    section .text

;***
;__aullshr - long shift right
;
;Purpose:
;       Does a unsigned Long Shift Right
;       Shifts a long right any number of bits.
;
;Entry:
;       EDX:EAX - long value to be shifted
;       CL    - number of bits to shift by
;
;Exit:
;       EDX:EAX - shifted value
;
;Uses:
;       CL is destroyed.
;
;*******************************************************************************
__aullshr:
    ;
    ; Checking: Only handle 64bit shifting or more
    ;
    cmp     cl, 64
    jae     ._exit

    ;
    ; Handle shifting between 0 and 31 bits
    ;
    cmp     cl, 32
    jae     .more32
    shrd    eax, edx, cl
    shr     edx, cl
    ret

    ;
    ; Handle shifting of 32-63 bits
    ;
.more32:
    mov     eax, edx
    xor     edx, edx
    and     cl, 31
    shr     eax, cl
    ret

    ;
    ; Invalid number (less then 32bits), return 0
    ;
._exit:
    xor     eax, eax
    xor     edx, edx
    ret

;***
;_allshl - long shift left
;
;Purpose:
;       Does a Long Shift Left (signed and unsigned are identical)
;       Shifts a long left any number of bits.
;
;Entry:
;       EDX:EAX - long value to be shifted
;       CL    - number of bits to shift by
;
;Exit:
;       EDX:EAX - shifted value
;
;Uses:
;       CL is destroyed.
;
;*******************************************************************************
__allshl:
;
; Handle shifts of 64 or more bits (all get 0)
;
        cmp     cl, 64
        jae     ._exit

;
; Handle shifts of between 0 and 31 bits
;
        cmp     cl, 32
        jae     .more32
        shld    edx,eax,cl
        shl     eax,cl
        ret

;
; Handle shifts of between 32 and 63 bits
;
.more32:
        mov     edx,eax
        xor     eax,eax
        and     cl,31
        shl     edx,cl
        ret

;
; return 0 in edx:eax
;
._exit:
        xor     eax,eax
        xor     edx,edx
        ret


