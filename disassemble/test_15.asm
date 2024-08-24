bits 16
; address = ??? + 0x0, instruction width = 3 ;
mov DX,6
; address = ??? + 0x3, instruction width = 3 ;
mov BP,1000
; address = ??? + 0x6, instruction width = 3 ;
mov SI,0
; address = ??? + 0x9, instruction width = 2 ;
jmp_addr9: mov [BP+SI],SI
; address = ??? + 0xb, instruction width = 3 ;
add SI,2
; address = ??? + 0xe, instruction width = 2 ;
cmp SI,DX
; address = ??? + 0x10, instruction width = 2 ;
jne jmp_addr9
; address = ??? + 0x12, instruction width = 3 ;
mov BX,0
; address = ??? + 0x15, instruction width = 2 ;
mov SI,DX
; address = ??? + 0x17, instruction width = 3 ;
sub BP,2
; address = ??? + 0x1a, instruction width = 2 ;
jmp_addr26: add BX,[BP+SI]
; address = ??? + 0x1c, instruction width = 3 ;
sub SI,2
; address = ??? + 0x1f, instruction width = 2 ;
jne jmp_addr26

