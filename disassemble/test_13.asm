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

