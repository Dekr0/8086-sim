bits 16

mov dx,6
mov bp,1000

mov si,0
jmp_addr9:
    mov [bp+si],si
    add si,2
    cmp si,dx
    jne jmp_addr9

mov bx,0
mov si,dx
sub bp,2
jmp_addr26:
    add bx,[bp+si]
    sub si,2
    jne jmp_addr26

