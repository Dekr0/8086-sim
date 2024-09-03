bits 16
; address = ??? + 0x0, instruction width = 6 ;
mov word [1000],1
; address = ??? + 0x6, instruction width = 6 ;
mov word [1002],2
; address = ??? + 0xc, instruction width = 6 ;
mov word [1004],3
; address = ??? + 0x12, instruction width = 6 ;
mov word [1006],4
; address = ??? + 0x18, instruction width = 3 ;
mov BX,1000
; address = ??? + 0x1b, instruction width = 5 ;
mov word [BX+4],10
; address = ??? + 0x20, instruction width = 4 ;
mov BX,[1000]
; address = ??? + 0x24, instruction width = 4 ;
mov CX,[1002]
; address = ??? + 0x28, instruction width = 4 ;
mov DX,[1004]
; address = ??? + 0x2c, instruction width = 4 ;
mov BP,[1006]
; Execution context:
; mov word [1000],1 ; IP:0000->0x06,
; mov word [1002],2 ; IP:0x06->0x0c,
; mov word [1004],3 ; IP:0x0c->0x12,
; mov word [1006],4 ; IP:0x12->0x18,
; mov BX,1000 ; IP:0x18->0x1b,B:0000->B:0x3e8,
; mov word [BX+4],10 ; IP:0x1b->0x20,
; mov BX,[1000] ; IP:0x20->0x24,B:0x3e8->B:0x01,
; mov CX,[1002] ; IP:0x24->0x28,C:0000->C:0x02,
; mov DX,[1004] ; IP:0x28->0x2c,D:0000->D:0x0a,
; mov BP,[1006] ; IP:0x2c->0x30,BP:0000->BP:0x04,

; CPU state:
; A:0000
; C:0x02
; D:0x0a
; B:0x01
; SP:0000
; BP:0x04
; SI:0000
; DI:0000
; ES:0000
; CS:0000
; SS:0000
; DS:0000
; IP:0x30
; flags: 
; Estimated EU's clock cycles: 152