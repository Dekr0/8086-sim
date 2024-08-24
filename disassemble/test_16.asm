bits 16
; address = ??? + 0x0, instruction width = 3 ;
mov BP,256
; address = ??? + 0x3, instruction width = 3 ;
mov DX,0
; address = ??? + 0x6, instruction width = 3 ;
jmp_addr6: mov CX,0
; address = ??? + 0x9, instruction width = 3 ;
jmp_addr9: mov [BP+0],CX
; address = ??? + 0xc, instruction width = 3 ;
mov [BP+2],DX
; address = ??? + 0xf, instruction width = 4 ;
mov byte [BP+3],-1
; address = ??? + 0x13, instruction width = 3 ;
add BP,4
; address = ??? + 0x16, instruction width = 3 ;
add CX,1
; address = ??? + 0x19, instruction width = 3 ;
cmp CX,64
; address = ??? + 0x1c, instruction width = 2 ;
jne jmp_addr9
; address = ??? + 0x1e, instruction width = 3 ;
add DX,1
; address = ??? + 0x21, instruction width = 3 ;
cmp DX,64
; address = ??? + 0x24, instruction width = 2 ;
jne jmp_addr6

