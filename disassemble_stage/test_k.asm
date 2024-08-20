bits 16
; address = ??? + 0x0, instruction width = 4
add bx,30000
; address = ??? + 0x4, instruction width = 4
add bx,10000
; address = ??? + 0x8, instruction width = 4
sub bx,5000
; address = ??? + 0xc, instruction width = 4
sub bx,5000
; address = ??? + 0x10, instruction width = 3
mov bx,1
; address = ??? + 0x13, instruction width = 3
mov cx,100
; address = ??? + 0x16, instruction width = 2
add bx,cx
; address = ??? + 0x18, instruction width = 3
mov dx,10
; address = ??? + 0x1b, instruction width = 2
sub cx,dx
; address = ??? + 0x1d, instruction width = 4
add bx,-25536
; address = ??? + 0x21, instruction width = 3
add cx,-90
; address = ??? + 0x24, instruction width = 3
mov sp,99
; address = ??? + 0x27, instruction width = 3
mov bp,98
; address = ??? + 0x2a, instruction width = 2
cmp bp,sp

; Execution context:
; add bx,30000
; flags register: ->P
; b:0000->b:0x7530

; add bx,10000
; flags register: P->SO
; b:0x7530->b:0x9c40

; sub bx,5000
; flags: SO -> PAS
; b:0x9c40->b:0x88b8

; sub bx,5000
; flags: PAS -> PO
; b:0x88b8->b:0x7530

; mov bx,1
; b:0x7530->b:0x01

; mov cx,100
; c:0000->c:0x64

; add bx,cx
; flags register: PO->P
; b:0x01->b:0x65

; mov dx,10
; d:0000->d:0x0a

; sub cx,dx
; flags: P -> PA
; c:0x64->c:0x5a

; add bx,-25536
; flags register: PA->PS
; b:0x65->b:0x9ca5

; add cx,-90
; flags register: PS->CPAZ
; c:0x5a->c:0000

; mov sp,99
; sp:0000->sp:0x63

; mov bp,98
; bp:0000->bp:0x62

; cmp bp,sp
; flags: CPAZ -> CPAS


; Final register state:
; reg a state: 0000
; reg c state: 0000
; reg d state: 0x0a
; reg b state: 0x9ca5
; reg sp state: 0x63
; reg bp state: 0x62
; reg si state: 0000
; reg di state: 0000
; reg es state: 0000
; reg cs state: 0000
; reg ss state: 0000
; reg ds state: 0000
; flags: CPAS