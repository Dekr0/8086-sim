bits 16
; address = ??? + 0x0, instruction width = 3 ;
mov AX,8738
; address = ??? + 0x3, instruction width = 3 ;
mov BX,17476
; address = ??? + 0x6, instruction width = 3 ;
mov CX,26214
; address = ??? + 0x9, instruction width = 3 ;
mov DX,-30584
; address = ??? + 0xc, instruction width = 2 ;
mov SS,AX
; address = ??? + 0xe, instruction width = 2 ;
mov DS,BX
; address = ??? + 0x10, instruction width = 2 ;
mov ES,CX
; address = ??? + 0x12, instruction width = 2 ;
mov Al,17
; address = ??? + 0x14, instruction width = 2 ;
mov Bh,51
; address = ??? + 0x16, instruction width = 2 ;
mov Cl,85
; address = ??? + 0x18, instruction width = 2 ;
mov Dh,119
; address = ??? + 0x1a, instruction width = 2 ;
mov Ah,Bl
; address = ??? + 0x1c, instruction width = 2 ;
mov Cl,Dh
; address = ??? + 0x1e, instruction width = 2 ;
mov SS,AX
; address = ??? + 0x20, instruction width = 2 ;
mov DS,BX
; address = ??? + 0x22, instruction width = 2 ;
mov ES,CX
; address = ??? + 0x24, instruction width = 2 ;
mov SP,SS
; address = ??? + 0x26, instruction width = 2 ;
mov BP,DS
; address = ??? + 0x28, instruction width = 2 ;
mov SI,ES
; address = ??? + 0x2a, instruction width = 2 ;
mov DI,DX
; Execution context:
; mov AX,8738 ; IP:0000->0x03,A:0000->A:0x2222,
; mov BX,17476 ; IP:0x03->0x06,B:0000->B:0x4444,
; mov CX,26214 ; IP:0x06->0x09,C:0000->C:0x6666,
; mov DX,-30584 ; IP:0x09->0x0c,D:0000->D:0x8888,
; mov SS,AX ; IP:0x0c->0x0e,SS:0000->SS:0x2222,
; mov DS,BX ; IP:0x0e->0x10,DS:0000->DS:0x4444,
; mov ES,CX ; IP:0x10->0x12,ES:0000->ES:0x6666,
; mov Al,17 ; IP:0x12->0x14,A:0x2222->A:0x2211,
; mov Bh,51 ; IP:0x14->0x16,B:0x4444->B:0x3344,
; mov Cl,85 ; IP:0x16->0x18,C:0x6666->C:0x6655,
; mov Dh,119 ; IP:0x18->0x1a,D:0x8888->D:0x7788,
; mov Ah,Bl ; IP:0x1a->0x1c,A:0x2211->A:0x4411,
; mov Cl,Dh ; IP:0x1c->0x1e,C:0x6655->C:0x6677,
; mov SS,AX ; IP:0x1e->0x20,SS:0x2222->SS:0x4411,
; mov DS,BX ; IP:0x20->0x22,DS:0x4444->DS:0x3344,
; mov ES,CX ; IP:0x22->0x24,ES:0x6666->ES:0x6677,
; mov SP,SS ; IP:0x24->0x26,SP:0000->SP:0x4411,
; mov BP,DS ; IP:0x26->0x28,BP:0000->BP:0x3344,
; mov SI,ES ; IP:0x28->0x2a,SI:0000->SI:0x6677,
; mov DI,DX ; IP:0x2a->0x2c,DI:0000->DI:0x7788,

; CPU state:
; A:0x4411
; C:0x6677
; D:0x7788
; B:0x3344
; SP:0x4411
; BP:0x3344
; SI:0x6677
; DI:0x7788
; ES:0x6677
; CS:0000
; SS:0x4411
; DS:0x3344
; IP:0x2c
; flags: 
; Estimated EU's clock cycles: 56