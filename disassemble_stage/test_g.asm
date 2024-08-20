bits 16
; address = ??? + 0x0, instruction width = 3
mov ax,1
; address = ??? + 0x3, instruction width = 3
mov bx,2
; address = ??? + 0x6, instruction width = 3
mov cx,3
; address = ??? + 0x9, instruction width = 3
mov dx,4
; address = ??? + 0xc, instruction width = 3
mov sp,5
; address = ??? + 0xf, instruction width = 3
mov bp,6
; address = ??? + 0x12, instruction width = 3
mov si,7
; address = ??? + 0x15, instruction width = 3
mov di,8

; Execution context:
; mov ax,1
; a:0000->a:0x01

; mov bx,2
; b:0000->b:0x02

; mov cx,3
; c:0000->c:0x03

; mov dx,4
; d:0000->d:0x04

; mov sp,5
; sp:0000->sp:0x05

; mov bp,6
; bp:0000->bp:0x06

; mov si,7
; si:0000->si:0x07

; mov di,8
; di:0000->di:0x08

; Final register state:
; reg a state: 0x01
; reg c state: 0x03
; reg d state: 0x04
; reg b state: 0x02
; reg sp state: 0x05
; reg bp state: 0x06
; reg si state: 0x07
; reg di state: 0x08
; reg es state: 0000
; reg cs state: 0000
; reg ss state: 0000
; reg ds state: 0000
; flags: 