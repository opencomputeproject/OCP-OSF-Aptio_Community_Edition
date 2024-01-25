;
; @file         MemOpsX32.nasm
; @brief        memcpy and memset implementation for 32-bit CPU mode.
;
; Copyright 2021 Advanced Micro Devices, Inc. All rights reserved.
;
%include "Porting.h"

        global ASM_TAG(memset)
        global ASM_TAG(memcpy)

        section .text

ASM_TAG(memset):
        mov     ecx, [esp+12]   ; ecx = Count
        mov     al, [esp+8]     ; al = Value
        push    edi
        mov     edi, [esp+4+4]  ; edi = pBuffer
        push    dword [esp+4+4] ; save pBuffer
        push    ebx
        ; fill EAX with the Value so that we can perform DWORD operatins
        mov     ah, al
        mov     bx, ax
        shl     eax,16
        mov     ax, bx
        ; if Counter is less then 4, jump to byte copy
        cmp     ecx, 4
        jb      .CopyByte
        ; check if the Buffer is 4-bytes aligned
        mov     edx, edi
        and     edx, 3
        ; if the Buffer is 4-bytes aligned, jump to DWORD copy
        jz      .CopyDword
        ; Buffer is not 4-bytes aligned
        ; Calculate 4-(Buffer%4), which is a number of bytes we have to copy before
        ; Buffer will reach 4-bytes boundary, and perform byte copy
        neg     edx
        add     edx, 4
        xchg    ecx, edx
        sub     edx, ecx
        rep     stosb
        mov     ecx, edx
.CopyDword:
        ; perform DWORD copy
        mov     edx, ecx
        shr     ecx, 2
        rep     stosd
        ; copy the remainder
        and     edx,3
        mov     ecx, edx
.CopyByte:
        rep     stosb

        pop     ebx
        pop     eax
        pop     edi
        ret

ASM_TAG(memcpy):
; dst [esp + 4]
; src [esp + 8]
; cnt [esp + 12]
        push    ebp
        mov     ebp, esp
        push    edi
        push    esi
        push    ebx
        pushf
        push    dword [ebp + 4 + 4]    ; dst
        mov     edi, [ebp + 4 + 4]     ; dst
        mov     esi, [ebp + 8 + 4]     ; src
        mov     ecx, [ebp + 12 + 4]    ; cnt
        mov     dl, 0
        mov     eax, esi
        sub     eax, edi
        jnb     .CopyForward
        lea     ebx, [esi + ecx]
        neg     eax
        cmp     ebx, edi
        jb      .CopyForward
        mov     esi, ebx
        lea     edi, [edi+ecx]
        mov     dl, 1
        std
.CopyForward:
        cmp     ecx, 4
        jb      .m8
        cmp     eax, 4
        jb      .m8
        mov     eax, esi
        mov     ebx, edi
        and     eax, 3
        and     ebx, 3
        test    dl, dl
        jz      .skip1
        dec     esi
        dec     edi
.skip1:
        cmp     eax, ebx
        jne     .m32
        test    eax, eax
        jz      .m32
        test    dl, dl
        jnz     .skip_nz1
        neg     eax
        add     eax, 4
.skip_nz1:
        xchg    eax, ecx
        sub     eax, ecx
        rep     movsb
        mov     ecx, eax
.m32:
        test    dl, dl
        jz      .skip2
        sub     esi, 3
        sub     edi, 3
.skip2:
        mov     eax, ecx
        shr     ecx, 2
        rep     movsd
        and     eax, 3
        jz      .memcpyend
        test    dl, dl
        jz      .skip3
        add     esi, 4
        add     edi, 4
.skip3:
        mov     ecx, eax
.m8:
        test    dl, dl
        jz      .skip4
        dec     esi
        dec     edi
.skip4:
        rep     movsb
.memcpyend:
        pop     eax
        popf
        pop     ebx
        pop     esi
        pop     edi
        pop     ebp
        ret
