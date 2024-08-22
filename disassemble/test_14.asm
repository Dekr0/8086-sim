bits 16
; address = ??? + 0x0, instruction width = 3 ;
mov dx,6
; address = ??? + 0x3, instruction width = 3 ;
mov bp,1000
; address = ??? + 0x6, instruction width = 3 ;
mov si,0
; address = ??? + 0x9, instruction width = 2 ;
jmp_addr9:
mov [bp+si],si
; address = ??? + 0xb, instruction width = 3 ;
add si,2
; address = ??? + 0xe, instruction width = 2 ;
cmp si,dx
; address = ??? + 0x10, instruction width = 2 ;
jne jmp_addr9
; address = ??? + 0x12, instruction width = 3 ;
mov bx,0
; address = ??? + 0x15, instruction width = 3 ;
mov si,0
; address = ??? + 0x18, instruction width = 2 ;
jmp_addr24:
mov cx,[bp+si]
; address = ??? + 0x1a, instruction width = 2 ;
add bx,cx
; address = ??? + 0x1c, instruction width = 3 ;
add si,2
; address = ??? + 0x1f, instruction width = 2 ;
cmp si,dx
; address = ??? + 0x21, instruction width = 2 ;
jne jmp_addr24

