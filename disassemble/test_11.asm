bits 16
; address = ??? + 0x0, instruction width = 3 ;
mov CX,200
; address = ??? + 0x3, instruction width = 2 ;
mov BX,CX
; address = ??? + 0x5, instruction width = 4 ;
add CX,1000
; address = ??? + 0x9, instruction width = 3 ;
mov BX,2000
; address = ??? + 0xc, instruction width = 2 ;
sub CX,BX

; Execution context:
; mov CX,200 ; IP:0000->0x03,C:0000->C:0xc8,
; mov BX,CX ; IP:0x03->0x05,B:0000->B:0xc8,
; add CX,1000 ; IP:0x05->0x09,flags:->A,C:0xc8->C:0x4b0,
; mov BX,2000 ; IP:0x09->0x0c,B:0xc8->B:0x7d0,
; sub CX,BX ; IP:0x0c->0x0e,flags:A->CS,C:0x4b0->C:0xfce0,

; CPU state:
; A:0000
; C:0xfce0
; D:0000
; B:0x7d0
; SP:0000
; BP:0000
; SI:0000
; DI:0000
; ES:0000
; CS:0000
; SS:0000
; DS:0000
; IP:0x0e
; flags: CS