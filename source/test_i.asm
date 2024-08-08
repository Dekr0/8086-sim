; ========================================================================
;
; (C) Copyright 2023 by Molly Rocket, Inc., All Rights Reserved.
;
; This software is provided 'as-is', without any express or implied
; warranty. In no event will the authors be held liable for any damages
; arising from the use of this software.
;
; Please see https://computerenhance.com for further information
;
; ========================================================================

; ========================================================================
; LISTING 45
; ========================================================================

bits 16

mov ax, 0x2222 ; a => 0x2222
mov bx, 0x4444 ; b => 0x4444
mov cx, 0x6666 ; c => 0x6666
mov dx, 0x8888 ; d => 0x8888

mov ss, ax ; ss => 0x2222
mov ds, bx ; ds => 0x4444
mov es, cx ; es => 0x6666

mov al, 0x11 ; a => 0x2211
mov bh, 0x33 ; b => 0x3344
mov cl, 0x55 ; c => 0x6655
mov dh, 0x77 ; d => 0x7788

mov ah, bl ; a => 0x4411
mov cl, dh ; c => 0x6677

mov ss, ax
mov ds, bx
mov es, cx

mov sp, ss
mov bp, ds
mov si, es
mov di, dx
