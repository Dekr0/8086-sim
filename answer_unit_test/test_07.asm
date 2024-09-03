bits 16
; address = ??? + 0x0, instruction width = 3 ;
mov AX,1
; address = ??? + 0x3, instruction width = 3 ;
mov BX,2
; address = ??? + 0x6, instruction width = 3 ;
mov CX,3
; address = ??? + 0x9, instruction width = 3 ;
mov DX,4
; address = ??? + 0xc, instruction width = 2 ;
mov SP,AX
; address = ??? + 0xe, instruction width = 2 ;
mov BP,BX
; address = ??? + 0x10, instruction width = 2 ;
mov SI,CX
; address = ??? + 0x12, instruction width = 2 ;
mov DI,DX
; address = ??? + 0x14, instruction width = 2 ;
mov DX,SP
; address = ??? + 0x16, instruction width = 2 ;
mov CX,BP
; address = ??? + 0x18, instruction width = 2 ;
mov BX,SI
; address = ??? + 0x1a, instruction width = 2 ;
mov AX,DI
; Execution context:
; mov AX,1 ; IP:0000->0x03,A:0000->A:0x01,
; mov BX,2 ; IP:0x03->0x06,B:0000->B:0x02,
; mov CX,3 ; IP:0x06->0x09,C:0000->C:0x03,
; mov DX,4 ; IP:0x09->0x0c,D:0000->D:0x04,
; mov SP,AX ; IP:0x0c->0x0e,SP:0000->SP:0x01,
; mov BP,BX ; IP:0x0e->0x10,BP:0000->BP:0x02,
; mov SI,CX ; IP:0x10->0x12,SI:0000->SI:0x03,
; mov DI,DX ; IP:0x12->0x14,DI:0000->DI:0x04,
; mov DX,SP ; IP:0x14->0x16,D:0x04->D:0x01,
; mov CX,BP ; IP:0x16->0x18,C:0x03->C:0x02,
; mov BX,SI ; IP:0x18->0x1a,B:0x02->B:0x03,
; mov AX,DI ; IP:0x1a->0x1c,A:0x01->A:0x04,

; CPU state:
; A:0x04
; C:0x02
; D:0x01
; B:0x03
; SP:0x01
; BP:0x02
; SI:0x03
; DI:0x04
; ES:0000
; CS:0000
; SS:0000
; DS:0000
; IP:0x1c
; flags: 
; Estimated EU's clock cycles: 32