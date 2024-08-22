bits 16
; address = ??? + 0x0, instruction width = 3 ;
mov cx,3
; address = ??? + 0x3, instruction width = 3 ;
mov bx,1000
; address = ??? + 0x6, instruction width = 3 ;
jmp_addr6:
add bx,10
; address = ??? + 0x9, instruction width = 3 ;
sub cx,1
; address = ??? + 0xc, instruction width = 2 ;
jne jmp_addr6

