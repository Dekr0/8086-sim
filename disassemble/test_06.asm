bits 16
; address = ??? + 0x0, instruction width = 3 ;
mov AX,1
; address = ??? + 0x3, instruction width = 3 ;
mov BX,2
; address = ??? + 0x6, instruction width = 3 ;
mov CX,3
; address = ??? + 0x9, instruction width = 3 ;
mov DX,4
; address = ??? + 0xc, instruction width = 3 ;
mov SP,5
; address = ??? + 0xf, instruction width = 3 ;
mov BP,6
; address = ??? + 0x12, instruction width = 3 ;
mov SI,7
; address = ??? + 0x15, instruction width = 3 ;
mov DI,8

; Execution context:
; mov AX,1 ; IP:0000->0x03,A:0000->A:0x01,
; mov BX,2 ; IP:0x03->0x06,B:0000->B:0x02,
; mov CX,3 ; IP:0x06->0x09,C:0000->C:0x03,
; mov DX,4 ; IP:0x09->0x0c,D:0000->D:0x04,
; mov SP,5 ; IP:0x0c->0x0f,SP:0000->SP:0x05,
; mov BP,6 ; IP:0x0f->0x12,BP:0000->BP:0x06,
; mov SI,7 ; IP:0x12->0x15,SI:0000->SI:0x07,
; mov DI,8 ; IP:0x15->0x18,DI:0000->DI:0x08,

; CPU state:
; A:0x01
; C:0x03
; D:0x04
; B:0x02
; SP:0x05
; BP:0x06
; SI:0x07
; DI:0x08
; ES:0000
; CS:0000
; SS:0000
; DS:0000
; IP:0x18
; flags: 