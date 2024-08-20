bits 16
; address = ??? + 0x0, instruction width = 3
mov ax,1
; address = ??? + 0x3, instruction width = 3
mov bx,2
; address = ??? + 0x6, instruction width = 3
mov cx,3
; address = ??? + 0x9, instruction width = 3
mov dx,4
; address = ??? + 0xc, instruction width = 2
mov sp,ax
; address = ??? + 0xe, instruction width = 2
mov bp,bx
; address = ??? + 0x10, instruction width = 2
mov si,cx
; address = ??? + 0x12, instruction width = 2
mov di,dx
; address = ??? + 0x14, instruction width = 2
mov dx,sp
; address = ??? + 0x16, instruction width = 2
mov cx,bp
; address = ??? + 0x18, instruction width = 2
mov bx,si
; address = ??? + 0x1a, instruction width = 2
mov ax,di

; Execution context:
; mov ax,1
; a:0000->a:0x01

; mov bx,2
; b:0000->b:0x02

; mov cx,3
; c:0000->c:0x03

; mov dx,4
; d:0000->d:0x04

; mov sp,ax
; sp:0000->sp:0x01

; mov bp,bx
; bp:0000->bp:0x02

; mov si,cx
; si:0000->si:0x03

; mov di,dx
; di:0000->di:0x04

; mov dx,sp
; d:0x04->d:0x01

; mov cx,bp
; c:0x03->c:0x02

; mov bx,si
; b:0x02->b:0x03

; mov ax,di
; a:0x01->a:0x04

; Final register state:
; reg a state: 0x04
; reg c state: 0x02
; reg d state: 0x01
; reg b state: 0x03
; reg sp state: 0x01
; reg bp state: 0x02
; reg si state: 0x03
; reg di state: 0x04
; reg es state: 0000
; reg cs state: 0000
; reg ss state: 0000
; reg ds state: 0000
; flags: 