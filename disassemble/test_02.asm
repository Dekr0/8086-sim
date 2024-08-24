bits 16
; address = ??? + 0x0, instruction width = 2 ;
mov SI,BX
; address = ??? + 0x2, instruction width = 2 ;
mov Dh,Al
; address = ??? + 0x4, instruction width = 2 ;
mov Cl,12
; address = ??? + 0x6, instruction width = 2 ;
mov Ch,-12
; address = ??? + 0x8, instruction width = 3 ;
mov CX,12
; address = ??? + 0xb, instruction width = 3 ;
mov CX,-12
; address = ??? + 0xe, instruction width = 3 ;
mov DX,3948
; address = ??? + 0x11, instruction width = 3 ;
mov DX,-3948
; address = ??? + 0x14, instruction width = 2 ;
mov Al,[BX+SI]
; address = ??? + 0x16, instruction width = 2 ;
mov BX,[BP+DI]
; address = ??? + 0x18, instruction width = 3 ;
mov DX,[BP+0]
; address = ??? + 0x1b, instruction width = 3 ;
mov Ah,[BX+SI+4]
; address = ??? + 0x1e, instruction width = 4 ;
mov Al,[BX+SI+4999]
; address = ??? + 0x22, instruction width = 2 ;
mov [BX+DI],CX
; address = ??? + 0x24, instruction width = 2 ;
mov [BP+SI],Cl
; address = ??? + 0x26, instruction width = 3 ;
mov [BP+0],Ch

