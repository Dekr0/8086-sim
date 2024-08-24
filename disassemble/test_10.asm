bits 16
; address = ??? + 0x0, instruction width = 4 ;
add BX,30000
; address = ??? + 0x4, instruction width = 4 ;
add BX,10000
; address = ??? + 0x8, instruction width = 4 ;
sub BX,5000
; address = ??? + 0xc, instruction width = 4 ;
sub BX,5000
; address = ??? + 0x10, instruction width = 3 ;
mov BX,1
; address = ??? + 0x13, instruction width = 3 ;
mov CX,100
; address = ??? + 0x16, instruction width = 2 ;
add BX,CX
; address = ??? + 0x18, instruction width = 3 ;
mov DX,10
; address = ??? + 0x1b, instruction width = 2 ;
sub CX,DX
; address = ??? + 0x1d, instruction width = 4 ;
add BX,-25536
; address = ??? + 0x21, instruction width = 3 ;
add CX,-90
; address = ??? + 0x24, instruction width = 3 ;
mov SP,99
; address = ??? + 0x27, instruction width = 3 ;
mov BP,98
; address = ??? + 0x2a, instruction width = 2 ;
cmp BP,SP

; Execution context:
; add BX,30000 ; IP:0000->0x04,flags:->P,B:0000->B:0x7530,
; add BX,10000 ; IP:0x04->0x08,flags:P->SO,B:0x7530->B:0x9c40,
; sub BX,5000 ; IP:0x08->0x0c,flags:SO->PAS,B:0x9c40->B:0x88b8,
; sub BX,5000 ; IP:0x0c->0x10,flags:PAS->PO,B:0x88b8->B:0x7530,
; mov BX,1 ; IP:0x10->0x13,B:0x7530->B:0x01,
; mov CX,100 ; IP:0x13->0x16,C:0000->C:0x64,
; add BX,CX ; IP:0x16->0x18,flags:PO->P,B:0x01->B:0x65,
; mov DX,10 ; IP:0x18->0x1b,D:0000->D:0x0a,
; sub CX,DX ; IP:0x1b->0x1d,flags:P->PA,C:0x64->C:0x5a,
; add BX,-25536 ; IP:0x1d->0x21,flags:PA->PS,B:0x65->B:0x9ca5,
; add CX,-90 ; IP:0x21->0x24,flags:PS->CPAZ,C:0x5a->C:0000,
; mov SP,99 ; IP:0x24->0x27,SP:0000->SP:0x63,
; mov BP,98 ; IP:0x27->0x2a,BP:0000->BP:0x62,
; cmp BP,SP ; IP:0x2a->0x2c,flags:CPAZ->CPAS,

; CPU state:
; A:0000
; C:0000
; D:0x0a
; B:0x9ca5
; SP:0x63
; BP:0x62
; SI:0000
; DI:0000
; ES:0000
; CS:0000
; SS:0000
; DS:0000
; IP:0x2c
; flags: CPAS