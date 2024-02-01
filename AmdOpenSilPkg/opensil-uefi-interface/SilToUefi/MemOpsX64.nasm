;
; @file         MemOpsX64.nasm
; @brief        memcpy and memset implementation for 64-bit CPU mode.
;
; Copyright 2021-2024 Advanced Micro Devices, Inc. All rights reserved.
;

        global memset
        global memcpy

        section .text

memset:
        push    rdi
        push    rbx
        push    rcx ; save pBuffer
        mov     rdi, rcx  ; rdi = pBuffer
        mov     rax, rdx ; al = Value
        mov     rcx, r8 ; rcx = Count
        ; fill EAX with the Value so that we can perform DWORD operatins
        mov     ah, al
        mov     bx, ax
        shl     rax, 16
        mov     ax, bx
        ; if Counter is less then 4, jump to byte copy
        cmp     rcx, 4
        jb      .CopyByte
        ; check if the Buffer is 4-bytes aligned
        mov     rdx, rdi
        and     rdx, 3
        ; if the Buffer is 4-bytes aligned, jump to DWORD copy
        jz      .CopyDword
        ; Buffer is not 4-bytes aligned
        ; Calculate 4-(Buffer%4), which is a number of bytes we have to copy before
        ; Buffer will reach 4-bytes boundary, and perform byte copy
        neg     rdx
        add     rdx, 4
        xchg    rcx, rdx
        sub     rdx, rcx
        rep     stosb
        mov     rcx, rdx
.CopyDword:
        ; perform DWORD copy
        mov     rdx, rcx
        shr     rcx, 2
        rep     stosd
        ; copy the remainder
        and     rdx,3
        mov     rcx, rdx
.CopyByte:
        rep     stosb
        ;;;
        pop     rax ; return pBuffer
        pop     rbx
        pop     rdi
        ret


memcpy:
        push    rdi
        push    rsi
        push    rbx
        pushf
        push    rcx             ; save pDestination
        mov     rsi, rdx        ; pSource
        mov     rdi, rcx        ; pDestination
        mov     rcx, r8         ; Count
        mov     dl, 0
        ; if pSource > pDestination CopyForward
        mov     rax, rsi
        sub     rax, rdi        ; rax = pSource-pDestination
        jnb     .CopyForward    ; if pSource-pDestination > 0 CopyForward
        ; if pSource+Count < pDestination then CopyForward
        lea     rbx, [rsi+rcx]  ; rbx = pSource + Count
        neg     rax             ; rax = pDestination - pSource
        cmp     rbx, rdi
        jb      .CopyForward    ; if (pSource + Count < pDestination ) CopyForward
        ; Copy Backward
        mov     rsi, rbx        ; rsi = pSource + Count
        lea     rdi, [rdi+rcx]  ; rdi = pDestination + Count
        mov     dl, 1           ; Flag to indicate that we are copying backward
        std                     ; set direction flag to copy backward
.CopyForward:
        cmp     rcx, 8          ; if (Counter<8) copy byte by byte
        jb      .m8
        cmp     rax, 8          ; if (pDestination - pSource < 8) copy byte by byte
        jb      .m8
        ; if pSource and pDestination are not 8 byte aligned
        ; Calculate 8-(Buffer%8), which is a number of bytes we have to copy to align the buffer
        ; if this number if the same for source and destinations
        ; copy several bytes to align them
        ; otherwise proceed to QWORD copy
        mov     rax, rsi
        mov     rbx, rdi
        and     rax, 7
        and     rbx, 7
        test    dl, dl
        jz      .skip1
        dec     rsi
        dec     rdi
.skip1:
        cmp     rax, rbx
        jne     .m64
        test    rax, rax
        jz      .m64
        test    dl, dl
        jnz     .skip_nz1
        neg     rax
        add     rax, 8
.skip_nz1:
        xchg    rax, rcx
        sub     rax, rcx
        rep     movsb
        mov     rcx, rax
.m64:
        test    dl, dl
        jz      .skip2
        sub     rsi, 7
        sub     rdi, 7
.skip2:
        mov     rax, rcx
        shr     rcx, 3
        rep     movsq
        and     rax, 7
        jz      .MemCpuEnd
        test    dl, dl
        jz      .skip3
        add     rsi, 8
        add     rdi, 8
.skip3:
        mov     rcx, rax
.m8:
        test    dl, dl
        jz      .skip4
        dec     rsi
        dec     rdi
.skip4:
        rep     movsb
.MemCpuEnd:
        pop     rax         ; return pDestination
        popf
        pop     rbx
        pop     rsi
        pop     rdi
        ret
