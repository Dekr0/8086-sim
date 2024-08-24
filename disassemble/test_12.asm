bits 16
; address = ??? + 0x0, instruction width = 3 ;
mov CX,3
; address = ??? + 0x3, instruction width = 3 ;
mov BX,1000
; address = ??? + 0x6, instruction width = 3 ;
jmp_addr6: add BX,10
; address = ??? + 0x9, instruction width = 3 ;
sub CX,1
; address = ??? + 0xc, instruction width = 2 ;
jne jmp_addr6

